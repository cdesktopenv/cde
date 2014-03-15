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
/* $XConsortium: ColorEdit.c /main/5 1995/10/30 13:08:03 rswiston $ */
/************************************<+>*************************************
 ***************************************************************************
 **
 **   File:        ColorEdit.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle Color Editor dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* +++++++++++++++++++++++++++++++++++++++*/
/*  include files                         */
/* +++++++++++++++++++++++++++++++++++++++*/
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include <math.h>

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/XmP.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Xm/VendorSEP.h>

#include <Dt/DialogBox.h>
#include <Dt/TitleBox.h>

#include <Dt/UserMsg.h>

#include "Help.h"
#include "Main.h"
#include "ColorMain.h"
#include "ColorFile.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "ColorEdit.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define  NONE       0
#define  RED        1
#define  GREEN      2
#define  BLUE       3
#define  HUE        4
#define  SATURATION 5
#define  VALUE      6

#define MAX_STR_LEN   128

#define SCALE_LEFT_POSITION        10
#define BUTTON_MARGIN              10
#define OLD_BUTTON_LEFT_POSITION   10
#define SCALE_WIDTH                300

#define SATURATION_BITS 		"Dtsatur"
#define VALUE_BITS			"Dtvalue"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static void CreateColorEditor( Widget parent) ;
static void sliderLayoutCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void InitializeNewButton( void ) ;
static void InitializeOldButton( void ) ;
static void GenerateColors( void ) ;
static Pixel GetPixel( Widget widget,
                       char *color_string ) ;
static void changRGB_CB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void changHSV_CB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void SetScales( XColor *rgb) ;
static void SetRGBHSVScales( 
                        XColor *rgb,
                        int h,
                        int s,
                        int v) ;
static void CopyPixelSet( 
                        ColorSet *color_set_dest,
                        ColorSet *color_set_src) ;
static void HSVtoRGB( 
                        int h,
                        int s,
                        int v,
                        unsigned short *r,
                        unsigned short *g,
                        unsigned short *b) ;
static double max( 
                        double x,
                        double y,
                        double z) ;
static double min( 
                        double x,
                        double y,
                        double z) ;
static void grabcolorCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void dialogBoxCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void _DtmapCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/* Global Variables                      */
/*+++++++++++++++++++++++++++++++++++++++*/
EditData edit;

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/
static saveRestore save = {FALSE, 0, };
static OldNewSame = False;

/************************************************************************
 *  ColorEditor() - Create ColorEditor dialog first time up.
 *  If has already been created, set up oldButton and newButton
 *  colors, set the scale values for the new color and manage 
 *  the dialog.
 ************************************************************************/
void 
ColorEditor(
        Widget parent,
        ColorSet *color_set )
{
    if (edit.DialogShell == NULL)
    {
        _DtTurnOnHourGlass(parent);  
        edit.color_set = color_set;
        CreateColorEditor(parent);   
        _DtTurnOffHourGlass(parent);  
    }
    else 
    {
        if (!XtIsManaged(edit.DialogShell))
	{
            edit.color_set = color_set;
	    CopyPixelSet(&edit.oldButtonColor,edit.color_set);

		/* update "old" button if necessary */
		if(style.visualClass==TrueColor || style.visualClass==DirectColor){
			XtVaSetValues(edit.oldButton,
				XmNbackground,edit.color_set->bg.pixel,
				XmNarmColor,edit.color_set->bg.pixel,
				XmNforeground,edit.color_set->fg.pixel,
				XmNtopShadowColor,edit.color_set->ts.pixel,
				XmNbottomShadowColor,edit.color_set->bs.pixel,NULL);
		}
	    InitializeNewButton();
	    SetScales(&edit.color_set->bg);
	    XtManageChild(edit.DialogShell);
            XMapRaised(style.display, XtWindow(edit.DialogShell));
	}
    }
}


/************************************************************************
 *   CreateColorEditor()
 *           Create the Color Editor Dialog
 ************************************************************************/
