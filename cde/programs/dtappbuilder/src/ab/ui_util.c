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
 *      $XConsortium: ui_util.c /main/3 1995/11/06 17:56:30 rswiston $
 *
 * @(#)ui_util.c	1.38 14 Feb 1994      cde_app_builder/src/ab
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 ***********************************************************************
 * ui_util.c - User-Interface support functions
 *	
 *
 ***********************************************************************
 */
#ifndef _POSIX_SOURCE	/* POSIX guarantees portability of time functions */
#define _POSIX_SOURCE 1
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <X11/Composite.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/XmAll.h>
/*
#include <Xm/Protocols.h>
#include <Xm/CascadeB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
*/
#include <Dt/SpinBox.h>
#include <Dt/TermPrim.h>
#include <Dt/Term.h>
#include <ab_private/XmAll.h>
#include <ab_private/x_util.h>
#include <ab_private/ui_util.h>
#include <ab_private/objxm.h>
#include <ab_private/ab.h>
#include <Dt/xpm.h>		/* will this be in include/Dt? */
#include "dtbuilder.h"
#include "dtb_utils.h"

extern Widget	AB_toplevel;

typedef struct
{
    XtIntervalId	timerId;
    BOOL		synced;
    time_t		start_time;
    long		timeout_ticks;
    time_t		last_expose_ticks;
    Display		*display;
    Window		window;
} SyncDataRec, *SyncData;


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void	init_obj_pixmaps();

static void	rubberband_finish(
		    Widget	widget,
		    XEvent	*event,
		    XtPointer	client_data
		);

static void	rubberband_release(
		    Widget widget,
		    XtPointer client_data,
		    XEvent *event,
		    Boolean *cont_dispatch
		);

static void	rubberband_draw(
		    Widget	widget,
		    XEvent      *event,
		    XtPointer	client_data
		);

/*************************************************************************
**                                                                      **
**       Data				                                **
**                                                                      **
**************************************************************************/

static const long sync_notify_value = (long)0x45a55a54;


/*
 * Array that maps from:
 *	(obj type, obj subtype) -> pixmap
 * It contains the data that will be used to create the pixmap.
 */
static UiObjPixmap	object_pixmaps[] = 
{
    /* { (AB_OBJECT_TYPE), (int), (char *), (Pixmap), u_int, u_int } */
    {AB_TYPE_MODULE, AB_NO_SUBTYPE, "DtABmdl", (Pixmap)NULL, 0, 0},

    {AB_TYPE_BASE_WINDOW, AB_NO_SUBTYPE, "DtABbw2", (Pixmap)NULL, 0, 0},

    {AB_TYPE_DIALOG, AB_NO_SUBTYPE, "DtABpuw2", (Pixmap)NULL, 0, 0},

    {AB_TYPE_FILE_CHOOSER, AB_NO_SUBTYPE, "DtABfsb2", (Pixmap)NULL, 0, 0},

    {AB_TYPE_CONTAINER, AB_CONT_BUTTON_PANEL, "DtABcnt2", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_ABSOLUTE, "DtABcnt2", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_RELATIVE, "DtABcnt2", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_PANED, "DtABpnw", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_MENU_BAR, "DtABmbr", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_TOOL_BAR, "DtABcnt2", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_FOOTER, "DtABcnt2", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CONTAINER, AB_CONT_GROUP, "DtABgrp", (Pixmap)NULL, 0, 0},

    {AB_TYPE_DRAWING_AREA, AB_NO_SUBTYPE, "DtABdrw2", (Pixmap)NULL, 0, 0},

    {AB_TYPE_TEXT_PANE, AB_NO_SUBTYPE, "DtABtxp2", (Pixmap)NULL, 0, 0},

    {AB_TYPE_BUTTON, AB_BUT_PUSH, "DtABbtn", (Pixmap)NULL, 0, 0},
    {AB_TYPE_BUTTON, AB_BUT_DRAWN, "DtABbtn", (Pixmap)NULL, 0, 0},
    {AB_TYPE_BUTTON, AB_BUT_MENU, "DtABmbt", (Pixmap)NULL, 0, 0},

    {AB_TYPE_LIST, AB_NO_SUBTYPE, "DtABlst", (Pixmap)NULL, 0, 0},

    {AB_TYPE_CHOICE, AB_CHOICE_OPTION_MENU, "DtABopm", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CHOICE, AB_CHOICE_EXCLUSIVE, "DtABrad", (Pixmap)NULL, 0, 0},
    {AB_TYPE_CHOICE, AB_CHOICE_NONEXCLUSIVE, "DtABchk", (Pixmap)NULL, 0, 0},

    {AB_TYPE_ITEM, AB_ITEM_FOR_MENU, "DtABcas", (Pixmap)NULL, 0, 0},
    {AB_TYPE_ITEM, AB_ITEM_FOR_MENUBAR, "DtABcas", (Pixmap)NULL, 0, 0},
    {AB_TYPE_ITEM, AB_ITEM_FOR_CHOICE, "DtABitm", (Pixmap)NULL, 0, 0},
    {AB_TYPE_ITEM, AB_ITEM_FOR_LIST, "DtABitm", (Pixmap)NULL, 0, 0},
    {AB_TYPE_ITEM, AB_ITEM_FOR_COMBO_BOX, "DtABitm", (Pixmap)NULL, 0, 0},
    {AB_TYPE_ITEM, AB_ITEM_FOR_SPIN_BOX, "DtABitm", (Pixmap)NULL, 0, 0},

    {AB_TYPE_TEXT_FIELD, AB_NO_SUBTYPE, "DtABtxf", (Pixmap)NULL, 0, 0},

    {AB_TYPE_MENU, AB_MENU_PULLDOWN, "DtABpum", (Pixmap)NULL, 0, 0},
    {AB_TYPE_SCALE, AB_SCALE_SCALE, "DtABsld", (Pixmap)NULL, 0, 0},
    {AB_TYPE_SCALE, AB_SCALE_GAUGE, "DtABgau", (Pixmap)NULL, 0, 0}, 
    {AB_TYPE_COMBO_BOX, AB_NO_SUBTYPE, "DtABcmb", (Pixmap)NULL, 0, 0},
    {AB_TYPE_SPIN_BOX, AB_NO_SUBTYPE, "DtABspb", (Pixmap)NULL, 0, 0},
    {AB_TYPE_TERM_PANE, AB_NO_SUBTYPE, "DtABtmp2", (Pixmap)NULL, 0, 0},
    {AB_TYPE_SEPARATOR, AB_NO_SUBTYPE, "DtABsep", (Pixmap)NULL, 0, 0},
    {AB_TYPE_LABEL, AB_NO_SUBTYPE, "DtABlbl", (Pixmap)NULL, 0, 0},

    {AB_TYPE_MESSAGE, AB_MSG_ERROR, "DtABmbx", (Pixmap)NULL, 0, 0},
    {AB_TYPE_MESSAGE, AB_MSG_INFORMATION, "DtABmbx", (Pixmap)NULL, 0, 0},
    {AB_TYPE_MESSAGE, AB_MSG_QUESTION, "DtABmbx", (Pixmap)NULL, 0, 0},
    {AB_TYPE_MESSAGE, AB_MSG_WARNING, "DtABmbx", (Pixmap)NULL, 0, 0},
    {AB_TYPE_MESSAGE, AB_MSG_WORKING, "DtABmbx", (Pixmap)NULL, 0, 0},

    {AB_TYPE_LAYERS, AB_NO_SUBTYPE, "DtABlyr", (Pixmap)NULL, 0, 0},

    /* The last entry has to be this !!*/
    {AB_TYPE_UNKNOWN, AB_NO_SUBTYPE, (char *)NULL, (Pixmap)NULL, 0, 0},
};

/*
 * Default pixmap
 */
static	Pixmap		default_pixmap = NULL;
static	unsigned int	default_pixmap_width = 0;
static	unsigned int	default_pixmap_height = 0;

/*
 * State variables for rubber banding
 */
static Boolean		rband_in_progress= False;
static Boolean		just_rbanded	  = False;
static XRectangle	rb_rect;
static Widget		rb_widget = NULL;
static BOOL		rb_first_time = TRUE;
static UiRubberBandFunc	rubberband_func = NULL;

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
void
ui_win_front(
    Widget widget
)
{
    Widget p_shell;

    if (widget == NULL)
	return;

    p_shell = ui_get_ancestor_shell(widget);

    if (XtIsRealized(p_shell))
        XRaiseWindow(XtDisplay(p_shell),XtWindow(p_shell));
    else
	if (util_get_verbosity() > 0)
	    fprintf(stderr,"ui_win_front: widget not realized\n");

}

void
ui_win_show(
    Widget	widget,
    BOOL	show,
    XtGrabKind  grab_kind
)
{
    Widget 	shell;
    Widget	dialog;

    shell = ui_get_ancestor_shell(widget);

    if (show)
	XtPopup(shell, grab_kind);
    else
	XtPopdown(shell);
}

void
ui_win_set_resizable(
    Widget	widget,
    BOOL	resizable,
    BOOL	remap
)
{
    Widget 	shell;
    int 	decor = 0;
    int		new_decor = 0;
    int 	func = 0;
    int		new_func = 0;

    shell = ui_get_ancestor_shell(widget);

    XtVaGetValues(shell, 
	XmNmwmDecorations, &decor, 
	XmNmwmFunctions,   &func,
	NULL);

    if (func & MWM_FUNC_ALL)
    {
	new_func |= MWM_FUNC_ALL;
	if (!resizable)
	    new_func |= MWM_FUNC_RESIZE;
    }
    else
    {
	new_func = func;
	if (resizable && !(func & MWM_FUNC_RESIZE))
	    new_func |= MWM_FUNC_RESIZE;
        else if (!resizable && (func & MWM_FUNC_RESIZE))
	    new_func &= ~MWM_FUNC_RESIZE;
    }

    if (decor & MWM_DECOR_ALL)
    {
	new_decor |= MWM_DECOR_ALL;
	if (!resizable)
	    new_decor |= MWM_DECOR_RESIZEH;
    }
    else
    {
        new_decor = decor;
        if (resizable && !(decor & MWM_DECOR_RESIZEH))
            new_decor |= MWM_DECOR_RESIZEH;
        else if (!resizable && (decor & MWM_DECOR_RESIZEH)) 
            new_decor &= ~MWM_DECOR_RESIZEH; 
    }

    if (new_func != func || new_decor != decor)
    {
	/* If requested, unmap & remap the window.
	 * Unfortunately, this is the only way to get the window
	 * manager to change the decorations for an already mapped
	 * window.
	 */
	if (remap)
    	    XtPopdown(shell);

    	XtVaSetValues(shell, 
		XmNmwmDecorations, new_decor, 
		XmNmwmFunctions,   new_func,
		NULL);

	if (remap)
    	    XtPopup(shell, XtGrabNone);
    }
}
	    
void
ui_field_set_string(
    Widget   	field,
    STRING	valuestr
)
{

    XtVaSetValues(field,
	XmNvalue, 	    (XtArgVal)(valuestr? valuestr : ""),
	XmNcursorPosition,  (XtArgVal)valuestr? strlen(valuestr) : 0,
	NULL);

}

STRING
ui_field_get_string(
    Widget   field
)
{
     char *string = NULL;

     if (XtIsSubclass(field, xmTextFieldWidgetClass))
     	string = XmTextFieldGetString(field);
     else if (XtIsSubclass(field, xmTextWidgetClass))
	string = XmTextGetString(field);

     return((STRING)string);
}

void
ui_field_select_string(
    Widget	field,
    BOOL	assign_focus
)
{
    STRING	valuestr = NULL;

    valuestr = ui_field_get_string(field);

    if (XtIsSubclass(field, xmTextFieldWidgetClass))
    	XmTextFieldSetSelection(field, 0, strlen(valuestr), CurrentTime);
    else if (XtIsSubclass(field, xmTextWidgetClass))
	XmTextSetSelection(field, 0, strlen(valuestr), CurrentTime);

    if (assign_focus) 
        XmProcessTraversal(field, XmTRAVERSE_CURRENT);

    util_free(valuestr);
}

void
ui_field_set_editable(
    Widget	field,
    BOOL	editable
)
{
    XtVaSetValues(field,
                XmNeditable,              editable,
                XmNcursorPositionVisible, editable,
		NULL);
}

void
ui_set_active(
    Widget	widget,
    BOOL	state
)
{
    XtSetSensitive(widget, (Boolean)state);

    if (XtIsComposite(widget))
    {
	int i, num_children = 0;
	WidgetList children = NULL;

	XtVaGetValues(widget, 
		XmNnumChildren,	&num_children,
		XmNchildren,	&children,
		NULL);

	for (i = 0; i < num_children; i++)
	{
            int c = 0, num_children_children = 0;
            WidgetList children_children = NULL;

	    XtSetSensitive(children[i], (Boolean)state);

	    /* REMIND: Hack to get scrolling list to grey out */

	    XtVaGetValues(children[i],
                XmNnumChildren, &num_children_children,
                XmNchildren,    &children_children,
                NULL);

	    if (num_children_children > 0)
	    {
		for (c = 0; c < num_children_children; c++)
		    XtSetSensitive(children_children[c], (Boolean)state);
	    }
	}
    }
}

void
ui_set_visible(
    Widget	widget,
    BOOL	viz
)
{
    if (viz == TRUE && !XtIsManaged(widget))
	XtManageChild(widget);

    else if (viz == FALSE && XtIsManaged(widget))
	XtUnmanageChild(widget);

}

void
ui_set_label_string(
    Widget      widget,
    STRING      string
)
{
    XmString    xmlabel;

    xmlabel = XmStringCreateLocalized(string);

    XtVaSetValues(widget, XmNlabelString, xmlabel, NULL);
 
    XmStringFree(xmlabel);
}

void
ui_set_label_glyph(
    Widget      widget,
    STRING      fileName
)
{
    Pixmap    labelPixmap            = NULL;
    Pixmap    labelInsensitivePixmap = NULL;
    
    XtVaGetValues(widget,
	XmNlabelPixmap,            &labelPixmap,
	XmNlabelInsensitivePixmap, &labelInsensitivePixmap,
	NULL);

    if (dtb_set_label_from_image_file(widget, fileName) < 0)
	return;
    
    /* sucess, destroy the old pixmaps */
    if (labelPixmap)
	XmDestroyPixmap(XtScreen(widget), labelPixmap);

    if (labelInsensitivePixmap)
	XmDestroyPixmap(XtScreen(widget), labelInsensitivePixmap);
}


Widget
ui_get_ancestor_shell(
    Widget	widget
)
{
    Widget shell = widget;

    while(shell && !XtIsSubclass(shell, shellWidgetClass))
        shell = XtParent(shell);

    return shell;

}

Widget
ui_get_ancestor_dialog(
    Widget	widget
)
{
    Widget dialog = widget;

    while(dialog && !XtIsSubclass(XtParent(dialog), shellWidgetClass))
	dialog = XtParent(dialog);

    return dialog;
}

Widget
ui_build_menu(
	Widget		parent,
	int		menu_type,
	int		num_columns,
	char		*menu_title,
	char		*menu_name,
	MenuItem	*menu_items
)
{
    Widget	menu,
   		cascade,
		widget;
    Arg		args[4];
    int 	i;
    XmString	xmlabel;
    int		n = 0;

    if (num_columns > 1)
    {
    	XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);    n++;
    	XtSetArg(args[n], XmNnumColumns, num_columns);   n++;
    }
    if (menu_type == XmMENU_PULLDOWN)
	menu = XmCreatePulldownMenu(parent, "pulldown_menu", args, n);
    else
    {
	XtSetArg(args[n], XmNwhichButton, AB_BMenu); n++;
        menu = XmCreatePopupMenu(parent, "popup_menu", args, n);
    }

    if (menu_type == XmMENU_PULLDOWN)
    {
        cascade = XtVaCreateManagedWidget(menu_name,
			xmCascadeButtonWidgetClass, parent,
			XmNsubMenuId,	menu,
			NULL);

	if (menu_title)
        {
	    xmlabel = XmStringCreateLocalized(menu_title);
	    XtVaSetValues(cascade, XmNlabelString, xmlabel, NULL);
	    XmStringFree(xmlabel);
	}
    }

    for (i = 0; menu_items[i].label != NULL; i++)
    {
        if (menu_items[i].subitems)
	{
	    widget = ui_build_menu(menu, XmMENU_PULLDOWN, 
				       menu_items[i].num_columns,
			               menu_items[i].label, 
				       menu_items[i].name,
				       (MenuItem *)menu_items[i].subitems);

	    XtVaSetValues(widget, XmNuserData, (XtArgVal)menu_items[i].user_data, NULL);
	}
	else
	{
	    widget = XtVaCreateManagedWidget(menu_items[i].name,
			         *menu_items[i].wclass, 
				  menu, 
                                  XmNuserData,    (XtArgVal)menu_items[i].user_data,
				  NULL);

	    if (menu_items[i].label != NULL)
	    {
		xmlabel = XmStringCreateLocalized(menu_items[i].label);
	  	XtVaSetValues(widget, XmNlabelString, xmlabel, NULL);
	    	XmStringFree(xmlabel);
	    }
	
	    /* If label is glyph type, then change type and call
	     * routine to set glyph.
	     */
	    if (menu_items[i].label_type == AB_LABEL_GLYPH)
	    {
        	XtVaSetValues(widget, XmNlabelType, XmPIXMAP, NULL);
		dtb_set_label_from_bitmap_data(widget,
				menu_items[i].pixwidth,
				menu_items[i].pixheight,
				menu_items[i].bits);

	    }
	}

	if (menu_items[i].active == FALSE)
	    XtSetSensitive(widget, FALSE);
 	else if (menu_items[i].callback != NULL)
	    XtAddCallback(widget, XmNactivateCallback, 
				menu_items[i].callback, 
				menu_items[i].client_data);
   }

   if (menu_type == XmMENU_POPUP)
	return menu;
   else
	return cascade;
}

