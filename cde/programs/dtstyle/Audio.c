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
 **   File:        Audio.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle Audio dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $XConsortium: Audio.c /main/4 1995/10/30 13:07:24 rswiston $ */
/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#include <errno.h>
#include <Xm/MwmUtil.h>

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/Scale.h>
#include <Xm/VendorSEP.h>
#include <Xm/LabelG.h>

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
#include "Audio.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define VOLUME_MAX          100
#define SCALE_WIDTH         200

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static Widget build_audioDlg( Widget shell) ;
static void formLayoutCB    ( Widget, XtPointer, XtPointer ) ;
static void _DtmapCB_audioDlg  ( Widget, XtPointer, XtPointer ) ;
static int getValue         ( Widget w) ;
static void valueChangedCB  ( Widget, XtPointer, XtPointer ) ;
static void systemDefaultCB ( Widget, XtPointer, XtPointer ) ;
static void ButtonCB        ( Widget, XtPointer, XtPointer ) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget      pictLabel;
    Widget      systemDefault;
    Widget      volLabGad;
    Widget      toneLabGad;
    Widget      durLabGad;
    Widget	volScale;
    Widget	toneScale;
    Widget	durScale;
    Boolean     systemDefaultFlag;
    XKeyboardState  values;
} Audio, *AudioPtr;

static Audio audio;
static saveRestore save = {FALSE, 0, };