static void 
CreateColorEditor(
        Widget parent )
{
	register int     n,i;
	Arg              args[MAX_ARGS];
        Widget           sampleTB;
        Widget           sampleForm;
        Widget		 sliderTB;
	XmString         string;               /* temp Xm string */
        Widget           widgetlist1[10];   /* main_form */
        Widget           widgetlist2[16];   /* sliderForm */
        Widget           widgetlist3[10];   /* sampleForm */
        int              widget_count1 = 0;
        int              widget_count2 = 0;
        int              widget_count3 = 0;
        XmString         button_string[NUM_LABELS]; 
        Pixel            foreground, background;
        int              height;
        WidgetList       children;
        Dimension        w, width_old, width_new;

        edit.current_scale = NONE;

        /* Set up DialogBox button labels. */
        button_string[0] = CMPSTR((String) _DtOkString);
        button_string[1] = CMPSTR((String) _DtCancelString);
        button_string[2] = CMPSTR((String) _DtHelpString);

        /* Note that save.poscnt has been initialized elsewhere.  
	 * save.posArgs may contain information from restoreColorEdit(). */

        XtSetArg(args[save.poscnt], XmNchildType, XmWORK_AREA);  save.poscnt++;
	XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  
	save.poscnt++;
	XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, 
		  button_string);  
	save.poscnt++;
        XtSetArg(save.posArgs[save.poscnt], XmNdefaultPosition, False);
        save.poscnt++;
	edit.DialogShell = __DtCreateDialogBoxDialog(parent,"colorEditDlg", 
						     save.posArgs, save.poscnt);
	XtAddCallback(edit.DialogShell, XmNmapCallback, sliderLayoutCB, NULL);
	XtAddCallback(edit.DialogShell, XmNcallback, dialogBoxCB, NULL);
        XtAddCallback(edit.DialogShell, XmNmapCallback, _DtmapCB, parent);
        XtAddCallback(edit.DialogShell, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_MODIFY_PALETTE_DIALOG);

        XmStringFree(button_string[0]);
        XmStringFree(button_string[1]);
        XmStringFree(button_string[2]);

        widgetlist1[0] = _DtDialogBoxGetButton(edit.DialogShell,2);

        n=0;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        XtSetArg(args[n], XmNcancelButton, widgetlist1[0]); n++;
        XtSetValues (edit.DialogShell, args, n);

        n=0;
	XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(17, 16, "Style Manager - Modify Color"))); n++;
        XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
        XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
        XtSetValues (XtParent(edit.DialogShell), args, n);
  
        n=0;
        XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
	edit.main_form = XmCreateForm(edit.DialogShell, "main_form", args, n);

        n=0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
        XtSetArg (args[n], XmNmarginWidth, 0);  n++;
        XtSetArg (args[n], XmNmarginHeight, 0);  n++;
        string = CMPSTR(((char *)GETMESSAGE(17, 4, "Color Sample")));
	XtSetArg (args[n], XmNtitleString, string); n++;
	sampleTB = _DtCreateTitleBox(edit.main_form, "sampleTB", args, n);
        widgetlist1[widget_count1++] = sampleTB;
        XmStringFree(string);
	
        n=0;
        XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
	sampleForm = XmCreateForm(sampleTB, "sampleForm", args, n);
	
        /* Create Old and New Buttons */
		if(style.visualClass==TrueColor || style.visualClass==DirectColor){
			edit.oldButtonColor.bg.pixel = edit.color_set->bg.pixel;
			edit.oldButtonColor.fg.pixel = edit.color_set->fg.pixel;
			edit.oldButtonColor.sc.pixel = edit.color_set->sc.pixel;
			edit.oldButtonColor.bs.pixel = edit.color_set->bs.pixel;
			edit.oldButtonColor.ts.pixel = edit.color_set->ts.pixel;
			CopyPixelSet(&edit.oldButtonColor,edit.color_set);
		}else{
			InitializeOldButton();
		}

        if(!OldNewSame) {
  	   n=0;
	   string =  CMPSTR(((char *)GETMESSAGE(17, 5, "Old")));
	   XtSetArg(args[n], XmNlabelString, string); n++;
	   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
	   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
	   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
           /* Set the old button color to the color of the selected button */
           XtSetArg(args[n], XmNforeground, edit.oldButtonColor.fg.pixel); n++;
           XtSetArg(args[n], XmNbackground, edit.oldButtonColor.bg.pixel); n++;
           XtSetArg(args[n], XmNarmColor, edit.oldButtonColor.sc.pixel); n++;
           XtSetArg(args[n], XmNtopShadowColor,
                                    edit.oldButtonColor.ts.pixel); n++;
           XtSetArg(args[n], XmNbottomShadowColor, 
                                    edit.oldButtonColor.bs.pixel); n++;

	   edit.oldButton =
                          XmCreatePushButton(sampleForm, "oldButton", args, n);
           widgetlist3[widget_count3++] = edit.oldButton;
	   XmStringFree(string);
        }

	string =  CMPSTR(((char *)GETMESSAGE(17, 6, "New")));
	n=0;
	XtSetArg(args[n], XmNlabelString, string); n++;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        if(!OldNewSame) {
	   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
	   XtSetArg (args[n], XmNleftWidget, edit.oldButton);  n++;
        }
        else {
	   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
        }
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
	edit.newButton = XmCreatePushButton(sampleForm, "newButton", args, n);
        widgetlist3[widget_count3++] = edit.newButton;
	XmStringFree(string);

        
        if(!OldNewSame) {
           /* make old and new button both the size of the larger of the two */
           width_old = XtWidth(edit.oldButton);
           width_new = XtWidth(edit.newButton);
           w = (width_old>width_new) ? width_old : width_new;

           n=0;
           XtSetArg (args[n], XmNrecomputeSize, False);  n++;
           XtSetArg (args[n], XmNwidth, w+2*BUTTON_MARGIN);  n++;
           XtSetArg (args[n], XmNheight, w+2*BUTTON_MARGIN);  n++;
           XtSetValues(edit.oldButton, args, n);
           XtSetValues(edit.newButton, args, n);
        }
        else
        {
           w = XtWidth(edit.newButton);

           n=0;
           XtSetArg (args[n], XmNrecomputeSize, False);  n++;
           XtSetArg (args[n], XmNwidth, w+2*BUTTON_MARGIN);  n++;
           XtSetArg (args[n], XmNheight, w+2*BUTTON_MARGIN);  n++;
           XtSetValues(edit.newButton, args, n);

        }

        InitializeNewButton();

	n=0;
	string =  CMPSTR(((char *)GETMESSAGE(17, 7, "Grab Color")));
	XtSetArg(args[n], XmNlabelString, string); n++;
        XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP);  n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
        XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
	edit.grabColor = XmCreatePushButtonGadget(sampleForm, "grabColor", args, n);
        widgetlist3[widget_count3++] = edit.grabColor;
	XtAddCallback(edit.grabColor, XmNactivateCallback, 
                      grabcolorCB, (XtPointer)NULL);	
	XmStringFree(string);

        /* Create frame for RGB/HSV scales */
	n = 0;
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, sampleTB); n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
        XtSetArg (args[n], XmNmarginWidth, 0);  n++;
        XtSetArg (args[n], XmNmarginHeight, 0);  n++;
	string =  CMPSTR(((char *)GETMESSAGE(17, 8, "Color Editor")));
	XtSetArg (args[n], XmNtitleString, string); n++;
	sliderTB = _DtCreateTitleBox(edit.main_form, "sliderTB", args, n);
        widgetlist1[widget_count1++] = sliderTB;
	XmStringFree(string);

	n = 0;
        XtSetArg(args[n], XmNhorizontalSpacing,  style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNverticalSpacing,  style.verticalSpacing); n++;
	edit.sliderForm = XmCreateForm(sliderTB, "sliderForm", args, n);

	/*
	 * Create Hue label and scale
	 */

        n = 0;
        string = CMPSTR(((char *)GETMESSAGE(17, 9, "Hue")));
        XtSetArg(args[n], XmNlabelString, string); n++;
        edit.hueLabel = XmCreateLabelGadget(edit.sliderForm, "hueLabel", args, n);
        widgetlist2[widget_count2++] = edit.hueLabel;
	XmStringFree(string);

	n = 0;
	XtSetArg(args[n], XmNshowValue, TRUE);  n++;
	XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_BOTTOM);  n++;
	XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS);  n++; 
	XtSetArg(args[n], XmNmaximum, 359);  n++;
	XtSetArg(args[n], XmNminimum, 0x0);  n++;
	XtSetArg(args[n], XmNincrement, 1); n++;
	edit.hueScale = XmCreateScale(edit.sliderForm, "hueScale", args, n);
        widgetlist2[widget_count2++] = edit.hueScale;
	XtAddCallback(edit.hueScale, XmNvalueChangedCallback, changHSV_CB, (XtPointer) HUE);
	XtAddCallback(edit.hueScale, XmNdragCallback, changHSV_CB, (XtPointer) HUE );

	/*
	 * Create RGB scales
	 */

        string =  CMPSTR(((char *)GETMESSAGE(17, 10, "R")));
        n = 0;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNlabelString, string); n++;
        XtSetArg(args[n], XmNmarginHeight, 0); n++;
        edit.redLabel = XmCreateLabelGadget(edit.sliderForm, "redLabel", args, n);
        widgetlist2[widget_count2++] = edit.redLabel;
	XmStringFree(string);

        string =  CMPSTR(((char *)GETMESSAGE(17, 11, "G")));
        n = 0;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNlabelString, string); n++;
        XtSetArg(args[n], XmNmarginHeight, 0); n++;
        edit.greenLabel = XmCreateLabelGadget(edit.sliderForm, "greenLabel", args, n);
        widgetlist2[widget_count2++] = edit.greenLabel;
	XmStringFree(string);

        string =  CMPSTR(((char *)GETMESSAGE(17, 12, "B")));
        n = 0;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNlabelString, string); n++;
        XtSetArg(args[n], XmNmarginHeight, 0); n++;
        edit.blueLabel = XmCreateLabelGadget(edit.sliderForm, "blueLabel", args, n);
        widgetlist2[widget_count2++] = edit.blueLabel;
	XmStringFree(string);

	n = 0;
	XtSetArg(args[n], XmNshowValue, TRUE);  n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT);  n++;
	XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS);  n++; 
	XtSetArg(args[n], XmNmaximum, 0xff);  n++;
	XtSetArg(args[n], XmNminimum, 0x0);  n++;
	XtSetArg(args[n], XmNincrement, 1); n++;
	XtSetArg(args[n], XmNscaleWidth, SCALE_WIDTH); n++; 
        widgetlist2[widget_count2++] = edit.redScale = 
                XmCreateScale(edit.sliderForm, "redScale", args, n);
	XtAddCallback(edit.redScale, XmNvalueChangedCallback, changRGB_CB, (XtPointer) RED);
	XtAddCallback(edit.redScale, XmNdragCallback, changRGB_CB, (XtPointer) RED );

        widgetlist2[widget_count2++] = edit.greenScale = 
                XmCreateScale(edit.sliderForm, "greenScale", args, n);
	XtAddCallback(edit.greenScale, XmNvalueChangedCallback, changRGB_CB, (XtPointer) GREEN );
	XtAddCallback(edit.greenScale, XmNdragCallback, changRGB_CB, (XtPointer) GREEN );

        widgetlist2[widget_count2++] = edit.blueScale = 
                XmCreateScale(edit.sliderForm, "blueScale", args, n);
	XtAddCallback(edit.blueScale, XmNvalueChangedCallback, changRGB_CB, (XtPointer) BLUE );
	XtAddCallback(edit.blueScale, XmNdragCallback, changRGB_CB, (XtPointer) BLUE);

	/*
	 * Create SV scales
	 */

	/* create Saturation pixmap */
	n=0;
	XtSetArg(args[n], XmNforeground, &foreground); n++;
	XtSetArg(args[n], XmNbackground, &background); n++;
	XtGetValues(edit.sliderForm, args, n);

        n = 0;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
        XtSetArg(args[n], XmNlabelPixmap, XmGetPixmap(style.screen, SATURATION_BITS,
                                     foreground, background)); n++;
        XtSetArg(args[n], XmNmarginHeight, 3); n++;
        edit.satLabel = XmCreateLabelGadget(edit.sliderForm, "satLabel", args, n);
        widgetlist2[widget_count2++] = edit.satLabel;

	/* create Value pixmap */
        n = 0;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END); n++;
        XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
        XtSetArg(args[n], XmNlabelPixmap, XmGetPixmap(style.screen, VALUE_BITS,
                                        foreground, background)); n++;
        XtSetArg(args[n], XmNmarginHeight, 3); n++;
        edit.valLabel = XmCreateLabelGadget(edit.sliderForm, "valLabel", args, n);
        widgetlist2[widget_count2++] = edit.valLabel;

	n = 0;
	XtSetArg(args[n], XmNshowValue, TRUE);  n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
	XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT);  n++;
	XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS);  n++; 
	XtSetArg(args[n], XmNmaximum, 0xff);  n++;
	XtSetArg(args[n], XmNminimum, 0x0);  n++;
	XtSetArg(args[n], XmNincrement, 1); n++;
	XtSetArg(args[n], XmNscaleWidth, SCALE_WIDTH); n++; 
        widgetlist2[widget_count2++] = edit.satScale = 
                XmCreateScale(edit.sliderForm, "satScale", args, n);
	XtAddCallback(edit.satScale, XmNvalueChangedCallback, changHSV_CB, (XtPointer) SATURATION);
	XtAddCallback(edit.satScale, XmNdragCallback, changHSV_CB, (XtPointer) SATURATION );

        widgetlist2[widget_count2++] = edit.valScale = 
                XmCreateScale(edit.sliderForm, "valScale", args, n);
	XtAddCallback(edit.valScale, XmNvalueChangedCallback, changHSV_CB, (XtPointer) VALUE);
	XtAddCallback(edit.valScale, XmNdragCallback, changHSV_CB, (XtPointer) VALUE );

        /* Set the scales to the bg component of the selected color */
        SetScales(&edit.color_set->bg);

        /* Do all the mapping */

        XtManageChildren(widgetlist3, widget_count3);
	XtManageChild(sampleForm);

        XtManageChildren(widgetlist2, widget_count2);
	XtManageChild(edit.sliderForm);

	XtManageChildren(widgetlist1, widget_count1);
	XtManageChild(edit.main_form);

        XtManageChild(edit.DialogShell);  

        if(TypeOfMonitor == XmCO_HIGH_COLOR)
        {
           /* set the trough colors of the RGB scales -
              get the composite children of the scales - 
	                  child[1] is the scrollbar */
           n=0;
           XtSetArg(args[n], XtNchildren, &children); n++;
           XtGetValues(edit.redScale, args, n);
           n=0;
	   XtSetArg(args[n], XmNtroughColor, GetPixel(edit.main_form,"red"));  n++;
           XtSetValues (children[1], args, n);

           n=0;
           XtSetArg(args[n], XtNchildren, &children); n++;
           XtGetValues(edit.greenScale, args, n);
           n=0;
	   XtSetArg(args[n], XmNtroughColor, GetPixel(edit.main_form,"green"));  n++; 
           XtSetValues (children[1], args, n);

           n=0;
           XtSetArg(args[n], XtNchildren, &children); n++;
           XtGetValues(edit.blueScale, args, n);
           n=0;
	   XtSetArg(args[n], XmNtroughColor, GetPixel(edit.main_form,"blue"));  n++; 
           XtSetValues (children[1], args, n);
        }

}


