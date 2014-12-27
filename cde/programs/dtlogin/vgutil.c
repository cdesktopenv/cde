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
/* $TOG: vgutil.c /main/11 1998/11/03 19:17:12 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vgutil.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Utility file for Dtgreet application.
 **
 **                This file contains global data declarations, resource
 **		   declarations, and various utility routines.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/


/***************************************************************************
 *
 *  Includes
 *
 ***************************************************************************/

#include <nl_types.h>
#include <stdio.h>
#include <setjmp.h>
#include <time.h>
#include <sys/signal.h>
#include <stdarg.h>
#define Va_start(a,b) va_start(a,b)

#include <X11/Xlibint.h>
#include <Xm/Xm.h>

#include "vg.h"
#include "vgmsg.h"

/****************************************************************************
 *
 *  Defines
 *
 ****************************************************************************/
#define NLSPATH_ENV  "NLSPATH=" \
                        CDE_INSTALLATION_TOP "/nls/msg/%L/%N.cat:" \
                        CDE_INSTALLATION_TOP "/lib/nls/msg/%L/%N.cat:" \
                        CDE_INSTALLATION_TOP "/lib/nls/msg/%l/%t/%c/%N.cat:" \
                        CDE_INSTALLATION_TOP "/lib/nls/msg/%l/%c/%N.cat"

#define NLS_CATALOG  "dtlogin"

/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/
static SIGVAL syncTimeout( int arg ) ;

/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/
static nl_catd	nl_fd;	/* message catalog file descriptor */

/***************************************************************************
 *
 *  ChangeBell
 *
 *  turn the keyboard bell on/off
 *
 *  Input:  "on",  "off"
 ***************************************************************************/

void 
ChangeBell( char *string )
{
    static int	percent = -1;
    
    XKeyboardControl control;
    XKeyboardState   state;
    
    if ( strcmp (string, "on" ) == 0 )
	control.bell_percent = percent;
    
    if ( strcmp (string, "off") == 0 ) {
	XGetKeyboardControl(dpyinfo.dpy, &state);
	percent = state.bell_percent;
	control.bell_percent = 0; 
    }	

    if (percent >= 0)
	XChangeKeyboardControl(dpyinfo.dpy, KBBellPercent, &control);
}



	
/***************************************************************************
 *
 *  InitArg
 *
 *  Initialize the wiget argument list before creating the widget. These
 *  are resources the user cannot override.
 *
 *  The values chosen for these resources are not necessarily the Motif
 *  defaults, but rather those that are most common for all the widgets. A
 *  particular widget may override any of these choices before creation.
 ***************************************************************************/

static XmTextScanType sarray[] = {XmSELECT_POSITION};