/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_audioBB                         */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
popup_audioBB(
        Widget shell )
{
    if (style.audioDialog == NULL) {
      _DtTurnOnHourGlass(shell);  
      build_audioDlg(shell);
      XtManageChild(style.audioDialog);
      _DtTurnOffHourGlass(shell);  
    }
    else 
    {
        XtManageChild(style.audioDialog);
        raiseWindow(XtWindow(XtParent(style.audioDialog)));
    }
    audio.systemDefaultFlag = False;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* build__audioDlg                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static Widget 
build_audioDlg(
        Widget shell )
{
    register int     i, n;
    Arg              args[MAX_ARGS];
    XmString         button_string[NUM_LABELS]; 
    XmString         string; 
    Widget           form;
    Pixmap           audioPixmap;
    int              count = 0;
    Widget           widget_list[12];

    /*get audio values for sliders*/
    XGetKeyboardControl(style.display, &audio.values);
    if(audio.values.bell_duration == 0)
	audio.values.bell_duration = 1;

    /* Set up DialogBoxDialog button labels */
    button_string[0] = CMPSTR((String) _DtOkString);
    button_string[1] = CMPSTR((String) _DtCancelString);
    button_string[2] = CMPSTR((String) _DtHelpString);

    /* Create toplevel DialogBox */
    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.  
     * save.posArgs may contain information from restoreBeep().*/

    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);  save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNdefaultPosition, False);
    save.poscnt++;
    style.audioDialog = 
        __DtCreateDialogBoxDialog(shell, "AudioDialog", save.posArgs, save.poscnt);
    XtAddCallback(style.audioDialog, XmNmapCallback, formLayoutCB, NULL);
    XtAddCallback(style.audioDialog, XmNmapCallback, _DtmapCB_audioDlg, shell);
    XtAddCallback(style.audioDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(style.audioDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_AUDIO_DIALOG);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widget_list[0] = _DtDialogBoxGetButton(style.audioDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widget_list[0]); n++;
    XtSetValues (style.audioDialog, args, n);

    n=0;
    XtSetArg(args[n], XmNtitle, ((char *)GETMESSAGE(6, 7, "Style Manager - Beep"))); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetValues (XtParent(style.audioDialog), args, n);

    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    form = XmCreateForm(style.audioDialog, "audioForm", args, n);

    n = 0;
    XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
    XtSetArg(args[n], XmNbehavior, XmICON_LABEL); n++;
    XtSetArg(args[n], XmNpixmapForeground, style.secBSCol); n++;
    XtSetArg(args[n], XmNpixmapBackground, style.secTSCol); n++;
    XtSetArg(args[n], XmNstring, NULL); n++;  
    XtSetArg(args[n], XmNshadowThickness, 0); n++;  
    XtSetArg(args[n], XmNimageName, BEEP_ICON); n++;  
    XtSetArg(args[n], XmNtraversalOn, False); n++;  
    widget_list[count++] = 
    audio.pictLabel = _DtCreateIcon(form, "audiopictLabel", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
    string = CMPSTR(((char *)GETMESSAGE(6, 2, "Default")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widget_list[count++] = 
    audio.systemDefault = XmCreatePushButtonGadget(form, "systemDefault", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    string = CMPSTR(((char *)GETMESSAGE(6, 3, "Volume")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    widget_list[count++] = 
    audio.volLabGad= XmCreateLabelGadget(form,"volLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNmaximum, VOLUME_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNvalue,audio.values.bell_percent); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    XtSetArg(args[n], XmNscaleWidth, SCALE_WIDTH); n++; 
    widget_list[count++] = 
    audio.volScale= XmCreateScale(form,"volScale", args, n);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    string = CMPSTR(((char *)GETMESSAGE(6, 4, "Tone")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    widget_list[count++] = 
    audio.toneLabGad= XmCreateLabelGadget(form,"toneLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    XtSetArg(args[n], XmNscaleWidth, SCALE_WIDTH); n++; 
    XtSetArg(args[n], XmNvalue,audio.values.bell_pitch); n++;
    widget_list[count++] = 
    audio.toneScale= XmCreateScale(form,"toneScale", args, n);

    n = 0;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
    string = CMPSTR(((char *)GETMESSAGE(6, 5, "Duration")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    widget_list[count++] = 
    audio.durLabGad= XmCreateLabelGadget(form,"durLabGad", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNdecimalPoints, (short)1); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    XtSetArg(args[n], XmNvalue,(int)audio.values.bell_duration/100); n++;
    XtSetArg(args[n], XmNscaleWidth, SCALE_WIDTH); n++; 
    widget_list[count++] = 
    audio.durScale= XmCreateScale(form,"durationScale", args, n);

    XtAddCallback(audio.systemDefault, XmNactivateCallback, systemDefaultCB, NULL);
    XtAddCallback(audio.volScale, XmNvalueChangedCallback, valueChangedCB, NULL);
    XtAddCallback(audio.toneScale, XmNvalueChangedCallback, valueChangedCB, NULL);
    XtAddCallback(audio.durScale, XmNvalueChangedCallback, valueChangedCB, NULL);

    XtManageChild(form);
    XtManageChildren(widget_list,count); 

    return(style.audioDialog);
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
    Dimension        ScaleHeight;
    Dimension        LabelHeight;
    Dimension        volLabelWidth;
    Dimension        toneLabelWidth;
    Dimension        durLabelWidth;
    Dimension        MaxLabelWidth;
    Dimension        TopOffset;
    Dimension        LeftOffset;


    /* get component sizes */
   
    ScaleHeight = XtHeight(audio.volScale);
    LabelHeight = XtHeight(audio.volLabGad);

    MaxLabelWidth = volLabelWidth = XtWidth(audio.volLabGad);

    toneLabelWidth = XtWidth(audio.toneLabGad);
    if (toneLabelWidth > MaxLabelWidth) 
    {
	MaxLabelWidth = toneLabelWidth;
    }

    durLabelWidth = XtWidth(audio.durLabGad);
    if (durLabelWidth > MaxLabelWidth) 
    {
	MaxLabelWidth = durLabelWidth;
    }
	
    TopOffset = style.verticalSpacing + ScaleHeight - LabelHeight;

    /* do form attachments */

    /* Picture Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (audio.pictLabel, args, n);

    /* system Default */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (audio.systemDefault, args, n);

    /* Volume Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          audio.pictLabel);     n++;
    XtSetArg(args[n], XmNtopOffset,          TopOffset);           n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    LeftOffset =   style.horizontalSpacing + (MaxLabelWidth - volLabelWidth);
    XtSetArg(args[n], XmNleftOffset,         LeftOffset);          n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE); n++;
    XtSetValues (audio.volLabGad, args, n);


    /* Volume Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       audio.volLabGad);    n++;
    XtSetArg(args[n], XmNbottomOffset,       0);                   n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget,         audio.volLabGad);     n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetValues (audio.volScale, args, n);

    /* Tone Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          audio.volScale);      n++;
    XtSetArg(args[n], XmNtopOffset,          TopOffset);           n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    LeftOffset =   style.horizontalSpacing + (MaxLabelWidth - toneLabelWidth);
    XtSetArg(args[n], XmNleftOffset,         LeftOffset);           n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE); n++;
    XtSetValues (audio.toneLabGad, args, n);


    /* Tone Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       audio.toneLabGad);    n++;
    XtSetArg(args[n], XmNbottomOffset,       0);                   n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget,         audio.toneLabGad);    n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetValues (audio.toneScale, args, n);

    /* Duration Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          audio.toneScale);     n++;
    XtSetArg(args[n], XmNtopOffset,          TopOffset);           n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    LeftOffset =   style.horizontalSpacing + (MaxLabelWidth - durLabelWidth);
    XtSetArg(args[n], XmNleftOffset,         LeftOffset);          n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (audio.durLabGad, args, n);


    /* Duration Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       audio.durLabGad);    n++;
    XtSetArg(args[n], XmNbottomOffset,       0);                   n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget,         audio.durLabGad);     n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetValues (audio.durScale, args, n);

    XtRemoveCallback(style.audioDialog, XmNmapCallback, formLayoutCB, NULL);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtmapCB_audioDlg                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
_DtmapCB_audioDlg(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    static int  first_time = 1;
    int         n;
    Arg         args[MAX_ARGS];


    if (first_time)
    {
        DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

        if (!save.restoreFlag)
            putDialog ((Widget)client_data, w);
   
        first_time = 0;
    }
    else
    {
	/*get audio values for sliders*/
	XGetKeyboardControl(style.display, &audio.values);
	if(audio.values.bell_duration == 0)
	    audio.values.bell_duration = 1;

	n=0;
	XtSetArg(args[n], XmNvalue,audio.values.bell_percent); n++;
	XtSetValues(audio.volScale, args, n);

	n=0;
	XtSetArg(args[n], XmNvalue,audio.values.bell_pitch); n++;
	XtSetValues(audio.toneScale, args, n);

	n=0;
	XtSetArg(args[n], XmNvalue,(int)audio.values.bell_duration/100); n++;
	XtSetValues(audio.durScale, args, n);
    }
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
    int                 n, value;
    Arg                 args[1];
    XKeyboardControl    kbdControl;
    int                 kbdControlMask;

    /*set audio parameters to val returned and ring audio*/

    kbdControlMask = KBBellPercent | KBBellPitch | KBBellDuration;
    kbdControl.bell_percent = getValue(audio.volScale);
    kbdControl.bell_pitch = getValue(audio.toneScale);
    kbdControl.bell_duration = getValue(audio.durScale)*100;
    XChangeKeyboardControl(style.display, kbdControlMask, &kbdControl);
    
    XBell(style.display,0);

    audio.systemDefaultFlag = False;
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
    int                 n, value;
    Arg                 args[1];
    XKeyboardControl    kbdControl;
    int                 kbdControlMask;
    XKeyboardState      kbdState;

    /*set audio parameters to val returned and ring bell*/

    kbdControlMask = KBBellPercent | KBBellPitch | KBBellDuration;
    kbdControl.bell_percent = -1;
    kbdControl.bell_pitch = -1;
    kbdControl.bell_duration = -1;
    XChangeKeyboardControl(style.display, kbdControlMask, &kbdControl);
    
    /*get audio values for sliders*/
    XGetKeyboardControl(style.display, &kbdState);

    n=0;
    XtSetArg(args[n], XmNvalue,kbdState.bell_percent); n++;
    XtSetValues(audio.volScale, args, n);

    n=0;
    XtSetArg(args[n], XmNvalue,kbdState.bell_pitch); n++;
    XtSetValues(audio.toneScale, args, n);

    n=0;
    XtSetArg(args[n], XmNvalue,(int)kbdState.bell_duration/100); n++;
    XtSetValues(audio.durScale, args, n);

    XBell(style.display,0);
 
    audio.systemDefaultFlag = True;
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

         /* if sendSetting is True send message to Session manager */

          if(style.smState.smSendSettings)
          {
             if(audio.systemDefaultFlag)
             {
                SmDefaultAudioSettings();                       
             }
             else
             {
                SmNewAudioSettings(getValue(audio.volScale),
                                   getValue(audio.toneScale),
                                   getValue(audio.durScale) * 100);
             }
          }
          break;

      case CANCEL_BUTTON:
	  /* reset to incoming audio values */ 
          XtUnmanageChild(w);

	  n=0;
          XtSetArg(args[n], XmNvalue,audio.values.bell_percent);
	  XtSetValues(audio.volScale, args, n);

	  n=0;
          XtSetArg(args[n], XmNvalue,audio.values.bell_pitch);
	  XtSetValues(audio.toneScale, args, n);

	  n=0;
          XtSetArg(args[n], XmNvalue,(int)audio.values.bell_duration/100);
	  XtSetValues(audio.durScale, args, n);

          kbdControlMask = KBBellPercent | KBBellPitch | KBBellDuration;
          kbdControl.bell_percent = audio.values.bell_percent;
          kbdControl.bell_pitch = audio.values.bell_pitch;
          kbdControl.bell_duration = audio.values.bell_duration;
          XChangeKeyboardControl(style.display, kbdControlMask, &kbdControl);

	  XBell(style.display,0);
          break;

      case HELP_BUTTON:
          XtCallCallbacks(style.audioDialog, XmNhelpCallback, (XtPointer)NULL);
	  break;

      default:
	  break;
    }
}


/************************************************************************
 * restoreAudio()
 *
 * restore any state information saved with saveAudio.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreAudio(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("audioDlg");
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
      popup_audioBB(shell);
}

/************************************************************************
 * saveAudio()
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
saveAudio(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;    /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.audioDialog != NULL) 
    {
        if (XtIsManaged(style.audioDialog))
            sprintf(bufr, "*audioDlg.ismapped: True\n");
        else
            sprintf(bufr, "*audioDlg.ismapped: False\n");

	/* Get and write out the geometry info for our Window */
	x = XtX(XtParent(style.audioDialog));
	y = XtY(XtParent(style.audioDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	sprintf(bufr, "%s*audioDlg.x: %d\n", bufr, x);
	sprintf(bufr, "%s*audioDlg.y: %d\n", bufr, y);
	if(-1 == write (fd, bufr, strlen(bufr))) {
		perror(strerror(errno));	
	}
    }
}

