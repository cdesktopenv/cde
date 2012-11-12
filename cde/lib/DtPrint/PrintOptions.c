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
/* $XConsortium: PrintOptions.c /main/4 1996/04/29 17:12:37 rswiston $ */

/*
 *+SNOTICE
 *
 *	$:$
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/* 
 *		     Common Desktop Environment
 *
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc.
 * (c) Copyright 1995 Digital Equipment Corp.
 * (c) Copyright 1995 Fujitsu Limited
 * (c) Copyright 1995 Hitachi, Ltd.
 *                                                                 
 *
 *                   RESTRICTED RIGHTS LEGEND                              
 *
 * Use, duplication, or disclosure by the U.S. Government is subject to
 * restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 * for non-DOD U.S. Government Departments and Agencies are as set forth in
 * FAR 52.227-19(c)(1,2).
 *
 * Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 * International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 * Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 * Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 * Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 * Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 * Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */
                                                                   
#include <Xm/XmAll.h>
#include "Dt/Print.h"
#include "PrintMsgsP.h"
#include "PrintOptionsP.h"


/******************************************************************************
 *
 * Static functions
 *
 ******************************************************************************/

static void
_DtPrintDestroyHdrFtrFrameCB(
	Widget		frame,
	XtPointer	client_data,
	XtPointer	call_data
	);
static void
_DtPrintDestroyMarginFrameCB(
	Widget		frame,
	XtPointer	client_data,
	XtPointer	call_data
	);

/******************************************************************************
 *
 * _DtPrint Header/Footer API
 *
 ******************************************************************************/
typedef struct dtpo_header_footer_spec
{
	Widget		button;
	void		*data;
	char		*string;
} DtpoHdrFtrSpec;

typedef struct dtpo_header_footer_frame
{
    Widget		frame,
			label,
			form,
			menu,
			top_left,
			top_right,
			bottom_left,
			bottom_right;
    
    int			nspecs;
    DtpoHdrFtrSpec	*specs;
} DtpoHdrFtrFrame;

static void
_DtPrintDestroyHdrFtrFrameCB(
	Widget		frame,
	XtPointer	client_data,
	XtPointer	call_data
	)
{
    DtpoHdrFtrFrame	*info = (DtpoHdrFtrFrame *) client_data;

    if (info == NULL) return;
    if (info->specs != NULL)
      XtFree((char*) info->specs);
    XtFree((char*) info);
}


/*
 * Function:  _DtPrintCreateHdrFtrFrame
 *
 * Creates a frame for specifying the contents of page headers and footers.
 * The frame contains four option menus, one for each _DtPrintHdrFtrEnum.
 * The pulldown menus for the option menu are constructed from the .string
 * field in the array of _DtPrintHdrFtrSpec in "specs".
 * 
 * _DtPrintCreateHdrFtrFrame copies the pointer to the array of
 * _DtPrintHdrFtrSpec.  IT DOES NOT COPY THE ELEMENTS OF THE ARRAY.
 *
 *	parent -	specifies the parent of the frame.
 *	nspecs -	specifies the number of entries in the specs array.
 *	specs -		array of header/footer specifiers.  The .string
 *			field is used to construct labels in the GUI.
 *			The .data field is returned in calls to
 *			_DtPrintGetHdrFtrSpec.
 *
 * Returns the widget id for the frame.
 */
