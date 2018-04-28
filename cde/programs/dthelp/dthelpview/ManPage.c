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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: ManPage.c /main/5 1996/09/30 11:28:22 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         ManPage.c
 **
 **   Project:	   Cache Creek 
 **
 **   Description: Contains the code for generating the man page dialog 
 **                used in the helpview program.
 **		  
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/* System Include Files  */
#include <stdio.h>
#include <signal.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <Xm/Protocols.h>
#include <Xm/MwmUtil.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/DialogS.h>

#include <Dt/Help.h>
#include <Dt/HelpDialog.h>

#include <DtI/HelposI.h>
#include <DtI/HourGlassI.h>
/* Local Includes */
#include "Main.h"
#include "UtilI.h"
#include "ManPageI.h"

/********    Static Function Declarations    ********/


static void ManTextUpdateCB(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data);
static void ManDisplayCB(
    Widget widget,
    XtPointer client_data,
    XtPointer callback);
static void CatchClose(
    Widget widget);




/****************************************************************************
 * Function:         CatchClose( Widget w);
 *                          
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Catches the window manager close requests and assigns our
 *                  CloseHelpCB to handel them.
 *
 ***************************************************************************/
static void CatchClose (
    Widget widget)
{

  Atom      wm_delete_window;
  Arg       args[2];

  /* Grab the window mgr close */
  wm_delete_window = XmInternAtom(XtDisplay(XtParent(widget)),
                                            "WM_DELETE_WINDOW", FALSE);
  XtSetArg(args[0], XmNdeleteResponse, XmDO_NOTHING);

  /* Current Help Dialog Window */
  XmAddWMProtocolCallback(XtParent(widget),wm_delete_window,
                          (XtCallbackProc)CloseAndExitCB, (XtPointer)widget);
  XtSetValues(XtParent(widget), args, 1);
  
} 




/****************************************************************************
 * Function:	    static void PostManDialog();
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Creates and manages a simple prompt dialog that allows a 
 *                  user to type in and display any manpage on the system.
 *
 ****************************************************************************/
void PostManDialog(
    Widget parent,
    int     argc,
    char    **argv)