static void 
sliderLayoutCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
	register int     n;
	Arg              args[12];
        Dimension        redLabelWidth;
        Dimension        greenLabelWidth;
        Dimension        blueLabelWidth;
        Dimension        satLabelWidth;
        Dimension        valLabelWidth;
        Dimension        MaxLabelWidth;
	Dimension        redLabelHeight;
	Dimension        satLabelHeight;
	Dimension        valLabelHeight;
	Dimension        redScaleHeight;
	Dimension        TopOffset;
	Dimension	 scale_width;
	Dimension	 pieces;


        /* 
	 * Do the scale and label attachments
	 */

        redLabelHeight = XtHeight(edit.redLabel);
        MaxLabelWidth = redLabelWidth = XtWidth(edit.redLabel);

        greenLabelWidth = XtWidth(edit.greenLabel);
        if (greenLabelWidth > MaxLabelWidth) 
	{
            MaxLabelWidth = greenLabelWidth;
        }
	
        blueLabelWidth = XtWidth(edit.blueLabel);
        if (blueLabelWidth > MaxLabelWidth) 
	{
            MaxLabelWidth = blueLabelWidth;
        }
	
        satLabelHeight = XtHeight(edit.satLabel);
        satLabelWidth = XtWidth(edit.satLabel);
        if (satLabelWidth > MaxLabelWidth) 
	{
            MaxLabelWidth = satLabelWidth;
        }

        valLabelHeight = XtHeight(edit.valLabel);
        valLabelWidth = XtWidth(edit.valLabel);
        if (valLabelWidth > MaxLabelWidth) 
	{
            MaxLabelWidth = valLabelWidth;
        }
	
        redScaleHeight = XtHeight(edit.redScale);


	/* Attach grab button to New button if they are overlapping */

        if(!OldNewSame)  
	    pieces = XtWidth(edit.oldButton) +
	             XtWidth(edit.newButton) +
		     XtWidth(edit.grabColor) +
                     4*style.horizontalSpacing;
	else
	    pieces = XtWidth(edit.newButton) +
		     XtWidth(edit.grabColor) +
                     3*style.horizontalSpacing;
	
	if (XtWidth(edit.main_form) < pieces)
	{
            n=0;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
	    XtSetArg(args[n], XmNleftWidget, edit.newButton);  n++;
	    XtSetValues (edit.grabColor, args, n);

	    scale_width = XtWidth(edit.main_form) - MaxLabelWidth - 
    	    		  XtWidth(edit.hueScale) - 4*style.horizontalSpacing;

            n=0;
	    XtSetArg(args[n], XmNscaleWidth, scale_width); n++; 
	    XtSetValues (edit.redScale, args, n);
	    XtSetValues (edit.greenScale, args, n);
	    XtSetValues (edit.blueScale, args, n);
	    XtSetValues (edit.satScale, args, n);
	    XtSetValues (edit.valScale, args, n);
	}

        /* Hue Label */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
        XtSetValues (edit.hueLabel, args, n);

        /* Hue Scale */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, edit.hueLabel); n++;
        XtSetArg(args[n], XmNtopOffset, style.verticalSpacing); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNbottomOffset, 
	    2*(style.verticalSpacing + redScaleHeight) + 40); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
        XtSetValues (edit.hueScale, args, n);

        /* Red Label */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, edit.hueLabel); n++;
        TopOffset = style.verticalSpacing + redScaleHeight - redLabelHeight;
        XtSetArg(args[n], XmNtopOffset, TopOffset); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNleftOffset, 
	    style.horizontalSpacing + (MaxLabelWidth - redLabelWidth)); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE); n++;
        XtSetValues (edit.redLabel, args, n);

        /* Green Label */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNtopOffset, TopOffset); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNleftOffset, 
	    style.horizontalSpacing + (MaxLabelWidth - greenLabelWidth)); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE); n++;
        XtSetValues (edit.greenLabel, args, n);

        /* Blue Label */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, edit.greenLabel); n++;
        XtSetArg(args[n], XmNtopOffset, TopOffset); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNleftOffset, 
	    style.horizontalSpacing + (MaxLabelWidth - blueLabelWidth)); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE); n++;
        XtSetValues (edit.blueLabel, args, n);

        /* Saturation Label */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, edit.blueLabel); n++;
        TopOffset = style.verticalSpacing + 40 + redScaleHeight - satLabelHeight;
        XtSetArg(args[n], XmNtopOffset, TopOffset); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNleftOffset, 
	    style.horizontalSpacing + (MaxLabelWidth - satLabelWidth)); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE); n++;
        XtSetValues (edit.satLabel, args, n);

        /* Value Label */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNtopWidget, edit.satLabel); n++;
        TopOffset = style.verticalSpacing + redScaleHeight - valLabelHeight;
        XtSetArg(args[n], XmNtopOffset, TopOffset); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
        XtSetArg(args[n], XmNleftOffset, 
	    style.horizontalSpacing + (MaxLabelWidth - valLabelWidth)); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE); n++;
        XtSetValues (edit.valLabel, args, n);

        /* Red Scale */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNbottomOffset,  0); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNleftWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNleftOffset,  style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNrightWidget, edit.hueScale); n++;
        XtSetValues (edit.redScale, args, n);

        /* Green Scale */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget, edit.greenLabel); n++;
        XtSetArg(args[n], XmNbottomOffset,  0); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNleftWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNleftOffset,  style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNrightWidget, edit.hueScale); n++;
        XtSetValues (edit.greenScale, args, n);

        /* Blue Scale */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget, edit.blueLabel); n++;
        XtSetArg(args[n], XmNbottomOffset,  0); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNleftWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNleftOffset,  style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNrightWidget, edit.hueScale); n++;
        XtSetValues (edit.blueScale, args, n);

        /* Saturation Scale */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget, edit.satLabel); n++;
        XtSetArg(args[n], XmNbottomOffset,  0); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNleftWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNleftOffset,  style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNrightWidget, edit.hueScale); n++;
        XtSetValues (edit.satScale, args, n);

        /* Value Scale */
        n=0;
        XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE); n++;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget, edit.valLabel); n++;
        XtSetArg(args[n], XmNbottomOffset,  0); n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNleftWidget, edit.redLabel); n++;
        XtSetArg(args[n], XmNleftOffset,  style.horizontalSpacing); n++;
        XtSetArg(args[n], XmNrightAttachment,  XmATTACH_WIDGET); n++;
        XtSetArg(args[n], XmNrightWidget, edit.hueScale); n++;
        XtSetValues (edit.valScale, args, n);

	XtRemoveCallback(edit.DialogShell, XmNmapCallback, sliderLayoutCB, NULL);

}


