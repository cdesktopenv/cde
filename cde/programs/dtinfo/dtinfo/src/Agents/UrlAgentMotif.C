/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *  $XConsortium: UrlAgentMotif.C /main/15 1996/12/10 16:28:28 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#include <WWL/WXmForm.h>
#include <WWL/WXmSeparator.h>

# include "UAS.hh"

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_MessageMgr
#define C_NodeMgr
#define L_Managers

#define C_PrintPanelAgent
#define C_HelpAgent
#define C_UrlAgent
#define L_Agents

#include <Prelude.h>

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

#define CLASS UrlAgent
#include "create_macros.hh"

// the following defines are for message index per catalog
// Set_UrlAgent:
#define ERR_LOCATOR_FORMAT      2
#define ERR_LOCATOR_RESOLUTION  3
#define ERR_NO_LOCATOR          6

extern bool g_scroll_to_locator;
extern char g_top_locator[];


UrlAgent::UrlAgent () {
    fShell = NULL;
    fPrefWindow = 0;
}

UrlAgent::~UrlAgent () {
    f_form.Unmanage ();
    XtDestroyWidget (XtParent(f_form));
}

void
UrlAgent::display (NodeWindowAgent *prefWindow) {
    if (fShell == 0)
	create_ui ();
    //f_form.Manage();
    XmProcessTraversal( (Widget)fTextField, XmTRAVERSE_CURRENT );

    //  Set the minimum size of this window...
    fShell.MinWidth(fShell.Width());
    fShell.MinHeight(fShell.Height());
    fShell.MaxHeight(fShell.Height());

    fShell.Popup ();
    fPoppedUp = TRUE;
    fPrefWindow = prefWindow;
}


// status flags for use with status return from document() method
// (these should never be changed in value)
#define ID_SUCCESS      1
#define BAD_ARG_FORMAT  2
#define LOAD_ERROR      3
#define NOT_FOUND       5

int
UrlAgent::document( char   *locator,
                    Boolean new_window )
{
  int   sts = ID_SUCCESS ;

  if(  (locator == NULL) ||
      ((locator != NULL) && ( *locator == '\0' )) )
  {
      //  checked here to avoid less specific error return, later
      message_mgr().error_dialog( (char*)UAS_String(CATGETS(
                    Set_UrlAgent, ERR_NO_LOCATOR,
                    "No locator value provided." )) ) ;
      return (sts = BAD_ARG_FORMAT);
  }

  UAS_Pointer<UAS_Common> d = NULL ;
  mtry
    {
      // perform basic syntax tests, but rely on the
      // precise format parsing to be done in the UAS layer

      if( !strchr( locator, ':' ) && !strchr( locator, '/' ) )
        {
          // assume given a unique locator ID for the target.
          // construct a fully-qualified form and pass it on.

          char *buffer =
                  new char[strlen("mmdb:LOCATOR=") + strlen(locator) + 1];
          sprintf (buffer, "mmdb:LOCATOR=%s", locator);
          d = UAS_Common::create( buffer ) ;
          delete [] buffer;

          // if no result, the unique ID was not found in a loaded infolib
          if( d == (const int)NULL ) sts = NOT_FOUND ;
        }
      else if( strchr( locator, ':' ) && strchr( locator, '=' ) )
        {
          // assume to have a fully-qualified locator

          d = UAS_Common::create( locator ) ;
        }
      else
        {
          message_mgr().error_dialog( (char*)UAS_String(CATGETS(
                        Set_UrlAgent, ERR_LOCATOR_FORMAT,
                       "Locator specification format error." )) ) ;
          sts = BAD_ARG_FORMAT ;
        }

      if( d != (const int)NULL )
        {
          // always use new window to display external requests
          if( new_window ) node_mgr().force_new_window() ;

          // (evil hack alert)
          g_scroll_to_locator = TRUE;
          strcpy( g_top_locator, locator ) ;

          d->retrieve();
        }
    }
  mcatch_any()
    {
      message_mgr().error_dialog( (char*)UAS_String(CATGETS(
                       Set_UrlAgent, ERR_LOCATOR_RESOLUTION,
                       "Locator resolution failed." )) ) ;

      // with create method used, we can't know whether it may be that: the
      // locator was not found in a loaded library (or a requested library)
      // using fully-qualified-locator syntax; an embedded infolib was not
      // found; or there was simply an error on the infolib, having found it.
      // -jcb
      if( sts != NOT_FOUND ) sts = LOAD_ERROR ;
    }
  end_try;

  return sts ;
}


/*
 * ------------------------------------------------------------------------
 * Name: CloseProgramCB
 *
 * Description:
 *
 *     Exit the program.
 *
 * Return value:
 *
 *     None.
 */