void
ui_populate_pulldown_menu(
	Widget		menu,
	int		num_columns,
	MenuItem	*menu_items
)
{
    Widget	widget;
    Arg		args[4];
    int 	i;
    XmString	xmlabel;
    int		n = 0;

    if (!menu)
	return;

    if (num_columns > 1)
    {
    	XtSetArg(args[n], XmNpacking, XmPACK_COLUMN);    n++;
    	XtSetArg(args[n], XmNnumColumns, num_columns);   n++;

	XtSetValues(menu, args, n);
    }

    for (i = 0; menu_items[i].label != NULL; i++)
    {
        if (menu_items[i].subitems)
	{
	    widget = ui_build_menu(menu, XmMENU_PULLDOWN, 
				       menu_items[i].num_columns,
			               menu_items[i].label, 
				       menu_items[i].name,
				       (MenuItem *)menu_items[i].subitems);

	    XtVaSetValues(widget, XmNuserData, (XtArgVal)menu_items[i].user_data, NULL);
	}
	else
	{
	    widget = XtVaCreateManagedWidget(menu_items[i].name,
			         *menu_items[i].wclass, 
				  menu, 
                                  XmNuserData,    (XtArgVal)menu_items[i].user_data,
				  NULL);

	    if (menu_items[i].label != NULL)
	    {
		xmlabel = XmStringCreateLocalized(menu_items[i].label);
	  	XtVaSetValues(widget, XmNlabelString, xmlabel, NULL);
	    	XmStringFree(xmlabel);
	    }
	
	    /* If label is glyph type, then change type and call
	     * routine to set glyph.
	     */
	    if (menu_items[i].label_type == AB_LABEL_GLYPH)
	    {
        	XtVaSetValues(widget, XmNlabelType, XmPIXMAP, NULL);
		dtb_set_label_from_bitmap_data(widget,
				menu_items[i].pixwidth,
				menu_items[i].pixheight,
				menu_items[i].bits);

	    }
	}

	if (menu_items[i].active == FALSE)
	    XtSetSensitive(widget, FALSE);
 	else if (menu_items[i].callback != NULL)
	    XtAddCallback(widget, XmNactivateCallback, 
				menu_items[i].callback, 
				menu_items[i].client_data);
   }

}


