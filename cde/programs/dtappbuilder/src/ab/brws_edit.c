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

/*
 *	$XConsortium: brws_edit.c /main/3 1995/11/06 17:20:54 rswiston $
 *
 *	@(#)brws_edit.c	1.2 28 Jan 1994	
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */



#include <sys/param.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include <ab_private/istr.h>
#include <ab/util_types.h>
#include <ab_private/abobj.h>
#include <ab_private/abobj_set.h>
#include <ab_private/proj.h>
#include <ab_private/brwsP.h>
#include <ab_private/objxm.h>

static void	browser_textf_activate(
		    Widget widget, 
		    XtPointer client_data,
		    XtPointer call_data
		);

static void
browser_textf_activate(
    Widget widget, 
    XtPointer client_data,
    XtPointer call_data
)
{
    ABObj	textf_obj = NULL;
    VNode	textf_node = NULL;
    char	*newName = NULL;

    XtVaGetValues(widget, XmNuserData, &textf_node, NULL); 

    if (!textf_node)
	return;

    textf_obj = (AB_OBJ *)textf_node->obj_data;

    newName = XmTextFieldGetString(widget);

    if (!newName)
	return;

    XtUnmanageChild(widget);
    XtVaSetValues(widget, XmNuserData, NULL, NULL); 

    if (!util_streq(obj_get_name(textf_obj), newName))
    {
        abobj_set_name(textf_obj, newName);
        proj_update_node(textf_obj);
        brws_update_node(textf_obj);
    }

    XtFree(newName);
}

void
brwsP_create_textf(
    Vwr		v
)
{
    BrowserUiObjects	*ui;

    if (!v)
	return;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (!ui->textf)
    {
        XmFontListEntry		font_list_entry;
        XmFontList		font_list;
	Widget			draw_area;

        draw_area = brws_draw_area(v);

	if (!draw_area)
	    return;

        font_list_entry = XmFontListEntryCreate(
                                XmFONTLIST_DEFAULT_TAG,
                                XmFONT_IS_FONT,
                                ui->sm_font);

        font_list = XmFontListAppendEntry(NULL, font_list_entry);

        ui->textf = XtVaCreateWidget("change_name",
			xmTextFieldWidgetClass, 
			draw_area,
			XmNfontList,     	font_list,
			XmNshadowThickness,     0,
			XmNhighlightThickness,  0,
			XmNmarginWidth,		BRWS_ELM_BBOX_MARGIN,
			XmNmarginHeight,	BRWS_ELM_BBOX_MARGIN,
			XmNmaxLength,           80,
			XmNbackground,		
			    BlackPixelOfScreen(XtScreen(ui->shell)),
			XmNforeground,		
			    WhitePixelOfScreen(XtScreen(ui->shell)),
			NULL);
		
        XtAddCallback(ui->textf, XmNactivateCallback,
                (XtCallbackProc)browser_textf_activate,
                (XtPointer)NULL);
    }
}

void
brwsP_destroy_textf(
    Vwr		v
)
{
    BrowserUiObjects	*ui;

    if (!v)
	return;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (ui->textf)
    {
	XtDestroyWidget(ui->textf);
	ui->textf = NULL;
    }
}

void
brwsP_hide_textf(
    Vwr		v
)
{
    BrowserUiObjects	*ui;

    if (!v)
	return;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (ui->textf && XtIsManaged(ui->textf))
        XtUnmanageChild(ui->textf);
}

/*
 * brwsP_show_textf()
 * Show the edit in place textfield on the node element specified
 * by 'elm_pos'.
 * Also, store the vnode as XmNuserData on the textfield.
 */
void
brwsP_show_textf(
    VNode	vnode,
    int		elm_pos
)
{
    Vwr			v;
    BrowserUiObj	ui;

    if (!vnode || (elm_pos < 0) || 
	!vnode->num_elements || 
	(elm_pos >= vnode->num_elements))
	return;

    if (!(v = vnode->browser))
	return;

    if (!(ui = aob_ui_from_browser(v)))
        return;

    if (ui->textf)
    {
	VNodeElm	elm = vnode->elements;

	if (!elm)
	    return;

        XtVaSetValues(ui->textf,
            XmNvalue,	(char *)elm[elm_pos].data,
            XmNx,	vnode->x + 
                        BRWS_ELM_BORDER_WIDTH,
            XmNy,	elm[elm_pos].y - 
			BRWS_ELM_BBOX_MARGIN,
            XmNwidth,	vnode->width - 
			(2 * BRWS_ELM_BORDER_WIDTH),
            XmNheight,	elm[elm_pos].height +
			(2 * BRWS_ELM_BBOX_MARGIN),
            XmNuserData,vnode,
            NULL);

        XtManageChild(ui->textf);
    }
}