int 
InitArg( WidgetType wtype )
{
    register int i;

    i = 0;


    /*
     *  Core resource set...
     */
     
     					/* bug workaround in toolkit	   */
    if (wtype == DrawingA   ||
        wtype == Frame      ||
	wtype == Label	    ||
	wtype == MessageBox ||
	wtype == PushB	    ||
	wtype == Text		) {
    
	XtSetArg(argt[i], XmNaccelerators,	NULL			); i++;
    }


    if (wtype == DrawingA   ||
        wtype == Frame      ||
	wtype == Form	    ||
	wtype == Label	    ||
	wtype == MessageBox ||
	wtype == PushB	    ||
	wtype == Text		) {
    
	XtSetArg(argt[i], XmNancestorSensitive,	True			); i++;
	/*		  XmNbackground,	(set by user)		*/
	/*		  XmNbackgroundPixmap,	(set by user)		*/
	/*		  XmNborderColor,	(set by user)		*/
	/*		  XmNborderPixmap,	(set by user)		*/
	XtSetArg(argt[i], XmNborderWidth,	0			); i++;
	/*		  XmNcolormap,		(default)		*/
	/*		  XmNdepth,		(set by Xt)		*/
	XtSetArg(argt[i], XmNdestroyCallback,	NULL			); i++;
	/*		  XmNheight,		(set by user)		*/
	XtSetArg(argt[i], XmNmappedWhenManaged,	True			); i++;
	/*		  XmNscreen,		(default)		*/
	XtSetArg(argt[i], XmNsensitive,		True			); i++;
	/*		  XmNtranslations,	(default)               */
	/*		  XmNwidth,		(set by user)		*/
	/*		  XmNx,			(set by user)		*/
	/*		  XmNy,			(set by user)		*/
    }



    /*
     *  Object resource set...
     */
     
    if (wtype == CascadeBG   ||
	wtype == LabelG	     ||
	wtype == PushBG      ||
	wtype == SeparatorG  ||
	wtype == ToggleBG	) {

	XtSetArg(argt[i], XmNdestroyCallback,	NULL			); i++;
    }



    /*
     *  RectObj resource set...
     */
     
    if (wtype == CascadeBG   ||
	wtype == LabelG	     ||
	wtype == PushBG	     ||
	wtype == SeparatorG  ||
	wtype == ToggleBG	) {
    
	XtSetArg(argt[i], XmNancestorSensitive,	True			); i++;
	XtSetArg(argt[i], XmNborderWidth,	0			); i++;
	/*		  XmNheight,		(set by user)		*/
	XtSetArg(argt[i], XmNsensitive,		True			); i++;
	/*		  XmNwidth,		(set by user)		*/
	/*		  XmNx,			(set by user)		*/
	/*		  XmNy,			(set by user)		*/
    }



    /*
     *  XmGadget  resource set...
     */
     
    if (wtype == CascadeBG   ||
	wtype == LabelG	     ||
	wtype == PushBG      ||
	wtype == SeparatorG  ||
	wtype == ToggleBG	) {
    
	XtSetArg(argt[i], XmNhelpCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNhighlightOnEnter,	True			); i++;
	/*		  XmNhighlightThickness,(set by user)		*/
	/*		  XmNshadowThickness,	(set by user)		*/
	XtSetArg(argt[i], XmNtraversalOn,	True			); i++;
	XtSetArg(argt[i], XmNunitType,		appInfo.unitType	); i++;
	XtSetArg(argt[i], XmNuserData,		NULL			); i++;
    }




    /*
     *  Composite resource set...
     */
     
    if (wtype == DrawingA   ||
        wtype == Frame	    ||
	wtype == Form	    ||
	wtype == MessageBox	) {
    
	XtSetArg(argt[i], XmNinsertPosition,	NULL			); i++;
    }



    /*
     *  XmPrimitive  resource set...
     */
     
    if (wtype == Label	    ||
	wtype == PushB	    ||
	wtype == Text		) {
    
	/*		  XmNbottomShadowColor,	(set by user)		*/
	/*		  XmNbottomShadowPixmap,(set by user)		*/
	/*		  XmNforeground,	(set by user)		*/
	XtSetArg(argt[i], XmNhelpCallback,	NULL			); i++;
	/*		  XmNhighlightColor,	(set by user)		*/
	XtSetArg(argt[i], XmNhighlightOnEnter,	False			); i++;
	/*		  XmNhighlightPixmap,	(set by user)		*/
	/*		  XmNhighlightThickness,(set by user)		*/
	/*		  XmNshadowThickness,	(set by user)		*/
	/*		  XmNtopShadowColor,	(set by user)		*/
	/*		  XmNtopShadowPixmap,	(set by user)		*/
	XtSetArg(argt[i], XmNtraversalOn,	True			); i++;
	XtSetArg(argt[i], XmNunitType,		appInfo.unitType	); i++;
	XtSetArg(argt[i], XmNuserData,		NULL			); i++;
    }



    /*
     *  XmForm Constraint resource set...
     */
     
    if (wtype == Form) {
    
	XtSetArg(argt[i], XmNbottomAttachment,	XmATTACH_NONE		); i++;
	XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_NONE		); i++;
	XtSetArg(argt[i], XmNresizable,		False			); i++;
	XtSetArg(argt[i], XmNrightAttachment,	XmATTACH_NONE		); i++;
	XtSetArg(argt[i], XmNtopAttachment,	XmATTACH_NONE		); i++;
    }


    /*
     *  XmManager resource set...
     */
     
    if (wtype == DrawingA   ||
        wtype == Frame	    ||
	wtype == Form	    ||
	wtype == MessageBox	) {
    
	/*		  XmNbottomShadowColor,	(set by user)		*/
	/*		  XmNbottomShadowPixmap,(set by user)		*/
	/*		  XmNforeground,	(set by user)		*/
	XtSetArg(argt[i], XmNhelpCallback,	NULL			); i++;
	/*		  XmNhighlightColor,	(set by user)		*/
	/*		  XmNhighlightPixmap,	(set by user)		*/
	/*		  XmNshadowThickness,	(set by user)		*/
	/*		  XmNtopShadowColor,	(set by user)		*/
	/*		  XmNtopShadowPixmap,	(set by user)		*/
	XtSetArg(argt[i], XmNunitType,		appInfo.unitType	); i++;
	XtSetArg(argt[i], XmNuserData,		NULL			); i++;
    }
    


    /*
     *  XmBulletinBoard resource set...
     */
     
    if (wtype == Form	    ||
    	wtype == MessageBox	) {
    
	XtSetArg(argt[i], XmNallowOverlap,	True			); i++;
	XtSetArg(argt[i], XmNautoUnmanage,	True			); i++;
	XtSetArg(argt[i], XmNbuttonFontList,	appInfo.labelFont	); i++;
	/*		  XmNcancelButton,	(set by Xm)		*/
	/*		  XmNdefaultButton,	(set by Xm)		*/
	XtSetArg(argt[i], XmNdefaultPosition,	True			); i++;
	XtSetArg(argt[i], XmNdialogStyle,	XmDIALOG_MODELESS	); i++;
	XtSetArg(argt[i], XmNdialogTitle,	NULL			); i++;
	XtSetArg(argt[i], XmNfocusCallback,	NULL			); i++;
/*XtSetArg(argt[i], XmNlabelFontList,	appInfo.textFont	); i++; */
	XtSetArg(argt[i], XmNmapCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNmarginHeight,	FromMM(10)		); i++;
	XtSetArg(argt[i], XmNmarginWidth,	FromMM(10)		); i++;
	XtSetArg(argt[i], XmNmapCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNnoResize,		True			); i++;
	XtSetArg(argt[i], XmNresizePolicy,	XmRESIZE_ANY		); i++;
	XtSetArg(argt[i], XmNshadowType,	XmSHADOW_OUT		); i++;
	XtSetArg(argt[i], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R );i++;
	XtSetArg(argt[i], XmNtextFontList,	appInfo.textFont	); i++;
	XtSetArg(argt[i], XmNtextTranslations,	NULL			); i++;
	XtSetArg(argt[i], XmNunmapCallback,	NULL			); i++;
    }
    


    /*
     *  XmCascadeButtonGadget resource set...
     */
     
    if (wtype == CascadeBG) {
    
	XtSetArg(argt[i], XmNactivateCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNcascadePixmap,	XmUNSPECIFIED_PIXMAP	); i++;
	XtSetArg(argt[i], XmNcascadingCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNmappingDelay,	100			); i++;
	XtSetArg(argt[i], XmNsubMenuId,		0			); i++;
    }
    


    /*
     *  XmDrawingArea resource set...
     */
     
    if (wtype == DrawingA) {
    
	XtSetArg(argt[i], XmNexposeCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNinputCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNmarginHeight,	0			); i++;
	XtSetArg(argt[i], XmNmarginWidth,	0			); i++;
	XtSetArg(argt[i], XmNresizeCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNresizePolicy,	XmRESIZE_NONE		); i++;
    }



    /*
     *  XmForm  resource set...
     */
     
    if (wtype == Form) {
    
	XtSetArg(argt[i], XmNfractionBase,	100			); i++;
	XtSetArg(argt[i], XmNhorizontalSpacing,	0			); i++;
	XtSetArg(argt[i], XmNrubberPositioning,	False			); i++;
	XtSetArg(argt[i], XmNverticalSpacing,	0			); i++;
    }
    


    /*
     *  XmFrame resource set...
     */
     
    if (wtype == Frame) {
    
	XtSetArg(argt[i], XmNmarginHeight,	0			); i++;
	XtSetArg(argt[i], XmNmarginWidth,	0			); i++;
	XtSetArg(argt[i], XmNshadowType,	XmSHADOW_OUT		); i++;
    }



    /*
     *  XmLabel/XmLabelGadget resource set...
     */

    if (wtype == CascadeBG   ||
	wtype == Label	     ||
	wtype == LabelG      ||
	wtype == PushB	     ||
	wtype == PushBG      ||
	wtype == ToggleBG	) {
    
	XtSetArg(argt[i], XmNaccelerator,	NULL			); i++;
	XtSetArg(argt[i], XmNacceleratorText,	NULL			); i++;
/*	XtSetArg(argt[i], XmNalignment,		XmALIGNMENT_CENTER	); i++;*/
	/*		  XmNalignment,		(default)		*/
	XtSetArg(argt[i], XmNfontList,		appInfo.labelFont	); i++;
	XtSetArg(argt[i], XmNlabelInsensitivePixmap,
						XmUNSPECIFIED_PIXMAP	); i++;
	XtSetArg(argt[i], XmNlabelPixmap,	XmUNSPECIFIED_PIXMAP	); i++;
	XtSetArg(argt[i], XmNlabelString,	NULL			); i++;
	XtSetArg(argt[i], XmNlabelType,		XmSTRING		); i++;
	/*		  XmNmarginBottom,	(default)		*/
	/*		  XmNmarginHeight,	(default)		*/
	/*		  XmNmarginLeft,	(default)		*/
	/*		  XmNmarginRight,	(default)		*/
	/*		  XmNmarginTop,		(default)		*/
	/*		  XmNmarginWidth,	(default)		*/
	XtSetArg(argt[i], XmNmnemonic,		NULL			); i++;
	XtSetArg(argt[i], XmNrecomputeSize,	False			); i++;
	XtSetArg(argt[i], XmNuserData,		NULL			); i++;
	XtSetArg(argt[i], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R ); i++;
    }
    


    /*
     *  XmMessageBox resource set...
     */
     
    if (wtype == MessageBox) {
    
	XtSetArg(argt[i], XmNcancelCallback,	NULL			); i++;	
	XtSetArg(argt[i], XmNcancelLabelString,	NULL			); i++;
	XtSetArg(argt[i], XmNdefaultButtonType,	XmDIALOG_OK_BUTTON	); i++;
	XtSetArg(argt[i], XmNdialogType,	XmDIALOG_MESSAGE	); i++;
	XtSetArg(argt[i], XmNhelpLabelString,	NULL			); i++;
	XtSetArg(argt[i], XmNmessageAlignment,  XmALIGNMENT_BEGINNING	); i++;
	XtSetArg(argt[i], XmNmessageString,	NULL			); i++;
	XtSetArg(argt[i], XmNminimizeButtons,	FALSE			); i++;
	XtSetArg(argt[i], XmNokCallback,	NULL			); i++;	
	XtSetArg(argt[i], XmNokLabelString,	NULL			); i++;
	/*		  XmNsymbolPixmap,	(set by Xm)		*/
    }



    /*
     *  XmPushButton/XmPushButtonGadget resource set...
     */
     
    if (wtype == PushB	  ||
	wtype == PushBG		) {
    
	XtSetArg(argt[i], XmNactivateCallback,		NULL		); i++;
	XtSetArg(argt[i], XmNarmCallback,		NULL		); i++;
	/*		  XmNarmColor,			(set by user)	*/
	/*		  XmNarmPixmap,			(set by user)	*/
	XtSetArg(argt[i], XmNdisarmCallback,		NULL		); i++;
	XtSetArg(argt[i], XmNfillOnArm,			True		); i++;
	XtSetArg(argt[i], XmNshowAsDefault,		0		); i++;
    }
    


    /*
     *  XmSeparatorGadget resource set...
     */
     
    if (wtype == SeparatorG) {
    
	XtSetArg(argt[i], XmNmargin,		0			); i++;
	XtSetArg(argt[i], XmNorientation,	XmHORIZONTAL		); i++;
	XtSetArg(argt[i], XmNseparatorType,	XmSHADOW_ETCHED_IN	); i++;
    }



    /*
     *  XmText  resource set...
     */
     
    if (wtype == Text ) {
    
	XtSetArg(argt[i], XmNactivateCallback,		NULL		); i++;
	XtSetArg(argt[i], XmNautoShowCursorPosition,	True		); i++;
	XtSetArg(argt[i], XmNcursorPosition,		0		); i++;
	XtSetArg(argt[i], XmNeditable,			True		); i++;
	XtSetArg(argt[i], XmNeditMode,		     XmSINGLE_LINE_EDIT	); i++;
	XtSetArg(argt[i], XmNfocusCallback,		NULL		); i++;
	XtSetArg(argt[i], XmNlosingFocusCallback,	NULL		); i++;
	XtSetArg(argt[i], XmNmarginHeight,	     TEXT_MARGIN_HEIGHT	); i++;
	XtSetArg(argt[i], XmNmarginWidth,	     TEXT_MARGIN_WIDTH	); i++;
	XtSetArg(argt[i], XmNmaxLength,			20		); i++;
	XtSetArg(argt[i], XmNmodifyVerifyCallback,	NULL		); i++;
	XtSetArg(argt[i], XmNmotionVerifyCallback,	NULL		); i++;
	/*		  XmNtopPosition,		(default)	*/
	/*		  XmNvalue,			(set by ??)	*/
    
	/*		XmText Input  Resource Set			*/
	XtSetArg(argt[i], XmNpendingDelete,		True		); i++;
	XtSetArg(argt[i], XmNselectionArray,		sarray		); i++;
	XtSetArg(argt[i], XmNselectThreshold,		1000		); i++;
    
	/*		XmText Output Resource Set			*/
	/*		  XmNblinkRate,			(set by user)	*/
	XtSetArg(argt[i], XmNfontList,		     appInfo.labelFont	); i++;
	XtSetArg(argt[i], XmNcursorPositionVisible,	True		); i++;
	XtSetArg(argt[i], XmNresizeHeight,		False		); i++;
	XtSetArg(argt[i], XmNresizeWidth,		False		); i++;
	/*		  XmNrows,			(ignored)	*/
	/*		  XmNwordWrap,			(ignored)	*/
    }
    


    /*
     *  XmToggleButtonGadget resource set...
     */
     
    if (wtype == ToggleBG) {
    
	XtSetArg(argt[i], XmNarmCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNdisarmCallback,	NULL			); i++;
	XtSetArg(argt[i], XmNfillOnSelect,	True			); i++;
	XtSetArg(argt[i], XmNindicatorOn,	True			); i++;
	XtSetArg(argt[i], XmNindicatorType,	XmONE_OF_MANY		); i++;
	/*		  XmNselectColor,	(set by user)		*/
	XtSetArg(argt[i], XmNselectInsensitivePixmap,
						XmUNSPECIFIED_PIXMAP	); i++;
	XtSetArg(argt[i], XmNselectPixmap,	XmUNSPECIFIED_PIXMAP	); i++;
	XtSetArg(argt[i], XmNset,		False			); i++;
	/*		  XmNspacing,		(default)		*/
	XtSetArg(argt[i], XmNvalueChangedCallback,
						NULL			); i++;
	XtSetArg(argt[i], XmNvisibleWhenOff,	False			); i++;
    }


    return (i);
}