void
ui_size_to_row_col(
    Widget	   text,
    unsigned short width,
    unsigned short height,
    int		   *row_ptr,
    int		   *col_ptr
)
{

    Widget	parent;
    Widget	vsb, hsb;
    Dimension	spacing    = 0;
    Dimension   text_spacing = 0;
    Dimension 	hsb_height = 0;
    Dimension	vsb_width  = 0;
    Dimension	margin_w = 0;
    Dimension	margin_h = 0;
    Dimension   p_margin_w = 0;
    Dimension   p_margin_h = 0;
    XmFontList  fontlist;
    XFontStruct  *font;
    unsigned long charwidth;
    unsigned long lineheight;
    Dimension    pane_width, pane_height;

    if (XtIsSubclass(text, dtTermWidgetClass))
    	XtVaGetValues(text,
                XmNmarginWidth, &margin_w,
                XmNmarginHeight,&margin_h,
		DtNuserFont,	&fontlist,
                NULL);
    else
        XtVaGetValues(text,
                XmNmarginWidth, &margin_w,
                XmNmarginHeight,&margin_h,
                XmNfontList,    &fontlist, 
		XmNlistSpacing, &text_spacing,
                NULL);

    parent = XtParent(text);

    if (XtIsSubclass(parent, xmScrolledWindowWidgetClass))
    {
	XtVaGetValues(parent,
		XmNhorizontalScrollBar,	&hsb,
		XmNverticalScrollBar,	&vsb,
		XmNspacing,		&spacing,
		NULL);
	if (hsb)
	    XtVaGetValues(hsb,
		XmNheight,		&hsb_height,
		NULL);
	if (vsb)
	    XtVaGetValues(vsb,
		XmNwidth,       	&vsb_width,
                NULL); 
    }
    else if (XtIsSubclass(parent, xmRowColumnWidgetClass))
	XtVaGetValues(parent,
		XmNmarginWidth, 	&p_margin_w,
		XmNmarginHeight,	&p_margin_h,
		NULL);

    font = objxm_fontlist_to_font(fontlist);

    if ((!XGetFontProperty(font, XA_QUAD_WIDTH, &charwidth)) || charwidth == 0) {
       if (font->per_char && font->min_char_or_byte2 <= '0' &&
                                 font->max_char_or_byte2 >= '0')
           charwidth = font->per_char['0' - font->min_char_or_byte2].width;
       else
           charwidth = font->max_bounds.width;
    }
    lineheight = font->max_bounds.ascent + font->max_bounds.descent +
			text_spacing;

    /* Calculate new pane size */
    pane_width  = width  - (vsb_width  + spacing) - (2*p_margin_w);
    pane_height = height - (hsb_height + spacing) - (2*p_margin_h);

    *row_ptr = (int)((pane_height - (2*margin_h))/lineheight);
    *col_ptr = (int)((pane_width  - (2*margin_w))/charwidth);

    /* For some reason, above calculations result in rows being 1 too
     * large for a scrolled list widget; put in workaround until
     * error in calculations is found...
     */
/*
 REMIND: versions of Motif beyond August 10 don't need this
    if (XtIsSubclass(text, xmListWidgetClass) && *row_ptr > 1)
	(*row_ptr)--;
*/

}

