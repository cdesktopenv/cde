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
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Mouse.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle Mouse dialog 
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*$XConsortium: Mouse.c /main/5 1996/07/19 13:45:42 pascale $*/

#include <X11/Xlib.h>
#include <errno.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/MwmUtil.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/VendorSEP.h>

#include <Dt/DialogBox.h>
#include <Dt/Message.h>
#include <Dt/SessionM.h>
#include <Dt/HourGlass.h>
#include <Dt/UserMsg.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Mouse.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define ACCEL_MIN        0
#define ACCEL_MAX        10
#define THRESH_MIN       0
#define THRESH_MAX       15
#define THRESHDEFAULT    10
#define DCLICK_MIN       0
#define DCLICK_MAX       10
#define POINTER2B        0
#define POINTER3B        1
#define DCLICKDEFAULT    500
#define RIGHT_HANDED     0
#define LEFT_HANDED      1
#define TRANSFER         0
#define ADJUST           1


#define LEFTMSG ((char *)GETMESSAGE(9, 1, "Left"))
#define RIGHTMSG ((char *)GETMESSAGE(9, 2, "Right"))
#define HANDMSG ((char *)GETMESSAGE(9, 3, "Handedness:"))
#define MIDDLEMSG ((char *)GETMESSAGE(9, 4, "Button 2:"))
#define MSG3  ((char *)GETMESSAGE(9, 10, "ERROR: setting pointer mapping\n"))
#define TRANSMSG ((char *)GETMESSAGE(9, 11, "Transfer"))
#define ADJMSG ((char *)GETMESSAGE(9, 12, "Adjust"))
#define WARN_MSG  ((char *)GETMESSAGE(9, 13, "The right and left mouse buttons will swap\n\
functions immediately after you click on OK.\n\
The `Test Double-Click' graphic in the Mouse\n\
window shows the current handedness by the \n\
positions of Button 1 and Button 3."))
#define MID_WARN_MSG_T  ((char *)GETMESSAGE(9, 14, "If you click on OK, mouse button 2 will be set\n\
to `transfer' (drag) at your next session."))
#define IMMEDIATE_MSG  ((char *)GETMESSAGE(9, 15, "The new double-click time will take effect\n\
as applications are restarted."))
#define  LATER_MSG  ((char *)GETMESSAGE(9, 16, "The new double-click time will take effect\n\
at your next session."))
#define MID_WARN_MSG_A  ((char *)GETMESSAGE(9, 19, "If you click on OK, mouse button 2 will be set\n\
to `adjust' at your next session. Button 2 may\n\
be used to adjust text and list selections.\n\
Transfer actions (drag) on text and lists will\n\
require the use of Button 1."))
#define MENUMSG ((char *)GETMESSAGE(9, 20, "Menu"))

#define B2	        "Dtms2B.bm"
#define B2_REV	        "Dtms2BR.bm"
#define B3	        "Dtms3B.bm"
#define B3_REV	        "Dtms3BR.bm"

static char *multiClickTimeString = "*multiClickTime:%d\n";
static char *enableBtn1TransferString = "*enableBtn1Transfer:%s\n";
static char *mouseString = "Mouse";

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static Widget build_mouseDialog( 
                        Widget shell) ;
static void formLayoutCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void _DtmapCB_mouseDialog( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static int getValue( 
                        Widget w) ;
static void valueChangedCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void dclickVCCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void dclickTestCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void reverseToggleCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void midreverseToggleCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void warnToggleCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void midwarnToggleCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void cancelWarnCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void midcancelWarnCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void systemDefaultCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ButtonCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget      pictFrame;
    Widget      pictRC;
    Widget      pictButton;
    Widget      pictLabel;
    Widget      handLabGad;
    Widget      rightToggle;
    Widget      leftToggle;
    Widget      middleLabGad;
    Widget      transferToggle;
    Widget      adjustToggle;
    Widget      systemDefault;
    Widget      accelLabGad;
    Widget      threshLabGad;
    Widget      dclickLabGad;
    Widget  	accelScale;
    Widget  	threshScale;
    Widget  	dclickScale;
    Pixmap      pixmap;
    Pixmap      b3; 
    Pixmap      b3_rev; 
    Pixmap      b2; 
    Pixmap      b2_rev;
    Pixmap      pixmapSel; 
    Pixmap      b3Sel; 
    Pixmap      b3_revSel; 
    Pixmap      b2Sel; 
    Pixmap      b2_revSel;
    int         type;    
    int         numButtons;
    int         accel_numerator;
    int         accel_denominator;
    int         threshold;
    Boolean     dclickChanged;
    Boolean     btn2Changed;
    XtIntervalId     timeID;
    int         mydclick;
    Boolean     systemDefaultFlag;
    unsigned char map_return[256];
    unsigned char new_map_return[256];
    Boolean     origHanded;
    Boolean     handed;
    Boolean     function;
    Boolean     origFunction;
} Mouse;

static Widget warnDialog;
static Widget midwarnDialog;

static Mouse mouse;
static saveRestore save = {FALSE, 0, };


/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_mouseBB                         */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
popup_mouseBB( Widget shell )
{
    if (style.mouseDialog == NULL) {
        _DtTurnOnHourGlass(shell);  
        build_mouseDialog(shell);
        mouse.systemDefaultFlag = True;
	mouse.dclickChanged = False;
	mouse.btn2Changed = False;
        XtManageChild(style.mouseDialog);
        _DtTurnOffHourGlass(shell);  
    }
    else 
    {
        XtManageChild(style.mouseDialog);
        raiseWindow(XtWindow(XtParent(style.mouseDialog)));
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* build__mouseDialog                    */
/*+++++++++++++++++++++++++++++++++++++++*/
static Widget 
build_mouseDialog( Widget shell )
{
    register int     i, n;
    Arg              args[MAX_ARGS];
    int              nmap;
    XmString         button_string[NUM_LABELS]; 
    Widget           form;
    int              count = 0;
    Widget           widget_list[15]; 
    XmString         string;
    
    Dimension        MaxLabelWidth1;
    Dimension        HandLabelWidth;
    Dimension        MiddleLabelWidth;
    Dimension        MaxToggleWidth;
    Dimension        RightToggleWidth;
    Dimension        TransferToggleWidth;

    XrmDatabase database;
    char *str_name;
    char *str_class;
    char **str_type_return;
    XrmValue value_return;
    
    /* check hil device# for 2 or 3 button mouse      */
    /* IMPORTANT NOTE:                                */
    /* X servers will return different values for     */
    /* mouse.numButtons in the call below.            */ 
    /* The HP X server will return 5 if a quarted     */ 
    /* threebutton mouse is present and a 3 if a      */
    /* quarted two-button mouse is present. The IBM   */
    /* does not support a two-button mouse.           */
    /* The cod ebelow always loads a three-button     */
    /* for IBM/Sun/HP unless a non-quarted two-button */
    /* mouse is present on the HP server.             */
    /* When porting this code to a non IBM/Sun/HP     */
    /* platform that has or supports a two-button     */
    /* mouse the code below may have to be changed to */
    /* take into account the behavior of the new      */
    /* server.                                        */ 
    
    nmap = 256;
    mouse.numButtons = XGetPointerMapping(style.display, mouse.map_return, 
					   nmap);
    if ( mouse.numButtons == 2 )
          mouse.type = POINTER2B;
    else if  ( mouse.numButtons == 3 || mouse.numButtons == 5 ) 
        mouse.type = POINTER3B;
    else
        mouse.type = 99;
    /* choose a bitmap based on button mapping   *
     * and number of buttons                     */

    if(mouse.type == POINTER3B) 
    {
        mouse.b3     = XmGetPixmap(style.screen, B3,
				 style.secBSCol, style.secTSCol);
        mouse.b3_rev = XmGetPixmap(style.screen, B3_REV,
                           	 style.secBSCol, style.secTSCol);
        mouse.b3Sel  = XmGetPixmap(style.screen, B3,
	                         style.secTSCol, style.secBSCol );
        mouse.b3_revSel = XmGetPixmap(style.screen, B3_REV,
 				 style.secTSCol, style.secBSCol);

        if(mouse.map_return[0] == 1) 
	{
            mouse.pixmap = mouse.b3;
            mouse.pixmapSel = mouse.b3Sel;
        }
        else 
	{
            mouse.pixmap = mouse.b3_rev;
            mouse.pixmapSel = mouse.b3_revSel;
        }
    }
    else /* 2 button mouse */ 
    {
	mouse.b2        = XmGetPixmap(style.screen, B2,
                             style.secBSCol, style.secTSCol);
	mouse.b2_rev    = XmGetPixmap(style.screen, B2_REV,
                            style.secBSCol, style.secTSCol);
        mouse.b2Sel     = XmGetPixmap(style.screen, B2,
                             style.secTSCol, style.secBSCol);
        mouse.b2_revSel = XmGetPixmap(style.screen, B2_REV,
                             style.secTSCol, style.secBSCol);

        if(mouse.map_return[0] == 1)
        {
            mouse.pixmap = mouse.b2;
            mouse.pixmapSel = mouse.b2Sel;
        }
        else
        {
            mouse.pixmap = mouse.b2_rev;
            mouse.pixmapSel = mouse.b2_revSel;
        }
    }

    /*Get the value of doubleclick*/
    mouse.mydclick = XtGetMultiClickTime(style.display);
    if (mouse.mydclick > 0)
      mouse.mydclick = mouse.mydclick/100;
    if (mouse.mydclick < 1)
      mouse.mydclick = 1;

    /*Get the value of the enableBtn1Transfer resource */
    /*from the data base. */

    str_name = "*enableBtn1Transfer";
    str_class = "*EnableBtn1Transfer";
    
    database = XrmGetDatabase(style.display); 
    if (!XrmGetResource(database, str_name, str_class, (char **)&str_type_return, &value_return))
      {	
	mouse.function = TRANSFER;
	mouse.origFunction = TRANSFER;
      }
    else 
      if (strcmp(value_return.addr, "True") == 0) 
	{
	  mouse.function = ADJUST;
	  mouse.origFunction = ADJUST;
	}
      else
	{
	  mouse.function = TRANSFER;
	  mouse.origFunction = TRANSFER;
	}
  
    /* Set up DialogBoxDialog button labels */
    button_string[0] = CMPSTR((String) _DtOkString);
    button_string[1] = CMPSTR((String) _DtCancelString);
    button_string[2] = CMPSTR((String) _DtHelpString);

    /* Create toplevel DialogBox */
    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.  
     * save.posArgs may contain information from restoreMouse().*/

    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  
    save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);
    save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNdefaultPosition, False); 
    save.poscnt++;
    style.mouseDialog = __DtCreateDialogBoxDialog(shell, mouseString, 
                   save.posArgs, save.poscnt);
    XtAddCallback(style.mouseDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(style.mouseDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_MOUSE_DIALOG);


    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widget_list[0] = _DtDialogBoxGetButton(style.mouseDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widget_list[0]); n++;
    XtSetValues (style.mouseDialog, args, n);

    n=0;
    XtSetArg(args[n], XmNtitle, ((char *)GETMESSAGE(9, 18, "Style Manager - Mouse"))); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;  
    XtSetValues (XtParent(style.mouseDialog), args, n);

    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    form = XmCreateForm(style.mouseDialog, "mouseForm", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
    string = CMPSTR(((char *)GETMESSAGE(9, 5, "Default")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    mouse.systemDefault= XmCreatePushButtonGadget(form, "systemDefault", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++; 
    widget_list[count++] = 
    mouse.pictFrame= XmCreateFrame(form, "pictFrame", args, n);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++; 
    XtSetArg(args[n], XmNadjustMargin, False); n++; 
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING); n++; 
    mouse.pictRC = XmCreateRowColumn(mouse.pictFrame, "pictRC", args, n);

    n = 0;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++; 
    XtSetArg(args[n], XmNlabelPixmap, mouse.pixmap); n++; 
    XtSetArg(args[n], XmNrecomputeSize, False); n++; 
    XtSetArg(args[n], XmNmarginWidth, 0); n++; 
    XtSetArg(args[n], XmNmarginHeight, 0); n++; 
    XtSetArg(args[n], XmNshadowThickness, 0); n++; 
    XtSetArg(args[n], XmNmultiClick, XmMULTICLICK_KEEP); n++;
    mouse.pictButton= XmCreatePushButton(mouse.pictRC, "mousePict", args, n);

    n = 0;
    string = CMPSTR ((char *)GETMESSAGE(9, 17, "Test\nDouble-Click"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    mouse.pictLabel= XmCreateLabelGadget(mouse.pictRC, "mouseLabel", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    string = CMPSTR(HANDMSG);
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    mouse.handLabGad= XmCreateLabelGadget(form,"handLabGad", args, n);
    XmStringFree(string);

    n=0;
    string = CMPSTR(RIGHTMSG); 
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;  
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    mouse.rightToggle= XmCreateToggleButtonGadget(form, 
                   "rightToggle", args, n);
    XmStringFree(string);

    n=0;
    string = CMPSTR(LEFTMSG); 
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;  
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    mouse.leftToggle= XmCreateToggleButtonGadget(form, 
                  "leftToggle", args, n);
    XmStringFree(string);


    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    string = CMPSTR(MIDDLEMSG);
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] =
    mouse.middleLabGad= XmCreateLabelGadget(form,"middleLabGad", args, n);
    XmStringFree(string);

    n=0;
    string = CMPSTR(TRANSMSG); 
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;  
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    mouse.transferToggle= XmCreateToggleButtonGadget(form, 
                   "transToggle", args, n);
    XmStringFree(string);

    n=0;
    string = CMPSTR(ADJMSG); 
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;  
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    mouse.adjustToggle= XmCreateToggleButtonGadget(form, 
                  "adjToggle", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    string = CMPSTR(((char *)GETMESSAGE(9, 6, "Double-Click")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    mouse.dclickLabGad= XmCreateLabelGadget(form,"dclickLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNmaximum, DCLICK_MAX); n++; 
    XtSetArg(args[n], XmNminimum, DCLICK_MIN); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS);  n++; 
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue, mouse.mydclick); n++;
    XtSetArg(args[n], XmNdecimalPoints, (short)1); n++;
    widget_list[count++] = 
    mouse.dclickScale= XmCreateScale(form,"dclickScale", args, n);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    string = CMPSTR(((char *)GETMESSAGE(9, 8, "Acceleration")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    mouse.accelLabGad= XmCreateLabelGadget(form,"accelLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNminimum, ACCEL_MIN); n++; 
    XtSetArg(args[n], XmNmaximum, ACCEL_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS);  n++; 
    XtSetArg(args[n], XmNshowValue, True); n++;
    widget_list[count++] = 
    mouse.accelScale= XmCreateScale(form,"accelScale", args, n);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    string = CMPSTR(((char *)GETMESSAGE(9, 9, "Threshold")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    mouse.threshLabGad= XmCreateLabelGadget(form,"threshLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNminimum, THRESH_MIN); n++; 
    XtSetArg(args[n], XmNmaximum, THRESH_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS);  n++; 
    XtSetArg(args[n], XmNshowValue, True); n++;
    widget_list[count++] = 
    mouse.threshScale= XmCreateScale(form,"threshScale", args, n);

    XtAddCallback(style.mouseDialog, XmNmapCallback, formLayoutCB, NULL);
    XtAddCallback(style.mouseDialog, XmNmapCallback, _DtmapCB_mouseDialog, shell);
    XtAddCallback(mouse.systemDefault, XmNactivateCallback, 
                  systemDefaultCB, NULL);
    
    /* calculate the max width for toggles for the overall alignment */
    /* Set labels to max label width */ 

    MaxLabelWidth1 = HandLabelWidth = XtWidth(mouse.handLabGad);
    MiddleLabelWidth = XtWidth(mouse.middleLabGad);
    if (MiddleLabelWidth > MaxLabelWidth1) 
    {
	MaxLabelWidth1 = MiddleLabelWidth;
    }
   
    MaxToggleWidth = RightToggleWidth = XtWidth(mouse.rightToggle);
    TransferToggleWidth = XtWidth(mouse.transferToggle);
    if (TransferToggleWidth > MaxToggleWidth) 
    {
	MaxToggleWidth = TransferToggleWidth;
    }
    

    if (mouse.function == TRANSFER)
      {
	XmToggleButtonGadgetSetState(mouse.transferToggle, True, True);
	XmToggleButtonGadgetSetState(mouse.adjustToggle, False, False);
      }
    else
      {
	XmToggleButtonGadgetSetState(mouse.adjustToggle, True, True);
	XmToggleButtonGadgetSetState(mouse.transferToggle, False, False);
      }

    XtVaSetValues(mouse.handLabGad, XmNwidth, MaxLabelWidth1, NULL);
    XtVaSetValues(mouse.middleLabGad, XmNwidth, MaxLabelWidth1, NULL);

    XtVaSetValues(mouse.rightToggle, XmNwidth, MaxToggleWidth, NULL);
    XtVaSetValues(mouse.transferToggle, XmNwidth, MaxToggleWidth, NULL);

    XtAddCallback(mouse.rightToggle, XmNvalueChangedCallback, 
                  warnToggleCB, (XtPointer)RIGHT_HANDED);

    XtAddCallback(mouse.leftToggle, XmNvalueChangedCallback, 
                  warnToggleCB, (XtPointer)LEFT_HANDED);

    XtAddCallback(mouse.transferToggle, XmNvalueChangedCallback, 
                  midwarnToggleCB, (XtPointer)TRANSFER);

    XtAddCallback(mouse.adjustToggle, XmNvalueChangedCallback, 
                  midwarnToggleCB, (XtPointer)ADJUST);

    XtAddCallback(mouse.accelScale, XmNvalueChangedCallback, 
                  valueChangedCB, NULL);

    XtAddCallback(mouse.threshScale, XmNvalueChangedCallback, 
                  valueChangedCB, NULL);
    XtAddCallback(mouse.dclickScale, XmNvalueChangedCallback, 
                  dclickVCCB, (caddr_t)mouse.pictButton);
    XtAddCallback(mouse.pictButton, XmNactivateCallback, 
                  dclickTestCB, NULL);  

    XtManageChild(form);
    XtManageChildren(widget_list,count);
    XtManageChild(mouse.pictRC);
    XtManageChild(mouse.pictButton);
    XtManageChild(mouse.pictLabel);
    XtManageChild(mouse.rightToggle);
    XtManageChild(mouse.leftToggle);

    /*
     * If there are only two mouse buttons then switch the
     * label for the transferToggle to "Menu" and mark it
     * insensitive but checked.
     * Otherwise manage both the transfer and adjust toggles.
     */
    if (mouse.numButtons == 2) {
     	n=0;
     	string = CMPSTR(MENUMSG); 
     	XtSetArg(args[n], XmNlabelString, string); n++;
	XtSetValues(mouse.transferToggle, args, n);
    	XmStringFree(string);
	XmToggleButtonGadgetSetState(mouse.transferToggle, True, False);
	XtManageChild(mouse.transferToggle);
    } else {
	XtManageChild(mouse.transferToggle);
	XtManageChild(mouse.adjustToggle);
    }

    return(style.mouseDialog);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* formLayoutCB                          */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
formLayoutCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int              n;
    Arg              args[MAX_ARGS];
    Dimension        MaxLabelWidth;
    Dimension        AccelLabelWidth;
    Dimension        ThreshLabelWidth;
    Dimension        DclickLabelWidth;
    Dimension        ScaleHeight;
    Dimension        LabelHeight;
    int              TopOffset;
    int              LeftOffset;

    /* Do the scale and label attachments */

    ScaleHeight = XtHeight(mouse.accelScale);

    MaxLabelWidth = AccelLabelWidth = XtWidth(mouse.accelLabGad);
    LabelHeight = XtHeight(mouse.accelLabGad);

    ThreshLabelWidth = XtWidth(mouse.threshLabGad);
    if (ThreshLabelWidth > MaxLabelWidth) 
    {
	MaxLabelWidth = ThreshLabelWidth;
    }

    DclickLabelWidth = XtWidth(mouse.dclickLabGad);
    if (DclickLabelWidth > MaxLabelWidth) 
    {
	MaxLabelWidth = DclickLabelWidth;
    }
   
    /* Mouse Picture PushButton */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNtopOffset,        style.verticalSpacing);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNleftOffset,       style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);      n++;
    XtSetValues (mouse.pictFrame, args, n);

    
    /* System Default Button */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNtopOffset,        style.verticalSpacing);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNrightOffset,      style.horizontalSpacing); n++;
    XtSetValues (mouse.systemDefault, args, n);
 
 
    /*  Handedness Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.systemDefault); n++;
    XtSetArg(args[n], XmNtopOffset,        4*style.verticalSpacing);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.pictFrame);    n++;
    XtSetArg(args[n], XmNleftOffset,       2*style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);    n++;
    XtSetValues (mouse.handLabGad, args, n);

    /* right toggle */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.handLabGad); n++;
    XtSetArg(args[n], XmNtopOffset,        -5);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.handLabGad);    n++;
    XtSetArg(args[n], XmNleftOffset,       style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);    n++;
    XtSetValues (mouse.rightToggle, args, n);
    
    /* left toggle */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.handLabGad); n++;
    XtSetArg(args[n], XmNtopOffset,        -5);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.rightToggle);    n++;
    XtSetArg(args[n], XmNleftOffset,       style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);    n++;
    XtSetValues (mouse.leftToggle, args, n);


    /*  Middle button Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.handLabGad); n++;
    XtSetArg(args[n], XmNtopOffset,        3*style.verticalSpacing);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.pictFrame);    n++;
    XtSetArg(args[n], XmNleftOffset,       2*style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);    n++;
    XtSetValues (mouse.middleLabGad, args, n);


    /* transfer toggle */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.middleLabGad); n++;
    XtSetArg(args[n], XmNtopOffset,        -5);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.rightToggle);    n++;
    XtSetArg(args[n], XmNleftOffset,       0); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);    n++;
    XtSetValues (mouse.transferToggle, args, n);
    
    /* adjust toggle */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.middleLabGad); n++;
    XtSetArg(args[n], XmNtopOffset,        -5);   n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.transferToggle);    n++;
    XtSetArg(args[n], XmNleftOffset,       style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);    n++;
    XtSetValues (mouse.adjustToggle, args, n);



    TopOffset = ScaleHeight + style.verticalSpacing - LabelHeight;

    /* DoubleClick Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.pictFrame) ;   n++;
    XtSetArg(args[n], XmNtopOffset,        TopOffset);          n++;    
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
    LeftOffset =  MaxLabelWidth + style.horizontalSpacing - DclickLabelWidth;
    XtSetArg(args[n], XmNleftOffset,       LeftOffset);         n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);      n++;
    XtSetValues (mouse.dclickLabGad, args, n);

    /* DoubleClick Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     mouse.dclickLabGad); n++;
    XtSetArg(args[n], XmNbottomOffset,     0);                  n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.dclickLabGad); n++;
    XtSetArg(args[n], XmNleftOffset,       2*style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
    XtSetValues (mouse.dclickScale, args, n);

    /* Acceleration Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.dclickLabGad); n++;
    XtSetArg(args[n], XmNtopOffset,        TopOffset);          n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
    LeftOffset =  MaxLabelWidth + style.horizontalSpacing - AccelLabelWidth;
    XtSetArg(args[n], XmNleftOffset,       LeftOffset);         n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);      n++;
    XtSetValues (mouse.accelLabGad, args, n);

    /* Acceleration Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     mouse.accelLabGad);  n++;
    XtSetArg(args[n], XmNbottomOffset,     0);                  n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.accelLabGad);  n++;
    XtSetArg(args[n], XmNleftOffset,       2*style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
    XtSetValues (mouse.accelScale, args, n);

    /* Threshold Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        mouse.accelScale);   n++;
    XtSetArg(args[n], XmNtopOffset,        TopOffset);          n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNbottomOffset,     style.verticalSpacing);   n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
    LeftOffset =  MaxLabelWidth + style.horizontalSpacing - ThreshLabelWidth;
    XtSetArg(args[n], XmNleftOffset,       LeftOffset);         n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);      n++;
    XtSetValues (mouse.threshLabGad, args, n);

    /* Threshold Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE);      n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     mouse.threshLabGad); n++;
    XtSetArg(args[n], XmNbottomOffset,     0);			n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNleftWidget,       mouse.threshLabGad); n++;
    XtSetArg(args[n], XmNleftOffset,       2*style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
    XtSetValues (mouse.threshScale, args, n);

    XtRemoveCallback(style.mouseDialog, XmNmapCallback, formLayoutCB, NULL);

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtmapCB_mouseDialog                     */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
_DtmapCB_mouseDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int         n;
    int         i;
    int         nmap;
    Arg         args[MAX_ARGS];
    static int  first_time = 1;

    if (first_time)
    {
        DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

        if (!save.restoreFlag)
	    putDialog((Widget)client_data, w);

        first_time = 0;
    }
    else
    {
        /* Don't need to do XGetPointerMapping first time dialog 
         * is mapped because it is done when dialog is created 
         * The type of device will not change between remapping 
         */
        nmap = 256;
        mouse.numButtons = XGetPointerMapping(style.display, 
                                mouse.map_return, nmap);
    }

    /* save mouse mapping values */
    for (i=0; i<mouse.numButtons; i++)
        mouse.new_map_return[i] = mouse.map_return[i];

    /* change Pointer Mapping */
    /* the ReverseToggleCB will set the correct Pixmap */
    if (mouse.map_return[0] == 1)  /* right-handed */
    {
        mouse.handed = RIGHT_HANDED;
        mouse.origHanded = RIGHT_HANDED;
	XmToggleButtonGadgetSetState(mouse.rightToggle, True, True);
	XmToggleButtonGadgetSetState(mouse.leftToggle, False, False);
    }
    else  /* left-handed */
    {
        mouse.handed = LEFT_HANDED;
        mouse.origHanded = LEFT_HANDED;
	XmToggleButtonGadgetSetState(mouse.leftToggle, True, True);
	XmToggleButtonGadgetSetState(mouse.rightToggle, False, False);
    }

    /* MultiClickTime will not change between remapping */

    XGetPointerControl(style.display, &mouse.accel_numerator, 
		&mouse.accel_denominator, &mouse.threshold);
    n=0;
    XtSetArg(args[n], XmNvalue,mouse.accel_numerator); n++;
    XtSetValues(mouse.accelScale, args, n);

    n=0;
    XtSetArg(args[n], XmNvalue,mouse.threshold); n++;
    XtSetValues(mouse.threshScale, args, n);

    
}

static int 
getValue(
        Widget w )
{
    int n, value;
    Arg args[1];

    n=0;
    XtSetArg(args[n], XmNvalue, &value); n++;
    XtGetValues(w, args, n);
    return(value);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* valueChangedCB                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
valueChangedCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    int     new_acceln, new_acceld, new_thresh;

     /* set mouse parameters to new scale values */
     new_acceld = 1;  
     new_acceln = getValue(mouse.accelScale);
     new_thresh = getValue(mouse.threshScale);

     XChangePointerControl(style.display, True, True, new_acceln,
                         new_acceld, new_thresh);

     mouse.systemDefaultFlag = False;

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* dclickVCCB                            */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
dclickVCCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XmScaleCallbackStruct *cb = (XmScaleCallbackStruct *) call_data;

  mouse.dclickChanged = True;
  XtSetMultiClickTime (style.display, (cb->value)*100);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* dclickTestCB                          */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
dclickTestCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    Arg  color[3], args[10];       /*  arg list            */
    register int    n;             /*  arg count           */
    XmPushButtonCallbackStruct *cb = (XmPushButtonCallbackStruct *)call_data;
    static int selectstate = False;

    if (cb->event->xbutton.button == 1)
    {
        if(cb->click_count == 2)
        {
	    if (selectstate) 
	    {
		n=0;
		XtSetArg(color[n], XmNlabelPixmap, mouse.pixmap); n++; 
		XtSetValues (w, color, n);

                if (style.useMultiColorIcons)
                {
                    n=0;
                    XtSetArg(color[n], XmNbackground, style.secBgCol); n++; 
                    XtSetValues (w, color, n);
                    XtSetValues (mouse.pictRC, color, n);
                    XtSetValues (mouse.pictLabel, color, n);
                }

		selectstate = False;
	    }   
	    else 
	    {
		/* This will change the  background color to select color,
		   but it causes the pixmap to redraw if IT was changed.*/ 

		n=0;
		XtSetArg(color[n], XmNlabelPixmap, mouse.pixmapSel); n++; 
		XtSetValues (w, color, n);
                
                if (style.useMultiColorIcons)
                {
                    n=0;
                    XtSetArg(color[n], XmNbackground, style.secSelectColor); n++; 
                    XtSetValues (w, color, n);
                    XtSetValues (mouse.pictRC, color, n);
                    XtSetValues (mouse.pictLabel, color, n);
                }

		selectstate = True;
	    }
        }
    }
}





/*+++++++++++++++++++++++++++++++++++++++**/
/* warnToggleCB - put up a warning dialog */
/* about changing the handedness.         */
/*+++++++++++++++++++++++++++++++++++++++**/

static void 
warnToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  
  register int     n;
  Arg              args[10];
  XmString         string;
  Boolean          set;


  if (w == mouse.leftToggle && mouse.handed == LEFT_HANDED) 
    {
      XmToggleButtonGadgetSetState(mouse.leftToggle, True, False);
      return;
    }
  if (w == mouse.rightToggle && mouse.handed == RIGHT_HANDED) 
    {
      XmToggleButtonGadgetSetState(mouse.rightToggle, True, False);
      return;
    }
  if (w == mouse.rightToggle)
    {
      XmToggleButtonGadgetSetState(mouse.leftToggle, False, False);
    }
  else
    {
      XmToggleButtonGadgetSetState(mouse.rightToggle, False, False);
    }
  
  n = 0;
  XtSetArg(args[n], XmNokLabelString, CMPSTR((String) _DtOkString)); n++;
  XtSetArg(args[n], XmNcancelLabelString, CMPSTR((String) _DtCancelString)); n++;
  string = CMPSTR(WARN_MSG);
  XtSetArg(args[n], XmNmessageString, string); n++;
  XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC ); n++;
  XtSetArg(args[n], XmNborderWidth, 3); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL); n++;
  XtSetArg(args[n], XmNdialogTitle, CMPSTR((char *)GETMESSAGE(2,2, "Notice"))); n++;
  warnDialog = XmCreateInformationDialog(style.shell, "WarnDialog", args, n);
  XmStringFree(string);
  XtUnmanageChild (XmMessageBoxGetChild(warnDialog, XmDIALOG_HELP_BUTTON)); 
  XtAddCallback(warnDialog, XmNokCallback, reverseToggleCB, client_data);
  XtAddCallback(warnDialog, XmNcancelCallback, cancelWarnCB, client_data); 
  XtManageChild(warnDialog);       
} 


/*++++++++++++++++++++++++++++++++++++++++++**/
/* midwarnToggleCB - put up a warning dialog */
/* about changing the function of the middle */
/* button.                                   */
/*++++++++++++++++++++++++++++++++++++++++++**/

static void 
midwarnToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  
  register int     n;
  Arg              args[10];
  XmString         warnstring;
  Boolean          set;
  
  if (w == mouse.adjustToggle && mouse.function == ADJUST)
    {
      XmToggleButtonGadgetSetState(mouse.adjustToggle, True, False);
      return;
    }
  if (w == mouse.transferToggle && mouse.function == TRANSFER)
    {
      XmToggleButtonGadgetSetState(mouse.transferToggle, True, False);
      return;
    }

  n = 0;
  XtSetArg(args[n], XmNokLabelString, CMPSTR((String) _DtOkString)); n++;
  XtSetArg(args[n], XmNcancelLabelString, CMPSTR((String) _DtCancelString)); n++;
  XtSetArg(args[n], XmNborderWidth, 3); n++;
  XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC ); n++;
  XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++; 
  XtSetArg(args[n], XmNdialogTitle, CMPSTR((char *)GETMESSAGE(2,2, "Notice"))); n++;
  midwarnDialog = XmCreateInformationDialog(style.shell, "MidWarnDialog", args, n);
  
  XtUnmanageChild (XmMessageBoxGetChild(midwarnDialog, XmDIALOG_HELP_BUTTON)); 
  
  
  if ((intptr_t)client_data == TRANSFER)
    {
      warnstring = CMPSTR(MID_WARN_MSG_T);
      XtVaSetValues(midwarnDialog, XmNmessageString, warnstring, NULL); n++; 
      
      XmToggleButtonGadgetSetState(mouse.adjustToggle, False, False);
    }
  else
    {
      warnstring = CMPSTR(MID_WARN_MSG_A);
      XtVaSetValues(midwarnDialog, XmNmessageString, warnstring, NULL); n++; 
      
      XmToggleButtonGadgetSetState(mouse.transferToggle, False, False);
    }
  XmStringFree(warnstring);

  
  XtAddCallback(midwarnDialog, XmNokCallback, midreverseToggleCB, 
		client_data); 
  XtAddCallback(midwarnDialog, XmNcancelCallback, midcancelWarnCB, 
		client_data); 
  XtManageChild(midwarnDialog);       
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* cancelWarnCB - callback for the      */
/* cancel button of the warnDialog      */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
cancelWarnCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

  register int     n;
  Arg              args[10];
  XmString         string;
  
  if (mouse.handed == RIGHT_HANDED)
    {
      XmToggleButtonGadgetSetState(mouse.rightToggle, True, False);
      XmToggleButtonGadgetSetState(mouse.leftToggle, False, False);
    }
  else
    {
      XmToggleButtonGadgetSetState(mouse.leftToggle, True, False);
      XmToggleButtonGadgetSetState(mouse.rightToggle, False, False);
    }
  XtDestroyWidget(warnDialog);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* midcancelWarnCB - callback for the    */
/* cancel button of the midwarnDialog    */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
midcancelWarnCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

  register int     n;
  Arg              args[10];
  XmString         string;
  
  if (mouse.function == TRANSFER)
    {
      XmToggleButtonGadgetSetState(mouse.transferToggle, True, True);
      XmToggleButtonGadgetSetState(mouse.adjustToggle, False, False);
    }
  else
    {
      XmToggleButtonGadgetSetState(mouse.adjustToggle, True, True);
      XmToggleButtonGadgetSetState(mouse.transferToggle, False, False);
    }
  XtDestroyWidget(midwarnDialog);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* reverseToggleCB                       */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
reverseToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  register int     n;
  int              i, j;
  Arg              args[MAX_ARGS];
  Boolean          set;
  Boolean          toggle;
  
  toggle = (Boolean) (intptr_t) client_data;
  
  if(mouse.type == POINTER3B)
    {
	if (toggle == LEFT_HANDED)
	  {
	    if (mouse.handed == LEFT_HANDED)
	      return;
	    mouse.handed = LEFT_HANDED;
	    mouse.pixmap = mouse.b3_rev;
	    mouse.pixmapSel = mouse.b3_revSel;
	    for(i = 0, j = 3; i < 3; i++, j--)
	      mouse.new_map_return[i] = j;
	    mouse.new_map_return[3] = 5;
	    mouse.new_map_return[4] = 4;
	  }
	else
	  {
	    if (mouse.handed == RIGHT_HANDED)
	      return;
	    mouse.handed = RIGHT_HANDED;
	    mouse.pixmap = mouse.b3;
	    mouse.pixmapSel = mouse.b3Sel;
	    for(i = 0, j = 1; i < 3; i++, j++)
	      mouse.new_map_return[i] = j;
	    mouse.new_map_return[3] = 4;
	    mouse.new_map_return[4] = 5;
	  }
      }
  else     /* 2B mouse */
    {
           if (toggle == LEFT_HANDED)
	     {
               if (mouse.handed == LEFT_HANDED)
		 return;
               mouse.handed = LEFT_HANDED;
               mouse.pixmap = mouse.b2_rev;
               mouse.pixmapSel = mouse.b2_revSel;
               for(i = 0, j = 2; i < 2; i++, j--)
		 mouse.new_map_return[i] = j;
	     }
           else
           {
	     if (mouse.handed == RIGHT_HANDED)
	       return;
	     mouse.handed = RIGHT_HANDED;
	     mouse.pixmap = mouse.b2;
               mouse.pixmapSel = mouse.b2Sel;
               for(i = 0, j = 1; i < 2; i++, j++)
		 mouse.new_map_return[i] = j;
           }
	 }
  
  n=0;
  XtSetArg(args[n], XmNlabelPixmap, mouse.pixmap); n++; 
        XtSetValues(mouse.pictButton, args, n);
  
  /* change Pointer Mapping */
  if (XSetPointerMapping(style.display, mouse.new_map_return, mouse.numButtons) 
                   != MappingSuccess)
    _DtSimpleError (progName, DtWarning, NULL, MSG3, NULL);
  
  mouse.systemDefaultFlag = False;
  XtDestroyWidget(warnDialog);
    
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* midreverseToggleCB                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
midreverseToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n;
    int              i, j;
    Arg              args[MAX_ARGS];
    Boolean          set;
    Boolean          toggle;

     toggle = (Boolean) (intptr_t) client_data;

    if (toggle == ADJUST)
      {
	if (mouse.function == ADJUST)
	  return;
	mouse.function = ADJUST;
      }
    else
      {
	if (mouse.function == TRANSFER)
	  return;
	mouse.function = TRANSFER;
      }
    mouse.btn2Changed = True;
    mouse.systemDefaultFlag = False;
    
    XtDestroyWidget(midwarnDialog);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* systemDefaultCB                       */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
systemDefaultCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  int           n, i, j;
  Arg           args[MAX_ARGS];
  Bool          do_accel, do_thresh;
  int           new_acceln, new_acceld, new_thresh;
  char          dclickRes[40];
  
  /* set default mouse values */
  new_acceld = -1;  
  new_acceln = -1;
  new_thresh = THRESHDEFAULT;
  do_accel = 1;
  do_thresh = 1;
  XChangePointerControl(style.display, do_accel, do_thresh, new_acceln,
			new_acceld, new_thresh);
  
  /*get default mouse values for sliders*/
  XGetPointerControl(style.display, &new_acceln, &new_acceld, &new_thresh);
  
  n=0;
  XtSetArg(args[n], XmNvalue,new_acceln); n++;
  XtSetValues(mouse.accelScale, args, n);
  
  n=0;
  XtSetArg(args[n], XmNvalue,new_thresh); n++;
  XtSetValues(mouse.threshScale, args, n);
  
  n=0;
  XtSetArg(args[n], XmNvalue,DCLICKDEFAULT/100); n++;
  XtSetValues(mouse.dclickScale, args, n);
  
  sprintf(dclickRes, multiClickTimeString, DCLICKDEFAULT);
  _DtAddToResource(style.display,dclickRes);
  
  /* change mouse mapping to default */
  if(mouse.type == POINTER3B)
    {
      mouse.pixmap = mouse.b3;
      mouse.pixmapSel = mouse.b3Sel;
      for(i = 0, j = 1; i < 3; i++, j++)
	mouse.new_map_return[i] = j;
      mouse.new_map_return[3] = 4;
      mouse.new_map_return[4] = 5;
    }
  else
    {
      mouse.pixmap = mouse.b2;
      mouse.pixmapSel = mouse.b2Sel;
      for(i = 0, j = 1; i < 2; i++, j++)
	mouse.new_map_return[i] = j;
    }
  
  if (mouse.handed != RIGHT_HANDED)
    {
      /* change Pointer Mapping */
      if(XSetPointerMapping(style.display, mouse.new_map_return,
			    mouse.numButtons) != MappingSuccess)
	_DtSimpleError (progName, DtWarning, NULL, MSG3, NULL);
      
      n=0;
      XtSetArg(args[n], XmNlabelPixmap, mouse.pixmap); n++;
      XtSetValues(mouse.pictButton, args, n);
      
      
      mouse.handed = RIGHT_HANDED;
      XmToggleButtonGadgetSetState(mouse.rightToggle, True, True);
      XmToggleButtonGadgetSetState(mouse.leftToggle, False, False);
      
    }
  
  if (mouse.function != TRANSFER)
    {
      mouse.function = TRANSFER;
      mouse.btn2Changed = True;
      XmToggleButtonGadgetSetState(mouse.transferToggle, True, True);
      XmToggleButtonGadgetSetState(mouse.adjustToggle, False, False);
    }
  
  mouse.systemDefaultFlag = True;
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* ButtonCB                              */
/* callback for PushButtons in DialogBox */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
ButtonCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int           n, i, j;
    Arg           args[MAX_ARGS];
    DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;
    Bool          do_accel, do_thresh;
    Bool          set;
    char          message1[6], message2[50], message3[6], message4[6];
    char          pointerStr[70];
    static char   dclickRes[40];
    static char   enableBtn1Res[40];
    int           dclick;
    int     new_acceln, new_acceld, new_thresh;

    switch (cb->button_position)
    {
      case OK_BUTTON:

         XtUnmanageChild(w);

         /* if sendSettings is true send data to Session Manager */

         if(style.smState.smSendSettings)
         {   
             if (mouse.systemDefaultFlag && !mouse.dclickChanged)
             {
                SmDefaultPointerSettings();                       
             }
             else /* dclick or one of the sliders changed */
             {
                sprintf(message1,"%d", mouse.numButtons );
                if(mouse.numButtons == 3 || mouse.numButtons == 5)
                   sprintf(message2,"%d_%d_%d", mouse.new_map_return[0],
                                                mouse.new_map_return[1], 
						mouse.new_map_return[2]);
                else
                   sprintf(message2,"%d_%d_%d_%d_%d", mouse.new_map_return[0],
                                                      mouse.new_map_return[1], 
						      mouse.new_map_return[2],
                                                      mouse.new_map_return[3], 
						      mouse.new_map_return[4]);
                sprintf(message3,"%d", getValue(mouse.accelScale));
                sprintf(message4,"%d", getValue(mouse.threshScale));

                sprintf(dclickRes, multiClickTimeString,
                                   getValue(mouse.dclickScale)*100);
		
                if (mouse.dclickChanged)
                {
		  if (style.xrdb.writeXrdbImmediate) {
		    InfoDialog(IMMEDIATE_MSG, style.shell, False);
		   _DtAddToResource(style.display,dclickRes);
	          }
		  else
		    InfoDialog(LATER_MSG, style.shell, False);
		}
		
                sprintf (pointerStr,"%s %s %s %s %s",
			 message1, message2, message3, message4, 
			 dclickRes );
		
                SmNewPointerSettings (pointerStr);
		
	      }
	   }
	 else 
	   {  /* sendSettings is False. If double click has changed update Xrdb */
	     
             if (mouse.dclickChanged) 
	       {
		 InfoDialog(IMMEDIATE_MSG, style.shell, False);
		 sprintf(dclickRes, multiClickTimeString,
			 getValue(mouse.dclickScale)*100);
		 _DtAddToResource(style.display,dclickRes);
	       }
	   }
	 mouse.dclickChanged = False;

	 if (mouse.btn2Changed)
	   if(mouse.function == TRANSFER)
	     sprintf(enableBtn1Res, enableBtn1TransferString, "button2_transfer");
	   else
	     sprintf(enableBtn1Res, enableBtn1TransferString, "True");
	   _DtAddToResource(style.display,enableBtn1Res);

	 break;
	 
       case CANCEL_BUTTON:
	 
	 XtUnmanageChild(w);
	 
	 /* check if values and changePonterControl only if needed */
	 
          new_acceln = getValue(mouse.accelScale);
          if (new_acceln != mouse.accel_numerator)
          {
              /* reset to incoming mouse values */ 
              n=0;
              XtSetArg(args[n], XmNvalue,mouse.accel_numerator); n++;
              XtSetValues(mouse.accelScale, args, n);
          }

          new_thresh = getValue(mouse.threshScale);
          if (new_thresh != mouse.threshold)
          {
              /* reset to incoming mouse values */ 
    	      n=0;
              XtSetArg(args[n], XmNvalue,mouse.threshold); n++;
	      XtSetValues(mouse.threshScale, args, n);
          }

	  n=0;
	  XtSetArg(args[n], XmNvalue,mouse.mydclick); n++;
	  XtSetValues(mouse.dclickScale, args, n);
	  mouse.dclickChanged = False;
          XtSetMultiClickTime(style.display, mouse.mydclick*100);

          if ((new_thresh != mouse.threshold) ||
              (new_acceln != mouse.accel_numerator))
          {
              do_accel = mouse.accel_numerator ? 1:0;
              do_thresh = mouse.threshold ? 1:0;
              XChangePointerControl(style.display, do_accel, do_thresh, 
                   mouse.accel_numerator, mouse.accel_denominator, 
                   mouse.threshold);
          }

	 
	 if (mouse.handed != mouse.origHanded)
	   {
	     if (XSetPointerMapping(style.display, mouse.map_return, 
				    mouse.numButtons) != MappingSuccess)
	       _DtSimpleError (progName, DtWarning, NULL, MSG3, NULL);
	     
	     /* load the correct mouse pixmaps */
	     
	     if(mouse.type == POINTER3B)
	       {
		 if (mouse.origHanded == LEFT_HANDED)
		   {
		     mouse.pixmap = mouse.b3_rev;
		     mouse.pixmapSel = mouse.b3_revSel;
		   }
		 else
		   {
		     if (mouse.origHanded == RIGHT_HANDED)
		       {
			 mouse.pixmap = mouse.b3;
			 mouse.pixmapSel = mouse.b3Sel;
		       }
		   }
	       }
	     else     /* 2B mouse */
	       {
		 if (mouse.origHanded == LEFT_HANDED)
		   {
		     mouse.pixmap = mouse.b2_rev;
		     mouse.pixmapSel = mouse.b2_revSel;
		   }
		 else
		   {
		     if (mouse.origHanded == RIGHT_HANDED)
		       {
			 mouse.pixmap = mouse.b2;
			 mouse.pixmapSel = mouse.b2Sel;
		       }
		   }
	       }
	     
	     n=0;
	     XtSetArg(args[n], XmNlabelPixmap, mouse.pixmap); n++; 
	     XtSetValues(mouse.pictButton, args, n);
	   }
	 
	 if (mouse.function != mouse.origFunction)
	   {
	     mouse.function = mouse.origFunction;
	     if (mouse.origFunction == TRANSFER)
	       {
		 XmToggleButtonGadgetSetState(mouse.transferToggle, True, True);
		 XmToggleButtonGadgetSetState(mouse.adjustToggle, False, False);
	       }
	     else
	       {
		 XmToggleButtonGadgetSetState(mouse.transferToggle, False, False);
		 XmToggleButtonGadgetSetState(mouse.adjustToggle, True, True);
	       }
	   }
	 break;
	 
       case HELP_BUTTON:
	 XtCallCallbacks(style.mouseDialog, XmNhelpCallback, (XtPointer)NULL);
	 break;
	 
       default:
	 break;
       }
  }


/************************************************************************
 * restoreMouse()
 *
 * restore any state information saved with saveMouse.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
************************************************************************/
void 
restoreMouse(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("Mouse");
    xrm_name [2] = 0;

    /* get x position */
    xrm_name [1] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
        XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr)); save.poscnt++;
        save.restoreFlag = True;
    }

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr)); save.poscnt++;
    }

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0) 
      popup_mouseBB(shell);
}

/************************************************************************
 * saveMouse()
 *
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with the
 * file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreBackdrop.  The suggested minimum is whether you are mapped, and your
 * location.
 ************************************************************************/
void 
saveMouse(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.mouseDialog != NULL) 
    {
        if (XtIsManaged(style.mouseDialog))
            sprintf(bufr, "*Mouse.ismapped: True\n");
        else
            sprintf(bufr, "*Mouse.ismapped: False\n");

	/* Get and write out the geometry info for our Window */
	x = XtX(XtParent(style.mouseDialog));
	y = XtY(XtParent(style.mouseDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	sprintf(bufr, "%s*Mouse.x: %d\n", bufr, x);
	sprintf(bufr, "%s*Mouse.y: %d\n", bufr, y);

	if(-1 == write (fd, bufr, strlen(bufr))) {
		perror(strerror(errno));
	}
    }
}
    