/***************************************************************************
 *
 *  FromMM
 *
 *  Convert from 100th Millimeter units into the widget's units.
 *
 *  This routine does NOT use the Motif Resolution Independence mechanism.
 *  Unless the user informs the X-server as to the current monitor size (or
 *  the monitor happens to be the default), it is possible that the
 *  millimeter screen size returned by the X-server is incorrect.  This
 *  causes the Motif Resolution Independence mechanism to generate incorrect
 *  values and skew the visuals.
 *
 *  For ease of specification, all sizes internal to Dtgreet are specified
 *  in a generic unit (approx.  100th Millimeters) and then converted to an
 *  equivalent pixel value by this routine.  All Dtgreet widgets default
 *  to pixel unit type.  Displays of the same resolution will contain
 *  identical pixel values, but the actual size on the screen may vary
 *  depending on the monitor size.
 *
 *  There are three conversion values, one each for LowRes (640x480), MedRes
 *  (1024x768), and HiRes (1280x1024) displays.  They are calculated to
 *  produce the same pixel values as does the Motif R.I. mechanism on the
 *  HP 13" Low Res. display, 16" Med. Res.  display and 19" Hi Res. display, 
 *  all pleasing visuals.
 *
 *  If the user specifies the widgets unit type to be 100TH_MILLIMETERS, the
 *  Motif Resolution Independence mechanism is reactivated and can be used by
 *  the user.  The internal sizes specified will not be converted by this
 *  routine but rather internally by Motif.
 *
 ***************************************************************************/