int
ui_set_busy_cursor(
    Window	window,
    BOOL	on
)
{
    static Cursor  busy_cursor = NULL;
    static Display *dpy        = NULL;

    if (on) /* Turn ON busy cursor */
    {
	dpy = XtDisplay(AB_toplevel);

	if (busy_cursor == NULL)
	    busy_cursor = XCreateFontCursor(dpy, XC_watch);

        XDefineCursor(dpy, window, busy_cursor);
    }
    else if (dpy != NULL) /* Turn OFF busy cursor */
    {
	XUndefineCursor(dpy, window);
	dpy = NULL;
    }
    return 0;

}

/*
 * init_obj_pixmaps()
 * Initialize object pixmaps
 */
static void
init_obj_pixmaps
(
)
{
    Display		*dpy;
    Pixmap		tmp;
    int			i, 
			x, 
			y,
    			status;
    unsigned int	w, h, d, bw;
    Window		root;
    static int		init = False;
    extern Widget	AB_toplevel;

    /*
     * Return immediately if this function was called once before
     */
    if (init)
	return;

    dpy = XtDisplay(AB_toplevel);

    /*
     * Loop thru object_pixmaps array uintil until last entry.
     * The last entry should have type == AB_TYPE_UNKNOWN
     */
    for (i=0; (object_pixmaps[i].type != AB_TYPE_UNKNOWN); ++i)
    {
	/*
	 * Create pixmap
	 */
        status = dtb_cvt_image_file_to_pixmap(AB_toplevel,
	    object_pixmaps[i].filename, &tmp);

        if (!status)
        {
	    /*
	     * Get width/height of pixmap
	     */
	    if (XGetGeometry(dpy, tmp, &root, &x, &y, &w, &h, &bw, &d))
	    {
	        object_pixmaps[i].pixmap = tmp;
	        object_pixmaps[i].width = w;
	        object_pixmaps[i].height = h;
	    }
	    else
	        fprintf(stderr, "XGetGeometry: returned error\n");
        }
    }

    /*
     * Create default pixmap
     */
    status = dtb_cvt_image_file_to_pixmap(AB_toplevel,
                        "DtABdfl", &default_pixmap);

    if (default_pixmap)
    {
	/*
	 * Get default pixmap width/height
	 */
        if (XGetGeometry(dpy, default_pixmap, 
		&root, &x, &y, &w, &h, &bw, &d))
        {
            default_pixmap_width = w;
            default_pixmap_height = h;
        }
        else
            fprintf(stderr, "XGetGeometry: returned error\n");
    }
    
    /*
     * Set init flag
     */
    init = True;
}


