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
 * $XConsortium: bil_loadatt.c /main/3 1995/11/06 18:24:16 rswiston $
 * 
 * @(#)bil_loadatt.c	1.115 15 Aug 1995
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
 * loadatt.c - Load attributes from BIL file
 * 
 * NOTE: the only exported identifier in this file is bilP_load_attribute.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ab/util_types.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include "../libABobj/obj_utils.h"
#include "load.h"
#include "bilP.h"
#include "bil_loadattP.h"

typedef STRING      LOADATT_FUNC(ABObj root_obj);
typedef LOADATT_FUNC *LOADATT_FUNC_PTR;

/*************************************************************************
**************************************************************************
**									**
**		INTERNAL FUNCTIONS     				 	**
**									**
**************************************************************************
**************************************************************************/

#define set_att(s) (abil_loadmsg_set_att(s))
#define clear_att() (abil_loadmsg_set_att(NULL))
#define nset_att(a) (bilP_load_set_current_att(a))
#define nclear_att() (bilP_load_set_current_att(AB_BIL_UNDEF))

/*
 */
ISTRING
bilP_get_string()
{
    STRING              tmp_str = NULL;
    ISTRING             value1;
    /* ISTRING	value2; */
    int                 valueToken = AB_BIL_UNDEF;
    int                 length;
    STRING              tmp = NULL;

    valueToken = bilP_load_get_value_type();
    if (valueToken == AB_BIL_VALUE_STRING)
    {
	tmp_str = bilP_load_get_value();
    }
    else if (valueToken == AB_BIL_NIL)
    {
	return NULL;
    }
    else
    {
	abil_print_load_err(ERR_WANT_STRING);
	return NULL;
    }

    value1 = istr_create(tmp_str);
    length = istr_len(value1);

#ifdef BOGUS
    /**** REMIND: strip off quotes, sometimes heap overwrite violation*/
    tmp = (char *) malloc(length * sizeof(char));
    for (i = 0; i < length - 2; i++)
    {
	tmp[i] = (istr_string(value1)[i + 1]);
    }
    tmp[i] = '\0';
    istr_destroy(value1);
    value2 = istr_create_alloced(tmp);
    return value2;
#endif				/* BOGUS */

    return value1;
}

/*************************************************************************
**************************************************************************
**									**
**		LOAD SPECIFIC ATTRIBUTES				**
**									**
**	bil_parse calls these functions.     				**
**************************************************************************
**************************************************************************/

/*
 */
STRING
bilP_load_att()
{

    return NULL;
}

/*
 */

STRING
bilP_load_att_children_begin(void)
{
    nset_att(AB_BIL_CHILDREN);
    return NULL;
}

STRING
bilP_load_att_children_end(void)
{
    return NULL;
}

STRING
bilP_load_att_east_attachment_begin(void)
{
    nset_att(AB_BIL_EAST_ATTACHMENT);
    return NULL;
}

STRING
bilP_load_att_east_attachment_end(void)
{
    return NULL;
}

STRING
bilP_load_att_west_attachment_begin(void)
{
    nset_att(AB_BIL_WEST_ATTACHMENT);
    return NULL;
}

STRING
bilP_load_att_west_attachment_end(void)
{
    return NULL;
}

STRING
bilP_load_att_north_attachment_begin(void)
{
    nset_att(AB_BIL_NORTH_ATTACHMENT);
    return NULL;
}

STRING
bilP_load_att_north_attachment_end(void)
{
    return NULL;
}

STRING
bilP_load_att_south_attachment_begin(void)
{
    nset_att(AB_BIL_SOUTH_ATTACHMENT);
    return NULL;
}

STRING
bilP_load_att_south_attachment_end(void)
{
    return NULL;
}

STRING
bilP_load_att_class(BIL_TOKEN valueToken)
{
    AB_OBJECT_TYPE      objType = AB_TYPE_UNKNOWN;
    STRING              errMessage = NULL;
    ABObj		obj = NULL;

    util_dprintf(3, "bilP_load_att_class %d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)));

    if (bilP_load.obj != NULL)
    {
	if (bilP_load.obj->type == AB_TYPE_MODULE)
	{

	    /*
	     * We've found an element inside a module
	     */
	    bilP_load.module = bilP_load.obj;
	    bilP_load.obj = NULL;
	}
	else
	{
	    if (bilP_load.obj->type == AB_TYPE_PROJECT)
	    {
		bilP_load.obj = NULL;
	    }
	    else
	    {
		obj_destroy(bilP_load.obj);
		bilP_load.obj = NULL;
	    }
	}
    }

    objType = AB_TYPE_UNKNOWN;
    switch (valueToken)
    {
    case AB_BIL_MODULE:
        if (bilP_load.project == NULL)
        {
            /* If the parser has read in the :module keyword and
             * bilP_load.project is NULL, then something is wrong.
             * The user probably tried Opening a module file.
             * This skips the rest of the BIL file and prints out
             * an error message.
             */
            fseek(AByyin, SEEK_END, 0);
            abil_print_load_err(ERR_NOT_PROJECT);
        }
	else
	    objType = AB_TYPE_MODULE;
	break;

    case AB_BIL_PROJECT:
	if (bilP_load.project == NULL)
	{
	    /* 
	     * MUST SET BOTH "project" and "module" TO BE THE PROJECT
	     * This is so that objects that belong to the project (e.g.,
	     * cross-module connections) can be handled like any other
	     * object.
	     */
	    objType = AB_TYPE_PROJECT;
	}
	else
	{
	    /* If the parser has read in the :project keyword
	     * and bilP_load.project is not NULL, then that
	     * probably means that the user tried importing a
	     * project into another project.  This skips the
	     * rest of the BIL file and prints out an error.
	     */
	    fseek(AByyin, SEEK_END, 0);
	    abil_print_load_err(ERR_NOT_MODULE);
	}
	break;

    case AB_BIL_CONNECTION:
	objType = AB_TYPE_ACTION;
	obj = obj_create(objType, NULL);

	/* Store the action on bilP_load.module. bilP_load.module
	 * is equal to bilP_load.project, if we're parsing a project 
	 * file. If they aren't equal, then we're parsing a module
	 * file, so the action obj will be parented off of the module 
	 * not the project in this case.
	 */
	if (bilP_load.module != NULL)
	{
	    obj_add_action(bilP_load.module, obj);
	}
	break;

    }

    bilP_load.obj = obj;	/* may be NULL */
    bilP_load.objType = objType;

    return errMessage;
}

STRING
bilP_load_att_class_name(BIL_TOKEN valueToken)
{
    int                 type = AB_BIL_UNDEF;
    STRING              class_name = NULL;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_CLASS_NAME);
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_IDENT)
	class_name = bilP_load_get_value();
    else
	abil_print_load_err(ERR_WANT_STRING);
    obj_set_class_name(obj, class_name);

    return NULL;
}

STRING
bilP_load_att_container_type(BIL_TOKEN valueToken)
{
    AB_CONTAINER_TYPE   containerType;

    nset_att(AB_BIL_CONTAINER_TYPE);
    containerType = bilP_token_to_container_type(valueToken);

    /* REMIND: we should obsolute AB_CONT_ABSOLUTE */
    if (containerType == AB_CONT_ABSOLUTE)
    {
	util_dprintf(1, "%s", abil_loadmsg("AB_CONT_ABSOLUTE is obsolete!"));
	containerType = AB_CONT_RELATIVE;
    }
    obj_set_subtype(bilP_load.obj, containerType);
    return NULL;
}

STRING
bilP_load_att_decimal_points(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    nset_att(AB_BIL_DECIMAL_POINTS);
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
        value = atoi(bilP_load_get_value());
    else
        abil_print_load_err(ERR_WANT_INTEGER);
    obj_set_decimal_points(obj, value);

    return NULL;
}

STRING
bilP_load_att_default(BIL_TOKEN valueToken)
{
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                is_default = FALSE;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_DEFAULT);
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
	return NULL;
    }
    is_default = bilP_token_to_bool(valueToken);
    obj_set_is_default(obj, is_default);

    return NULL;
}
STRING
bilP_load_att_direction(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    AB_DIRECTION	obj_type;

    set_att(":direction");
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
        abil_print_load_err(ERR_WANT_KEYWORD);
        return NULL;
    }   
    obj_type = bilP_token_to_direction(type);
    if (obj_set_direction(obj, obj_type) < 0)
    {
        abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
    }
 
    return NULL;
}

STRING
bilP_load_att_drag_cursor(BIL_TOKEN valueToken)
{
    ISTRING	cursor = bilP_get_string();
    obj_set_drag_cursor(bilP_load.obj, istr_string(cursor));
    istr_destroy(cursor);
    return NULL;
}

STRING
bilP_load_att_drag_cursor_mask(BIL_TOKEN valueToken)
{
    ISTRING	mask = bilP_get_string();
    obj_set_drag_cursor_mask(bilP_load.obj, istr_string(mask));
    istr_destroy(mask);
    return NULL;
}

STRING
bilP_load_att_drag_enabled(BIL_TOKEN valueToken)
{
    BOOL	draggable;
    nset_att(AB_BIL_DRAG_ENABLED);
    draggable = bilP_token_to_bool(valueToken);
    obj_set_drag_initially_enabled(bilP_load.obj, draggable);
    return NULL;
}

STRING
bilP_load_att_drag_ops_begin()
{
    nset_att(AB_BIL_DRAG_OPS);
    return NULL;
}