int 
FromMM( int mm )
{
    int			pixel;
    
    if ( appInfo.unitType == Xm100TH_MILLIMETERS )
	return(mm);
    

    /*
     *  convert 100thMillimeters to Pixels. Do not round up to the next
     *  pixel...
     */
     
    if ( HIRES ) 
	pixel = (mm * 0.035854);		/* HiRes	*/

    else  if ( MEDRES )
	pixel = (mm * 0.033574);		/* MedRes	*/

    else 
	pixel = (mm * 0.026016);		/* LowRes	*/

    return(pixel);

}




/***************************************************************************
 *
 *  FromPixel
 *
 *  Convert from pixel units into the widget's units
 ***************************************************************************/

int 
FromPixel( Widget w, int orientation, int pixel )
{
    Arg			argt[10];
    unsigned char	unit_type;
    
    XtSetArg(argt[0], XmNunitType, &unit_type);
    XtGetValues(w, argt, 1);
    
    if (unit_type == XmPIXELS)
	return(pixel);
    else
	return(XmConvertUnits(w,
			      orientation,
			      XmPIXELS,
			      pixel,
			      (int)unit_type));
}




/***************************************************************************
 *
 *  GetBiggest
 *
 *  Utility function to help determine largest of a set of widgets.
 ***************************************************************************/