/*
 * ui_get_obj_pixmap
 * based on an object's type and subtype, return a pixmap, and
 * it's width/height. This pixmap typically can be used to represent
 * the object in viewers/browsers.
 */
void
ui_get_obj_pixmap
(
    AB_OBJ		*obj,
    Pixmap		*pixmap,	/* RETURN */
    unsigned int	*width,		/* RETURN */
    unsigned int	*height		/* RETURN */
)
{
    int			i;
    AB_OBJECT_TYPE	type;
    int			subtype;
    Pixmap		p = NULL;
    BOOL		found = FALSE;

    if (!obj || !pixmap || !width || !height)
	return;

    /*
     * Initialize pixmaps
     */
    init_obj_pixmaps();

    /*
     * Get object type/subtype
     */
    type = obj_get_type(obj);
    subtype = obj_get_subtype(obj);

    /*
     * Special case for scale/gauge
     * The subtype field is not used. Instead it's read-only
     * state is used to determine if it is a scale/gauge.
     */
    if (type == AB_TYPE_SCALE)
    {
        if (obj_get_read_only(obj) == False)
	    subtype = AB_SCALE_SCALE;
        else /* Gauge */
	    subtype = AB_SCALE_GAUGE;
    }

    /*
     * Search for object type/subtype match
     */
    for (i=0; (object_pixmaps[i].type != AB_TYPE_UNKNOWN); ++i)
    {
	if ((type == object_pixmaps[i].type) &&
	    (subtype == object_pixmaps[i].subtype))
	{
	    *pixmap = object_pixmaps[i].pixmap;
	    *width = object_pixmaps[i].width;
	    *height = object_pixmaps[i].height;

	    if (*pixmap)
	        found = TRUE;

	    break;
	}
    }

    /*
     * If no match, return the default pixmap
     */
    if (!found)
    {
        *pixmap = default_pixmap;
        *width = default_pixmap_width;
        *height = default_pixmap_height;
    }
}

void
ui_add_window_close_callback(
    Widget	   shell,
    XtCallbackProc delete_callback,
    XtPointer	   client_data,
    unsigned char  delete_response
)
{
    Atom WM_DELETE_WINDOW;

    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", False);

    XtVaSetValues(shell, XmNdeleteResponse, delete_response, NULL);
    XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, delete_callback, client_data);

}

void
ui_remove_window_close_callback(
    Widget	   shell,
    XtCallbackProc delete_callback,
    XtPointer	   client_data
)
{
    Atom WM_DELETE_WINDOW;

    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(shell),"WM_DELETE_WINDOW",False);
    XmRemoveWMProtocolCallback(shell,WM_DELETE_WINDOW, 
	delete_callback, client_data);
}

void
ui_add_window_iconify_handler(
    Widget		shell,
    XtEventHandler 	iconify_proc,
    XtPointer		client_data
)
{
    XtAddEventHandler(shell, StructureNotifyMask, False, iconify_proc,
        	(XtPointer)client_data);
}

void
ui_remove_window_iconify_handler(
    Widget              shell,
    XtEventHandler      iconify_proc,
    XtPointer           client_data 
) 
{ 
    XtRemoveEventHandler(shell, StructureNotifyMask, False, iconify_proc,
                (XtPointer)client_data);
}

void
ui_refresh_widget_tree(
    Widget	widget
)
{
    WidgetList	children;
    int		num_children = 0;
    int		i;

    if (widget == NULL || !XtIsWidget(widget) || !XtIsRealized(widget))
	return;

    if (XtIsSubclass(widget, compositeWidgetClass))
    {
	XtVaGetValues(widget,
		XmNnumChildren,	&num_children,
		XmNchildren,	&children,
		NULL);

	/* Use recursion to traverse all the way to leaf nodes...*/
	for (i=0; i < num_children; i++)
	    ui_refresh_widget_tree(children[i]);
    }
    XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, TRUE);
}


static Bool		
event_is_expose(XEvent *event)
{
    Bool	is_expose = FALSE;

    switch (event->type)
    {
	case CreateNotify:
	case DestroyNotify:
	case Expose:
	case GraphicsExpose:
	case MapNotify:
	case MapRequest:
	case NoExpose:
	case UnmapNotify:
	case VisibilityNotify:
	    is_expose= TRUE;
	break;
    }

    return is_expose;
}