void
CloseProgramCB(
		Widget,
		XtPointer client_data,
		XtPointer /*call_data*/)
{
    AppPrintData * p = (AppPrintData *) client_data ;
    /* we want to wait for the current job to complete before exiting */
    
    //  If a job is running, just unmap the windows and install itself
    //  as endjob callback, which will be called when printed_lines is
    //  back to zero

    // ?????? what is the criteria for a job stil running ?????
#if 0
    if (p->printed_lines) {
	//  Put up a dialog saying it's waiting for the job
	//   to complete 
	XtAddCallback(p->print_shell, XmNendJobCallback, CloseProgramCB, p);
    } else {
	exit(0);
    }
#endif
}


//  need to implement silent printing...
//  when non-silent printing, dialogs come up, otherwise they don't
//  but we need extra code for this
int
UrlAgent::print_document(char *locator)
{
    int   sts = ID_SUCCESS ;

    // set up for printing
    AppPrintData* p = window_system().GetAppPrintData();
    
    if(  (locator == NULL) ||
	 ((locator != NULL) && ( *locator == '\0' )) )
    {
	//  checked here to avoid less specific error return, later

	if (!(window_system().videoShell()->silent)) {
	    message_mgr().error_dialog( (char*)UAS_String(CATGETS(
		Set_UrlAgent, ERR_NO_LOCATOR,
		"No locator value provided." )) ) ;
	}
	return (sts = BAD_ARG_FORMAT);
    }
    
    UAS_Pointer<UAS_Common> d = NULL ;
    mtry
    {
	// perform basic syntax tests, but rely on the
	// precise format parsing to be done in the UAS layer
	
	if( !strchr( locator, ':' ) && !strchr( locator, '/' ) )
        {
	    // assume given a unique locator ID for the target.
	    // construct a fully-qualified form and pass it on.
	    
	    char *buffer =
		new char[strlen("mmdb:LOCATOR=") + strlen(locator) + 1];
	    sprintf (buffer, "mmdb:LOCATOR=%s", locator);
	    d = UAS_Common::create( buffer ) ;
	    delete [] buffer;
	    
	    // if no result, the unique ID was not found in a loaded infolib
	    if( d == (const int)NULL ) sts = NOT_FOUND ;
        }
	else if( strchr( locator, ':' ) && strchr( locator, '=' ) )
        {
	    // assume to have a fully-qualified locator
	    
	    d = UAS_Common::create( locator ) ;
        }
	else
        {
	    if ((!window_system().videoShell()->silent)) {
		message_mgr().error_dialog( (char*)UAS_String(CATGETS(
		    Set_UrlAgent, ERR_LOCATOR_FORMAT,
		    "Locator specification format error." )) ) ;
	    }
	    sts = BAD_ARG_FORMAT ;
        }

	// if we have a setion add it to the print list
	if( d != (const int)NULL ) {	    
 
	    //  Need to put this single node on the print list so that
	    //  reset_ui() can determine the number of nodes being printed 
	    
	    xList<UAS_Pointer<UAS_Common> > * print_list = new xList<UAS_Pointer<UAS_Common> >;
	    print_list->append(d);
	    
	    p->f_outline_element = NULL;
	    p->f_print_list = print_list;
	    
	    //  Create the print setup box as the child of the top level shell
	    CreatePrintSetup(window_system().toplevel(), p);
	    
	    // set the cancel button to exit the program
	    XtAddCallback(p->f_print_dialog, DtNcancelCallback, CloseProgramCB, p);

	    // if silent printing, rely on default print data
	    
	    if (window_system().videoShell()->silent) {

		if(p->f_print_data->print_display == (Display*)NULL)
		{
		    
		    // first time thru print setup, so get default data 
		    
		    if (DtPrintFillSetupData(p->f_print_dialog, p->f_print_data)
			!= DtPRINT_SUCCESS) {
			return LOAD_ERROR;
		    }
		}
		
		DoPrint(window_system().toplevel(), p) ;
	    }
	    
	    // otherwise silent printing is not requested so put up print setup box

	    else {
		//  Manage the print setup box		
		XtManageChild(p->f_print_dialog); 
	    }
        }
    }
    mcatch_any()
	{
	    
	    if ((!window_system().videoShell()->silent)) {
		message_mgr().error_dialog( (char*)UAS_String(CATGETS(
		    Set_UrlAgent, ERR_LOCATOR_RESOLUTION,
		    "Locator resolution failed." )) ) ;
	    }
		
	    // with create method used, we can't know whether it may be that: the
	    // locator was not found in a loaded library (or a requested library)
	    // using fully-qualified-locator syntax; an embedded infolib was not
	    // found; or there was simply an error on the infolib, having found it.
	    // -jcb
	    if( sts != NOT_FOUND ) sts = LOAD_ERROR ;
	}
    end_try;
    
    return sts ;
}