STRING
bilP_load_att_drag_ops_end()
{
    clear_att();
    return NULL;
}

STRING
bilP_load_att_drag_to_root_allowed(BIL_TOKEN valueToken)
{
    BOOL	ok;
    nset_att(AB_BIL_DRAG_TO_ROOT_ALLOWED);
    ok = bilP_token_to_bool(valueToken);
    obj_set_drag_to_root_allowed(bilP_load.obj, ok);
    return NULL;
}

STRING
bilP_load_att_drag_types_begin()
{
    nset_att(AB_BIL_DRAG_TYPES);
    return NULL;
}

STRING
bilP_load_att_drag_types_end()
{
    clear_att();
    return NULL;
}

STRING
bilP_load_att_drop_enabled(BIL_TOKEN valueToken)
{
    BOOL	droppable;
    nset_att(AB_BIL_DROP_ENABLED);
    droppable = bilP_token_to_bool(valueToken);
    obj_set_drop_initially_enabled(bilP_load.obj, droppable);
    return NULL;
}

STRING
bilP_load_att_drop_site_child_allowed(BIL_TOKEN valueToken)
{
    BOOL	dropAllowed = bilP_token_to_bool(valueToken);
    nset_att(AB_BIL_DROP_SITE_CHILD_ALLOWED);
    obj_set_drop_on_children_is_allowed(bilP_load.obj, dropAllowed);
    return NULL;
}

STRING
bilP_load_att_drop_ops_begin()
{
    nset_att(AB_BIL_DROP_OPS);
    return NULL;
}

STRING
bilP_load_att_drop_ops_end()
{
    clear_att();
    return NULL;
}

STRING
bilP_load_att_drop_types_begin()
{
    nset_att(AB_BIL_DROP_TYPES);
    return NULL;
}

STRING
bilP_load_att_drop_types_end()
{
    clear_att();
    return NULL;
}

STRING
bilP_load_att_files_begin()
{
    nset_att(AB_BIL_FILES);
    return NULL;
}

STRING
bilP_load_att_files_end()
{
    clear_att();
    return NULL;
}

STRING
bilP_load_att_filter_pattern(BIL_TOKEN valueToken)
{
    ISTRING             filter;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_FILTER_PATTERN);
    filter = bilP_get_string();

    obj_set_filter_pattern(obj, istr_string(filter));
    istr_destroy(filter);

    return NULL;
}

STRING
bilP_load_att_icon_label(BIL_TOKEN valueToken)
{
    ISTRING             icon_label = NULL;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_ICON_LABEL);
    icon_label = bilP_get_string();
    obj_set_icon_label(obj, istr_string(icon_label));
    istr_destroy(icon_label);

    return NULL;
}

STRING
bilP_load_att_item_type(BIL_TOKEN valueToken)
{
    AB_ITEM_TYPE        itemType = AB_ITEM_FOR_UNDEF;

    nset_att(AB_BIL_ITEM_TYPE);
    itemType = bilP_token_to_item_type(valueToken);
    obj_set_subtype(bilP_load.obj, itemType);

    return NULL;
}

STRING
bilP_load_att_menu(BIL_TOKEN valueToken)
{
    ABObj               newObj = NULL;
    STRING              stringValue = bilP_load_get_value();

    if (valueToken == AB_BIL_VALUE_IDENT)
    {
	newObj = obj_scoped_find_or_create_undef(bilP_load.module, 
			stringValue, AB_TYPE_MENU);
	obj_set_menu_name(bilP_load.obj, stringValue);
    }
    else if (valueToken == AB_BIL_NIL)
    {
    }
    else
    {
	abil_print_load_err(ERR_WANT_NAME);
    }

    return NULL;
}

STRING
bilP_load_att_menu_title(BIL_TOKEN valueToken)
{
    ISTRING             title;
    ABObj               obj = bilP_load.obj;

    set_att(":menu-title");
    title = bilP_get_string();
    obj_set_menu_title(obj, istr_string(title));
    istr_destroy(title);
 
    return NULL;
}

STRING
bilP_load_att_menu_type(BIL_TOKEN valueToken)
{
    AB_MENU_TYPE        menuType;

/*
    nset_att(AB_BIL_MENU_TYPE);
    menuType = bilP_token_to_menu_type(valueToken);
    obj_set_subtype(bilP_load.obj, menuType);
*/

    return NULL;
}

/*
 */
STRING
bilP_load_att_accelerator(BIL_TOKEN valueToken)
{
    ISTRING             accel;
    ABObj               obj = bilP_load.obj;

    set_att(":accelerator");
    accel = bilP_get_string();
    obj_set_accelerator(obj, istr_string(accel));
    istr_destroy(accel);

    return NULL;
}

STRING
bilP_load_att_mnemonic(BIL_TOKEN valueToken)
{
    ISTRING             mnemon;
    ABObj               obj = bilP_load.obj;

    set_att(":mnemonic");
    mnemon = bilP_get_string();
    obj_set_mnemonic(obj, istr_string(mnemon));
    istr_destroy(mnemon);

    return NULL;
}

STRING
bilP_load_att_alignment(BIL_TOKEN valueToken)
{
    int                 align_string = AB_BIL_UNDEF;
    AB_ALIGNMENT        alignment;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_LABEL_ALIGNMENT);
    if ((align_string = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
	abil_print_load_err(ERR_WANT_KEYWORD);
	return NULL;
    }

    alignment = bilP_token_to_alignment(align_string);

    if (obj_is_container(obj))
    {
	if (obj->info.container.group_type == AB_GROUP_COLUMNS)
	    obj->info.container.col_align = alignment;
	else if (obj->info.container.group_type == AB_GROUP_ROWS)
	    obj->info.container.row_align = alignment;
	else
	    abil_print_load_err(ERR_UNKNOWN);
    }
    else if (obj_is_button(obj) || obj_is_item(obj)
	     || (obj_is_label(obj)))
    {
	obj_set_label_alignment(obj, alignment);
    }
    else
    {
	abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
	return NULL;
    }

    return NULL;
}

STRING
bilP_load_att_col_alignment(BIL_TOKEN valueToken)
{
    int                 align_string = AB_BIL_UNDEF;
    AB_ALIGNMENT        alignment;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_COLUMN_ALIGNMENT);
    if ((align_string = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
	abil_print_load_err(ERR_WANT_KEYWORD);
	return NULL;
    }

    alignment = bilP_token_to_alignment(align_string);
    obj->info.container.col_align = alignment;

    return NULL;
}

STRING
bilP_load_att_res_file_types_begin(void)
{
    nset_att(AB_BIL_RES_FILE_TYPES);
    return NULL;
}

STRING
bilP_load_att_res_file_types_end(void)
{
    return NULL;
}


STRING
bilP_load_att_row_alignment(BIL_TOKEN valueToken)
{
    int                 align_string = AB_BIL_UNDEF;
    AB_ALIGNMENT        alignment;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_ROW_ALIGNMENT);
    if ((align_string = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
	abil_print_load_err(ERR_WANT_KEYWORD);
	return NULL;
    }

    alignment = bilP_token_to_alignment(align_string);
    obj->info.container.row_align = alignment;

    return NULL;
}


STRING
bilP_load_att_bg_color(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             bg_color = NULL;
    int                 type;

    nset_att(AB_BIL_BG_COLOR);
    bg_color = bilP_get_string();
    obj_set_bg_color(obj, istr_string(bg_color));

    return NULL;
}

STRING
bilP_load_att_button_type(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    AB_BUTTON_TYPE      obj_type;

    set_att(":button-type");
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);
    obj_type = bilP_token_to_button_type(type);

    if (!obj_is_button(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj_set_button_type(obj, obj_type);

    return NULL;
}


STRING
bilP_load_att_drawarea_height(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":drawarea-height");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    if (value >= 0)
	obj_set_drawarea_height(obj, value);

    return NULL;
}

STRING
bilP_load_att_drawarea_width(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":drawarea-width");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    if (value >= 0)
	obj_set_drawarea_width(obj, value);

    return NULL;
}

STRING
bilP_load_att_fg_color(BIL_TOKEN valueToken)
{
    ISTRING             fg_color = NULL;
    int                 type;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_FG_COLOR);
    fg_color = bilP_get_string();
    obj_set_fg_color(obj, istr_string(fg_color));
    return NULL;
}

STRING
bilP_load_att_msg_type(BIL_TOKEN valueToken)
{
    int                 type = AB_BIL_UNDEF;
    AB_MESSAGE_TYPE 	obj_type;
    ABObj               obj = bilP_load.obj;

    set_att(":message-type");
    if (!obj_is_message(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJECT);

    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
	abil_print_load_err(ERR_WANT_KEYWORD);
    }
    obj_type = bilP_token_to_msg_type(type);
    if (type == AB_MSG_UNDEF)
	abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);

    obj_set_msg_type(obj, obj_type);

    return NULL;
}

STRING
bilP_load_att_group_type(BIL_TOKEN valueToken)
{
    AB_GROUP_TYPE   groupType;

    nset_att(AB_BIL_GROUP_TYPE);
    groupType = bilP_token_to_group_type(valueToken);
    obj_set_group_type(bilP_load.obj, groupType);
    return NULL;
}

STRING
bilP_load_att_hattach_type(BIL_TOKEN valueToken)
{
    AB_ATTACH_TYPE   hattachType;

    nset_att(AB_BIL_HATTACH_TYPE);
    hattachType = bilP_token_to_attachment(valueToken);
    obj_set_hattach_type(bilP_load.obj, hattachType);
    return NULL;
}