void 
GetBiggest( Widget widget, Dimension *width, Dimension *height )
{
    Dimension new_width, new_height;

    XtSetArg(argt[0], XmNwidth, &new_width);
    XtSetArg(argt[1], XmNheight, &new_height);

    XtGetValues(widget, argt, 2);

    if (width != NULL && new_width > *width)
	*width = new_width;
    if (height != NULL && new_height > *height)
	*height = new_height;
}




/***************************************************************************
 *
 *  LogError
 *
 *  send errors to the error log. This routine accepts a variable number
 *  of arguments.
 ***************************************************************************/

void 
LogError( unsigned char *fmt, ...)
{
    va_list  args;
    time_t   timer;
    
    Va_start(args,fmt);
    
    if (errorLogFile && errorLogFile[0] && 
        (freopen(errorLogFile, "a", stderr) != NULL)) {

	timer = time(NULL);
	fprintf(stderr, "\n%s", ctime(&timer));
	fprintf (stderr, "error (pid %ld): ", (long)getpid());

	vfprintf (stderr, (char *)fmt, args);
	fflush (stderr);
    }

    va_end(args);
}


/***************************************************************************
 *
 *  CloseCatalog
 *
 ***************************************************************************/

void
CloseCatalog()
{
    catclose(nl_fd);
}

