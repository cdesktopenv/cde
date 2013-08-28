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
 *  $TOG: AddLibraryAgentMotif.C /main/10 1998/08/05 11:46:23 mgreess $
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

#define C_List
#define C_TOC_Element
#define L_Basic

#define C_EnvMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#define C_AddLibraryAgent
#define C_HelpAgent
#define L_Agents
#include <sstream>
using namespace std;

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include <Prelude.h>

#include "Registration.hh"

#define CLASS AddLibraryAgent
#include "create_macros.hh"

#include <WWL/WComposite.h>
#include <WWL/WXmSeparator.h>

AddLibraryAgent::AddLibraryAgent (Widget parent):
	f_shell (parent, True, "add_library_agent"),
	f_done (False), f_cancelled (False), f_file_sel (NULL) {

    create_ui (parent);
}

AddLibraryAgent::~AddLibraryAgent() {
    f_form.Unmanage ();
    XtDestroyWidget (XtParent(f_form));
}

void
AddLibraryAgent::create_ui (Widget parent) {

    XmStringLocalized mtfstring;
    String	      string;

    window_system().register_full_modal_shell (&f_shell);

    string = msg_catalog_mgr().catgets(Set_AddLibraryAgent, 1);
    XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

    // order of these macros must match the .res file occurences for same
    ASSN (WXmForm,		f_form,		f_shell,	"form");
    DECLM (WXmForm,		sub_form,	f_form,		"sub_form");
    DECLM (WXmLabel,		label,		sub_form,	"theLabel");
    DECLM (WXmPushButton,	pick,		sub_form,	"pick");
    ASSNM (WXmTextField,	f_text,		sub_form,	"text");
    DECLM (WXmSeparator,	sep,		f_form,		"separator");
    ASSNM (WXmPushButton,	f_ok,		f_form,		"ok");
    ASSNM (WXmPushButton,	f_clr,		f_form,		"clear");
    DECLM (WXmPushButton,	canc,		f_form,		"cancel");
    DECLM (WXmPushButton,	help,		f_form,		"help");

    mtfstring =  msg_catalog_mgr().catgets(Set_AgentLabel, 160);
    XtVaSetValues(label, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring =  msg_catalog_mgr().catgets(Set_AgentLabel, 161);
    XtVaSetValues(f_ok, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring =  msg_catalog_mgr().catgets(Set_AgentLabel, 162);
    XtVaSetValues(canc, XmNlabelString, (XmString)mtfstring, NULL);

    mtfstring =  CATGETS(Set_AgentLabel, 223, "Clear");
    XtVaSetValues(f_clr, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring =  CATGETS(Set_AgentLabel, 270, "...");
    XtVaSetValues(pick, XmNlabelString, (XmString)mtfstring, NULL);
    mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
    XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);


    f_form.DefaultButton (f_ok);
    SET_CALLBACK (f_text, ValueChanged, textChanged);
    SET_CALLBACK (f_text, ModifyVerify, verifyNoSpace);
    ON_ACTIVATE (pick, pick_dir);
    ON_ACTIVATE (f_ok, ok);
    ON_ACTIVATE (f_clr, clear);
    ON_ACTIVATE (canc, cancel);
    help_agent().add_activate_help (help, (char*)"add_infolib_help");

    f_shell.Realize ();
    // XtVaSetValues(f_form, XmNinitialFocus, (Widget)f_text, NULL);
}

void
AddLibraryAgent::ok () {
    f_done = TRUE;
    f_form.Unmanage ();
}

void
AddLibraryAgent::clear () {
 // autoUnmanage must be set to False for this button to work properly

    XmTextFieldSetString( f_text, (char*)"" );
}

void
AddLibraryAgent::cancel () {
    f_done = TRUE;
    f_cancelled = TRUE;
    f_form.Unmanage ();
}

void
AddLibraryAgent::textChanged (WCallback *wcb) {
    char *theText = XmTextFieldGetString (wcb->GetWidget());
    if( theText && *theText ) {
        f_ok.SetSensitive (True);
        f_clr.SetSensitive (True);
    }
    else
    {
        f_ok.SetSensitive (False);
        f_clr.SetSensitive (False);
    }
    XtFree (theText);
}

void
AddLibraryAgent::verifyNoSpace (WCallback *wcb) {
    CALL_DATA (XmTextVerifyCallbackStruct, tvp);
    tvp->doit = True;
    if (tvp->text != NULL) {
	int length = tvp->text->length;
	char *cp = tvp->text->ptr;
	for ( ; length > 0; cp ++, length --) {
	    //if (*cp == ' ' || *cp == '\t' || *cp == '\n') {
	    if (*cp == '\t' || *cp == '\n') {
		tvp->doit = False;
		return;
	    }
	}
    }
}

char *
AddLibraryAgent::getInfolibName () {

    f_done = f_cancelled = False;
    f_form.Manage();
    XmProcessTraversal( (Widget)f_text, XmTRAVERSE_CURRENT );
    // XtSetKeyboardFocus( (Widget)f_form, (Widget)f_text );
    // ((WXmForm *) XtParent(XtParent (f_text)))->InitialFocus (f_text);

    f_shell.MinWidth (f_shell.Width());
    f_shell.MinHeight (f_shell.Height());
    f_shell.MaxHeight (f_shell.Height());

    XtAppContext app_context = window_system().app_context ();
    XEvent event;
    while (!f_done) {
	XtAppNextEvent (app_context, &event);
	XtDispatchEvent (&event);
    }
    if (f_cancelled)
	return NULL;
    return XmTextFieldGetString (f_text);
}

void
AddLibraryAgent::pick_dir () {

  if( !f_file_sel )
  {
    Arg args[20];
    unsigned int len, slen;
    int n;

    XmString title_str = XmStringCreateLocalized(
          CATGETS(Set_AddLibraryAgent, 10, "Dtinfo: Infolib Selection"));
    XmString no_libs_str = XmStringCreateLocalized(
          CATGETS(Set_AddLibraryAgent, 11, "No Infolibs Present"));
    XmString patt_str = XmStringCreateLocalized( (char*)"*.dti" );
    XmString infolibs_str = XmStringCreateLocalized( (char*)"Infolibs" );

    // default initial directory on first entry
    unsigned int buflen = 256;
    char *buf = new char[buflen];

// Note: infolibs need to be placed in platform-portably-named
//       LANG-type subdirectories, so the normalized CDE language
//       string can be used for lookup. For now, must assume the
//       user environment LANG is OK ...
    char *tmpstr;
    if( (tmpstr = getenv("LANG")) )
	snprintf( buf, buflen, "/usr/dt/infolib/%s/", tmpstr );
    else
	snprintf( buf, buflen, "/usr/dt/infolib/%s/", env().lang() );
//
    XmString basedir_str = XmStringCreateLocalized( buf );
    slen = strlen(buf);
    len = MIN(5, buflen - 1 - slen);
    *((char *) memcpy(buf + slen, "*.dti", len) + len) = '\0';
    XmString initdir_str = XmStringCreateLocalized( buf );
    delete [] buf;

    n = 0;
    XtSetArg(args[n], XmNdialogTitle, title_str); n++;
    XtSetArg(args[n], XmNdirectory, basedir_str); n++;
    XtSetArg(args[n], XmNdirMask, initdir_str); n++;
    XtSetArg(args[n], XmNfileListLabelString, infolibs_str); n++;
    XtSetArg(args[n], XmNfileTypeMask, XmFILE_DIRECTORY);  n++;
    XtSetArg(args[n], XmNnoMatchString, no_libs_str); n++;
    XtSetArg(args[n], XmNpattern, patt_str); n++;
    XtSetArg(args[n], XmNpathMode, XmPATH_MODE_RELATIVE); n++;

    f_file_sel = XmCreateFileSelectionDialog(
                      f_shell, (char*)"pick_dir_file", args, n );

    XmStringFree(basedir_str);
    XmStringFree(initdir_str);
    XmStringFree(infolibs_str);
    XmStringFree(patt_str);
    XmStringFree(no_libs_str);
    XmStringFree(title_str);

    XtAddCallback ( f_file_sel,
                      XmNokCallback,
                      &AddLibraryAgent::fileSelectedCallback,
                      (XtPointer) this );
    XtAddCallback ( f_file_sel,
                      XmNcancelCallback,
                      &AddLibraryAgent::cancelCallback,
                      (XtPointer) this );
  }
  XtManageChild( f_file_sel );
}

void
AddLibraryAgent::cancelCallback ( Widget w,
                                     XtPointer,
                                     XtPointer )
{
    XtUnmanageChild ( w );   // Bring the file selection dialog down.
}

void
AddLibraryAgent::fileSelectedCallback ( Widget    w,
                                          XtPointer client_data,
                                          XtPointer call_data )
{
    AddLibraryAgent * obj = (AddLibraryAgent *) client_data;

    XmFileSelectionBoxCallbackStruct *cb =
        (XmFileSelectionBoxCallbackStruct *) call_data;
    char     *name   = NULL;
    XmString  xmstr  = cb->value;  // The selected file
    int       status = 0;

    if ( xmstr )   // Make sure a file was selected
    {
        // Extract the first character string matching the default
        // character set from the compound string

        status = XmStringGetLtoR ( xmstr, XmFONTLIST_DEFAULT_TAG, &name );
         if(status && strlen(name)<1)
         {
            XtFree(name);
            return;
         }
         if( status )
         {
            XmTextFieldSetString( (Widget) obj->f_text, name );
            // XtSetKeyboardFocus( (Widget) obj->f_form, (Widget) obj->f_ok );
            XmProcessTraversal( (Widget) obj->f_ok, XmTRAVERSE_CURRENT );
         }
    }
    XtUnmanageChild ( w );   // Bring the file selection dialog down.
}