Widget
_DtPrintCreateHdrFtrFrame(
	Widget		parent,
	int		nspecs,
	char		**spec_strings,
	void		**spec_data
	)
{
    DtpoHdrFtrFrame	*info;
    Arg		args[16];
    int		n;
    int		i;
    XmString	xms;

    info = (DtpoHdrFtrFrame *) XtMalloc( sizeof(DtpoHdrFtrFrame) );
    info->nspecs = nspecs;
    info->specs = (DtpoHdrFtrSpec *) XtMalloc(nspecs*sizeof(DtpoHdrFtrSpec));
    for (i=0; i<nspecs; i++)
    {
        info->specs[i].string = spec_strings[i];
	if (spec_data != (void*) NULL)
          info->specs[i].data = spec_data[i];
	else
	  info->specs[i].data = (void*) NULL;
	info->specs[i].button = (Widget) NULL;
    }


    /*
     * Create the frame and attach add a destroyCallback to clean up
     * memory allocated for this object.
     */
    n = 0;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNuserData, info); n++;
    info->frame = XmCreateFrame(parent, "HdrFtrFrame", args, n);
    XtAddCallback(
		info->frame,
		XmNdestroyCallback,
		_DtPrintDestroyHdrFtrFrameCB,
		(XtPointer) info
		);

    /*
     * Create a label child for the frame
     */
    xms = XmStringCreateLocalized(DTPO_HEADERFOOTER_FRAME_LABEL);
    n = 0;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    info->label = XmCreateLabel(info->frame, "HdrFtrLabel", args, n);
    XtManageChild(info->label);
    XmStringFree(xms);

    /*
     * Create a form work area child which will be populated by the
     * OptionMenu's.
     */
    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNfractionBase, 2); n++;
    info->form = XmCreateForm(info->frame, "HdrFtrForm", args, n);

    /*
     * Create the pulldown menu for the option menus.
     * Store the button index in the userData resource of each button.
     */
    info->menu = XmCreatePulldownMenu(parent, "HdrFtrMenu", NULL, 0);
    for (i=0; i<info->nspecs; i++)
    {
	static char	button_label[32];

	sprintf(button_label, "Button%d", i);
	xms = XmStringCreateLocalized(info->specs[i].string);
	n = 0;
	XtSetArg(args[n], XmNlabelString, xms); n++;
	XtSetArg(args[n], XmNuserData, i); n++;
	info->specs[i].button =
	    XmCreatePushButtonGadget(info->menu, button_label, args, n);
        XmStringFree(xms);
        XtManageChild(info->specs[i].button);
    }


    /*
     * Create the option menus using the menu created above.
     */
    xms = XmStringCreateLocalized(DTPO_HEADERFOOTER_FRAME_TOP_LEFT_LABEL);

    n=0;
    XtSetArg(args[n], XmNsubMenuId, info->menu); n++;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNuserData, info); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, 1); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;

    info->top_left = XmCreateOptionMenu(
				info->form,
				"PageHeaderLeftOM",
				args, n
				);
    XmStringFree(xms);
    XtManageChild(info->top_left);

    xms = XmStringCreateLocalized(DTPO_HEADERFOOTER_FRAME_TOP_RIGHT_LABEL);

    n=0;
    XtSetArg(args[n], XmNsubMenuId, info->menu); n++;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNuserData, info); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->top_left); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

    info->top_right = XmCreateOptionMenu(
				info->form,
				"PageHeaderRightOM",
				args, n
				);
    XmStringFree(xms);
    XtManageChild(info->top_right);

    xms = XmStringCreateLocalized(DTPO_HEADERFOOTER_FRAME_BOTTOM_LEFT_LABEL);

    n=0;
    XtSetArg(args[n], XmNsubMenuId, info->menu); n++;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNuserData, info); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->top_left); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->top_left); n++;
    XtSetArg(args[n], XmNrightOffset, 0); n++;

    info->bottom_left = XmCreateOptionMenu(
				info->form,
				"PageFooterLeftOM",
				args, n
				);
    XmStringFree(xms);
    XtManageChild(info->bottom_left);

    xms = XmStringCreateLocalized(DTPO_HEADERFOOTER_FRAME_BOTTOM_RIGHT_LABEL);

    n=0;
    XtSetArg(args[n], XmNsubMenuId, info->menu); n++;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNuserData, info); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->bottom_left); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->top_right); n++;
    XtSetArg(args[n], XmNrightOffset, 0); n++;

    info->bottom_right = XmCreateOptionMenu(
				info->form,
				"PageFooterRightOM",
				args, n
				);
    XmStringFree(xms);
    XtManageChild(info->bottom_right);

    XtManageChild(info->form);
    return info->frame;
}

/*
 * Function:  _DtPrintHdrFtrFrameEnumToWidget
 *
 * Returns the widget id of the option menu corresponding the specified
 * header or footer.
 *
 *	frame -	specifies the HdrFtr frame
 *	which -	specifies the particular header or footer in "frame".
 *
 * Returns the widget id of the option menu for "which"
 */