/***************************************************************************
 *
 *  OpenCatalog
 *
 ***************************************************************************/

void
OpenCatalog()
{
    static int initialized = 0;
    char *nlspath, *newnlspath;

    if (initialized) return;
    initialized = 1;

    nlspath = getenv("NLSPATH");
 
    if (NULL == nlspath || 0 == strlen(nlspath))
    {
        newnlspath = malloc(8 + strlen(NLSPATH_ENV) + 1);
        sprintf(newnlspath, "NLSPATH=%s", NLSPATH_ENV);
    }
    else
    {
        newnlspath = malloc(8 + strlen(nlspath) + 1 + strlen(NLSPATH_ENV) + 1);
        sprintf(newnlspath, "NLSPATH=%s:%s", nlspath, NLSPATH_ENV);
    }

    putenv(newnlspath);

    /*
     * open the message catalog. If the language_specific version cannot
     * be opened, try the default...
     */
    if (NULL != langenv)
    {
	nl_fd = catopen(NLS_CATALOG, NL_CAT_LOCALE);
	if (0 > (long) nl_fd)
	  LogError((unsigned char*) MC_DEF_LOG_NO_MSGCAT, langenv);
    }
}


/***************************************************************************
 *
 *  ReadCatalog
 *
 *  read a string from the message catalog and convert to unsigned char *
 ***************************************************************************/

unsigned char *
ReadCatalog(int setn, int msgn, char *dflt)
{
    OpenCatalog();
    if ((0 > (long) nl_fd) || (NULL == langenv))
      return (unsigned char*) dflt;
    else
      return (unsigned char*) catgets(nl_fd, setn, msgn, dflt);
}
 
/***************************************************************************
 *
 *  ReadCatalogXms
 *
 *  read a string from the message catalog and convert to compound string
 ***************************************************************************/

XmString
ReadCatalogXms(int setn, int msgn, char *dflt)
{
    OpenCatalog();
    if (0 > (long) nl_fd)
      return XmStringCreateLocalized(dflt);
    else
      return XmStringCreateLocalized((char*) ReadCatalog(setn, msgn, dflt));
}



/***************************************************************************
 *
 *  SecureDisplay/UnsecureDisplay
 *
 *  grab/release the server and keyboard
 ***************************************************************************/

static jmp_buf	syncJump;
static int	grabServer;		/* Boolean on grabbing server	   */
static int	grabTimeout;		/* timeout to grab server	   */

static SIGVAL
syncTimeout( int arg )
{
    longjmp (syncJump, 1);
}

int 
SecureDisplay( void )
{
    
    char *t;
    

    /*
     *  get grab values from the environment...
     */

    grabServer  = ((t = (char *)getenv(GRABSERVER))  == NULL ? 0 : atoi(t));
    grabTimeout = ((t = (char *)getenv(GRABTIMEOUT)) == NULL ? 0 : atoi(t));


    /*
     *  grab server then the keyboard...
     */
    signal (SIGALRM, syncTimeout);
    if (setjmp (syncJump)) {
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_NO_SECDPY,MC_DEF_LOG_NO_SECDPY),
		   dpyinfo.name);
	return(1);
    }
    alarm ((unsigned int) grabTimeout);
    XGrabServer (dpyinfo.dpy);
    if (XGrabKeyboard (dpyinfo.dpy,
    		       DefaultRootWindow (dpyinfo.dpy),
		       True,
		       GrabModeAsync,
		       GrabModeAsync,
		       CurrentTime) != GrabSuccess) {
	alarm (0);
	signal (SIGALRM, SIG_DFL);
	LogError(ReadCatalog(MC_LOG_SET,MC_LOG_NO_SECKEY,MC_DEF_LOG_NO_SECKEY),
		  dpyinfo.name);
	return(1);
    }

    alarm (0);
    signal (SIGALRM, SIG_DFL);