{

  Widget separator;
  Widget manLabel;
  XmString labelStr;
  Arg args[20];
  int n;
  char * title;
  Pixel foreground;
  Pixel background;
       
  Colormap colormap;
  Pixel selectColor;
  Pixel topShadowColor;
  Pixel bottomShadowColor;
  Pixel foregroundColor;
 

   /*  Create the shell and form used for the dialog.  */

   title = XtNewString(((char *)_DTGETMESSAGE(7, 11, "Man Page")));
   n = 0;
   XtSetArg (args[n], XmNtitle, title);	        n++;
   XtSetArg (args[n], XmNallowShellResize, False);	n++;
   manWidget =  XmCreateDialogShell(parent, "manWidget", args, n);
   XtFree(title);

   /* Set the useAsyncGeo on the shell */
   n = 0;
   XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
   XtSetValues (XtParent(manWidget), args, n);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);				n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNshadowThickness, 1);				n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);			n++;
   XtSetArg (args[n], XmNautoUnmanage, False);				n++;
   manForm = XmCreateForm (manWidget, "manForm", args, n);

   /*  Get the select color and margin widths and heights  */
   n = 0;
   XtSetArg(args[n], XmNforeground, &foreground);		++n;
   XtSetArg(args[n], XmNbackground, &background);		++n;
   XtSetArg (args[n], XmNcolormap,  &colormap);		++n;
   XtGetValues(manForm, args, n);

   XmGetColors (XtScreen (manForm), colormap, background,
	       &foregroundColor, &topShadowColor,
	       &bottomShadowColor, &selectColor);

   labelStr = XmStringCreateLocalized((char *)_DTGETMESSAGE(7, 12,"Man Page:"));
   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); 	++n;
   XtSetArg(args[n], XmNleftOffset,5);			        ++n;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		++n;
   XtSetArg(args[n], XmNtopOffset, 15);				++n;
   XtSetArg(args[n], XmNlabelType, XmSTRING);			++n;
   XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);	++n;
   XtSetArg(args[n], XmNtraversalOn, False);			++n;
   XtSetArg(args[n], XmNlabelString, labelStr);		++n;
   manLabel = XmCreateLabelGadget(manForm, "manLabel", args, n);
   XtManageChild (manLabel);
   XmStringFree(labelStr); 

   n = 0;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	++n;
   XtSetArg(args[n], XmNleftWidget, manLabel);  		++n;
   XtSetArg(args[n], XmNleftOffset, 5);		                ++n;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		++n;
   XtSetArg(args[n], XmNtopOffset, 10);				++n;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	++n;
   XtSetArg(args[n], XmNrightOffset, 5);			++n;
   XtSetArg(args[n], XmNeditable, True);			++n;
   XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT);		++n;
   XtSetArg(args[n], XmNbackground, selectColor); 		++n;
   manText = XmCreateTextField(manForm, "man_text", args, n);
   XtManageChild (manText);  
   XtAddCallback(manText, XmNvalueChangedCallback, 
                 ManTextUpdateCB,(XtPointer) NULL);
 
   /*  Create a separator between the buttons  */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget,manLabel);n++;
   XtSetArg (args[n], XmNtopOffset, 20);				n++;
   separator =  XmCreateSeparatorGadget (manForm, "separator", args, n);
   XtManageChild (separator);

   /*  Create the action buttons along the bottom */
   labelStr = XmStringCreateLocalized ((char *)_DTGETMESSAGE(7, 13,
							"Show Man Page"));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelStr);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 1);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 49);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   manBtn = XmCreatePushButtonGadget(manForm, "manBtn", args, n);
   XtManageChild (manBtn);
   XmStringFree(labelStr);     
   XtSetSensitive(manBtn, False);

   XtAddCallback(manBtn, XmNactivateCallback, ManDisplayCB,
                 (XtPointer) manForm);
  

   /* Build the Close button */
   labelStr = XmStringCreateLocalized((char *)_DTGETMESSAGE(7, 16,"Exit"));

   n = 0;
   XtSetArg (args[n], XmNlabelString, labelStr);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 51);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 99);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   closeBtn = XmCreatePushButtonGadget (manForm, "closeBtn", args, n);
   XtManageChild (closeBtn);

   XtAddCallback(closeBtn, XmNactivateCallback, CloseAndExitCB,
                 (XtPointer) NULL);
  	     
   XmStringFree (labelStr);

   XtSetArg (args[0], XmNdefaultButton, closeBtn);
   XtSetValues (manForm, args, 1);

   /** force tabs to go to each widget and in right order **/
   XtSetArg (args[0], XmNnavigationType, XmSTICKY_TAB_GROUP);
   XtSetValues (manText,args,1);
   XtSetValues (manBtn,args,1);
   XtSetValues (closeBtn,args,1);

   /** put focus on the text field **/
   XtSetArg (args[0], XmNinitialFocus, manText);
   XtSetValues (manForm,args,1);


   /*  Adjust the decorations for the dialog shell of the dialog  */
   n = 0;
   XtSetArg(args[n], XmNmwmFunctions, 
            MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_MAXIMIZE |
            MWM_FUNC_CLOSE);   n++;
   XtSetArg (args[n], XmNmwmDecorations,
            MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU | 
            MWM_DECOR_MINIMIZE | MWM_DECOR_MAXIMIZE); n++;
   XtSetValues (manWidget, args, n);
    
    
   XtManageChild(manForm);
 
   /* Add the CatchClose here so we catch the window manager close requests */
   CatchClose(manForm);

   /* Set the wm_command property on the help dialog window */
   if (argc != 0)
     XSetCommand(XtDisplay(manWidget), XtWindow(manWidget), argv, argc);

}

  
/**************************************************************************
 * Function:   ManTextUpdateCB
 *
 *   ManText is called when the string in the man text Widget changes.  We
 * activate or deactivate the display man page PushButton.
 *
 * Called by:
 **************************************************************************/

static void ManTextUpdateCB(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data )
{
  char *textString;
  Arg args[2];

  /* Get the text string to check its length, then set the search buttons' */
  /* sensitivity appropriately. */
  
  textString = XmTextFieldGetString(widget);
  if (strlen(textString) == 0)
    {
       XtSetSensitive(manBtn, False);
       XtSetArg (args[0], XmNdefaultButton, closeBtn);
       XtSetValues (manForm, args, 1);
    }
  else
    {
      XtSetSensitive(manBtn, True);
      XtSetArg (args[0], XmNdefaultButton, manBtn);
      XtSetValues (manForm, args, 1);

    }
  XtFree(textString);

} /* End ManTextCB */


/*******************************************************************
 * Function: ManDisplayCB
 *
 * ManDisplayCB is called when the user hits Return in the man text
 * Widget or clicks  the display man PushButton.
 *******************************************************************/
static void ManDisplayCB(
    Widget widget,
    XtPointer client_data,
    XtPointer call_data )
{

  char *manPageStr;  
  
  _DtHelpTurnOnHourGlass((Widget) client_data);
  manPageStr = XmTextFieldGetString(manText);  

  DisplayMan(topLevel, manPageStr, NO_EXIT_ON_CLOSE);
  
  _DtHelpTurnOffHourGlass((Widget) client_data);      
  XtFree(manPageStr);

} /* End ManDisplayCB */