STRING
bilP_load_att_height(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":height");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else if (type == AB_BIL_VALUE_FLOAT)
	value = (int) (atof(bilP_load_get_value()));
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    obj->height = value;

    return NULL;
}

STRING
bilP_load_att_height_max(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":height-max");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else if (type == AB_BIL_VALUE_FLOAT)
	value = (int) (atof(bilP_load_get_value()));
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    obj->height_max = value;

    return NULL;
}

STRING
bilP_load_att_height_resizable(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                resizable;

    set_att(":height-resizable");
    resizable = bilP_token_to_bool(valueToken);
    obj_set_height_is_resizable(obj, resizable);

    return NULL;
}

STRING
bilP_load_att_help_text(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             tmp_str = NULL;

    set_att(":help-text");
    tmp_str = bilP_get_string();
    obj_set_help_text(obj, istr_string(tmp_str));
    istr_destroy(tmp_str);

    return NULL;
}

STRING
bilP_load_att_hscrollbar(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL		sb_flag = FALSE;
    AB_SCROLLBAR_POLICY	hscrollbar;

    set_att(":hscrollbar");
  
    util_dprintf(3, "bilP_load_att_hscrollbar %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    /* REMIND: (mcv) This is here for backwards compatibility */
    if (bilP_token_is_bool(valueToken))
    {
	sb_flag = bilP_token_to_bool(valueToken);	
	if (sb_flag)
	    obj_set_hscrollbar_policy(obj, AB_SCROLLBAR_ALWAYS);
	else
	    obj_set_hscrollbar_policy(obj, AB_SCROLLBAR_NEVER);
    }
    else  /* The real deal */
    {
	hscrollbar = bilP_token_to_scrollbar_policy(valueToken);
	obj_set_hscrollbar_policy(obj, hscrollbar);
    }

    return NULL;
}

STRING
bilP_load_att_hspacing(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 tmp_int;
    int                 type;

    nset_att(AB_BIL_HSPACING);
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	tmp_int = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    if (obj_is_container(obj))
    {
	obj->info.container.hspacing = tmp_int;
    }

    return NULL;
}

STRING
bilP_load_att_hoffset(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 tmp_int;
    int                 type;

    set_att(":hoffset");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	tmp_int = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    if (!obj_is_container(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.container.hoffset = tmp_int;

    return NULL;
}

/*
 * for base window - icon file
 */
STRING
bilP_load_att_icon(BIL_TOKEN valueToken)
{
    ISTRING             icon = NULL;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_ICON_FILE);
    icon = bilP_get_string();
    obj_set_icon(obj, istr_string(icon));
    istr_destroy(icon);

    return NULL;
}

STRING
bilP_load_att_icon_mask(BIL_TOKEN valueToken)
{
    ISTRING             mask = NULL;
    ABObj               obj = bilP_load.obj;

    nset_att(AB_BIL_ICON_MASK_FILE);
    mask = bilP_get_string();
    obj_set_icon_mask(obj, istr_string(mask));
    istr_destroy(mask);

    return NULL;
}

STRING
bilP_load_att_label(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             value;

    set_att(":label");
    value = bilP_get_string();
    /*
     * REMIND: aim -remove once all BIL files are converted
     * Since we previously supported storing the complete graphic
     * filename (including extension), look to see if the extension
     * exists; if so, strip it off so that when the file is saved
     * back out, the format will be correct.
     */ 
    if (bilP_load.obj->label_type == AB_LABEL_GLYPH) 
    { 
        char            filename[512];
        STRING          ext;
 
        strcpy(filename, istr_string(value)); 
        if (util_file_name_has_extension(filename, "pm") || 
            util_file_name_has_extension(filename, "xpm") || 
            util_file_name_has_extension(filename, "bm") || 
            util_file_name_has_extension(filename, "xbm")) 
        {
            ext = strrchr(filename, '.');
            ext[0] = '\0'; /* strip off extension */
            istr_destroy(value);
            value = istr_create(filename);
        }
    }   
    obj_set_label(obj, istr_string(value));
    istr_destroy(value);
    return NULL;
}

STRING
bilP_load_att_label_emphasis(BIL_TOKEN valueToken)
{
    int                 label_emphasis = AB_BIL_UNDEF;
    AB_LABEL_TYPE       obj_type;

    set_att(":label-emphasis");
    if ((label_emphasis = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);

    /*** REMIND: not in ABObj
            obj_type = ab_bil_token_to_label_emphasis(label_emphasis);
	    obj->label_emphasis= obj_type;
    ***/
    return NULL;
}

STRING
bilP_load_att_label_type(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 label_type = AB_BIL_UNDEF;
    AB_LABEL_TYPE       obj_type;

    set_att(":label-type");
    if ((label_type = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);

    obj_type = bilP_token_to_label_type(label_type);
    obj->label_type = obj_type;
    return NULL;
}

STRING
bilP_load_att_max_label(BIL_TOKEN valueToken)
{
    ISTRING             value;

    set_att(":max-label");
    value = bilP_get_string();
    /*** REMIND: not in ABObj
            abo_set_max_label(obj,istr_string(value));
    ***/
    istr_destroy(value);

    return NULL;
}

STRING
bilP_load_att_max_value(BIL_TOKEN valueToken)
{
    /* REMIND: METHOD obj_set_max_value NOT IMPLEMENTED */
    ABObj               obj = bilP_load.obj;
    int                 tmp_int = 0;
    int                 type;

    set_att(":max-value");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
    {
	tmp_int = atoi(bilP_load_get_value());
    }
    else
    {
	abil_print_load_err(ERR_WANT_INTEGER);
	return NULL;
    }

    obj_set_max_value(obj, tmp_int);

    return NULL;
}

STRING
bilP_load_att_min_label(BIL_TOKEN valueToken)
{
    ISTRING             tmp_str;

    set_att(":min-label");
    tmp_str = bilP_get_string();
    /*** REMIND: not in ABObj
            abo_set_min_label(obj,istr_string(tmp_str));
    ***/
    istr_destroy(tmp_str);

    return NULL;
}

STRING
bilP_load_att_min_value(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    nset_att(AB_BIL_MIN_VALUE);
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    obj_set_min_value(obj, value);

    return NULL;
}

#ifdef BOGUS

/*
 * Loads in a module file name for a project
 */
STRING
bilP_load_att_module_files(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             module;
    ABObj               module_obj;

    set_att(":module-files");
    module = bilP_get_string();

    if (module == NULL)
	return NULL;

    if (obj->type != AB_TYPE_PROJECT)
	abil_print_load_err(ERR_UNKNOWN_OBJECT);
    module_obj = obj_create(AB_TYPE_UNKNOWN, NULL);
    obj_append_child(obj, module_obj);
    obj_set_file(module_obj, istr_string(module));
    obj_set_name(module_obj, module);
    obj_set_loaded(module_obj, FALSE);
    istr_destroy(module);

    return NULL;
}

#endif				/* BOGUS */

STRING
bilP_load_att_multiple_selections(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                selection;

    /* for exclusive and non-exclusive in scrolling-list */
/*
    set_att(":multiple-selections");
    selection = bilP_token_to_bool(valueToken);
    if (!obj_is_list(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.list.multiple_selections = selection;
*/
    util_dprintf(1, "%s", abil_loadmsg("multiple-selections attribute is obsolete!"));

    return NULL;
}

/*
 * If an object with the given name exists, we're gonna grab it in place of
 * this one.
 */
STRING
bilP_load_att_name(BIL_TOKEN valueToken)
{
#define load_type (bilP_load.objType)
#define load_obj (bilP_load.obj)
#define load_module (bilP_load.module)
#define load_compObjsRead (bilP_load.compObjsRead)
#define load_project (bilP_load.project)
    STRING              errmsg = NULL;
    STRING              string_name = NULL;
    ISTRING             name = NULL;
    int                 type = AB_BIL_UNDEF;
    ABObj               forward_obj = NULL;

    nset_att(AB_BIL_NAME);
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_IDENT)
	string_name = bilP_load_get_value();
    else
    {
	abil_print_load_err(ERR_WANT_STRING);
	goto epilogue;
    }

    abil_loadmsg_set_object(string_name);

    util_dprintf(3, "bilP_load_att_name(%s)\n", string_name);

    name = istr_create(string_name);
    if ((istr_string(name) != NULL) &&
	(!ab_c_ident_is_ok(istr_string(name))))
    {
	istr_destroy(name);
	abil_print_load_err(ERR_BAD_IDENTIFIER);
    }
    abil_loadmsg_set_object(istr_string(name));

    /*
     * Find any forward reference to this object
     */
    if (load_type == AB_TYPE_MODULE)
    {
	/* always have to look in the project for a module */
	if (load_project != NULL)
	    forward_obj = obj_find_module_by_name(load_project, string_name);
    }
    else
    {
        forward_obj = obj_find_by_name(load_module, istr_string(name));
    }

    /*
     * Reconcile the forward reference (if there is one)
     */
    if (forward_obj != NULL)
    {
	if (!obj_is_defined(forward_obj))
	{
	    /* 
	     * An undefined object with this name exists - it was
	     * forward-referenced. BIL guarantees that the first
	     * attribute found for the definition of an object is
	     * the name, so we can just add the additional attributes
	     * into the forward reference.
	     */
	    AB_OBJECT_TYPE      obj_type = AB_TYPE_UNKNOWN;
	    ABObj		doomed_obj = NULL;

	    /* 
	     * Set the "current" object to be the "forward" reference
	     */
	    load_obj = forward_obj;

	    /* 
	     * Mark the current obj as being defined (we're at the beginning
	     * of its definition, now).
	     */
	    obj_set_is_defined(load_obj, TRUE);
	}
	else
	{
	    /* a duplicate name! */
	    char                msg[1024], *obj_name;

	    /* The object has not been created yet, so create it
	     * here.
	     */
	    load_obj = obj_create(load_type, NULL);
	    obj_set_name(load_obj, istr_string(name));

	    if (obj_is_module(load_obj))
	    {
		if (load_project != NULL)
		    obj_ensure_unique_name(load_obj, load_project, -1);
		load_module = load_obj;
		objlist_add_obj(load_compObjsRead, load_module, NULL);
		if (load_project != NULL)
		{
		    obj_append_child(load_project, load_module);
		}
	    }
	    else
	    {
		obj_ensure_unique_name(load_obj, load_module, -1);
	    }

	    obj_name = obj_get_name(load_obj);
	    sprintf(msg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 36,
		  "Duplicate name.  Changed to %s.\n\t(This may be a conflict with the interface or project name)\n"), obj_name);
	    abil_print_custom_load_err(msg);
	}
    } 
    else /* forward_obj == NULL */
    {
	/* brand new object! */
	load_obj = obj_create(load_type, NULL);
	obj_set_name(load_obj, istr_string(name));

	/*
         * Be sure the load fields are set up properly
         */
        switch (obj_get_type(load_obj))
        {
        case AB_TYPE_MODULE:
	    load_module = load_obj;
	    objlist_add_obj(load_compObjsRead, load_module, NULL);
	    if (load_project != NULL)
	    {
	        obj_append_child(load_project, load_module);
	    }
	    break;

        case AB_TYPE_PROJECT:
	    if (load_project == NULL)
	    {
	        /* 
	         * MUST SET BOTH "project" and "module" TO BE THE PROJECT
	         * This is so that objects that belong to the project (e.g.,
	         * cross-module connections) can be handled like any other
	         * object.
	         */
	        load_project = load_obj;
	        load_module = load_obj;
	    }
	    else
	    {
	        /* leave type unknown - ignore it */
	    }
	    break;
	} /* switch obj_get_type() */

        if (obj_is_module(load_obj) || obj_is_project(load_obj))
        {
	    obj_set_file(load_obj, istr_string(bilP_load.fileName));
        }
    } /* forward_obj == NULL */

    /*
     * Keep track of what we're up to
     */
    abil_loadmsg_set_object(obj_get_name(load_obj));

epilogue:
    istr_destroy(name);
    return errmsg;
#undef load_type
#undef load_obj
#undef load_module
#undef load_compObjsRead
#undef load_project
}

/*
 * only for menus and choices
 */
STRING
bilP_load_att_num_columns(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 num_columns = 0;
    int                 type;

    set_att(bilP_token_to_string(valueToken));
    if (valueToken == AB_BIL_VALUE_INT)
	num_columns = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    switch (obj->type)
    {
    case AB_TYPE_MENU:
	if (num_columns > 0)
	{

	    /*
	     * value is # of items
	     */
	}
	break;

    default:
	obj_set_num_columns(obj, num_columns);
	break;
    }

    return NULL;
}

/*
 * used for scales and gauges
 */
STRING
bilP_load_att_orientation(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    AB_ORIENTATION      obj_type;

    set_att(":orientation");
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
	abil_print_load_err(ERR_WANT_KEYWORD);
	return NULL;
    }
    obj_type = bilP_token_to_orientation(type);
    if (obj_set_orientation(obj, obj_type) < 0)
    {
	abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
    }

    return NULL;
}

/*
 */
STRING
bilP_load_att_process_string(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             value;

    set_att(":process-string");
    value = bilP_get_string();
    obj_set_process_string(obj, istr_string(value));
    istr_destroy(value);
    return NULL;
}

STRING
bilP_load_att_reference_point(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    AB_COMPASS_POINT    obj_type;

    /* REMIND: what about reference point?! */
    set_att(":reference-point");
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);
    obj_type = bilP_token_to_compass_point(type);
    if (!obj_is_container(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.container.ref_point = obj_type;
    return NULL;
}

STRING
bilP_load_att_resource(BIL_TOKEN valueToken)
{
    int                 type = AB_BIL_UNDEF;
    STRING              value1 = NULL;
    STRING              value2 = NULL;
    BOOL                resource;

    return NULL;
}


/*
 * for text-field, text and scrolling-list
 */
STRING
bilP_load_att_read_only(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                read_only;

    set_att(":read-only");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    if (!obj_is_list(obj))
    {
    	read_only = bilP_token_to_bool(valueToken);
    	obj_set_read_only(obj, read_only);
    }

    return NULL;
}

/*
 * for projects
 */
STRING
bilP_load_att_root_window(BIL_TOKEN valueToken)
{
    ABObj		newObj;
    STRING              stringValue = bilP_load_get_value(); 

    if (!obj_is_project(bilP_load.obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);

    if (valueToken == AB_BIL_VALUE_IDENT)
    {
        newObj = obj_scoped_find_or_create_undef(bilP_load.obj,
                                stringValue, AB_TYPE_UNKNOWN);

        obj_set_root_window(bilP_load.obj, newObj);
    }
    else
        abil_print_load_err(ERR_WANT_NAME);

    return NULL;
}

STRING
bilP_load_att_selection_required(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                selection_required;

    /* for choices & scrolling-list */
    set_att(":selection-required");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    selection_required = bilP_token_to_bool(valueToken);
    obj_set_selection_required(obj, selection_required);

    return NULL;
}

STRING
bilP_load_att_sessioning_method(BIL_TOKEN valueToken)
{
    ABObj               	obj = bilP_load.obj;
    AB_SESSIONING_METHOD	sessioning_method;

    set_att(":sessioning-method");
    if (!obj_is_project(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    sessioning_method = bilP_token_to_sessioning_method(valueToken);
    obj_set_sessioning_method(obj, sessioning_method);

    return NULL;
}

STRING
bilP_load_att_tt_desktop_level(BIL_TOKEN valueToken)
{
    ABObj                       obj = bilP_load.obj;
    AB_TOOLTALK_LEVEL		tt_level;

    set_att(":desktop-level");
    if (!obj_is_project(obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    tt_level = bilP_token_to_tt_desktop_level(valueToken);
    obj_set_tooltalk_level(obj, tt_level);

    return NULL;
}


STRING
bilP_load_att_choice_type(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    AB_CHOICE_TYPE      obj_type;
    int                 type;

    nset_att(AB_BIL_CHOICE_TYPE);
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);
    obj_type = bilP_token_to_choice_type(type);
    if (!obj_is_choice(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.choice.type = obj_type;

    return NULL;
}

/*
 * for control-area, term and text panes - Xview puts a 1 pixel border OL
 * says don't
 */
STRING
bilP_load_att_border_frame(BIL_TOKEN valueToken)
{
    AB_LINE_TYPE      borderType = AB_LINE_UNDEF;

    set_att(":border-frame");
    borderType = bilP_token_to_border_frame(valueToken);
    obj_set_border_frame(bilP_load.obj, borderType);

    return NULL;
}

STRING
bilP_load_att_has_footer(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    BOOL                has_footer;

    util_dprintf(3, "bilP_load_att_has_footer %d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)));

    set_att(":has-footer");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    has_footer = bilP_token_to_bool(valueToken);
/*
    REMIND: has-footer attribute is now obsolete
    obj_set_has_footer(obj, has_footer);
*/
    return NULL;
}

/*
 * for text-field
 */
STRING
bilP_load_att_max_length(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 tmp_int;
    int                 type;

    set_att(":max-length");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	tmp_int = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    if (tmp_int > 0)
    {
	obj->info.text.max_length = tmp_int;
    }

    return NULL;
}

STRING
bilP_load_att_tear_off(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                tear_off;

    set_att(":tear-off");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    else
    {
	tear_off = bilP_token_to_bool(valueToken);
    }
    if (!obj_is_menu(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.menu.tear_off = tear_off;

    return NULL;
}

STRING
bilP_load_att_text_type(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type;
    AB_TEXT_TYPE        text_type;

    /* for alpha/numeric text-field */
    set_att(":text-type");
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);
    text_type = bilP_token_to_text_type(type);
    if (!obj_is_text(obj) && !obj_is_spin_box(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj_set_text_type(obj, text_type);

    return NULL;
}

STRING
bilP_load_att_increment(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    nset_att(AB_BIL_INCREMENT);
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
        value = atoi(bilP_load_get_value());
    else
        abil_print_load_err(ERR_WANT_INTEGER);
    obj_set_increment(obj, value);

    return NULL;
}

STRING
bilP_load_att_initial_value(BIL_TOKEN valueToken)
{
    ISTRING             istr_value = NULL;
    int                 ival = 0;

    set_att(":initial-value");
    if (valueToken == AB_BIL_VALUE_INT)
    {
	ival = atoi(bilP_load_get_value());
	obj_set_initial_value_int(bilP_load.obj, ival);
    }
    else if (valueToken == AB_BIL_VALUE_STRING)
    {

	/*
	 * if (bilP_token_is_string(valueToken))
	 */
	istr_value = bilP_get_string();
	obj_set_initial_value_string(bilP_load.obj, istr_string(istr_value));
    }

    /*
     * istr_destroy(istr_value);
     */
    return NULL;
}

STRING
bilP_load_att_vattach_type(BIL_TOKEN valueToken)
{
    AB_ATTACH_TYPE   vattachType;

    nset_att(AB_BIL_VATTACH_TYPE);
    vattachType = bilP_token_to_attachment(valueToken);
    obj_set_vattach_type(bilP_load.obj, vattachType);
    return NULL;
}

STRING
bilP_load_att_vendor(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             value;

    set_att(":vendor");
    if (!obj_is_project(obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    value = bilP_get_string();
    obj_set_vendor(obj, istr_string(value));
    istr_destroy(value);
    return NULL;
}

STRING
bilP_load_att_version(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             value;

    set_att(":version");
    if (!obj_is_project(obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    value = bilP_get_string();
    obj_set_version(obj, istr_string(value));
    istr_destroy(value);
    return NULL;
}

STRING
bilP_load_att_visible(BIL_TOKEN valueToken)
{
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                visible;

    set_att(":visible");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    visible = bilP_token_to_bool(valueToken);
    obj_set_is_initially_visible(bilP_load.obj, visible);

    return NULL;
}

STRING
bilP_load_att_voffset(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 tmp_int;
    int                 type;

    set_att(":voffset");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	tmp_int = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    if (!obj_is_container(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.container.voffset = tmp_int;

    return NULL;
}

STRING
bilP_load_att_vscrollbar(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL		sb_flag = FALSE;
    AB_SCROLLBAR_POLICY vscrollbar;

    set_att(":vscrollbar");

    util_dprintf(3, "bilP_load_att_vscrollbar %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    /* REMIND: (mcv) This is here for backwards compatibility */
    if (bilP_token_is_bool(valueToken))
    {
        sb_flag = bilP_token_to_bool(valueToken);  
        if (sb_flag)
            obj_set_vscrollbar_policy(obj, AB_SCROLLBAR_ALWAYS); 
        else 
            obj_set_vscrollbar_policy(obj, AB_SCROLLBAR_NEVER); 
    } 
    else  /* The real deal */ 
    {
	vscrollbar = bilP_token_to_scrollbar_policy(valueToken);
	obj_set_vscrollbar_policy(obj, vscrollbar);
    } 

    return NULL;
}

STRING
bilP_load_att_vspacing(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 tmp_int;
    int                 type;

    set_att(":vspacing");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	tmp_int = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    if (!obj_is_container(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj->info.container.vspacing = tmp_int;

    return NULL;
}

/*
 * only for scrolling_list and choices
 */
STRING
bilP_load_att_num_rows(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 rows = 0;
    int                 type;

    set_att(":num-rows");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	rows = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    if (obj_set_num_rows(obj, rows) < 0)
    {
  	abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
    }

    return NULL;
}

STRING
bilP_load_att_selected(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;

    set_att(":selected");
    if (!bilP_token_is_bool(valueToken))
	abil_print_load_err(ERR_WANT_BOOLEAN);
    obj_set_is_initially_selected(obj, bilP_token_to_bool(valueToken));

    return NULL;
}

STRING
bilP_load_att_type(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    AB_OBJECT_TYPE      obj_type = AB_TYPE_UNKNOWN;

    nset_att(AB_BIL_TYPE);
    obj_type = bilP_token_to_object_type(valueToken);

    /*
     * Make sure the type matched the forward ref
     */
    if (   (obj_get_type(obj) != AB_TYPE_UNDEF)
	&& (obj_get_type(obj) != AB_TYPE_UNKNOWN)
	&& (obj_type != AB_TYPE_UNDEF)
	&& (obj_type != AB_TYPE_UNKNOWN)
	&& (obj_get_type(obj) != obj_type) )
    {
	char	msg[1024];
	*msg = 0;
	
	 /* REMIND: make cat entry!
	 sprintf(msg, 
		catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 36,
		  "Duplicate name.  Changed to %s.\n\t(This may be a conflict with the interface or project name)\n"),
		    obj_name);
	*/
	sprintf(msg, "Object has two different types. Aborting.\n");
	abil_print_custom_load_err(msg);
    }

    obj_set_type(obj, obj_type);

    return NULL;
}

/*
 * Currently, all generations of guide have ignored user-data.
 */
STRING
bilP_load_att_user_data(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             userdata;

    set_att(":user-data");
    userdata = bilP_get_string();
    obj_set_user_data(obj, istr_string(userdata));
    istr_destroy(userdata);

    return NULL;
}

STRING
bilP_load_att_width(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":width");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    if (value >= 0)
	obj->width = value;

    return NULL;
}

STRING
bilP_load_att_width_max(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":width-max");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    obj->width_max = value;

    return NULL;
}

STRING
bilP_load_att_width_resizable(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                resizable;

    set_att(":width-resizable");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    resizable = bilP_token_to_bool(valueToken);
    obj_set_width_is_resizable(obj, resizable);

    return NULL;
}

STRING
bilP_load_att_x(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":x");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    obj->x = value;

    return NULL;
}

STRING
bilP_load_att_y(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":y");
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
	value = atoi(bilP_load_get_value());
    else
	abil_print_load_err(ERR_WANT_INTEGER);
    obj->y = value;

    return NULL;
}


/*
 * Token dictates the type
 */
STRING
bilP_load_list_value(BIL_TOKEN token)
{
#define loadObj (bilP_load.obj)
    ABObj               newObj = NULL;
    STRING              stringValue = bilP_load_get_value();
    ABDndOpFlags	dndOpFlag = 0;
    ABDndTypeFlags	dndTypeFlag = 0;

    util_dprintf(3, "bilP_load_list_value(%s)\n", stringValue);

    switch (bilP_load.att)
    {
	case AB_BIL_DRAG_OPS:
	    dndOpFlag = bilP_token_to_dnd_op_flag(token);
	    obj_set_drag_ops(loadObj, obj_get_drag_ops(loadObj) | dndOpFlag);
	break;

	case AB_BIL_DRAG_TYPES:
	    dndTypeFlag = bilP_token_to_dnd_type_flag(token);

	    /* CDE1.0 had a bug where multiple drag types could be selected
	     * and stored in the bil file.  Now that this bug is fixed
	     * and only one drag type is permitted, we must "throw away"
	     * any additional types specified in older bil files so that
	     * the first type becomes the value.
	     */
	    if (obj_get_drag_types(loadObj) == ABDndTypeNone)
	    	obj_set_drag_types(loadObj, dndTypeFlag);
	    else
        	util_dprintf(1, "%s", 
			abil_loadmsg(catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 48,
			"only 1 drag-type allowed - discarding additional drag-type")));
	break;

	case AB_BIL_DROP_OPS:
	    dndOpFlag = bilP_token_to_dnd_op_flag(token);
	    obj_set_drop_ops(loadObj, obj_get_drop_ops(loadObj) | dndOpFlag);
	break;

	case AB_BIL_DROP_TYPES:
	    dndTypeFlag = bilP_token_to_dnd_type_flag(token);
	    obj_set_drop_types(loadObj, 
				obj_get_drop_types(loadObj) | dndTypeFlag);
	break;

	case AB_BIL_FILES:
	    newObj = obj_create(AB_TYPE_FILE, NULL);
	    obj_set_name(newObj, stringValue);
	    obj_append_child(bilP_load.obj, newObj);
	break;

	case AB_BIL_CHILDREN:
	    newObj = obj_scoped_find_or_create_undef(
			bilP_load.obj, stringValue, AB_TYPE_UNKNOWN);
	    if (obj_get_parent(newObj) != bilP_load.obj)
	    {
		obj_reparent(newObj, bilP_load.obj);
	    }
	break;

	case AB_BIL_RES_FILE_TYPES:
	{
	    AB_ARG_CLASS    argClass = bilP_token_to_arg_class(token);
	    AB_ARG_CLASS_FLAGS	argFlag = util_arg_class_to_flag(argClass);
	    assert(obj_is_project(bilP_load.obj));

	    obj_set_res_file_arg_classes(bilP_load.obj,
		obj_get_res_file_arg_classes(bilP_load.obj) | argFlag);
	}
	break;

        case AB_BIL_WIN_CHILDREN:
            newObj = obj_scoped_find_or_create_undef(
                        	bilP_load.module, 
				stringValue, 
				AB_TYPE_DIALOG);
	    obj_set_win_parent(newObj, bilP_load.obj);
	break;
    }


    return NULL;
#undef loadObj
}

int
bilP_load_set_current_att(BIL_TOKEN att)
{
    STRING              attName = bilP_token_to_string(att);

    bilP_load.att = att;
    if (att == AB_BIL_UNDEF)
    {
	attName = NULL;
    }
    abil_loadmsg_set_att(bilP_token_to_string(att));
    return 0;
}

/*
 * STRING bilP_load_att_north_attachment(BIL_TOKEN valueToken) {
 * AB_ATTACH_TYPE   	obj_type; int			type = AB_BIL_UNDEF;
 * 
 * set_att(":north-attachment"); if ((type = bilP_load_get_token()) ==
 * AB_BIL_UNDEF) abil_print_load_err(ERR_WANT_KEYWORD); if (valueToken
 * != AB_BIL_NIL) { obj_type = bilP_token_to_attachment(type);
 * obj_set_attach_type(bilP_load.obj, AB_CP_NORTH, obj_type); } return NULL;
 * }
 * 
 * STRING bilP_load_att_south_attachment(BIL_TOKEN valueToken) { AB_ATTACH_TYPE
 * obj_type; int             type = AB_BIL_UNDEF;
 * 
 * set_att(":south-attachment"); if ((type = bilP_load_get_token()) ==
 * AB_BIL_UNDEF) abil_print_load_err(ERR_WANT_KEYWORD); if (valueToken
 * != AB_BIL_NIL) {  obj_type = bilP_token_to_attachment(type);
 * obj_set_attach_type(bilP_load.obj, AB_CP_SOUTH, obj_type); } return NULL;
 * }
 * 
 * STRING bilP_load_att_east_attachment(BIL_TOKEN valueToken) { AB_ATTACH_TYPE
 * obj_type; int             type = AB_BIL_UNDEF;
 * 
 * set_att(":east_attachment"); if ((type = bilP_load_get_token()) ==
 * AB_BIL_UNDEF)  abil_print_load_err(ERR_WANT_KEYWORD); if (valueToken
 * != AB_BIL_NIL) { obj_type = bilP_token_to_attachment(type);
 * obj_set_attach_type(bilP_load.obj, AB_CP_EAST, obj_type); } return NULL; }
 * 
 * STRING bilP_load_att_west_attachment(BIL_TOKEN valueToken) { AB_ATTACH_TYPE
 * obj_type; int             type = AB_BIL_UNDEF;
 * 
 * set_att(":west-attachment"); if ((type = bilP_load_get_token()) ==
 * AB_BIL_UNDEF)   abil_print_load_err(ERR_WANT_KEYWORD); if (valueToken
 * != AB_BIL_NIL) { obj_type = bilP_token_to_attachment(type);
 * obj_set_attach_type(bilP_load.obj, AB_CP_WEST, obj_type); } return NULL; }
 * 
 * STRING bilP_load_att_north_offset(BIL_TOKEN valueToken) { int     type,
 * value=0;
 * 
 * set_att(":north-offset"); if ((type = bilP_load_get_value_type()) ==
 * AB_BIL_VALUE_INT) { value = atoi(bilP_load_get_value());
 * obj_set_attach_offset(bilP_load.obj, AB_CP_NORTH, value); } else
 * abil_print_load_err(ERR_WANT_INTEGER);
 * 
 * return NULL; }
 * 
 * STRING bilP_load_att_south_offset(BIL_TOKEN valueToken) { int     type,
 * value=0;
 * 
 * set_att(":south-offset"); if ((type = bilP_load_get_value_type()) ==
 * AB_BIL_VALUE_INT) { value = atoi(bilP_load_get_value());
 * obj_set_attach_offset(bilP_load.obj, AB_CP_SOUTH, value); } else
 * abil_print_load_err(ERR_WANT_INTEGER);
 * 
 * return NULL; }
 * 
 * STRING bilP_load_att_east_offset(BIL_TOKEN valueToken) { int     type,
 * value=0;
 * 
 * set_att(":east-offset"); if ((type = bilP_load_get_value_type()) ==
 * AB_BIL_VALUE_INT) { value = atoi(bilP_load_get_value());
 * obj_set_attach_offset(bilP_load.obj, AB_CP_EAST, value); } else
 * abil_print_load_err(ERR_WANT_INTEGER);
 * 
 * return NULL; }
 * 
 * STRING bilP_load_att_west_offset(BIL_TOKEN valueToken) { int     type,
 * value=0;
 * 
 * set_att(":west-offset"); if ((type = bilP_load_get_value_type()) ==
 * AB_BIL_VALUE_INT) { value = atoi(bilP_load_get_value());
 * obj_set_attach_offset(bilP_load.obj, AB_CP_WEST, value);  } else
 * abil_print_load_err(ERR_WANT_INTEGER);
 * 
 * return NULL; }
 * 
 * STRING bilP_load_att_north_value(BIL_TOKEN valueToken) { ABObj	target= NULL;
 * STRING	name= NULL; int		value;
 * 
 * switch (obj_get_attach_type(bilP_load.obj, AB_CP_NORTH)) { case
 * AB_ATTACH_OBJ: case AB_ATTACH_ALIGN_OBJ_EDGE: name= bilP_load_get_value();
 * target= obj_find_by_name(bilP_load.project, name);
 * obj_set_attach_value(bilP_load.obj, AB_CP_NORTH, (void*)target); break;
 * case AB_ATTACH_GRIDLINE: value = atoi(bilP_load_get_value());
 * obj_set_attach_value(bilP_load.obj, AB_CP_NORTH, (void*)value); break; }
 * return NULL; }
 * 
 * STRING bilP_load_att_south_value(BIL_TOKEN valueToken) { ABObj	target= NULL;
 * STRING	name= NULL; int		value;
 * 
 * switch (obj_get_attach_type(bilP_load.obj, AB_CP_SOUTH)) { case
 * AB_ATTACH_OBJ: case AB_ATTACH_ALIGN_OBJ_EDGE: name= bilP_load_get_value();
 * target= obj_find_by_name(bilP_load.project, name);
 * obj_set_attach_value(bilP_load.obj, AB_CP_SOUTH, (void*)target); break;
 * case AB_ATTACH_GRIDLINE: value = atoi(bilP_load_get_value());
 * obj_set_attach_value(bilP_load.obj, AB_CP_SOUTH, (void*)value); break; }
 * return NULL; }
 * 
 * STRING bilP_load_att_east_value(BIL_TOKEN valueToken) { ABObj	target= NULL;
 * STRING	name= NULL; int		value;
 * 
 * switch (obj_get_attach_type(bilP_load.obj, AB_CP_EAST)) { case AB_ATTACH_OBJ:
 * case AB_ATTACH_ALIGN_OBJ_EDGE: name= bilP_load_get_value(); target=
 * obj_find_by_name(bilP_load.project, name);
 * obj_set_attach_value(bilP_load.obj, AB_CP_EAST, (void*)target); break;
 * case AB_ATTACH_GRIDLINE: value = atoi(bilP_load_get_value());
 * obj_set_attach_value(bilP_load.obj, AB_CP_EAST, (void*)value); break; }
 * return NULL; }
 * 
 * STRING bilP_load_att_west_value(BIL_TOKEN valueToken) { ABObj	target= NULL;
 * STRING	name= NULL; int		value;
 * 
 * switch (obj_get_attach_type(bilP_load.obj, AB_CP_WEST)) { case AB_ATTACH_OBJ:
 * case AB_ATTACH_ALIGN_OBJ_EDGE: name= bilP_load_get_value(); target=
 * obj_find_by_name(bilP_load.project, name);
 * obj_set_attach_value(bilP_load.obj, AB_CP_WEST, (void*)target); break;
 * case AB_ATTACH_GRIDLINE: value = atoi(bilP_load_get_value());
 * obj_set_attach_value(bilP_load.obj, AB_CP_WEST, (void*)value); break; }
 * return NULL; }
 */

STRING
bilP_load_att_is_help_item(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;
    BOOL                is_help_item;

    set_att(":is-help-item");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    is_help_item = bilP_token_to_bool(valueToken);
    obj_set_is_help_item(obj, is_help_item);
    return NULL;
}

STRING
bilP_load_att_initial_state(BIL_TOKEN valueToken)
{
    nset_att(AB_BIL_INITIAL_STATE);
    return NULL;
/*
    ABObj               obj = bilP_load.obj;
    BIL_TOKEN           init_state = AB_BIL_UNDEF;
    AB_OBJECT_STATE     obj_state = AB_STATE_UNDEF;

    nset_att(AB_BIL_INITIAL_STATE);
    init_state = bilP_load_get_token();

    util_dprintf(3, "bilP_load_att_initial_state %d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)));

	if (init_state == AB_BIL_UNDEF)
	{
	    abil_print_load_err(ERR_WANT_KEYWORD);
	    return NULL;
	}
    }

    obj_state = bilP_token_to_initial_state(init_state);
    obj_set_initial_state(obj, obj_state);
    return NULL;
*/
}

STRING
bilP_load_attachment_type(BIL_TOKEN valueToken)
{
    AB_ATTACH_TYPE      obj_type = AB_ATTACH_UNDEF;
    int                 type = AB_BIL_UNDEF;

    util_dprintf(3, "bilP_load_attachment_type: %d/%s\n\t%d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)),
	       bilP_load.att,
	       str_safe(bilP_token_to_string(bilP_load.att)));

    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
	abil_print_load_err(ERR_WANT_KEYWORD);
    if (valueToken != AB_BIL_NIL)
    {
	obj_type = bilP_token_to_attachment(valueToken);
	switch (bilP_load.att)
	{
	case AB_BIL_EAST_ATTACHMENT:
	    obj_set_attach_type(bilP_load.obj, AB_CP_EAST, obj_type);
	    break;
	case AB_BIL_WEST_ATTACHMENT:
	    obj_set_attach_type(bilP_load.obj, AB_CP_WEST, obj_type);
	    break;
	case AB_BIL_NORTH_ATTACHMENT:
	    obj_set_attach_type(bilP_load.obj, AB_CP_NORTH, obj_type);
	    break;
	case AB_BIL_SOUTH_ATTACHMENT:
	    obj_set_attach_type(bilP_load.obj, AB_CP_SOUTH, obj_type);
	    break;
	}
    }
    return NULL;
}

STRING
bilP_load_attachment_value(BIL_TOKEN valueToken)
{
    ABObj               target = NULL;
    STRING              name = NULL;
    long                value = 0;
    AB_COMPASS_POINT    cp = AB_CP_UNDEF;

    util_dprintf(3, "bilP_load_attachment_value: %d/%s\n\t%d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)),
	       bilP_load.att,
	       str_safe(bilP_token_to_string(bilP_load.att)));

    switch (bilP_load.att)
    {
	case AB_BIL_EAST_ATTACHMENT:
	    cp = AB_CP_EAST;
	    break;
    	case AB_BIL_WEST_ATTACHMENT:
	    cp = AB_CP_WEST;
	    break;
    	case AB_BIL_NORTH_ATTACHMENT:
	    cp = AB_CP_NORTH;
	    break;
    	case AB_BIL_SOUTH_ATTACHMENT:
	    cp = AB_CP_SOUTH;
	    break;
    }

    switch (obj_get_attach_type(bilP_load.obj, cp))
    {
	case AB_ATTACH_OBJ:
	case AB_ATTACH_ALIGN_OBJ_EDGE:
	    name = bilP_load_get_value();
	    target = obj_scoped_find_or_create_undef(bilP_load.module, name,
			AB_TYPE_UNKNOWN);
	    obj_set_attach_value(bilP_load.obj, cp, (void *) target);
	    break;
	case AB_ATTACH_GRIDLINE:
	case AB_ATTACH_CENTER_GRIDLINE:
	    value = atoi(bilP_load_get_value());
	    obj_set_attach_value(bilP_load.obj, cp, (void *) value);
	    break;
    }

    return NULL;
}

STRING
bilP_load_attachment_offset(BIL_TOKEN valueToken)
{
    AB_COMPASS_POINT    cp = AB_CP_UNDEF;
    BIL_TOKEN           type = AB_BIL_UNDEF;
    int                 value = 0;

    util_dprintf(3, "bilP_load_attachment_offset: %d/%s\n\t%d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)),
	       bilP_load.att,
	       str_safe(bilP_token_to_string(bilP_load.att)));

    switch (bilP_load.att)
    {
    case AB_BIL_EAST_ATTACHMENT:
	cp = AB_CP_EAST;
	break;
    case AB_BIL_WEST_ATTACHMENT:
	cp = AB_CP_WEST;
	break;
    case AB_BIL_NORTH_ATTACHMENT:
	cp = AB_CP_NORTH;
	break;
    case AB_BIL_SOUTH_ATTACHMENT:
	cp = AB_CP_SOUTH;
	break;
    }
    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
    {
	value = atoi(bilP_load_get_value());
	obj_set_attach_offset(bilP_load.obj, cp, value);
    }
    else
	abil_print_load_err(ERR_WANT_INTEGER);

    return NULL;
}

/*
STRING
bilP_load_att_parent(BIL_TOKEN valueToken)
{
    ABObj               newObj = NULL;
    STRING              stringValue = bilP_load_get_value();

    util_dprintf(3, "bilP_load_att_parent: %d/%s\n\t%d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)),
	       bilP_load.att,
	       str_safe(bilP_token_to_string(bilP_load.att)));

    nset_att(AB_BIL_PARENT);
    if (valueToken == AB_BIL_VALUE_IDENT)
    {
	newObj = obj_find_by_name(bilP_load.module, stringValue);
	if (newObj == NULL)
	{

	    newObj = obj_create(AB_TYPE_UNKNOWN, NULL);
	    obj_set_name(newObj, stringValue);
	}
	obj_set_win_parent(bilP_load.obj, newObj);
    }
    else
	abil_print_load_err(ERR_WANT_NAME);

    return NULL;
}
*/

STRING
bilP_load_att_resizable(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    BOOL                resizable;

    util_dprintf(3, "bilP_load_att_resizable %d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)));

    set_att(":resizable");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    resizable = bilP_token_to_bool(valueToken);
    obj_set_resizable(obj, resizable);
    return NULL;
}

/* REMIND: (mcv) take out when bil files have been converted.
 * AB_BIL_SB_ALWAYS_VISIBLE is an obsolete attribute.
 * AB_BIL_HSCROLL and AB_BIL_VSCROLL now take values
 * of :never, :always, :when-needed, thus making this
 * attribute unnessary.  This routine is here so that
 * old bil files won't break.
 */
STRING
bilP_load_att_sb_always_visible(BIL_TOKEN valueToken)
{
    return NULL;
}

STRING
bilP_load_att_label_position(BIL_TOKEN valueToken)
{
    AB_COMPASS_POINT    pos = AB_CP_UNDEF;
    int                 val = AB_BIL_UNDEF;

    util_dprintf(3, "bilP_load_att_label_position %d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)));

    if ((val = bilP_load_get_token()) == AB_BIL_UNDEF)
    {
        abil_print_load_err(ERR_WANT_KEYWORD);
        return NULL;
    }

    nset_att(AB_BIL_LABEL_POSITION);
    pos = bilP_token_to_compass_point(valueToken);
    obj_set_label_position(bilP_load.obj, pos);
    return NULL;
}

STRING
bilP_load_att_help_volume(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             tmp_str = NULL;

    set_att(":help-volume");
    tmp_str = bilP_get_string();
    if (!obj_is_action(obj))
	obj_set_help_volume(obj, istr_string(tmp_str));
    else
	obj_set_func_help_volume(obj, istr_string(tmp_str));
    istr_destroy(tmp_str);

    return NULL;
}

STRING
bilP_load_att_help_location(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    ISTRING             tmp_str = NULL;

    set_att(":help-location");
    tmp_str = bilP_get_string();
    if (!obj_is_action(obj))
	obj_set_help_location(obj, istr_string(tmp_str));
    else
	obj_set_func_help_location(obj, istr_string(tmp_str));
    istr_destroy(tmp_str);

    return NULL;
}

STRING
bilP_load_att_word_wrap(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    BOOL                word_wrap = FALSE;

    util_dprintf(3, "bilP_load_att_resizable %d/%s\n",
	       valueToken,
	       str_safe(bilP_token_to_string(valueToken)));

    set_att(":word-wrap");
    if (!bilP_token_is_bool(valueToken))
    {
	abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    word_wrap = bilP_token_to_bool(valueToken);
    obj_set_word_wrap(obj, word_wrap);
    return NULL;
}

/* 
 * Here for backwards compatibility. 
 */
STRING
bilP_load_att_win_parent(BIL_TOKEN valueToken)
{
    return NULL;
}

STRING
bilP_load_att_line_style(BIL_TOKEN valueToken)
{
    AB_LINE_TYPE      lineStyle = AB_LINE_UNDEF;

    set_att(":line-style");
    lineStyle = bilP_token_to_line_style(valueToken);
    obj_set_line_style(bilP_load.obj, lineStyle);

    return NULL;
}

STRING
bilP_load_att_arrow_style(BIL_TOKEN valueToken)
{
    AB_ARROW_STYLE      arrowStyle = AB_ARROW_UNDEF;

    set_att(":arrow-style");
    arrowStyle = bilP_token_to_arrow_style(valueToken);
    obj_set_arrow_style(bilP_load.obj, arrowStyle);

    return NULL;
}

STRING
bilP_load_att_active(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;

    set_att(":active");
    if (!bilP_token_is_bool(valueToken))
        abil_print_load_err(ERR_WANT_BOOLEAN);
    obj_set_is_initially_active(obj, bilP_token_to_bool(valueToken));

    return NULL;
}

STRING
bilP_load_att_iconic(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 type = AB_BIL_UNDEF;
    STRING              value = NULL;

    set_att(":iconic");
    if (!bilP_token_is_bool(valueToken))
        abil_print_load_err(ERR_WANT_BOOLEAN);
    obj_set_is_initially_iconic(obj, bilP_token_to_bool(valueToken));

    return NULL;
}

STRING
bilP_load_att_combo_box_style(BIL_TOKEN valueToken)
{
/*
  NOW Being replaced with "read_only"

    AB_COMBO_BOX_STYLE      comboboxStyle = AB_COMBO_BOX_UNDEF;

    set_att(":combobox-style");

    comboboxStyle = bilP_token_to_combo_box_style(valueToken);
    obj_set_combo_box_style(bilP_load.obj, comboboxStyle);
*/

    return NULL;
}

STRING
bilP_load_att_show_value( BIL_TOKEN valueToken)
{
    BOOL	show_val = FALSE;
    ABObj	obj = bilP_load.obj;

    util_dprintf(3, "bilP_load_att_show_value %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    set_att(":show-value");
    if (!bilP_token_is_bool(valueToken))
    {
        abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    show_val = bilP_token_to_bool(valueToken);
    obj_set_show_value(obj, show_val);
    return NULL;
}

STRING
bilP_load_att_slider_type( BIL_TOKEN valueToken)
{
/*
    ABObj		obj = bilP_load.obj;
    AB_SLIDER_TYPE      obj_type = AB_SLIDER_UNDEF;
    int                 type;

    util_dprintf(3, "bilP_load_att_slider_type %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
    
    nset_att(AB_BIL_SLIDER_TYPE);
    if ((type = bilP_load_get_token()) == AB_BIL_UNDEF)
        abil_print_load_err(ERR_WANT_KEYWORD);
    obj_type = bilP_token_to_slider_type(type);
    if (!obj_is_slider(obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    obj_set_slider_type(obj, obj_type);
*/
/* Replaced with read_only */

    return NULL;
} 

STRING
bilP_load_att_selection_mode(BIL_TOKEN valueToken)
{
    AB_SELECT_TYPE	selectionMode = AB_SELECT_UNDEF;

    set_att(":selection-mode");
    selectionMode = bilP_token_to_selection_mode(valueToken);
    obj_set_selection_mode(bilP_load.obj, selectionMode);

    return NULL;
}

STRING
bilP_load_att_ok_label(BIL_TOKEN valueToken)
{
    ISTRING	ok_label;

    util_dprintf(3, "bilP_load_att_ok_label %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    set_att(":ok-label");
    if (!obj_is_file_chooser(bilP_load.obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    ok_label = bilP_get_string();
    obj_set_ok_label(bilP_load.obj, istr_string(ok_label));
    istr_destroy(ok_label);
    return NULL;
}

STRING
bilP_load_att_auto_dismiss(BIL_TOKEN valueToken)
{
    BOOL	auto_dismiss;

    util_dprintf(3, "bilP_load_att_auto_dismiss %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
 
    set_att(":auto-dismiss");
    if (!obj_is_file_chooser(bilP_load.obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    if (!bilP_token_is_bool(valueToken))
    {
        abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    auto_dismiss = bilP_token_to_bool(valueToken);
    obj_set_auto_dismiss(bilP_load.obj, auto_dismiss);
    return NULL; 
}

STRING
bilP_load_att_directory(BIL_TOKEN valueToken)
{ 
    ISTRING        dir;
 
    util_dprintf(3, "bilP_load_att_directory %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
  
    set_att(":directory");                                
    if (!obj_is_file_chooser(bilP_load.obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    dir = bilP_get_string();
    obj_set_directory(bilP_load.obj, istr_string(dir));
    istr_destroy(dir);
    return NULL;
}

STRING 
bilP_load_att_file_type_mask(BIL_TOKEN valueToken) 
{  
    AB_FILE_TYPE_MASK	ftm;
  
    util_dprintf(3, "bilP_load_att_file_type_mask %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken))); 
   
    set_att(":file-type-mask");                                 
    if (!obj_is_file_chooser(bilP_load.obj)) 
        abil_print_load_err(ERR_UNKNOWN_OBJ); 
    ftm = bilP_token_to_file_type_mask(valueToken); 
    obj_set_file_type_mask(bilP_load.obj, ftm);
    return NULL; 
} 

STRING
bilP_load_att_message(BIL_TOKEN valueToken)
{
    ISTRING        msg;

    util_dprintf(3, "bilP_load_att_message %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    set_att(":message");                             
    if (!obj_is_message(bilP_load.obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    msg = bilP_get_string();
    obj_set_msg_string(bilP_load.obj, istr_string(msg));
    istr_destroy(msg);
    return NULL;
}

/* Here for backwards compatibility */
STRING
bilP_load_att_action1_button(BIL_TOKEN valueToken)
{
    util_dprintf(3, "bilP_load_att_action1_button %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
    return NULL;
}

/* Here for backwards compatibility */
STRING
bilP_load_att_action2_button(BIL_TOKEN valueToken)
{
    util_dprintf(3, "bilP_load_att_action2_button %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
    return NULL;
}

STRING
bilP_load_att_cancel_button(BIL_TOKEN valueToken)
{
    BOOL        cancel_button;
 
    util_dprintf(3, "bilP_load_att_cancel_button %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
 
    set_att(":cancel-button");
    if (!obj_is_message(bilP_load.obj))
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    if (!bilP_token_is_bool(valueToken))
    {
        abil_print_load_err(ERR_WANT_BOOLEAN);
    }
    cancel_button = bilP_token_to_bool(valueToken);
    obj_set_cancel_button(bilP_load.obj, cancel_button);
    return NULL;
}

STRING
bilP_load_att_help_button(BIL_TOKEN valueToken)
{
    BOOL        help_button;
    ABObj	buttonObj;
    STRING	strValue;
 
    util_dprintf(3, "bilP_load_att_help_button %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
 
    set_att(":help-button");
    if (obj_is_message(bilP_load.obj)) {
        if (!bilP_token_is_bool(valueToken))
        {
            abil_print_load_err(ERR_WANT_BOOLEAN);
        }
        help_button = bilP_token_to_bool(valueToken);
        obj_set_help_button(bilP_load.obj, help_button);
        return NULL;
    }
    else if (obj_is_popup_win(bilP_load.obj)) {
        if (bilP_load_get_value_type() == AB_BIL_VALUE_IDENT)
        {
            strValue = bilP_load_get_value();
            if (strValue != NULL)
            {
                buttonObj = obj_scoped_find_or_create_undef(bilP_load.module,
                                strValue, AB_TYPE_UNKNOWN);

                obj_set_help_act_button(bilP_load.obj, buttonObj);
            }
        }
        else
        {
            abil_print_load_err(ERR_WANT_STRING);
        }
	return NULL;
    }
    else
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    return NULL;
}

STRING
bilP_load_att_action1_label(BIL_TOKEN valueToken)
{
    ISTRING     action1_label;

    util_dprintf(3, "bilP_load_att_action1_label %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    set_att(":action1-label");
    if (!obj_is_message(bilP_load.obj))
    {
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    }
    action1_label = bilP_get_string();
    obj_set_action1_label(bilP_load.obj, istr_string(action1_label));
    istr_destroy(action1_label);
    return NULL;
}

STRING
bilP_load_att_action2_label(BIL_TOKEN valueToken)
{
    ISTRING     action2_label;

    util_dprintf(3, "bilP_load_att_action2_label %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));

    set_att(":action2-label");
    if (!obj_is_message(bilP_load.obj))
    {
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    }
    action2_label = bilP_get_string();
    obj_set_action2_label(bilP_load.obj, istr_string(action2_label));
    istr_destroy(action2_label);
    return NULL;
}

STRING
bilP_load_att_action3_label(BIL_TOKEN valueToken)
{
    ISTRING     action3_label;
 
    util_dprintf(3, "bilP_load_att_action3_label %d/%s\n",
               valueToken,
               str_safe(bilP_token_to_string(valueToken)));
 
    set_att(":action3-label");
    if (!obj_is_message(bilP_load.obj))
    {
        abil_print_load_err(ERR_UNKNOWN_OBJ);
    }
    action3_label = bilP_get_string();
    obj_set_action3_label(bilP_load.obj, istr_string(action3_label));
    istr_destroy(action3_label);
    return NULL;
}

STRING
bilP_load_att_default_button(BIL_TOKEN valueToken)
{
    int                 button = AB_BIL_UNDEF;
    AB_DEFAULT_BUTTON	obj_default_btn;
    ABObj		buttonObj;
    STRING		strValue;
    ABObj               obj = bilP_load.obj;

    set_att(":default-button");

    if (!obj_is_popup_win(obj) && !obj_is_message(obj))
    {
        abil_print_load_err(ERR_UNKNOWN_OBJECT);
	return NULL;
    }

    if (obj_is_popup_win(obj)) /* Looking for Button Object Name */
    {
    	if (bilP_load_get_value_type() == AB_BIL_VALUE_IDENT)
    	{
            strValue = bilP_load_get_value();
            if (strValue != NULL)
            {
                buttonObj = obj_scoped_find_or_create_undef(bilP_load.module,
                                strValue, AB_TYPE_UNKNOWN);
	
		obj_set_default_act_button(obj, buttonObj);
            }
    	}
    	else
 	{
            abil_print_load_err(ERR_WANT_STRING);
	}
    }
    else if (obj_is_message(obj))
    {
	if ((button = bilP_load_get_token()) == AB_BIL_UNDEF)
	{
            abil_print_load_err(ERR_WANT_KEYWORD);
	}
	else
	{
	    obj_default_btn = bilP_token_to_default_button(button);
	    if (obj_default_btn == AB_DEFAULT_BTN_UNDEF)
	    {
		abil_print_load_err(ERR_BAD_ATT_FOR_OBJ);
	    }
	    else
	    {
		obj_set_default_btn(obj, obj_default_btn);
	    }
	}
    }
 
    return NULL;
}

STRING
bilP_load_att_pane_max_height(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;

    set_att(":pane-max-height");

    if (!obj_is_pane(obj) && !obj_is_layers(obj))
        abil_print_load_err(ERR_UNKNOWN_OBJECT);

    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
        value = atoi(bilP_load_get_value());
    else
        abil_print_load_err(ERR_WANT_INTEGER);
    obj_set_pane_max(obj, value);

    return NULL;
}

STRING
bilP_load_att_pane_min_height(BIL_TOKEN valueToken)
{
    ABObj               obj = bilP_load.obj;
    int                 value = 0;
    int                 type;
 
    set_att(":pane-min-height");

    if (!obj_is_pane(obj) && !obj_is_layers(obj))
        abil_print_load_err(ERR_UNKNOWN_OBJECT);

    if ((type = bilP_load_get_value_type()) == AB_BIL_VALUE_INT)
        value = atoi(bilP_load_get_value());
    else 
        abil_print_load_err(ERR_WANT_INTEGER);
    obj_set_pane_min(obj, value); 
 
    return NULL;
}

STRING
bilP_load_att_i18n_enabled(BIL_TOKEN valueToken)
{
    ABObj               	obj = bilP_load.obj;
    BOOL			i18n_enabled;

    set_att(":enabled");
    if (!obj_is_project(obj))
	abil_print_load_err(ERR_UNKNOWN_OBJ);
    i18n_enabled = bilP_token_to_bool(valueToken);
    obj_set_i18n_enabled(obj, i18n_enabled);

    return NULL;
}

STRING
bilP_load_att_win_children_begin(void)
{
    nset_att(AB_BIL_WIN_CHILDREN);
    return NULL;
}

STRING
bilP_load_att_win_children_end(void)
{
    clear_att();
    return NULL;
}