/*    pseudoReset (dpy);*/

    /*
     *  release server if grab not requested...
     */
    if (!grabServer)
    {
	XUngrabServer (dpyinfo.dpy);
	XSync (dpyinfo.dpy, 0);
    }

    return (0);
}


void 
UnsecureDisplay( void )
{
    XUngrabKeyboard(dpyinfo.dpy, CurrentTime);
    if (grabServer)
	XUngrabServer (dpyinfo.dpy);
    XSync (dpyinfo.dpy, 0);
}




/***************************************************************************
 *
 *  SetResourceDatabase
 *
 *  
 *  set up display's resource database with defaults and user values
 *
 *  Starting with DT 3.0, the widget's unit type defaults to XmPIXELS 
 *  rather than Xm100TH_MILLIMETERS. For backwards compatibility, it is
 *  necessary to determine if the user has specified Xm100TH_MILLIMETERS
 *  for the widget's unit type.
 *
 ***************************************************************************/

void 
SetResourceDatabase( void )
{

typedef struct {
	char *name;
	char *value;
    } DefResource;


    int 	i;
    char	defaultWidth[32], defaultHeight[32];
    char	*rmtype;	/* for XrmGetResource()			   */
    XrmValue	rmvalue;	/* for XrmGetResource()			   */

    static XrmDatabase	defDb;
    	   XrmDatabase	userDb; 


static DefResource defResource[] = {
    { "*unitType", 			XmNPIXELS		},
    { "*matte.x",                        "-1"                    },
    { "*matte.y",                        "-1"                    },
    { "*login_matte.topPosition", 	LOGIN_TOP_POSITION	},
    { "*login_matte.bottomPosition", 	LOGIN_BOTTOM_POSITION	},
    { "*login_matte.leftPosition", 	LOGIN_LEFT_POSITION	},
    { "*login_matte.rightPosition", 	LOGIN_RIGHT_POSITION	},
    { "*table.background", 		TABLE_BACKGROUND	},
    { "*login_text.foreground",		TEXT_FOREGROUND		},
    { "*passwd_text.foreground",	TEXT_FOREGROUND		},
    };

# define NUM_RESOURCES   (sizeof defResource / sizeof defResource[0])


static DefResource defResourceBW[] = {
    { "*background", 			BW_BACKGROUND		},
    { "*foreground", 			BW_FOREGROUND		},
    { "*login_text.background",		BW_TEXT_BACKGROUND	},
    { "*passwd_text.background",	BW_TEXT_BACKGROUND	},
    };

# define NUM_BW_RESOURCES   (sizeof defResourceBW / sizeof defResourceBW[0])


static DefResource defResourceGS[] = {
    { "*logo*Background",		BW_LOGO_BACKGROUND	},
    };

# define NUM_GS_RESOURCES   (sizeof defResourceGS / sizeof defResourceGS[0])


static DefResource defResourceCO[] = {
    { "*background", 			CO_BACKGROUND		},
    { "*foreground", 			CO_FOREGROUND		},
    { "*login_text.background",		CO_TEXT_BACKGROUND	},
    { "*passwd_text.background",	CO_TEXT_BACKGROUND	},
    { "*highlightColor",		CO_HIGHLIGHT		},
    { "*XmToggleButtonGadget*selectColor",
    						CO_SELECTCOLOR		},
    };

# define NUM_CO_RESOURCES   (sizeof defResourceCO / sizeof defResourceCO[0])


#ifdef VG_TRACE
    vg_TRACE_EXECUTION("SetResourceDatabase ...");
#endif /* VG_TRACE */

    /*
     *  check if user specified 100TH_MILLIMETERS as the unit type...
     *
     *  initialize the local control variable "appInfo.unitType" to the 
     *  unit type...
     */

    appInfo.unitType = XmPIXELS;
    
    if ( XrmGetResource(XtDatabase(dpyinfo.dpy),
    		   VnNmatteUnitType,  VnCMatteUnitType,
		   &rmtype, &rmvalue ) ) {
	
	if ( strcmp (rmvalue.addr, XmN100TH_MILLIMETERS) == 0 )
	    appInfo.unitType = Xm100TH_MILLIMETERS;
    }


    /*
     *  build new resource database with default values....
     */
     
    defDb = XtDatabase(dpyinfo.dpy);
    
    sprintf(defaultWidth,  "%d", FromMM(MATTE_WIDTH));
    sprintf(defaultHeight, "%d", FromMM(MATTE_HEIGHT));

	/*
    XrmPutStringResource(&defDb, VnNmatteWidth,  defaultWidth);
    XrmPutStringResource(&defDb, VnNmatteHeight, defaultHeight);
	*/

    if(!XrmGetResource(defDb, "*labelFont", "*LabelFont",
       &rmtype, &rmvalue))
        XrmPutStringResource(&defDb, "*labelFont",
    			     ( HIRES ? LABEL_FONT_HRES : LABEL_FONT));

    if(!XrmGetResource(defDb, "*textFont", "*TextFont",
       &rmtype, &rmvalue))
        XrmPutStringResource(&defDb, "*textFont",
			     ( LOWRES ? TEXT_FONT_LRES : TEXT_FONT));

    if(!XrmGetResource(defDb, "*greeting.fontList",
       "*Greeting.FontList", &rmtype, &rmvalue))
        XrmPutStringResource(&defDb, "*greeting.fontList", GREET_FONT);


    for (i = 0; i < NUM_RESOURCES; i++)
	XrmPutStringResource(&defDb,
	    		     defResource[i].name,
			     defResource[i].value);

    if ( dpyinfo.depth < 4 )
	for (i = 0; i < NUM_BW_RESOURCES; i++)
	    XrmPutStringResource(&defDb,
	    			 defResourceBW[i].name,
				 defResourceBW[i].value);
    else				 
	for (i = 0; i < NUM_CO_RESOURCES; i++)
	    XrmPutStringResource(&defDb,
	    			 defResourceCO[i].name,
				 defResourceCO[i].value);

    if ( dpyinfo.visual->class == GrayScale )
	for (i = 0; i < NUM_GS_RESOURCES; i++)
	    XrmPutStringResource(&defDb,
	    			 defResourceGS[i].name,
				 defResourceGS[i].value);

    

    /*
     *  merge user's specifications with program defaults...
     */


    /*
     *  replace display's resource database with merged database...
     */
     
     
    /*
     *  Try to compensate if the user specifies the matte dimensions in
     *  100TH_MILLIMETERS, but forgets to also specify that unit type...
     */

    userDb = XtDatabase(dpyinfo.dpy);

    if ( XrmGetResource(userDb, VnNmatteWidth, VnCMatteWidth,
		   &rmtype, &rmvalue ) ) {

	sscanf(rmvalue.addr, "%d", &i);
	if ( appInfo.unitType == XmPIXELS  &&  i >= dpyinfo.width ) {
	    XrmPutStringResource(&userDb, VnNmatteWidth, defaultWidth);
	    LogError(
		ReadCatalog(MC_LOG_SET,MC_LOG_BAD_MATTE,MC_DEF_LOG_BAD_MATTE),	
		dpyinfo.name);
	}
    }

    if ( XrmGetResource(userDb, VnNmatteHeight, VnCMatteHeight,
		   &rmtype, &rmvalue ) ) {

	sscanf(rmvalue.addr, "%d", &i);
	if ( appInfo.unitType == XmPIXELS  &&  i >= dpyinfo.height ) {
	    XrmPutStringResource(&userDb, VnNmatteHeight, defaultHeight);
	    LogError(
		ReadCatalog(MC_LOG_SET,MC_LOG_BAD_HMATTE,MC_DEF_LOG_BAD_HMATTE),
		dpyinfo.name);
	}
    }
    
}    