Widget
_DtPrintHdrFtrFrameEnumToWidget(
	Widget			frame,
	_DtPrintHdrFtrEnum	which
	)
{
    DtpoHdrFtrFrame	*info;
    Widget		widget = NULL;

    XtVaGetValues(frame, XmNuserData, &info, NULL);
    if (info == NULL)
      return widget;

    switch (which)
    {
	case DTPRINT_OPTION_HEADER_LEFT:
	  widget = info->top_left; break;
	case DTPRINT_OPTION_HEADER_RIGHT:
	  widget = info->top_right; break;
	case DTPRINT_OPTION_FOOTER_LEFT:
	  widget = info->bottom_left; break;
	case DTPRINT_OPTION_FOOTER_RIGHT:
	  widget = info->bottom_right; break;
	default:
	  widget = NULL;
    }

    return widget;
}

/*
 * Function:  _DtPrintHdrFtrFrameMenuWidgets
 *
 *	widget -specifies the option menu
 *
 * Returns the widgets which make up the pulldown menu.
 */
void
_DtPrintHdrFtrFrameMenuWidgets(
	Widget			frame,
	Widget			*menu,
	int			*nmenu_buttons,
	Widget			**menu_buttons
	)
{
    int			i;
    DtpoHdrFtrFrame	*info;

    if (frame == NULL) return;

    XtVaGetValues(frame, XmNuserData, &info, NULL);
    if (info == NULL) return;

    if (menu != NULL)
      *menu = info->menu;

    if (nmenu_buttons != NULL)
      *nmenu_buttons = info->nspecs;

    if (menu_buttons != NULL)
    {
	if (*menu_buttons == NULL)
          *menu_buttons = (Widget *) XtMalloc( info->nspecs * sizeof(Widget) );
    
        for (i=0; i<info->nspecs; i++)
	  (*menu_buttons)[i] = info->specs[i].button;
    }
}

/*
 * Function:  _DtPrintGetHdrFtrIndex
 *
 *	option -	specifies the HdrFtr option menu
 *
 * Returns the index of the active menu option of the specified
 * option menu. 
 */
int
_DtPrintGetHdrFtrIndex(
	Widget			option
	)
{
    int		index = -1;
    Widget	selected;

    if (! option) return index;

    XtVaGetValues(option, XmNmenuHistory, &selected, NULL);
    XtVaGetValues(selected, XmNuserData, &index, NULL);
    return index;
}

/*
 * Function:  _DtPrintGetHdrFtrData
 *
 *	option -	specifies the HdrFtr option menu
 *
 * Returns the data associated with the active menu option of the specified
 * option menu. 
 */
void *
_DtPrintGetHdrFtrData(
	Widget			option
	)
{
    int			index = 0;
    DtpoHdrFtrFrame	*info = NULL;

    if (! option) return (void*) NULL;
    XtVaGetValues(option, XmNuserData, &info, NULL);
    if (info == NULL) return;

    index = _DtPrintGetHdrFtrIndex(option);
    return info->specs[index].data;
}

/*
 * Function:  _DtPrintGetHdrFtrString
 *
 *	option -	specifies the HdrFtr option menu
 *
 * Returns the string associated with the active menu option of the specified
 * option menu. 
 */
char *
_DtPrintGetHdrFtrString(
	Widget			option
	)
{
    int			index = 0;
    DtpoHdrFtrFrame	*info = NULL;

    if (! option) return (void*) NULL;
    XtVaGetValues(option, XmNuserData, &info, NULL);
    if (info == NULL) return;

    index = _DtPrintGetHdrFtrIndex(option);
    return info->specs[index].string;
}

/*
 * Function:  _DtPrintSetHdrFtrByData
 *
 * Displays the specified menu item as the currently active option.
 *
 *	option -	specifies the HdrFtr option menu
 *	data -		data associated with the option to be displayed.
 */