/************************************************************************
 *   InitializeNewButton()
 *           set NewButton colors to point to the PixelSet being
 *           modified so they both get updated.
 ************************************************************************/
static void 
InitializeNewButton( void )
{
	register int     n;
	Arg              args[6];

        n=0;
	XtSetArg(args[n], XmNforeground, edit.color_set->fg.pixel); n++;
        XtSetArg(args[n], XmNbackground, edit.color_set->bg.pixel); n++;
        XtSetArg(args[n], XmNarmColor, edit.color_set->sc.pixel); n++;
        if(UsePixmaps == FALSE)
        {
           XtSetArg(args[n], XmNtopShadowColor, 
                                      edit.color_set->ts.pixel); n++;
           XtSetArg(args[n], XmNbottomShadowColor, 
                                      edit.color_set->bs.pixel); n++;
        }
        XtSetValues(edit.newButton,args,n);
}


/************************************************************************
 *   InitializeOldButton()
 *           Called only once from CreateColorEditor.  Allocates a
 *           new PixelSet to hold onto the original ColorSet values
 *           while the newButton gets updated.  Shouldn't be called if
 *           monitor is B_W.
 ************************************************************************/
static void 
InitializeOldButton( void )
{
    register int     i,n, numOfPixels;
    unsigned long    *pixels;
    unsigned long    plane_mask;
    int              hue, val, sat, status;

    if(UsePixmaps == FALSE)
       if(FgColor == DYNAMIC)
          numOfPixels = 5;
       else /* FgColor == BLACK or WHITE */
          numOfPixels = 4;
    else  /* UsePixmaps == TRUE */
       if(FgColor == DYNAMIC)
          numOfPixels = 3;
       else /* FgColor == BLACK or WHITE */
          numOfPixels = 2;

   /* Allocate enough space to store numOfPixels. */
     pixels = (unsigned long *)XtMalloc (numOfPixels * sizeof (unsigned long));

   /* Allocate new color cells for the new button */
     status = XAllocColorCells (style.display, style.colormap,
                          0, &plane_mask, 0, pixels, numOfPixels);
     
     if(status == FALSE)
     {
      /* Dialog stating that can't allocate enough pixels to color 
         the old button. Just don't create old button.*/
         edit.oldButtonColor.bg.pixel = edit.color_set->bg.pixel;
         edit.oldButtonColor.bg.flags = edit.color_set->bg.flags;
         edit.oldButtonColor.fg.pixel = edit.color_set->fg.pixel;
         edit.oldButtonColor.fg.flags = edit.color_set->fg.flags;
         edit.oldButtonColor.sc.pixel = edit.color_set->sc.pixel;
         edit.oldButtonColor.sc.flags = edit.color_set->sc.flags;
         edit.oldButtonColor.bs.pixel = edit.color_set->bs.pixel;
         edit.oldButtonColor.bs.flags = edit.color_set->bs.flags;
         edit.oldButtonColor.ts.pixel = edit.color_set->ts.pixel;
         edit.oldButtonColor.ts.flags = edit.color_set->ts.flags;
         OldNewSame = True;
     }
     else
     {

       /*  Assign the pixels */
         n=0;
         edit.oldButtonColor.bg.pixel = pixels[n++];
         edit.oldButtonColor.bg.flags = DoRed | DoGreen | DoBlue;
         edit.oldButtonColor.sc.pixel = pixels[n++];
         edit.oldButtonColor.sc.flags = DoRed | DoGreen | DoBlue;
         if(FgColor == DYNAMIC)
         {
            edit.oldButtonColor.fg.pixel = pixels[n++];
            edit.oldButtonColor.fg.flags = DoRed | DoGreen | DoBlue;
         } 
         else
         {
            edit.oldButtonColor.fg.pixel = edit.color_set->fg.pixel;
            edit.oldButtonColor.fg.flags = edit.color_set->fg.flags;
         }
         if(UsePixmaps == FALSE)
         {
            edit.oldButtonColor.bs.pixel = pixels[n++];
            edit.oldButtonColor.bs.flags = DoRed | DoGreen | DoBlue;
            edit.oldButtonColor.ts.pixel = pixels[n];
            edit.oldButtonColor.ts.flags = DoRed | DoGreen | DoBlue;
         }
         else
         {
            edit.oldButtonColor.bs.pixel = edit.color_set->bs.pixel;
            edit.oldButtonColor.bs.flags = edit.color_set->bs.flags;
            edit.oldButtonColor.ts.pixel = edit.color_set->ts.pixel;
            edit.oldButtonColor.ts.flags = edit.color_set->ts.flags;
         }
     }

    CopyPixelSet(&edit.oldButtonColor,edit.color_set);
    
    RGBtoHSV(edit.oldButtonColor.bg.red, 
	     edit.oldButtonColor.bg.green, 
	     edit.oldButtonColor.bg.blue, 
	     &hue, &sat, &val);
    
    XtFree((char *)pixels);
}

