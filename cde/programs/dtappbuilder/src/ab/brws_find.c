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
 *	$XConsortium: brws_find.c /main/3 1995/11/06 17:21:05 rswiston $
 *
 *	@(#)brws_find.c	1.8 12 Aug 1994
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
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <sys/param.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/ScrollBar.h>
#include <Xm/SelectioB.h>
#include <ab_private/obj.h>
#include <ab/util_types.h>
#include <ab_private/abobj.h>
#include <ab_private/proj.h>
#include <ab_private/brwsP.h>
#include <ab_private/ab.h>
#include <ab_private/objxm.h>

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/
static void		find_callback(
			    Widget      widget,
			    XtPointer   client_data,
			    XmSelectionBoxCallbackStruct *call_data
			);

static void		cancel_callback(
			    Widget      widget,
			    XtPointer   client_data,
			    XmSelectionBoxCallbackStruct *call_data
			);


/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

void
brwsP_create_find_box(
    ABBrowser	b
)
{
    Vwr			v;
    BrowserUiObjects	*ui;

    if (!b)
	return;

    v = b->module;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (!ui->find_box)
    {
	char		*title;
	XmString	sel_label,
			ok_label;

        XtCallbackRec	find_callback_list[] = {
                {(XtCallbackProc)find_callback, (XtPointer) NULL},
                {(XtCallbackProc) NULL, (XtPointer) NULL}
            };
        XtCallbackRec	cancel_callback_list[] = {
                {(XtCallbackProc)cancel_callback, (XtPointer) NULL},
                {(XtCallbackProc) NULL, (XtPointer) NULL}
            };
	Arg		args[3];
	int		num_args = 0;

	XtSetArg(args[num_args], XmNautoUnmanage, FALSE);  num_args++;
	ui->find_box = XmCreatePromptDialog(ui->shell,
			"find_box",
			args,
			num_args);

	title = XtNewString(catgets(Dtb_project_catd, 100, 230, 
			"Module Browser: Find Object"));
        XtVaSetValues(XtParent(ui->find_box), 
			XmNtitle, title,
			NULL);
	XtFree(title);

	sel_label = XmStringCreateLocalized(
		catgets(Dtb_project_catd, 100, 231, "Object Name"));
	ok_label = XmStringCreateLocalized(
		catgets(Dtb_project_catd, 100, 232, "Find"));
	XtVaSetValues(ui->find_box,
		XmNselectionLabelString, sel_label,
		XmNokLabelString, ok_label,
		XmNokCallback, &find_callback_list,
		XmNcancelCallback, &cancel_callback_list,
		XmNuserData, b,
		NULL);
	
	XmStringFree(sel_label);
	XmStringFree(ok_label);
    }
}

void
brwsP_destroy_find_box(
    Vwr		v
)
{
    BrowserUiObjects	*ui;

    if (!v)
	return;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (ui->find_box)
    {
	XtDestroyWidget(ui->find_box);
	ui->find_box = NULL;
    }
}

void
brwsP_show_find_box(
    Vwr		v
)
{
    BrowserUiObjects	*ui;

    if (!v)
	return;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (ui->find_box)
    {
	XtManageChild(ui->find_box);
    }
}

void
brwsP_hide_find_box(
    Vwr		v
)
{
    BrowserUiObjects	*ui;

    if (!v)
	return;

    ui = aob_ui_from_browser(v);

    if (!ui)
        return;

    if (ui->find_box && XtIsManaged(ui->find_box))
	XtManageChild(ui->find_box);
}

/*
 * Find callback
 * Using the name/type supplied by the user, search for an object 
 * that matches the name/type. (We currently only support find based
 * on name).
 * Also, scroll the window as best we can so that the found object
 * is in the center of the window.
 */
static void
find_callback(
    Widget      widget,
    XtPointer   client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    ABBrowser		b = NULL;
    Vwr			v;
    VNode		root_node;
    BrowserUiObj	ui;
    DtbBrwsMainwindowInfo	instance;
    ABObj		root_obj;
    ABObj		found_obj;
    char		*obj_name_str = NULL;
    char		*obj_type_str = NULL;

    XtVaGetValues(widget, XmNuserData, &b, NULL); 

    if (!b)
	return;

    /*
     * Get the root ABObj to use as the start of our search
     */
    v = b->module;
    root_node = v->current_tree;
    root_obj = (ABObj)root_node->obj_data;

    ui = aob_ui_from_browser(v);
    instance = (DtbBrwsMainwindowInfo)ui->ip;

    if (!instance->detailed_drawarea)
	return;

    /*
     * Get instance name entered by user
     */
    obj_name_str = objxm_xmstr_to_str(call_data->value);

    if (!obj_name_str)
	return;

    found_obj = obj_find_by_name(root_obj, obj_name_str);

    if (found_obj)
    {
	/*
	 **********************************************************
	 * Center browser node to the middle of the scrolled window
	 **********************************************************
	 */
        brws_center_on_obj(b, found_obj);

	/*
	 * Select object on UI
	 */
	abobj_deselect_all(obj_get_project(found_obj));
	abobj_select(found_obj);
    }

    if (obj_name_str)
	XtFree(obj_name_str);
    /*
    obj_find_by_name(ABObj root, STRING name);
    obj_find_by_type(ABObj root, AB_OBJECT_TYPE type);
    obj_find_by_name_and_type(ABObj root, STRING name, AB_OBJECT_TYPE type);
    */

}

/*
 * Cancel callback
 */
static void
cancel_callback(
    Widget      widget,
    XtPointer   client_data,
    XmSelectionBoxCallbackStruct *call_data
)
{
    XtUnmanageChild(widget);
}
