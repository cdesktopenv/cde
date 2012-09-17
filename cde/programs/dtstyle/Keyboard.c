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
 **   File:        Keyboard.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the DtStyle keyboard dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $XConsortium: Keyboard.c /main/4 1995/10/30 13:10:18 rswiston $ */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/VendorSEP.h>

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>

#include <Dt/Message.h>
#include <Dt/HourGlass.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"
#include "Protocol.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Keyboard.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define VOLUME_MAX       100
#define SCALE_WIDTH      200

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/


static Widget build_keyboardDlg( Widget shell) ;
static void layoutCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void _DtmapCB_keyboardDlg( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static int getValue( Widget w) ;
static void valueChangedCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void autoRepeatToggleCB( 
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
    Widget          pictLabel;
    Widget          systemDefault;
    Widget          volumeLabGad;
    Widget          volumeScale;
    Widget          autoRepeatToggle;
    XKeyboardState  values;
    int             new_key_click_percent;
    int             new_autoRepeat;
    Boolean         systemDefaultFlag;
} Kbd, *KdbPtr;

static Kbd kbd;
static saveRestore save = {FALSE, 0, };

/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_kbdDialog                       */
/*+++++++++++++++++++++++++++++++++++++++*/
void 
popup_keyboardBB(
        Widget shell )
{
    if (style.kbdDialog == NULL) 
    {
      _DtTurnOnHourGlass(shell);  
      build_keyboardDlg(shell);
      XtManageChild(style.kbdDialog);
      _DtTurnOffHourGlass(shell);  
    }
    else 
    {
      XtManageChild(style.kbdDialog);
      raiseWindow(XtWindow(XtParent(style.kbdDialog)));
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* build__keyboardDlg                    */
/*+++++++++++++++++++++++++++++++++++++++*/
static Widget 
build_keyboardDlg(
        Widget shell )
{
    register int     i, n;
    Arg              args[MAX_ARGS];
    Widget           widget_list[12]; 
    int              count = 0;
    Widget           form;
    Pixel            foreground, background;
    Boolean          set;
    XmString         button_string[NUM_LABELS]; 
    XmString         string;

    /* Set up DialogBoxDialog button labels */
    button_string[0] = CMPSTR(_DtOkString);
    button_string[1] = CMPSTR(_DtCancelString);
    button_string[2] = CMPSTR(_DtHelpString);

    /* Create toplevel DialogBox */

    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.  
     * save.posArgs may contain information from restoreBeep().*/

    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  
    save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);  
    save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNdefaultPosition, False); 
    save.poscnt++;
    style.kbdDialog = __DtCreateDialogBoxDialog(shell, "KeyboardDialog", save.posArgs, save.poscnt);
    XtAddCallback(style.kbdDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(style.kbdDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_KEYBOARD_DIALOG);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widget_list[0] = _DtDialogBoxGetButton(style.kbdDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widget_list[0]); n++;
    XtSetValues (style.kbdDialog, args, n);

    n=0;
    XtSetArg(args[n], XmNtitle, ((char *)GETMESSAGE(13, 6, "Style Manager - Keyboard"))); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions,  DIALOG_MWM_FUNC ); n++;
    XtSetValues (XtParent(style.kbdDialog), args, n);

    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    form = XmCreateForm(style.kbdDialog, "keyboardForm", args, n);

    /* create keyboard pixmap */

    n = 0;
    XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
    XtSetArg(args[n], XmNbehavior, XmICON_LABEL); n++;
    XtSetArg(args[n], XmNpixmapForeground, style.secBSCol); n++;
    XtSetArg(args[n], XmNpixmapBackground, style.secTSCol); n++;
    XtSetArg(args[n], XmNstring, NULL); n++;  
    XtSetArg(args[n], XmNshadowThickness, 0); n++;  
    XtSetArg(args[n], XmNimageName, KEYBOARD_ICON); n++;  
    XtSetArg(args[n], XmNtraversalOn, False); n++;  
    widget_list[count++] = 
    kbd.pictLabel= _DtCreateIcon(form, "keyboardpictLabel", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
    string = CMPSTR(((char *)GETMESSAGE(13, 2, "Default")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    widget_list[count++] = 
    kbd.systemDefault= XmCreatePushButtonGadget(form, "systemDefault", args, n);
    XmStringFree(string);
    
    n = 0;
    set = (kbd.values.global_auto_repeat == AutoRepeatModeOn) ? True : False;
    XtSetArg(args[n], XmNset, set);  n++;
    string = CMPSTR(((char *)GETMESSAGE(13, 3, "Auto Repeat")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    widget_list[count++] = 
    kbd.autoRepeatToggle= XmCreateToggleButtonGadget(form, "autoRepeatToggle", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    string = CMPSTR(((char *)GETMESSAGE(13, 4, "Click Volume")));
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    kbd.volumeLabGad= XmCreateLabelGadget(form,"volumeLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNmaximum, VOLUME_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    XtSetArg(args[n], XmNscaleWidth, SCALE_WIDTH); n++; 
    widget_list[count++] = 
    kbd.volumeScale= XmCreateScale(form,"volumeScale", args, n);

    XtAddCallback(style.kbdDialog, XmNmapCallback, layoutCB, NULL);
    XtAddCallback(style.kbdDialog, XmNmapCallback, _DtmapCB_keyboardDlg, shell);
    XtAddCallback(kbd.systemDefault, XmNactivateCallback, systemDefaultCB, NULL);
    XtAddCallback(kbd.volumeScale, XmNvalueChangedCallback, valueChangedCB, NULL);
    XtAddCallback(kbd.autoRepeatToggle, XmNvalueChangedCallback, autoRepeatToggleCB, NULL);

    XtManageChild(form);
    XtManageChildren(widget_list,count); 

    return(style.kbdDialog);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* layoutCB                              */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
layoutCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int              n;
    Arg              args[MAX_ARGS];

    /* Picture Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (kbd.pictLabel, args, n);

    /* system Default */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (kbd.systemDefault, args, n);

    /* auto repeat toggle */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          kbd.pictLabel);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (kbd.autoRepeatToggle, args, n);

    /* Volume Label */
    n=0;

    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (kbd.volumeLabGad, args, n);


    /* Volume Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          kbd.autoRepeatToggle);n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);  n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget,         kbd.volumeLabGad);    n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetValues (kbd.volumeScale, args, n);

    XtRemoveCallback(style.kbdDialog, XmNmapCallback, layoutCB, NULL);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtmapCB_keyboardDlg                     */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
_DtmapCB_keyboardDlg(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    static int  first_time = 1;
    int         n;
    Arg         args[MAX_ARGS];
    Boolean     set;

    if (first_time)
    {
        DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

        if (!save.restoreFlag)
	    putDialog ((Widget)client_data, w);

        first_time = 0;
    }

    /* get keyboard values for click volume */
    XGetKeyboardControl(style.display, &kbd.values);
    kbd.new_key_click_percent = kbd.values.key_click_percent;
    kbd.new_autoRepeat = kbd.values.global_auto_repeat;

    n=0;
    XtSetArg(args[n], XmNvalue, kbd.values.key_click_percent); n++;
    XtSetValues(kbd.volumeScale, args, n);

    n=0;
    set = (kbd.values.global_auto_repeat == AutoRepeatModeOn) ? True : False;
    XtSetArg(args[n], XmNset, set);  n++;
    XtSetValues(kbd.autoRepeatToggle, args, n);
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

    XKeyboardControl    kbdControl;
    int                 kbdControlMask;

    /* set keyboard click volume to scale value */

    kbdControlMask = KBKeyClickPercent;
    kbdControl.key_click_percent = kbd.new_key_click_percent =
                  getValue(kbd.volumeScale);
    XChangeKeyboardControl(style.display, kbdControlMask, &kbdControl);
    kbd.systemDefaultFlag = False;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* autoRepeatToggleCB                    */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
autoRepeatToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    /* set auto repeat to new value */

    if (((XmToggleButtonCallbackStruct *)call_data)->set)
    {
        XAutoRepeatOn(style.display);
        kbd.new_autoRepeat = AutoRepeatModeOn;
    }
    else
    {
        XAutoRepeatOff(style.display);
        kbd.new_autoRepeat = AutoRepeatModeOff;
    }
    kbd.systemDefaultFlag = False;
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
    int                 n;
    Arg                 args[MAX_ARGS];
    XKeyboardControl    kbdControl;
    int                 kbdControlMask;
    XKeyboardState      kbdState;
    Boolean             set;

    /* set keyboard click volume to system default value */

    kbdControlMask = KBKeyClickPercent | KBAutoRepeatMode;
    kbdControl.key_click_percent = -1;
    kbdControl.auto_repeat_mode = AutoRepeatModeDefault;
    XChangeKeyboardControl(style.display, kbdControlMask, &kbdControl);

    XGetKeyboardControl(style.display, &kbdState);

    n=0;
    XtSetArg(args[n], XmNvalue, kbdState.key_click_percent); n++;
    XtSetValues(kbd.volumeScale, args, n);

    n=0;
    set = (kbdState.global_auto_repeat == AutoRepeatModeOn) ? True : False;
    XtSetArg(args[n], XmNset, set);  n++;
    XtSetValues(kbd.autoRepeatToggle, args, n);

    kbd.systemDefaultFlag = True;
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
    int                 n;
    Arg                 args[MAX_ARGS];
    XKeyboardControl    kbdControl;
    int                 kbdControlMask;
    DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;


    switch (cb->button_position)
    {
      case OK_BUTTON:
          XtUnmanageChild (w);

          /* if sendSettings is true send message to Session Manager */

          if(style.smState.smSendSettings)
          {
             if(kbd.systemDefaultFlag)
             {
                SmDefaultKeyboardSettings();
             }
             else
             {
                SmNewKeyboardSettings( kbd.new_key_click_percent, /* 0-100 */
                                       kbd.new_autoRepeat);       /* 0,1   */
             }
          }
          break;

      case CANCEL_BUTTON:
	  /* reset to incoming keyboard values */ 

          n=0;
	  XtSetArg(args[n], XmNvalue, kbd.values.key_click_percent); n++;
	  XtSetValues(kbd.volumeScale, args, n);

          n=0;
	  XtSetArg(args[n], XmNset, kbd.values.global_auto_repeat); n++;
	  XtSetValues(kbd.autoRepeatToggle, args, n);

          kbdControlMask = KBKeyClickPercent | KBAutoRepeatMode;
          kbdControl.key_click_percent = kbd.values.key_click_percent;
          kbdControl.auto_repeat_mode = kbd.values.global_auto_repeat;
          XChangeKeyboardControl(style.display, kbdControlMask, &kbdControl);

          XtUnmanageChild(w);
          break;

      case HELP_BUTTON:
          XtCallCallbacks(style.kbdDialog, XmNhelpCallback, (XtPointer)NULL);
	  break;

      default:
	  break;
    }
}


/************************************************************************
 * restoreKeybd()
 *
 * restore any state information saved with saveKeybd.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreKeybd(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("keyboardDlg");
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
      popup_keyboardBB(shell);
}

/************************************************************************
 * saveKeybd()
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
saveKeybd(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.kbdDialog != NULL) 
    {
        if (XtIsManaged(style.kbdDialog))
            sprintf(bufr, "*keyboardDlg.ismapped: True\n");
        else
            sprintf(bufr, "*keyboardDlg.ismapped: False\n");

        /* Get and write out the geometry info for our Window */
        x = XtX(XtParent(style.kbdDialog));
        y = XtY(XtParent(style.kbdDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

        sprintf(bufr, "%s*keyboardDlg.x: %d\n", bufr, x);
        sprintf(bufr, "%s*keyboardDlg.y: %d\n", bufr, y);

        write (fd, bufr, strlen(bufr));
    }
}