static void
sync_timeout_proc(
			    XtPointer		clientData,
			    XtIntervalId	*intervalIdPtr
)
{
    SyncData		syncData = (SyncData)clientData;
    time_t		cur_time = time(NULL);
    BOOL		done = FALSE;

    if (syncData->synced)
    {
	return;
    }

    done =
	((syncData->timeout_ticks - syncData->last_expose_ticks) >= 5);

    if (done)
    {
	XEvent	event;
	Display	*display = syncData->display;
	Window	window = syncData->window;
	int	i = 0;
	int	rc = 0;
	long	event_mask = 0;

	syncData->synced = TRUE;

	/*
	 * Fill in the event
	 */
	event.type = ClientMessage;
	event.xclient.display = display;
	event.xclient.window = window;
	event.xclient.message_type = 0;
	event.xclient.format = 32;
        /* data.l[] can hold only 5 longs */
	for (i = 0; i < 5; ++i)
	{
	    event.xclient.data.l[i] = sync_notify_value;
	}
	rc = XSendEvent(display, window, True, event_mask, &event);
	if (rc == 0)
	{
	    util_dprintf(0, "BIG TIME ERROR: send event failed\n");
	}
    }

    if (!(syncData->synced))
    {
        syncData->timerId = XtAppAddTimeOut(
		XtWidgetToApplicationContext(AB_toplevel), 100, 
		sync_timeout_proc, (XtPointer)clientData);
    }

    ++(syncData->timeout_ticks);
}


int
ui_sync_display_of_widget(Widget widget)
{
    int			return_value = 0;
    XtAppContext	appContext = 
				XtWidgetToApplicationContext(widget);
    XEvent		eventRec, *event = &eventRec;
    SyncDataRec		syncData;
    Bool		ignore_event = FALSE;
    Widget		ancestor = widget;
    Widget		last_ancestor = ancestor;
    Widget		sync_widget = NULL;
    Screen		*screen = NULL;

    syncData.timerId = 0;
    syncData.synced = FALSE;
    syncData.start_time = time(NULL);
    syncData.last_expose_ticks = 0;
    syncData.timeout_ticks = 0;
    syncData.display = NULL;
    syncData.window = NULL;
#define last_expose_ticks (syncData.last_expose_ticks)
#define synced (syncData.synced)
#define timeout_ticks (syncData.timeout_ticks)

    syncData.display = XtDisplay(widget);
    screen = XtScreen(widget);
    syncData.window = RootWindowOfScreen(screen);

    /*
     * Find topmost parent of this widget that belongs to application.
     * This is in case this widget is destroyed (common for popups)
     */
    last_ancestor = ancestor = widget;
    while ((ancestor != NULL) && 
	   (XtWidgetToApplicationContext(ancestor) == appContext))
    {
	last_ancestor = ancestor;
	ancestor = XtParent(ancestor);
    }
    sync_widget = last_ancestor;
    syncData.window = XtWindow(sync_widget);

    syncData.timerId = XtAppAddTimeOut(appContext, 100, 
				sync_timeout_proc, (XtPointer)&syncData);

    while (!synced)
    {
	XtAppNextEvent(appContext, event);
	ignore_event = (   (event->type == ClientMessage) 
			&& (event->xclient.data.l[0] == sync_notify_value));
	if (!ignore_event)
	{
	    XtDispatchEvent(event);
	}
	
	if (event_is_expose(event))
	{
	    last_expose_ticks = timeout_ticks;
	}
	if (difftime(time(NULL), syncData.start_time) >= 5)
	{
	    /* we've done this long enough - give up */
	    synced = TRUE;
	}
    } /* while !synced */

    XtRemoveTimeOut(syncData.timerId); syncData.timerId = 0;
    return return_value;
#undef last_expose_ticks
#undef synced
#undef timeout_ticks
}

/*
 * Rubber banding convenience routines
 */

/*
 * Rubberbanding has just finished.
 * Finish up rubber banding:
 *	- erase last box drawn
 *	- call rubber band func
 *	- reset some state variables
 */
static void
rubberband_finish(
    Widget	widget,
    XEvent	*event,
    XtPointer	client_data
)
{
    if (!rb_first_time)
    {
    	/* erase last box */
    	x_box_r(rb_widget, &rb_rect);

	/*
	 * If a rubber band func was supplied, call it
	 */
	if (rubberband_func)
	    rubberband_func(widget, event, &rb_rect, client_data);

	rubberband_func = NULL;
	rb_first_time = TRUE;
    }
}

/*
 * EventHandler: rubberband action is completed...
 * Call rubberband_finish() which calls the rubber 
 * band func.
 */
static void
rubberband_release(
    Widget widget,
    XtPointer client_data,
    XEvent *event,
    Boolean *cont_dispatch
)
{
    if (event->type != ButtonRelease)
        return;

    if (rband_in_progress)
    {
        XtUngrabPointer(widget, CurrentTime);
        XtRemoveEventHandler(widget, ButtonReleaseMask, False,
            rubberband_release, client_data);
        rband_in_progress = False;

	if (just_rbanded)
	{
            rubberband_finish(widget, event, client_data);
	    just_rbanded = False;
	    *cont_dispatch = False;
	}
    }
}

/*
 * rubberband_draw()
 * Draws the rubber band box seen when the mouse is dragged
 */
static void
rubberband_draw(
    Widget	widget,
    XEvent      *event,
    XtPointer	client_data
)
{
    short              x,y;

    if (event->type == ButtonPress)
    {
        x = (short)((XButtonEvent*)event)->x;
        y = (short)((XButtonEvent*)event)->y;
    }
    else if (event->type == MotionNotify)
    {
    	x = (short)((XMotionEvent*)event)->x;
    	y = (short)((XMotionEvent*)event)->y;
    }
    else
	return;

    if (rb_first_time)
    {
        rb_widget = widget;

        rb_rect.x = x;
        rb_rect.y = y;

	rb_first_time = FALSE;
    }
    else
        x_box_r(rb_widget, &rb_rect);

    rb_rect.width  = x - rb_rect.x;
    rb_rect.height = y - rb_rect.y;

    x_box_r(rb_widget, &rb_rect); 

}