/************************************************************************
 *   GenerateColors()
 *           Generates new RGB values for fg, ts, bs, sc based on bg.
 *           The Color generation routine will be exported in motif1.1
 *           The generated colors are then used to update the pixels
 *           of the ColorSet being edited.
 ************************************************************************/
static void 
GenerateColors( void )
{
    int hue, sat, val;
    int         j=0;
    XColor      colors[5];

    if (edit.calcRGB == NULL) 
        edit.calcRGB = XmGetColorCalculation();

    (*edit.calcRGB) (&edit.color_set->bg, &edit.color_set->fg, 
		     &edit.color_set->sc, &edit.color_set->ts, 
		     &edit.color_set->bs);

    RGBtoHSV(edit.color_set->bg.red, edit.color_set->bg.green,
                edit.color_set->bg.blue,
                &hue, &sat, &val);

    colors[j++] =  edit.color_set->bg;
    colors[j++] =  edit.color_set->sc;

    if (FgColor == DYNAMIC)
        colors[j++] =  edit.color_set->fg;

    if(UsePixmaps == FALSE)
    {
       colors[j++] =  edit.color_set->ts;
       colors[j++] =  edit.color_set->bs;
    }
    else
    {
        edit.color_set->ts.red = 65535;
        edit.color_set->ts.green = 65535;
        edit.color_set->ts.blue = 65535;

        edit.color_set->bs.red = 0;
        edit.color_set->bs.green = 0;
        edit.color_set->bs.blue = 0;
    }

    if(style.visualClass==PseudoColor || style.visualClass==StaticColor)
	{
		XStoreColors(style.display, style.colormap, colors, j );
	}
	else if(style.visualClass==TrueColor || style.visualClass==DirectColor)
	{
		static unsigned long pixels[4];
		static int count=0;

		if(count){
			XFreeColors(style.display,style.colormap,pixels,count,0);
			count=0;
		}

		if(XAllocColor(style.display,style.colormap,&edit.color_set->fg))
			pixels[count++]=edit.color_set->fg.pixel;
		if(XAllocColor(style.display,style.colormap,&edit.color_set->bg))
			pixels[count++]=edit.color_set->bg.pixel;
		if(XAllocColor(style.display,style.colormap,&edit.color_set->ts))
			pixels[count++]=edit.color_set->ts.pixel;
		if(XAllocColor(style.display,style.colormap,&edit.color_set->bs))
			pixels[count++]=edit.color_set->bs.pixel;

		XtVaSetValues(edit.newButton,
			XmNbackground,edit.color_set->bg.pixel,
			XmNarmColor,edit.color_set->bg.pixel,
			XmNforeground,edit.color_set->fg.pixel,
			XmNtopShadowColor,edit.color_set->ts.pixel,
			XmNbottomShadowColor,edit.color_set->bs.pixel,NULL);
	}
 }