void
UrlAgent::create_ui () {
    //
    //  Build the widgets...
    //
    int               decorations=MWM_DECOR_BORDER  |
                                  MWM_DECOR_RESIZEH |
                                  MWM_DECOR_TITLE   |
                                  MWM_DECOR_MENU;
    fShell = WTopLevelShell (window_system().toplevel(), WPopup, "open_url");
    window_system().register_shell (&fShell);

    XmStringLocalized mtfstring;
    String	      string;

    string = CATGETS(Set_UrlAgent, 1, "Dtinfo: Open Locator");
    XtVaSetValues((Widget)fShell,
                  XmNtitle, string,
                  XmNmwmDecorations, decorations,
                  NULL);

    ASSN	(WXmForm,	f_form,		fShell,		"form");
    ASSNM	(WXmPushButton,	fOk,		f_form,		"ok");
    ASSNM	(WXmPushButton,	fApply,		f_form,		"apply");
    ASSNM	(WXmPushButton, fClear,		f_form,		"clear");
    DECLM	(WXmPushButton, close,		f_form,		"close");
    DECLM	(WXmPushButton, help,		f_form,		"help");
    DECLM	(WXmSeparator,  sep,		f_form,		"separator");
    ASSNM	(WXmTextField,	fTextField,	f_form,		"text_field");
    DECLM	(WXmLabel,	label,		f_form,		"the_label");

    mtfstring = CATGETS(Set_AgentLabel, 222, "Enter a Unique Locator:");
    XtVaSetValues(label, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 161, "OK");
    XtVaSetValues(fOk, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 201, "Apply");
    XtVaSetValues(fApply, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 223, "Clear");
    XtVaSetValues(fClear, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 12, "Close");
    XtVaSetValues(close, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
    XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);

    //
    //  Set up callbacks...
    //
    SET_CALLBACK (fShell,Popdown,popdown);
    SET_CALLBACK (fTextField,ValueChanged,valueChanged);
    SET_CALLBACK (fTextField,ModifyVerify,modifyVerify);
    ON_ACTIVATE (fOk,displayDocumentAndClose);
    ON_ACTIVATE (fApply,displayDocument);
    ON_ACTIVATE (close,close);
    ON_ACTIVATE (fClear,clear);
    help_agent().add_activate_help (help, (char*)"open_url_help");

    //
    //  A few clean up things...
    //
    f_form.DefaultButton (fApply);
    f_form.ShadowThickness (0);
    f_form.Manage();

    fShell.Realize();

    //
    //  Desensitize the ok and apply buttons.
    //
    XtSetSensitive (fOk, False);
    XtSetSensitive (fApply, False);

    fPoppedUp = TRUE;


    //
    //  Get the default home page for the user and stuff
    //  it in the text field...
    //
}

void
UrlAgent::close () {
    fShell.Popdown ();
}

void
UrlAgent::popdown () {
    fPoppedUp = FALSE;
}

void
UrlAgent::displayDocumentAndClose () {
    close ();
    displayDocument ();
}

void
UrlAgent::displayDocument () {

    if (fPrefWindow)
	node_mgr().set_preferred_window(fPrefWindow);

    char *url = XmTextFieldGetString (fTextField);
//    if (!(url && *url)) {
//	XBell (XtDisplay(fTextField), 100);
//	XtFree (url);
//	return;
//    }
    document(url, 0);
    XtFree(url);

//    try {
//	UAS_Pointer<UAS_Common> d = UAS_Common::create (url);
//	XtFree (url);
//	if (d != (const int)NULL) {
//	    d->retrieve ();
//	} else {
//	    message_mgr().error_dialog ((char*)
//		UAS_String(CATGETS(Set_Messages, 70,
//		"Invalid access method in url.")));
//	}
//    }
//    mcatch_any() {
//	message_mgr().error_dialog ((char*)
//		UAS_String(CATGETS(Set_Messages, 71,
//		"Document creation failed.")));
//    }
//    end_try;
    return;
}

void
UrlAgent::valueChanged () {
    char *url = XmTextFieldGetString (fTextField);
    int sensitive = url && *url;
    XtFree (url);
    XtSetSensitive (fOk, sensitive);
    XtSetSensitive (fApply, sensitive);
    XtSetSensitive (fClear, sensitive);
}

void
UrlAgent::modifyVerify (WCallback *wcb) {
    XmTextVerifyPtr cbs = (XmTextVerifyPtr) wcb->CallData();
    cbs->doit = True;
    for (int i = 0; i < cbs->text->length; i ++) {
	if (cbs->text->ptr[i] == ' ' || cbs->text->ptr[i] == '\t') {
	    cbs->doit = False;
	    return;
	}
    }
}

void
UrlAgent::clear () {
    XmTextFieldSetString (fTextField, (char*)"");
    valueChanged ();
}