/*
 * Starts up the rubber band UI.
 * It grab the pointer and adds an event handler 
 * which detects the ButtonRelease. It also sets the
 * rubber band func.
 */
int
ui_initiate_rubberband(
    Widget		widget,
    Boolean		confine_to_window,
    UiRubberBandFunc	rb_func,
    XtPointer		client_data
)
{

    XtAddEventHandler(widget, ButtonReleaseMask,  False,
                rubberband_release, client_data);
 
    if (XtGrabPointer(widget, False,
        ButtonReleaseMask | ButtonMotionMask | PointerMotionMask,
        GrabModeAsync, GrabModeAsync,  
	confine_to_window ? XtWindow(widget) : None,
            NULL, CurrentTime) == GrabSuccess)
    {
	rband_in_progress = True;
        rubberband_func = rb_func;
        return OK;
    }

    rband_in_progress = False;
    rubberband_func = NULL;
    return ERROR;
}


/*
 * ui_button_drag: drag action ...
 * Called by the drag button event handler for the widget
 * where rubber banding is desired.
 */
void
ui_button_drag(
    Widget	widget,
    XEvent	*event,
    XtPointer	client_data
)
{
    if (event->type == MotionNotify)
    {
        if (rband_in_progress)
        {
            rubberband_draw(widget, event, client_data);
            just_rbanded = True;
        }
    }
}


Widget
ui_optionmenu_add_item(
    Widget      opmenu,
    STRING      item_str
)
{
    XmString    xmitem;
    Widget	cascade_btn = NULL;
    Widget	menu = NULL;
    Widget	mpb = NULL;
    int		ret = 0;
 
    if ((cascade_btn = XmOptionButtonGadget(opmenu)) != NULL)
    {
	XtVaGetValues(cascade_btn, XmNsubMenuId, &menu, NULL);
	if (menu)
	{
	    xmitem = XmStringCreateLocalized(item_str);
            mpb = XtVaCreateManagedWidget(item_str,
                xmPushButtonWidgetClass,
                menu,
                XmNlabelString, xmitem,
                NULL);
	    XmStringFree(xmitem);
 	}
    }

    return (mpb);
}

int 
ui_optionmenu_delete_item( 
    Widget      opmenu, 
    STRING      item_str
) 
{ 
    Widget      item = NULL; 
    int		iRet = 0;
  
    item = ui_optionmenu_find_item(opmenu, item_str); 
    if (item != NULL)
    {
	XtDestroyWidget(item);
    }
    else
    {
	iRet = -1;
    }

    return (iRet);
}

Widget
ui_optionmenu_replace_item(
    Widget      opmenu,
    STRING      old_item_str,
    STRING      new_item_str
)
{
    Widget	item = NULL;
    Widget	opmenu_label = NULL;
    XmString	xmstr = NULL;
    XmString    new_item_xmstr = NULL;
    XmString	old_item_xmstr = NULL;

    item = ui_optionmenu_find_item(opmenu, old_item_str);
    if (item != NULL)
    {
	new_item_xmstr = XmStringCreateLocalized(new_item_str);

	/* Check if the item we're replacing is the one
	 * which is currently showing in the optionmenu.
	 * If so, change the string.
	 */
	old_item_xmstr = XmStringCreateLocalized(old_item_str);
	opmenu_label = XmOptionButtonGadget(opmenu);
	XtVaGetValues(opmenu_label, XmNlabelString, &xmstr, NULL);
	if (XmStringCompare(xmstr, old_item_xmstr))
	{
	    XtVaSetValues(opmenu_label, 
			  XmNlabelString, new_item_xmstr, 
			  NULL); 
	}

	/* Change the button label to the new string */
	XtVaSetValues(item, XmNlabelString, new_item_xmstr, NULL);

	XmStringFree(xmstr);
	XmStringFree(old_item_xmstr);
	XmStringFree(new_item_xmstr);
    }
    return (item);
}

Widget
ui_optionmenu_find_item(
    Widget      opmenu,
    STRING      item_str
)
{
    Widget      cascade_btn = NULL;
    Widget      menu = NULL;
    Widget	found_item = NULL;
    WidgetList	children = NULL;
    XmString    search_item, child = NULL;
    int         i, numChildren = 0;
    BOOL	Found = FALSE;

    if ((cascade_btn = XmOptionButtonGadget(opmenu)) != NULL)
    { 
        XtVaGetValues(cascade_btn, XmNsubMenuId, &menu, NULL);
        if (menu)   
        {
	    search_item = XmStringCreateLocalized(item_str);
	    XtVaGetValues(menu,
                        XmNnumChildren, &numChildren,
                        XmNchildren, &children,
                        NULL);
	    for (i=0; i < numChildren && !Found; i++)
	    {
		XtVaGetValues(children[i], XmNlabelString, &child, NULL);
		if (XmStringCompare(search_item, child))
		{
		    Found = TRUE;
		    found_item = children[i];
		}
	    }
	    XmStringFree(search_item);
	    XmStringFree(child);
	}
    }
    return (found_item);
}

int
ui_optionmenu_num_items(
    Widget	opmenu
)
{
    Widget      cascade_btn = NULL;
    Widget      menu = NULL;
    int         numChildren = 0;

    if ( (opmenu != NULL) &&
	 ((cascade_btn = XmOptionButtonGadget(opmenu)) != NULL)
       )
    {
        XtVaGetValues(cascade_btn, XmNsubMenuId, &menu, NULL);
        if (menu)
	{
            XtVaGetValues(menu,
                        XmNnumChildren, &numChildren,
                        NULL);
	}
    }
    return (numChildren);
}