void
_DtPrintSetHdrFtrByData(
	Widget			option,
	void			*data
	)
{
    int			i;
    DtpoHdrFtrFrame	*info;

    if (option == NULL) return;
    XtVaGetValues(option, XmNuserData, &info, NULL);
    if (info == NULL) return;

    for (i=0; i<info->nspecs; i++)
      if (info->specs[i].data == data)
      {
          XtVaSetValues(option, XmNmenuHistory, info->specs[i].button, NULL);
	  return;
      }
}

/*
 * Function:  _DtPrintSetHdrFtrByIndex
 *
 * Displays the specified menu item as the currently active option.
 *
 *	option -	specifies the HdrFtr option menu
 *	index -		specifies the option to be displayed.
 */
void
_DtPrintSetHdrFtrByIndex(
	Widget			option,
	int			index
	)
{
    DtpoHdrFtrFrame	*info;

    if (option == NULL) return;
    XtVaGetValues(option, XmNuserData, &info, NULL);
    if (info == NULL) return;

    XtVaSetValues(option, XmNmenuHistory, info->specs[index].button, NULL);
}

/*
 * Function:  _DtPrintSetHdrFtrByString
 *
 * Displays the specified menu item as the currently active option.
 *
 *	option -	specifies the HdrFtr option menu
 *	string -	string associated with the option to be displayed.
 */
void
_DtPrintSetHdrFtrByString(
	Widget			option,
	char			*string
	)
{
    int			i;
    DtpoHdrFtrFrame	*info;

    if (option == NULL) return;
    XtVaGetValues(option, XmNuserData, &info, NULL);
    if (info == NULL) return;

    for (i=0; i<info->nspecs; i++)
      if (strcmp(info->specs[i].string,string) == 0)
      {
          XtVaSetValues(option, XmNmenuHistory, info->specs[i].button, NULL);
	  return;
      }
}



/******************************************************************************
 *
 * _DtPrint Margin API
 *
 ******************************************************************************/
typedef struct dtpo_margin_frame
{
    Widget	frame,
		label,
		form,

		top,
		right,
		bottom,
		left,

		top_label,
		right_label,
		bottom_label,
		left_label;
} DtpoMarginFrame;

static void
_DtPrintDestroyMarginFrameCB(
	Widget		frame,
	XtPointer	client_data,
	XtPointer	call_data
	)
{
    DtpoHdrFtrFrame	*info = (DtpoHdrFtrFrame *) client_data;

    if (info == NULL) return;
    XtFree((char*) info);
}

/*
 * Function:  _DtPrintCreateMarginFrame
 *
 * Creates a frame for specifying margins.  The frame contains four text
 * fields, one for each _DtPrintMarginEnum.
 *
 *	parent -	specifies the parent of the frame.
 *
 * Returns the widget id of the frame.
 */
