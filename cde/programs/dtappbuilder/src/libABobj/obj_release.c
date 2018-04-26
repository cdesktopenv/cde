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
 * $XConsortium: obj_release.c /main/3 1995/11/06 18:38:18 rswiston $
 * 
 * @(#)obj_release.c	3.49 26 Jan 1995
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * obj_release.c - release gobj structures (free any resources a gobj may be
 * holding - e.g., strings).
 */

#include <stdlib.h>
#include <ab_private/util.h>
#include "objP.h"

int
obj_destruct(ABObj obj)
{
    istr_destroy(obj->name);
    istr_destroy(obj->class_name);
    util_free(obj->attachments);
    istr_destroy(obj->user_data);
    istr_destroy(obj->bg_color);
    istr_destroy(obj->fg_color);
    istr_destroy(obj->label);
    istr_destroy(obj->menu_name);

    obj_destruct_type_specific_info(obj);

    objP_lists_remove(obj);
    return 0;
}


static int
obj_destruct_action(ABObj obj)
{
    return 0;
}


static int
obj_destruct_action_list(ABObj obj)
{
    return 0;
}

static int
obj_destruct_button(ABObj obj)
{
    AB_BUTTON_INFO     *info = &(obj->info.button);
    return 0;
}

static int
obj_destruct_drawing_area(ABObj obj)
{
	AB_DRAWING_AREA_INFO	*info= &(obj->info.drawing_area);
        istr_destroy(info->menu_title); 
	return 0;
}

static int
obj_destruct_combo_box(ABObj obj)
{
    AB_COMBO_BOX_INFO  *info = &(obj->info.combo_box);
    return 0;
}

static int
obj_destruct_container(ABObj obj)
{
	AB_CONTAINER_INFO	*info= &(obj->info.container);
	istr_destroy(info->menu_title);
	return 0;
}

static int
obj_destruct_file_chooser(ABObj obj)
{
    AB_FILE_CHOOSER_INFO *info = &(obj->info.file_chooser);
    istr_destroy(info->filter_pattern);
    istr_destroy(info->ok_label);
    istr_destroy(info->directory);
    return 0;
}

static int
obj_destruct_msg(ABObj obj)
{
    AB_MESSAGE_INFO    *info = &(obj->info.message);
    istr_destroy(info->msg_string);
    istr_destroy(info->action1_label);
    istr_destroy(info->action2_label);
    istr_destroy(info->action3_label);
    return 0;
}

static int
obj_destruct_module(ABObj obj)
{
    AB_MODULE_INFO     *info = &(obj->info.module);
    istr_destroy(info->file);
    istr_destroy(info->stubs_file);
    istr_destroy(info->ui_file);
    strlist_destroy(info->obj_names_list);
    return 0;
}

static int
obj_destruct_item(ABObj obj)
{
	AB_ITEM_INFO	*info= &(obj->info.item);	
	istr_destroy(info->accelerator);
	istr_destroy(info->mnemonic);
	return 0;
}

static int
obj_destruct_layers(ABObj obj)
{
    obj = obj;			/* avoid warnings */
    return 0;
}

static int
obj_destruct_menu(ABObj obj)
{
    AB_MENU_INFO       *info = &(obj->info.menu);
    return 0;
}

static int
obj_destruct_project(ABObj obj)
{
    AB_PROJECT_INFO    *info = &(obj->info.project);
    istr_destroy(info->file);
    istr_destroy(info->stubs_file);
    istr_destroy(info->vendor);
    istr_destroy(info->version);
    strlist_destroy(info->obj_names_list);
    return 0;
}

static int
obj_destruct_choice(ABObj obj)
{
    AB_CHOICE_INFO     *info = &(obj->info.choice);
    info = info;		/* avoid warnings */
    return 0;
}

static int
obj_destruct_separator(ABObj obj)
{
    AB_SEPARATOR_INFO  *info = &(obj->info.separator);
    return 0;
}

static int
obj_destruct_scale(ABObj obj)
{
    AB_SCALE_INFO      *info = &(obj->info.scale);
    return 0;
}

static int
obj_destruct_spin_box(ABObj obj)
{
    AB_SPIN_BOX_INFO   *info = &(obj->info.spin_box);
    return 0;
}

static int
obj_destruct_list(ABObj obj)
{
	AB_LIST_INFO	*info= &(obj->info.list);
        istr_destroy(info->menu_title); 
	return 0;
}

static int
obj_destruct_term_pane(ABObj obj)
{
    istr_destroy(obj->info.term.process_string);
    istr_destroy(obj->info.term.menu_title);
    return 0;
}

static int
obj_destruct_text(ABObj obj)
{
	AB_TEXT_INFO	*info= &(obj->info.text);
	istr_destroy(info->initial_value_string);
        istr_destroy(info->menu_title); 
	return 0;
}

static int
obj_destruct_window(ABObj obj)
{
    AB_WINDOW_INFO     *info = &(obj->info.window);

    istr_destroy(info->icon);
	istr_destroy(info->icon_mask);
    istr_destroy(info->icon_label);
    return 0;
}


int
obj_destruct_type_specific_info(ABObj obj)
{
    int                 (*func) (ABObj obj) = NULL;
    int                 retval = 0;

    switch (obj->type)
    {
    case AB_TYPE_ACTION:
	func = obj_destruct_action;
	break;

    case AB_TYPE_ACTION_LIST:
	func = obj_destruct_action_list;
	break;

    case AB_TYPE_BASE_WINDOW:
    case AB_TYPE_DIALOG:
	func = obj_destruct_window;
	break;

    case AB_TYPE_BUTTON:
	func = obj_destruct_button;
	break;

    case AB_TYPE_CHOICE:
	func = obj_destruct_choice;
	break;

    case AB_TYPE_COMBO_BOX:
	func = obj_destruct_combo_box;
	break;

    case AB_TYPE_FILE_CHOOSER:
	func = obj_destruct_file_chooser;
	break;

    case AB_TYPE_MESSAGE:
	func = obj_destruct_msg;
	break;

    case AB_TYPE_CONTAINER:
	func = obj_destruct_container;
	break;

    case AB_TYPE_DRAWING_AREA:
	func = obj_destruct_drawing_area;
	break;

    case AB_TYPE_MODULE:
	func = obj_destruct_module;
	break;

    case AB_TYPE_ITEM:
	func = obj_destruct_item;
	break;

    case AB_TYPE_LAYERS:
	func = obj_destruct_layers;
	break;

    case AB_TYPE_LIST:
	func = obj_destruct_list;
	break;

    case AB_TYPE_MENU:
	func = obj_destruct_menu;
	break;

    case AB_TYPE_PROJECT:
	func = obj_destruct_project;
	break;

    case AB_TYPE_SEPARATOR:
	func = obj_destruct_separator;
	break;

    case AB_TYPE_SCALE:
	func = obj_destruct_scale;
	break;

    case AB_TYPE_SPIN_BOX:
	func = obj_destruct_spin_box;
	break;

    case AB_TYPE_TERM_PANE:
	func = obj_destruct_term_pane;
	break;

    case AB_TYPE_TEXT_FIELD:
    case AB_TYPE_TEXT_PANE:
	func = obj_destruct_text;
	break;

    case AB_TYPE_LABEL:
    case AB_TYPE_UNKNOWN:
	func = NULL;
	break;

    default:
	retval = -1;
	break;
    }

    if ((retval >= 0) && (func != NULL))
    {
	retval = func(obj);
    }
    return retval;
}