void
ui_optionmenu_change_label(
    Widget	opmenu,
    STRING	new_str
)
{
    Widget	opmenu_label = NULL;
    XmString	new_xmstr;
    XmString	old_xmstr = NULL;

    opmenu_label = XmOptionButtonGadget(opmenu);
    if ((opmenu_label != NULL) && (new_str != NULL))
    {
	new_xmstr = XmStringCreateLocalized(new_str);
	XtVaGetValues(opmenu_label, XmNlabelString, &old_xmstr, NULL);
	
	/* If the two labels are different, then change
	 * the optionmenu label.
	 */
	if (!XmStringCompare(old_xmstr, new_xmstr))
	{
	    XtVaSetValues(opmenu_label,
                          XmNlabelString, new_xmstr,
                          NULL);
	}
	XmStringFree(new_xmstr);
    }
}

void
ui_optionmenu_change_label_pixmap(
    Widget      opmenu,
    Pixmap      pixmap
)
{
    Widget opmenu_label;
 
    opmenu_label = XmOptionButtonGadget(opmenu);
 
    if (opmenu_label != NULL)
    {
        XtVaSetValues(opmenu_label,
                XmNlabelPixmap, pixmap,
                XmNlabelType,   XmPIXMAP,
                NULL);
    }
}

/*
** Set the label string on an object
** (converts it to XmString internally, if needed)
*/
void
ui_obj_set_label_string(
    ABObj       obj,
    STRING	label
)
{
    ABObj labelObj   = NULL;
    
    if (obj == NULL)
	return;
	
    switch (obj_get_type(obj))
    {
	case AB_TYPE_BUTTON:
	case AB_TYPE_CHOICE:
	case AB_TYPE_COMBO_BOX:
	case AB_TYPE_LABEL:
	case AB_TYPE_LIST:
	case AB_TYPE_SPIN_BOX:
	case AB_TYPE_SCALE:
	case AB_TYPE_TEXT_FIELD:
	    labelObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
	    if (labelObj == NULL || objxm_get_widget(labelObj) == NULL)
		return;

	    ui_set_label_string(objxm_get_widget(labelObj), label);
	    break;

	case AB_TYPE_ITEM:
	    switch(obj_get_item_type(obj))
	    {
		case AB_ITEM_FOR_MENU:
		case AB_ITEM_FOR_MENUBAR:
		case AB_ITEM_FOR_CHOICE:
		    labelObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
		    if (labelObj == NULL || objxm_get_widget(labelObj) == NULL)
			return;

		    ui_set_label_string(objxm_get_widget(labelObj), label);
		    break;

		case AB_ITEM_FOR_COMBO_BOX:
		case AB_ITEM_FOR_LIST:
		case AB_ITEM_FOR_SPIN_BOX:
		    {
			ABObj        p_obj = obj_get_parent(obj);
			Widget       parent = objxm_get_widget(p_obj);
			AB_ITEM_TYPE itype = (AB_ITEM_TYPE)obj_get_subtype(obj);
			int          pos;
			int          num_items;
			XmString     xmitem;
	    
			if (parent != NULL)
			{
			    xmitem = XmStringCreateLocalized(label);
			    pos = obj_get_child_num(obj);
			    pos++; /* XmList starts at 1 */
		
			    if (obj_is_combo_box_item(obj))
				parent = ui_combobox_get_list_widget(parent);
		
			    if (obj_is_list_item(obj) ||
				obj_is_combo_box_item(obj))
				XtVaGetValues(parent,
				    XmNitemCount, &num_items,
				    NULL);
			    else if (obj_is_spin_box_item(obj))
				XtVaGetValues(parent,
				    DtNnumValues, &num_items,
				    NULL);

			    if (pos <= num_items)
			    {
				if (obj_is_list_item(obj) ||
				    obj_is_combo_box_item(obj))
				{
				    XmListReplacePositions(parent, &pos,
					&xmitem, 1);
				}
				else
				{
				    DtSpinBoxDeletePos(parent, pos);
				    DtSpinBoxAddItem(parent, xmitem, pos);
				}
			    }
			    XmStringFree(xmitem);
			}
		    }
		    break;

		default:
		    break;
	    }
	    break;
	    
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_DIALOG:
	case AB_TYPE_FILE_CHOOSER:
	    labelObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
	    if (labelObj == NULL || objxm_get_widget(labelObj) == NULL)
		return;

	    XtVaSetValues(objxm_get_widget(labelObj), XmNtitle, label, NULL);
	    break;

	default:
	    break;
    }
}

/*
** Set the label glyph (ie graphic) on an object
*/
void
ui_obj_set_label_glyph(
    ABObj       obj,
    STRING	fileName
)
{
    ABObj  labelObj = NULL;
    
    if (obj == NULL)
	return;

    if (util_strempty(fileName))
	return;
    
    labelObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);
    if (labelObj == NULL || objxm_get_widget(labelObj) == NULL)
	return;

    switch (obj_get_type(obj))
    {
	case AB_TYPE_BUTTON:
	case AB_TYPE_CHOICE:
	case AB_TYPE_COMBO_BOX:
	case AB_TYPE_LABEL:
	case AB_TYPE_LIST:
	case AB_TYPE_SPIN_BOX:
	case AB_TYPE_SCALE:
	case AB_TYPE_TEXT_FIELD:
	    ui_set_label_glyph(objxm_get_widget(labelObj), fileName);
	    break;

	case AB_TYPE_ITEM:
	    switch(obj_get_item_type(obj))
	    {
		case AB_ITEM_FOR_MENU:
		case AB_ITEM_FOR_MENUBAR:
		case AB_ITEM_FOR_CHOICE:
		    ui_set_label_glyph(objxm_get_widget(labelObj), fileName);
		    break;

		default:
		    break;
	    }
	    break;
	    
	default:
	    break;
    }
}

/*
** Set the label on an object
**     string and glyphs are supported
*/
void
ui_obj_set_label(
    ABObj       obj,
    STRING	label
)
{
    if (obj == NULL)
	return;

    switch (obj_get_label_type(obj))
    {
	case AB_LABEL_STRING:
	    ui_obj_set_label_string(obj, label);
	    break;

	case AB_LABEL_GLYPH:
	    ui_obj_set_label_glyph(obj, label);
	    break;

	default:
	    return;
    }
}