Widget
_DtPrintCreateMarginFrame(
	Widget			parent
	)
{
    DtpoMarginFrame
		*info;
    Arg		args[16];
    int		n;
    XmString	xms;

    info = (DtpoMarginFrame *) XtMalloc( sizeof(DtpoMarginFrame) );

    n = 0;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNuserData, info); n++;
    info->frame = XmCreateFrame(parent, "MarginFrame", args, n);
    XtAddCallback(
		info->frame,
		XmNdestroyCallback,
		_DtPrintDestroyMarginFrameCB,
		(XtPointer) info
		);

    xms = XmStringCreateLocalized(DTPO_MARGIN_FRAME_LABEL);
    n = 0;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    info->label = XmCreateLabel(info->frame, "MarginLabel", args, n);
    XtManageChild(info->label);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNfractionBase, 2); n++;
    info->form = XmCreateForm(info->frame, "MarginForm", args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, 1); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    info->top = XmCreateTextField(info->form, "MarginTopText", args, n);
    XtManageChild(info->top);

    xms = XmStringCreateLocalized(DTPO_MARGIN_FRAME_TOP_MARGIN_LABEL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->top); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->top); n++;
    XtSetArg(args[n], XmNrightOffset, 3); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, info->top); n++;
    XtSetArg(args[n], XmNbottomOffset, 0); n++;
    info->top_label =
      XmCreateLabelGadget(info->form, "MarginTopLabel", args, n);
    XmStringFree(xms);
    XtManageChild(info->top_label);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->top); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    info->right = XmCreateTextField(info->form, "MarginRightText", args, n);
    XtManageChild(info->right);

    xms = XmStringCreateLocalized(DTPO_MARGIN_FRAME_RIGHT_MARGIN_LABEL);
    n = 0;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->right); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->right); n++;
    XtSetArg(args[n], XmNrightOffset, 3); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, info->right); n++;
    XtSetArg(args[n], XmNbottomOffset, 0); n++;
    info->right_label =
      XmCreateLabelGadget(info->form, "MarginRightLabel", args, n);
    XmStringFree(xms);
    XtManageChild(info->right_label);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->top); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->top); n++;
    XtSetArg(args[n], XmNrightOffset, 0); n++;
    info->bottom = XmCreateTextField(info->form, "MarginBottomText", args, n);
    XtManageChild(info->bottom);

    xms = XmStringCreateLocalized(DTPO_MARGIN_FRAME_BOTTOM_MARGIN_LABEL);
    n = 0;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->bottom); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->bottom); n++;
    XtSetArg(args[n], XmNrightOffset, 3); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, info->bottom); n++;
    XtSetArg(args[n], XmNbottomOffset, 0); n++;
    info->bottom_label =
      XmCreateLabelGadget(info->form, "MarginBottomLabel", args, n);
    XmStringFree(xms);
    XtManageChild(info->bottom_label);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->bottom); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->right); n++;
    XtSetArg(args[n], XmNrightOffset, 0); n++;
    info->left = XmCreateTextField(info->form, "MarginLeftText", args, n);
    XtManageChild(info->left);

    xms = XmStringCreateLocalized(DTPO_MARGIN_FRAME_LEFT_MARGIN_LABEL);
    n = 0;
    XtSetArg(args[n], XmNlabelString, xms); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, info->left); n++;
    XtSetArg(args[n], XmNtopOffset, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, info->left); n++;
    XtSetArg(args[n], XmNrightOffset, 3); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget, info->left); n++;
    XtSetArg(args[n], XmNbottomOffset, 0); n++;
    info->left_label =
      XmCreateLabelGadget(info->form, "MarginLeftLabel", args, n);
    XmStringFree(xms);
    XtManageChild(info->left_label);

    XtManageChild(info->form);
    return info->frame;
}

/*
 * Function:  _DtPrintMarginFrameEnumToWidget
 *
 *	frame -	specifies the HdrFtr frame
 *	which -	specifies the particular margin in "frame".
 *
 * Returns the widget id of the textfield for "which"
 */
Widget
_DtPrintMarginFrameEnumToWidget(
	Widget			frame,
	_DtPrintMarginEnum	which
	)
{
    DtpoMarginFrame	*info;
    Widget		widget = NULL;

    XtVaGetValues(frame, XmNuserData, &info, NULL);
    if (info == NULL)
      return widget;

    switch (which)
    {
	case DTPRINT_OPTION_MARGIN_TOP:
	  widget = info->top; break;
	case DTPRINT_OPTION_MARGIN_RIGHT:
	  widget = info->right; break;
	case DTPRINT_OPTION_MARGIN_BOTTOM:
	  widget = info->bottom; break;
	case DTPRINT_OPTION_MARGIN_LEFT:
	  widget = info->left; break;
	default:
	  break;
    }

    return widget;
}

/*
 * Function:  _DtPrintGetMarginSpec
 *
 *	margin -	specifies the margin widget.
 *
 * Returns a copy of the contents of the specified margin.
 * The calling function should free this string when done with it.
 */
char*
_DtPrintGetMarginSpec(
	Widget			margin
	)
{
    char		*spec = NULL;

    if (margin)
	spec = XmTextFieldGetString(margin);

    return spec;
}

/*
 * Function:  _DtPrintSetMarginSpec
 *
 *	margin -specifies the margin widget.
 *	spec  - specifies the string to be displayed in the GUI.
 *
 */
void
_DtPrintSetMarginSpec(
	Widget			margin,
	char			*spec
	)
{
    if (margin)
      XmTextFieldSetString(margin, spec);
}
