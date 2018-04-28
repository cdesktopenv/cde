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
 *	$XConsortium: obj_init.c /main/3 1995/11/06 18:35:13 rswiston $
 *
 *	@(#)obj_init.c	3.114 03 Feb 1995	
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


/*
 *  init.c - initialize gobj structures
 */

#include <stdlib.h>
#include <string.h>
#include "objP.h"

#ifndef abo_attachments_create
#define abo_attachments_create(o)	(o)->attachments = \
		(ABAttachListPtr)util_malloc(sizeof(ABAttachmentList))
#endif
#ifndef abo_attachments_destroy
#define abo_attachments_destroy(o)	util_free((o)->attachments)
#endif

int
obj_init_attachments(
    ABObj	obj
)
{
    if (obj->attachments == NULL)
	abo_attachments_create(obj);

    obj->attachments->north.type   = AB_ATTACH_NONE;
    obj->attachments->north.value  = (void*)NULL;
    obj->attachments->north.offset = 0;

    obj->attachments->south.type   = AB_ATTACH_NONE;
    obj->attachments->south.value  = (void*)NULL;
    obj->attachments->south.offset = 0;

    obj->attachments->east.type    = AB_ATTACH_NONE;
    obj->attachments->east.value   = (void*)NULL;
    obj->attachments->east.offset  = 0;
 
    obj->attachments->west.type    = AB_ATTACH_NONE; 
    obj->attachments->west.value   = (void*)NULL; 
    obj->attachments->west.offset  = 0; 

    return 0;
}
    

int
obj_construct(ABObj obj, AB_OBJECT_TYPE type, ABObj parent)
{
	/* Assume 0 is:  NULL, 0, FALSE */
	memset((void*)obj, 0, sizeof(ABObjRec));

	obj->type = type;
#ifdef DEBUG
	obj->debug_name = istr_string(obj->name);
	obj->debug_last_verify_time = 0;
#endif
	obj->flags= NoFlags;
	obj->impl_flags = ObjFlagIsDefined | ObjFlagIsInitiallyVisible | ObjFlagIsInitiallyActive;
	obj->x= -1;
	obj->y= -1;
	obj->width= -1;
	obj->width_max= -1;
	obj->height= -1;
	obj->height_max= -1;
	obj->label_type = AB_LABEL_STRING;
	obj->border_frame = AB_LINE_NONE;

	/*
	 * type is set appropriately above
	 */
	obj_construct_type_specific_info(obj);

	obj_set_parent(obj, parent);

	objP_lists_add(obj);

	return OK;
}


static int
obj_construct_action(ABObj obj)
{
	AB_ACTION_INFO	*info= &(obj->info.action);

	/* Assume 0 is:  NULL, 0, FALSE */
	memset((void*)info, 0, sizeof(AB_ACTION_INFO));
	info->func_type = AB_FUNC_BUILTIN;
	info->func_value.builtin= AB_STDACT_UNDEF;
	info->arg_type = AB_ARG_VOID_PTR;
	info->arg_value.fval = (double)0.0;
	return 0;
}


static int 
obj_construct_action_list(ABObj obj)
{
	return 0;
}

static int
obj_construct_button(ABObj obj)
{
	AB_BUTTON_INFO	*info= &(obj->info.button);

	info->type = AB_BUT_PUSH;
	info->label_alignment = AB_ALIGN_CENTER;
	return 0;
}

static int
obj_construct_drawing_area(ABObj obj)
{
	AB_DRAWING_AREA_INFO	*info= &(obj->info.drawing_area);
	
	info->drawarea_width  = 0;
	info->drawarea_height = 0;
	info->hscrollbar = AB_SCROLLBAR_ALWAYS;
	info->vscrollbar = AB_SCROLLBAR_ALWAYS;
        info->pane_min = 1;
        info->pane_max = 1000;
    	info->menu_title = NULL;
	return 0;
}

static int
obj_construct_combo_box(ABObj obj)
{
	AB_COMBO_BOX_INFO	*info= &(obj->info.combo_box);
        obj_set_impl_flags(obj, ObjFlagIsReadOnly);
        info->label_position = AB_CP_WEST; 
	return 0;
}

static int
obj_construct_container(ABObj obj)
{
	AB_CONTAINER_INFO	*info= &(obj->info.container);

	info->type = AB_CONT_ABSOLUTE;
	info->has_border= FALSE;
	info->packing= AB_PACK_NONE;
	info->group_type = AB_GROUP_IGNORE;
	info->col_align= AB_ALIGN_TOP;
	info->num_columns= 0;
	info->hoffset= 0;
	info->hspacing= 0;
	info->row_align= AB_ALIGN_LEFT;
	info->num_rows= 1;
	info->voffset= 0;
	info->vspacing= 0;
	info->pane_min = 1;
	info->pane_max = 1000;
    	info->menu_title = NULL;
	return 0;
}


static int
obj_construct_file_chooser(ABObj obj)
{
	AB_FILE_CHOOSER_INFO	*info= &(obj->info.file_chooser);

	/* A file chooser should never be mapped upon 
	 * application start-up.
	 */
	obj_set_is_initially_visible(obj, FALSE);

        info->win_parent = NULL;
	info->filter_pattern= NULL;
	info->ok_label = NULL;
	info->auto_dismiss = TRUE;
	info->directory = NULL;
	info->file_type_mask = AB_FILE_REGULAR;  
	return 0;
}

static int
obj_construct_msg(ABObj obj)
{
        AB_MESSAGE_INFO	*info = &(obj->info.message);

        info->type = AB_MSG_UNDEF;
        info->msg_string = NULL;
	info->action1_label = NULL;
	info->action2_label = NULL;
	info->action3_label = NULL;
	info->cancel_button = TRUE;
	info->help_button = TRUE;
	info->default_btn = AB_DEFAULT_BTN_UNDEF;
        return 0;
}

static int
obj_construct_module(ABObj obj)
{
	AB_MODULE_INFO	*info= &(obj->info.module);

	info->file = NULL;
	info->stubs_file = NULL;
	info->ui_file = NULL;
	info->obj_names_list = strlist_create();
	strlist_set_sort_order(info->obj_names_list, STRLIST_SORT_BEST);
	info->write_me = FALSE;
	return 0;
}

static int
obj_construct_item(ABObj obj)
{
	AB_ITEM_INFO	*info= &(obj->info.item);

	info->type = AB_ITEM_FOR_UNDEF;
	info->is_initially_selected= FALSE;
	info->is_help_item= FALSE;
	info->accelerator= NULL;
	info->mnemonic = NULL;
	info->line_style = AB_LINE_UNDEF;
	return 0;
}

static int
obj_construct_label(ABObj obj)
{
        AB_LABEL_INFO  *info= &(obj->info.label);

        info->label_alignment= AB_ALIGN_CENTER;
        return 0;
}

static int
obj_construct_layers(ABObj obj)
{
	AB_LAYER_INFO	*info = &(obj->info.layer);

	info->pane_min = 1;
	info->pane_max = 1000;
	return 0;
}

static int
obj_construct_menu(ABObj obj)
{
	AB_MENU_INFO	*info= &(obj->info.menu);

	info->type = AB_MENU_PULLDOWN;
	info->tear_off= FALSE;
	info->exclusive = FALSE;
	return 0;
}

static int
obj_construct_project(ABObj obj)
{
	AB_PROJECT_INFO	*info= &(obj->info.project);

	info->file = NULL;
	info->stubs_file = NULL;
	info->vendor= NULL;
	info->version= NULL;
	info->root_window= NULL;
	info->obj_names_list = strlist_create();
	strlist_set_sort_order(info->obj_names_list, STRLIST_SORT_BEST);
	info->is_default= FALSE;
	info->res_file_arg_classes = AB_ARG_CLASS_FLAGS_NONE;
	info->browsers= NULL;
	info->session_mgmt.sessioning_method= AB_SESSIONING_NONE;
	info->tooltalk.level= AB_TOOLTALK_NONE;
	info->session_mgmt.sessioning_method= AB_SESSIONING_NONE;
	info->i18n.enabled= FALSE;
	return 0;
}

static int
obj_construct_choice(ABObj obj)
{
	AB_CHOICE_INFO	*info= &(obj->info.choice);

	info->type = AB_CHOICE_EXCLUSIVE;
	info->orientation= AB_ORIENT_HORIZONTAL;
	info->label_position = AB_CP_WEST;
	return 0;
}

static int
obj_construct_separator(ABObj obj)
{
	AB_SEPARATOR_INFO	*info= &(obj->info.separator);

	info->orientation = AB_ORIENT_HORIZONTAL;
	info->line_style = AB_LINE_NONE;
	return 0;
}

static int
obj_construct_scale(ABObj obj)
{
	AB_SCALE_INFO	*info= &(obj->info.scale);

	info->initial_value = 0;
	info->orientation= AB_ORIENT_HORIZONTAL;
	info->direction= AB_DIR_LEFT_TO_RIGHT;
	info->min_value = 0;
	info->max_value = 0;
	info->increment= 0;
	info->decimal_points= 0;
	info->show_value = FALSE;
	info->label_position = AB_CP_WEST;
	return 0;
}

static int
obj_construct_spin_box(ABObj obj)
{
	AB_SPIN_BOX_INFO	*info= &(obj->info.spin_box);
	info->type = AB_TEXT_NUMERIC;
	info->arrow_style = AB_ARROW_END;
	info->label_position = AB_CP_WEST;
	info->min_value = 0;
	info->max_value = 0;
        info->initial_value = 0;
	info->increment= 1;
	info->decimal_points = 0;
	return 0;
}

static int
obj_construct_list(ABObj obj)
{
	AB_LIST_INFO	*info= &(obj->info.list);

	info->selection_mode = AB_SELECT_SINGLE;
	info->selection_required= FALSE;
	info->num_rows= -1;
	info->vscrollbar  = AB_SCROLLBAR_ALWAYS;
	info->hscrollbar  = AB_SCROLLBAR_NEVER;
        info->label_position = AB_CP_WEST;
    	info->menu_title = NULL;
	return 0;
}

static int
obj_construct_term_pane(
    ABObj obj
)
{
    AB_TERM_INFO *info= &(obj->info.term);

    info->num_rows = 1;
    info->num_columns = 0;
    info->vscrollbar  = AB_SCROLLBAR_NEVER;
    info->process_string = NULL;
    info->pane_min = 1;
    info->pane_max = 1000;
    info->menu_title = NULL;
    return 0;
}

static int
obj_construct_text(ABObj obj)
{
	AB_TEXT_INFO	*info= &(obj->info.text);

	info->type = AB_TEXT_ALPHANUMERIC;
	info->num_rows= 1;
	info->num_columns= 0;
	info->max_length= 0;
	info->initial_value_string= NULL;
	info->initial_value_int= 0;
	info->word_wrap=FALSE;
	info->has_border= FALSE;
	info->label_position = AB_CP_WEST;
	info->vscrollbar  = AB_SCROLLBAR_ALWAYS;
	info->hscrollbar  = AB_SCROLLBAR_NEVER;
        info->pane_min = 1;
        info->pane_max = 1000;
    	info->menu_title = NULL;
	return 0;
}

static int
obj_construct_window(ABObj obj)
{
	AB_WINDOW_INFO	*info= &(obj->info.window);

	info->win_parent = NULL;
	info->default_act_button = NULL;
	info->help_act_button = NULL;
	info->resizable = TRUE;
	info->is_initially_iconic = FALSE;
	info->icon = NULL;
	info->icon_mask = NULL;
	info->icon_label = NULL;
	return 0;
}


int
obj_construct_type_specific_info(ABObj obj)
{
	int	(*func)(ABObj obj)= NULL;
	int	retval= 0;

	if (obj_is_item(obj))
	{
		func = obj_construct_item;
	}
	else if (obj_is_window(obj) && !obj_is_file_chooser(obj))
	{
		func = obj_construct_window;
	}
	if (func == NULL)
	{
		switch (obj->type)
		{
		case AB_TYPE_ACTION: 
			func = obj_construct_action;
		break;

		case AB_TYPE_ACTION_LIST:
			func = obj_construct_action_list;	
		break;

		case AB_TYPE_BUTTON:
			func = obj_construct_button;
		break;

		case AB_TYPE_CHOICE:
			func = obj_construct_choice;
			break;

		case AB_TYPE_COMBO_BOX:
			func = obj_construct_combo_box;
		break;

		case AB_TYPE_FILE_CHOOSER:
			func = obj_construct_file_chooser;
		break;

		case AB_TYPE_MESSAGE:
			func = obj_construct_msg;
		break;

		case AB_TYPE_CONTAINER:
			func = obj_construct_container;
		break;

		case AB_TYPE_DRAWING_AREA:
			func = obj_construct_drawing_area;
		break;

		case AB_TYPE_MODULE:
			func = obj_construct_module;
		break;

		case AB_TYPE_LABEL:
			func = obj_construct_label;
		break;

		case AB_TYPE_LAYERS:
			func = obj_construct_layers;
		break;

		case AB_TYPE_LIST:
			func = obj_construct_list;
		break;
		
		case AB_TYPE_MENU:
			func = obj_construct_menu;
		break;

		case AB_TYPE_PROJECT:
			func = obj_construct_project;
		break;

		case AB_TYPE_SEPARATOR:
			func = obj_construct_separator;
		break;

		case AB_TYPE_SCALE:
			func = obj_construct_scale;
		break;

		case AB_TYPE_SPIN_BOX:
			func = obj_construct_spin_box;
		break;

		case AB_TYPE_TERM_PANE:
		        func = obj_construct_term_pane;
		break;
		case AB_TYPE_TEXT_FIELD:
		case AB_TYPE_TEXT_PANE:
			func = obj_construct_text;
		break;

		case AB_TYPE_UNKNOWN:
		break;

		default:
			retval= -1;
		break;
		} /* switch */
	} /* if func */

	if ((retval >= 0) && (func != NULL))
	{
		retval= func(obj);
	}
	return retval;
}