/************************************************************************
 *   changeRGB_CB()
 *           Called when one of the RGB scales is moved
 ************************************************************************/
static void 
changRGB_CB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int reason_code;
    int value;
    int color;

    reason_code = ((XmAnyCallbackStruct *)call_data)->reason;
    if ( reason_code == XmCR_VALUE_CHANGED || reason_code == XmCR_DRAG )
    {
	color = (int) client_data;
	value = ((XmScaleCallbackStruct *)call_data)->value;

	if (edit.current_scale == NONE)
	{
	    edit.current_scale = color;	      
	    /*
	     * Shift value -- to make up for scale max of only 0xff
	     */
	    value <<= 8;
	    switch (color)
	    {
		  case RED:
		      edit.color_set->bg.red = value;
		      break;
		  case GREEN:
		      edit.color_set->bg.green = value;
		      break;
		  case BLUE:
		      edit.color_set->bg.blue = value;
		      break;
		  default:
                      /* this case should never be hit */
		      return;
	    }

	    SetScales(&edit.color_set->bg);
	    GenerateColors();
            edit.current_scale = NONE;
	}
    }
}


/************************************************************************
 *   changHSV_CB()
 *           Called when one of the HSV scales is moved
 ************************************************************************/
static void 
changHSV_CB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int reason_code;
    int value;
    int scale;
    int hue, sat, val;

    reason_code = ((XmAnyCallbackStruct *)call_data)->reason;
    if ( reason_code == XmCR_VALUE_CHANGED || reason_code == XmCR_DRAG )
    {
	scale = (int) client_data;
	value = ((XmScaleCallbackStruct *)call_data)->value;

	if (edit.current_scale == NONE)
	{
	    edit.current_scale = scale;	      
	    switch (scale)
	    {
		  case HUE:
		     hue = value;
		     XmScaleGetValue(edit.satScale, &sat);
		     XmScaleGetValue(edit.valScale, &val);
		     break;
		  case SATURATION:
		     XmScaleGetValue(edit.hueScale, &hue);
		     sat = value;
		     XmScaleGetValue(edit.valScale, &val);
		     break;
		  case VALUE:
		     XmScaleGetValue(edit.hueScale, &hue);
		     XmScaleGetValue(edit.satScale, &sat);
		     val = value;
		     break;
		  default:
                     /* this case should never be hit */
		     return;
	    }

	    HSVtoRGB(hue, sat, val, 
		     &edit.color_set->bg.red, 
		     &edit.color_set->bg.green,
		     &edit.color_set->bg.blue);

	    SetRGBHSVScales(&edit.color_set->bg, hue, sat, val);
	    GenerateColors();
            edit.current_scale = NONE;
	}      
     }
}


/************************************************************************
 *   SetScales()
 *           passed a XColor, generates HSV values and updates all scales.
 ************************************************************************/
static void 
SetScales(
        XColor *rgb )
{
        int      h, s, v;

        RGBtoHSV(rgb->red, rgb->green, rgb->blue, &h, &s, &v);

        SetRGBHSVScales(rgb, h, s, v);
}


/************************************************************************
 *   SetRGBHSVScales()
 *           updates RGB and HSV scales
 ************************************************************************/
static void 
SetRGBHSVScales(
        XColor *rgb,
        int h,
        int s,
        int v )
{
	XmScaleSetValue(edit.redScale,rgb->red >> 8);
	XmScaleSetValue(edit.greenScale,rgb->green >> 8);
	XmScaleSetValue(edit.blueScale,rgb->blue >> 8);

        XmScaleSetValue(edit.hueScale,h);
        XmScaleSetValue(edit.satScale,s);
        XmScaleSetValue(edit.valScale,v);
}


/************************************************************************
 *   CopyPixelSet()
 *   
 ************************************************************************/