/***************************************************************************
 *
 *  ToPixel
 *
 *  Convert from the widget's units into pixel units
 ***************************************************************************/

int 
ToPixel( Widget w, int orientation, int pixel )
{
    Arg			argt[10];
    unsigned char	unit_type;
    
    XtSetArg(argt[0], XmNunitType, &unit_type);
    XtGetValues(w, argt, 1);
    
    if (unit_type == XmPIXELS)
	return(pixel);
    else {
	return(XmConvertUnits(w,
			      orientation,
			      (int)unit_type,
			      pixel,
			      XmPIXELS));
    }
}



#if 0
/***************************************************************************
 *
 *  _DtMessage Catalog Stubs
 *
 *  These stub routines can be used for porting to systems that do not yet
 *  support message catalogs. Replace the above "0" with an appropriate
 *  define for your target system. Do the same for the external defines
 *  in "vg.h".
 ***************************************************************************/

nl_catd
catopen(name, oflag)
  char	  *name;
  int	   oflag;

{
  return (555);
}

  
int
catclose(catd)
  nl_catd  catd;

{
  return;
}
  

char *
catgets(catd, set_num, msg_num, def_str)
  nl_catd  catd;
  int	   set_num;
  int	   msg_num;
  char	  *def_str;

{
    return (def_str);
}    

#endif