static void 
CopyPixelSet(
        ColorSet *color_set_dest,
        ColorSet *color_set_src )
{
    Arg          args[6];
    int          n;
    int              j=0;
    XColor           colors[5];

    color_set_dest->bg.red = color_set_src->bg.red;
    color_set_dest->bg.blue = color_set_src->bg.blue;
    color_set_dest->bg.green = color_set_src->bg.green;
    if(OldNewSame)
       color_set_dest->bg.pixel = color_set_src->bg.pixel;
    colors[j++] =  color_set_dest->bg;

    color_set_dest->sc.red = color_set_src->sc.red;
    color_set_dest->sc.blue = color_set_src->sc.blue;
    color_set_dest->sc.green = color_set_src->sc.green;
    if(OldNewSame)
       color_set_dest->bg.pixel = color_set_src->bg.pixel;
    colors[j++] =  color_set_dest->sc;

    color_set_dest->fg.red = color_set_src->fg.red;
    color_set_dest->fg.blue = color_set_src->fg.blue;
    color_set_dest->fg.green = color_set_src->fg.green;
    if(FgColor == DYNAMIC)
    {
       if(OldNewSame)
          color_set_dest->bg.pixel = color_set_src->bg.pixel;
       colors[j++] =  color_set_dest->fg;
    }

    color_set_dest->ts.red = color_set_src->ts.red;
    color_set_dest->ts.blue = color_set_src->ts.blue;
    color_set_dest->ts.green = color_set_src->ts.green;
    color_set_dest->bs.red = color_set_src->bs.red;
    color_set_dest->bs.blue = color_set_src->bs.blue;
    color_set_dest->bs.green = color_set_src->bs.green;
    if(UsePixmaps == FALSE)
    {
       if(OldNewSame)
          color_set_dest->bg.pixel = color_set_src->bg.pixel;
       colors[j++] =  color_set_dest->ts;
       colors[j++] =  color_set_dest->bs;
    }
    if(OldNewSame && edit.oldButton != NULL)
    {
       n=0;
       XtSetArg(args[n], XmNforeground, color_set_dest->fg.pixel); n++;
       XtSetArg(args[n], XmNbackground, color_set_dest->bg.pixel); n++;
       XtSetArg(args[n], XmNarmColor, color_set_dest->sc.pixel); n++;
       XtSetArg(args[n], XmNtopShadowColor,
                                    color_set_dest->ts.pixel); n++;
       XtSetArg(args[n], XmNbottomShadowColor, 
                                    color_set_dest->bs.pixel); n++;

       XtSetValues(edit.oldButton, args, n);
    }
	if(style.visualClass == PseudoColor || style.visualClass == GrayScale)
		XStoreColors(style.display, style.colormap, colors, j );

}


/************************************************************************
 *   RGBtoHSV()
 *
 ************************************************************************/
void 
RGBtoHSV(
#if NeedWidePrototypes
        unsigned int r ,
        unsigned int g ,
        unsigned int b ,
#else
        unsigned short r,
        unsigned short g,
        unsigned short b,
#endif
        int *h,
        int *s,
        int *v )
{
   double red, green, blue;
   double red1, green1, blue1;
   double hue, saturation, value;
   double base;


   red1 = (double)r / 65280.0;
   green1 = (double)g / 65280.0;
   blue1 = (double)b / 65280.0;

   value = max(red1, green1, blue1);
   base = min(red1, green1, blue1);

   if(value != 0.0)
      saturation = (value - base) / value;
   else
      saturation = 0.0;

   hue = 0.0;

   if( saturation != 0.0 )
   {
      red = (value - red1 ) / (value - base);
      green = (value - green1 ) / (value - base);
      blue = (value - blue1 ) / (value - base);

      if(value == red1)
      {
         if(base == green1)
            hue = 5.0 + blue;
         else
            hue = 1.0 - green;
      }
      else if(value == green1)
      {
         if(base == blue1)
            hue = 1.0 + red;
         else
            hue = 3.0 - blue;
      }
      else
      {
         if(base == red1)
            hue = 3.0 + green;
         else
            hue = 5.0 - red;
      }
      hue *= 60.0;
      if( hue == 360.0)
        hue = 0.0;
    }

    *h = (int)hue;
    *s = ((int)(saturation * 65280.0)) >> 8;
    *v = ((int)(value * 65280.0)) >> 8;
}

/************************************************************************
 *   HSVtoRGB()
 *
 *  Converts hue, saturation, and value to RGB values.  
 *  Hue is in the range 0 to 360, while saturation and value
 *  are in the range 0 to 255
 ************************************************************************/
static void 
HSVtoRGB(
        int h,
        int s,
        int v,
        unsigned short *r,
        unsigned short *g,
        unsigned short *b )
{
   double p1, p2, p3;
   double hue, sat, val;
   double red, green, blue;
   double i, f;

   hue = (double)h / 60.0;
   i = floor(hue);
   f = hue - i;

   val = (double)v / 255.0;
   sat = (double)s / 255.0;

   p1 = val * (1.0 - sat);
   p2 = val * (1.0 -(sat * f));
   p3 = val * (1.0 -(sat * (1.0 - f)));

   switch((int)i)
   {
     case 0:
       red = val;
       green = p3;
       blue = p1;
       break;
     case 1:
       red = p2;
       green = val;
       blue = p1;
       break;
     case 2:
       red = p1;
       green = val;
       blue = p3;
       break;
     case 3:
       red = p1;
       green = p2;
       blue = val;
       break;
     case 4:
       red = p3;
       green = p1;
       blue = val;
       break;
     case 5:
       red = val;
       green = p1;
       blue = p2;
       break;
   }

   *r = (int)(red * 65280.0);
   *g = (int)(green * 65280.0);
   *b = (int)(blue * 65280.0);

}


/************************************************************************
 *   max()
 * 
 ************************************************************************/
static double 
max(
        double x,
        double y,
        double z )
{
   if(x >= y && x >= z)
      return(x);

   if(y >= z && y >= x)
      return(y);

   if(z >= y && z >= x)
      return(z);
}

/************************************************************************
 *   min()
 *
 ************************************************************************/
static double 
min(
        double x,
        double y,
        double z )
{
   if(x <= y && x <= z)
      return(x);

   if(y <= z && y <= x)
      return(y);

   if(z <= y && z <= x)
      return(z);
}

/************************************************************************
 *   grabcolorCB()
 *           Call back routine for grabbing a color from the screen
 ************************************************************************/
static void 
grabcolorCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
	Cursor cursor = XCreateFontCursor(style.display, XC_crosshair);
        XImage *image_ptr; 
        Position x,y;
        XEvent event;
        Pixel pixel;
        XColor colorStruct;
        char           color_string[MAX_STR_LEN];
        int status, offset;
        Boolean notDone=True;
        KeySym keySym;

       /* grab the pointer using target cursor */
        status = XtGrabPointer(style.colorDialog, TRUE, 
                                ButtonPressMask | ButtonReleaseMask,
                                GrabModeAsync, GrabModeAsync, None, 
                                cursor, CurrentTime); 
	if (status != GrabSuccess)
        {
               _DtSimpleError (progName, DtWarning, NULL, 
                    ((char *)GETMESSAGE(17, 14, "Warning, couldn't grab pointer.\n")), NULL);
               return;
        }
       
       /* grab the keyboard so we can get the ESC button press */
        status = XtGrabKeyboard(style.colorDialog, False, GrabModeAsync, 
                                          GrabModeAsync, CurrentTime);
	if (status != GrabSuccess)
        {
               XtUngrabPointer (style.colorDialog, CurrentTime);
               _DtSimpleError (progName, DtWarning, NULL, 
                    ((char *)GETMESSAGE(17, 15, "Warning, couldn't grab Keyboard.\n")), NULL);
               return;
        }
 
        while(notDone)
        {
           XtNextEvent(&event);
           
           switch (event.type) {
              case ButtonPress:
                 break;
              case ButtonRelease:
                 notDone = False;
                 break;
              case KeyPress:
                /* look for ESC key press and stop if we get one */
                 if( event.xkey.state & ShiftMask)
                   offset = 1;
                 else
                   offset = 0;
                 
                 keySym = XLookupKeysym((XKeyEvent *)&event, offset);
                 if (keySym == XK_Escape)
                 {
                    XtUngrabKeyboard (style.colorDialog, CurrentTime);
                    XtUngrabPointer (style.colorDialog, CurrentTime);
                    return;
                 }
             default:
                 XtDispatchEvent(&event);
           }
        }

        XtUngrabKeyboard (style.colorDialog, CurrentTime);
        XtUngrabPointer (style.colorDialog, CurrentTime);

        x = (Position)event.xbutton.x_root;
        y = (Position)event.xbutton.y_root;
        image_ptr = XGetImage (style.display, style.root, x, y, 1, 1, 
			       AllPlanes, ZPixmap);
        pixel = (Pixel) XGetPixel (image_ptr, 0, 0);
        XDestroyImage (image_ptr);

        colorStruct.pixel = pixel;

        XQueryColor (style.display, style.colormap, &colorStruct);

        edit.color_set->bg.red = colorStruct.red;
        edit.color_set->bg.green = colorStruct.green;
        edit.color_set->bg.blue = colorStruct.blue;

        SetScales(&edit.color_set->bg);
        GenerateColors();

}

/************************************************************************
 *   GetPixel()
 *   
 ************************************************************************/
static Pixel 
GetPixel(
        Widget widget,
        char *color_string )
{
    XrmValue from, to;

    from.size = strlen(color_string) + 1;
    if (from.size < sizeof(String))
        from.size = sizeof(String);
    from.addr = color_string;
    XtConvert(widget, XmRString, &from, XmRPixel, &to);

    return ((Pixel) *((Pixel *) to.addr));
}



/*
**  dialogBoxCB
**      Process callback from the Ok, Cancel and Help pushButtons in the 
**      DialogBox.
*/
static void 
dialogBoxCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    palette *tmp_palette;
    DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;

    switch (cb->button_position)
    {
      case OK_BUTTON:

	  WriteOutPalette( pCurrentPalette->name);

          if(strcmp(pCurrentPalette->name, defaultName) == 0)
          {
             UpdateDefaultPalette();
             SaveOrgPalette();
          }

	  XtUnmanageChild(edit.DialogShell);
          break;

      case CANCEL_BUTTON:
          CopyPixelSet(edit.color_set, &edit.oldButtonColor);
          XtUnmanageChild(edit.DialogShell);
          break;
         
      case HELP_BUTTON:
        XtCallCallbacks(edit.DialogShell, XmNhelpCallback, (XtPointer)NULL);
	break;

      default:
	break;
    }
}



/************************************************************************
 * _DtmapCB
 *
 ************************************************************************/
static void 
_DtmapCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    Position	 newX,newY;
    Position	 x,y;
    Dimension	 editWidth,editHeight;
    Dimension	 width,height;
    Arg          args[6];
    int          n;
    XtWidgetGeometry reply;
    Widget       parent = (Widget) client_data;


    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));
    
    if (!save.restoreFlag)
    {
	/* get x,y,width,height of parent shell */

        x = XtX(XtParent(parent));
        y = XtY(XtParent(parent));
        height = XtHeight(parent);
        width = XtWidth(parent);

        editHeight = XtHeight(edit.DialogShell);
        editWidth = XtWidth(edit.DialogShell);

	newX = x + width/2 - editWidth/2;
	if (newX < 0)
	    newX = 0;
	newY = y + height/2 - editHeight;
	if (newY < 0)
	    newY = y + height;
	    
	n = 0;
	XtSetArg(args[n], XmNx, newX); n++;
	XtSetArg(args[n], XmNy, newY); n++;
	XtSetValues(edit.DialogShell,args,n);

    }

    XtRemoveCallback(edit.DialogShell, XmNmapCallback, _DtmapCB, NULL);

}


/****************************************************
 * restoreColor(shell, db)
 * restore any state information saved with saveColor.  
 * This is called from restoreSession with the application shell 
 * and the special xrm database retrieved for restore.
 ****************************************************/
void 
restoreColorEdit(
        Widget shell,
        XrmDatabase db )
{

  XrmName xrm_name[5];
  XrmRepresentation rep_type;
  XrmValue value;

    xrm_name [0] = XrmStringToQuark ("colorEditDlg");
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

    /*Collect any other parameters you saved into static variables.*/

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0)
      XtCallCallbacks(modifyColorButton, XmNactivateCallback, NULL);
}


/****************************************************
 * saveColorEdit(fd)
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with 
 * the file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreColor.  The suggested minimum is whether you are mapped, and your
 * location.
 *****************************************************/
void 
saveColorEdit(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (edit.DialogShell != NULL) 
    {
        if (XtIsManaged(edit.DialogShell))
            sprintf(bufr, "*colorEditDlg.ismapped: True\n");
        else
            sprintf(bufr, "*colorEditDlg.ismapped: False\n");

        /* Get and write out the geometry info for our Window */
        x = XtX(XtParent(edit.DialogShell));
        y = XtY(XtParent(edit.DialogShell));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

        sprintf(bufr, "%s*colorEditDlg.x: %d\n", bufr, x);
        sprintf(bufr, "%s*colorEditDlg.y: %d\n", bufr, y);
        /*any other parameter you want to save goes here*/
        write (fd, bufr, strlen(bufr));
    }
}

