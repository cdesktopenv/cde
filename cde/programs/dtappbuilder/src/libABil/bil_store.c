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
 *      $XConsortium: bil_store.c /main/3 1995/11/06 18:26:05 rswiston $
 *
 *      @(#)bil_store.c	1.145 22 Mar 1995
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
 *  bil_store.c - storing routines for the ABObj tree.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/types.h>
#include <ab/util_types.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/bil.h>
#include "load.h"
#include "bilP.h"

/*************************************************************************
**
**
**       Constants (#define and const)
**
**
**
**************************************************************************/
/* a value_column that is a multiple of 8 saves a good deal of space, 
 * because tabs can be used, instead of spaces.
 */
#define VALUE_COLUMN            32      /* column to start value */

/*************************************************************************
**
**
**       Private Functions (C declarations and macros)
**
**
**
**************************************************************************/

#define get_from_name(obj)	(obj_get_name(obj_get_from(obj)))
#define get_to_name(obj)	(obj_get_name(obj_get_to(obj)))

#ifdef BOGUS
static int		store_project_tree(
    			    ABObj		root,
    			    STRING		filename,
			    BIL_SAVE_TYPE	save_type
			);
#endif /* BOGUS */

static BIL_TOKEN *	get_object_attrs(
        		    ABObj	obj
			);
static int		store_object(
			    FILE		*outFile,
    			    ABObj       	obj,
			    BIL_SAVE_TYPE	save_type
			);
static int 		store_attribute(
			    FILE	*outFile,
    			    ABObj	obj,
    			    BIL_TOKEN	attr,
			    BIL_SAVE_TYPE	save_type
			);
static int		store_res_file_types(FILE *outFile, ABObj obj);
static int		store_dnd_ops(FILE *outFile, ABDndOpFlags ops);
static int		store_dnd_types(FILE *outFile, ABDndTypeFlags types);
static int 		store_action(
			    FILE	*outFile,
    			    ABObj	obj,
			    BIL_SAVE_TYPE	save_type
			);
static STRING		create_bil_file_list(
			    ABObj 	proj
			);
static int		bilP_write_tree(
    		  	    FILE	*outFile,
    			    ABObj	root,
			    BIL_SAVE_TYPE	save_type
			);
#ifdef BOGUS
static int		save_cross_module_actions(
			    FILE        *outFile,
			    ABObj       root,
			    BIL_SAVE_TYPE	save_type
			);
#endif /* BOGUS */
static BOOL		attr_is_undef(
			    BIL_TOKEN	attr,
			    ABObj	obj
			);
static BOOL		skip_attr(
			    BIL_TOKEN		attr,
			    ABObj		obj,
			    BIL_SAVE_TYPE	save_type
		  	);

static int		bilP_write_tree_ui_objs(
			    FILE		*outFile,
    			    ABObj		root,
    			    BIL_SAVE_TYPE	save_type
			);
static int		bilP_write_tree_conn_objs(
    			    FILE		*outFile,
    			    ABObj		root,
    			    BIL_SAVE_TYPE	save_type
			);


/*
 * DEBUGGING ROUTINES
 */
#ifdef DEBUG
static int		util_dabort_impl(int abort_debug_level);

static BOOL 		attachment_is_ok(
			    ABObj               obj,
			    AB_COMPASS_POINT    attach_dir
			);
static int		check_attachments(
			    ABObj	obj
			);
static int 		bilP_obj_verify(
			    ABObj       obj
			);
#endif /* DEBUG */


#ifdef DEBUG
#define util_dabort(_lvl) util_dabort_impl(_lvl)
#else
#define util_dabort(_lvl) /* ignore this */
#endif

/*************************************************************************
**
**
**       Function Definitions
**
**
**
**************************************************************************/

/*
 *
 */
static BOOL
attr_is_undef(
    BIL_TOKEN	attr,
    ABObj	obj
)
{
    char 		buf[256];
    *buf = 0;

    switch (attr)
    {
	case AB_BIL_BG_COLOR:
	    return( obj_get_bg_color(obj) == NULL );    

	case AB_BIL_CHILDREN:
	    return (obj_get_num_salient_ui_children(obj) == 0);

	case AB_BIL_WIN_CHILDREN:
	    return (obj_get_num_win_children(obj) == 0);

	case AB_BIL_DRAG_CURSOR:
	    return util_strempty(obj_get_drag_cursor(obj));
	break;

	case AB_BIL_DRAG_CURSOR_MASK:
	    return util_strempty(obj_get_drag_cursor_mask(obj));
	break;

	case AB_BIL_DRAG_ENABLED:
	    return (   (obj_get_drag_ops(obj) == 0) /* no dnd at all */
		    || (obj_get_drag_types(obj) == 0) );
	break;

	case AB_BIL_DRAG_OPS:
	    return (obj_get_drag_ops(obj) == 0);
	break;

	case AB_BIL_DRAG_TYPES:
	    return (obj_get_drag_types(obj) == 0);
	break;

	case AB_BIL_DROP_ENABLED:
	case AB_BIL_DROP_SITE_CHILD_ALLOWED: /* undef if drop-enabled undef */
	    return (   (obj_get_drop_ops(obj) == 0) /* no dnd at all */
		    || (obj_get_drop_types(obj) == 0) );
	break;

	case AB_BIL_DROP_OPS:
	    return (obj_get_drop_ops(obj) == 0);
	break;

	case AB_BIL_DROP_TYPES:
	    return (obj_get_drop_types(obj) == 0);
	break;

	case AB_BIL_FG_COLOR:
	    return( obj_get_fg_color(obj) == NULL );

	case AB_BIL_HATTACH_TYPE:
	     return (obj_get_hattach_type(obj) == AB_ATTACH_UNDEF);

	case AB_BIL_LABEL:
	    return( util_strempty(obj_get_label(obj)) );

	case AB_BIL_LABEL_TYPE:
	    if ( util_strempty(obj_get_label(obj)) && 
		 (obj_get_label_type(obj) == AB_LABEL_STRING)
		)
	    {
		return (True);
	    }
	    else return (False);

	/* REMIND: No object seems to have this attribute */
	case AB_BIL_PACKING:
	    if ( obj_get_packing(obj) == AB_PACK_UNDEF )		
	    {
		util_error(
		    catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 41,
		       "packing is undefined\n"));
		return (True);
	    }
	    else return (False);

	case AB_BIL_PROCESS_STRING:
	    return (util_strempty(obj_get_process_string(obj)));

	case AB_BIL_MENU:
	    return (util_strempty(obj_get_menu_name(obj)));

	case AB_BIL_MENU_TITLE:
	    return (util_strempty(obj_get_menu_title(obj)));

/*
        case AB_BIL_MENU_TYPE:
	    if ( obj_get_menu_type(obj) == AB_MENU_UNDEF )
	    {
		util_error(
		    catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 42,
		       "menu type is undefined\n"));
		return (True);
	    }
	    return (False);
*/
	case AB_BIL_LABEL_ALIGNMENT:
	    return (util_strempty(obj_get_label(obj)));

	/* REMIND: No object seems to have this attribute */
	case AB_BIL_REFERENCE_POINT:
	    if (obj_get_ref_point(obj) == AB_CP_UNDEF)
	    {
		util_error(
		    catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 42,
			"reference point is undefined\n"));
		return (True);
	    }
	    else return (False);

	case AB_BIL_ACCELERATOR:
	    return(util_strempty(obj_get_accelerator(obj)));

	case AB_BIL_INITIAL_VALUE:
	{
	    AB_OBJECT_TYPE      type;

	    type = obj_get_type(obj);
	    if (type == AB_TYPE_SCALE || type == AB_TYPE_SPIN_BOX)
		return( False );
	    else 
		if (type == AB_TYPE_TEXT_FIELD || 
		    type == AB_TYPE_TEXT_PANE) 
		   return (util_strempty(obj_get_initial_value_string(obj)));
	}

        case AB_BIL_TO: 
	case AB_BIL_ARG_TYPE:
            return (obj_get_func_type(obj) != AB_FUNC_BUILTIN);

	case AB_BIL_VATTACH_TYPE:
	     return (obj_get_vattach_type(obj) == AB_ATTACH_UNDEF);

	case AB_BIL_ARG_VALUE:
	{
	    AB_ARG_TYPE        type = AB_ARG_UNDEF;
	    type = obj_get_arg_type(obj);
	    switch (type)
	    {
		case AB_ARG_FLOAT:
		case AB_ARG_INT:
		    return (False);

		case AB_ARG_VOID_PTR:
		case AB_ARG_STRING:
		    return (util_strempty(obj_get_arg_string(obj)));

		default:
		    return (True);
	    }
	}

	case AB_BIL_NORTH_ATTACHMENT:
	case AB_BIL_EAST_ATTACHMENT:
	case AB_BIL_SOUTH_ATTACHMENT:
	case AB_BIL_WEST_ATTACHMENT:
	{
	    AB_ATTACH_TYPE	attach_type = AB_ATTACH_UNDEF;
	    AB_COMPASS_POINT	attach_dir = AB_CP_UNDEF;
	    switch (attr)
	    {
		case AB_BIL_NORTH_ATTACHMENT: attach_dir = AB_CP_NORTH; break;
		case AB_BIL_EAST_ATTACHMENT: attach_dir = AB_CP_EAST; break;
		case AB_BIL_SOUTH_ATTACHMENT: attach_dir = AB_CP_SOUTH; break;
		case AB_BIL_WEST_ATTACHMENT: attach_dir = AB_CP_WEST; break;
	    }
	    attach_type = obj_get_attach_type(obj, attach_dir);
	    return (   (attach_type == AB_ATTACH_UNDEF)
		    || (attach_type == AB_ATTACH_NONE) );
	}

        case AB_BIL_LABEL_POSITION:
	    return (util_strempty(obj_get_label(obj)));

	case AB_BIL_ICON_FILE:
	    return (util_strempty(obj_get_icon(obj)));

	case AB_BIL_ICON_MASK_FILE:
	    return (util_strempty(obj_get_icon_mask(obj)));

	case AB_BIL_ICON_LABEL:
            return (util_strempty(obj_get_icon_label(obj)));

        case AB_BIL_BORDER_FRAME:
	{
	    AB_LINE_TYPE	border_type = obj_get_border_frame(obj);
	    return (border_type == AB_LINE_UNDEF);
	}

	case AB_BIL_HELP_LOCATION:
	    if (!obj_is_action(obj))
		return (util_strempty(obj_get_help_location(obj)));
	    else
		return (util_strempty(obj_get_func_help_location(obj)));

	case AB_BIL_HELP_TEXT:
	    return (util_strempty(obj_get_help_text(obj)));

	case AB_BIL_HELP_VOLUME:
	    if (!obj_is_action(obj))
		return (util_strempty(obj_get_help_volume(obj)));
	    else
		return (util_strempty(obj_get_func_help_volume(obj)));

	case AB_BIL_LINE_STYLE:
	{
	    AB_LINE_TYPE	line_style = obj_get_line_style(obj);
	    return (line_style == AB_LINE_UNDEF);
	}

        case AB_BIL_MNEMONIC:
            return (util_strempty(obj_get_mnemonic(obj)));

	case AB_BIL_WIN_PARENT:
	    /* A NULL window parent for the dialog is valid. */
	    return (obj_get_win_parent(obj) == NULL);

	case AB_BIL_DEFAULT_BUTTON:
	    if (obj_is_popup_win(obj))
		return(obj_get_default_act_button(obj) == NULL);
	    break;

	case AB_BIL_DIRECTORY:
	    return (util_strempty(obj_get_directory(obj)));

	case AB_BIL_FILTER_PATTERN:
	    return (util_strempty(obj_get_filter_pattern(obj)));

	case AB_BIL_MESSAGE:
	    return (util_strempty(obj_get_msg_string(obj)));

	case AB_BIL_OK_LABEL:
	    return (util_strempty(obj_get_ok_label(obj)));

	case AB_BIL_ACTION:
	    return ((obj_get_func_type(obj) == AB_FUNC_ON_ITEM_HELP)
		|| (obj_get_func_type(obj) == AB_FUNC_HELP_VOLUME));

	case AB_BIL_ACTION1_LABEL:
	    return (util_strempty(obj_get_action1_label(obj)));

	case AB_BIL_ACTION2_LABEL:
	    return (util_strempty(obj_get_action2_label(obj)));

	case AB_BIL_ACTION3_LABEL:
	    return (util_strempty(obj_get_action3_label(obj)));

	/* We do NOT write out the Position for Windows! */
	case AB_BIL_X:
	    return (obj_is_window(obj) || obj_get_x(obj) == -1);

	case AB_BIL_Y:
	    return (obj_is_window(obj) || obj_get_y(obj) == -1);

	case AB_BIL_WIDTH:
	    return (obj_get_width(obj) == -1);

	case AB_BIL_HEIGHT:
	    return (obj_get_height(obj) == -1);

	case AB_BIL_VENDOR:
	    return (util_strempty(obj_get_vendor(obj)));

	case AB_BIL_VERSION:
	    return (util_strempty(obj_get_version(obj)));

        case AB_BIL_ROOT_WINDOW:
            return (obj_get_root_window(obj) == NULL);

	case AB_BIL_PANE_MAX_HEIGHT:
	{
	    ABObj	parent = NULL;
	    parent = obj_get_parent(obj);
	    if ((parent != NULL) && obj_is_paned_win(parent))
		return (obj_get_pane_max(obj) == 1000);
	    else 
		return (True);
	}

        case AB_BIL_PANE_MIN_HEIGHT:
	{
	    ABObj	parent = NULL;
            parent = obj_get_parent(obj); 
            if ((parent != NULL) && obj_is_paned_win(parent))
		return (obj_get_pane_min(obj) == 1); 
            else 
		return (True);
	}	    

	case AB_BIL_HELP_BUTTON:
	    if (obj_is_popup_win(obj))
	    {
		ABObj	helpb = NULL;
		helpb = obj_get_help_act_button(obj);
		return (helpb == NULL);
	    }
	    else return (False);

    } /* End switch (attr) */

    return False;
}


/*
 * Returns TRUE if the attribute should not be written to the BIL file.
 * Generally, this is true if the attribute has a default or undefined value
 */
static BOOL
skip_attr(
    BIL_TOKEN		attr,
    ABObj		obj,
    BIL_SAVE_TYPE	save_type
)
{
    ABObj	parent;

    save_type = save_type;		/* avoid compiler warning */

    if (attr_is_undef(attr, obj))
    {
	return True;
    }

    /*
     * Put out only column stuff for column groups and only
     * row stuff for row groups.
     */
    if (obj_is_group(obj))
    {
        if ((obj_get_group_type(obj) == AB_GROUP_ROWS) && 
	    (attr == AB_BIL_COLUMN_ALIGNMENT ||
	     attr == AB_BIL_NUM_ROWS ||
	     attr == AB_BIL_NUM_COLUMNS ||
	     attr == AB_BIL_VOFFSET ||
	     attr == AB_BIL_VATTACH_TYPE))
	    return TRUE;

        if ((obj_get_group_type(obj) == AB_GROUP_COLUMNS) && 
	    (attr == AB_BIL_ROW_ALIGNMENT ||
	     attr == AB_BIL_NUM_ROWS ||
	     attr == AB_BIL_NUM_COLUMNS ||
	     attr == AB_BIL_HOFFSET ||
	     attr == AB_BIL_HATTACH_TYPE))
	    return TRUE;

        if ((obj_get_group_type(obj) == AB_GROUP_IGNORE) && 
	    (attr == AB_BIL_COLUMN_ALIGNMENT ||
	     attr == AB_BIL_NUM_ROWS ||
	     attr == AB_BIL_NUM_COLUMNS ||
	     attr == AB_BIL_VOFFSET ||
	     attr == AB_BIL_VATTACH_TYPE ||
	     attr == AB_BIL_ROW_ALIGNMENT ||
	     attr == AB_BIL_NUM_ROWS ||
	     attr == AB_BIL_NUM_COLUMNS ||
	     attr == AB_BIL_HOFFSET ||
	     attr == AB_BIL_HATTACH_TYPE))
	    return TRUE;
    }

    parent = obj_get_parent(obj);

    if (parent)
        parent = obj_get_root(parent);

    /*
     * object is group member
     * If group type is anything other than AB_GROUP_IGNORE, don't write out
     * position and attachment attributes
     */
    if (parent && 
	obj_is_group(parent) && 
	(obj_get_group_type(parent) != AB_GROUP_IGNORE) &&
	    (attr == AB_BIL_X ||
	     attr == AB_BIL_Y ||
	     attr == AB_BIL_NORTH_ATTACHMENT ||
	     attr == AB_BIL_SOUTH_ATTACHMENT ||
	     attr == AB_BIL_EAST_ATTACHMENT ||
	     attr == AB_BIL_WEST_ATTACHMENT))
        return TRUE;

    /*
     * Only write out drag options if drag and drop is actually being used
     * on this object
     */
    if (   (attr == AB_BIL_DRAG_TO_ROOT_ALLOWED)
	&& attr_is_undef(AB_BIL_DRAG_OPS, obj) )
    {
	/* drag not enabled - don't bother with this att */
	return TRUE;
    }

    if (obj_is_project(obj))
    {
	if ((attr == AB_BIL_FILES) && (save_type != BIL_SAVE_FILE_PER_MODULE))
	{
	    /* encapsulated or dump - don't write files attribute */
	    return TRUE;
	}
    }

    return FALSE;
}


STRING
bil_get_filename(
    ABObj proj,
    ABObj module
)
{
    char	proj_dir[MAXPATHLEN]= "", 
		module_dir[MAXPATHLEN]= "",
    		module_file[MAXPATHLEN]= "";
    STRING	proj_fullpath, mod_fullpath;

    if ( proj != NULL && module != NULL )
    {
	proj_fullpath = obj_get_file(proj);
	mod_fullpath = obj_get_file(module);
	if (proj_fullpath != NULL && mod_fullpath != NULL)
	{
	    util_get_dir_name_from_path(proj_fullpath, proj_dir, MAXPATHLEN);
	    util_get_dir_name_from_path(mod_fullpath, module_dir, MAXPATHLEN);
	    util_get_file_name_from_path(mod_fullpath, module_file, MAXPATHLEN);

	    /* If the project file is saved in the same directory
	     * as the bil file, then simply return the name of the
	     * bil file.
	     */
	    if (strcmp(proj_dir, module_dir) == 0)
	    {
		return(module_file);
	    }
	    /* Else return the absolute path for the bil file */
	    else
	    {	
		return(obj_get_file(module));
	    }
	}
 	else
	{
	    if (proj_fullpath == NULL && mod_fullpath != NULL)
	    {
		return(mod_fullpath);
	    }
	    else return NULL;
	}
    }
    return NULL;
}
    
/*
 * Return a list of object attributes for the specified object type.
 */
static BIL_TOKEN        * 
get_object_attrs(
    ABObj	obj
)
{
    BIL_TOKEN		*returnAttrs = NULL;
    AB_OBJECT_TYPE	type;

    type = obj_get_type(obj);
    if ( type == AB_TYPE_ITEM )
    {
	AB_ITEM_TYPE	subtype;
	
	subtype = obj_get_item_type(obj);
	switch (subtype)
	{
	    case AB_ITEM_FOR_CHOICE:
	    {
                static BIL_TOKEN  attrs[] = 
		{
			AB_BIL_TYPE,
                        AB_BIL_ITEM_TYPE,
			AB_BIL_LABEL_TYPE,
			AB_BIL_LABEL,
			AB_BIL_ACTIVE,
			AB_BIL_SELECTED,
			AB_BIL_BG_COLOR,
			AB_BIL_FG_COLOR,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,
                };
                         
                returnAttrs = attrs;
	    }
	    break;
            case AB_ITEM_FOR_COMBO_BOX:
            {
                static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,
                        AB_BIL_ITEM_TYPE,
                        AB_BIL_LABEL,
                        AB_BIL_SELECTED,
                        -1,
                };

                returnAttrs = attrs;
            }
	    break;
	    case AB_ITEM_FOR_LIST:
	    {
                static BIL_TOKEN  attrs[] =
                {
			AB_BIL_TYPE,
                        AB_BIL_ITEM_TYPE,
			AB_BIL_LABEL,
			AB_BIL_SELECTED,
                        -1,
                };
                         
                returnAttrs = attrs;
	    }
	    break;
	    case AB_ITEM_FOR_MENU:
	    {
                static BIL_TOKEN  attrs[] =
                {
			AB_BIL_TYPE,
                        AB_BIL_ITEM_TYPE,
			AB_BIL_LABEL_TYPE,
			AB_BIL_LABEL,
			AB_BIL_ACCELERATOR,
			AB_BIL_MNEMONIC,
                        AB_BIL_LINE_STYLE,
			AB_BIL_MENU,
                        AB_BIL_BG_COLOR,
                        AB_BIL_FG_COLOR,
                        AB_BIL_ACTIVE, 
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,
                };
                         
                returnAttrs = attrs;
	    }
	    break;
	    case AB_ITEM_FOR_MENUBAR:
	    {
                static BIL_TOKEN  attrs[] =
                {
			AB_BIL_TYPE,
                        AB_BIL_ITEM_TYPE,
			AB_BIL_LABEL_TYPE,
			AB_BIL_LABEL,
			AB_BIL_IS_HELP_ITEM,
                        AB_BIL_MNEMONIC,
			AB_BIL_MENU,
                        AB_BIL_BG_COLOR, 
                        AB_BIL_FG_COLOR,
                        AB_BIL_ACTIVE, 
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,
                };
                         
                returnAttrs = attrs;
	    }
	    break;
            case AB_ITEM_FOR_SPIN_BOX:
            {
                static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,
                        AB_BIL_ITEM_TYPE,
                        AB_BIL_LABEL,
                        AB_BIL_SELECTED,
                        -1,
                };

                returnAttrs = attrs;
            }
	    break;
	}
    }
    else	
    {
	switch (type)
	{
            case AB_TYPE_PROJECT:
            {
                static BIL_TOKEN  attrs[] =
                {
			AB_BIL_FILES,
			AB_BIL_VENDOR,
			AB_BIL_VERSION,
			AB_BIL_ROOT_WINDOW,
			AB_BIL_SESSION_MGMT,
			AB_BIL_TOOLTALK,
			AB_BIL_I18N,
			AB_BIL_RES_FILE_TYPES,
                        -1,
                };
                
                returnAttrs = attrs;
            }
	    break;
                 
            case AB_TYPE_MODULE:
            {
                static BIL_TOKEN  attrs[] =
                {
                        -1,
                };
                
                returnAttrs = attrs;
            }
	    break;
                 
        case AB_TYPE_BASE_WINDOW:
        {
                static BIL_TOKEN  attrs[] =
                {
			AB_BIL_TYPE,
			AB_BIL_X,
			AB_BIL_Y, 
			AB_BIL_WIDTH,
			AB_BIL_HEIGHT,
			AB_BIL_BG_COLOR,
			AB_BIL_FG_COLOR,
			AB_BIL_ICON_FILE,
			AB_BIL_ICON_MASK_FILE,
			AB_BIL_ICON_LABEL,
			AB_BIL_LABEL,
			AB_BIL_RESIZABLE,
                        AB_BIL_VISIBLE, 
			AB_BIL_ICONIC,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
			AB_BIL_DRAG_ENABLED,
			AB_BIL_DRAG_CURSOR,
			AB_BIL_DRAG_CURSOR_MASK,
			AB_BIL_DRAG_OPS,
			AB_BIL_DRAG_TO_ROOT_ALLOWED,
			AB_BIL_DRAG_TYPES,
			AB_BIL_DROP_ENABLED,
			AB_BIL_DROP_SITE_CHILD_ALLOWED,
			AB_BIL_DROP_OPS,
			AB_BIL_DROP_TYPES,
			AB_BIL_CHILDREN,
			AB_BIL_WIN_CHILDREN,
                        -1,
                };
                
                returnAttrs = attrs;
        }
	break;


        case AB_TYPE_COMBO_BOX:   
        {    
                static BIL_TOKEN  attrs[] =    
                {         
                        AB_BIL_TYPE,      
			AB_BIL_LABEL_TYPE,
                        AB_BIL_LABEL,
                        AB_BIL_LABEL_POSITION,
			AB_BIL_READ_ONLY,
			AB_BIL_VISIBLE,
			AB_BIL_ACTIVE,
                        AB_BIL_X,      
                        AB_BIL_Y,      
                        AB_BIL_WIDTH,      
                        AB_BIL_HEIGHT,      
                        AB_BIL_BG_COLOR,      
                        AB_BIL_FG_COLOR,      
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        AB_BIL_CHILDREN,
                        -1,   
                };  
            
                returnAttrs = attrs;
        } 
	break;

        case AB_TYPE_CONTAINER:
        {
	    AB_CONTAINER_TYPE	cont_type;
	    
	    cont_type = obj_get_container_type(obj);
	    switch (cont_type)
	    {
		case AB_CONT_GROUP:
		{
		    static BIL_TOKEN  attrs[] =
		    {
			    AB_BIL_TYPE,
			    AB_BIL_CONTAINER_TYPE, 
			    AB_BIL_ACTIVE,
			    AB_BIL_VISIBLE,
			    AB_BIL_X,
			    AB_BIL_Y,
			    AB_BIL_WIDTH,
			    AB_BIL_HEIGHT,
			    AB_BIL_GROUP_TYPE,
			    AB_BIL_COLUMN_ALIGNMENT,
			    AB_BIL_NUM_COLUMNS,
			    AB_BIL_VOFFSET,
			    AB_BIL_VATTACH_TYPE,
			    AB_BIL_ROW_ALIGNMENT,
			    AB_BIL_NUM_ROWS,
			    AB_BIL_HOFFSET,
			    AB_BIL_HATTACH_TYPE,
			    AB_BIL_BG_COLOR,
			    AB_BIL_FG_COLOR,
			    AB_BIL_BORDER_FRAME,
			    AB_BIL_NORTH_ATTACHMENT,
			    AB_BIL_SOUTH_ATTACHMENT,
			    AB_BIL_EAST_ATTACHMENT,
			    AB_BIL_WEST_ATTACHMENT,
			    AB_BIL_HELP_VOLUME,
			    AB_BIL_HELP_LOCATION,
			    AB_BIL_HELP_TEXT,
			    AB_BIL_DRAG_ENABLED,
			    AB_BIL_DRAG_CURSOR,
			    AB_BIL_DRAG_CURSOR_MASK,
			    AB_BIL_DRAG_OPS,
			    AB_BIL_DRAG_TO_ROOT_ALLOWED,
			    AB_BIL_DRAG_TYPES,
			    AB_BIL_DROP_ENABLED,
			    AB_BIL_DROP_SITE_CHILD_ALLOWED,
			    AB_BIL_DROP_OPS,
			    AB_BIL_DROP_TYPES,
			    AB_BIL_CHILDREN,
			    -1,
		    };

                    returnAttrs = attrs;
		}
		break;
		case AB_CONT_MENU_BAR:
		{
		    static BIL_TOKEN  attrs[] =
		    {
			    AB_BIL_TYPE,
			    AB_BIL_CONTAINER_TYPE, 
			    AB_BIL_WIDTH,
			    AB_BIL_HEIGHT,
			    AB_BIL_BG_COLOR,
			    AB_BIL_FG_COLOR,
			    AB_BIL_ACTIVE,
			    AB_BIL_MENU,
			    AB_BIL_HELP_VOLUME,
			    AB_BIL_HELP_LOCATION,
			    AB_BIL_HELP_TEXT,
			    AB_BIL_CHILDREN,
                        -1,
		    };

                    returnAttrs = attrs;
		}
		break;
		case AB_CONT_PANED:
		{
		    static BIL_TOKEN  attrs[] =
		    {
			    AB_BIL_TYPE,
			    AB_BIL_CONTAINER_TYPE, 
			    AB_BIL_X,
			    AB_BIL_Y,
			    AB_BIL_VISIBLE,
                            AB_BIL_ACTIVE,
			    AB_BIL_NORTH_ATTACHMENT,
			    AB_BIL_SOUTH_ATTACHMENT,
			    AB_BIL_EAST_ATTACHMENT,
			    AB_BIL_WEST_ATTACHMENT,
			    AB_BIL_CHILDREN,
			    -1,
		    };

                    returnAttrs = attrs;
		}
		break;
		case AB_CONT_RELATIVE:
		{
		    static BIL_TOKEN  attrs[] =
		    {
			    AB_BIL_TYPE,
			    AB_BIL_CONTAINER_TYPE, 
			    AB_BIL_X,
			    AB_BIL_Y,
			    AB_BIL_WIDTH,
			    AB_BIL_HEIGHT,
			    AB_BIL_BG_COLOR,
			    AB_BIL_FG_COLOR,
			    AB_BIL_MENU,
			    AB_BIL_MENU_TITLE,
			    AB_BIL_VISIBLE,
			    AB_BIL_BORDER_FRAME,
			    AB_BIL_PANE_MIN_HEIGHT,
			    AB_BIL_PANE_MAX_HEIGHT,
			    AB_BIL_NORTH_ATTACHMENT,
			    AB_BIL_SOUTH_ATTACHMENT,
			    AB_BIL_EAST_ATTACHMENT,
			    AB_BIL_WEST_ATTACHMENT,
			    AB_BIL_HELP_VOLUME,
			    AB_BIL_HELP_LOCATION,
			    AB_BIL_HELP_TEXT,
			    AB_BIL_DRAG_ENABLED,
			    AB_BIL_DRAG_CURSOR,
			    AB_BIL_DRAG_CURSOR_MASK,
			    AB_BIL_DRAG_OPS,
			    AB_BIL_DRAG_TO_ROOT_ALLOWED,
			    AB_BIL_DRAG_TYPES,
			    AB_BIL_DROP_ENABLED,
			    AB_BIL_DROP_SITE_CHILD_ALLOWED,
			    AB_BIL_DROP_OPS,
			    AB_BIL_DROP_TYPES,
			    AB_BIL_CHILDREN,
			    -1,
		    };
                    returnAttrs = attrs;
		}
		break;
                case AB_CONT_TOOL_BAR:
                case AB_CONT_FOOTER:
                case AB_CONT_BUTTON_PANEL:
                {
                    static BIL_TOKEN  attrs[] =
                    {
                            AB_BIL_TYPE,
                            AB_BIL_CONTAINER_TYPE,
                            AB_BIL_WIDTH,
                            AB_BIL_HEIGHT,
                            AB_BIL_BG_COLOR,
                            AB_BIL_FG_COLOR,
                            AB_BIL_MENU,
                            AB_BIL_MENU_TITLE,
                            AB_BIL_VISIBLE,
                            AB_BIL_BORDER_FRAME,
                            AB_BIL_HELP_VOLUME,
                            AB_BIL_HELP_LOCATION,
                            AB_BIL_HELP_TEXT,
			    AB_BIL_DRAG_ENABLED,
			    AB_BIL_DRAG_CURSOR,
			    AB_BIL_DRAG_CURSOR_MASK,
			    AB_BIL_DRAG_OPS,
			    AB_BIL_DRAG_TO_ROOT_ALLOWED,
			    AB_BIL_DRAG_TYPES,
			    AB_BIL_DROP_ENABLED,
			    AB_BIL_DROP_SITE_CHILD_ALLOWED,
			    AB_BIL_DROP_OPS,
			    AB_BIL_DROP_TYPES,
                            AB_BIL_CHILDREN,
                            -1,
                    };
                    returnAttrs = attrs;
		}
		break;
	    }
        }
	break;
                 
        case AB_TYPE_BUTTON:
        {
                static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,
			AB_BIL_BUTTON_TYPE,
			AB_BIL_X, 
                        AB_BIL_Y, 
                        AB_BIL_WIDTH, 
                        AB_BIL_HEIGHT, 
                        AB_BIL_BG_COLOR, 
                        AB_BIL_FG_COLOR, 
                        AB_BIL_LABEL_TYPE,
			AB_BIL_LABEL_ALIGNMENT,
                        AB_BIL_LABEL, 
			AB_BIL_MENU,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,
                };
                
                returnAttrs = attrs;
        }
	break;

	case AB_TYPE_CHOICE: 
	{ 
		static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,
			AB_BIL_CHOICE_TYPE,
                        AB_BIL_X,   
                        AB_BIL_Y,  
                        AB_BIL_WIDTH,  
                        AB_BIL_HEIGHT,  
                        AB_BIL_BG_COLOR,  
                        AB_BIL_FG_COLOR,  
                        AB_BIL_LABEL_TYPE,
                        AB_BIL_LABEL,  
			AB_BIL_LABEL_POSITION,
			AB_BIL_NUM_COLUMNS,
			AB_BIL_ORIENTATION,
			AB_BIL_SELECTION_REQUIRED,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
			AB_BIL_CHILDREN,
			-1,
		};

		returnAttrs = attrs;
        }         
	break;

        case AB_TYPE_DIALOG:
        {
                static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,
                        AB_BIL_X,
                        AB_BIL_Y,
                        AB_BIL_WIDTH,
                        AB_BIL_HEIGHT,
                        AB_BIL_BG_COLOR,
                        AB_BIL_FG_COLOR,
                        AB_BIL_LABEL,
			AB_BIL_RESIZABLE,
                        AB_BIL_VISIBLE, 
			AB_BIL_DEFAULT_BUTTON,
			AB_BIL_HELP_BUTTON,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
			AB_BIL_DRAG_ENABLED,
			AB_BIL_DRAG_CURSOR,
			AB_BIL_DRAG_CURSOR_MASK,
			AB_BIL_DRAG_OPS,
			AB_BIL_DRAG_TO_ROOT_ALLOWED,
			AB_BIL_DRAG_TYPES,
			AB_BIL_DROP_ENABLED,
			AB_BIL_DROP_SITE_CHILD_ALLOWED,
			AB_BIL_DROP_OPS,
			AB_BIL_DROP_TYPES,
			AB_BIL_CHILDREN,
                        -1,
                };

                returnAttrs = attrs;
        }         
	break;

        case AB_TYPE_DRAWING_AREA:
        {
                static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,
                        AB_BIL_X,
                        AB_BIL_Y,
                        AB_BIL_WIDTH,
                        AB_BIL_HEIGHT,
			AB_BIL_DRAWAREA_WIDTH,
			AB_BIL_DRAWAREA_HEIGHT,
                        AB_BIL_BG_COLOR,
                        AB_BIL_FG_COLOR,
			AB_BIL_BORDER_FRAME,
			AB_BIL_MENU,
                        AB_BIL_MENU_TITLE,
			AB_BIL_HSCROLLBAR,
			AB_BIL_VSCROLLBAR,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_PANE_MIN_HEIGHT,
			AB_BIL_PANE_MAX_HEIGHT,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
			AB_BIL_DRAG_ENABLED,
			AB_BIL_DRAG_CURSOR,
			AB_BIL_DRAG_CURSOR_MASK,
			AB_BIL_DRAG_OPS,
			AB_BIL_DRAG_TO_ROOT_ALLOWED,
			AB_BIL_DRAG_TYPES,
			AB_BIL_DROP_ENABLED,
			AB_BIL_DROP_SITE_CHILD_ALLOWED,
			AB_BIL_DROP_OPS,
			AB_BIL_DROP_TYPES,
			-1,
		};
	
		returnAttrs = attrs;
	}
	break;

        case AB_TYPE_LABEL:
        { 
                static BIL_TOKEN  attrs[] =  
                {  
                        AB_BIL_TYPE,  
                        AB_BIL_X,  
                        AB_BIL_Y,  
                        AB_BIL_WIDTH,  
                        AB_BIL_HEIGHT,  
                        AB_BIL_BG_COLOR,  
                        AB_BIL_FG_COLOR,  
                        AB_BIL_LABEL_TYPE,
                        AB_BIL_LABEL,  
/*
			AB_BIL_LABEL_STYLE,
*/
			AB_BIL_LABEL_ALIGNMENT,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_MENU,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
			AB_BIL_DRAG_ENABLED,
			AB_BIL_DRAG_CURSOR,
			AB_BIL_DRAG_CURSOR_MASK,
			AB_BIL_DRAG_OPS,
			AB_BIL_DRAG_TO_ROOT_ALLOWED,
			AB_BIL_DRAG_TYPES,
			AB_BIL_DROP_ENABLED,
			AB_BIL_DROP_SITE_CHILD_ALLOWED,
			AB_BIL_DROP_OPS,
			AB_BIL_DROP_TYPES,
                        -1,
                };
         
                returnAttrs = attrs;
        }
	break;

        case AB_TYPE_LAYERS: 
        { 
                static BIL_TOKEN  attrs[] =   
                {   
                        AB_BIL_TYPE,   
                        AB_BIL_X,   
                        AB_BIL_Y,   
                        AB_BIL_WIDTH,   
                        AB_BIL_HEIGHT,   
                        AB_BIL_BG_COLOR,   
                        AB_BIL_FG_COLOR,   
			AB_BIL_PANE_MIN_HEIGHT,
			AB_BIL_PANE_MAX_HEIGHT,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_CHILDREN,
                        -1, 
                }; 
          
                returnAttrs = attrs; 
        } 
	break;

        case AB_TYPE_LIST:  
        {  
                static BIL_TOKEN  attrs[] =   
                {       
                        AB_BIL_TYPE,    
                        AB_BIL_X,    
                        AB_BIL_Y,    
                        AB_BIL_WIDTH,    
                        AB_BIL_HEIGHT,    
                        AB_BIL_BG_COLOR,    
                        AB_BIL_FG_COLOR,    
                        AB_BIL_LABEL_TYPE,  
                        AB_BIL_LABEL,    
                        AB_BIL_LABEL_POSITION,    
			AB_BIL_SELECTION_MODE,
			AB_BIL_SELECTION_REQUIRED,
			AB_BIL_MENU,
                        AB_BIL_MENU_TITLE,
			AB_BIL_NUM_ROWS,
/*
			AB_BIL_HSCROLLBAR,
			AB_BIL_VSCROLLBAR,
*/
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
			AB_BIL_CHILDREN,
                        -1,  
                };  
           
                returnAttrs = attrs;  
        }  
	break;

        case AB_TYPE_MENU:   
        {   
                static BIL_TOKEN  attrs[] =   
                {        
                        AB_BIL_TYPE,     
			AB_BIL_TEAR_OFF,
                        AB_BIL_BG_COLOR,     
                        AB_BIL_FG_COLOR,     
                        AB_BIL_LABEL,     
			AB_BIL_CHILDREN,
                        -1,   
                };   
            
                returnAttrs = attrs;
        }  
	break;

        case AB_TYPE_SEPARATOR:   
	{    
                static BIL_TOKEN  attrs[] =    
                {         
                        AB_BIL_TYPE,      
			AB_BIL_LINE_STYLE,
			AB_BIL_ORIENTATION,
                        AB_BIL_X,      
                        AB_BIL_Y,      
                        AB_BIL_WIDTH,      
                        AB_BIL_HEIGHT,      
                        AB_BIL_BG_COLOR,      
                        AB_BIL_FG_COLOR,      
			AB_BIL_VISIBLE,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,   
                };  
            
                returnAttrs = attrs;
        } 
	break;

        case AB_TYPE_SCALE:
	{
                static BIL_TOKEN  attrs[] =
                {
                        AB_BIL_TYPE,       
                        AB_BIL_LABEL_TYPE,
                        AB_BIL_LABEL,
                        AB_BIL_LABEL_POSITION,
			AB_BIL_READ_ONLY,
			AB_BIL_ORIENTATION,
			AB_BIL_DIRECTION,
                        AB_BIL_MIN_VALUE,
                        AB_BIL_MAX_VALUE,
                        AB_BIL_INITIAL_VALUE,
                        AB_BIL_INCREMENT,
                        AB_BIL_DECIMAL_POINTS,
                        AB_BIL_SHOW_VALUE,
                        AB_BIL_X,       
                        AB_BIL_Y,       
                        AB_BIL_WIDTH,       
                        AB_BIL_HEIGHT,       
                        AB_BIL_BG_COLOR,       
                        AB_BIL_FG_COLOR,       
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,   
                };   
             
                returnAttrs = attrs; 
        } 
	break;

        case AB_TYPE_SPIN_BOX:   
        {    
                static BIL_TOKEN  attrs[] =    
                {         
                        AB_BIL_TYPE,      
			AB_BIL_LABEL_TYPE,
                        AB_BIL_LABEL,
                        AB_BIL_LABEL_POSITION,
			AB_BIL_TEXT_TYPE,
			AB_BIL_ARROW_STYLE,
                        AB_BIL_MIN_VALUE,
                        AB_BIL_MAX_VALUE,
                        AB_BIL_INITIAL_VALUE,
			AB_BIL_INCREMENT,
			AB_BIL_DECIMAL_POINTS,
			AB_BIL_VISIBLE,
			AB_BIL_ACTIVE,
                        AB_BIL_X,      
                        AB_BIL_Y,      
                        AB_BIL_WIDTH,      
                        AB_BIL_HEIGHT,      
                        AB_BIL_BG_COLOR,      
                        AB_BIL_FG_COLOR,      
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        AB_BIL_CHILDREN,
                        -1,   
                };  
            
                returnAttrs = attrs;
        } 
	break;

        case AB_TYPE_TERM_PANE:
        { 
                static BIL_TOKEN  attrs[] = 
                { 
                        AB_BIL_TYPE,        
                        AB_BIL_X,        
                        AB_BIL_Y,        
                        AB_BIL_WIDTH,        
                        AB_BIL_HEIGHT,        
                        AB_BIL_BG_COLOR,       
                        AB_BIL_FG_COLOR,        
			AB_BIL_BORDER_FRAME,
                        AB_BIL_LABEL_TYPE,
                        AB_BIL_LABEL,        
                        AB_BIL_NUM_ROWS,
                        AB_BIL_NUM_COLUMNS,
                        AB_BIL_VSCROLLBAR,
			AB_BIL_PROCESS_STRING,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_PANE_MIN_HEIGHT,
			AB_BIL_PANE_MAX_HEIGHT,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,  
                };  
             
                returnAttrs = attrs;
        }
	break;

        case AB_TYPE_TEXT_FIELD: 
        {  
                static BIL_TOKEN  attrs[] =  
                {  
                        AB_BIL_TYPE,         
			AB_BIL_TEXT_TYPE,
                        AB_BIL_X,         
                        AB_BIL_Y,         
                        AB_BIL_WIDTH,         
                        AB_BIL_HEIGHT,         
                        AB_BIL_BG_COLOR,        
                        AB_BIL_FG_COLOR,         
                        AB_BIL_LABEL_TYPE, 
                        AB_BIL_LABEL,         
                        AB_BIL_LABEL_POSITION,         
			AB_BIL_NUM_COLUMNS,
			AB_BIL_MAX_LENGTH,
			AB_BIL_INITIAL_VALUE,
			AB_BIL_READ_ONLY,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_MENU,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,   
                };   
              
                returnAttrs = attrs; 
        } 
	break;

        case AB_TYPE_TEXT_PANE: 
        {  
                static BIL_TOKEN  attrs[] =  
                {  
                        AB_BIL_TYPE,         
			AB_BIL_TEXT_TYPE,
                        AB_BIL_X,         
                        AB_BIL_Y,         
                        AB_BIL_WIDTH,         
                        AB_BIL_HEIGHT,         
                        AB_BIL_BG_COLOR,        
                        AB_BIL_FG_COLOR,         
			AB_BIL_BORDER_FRAME,
			AB_BIL_NUM_ROWS,
			AB_BIL_NUM_COLUMNS,
			AB_BIL_HSCROLLBAR,
			AB_BIL_VSCROLLBAR,
			AB_BIL_WORD_WRAP,
			AB_BIL_INITIAL_VALUE,
			AB_BIL_READ_ONLY,
                        AB_BIL_ACTIVE, 
                        AB_BIL_VISIBLE, 
			AB_BIL_MENU,
                        AB_BIL_MENU_TITLE,
			AB_BIL_PANE_MIN_HEIGHT,
			AB_BIL_PANE_MAX_HEIGHT,
			AB_BIL_NORTH_ATTACHMENT,
			AB_BIL_SOUTH_ATTACHMENT,
			AB_BIL_EAST_ATTACHMENT,
			AB_BIL_WEST_ATTACHMENT,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
			AB_BIL_HELP_TEXT,
                        -1,   
                };   
              
                returnAttrs = attrs; 
        } 
	break;

        case AB_TYPE_ACTION: 
        { 
                static BIL_TOKEN  attrs[] =
                { 
                        AB_BIL_FROM,
                        AB_BIL_TO,
                        AB_BIL_WHEN,
                        AB_BIL_ACTION_TYPE,
                        AB_BIL_ACTION,
			AB_BIL_ARG_TYPE,
			AB_BIL_ARG_VALUE,
			AB_BIL_HELP_VOLUME,
			AB_BIL_HELP_LOCATION,
                        -1, 
		};        
              
                returnAttrs = attrs;
        }
	break;

	case AB_TYPE_FILE_CHOOSER: 
	{
		static BIL_TOKEN  attrs[] =
	 	{
			AB_BIL_TYPE,
                        AB_BIL_X,
                        AB_BIL_Y,
                        AB_BIL_BG_COLOR,
                        AB_BIL_FG_COLOR,
			AB_BIL_VISIBLE,
                        AB_BIL_LABEL,
			AB_BIL_FILTER_PATTERN,
			AB_BIL_OK_LABEL,
			AB_BIL_AUTO_DISMISS,
			AB_BIL_DIRECTORY,
			AB_BIL_FILE_TYPE_MASK,
                        AB_BIL_HELP_VOLUME,
                        AB_BIL_HELP_LOCATION,
                        AB_BIL_HELP_TEXT,
			-1,
		};
		returnAttrs = attrs;
	}
	break;

	case AB_TYPE_MESSAGE:
	{
		static BIL_TOKEN  attrs[] =
	 	{
			AB_BIL_TYPE,
                        AB_BIL_MESSAGE_TYPE,
                        AB_BIL_LABEL,
			AB_BIL_MESSAGE,
			AB_BIL_ACTION1_LABEL,
			AB_BIL_ACTION2_LABEL,
			AB_BIL_ACTION3_LABEL,
			AB_BIL_CANCEL_BUTTON,
			AB_BIL_HELP_BUTTON,
			AB_BIL_DEFAULT_BUTTON,
                        AB_BIL_HELP_VOLUME,
                        AB_BIL_HELP_LOCATION,
                        AB_BIL_HELP_TEXT,
			-1,
		};
		returnAttrs = attrs;
	}
	break;
	}
    } /* !AB_TYPE_ITEM */

    return returnAttrs;
}

static int
store_object(
    FILE		*outFile,
    ABObj		obj,
    BIL_SAVE_TYPE	save_type
)
{
    AB_OBJECT_TYPE	obj_type;
    BIL_TOKEN		*attrp = NULL;

#ifdef DEBUG
    if (bilP_obj_verify(obj) < 0)
	util_dabort(1);
#endif
    obj_type = obj_get_type(obj);
    abio_put_keyword(outFile, obj_type);
    abio_putc(outFile, ' ');
    abio_put_keyword_name(outFile, obj_get_name(obj));
    abio_putc(outFile, '\n');
    abio_puts(outFile, abio_bil_object_begin_string());
    abio_putc(outFile, '\n');
    abio_set_indent(outFile, 1);

/* Write out all resource pairs out */

/* get_object_attrs returns an array of BIL_TOKENs
 * which represent the attributes for a specific object
 * type.
 */
    for (attrp = get_object_attrs(obj); (attrp != NULL) && (*attrp != -1); 
	++attrp)
    {
        store_attribute(outFile, obj, *attrp, save_type);
    }
    abio_set_indent(outFile, 0);

    if (! (obj_is_module(obj) || obj_is_project(obj)) )
    {
	abio_puts(outFile, abio_bil_object_end_string());
    }
    abio_putc(outFile, '\n');
    return 0;
} 

/*
 * Store one attribute.
 */
static int
store_attribute(
    FILE		*outFile,
    ABObj          	obj,
    BIL_TOKEN      	attr,
    BIL_SAVE_TYPE	save_type
)
{
    int		attrWidth = 0;
    int		newAttrWidth = 0;
    int		attrLen = 0;
    BOOL	firstWidthIter = TRUE;
    char	attrbuf[1024]; /* attribute name string */
 
    /* If the attribute is NULL, then don't bother writing it
     * out to the output file, just return.
     */
    if (skip_attr(attr, obj, save_type))
 	return 0;

    /*
     * Build and write out the attribute name string, lining up the
     * values in their proper columns.
     * Assume: indent char is tab
     */
    assert(abio_get_indent_char(outFile) == '\t');
    strcpy(attrbuf, bilP_token_to_string(attr));
    attrLen = strlen(attrbuf);
    attrWidth = (abio_get_indent(outFile) * 8) + attrLen;
    firstWidthIter = TRUE;	/* go through at least once */

    while ((attrWidth < VALUE_COLUMN) || firstWidthIter)
    {
	firstWidthIter = FALSE;
	newAttrWidth = ((attrWidth/8) * 8) + 8;
	if (newAttrWidth <= VALUE_COLUMN)
	{
	    attrbuf[attrLen++] = '\t';
	    attrbuf[attrLen] = 0;
	    attrWidth = newAttrWidth;
	}
	else
	{
	    attrbuf[attrLen++] = ' ';
	    attrbuf[attrLen] = 0;
	    ++attrWidth;
	}
    }
    abio_puts(outFile, attrbuf);

    /*
     * Output the attribute/value pair.
     */
    switch (attr)
    {
	/* Attributes/resources common to all ABobj's */
	case AB_BIL_TYPE:
	    abio_puts(outFile, util_object_type_to_string(obj_get_type(obj)));
	    break;

	case AB_BIL_X:
 	    abio_puts(outFile, abio_integer_string(obj_get_x(obj)));
	    break;

	case AB_BIL_Y:
 	    abio_puts(outFile, abio_integer_string(obj_get_y(obj)));
	    break;

	case AB_BIL_WIDTH:
 	    abio_puts(outFile, abio_integer_string(obj_get_width(obj)));
	    break;

	case AB_BIL_HEIGHT:
 	    abio_puts(outFile, abio_integer_string(obj_get_height(obj)));
	    break;

	case AB_BIL_DRAG_CURSOR:
	    abio_put_string(outFile, obj_get_drag_cursor(obj));
	break;

	case AB_BIL_DRAG_CURSOR_MASK:
	    abio_put_string(outFile, obj_get_drag_cursor_mask(obj));
	break;

	case AB_BIL_DRAG_ENABLED:
	    abio_puts(outFile, 
		abio_bil_boolean_string(obj_get_drag_initially_enabled(obj)));
	break;

	case AB_BIL_DRAG_OPS:
	    store_dnd_ops(outFile, obj_get_drag_ops(obj));
	break;

	case AB_BIL_DRAG_TO_ROOT_ALLOWED:
	    abio_puts(outFile, 
		abio_bil_boolean_string(obj_get_drag_to_root_allowed(obj)));
	break;

	case AB_BIL_DRAG_TYPES:
	    store_dnd_types(outFile, obj_get_drag_types(obj));
	break;

	case AB_BIL_DRAWAREA_WIDTH:
	    abio_puts(outFile, abio_integer_string(obj_get_drawarea_width(obj)));
	    break;

        case AB_BIL_DRAWAREA_HEIGHT: 
            abio_puts(outFile, abio_integer_string(obj_get_drawarea_height(obj))); 
            break; 

	case AB_BIL_DROP_ENABLED:
	    abio_puts(outFile, 
		abio_bil_boolean_string(obj_get_drop_initially_enabled(obj)));
	break;

	case AB_BIL_DROP_SITE_CHILD_ALLOWED:
	    abio_puts(outFile, 
		abio_bil_boolean_string(obj_drop_on_children_is_allowed(obj)));
	break;

	case AB_BIL_DROP_OPS:
	    store_dnd_ops(outFile, obj_get_drop_ops(obj));
	break;

	case AB_BIL_DROP_TYPES:
	    store_dnd_types(outFile, obj_get_drop_types(obj));
	break;

        case AB_BIL_BG_COLOR:
	    abio_put_string(outFile, obj_get_bg_color(obj));
	    break;

	case AB_BIL_FG_COLOR:
	    abio_put_string(outFile, obj_get_fg_color(obj));
	    break;

	case AB_BIL_ICON_LABEL:
            abio_put_string(outFile, obj_get_icon_label(obj));
	    break;

	case AB_BIL_LABEL:
	    abio_put_string(outFile, obj_get_label(obj));
	    break;

	case AB_BIL_LABEL_TYPE:
	{
	    AB_LABEL_TYPE	lbl_type = AB_LABEL_UNDEF;
	    lbl_type = obj_get_label_type(obj);
	    abio_puts(outFile, bilP_label_type_to_string(lbl_type));
	    break;
	}

	case AB_BIL_LABEL_POSITION:
	    abio_puts(outFile, bilP_compass_point_to_string(obj_get_label_position(obj)));
	    break;
/*
	case AB_BIL_LABEL_STYLE:
	{
	    AB_LABEL_STYLE	lbl_style;
	    lbl_style = obj_get_label_style(obj);
	    if (lbl_style)
	    {
		abio_puts(outFile, bilP_label_style_to_string(lbl_style));
	    }
	    break;
	}
*/

	/* Attributes specific to a particular ABObj */

	case AB_BIL_FILES:
	{
	    STRING	bil_files = NULL;

	    if ((bil_files = create_bil_file_list(obj)) != NULL)
	    {
                abio_puts(outFile, abio_bil_object_begin_string());
		abio_puts(outFile, bil_files);
		abio_puts(outFile, abio_bil_object_end_string());
		util_free(bil_files);
	    }
	    else
		util_dprintf(1, "malloc or copy failed for bil file list");
	    break;
	}

	case AB_BIL_WIN_CHILDREN: 
	{
	    ABObj		refObj = NULL,
				objModule = NULL,
				refModule = NULL;
	    AB_OBJ_REF_TYPE     refType = AB_REF_UNDEF;
	    STRING		refObj_mod_name = NULL;
	    ABObjList   	refs = NULL; 
	    void        	*voidRefType = NULL; 
	    int         	numRefs = 0, i = 0; 

	    if ((refs = obj_get_refs_to(obj)) != NULL)
	    { 
		/* Go ahead and write '(' to the file because
		 * this won't get called unless there are
		 * dialogs whose win-parent is this object.
		 */
		abio_puts(outFile, abio_bil_object_begin_string());
		abio_putc(outFile, '\n');
		abio_set_indent(outFile, 2);

		numRefs = objlist_get_num_objs(refs); 
		for (i = 0; i < numRefs; i++)
		{
		    refObj = objlist_get_obj(refs, i, &voidRefType);
		    refType = (AB_OBJ_REF_TYPE)voidRefType;
 
		    if (refType == AB_REF_WIN_PARENT)
		    { 
			refModule = obj_get_module(refObj);
			objModule = obj_get_module(obj);

			if (objModule == refModule)
			{
			    abio_puts(outFile, obj_get_name(refObj));
			    abio_putc(outFile, '\n');
			}
			else
			{
			    refObj_mod_name = obj_get_name(refModule);
			    abio_printf(outFile, "%s.%s\n", 
					refObj_mod_name,
					obj_get_name(refObj));
			}
		    }
		}

		abio_set_indent(outFile, 1);
		abio_puts(outFile, abio_bil_object_end_string());

		objlist_destroy(refs);
            }

	    break;
	}

	case AB_BIL_WIN_PARENT:
        {
            ABObj       obj_module= NULL;
	    ABObj	win_parent= NULL;
	    ABObj	win_parent_module= NULL;
            STRING      win_parent_mod_name = NULL;

	    win_parent= obj_get_win_parent(obj);
	    if (win_parent == NULL)
	    {
		abio_puts(outFile, bilP_token_to_string(AB_BIL_UNDEF_KEYWORD));
		break;
	    }
            obj_module = obj_get_module(obj);
            win_parent_module = obj_get_module(win_parent);
            if (obj_module == win_parent_module)
            {
		abio_puts(outFile, obj_get_name(win_parent));
            }
            else  /* The dialog's parent is in another module */
            {
                win_parent_mod_name = obj_get_name(win_parent_module);
                abio_puts(outFile, win_parent_mod_name);
                abio_putc(outFile, '.');
                abio_puts(outFile, obj_get_name(win_parent));
            }
            break;
        }

	case AB_BIL_BORDER_FRAME:
	    abio_puts(outFile, bilP_border_frame_to_string(obj_get_border_frame(obj)));
	    break;

	case AB_BIL_PACKING:
	    abio_puts(outFile, bilP_packing_to_string(obj_get_packing(obj)));
	    break;

	case AB_BIL_PROCESS_STRING:
            abio_put_string(outFile, obj_get_process_string(obj));
	    break;

	case AB_BIL_RES_FILE_TYPES:
	    store_res_file_types(outFile, obj);
	break;

	case AB_BIL_ROW_ALIGNMENT:
	    abio_puts(outFile, bilP_alignment_to_string(obj_get_row_align(obj)));
	    break;

	case AB_BIL_COLUMN_ALIGNMENT:
	    abio_puts(outFile, bilP_alignment_to_string(obj_get_col_align(obj)));
	    break;

	/* REMIND: Currently, there is only one resizable field
	 * 	   in AB_WINDOW_INFO struct.  There is no
	 *	   differentiation between resizable width 
	 *	   and height.  Do we need to change the
	 *	   struct to add both a width_resizable field 
	 *	   and a height_resizable field?
	 */
/*
	case AB_BIL_HEIGHT_RESIZABLE:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_height_resizable(obj)));
	    break;

	case AB_BIL_WIDTH_RESIZABLE:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_width_resizable(obj)));
	    break;
*/
	case AB_BIL_RESIZABLE:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_resizable(obj)));
	    break;

	case AB_BIL_CONTAINER_TYPE:
	    abio_puts(outFile, bilP_container_type_to_string(obj_get_container_type(obj)));
	    break;

	case AB_BIL_MENU:
	    abio_puts(outFile, obj_get_menu_name(obj));
	    break;

        case AB_BIL_MENU_TITLE:
            abio_put_string(outFile, obj_get_menu_title(obj));
            break;
/*
	case AB_BIL_MENU_TYPE:
	    abio_puts(outFile, bilP_menu_type_to_string(obj_get_menu_type(obj)));
	    break;
*/

	case AB_BIL_GROUP_TYPE:
	    abio_puts(outFile, bilP_group_type_to_string(obj_get_group_type(obj)));
	    break;

	case AB_BIL_LABEL_ALIGNMENT:
	    switch (obj_get_type(obj))
	    {
		case AB_TYPE_BUTTON:
		case AB_TYPE_LABEL:
		    abio_puts(outFile, bilP_alignment_to_string(obj_get_label_alignment(obj)));
		    break;
	    }
	    break;

	case AB_BIL_NUM_COLUMNS:
	    abio_puts(outFile, abio_integer_string(obj_get_num_columns(obj)));
	    break;

	case AB_BIL_HATTACH_TYPE:
	    abio_puts(outFile, bilP_attachment_to_string(obj_get_hattach_type(obj)));
	    break;

	case AB_BIL_VATTACH_TYPE:
	    abio_puts(outFile, bilP_attachment_to_string(obj_get_vattach_type(obj)));
	    break;

	case AB_BIL_HOFFSET:
	    abio_puts(outFile, abio_integer_string(obj_get_hoffset(obj)));
	    break;

	case AB_BIL_HSPACING:
	    abio_puts(outFile, abio_integer_string(obj_get_hspacing(obj)));
	    break;

	case AB_BIL_REFERENCE_POINT:
	    abio_puts(outFile, bilP_compass_point_to_string(obj_get_ref_point(obj)));
	    break;

	case AB_BIL_NUM_ROWS:
	    abio_puts(outFile, abio_integer_string(obj_get_num_rows(obj)));
	    break;

        case AB_BIL_VENDOR:
            abio_put_string(outFile, obj_get_vendor(obj));
            break;

        case AB_BIL_VERSION:
            abio_put_string(outFile, obj_get_version(obj));
            break;

        case AB_BIL_ROOT_WINDOW:
	{
            ABObj       root_win= NULL;
            ABObj       root_win_module= NULL;
            STRING      root_win_mod_name = NULL;
 
            root_win= obj_get_root_window(obj);
	    if (root_win != NULL)
	    {
                root_win_module = obj_get_module(root_win);
                root_win_mod_name = obj_get_name(root_win_module);
                abio_puts(outFile, root_win_mod_name);
                abio_putc(outFile, '.');
                abio_puts(outFile, obj_get_name(root_win));
            }
            break;
	}
	case AB_BIL_VISIBLE:
            abio_puts(outFile, abio_bil_boolean_string(obj_is_initially_visible(obj)));
	    break;

	case AB_BIL_VOFFSET:
	    abio_puts(outFile, abio_integer_string(obj_get_voffset(obj)));
	    break;

	case AB_BIL_VSPACING:
	    abio_puts(outFile, abio_integer_string(obj_get_vspacing(obj)));
	    break;

        case AB_BIL_BUTTON_TYPE:
	    abio_puts(outFile, bilP_button_type_to_string(obj_get_button_type(obj)));
	    break;

	case AB_BIL_ORIENTATION:
	    abio_puts(outFile, bilP_orientation_to_string(obj_get_orientation(obj)));
	    break;

	case AB_BIL_DIRECTION:
	    abio_puts(outFile, bilP_direction_to_string(obj_get_direction(obj)));
	    break;

	case AB_BIL_SELECTION_REQUIRED:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_selection_required(obj)));
	    break;

	case AB_BIL_HSCROLLBAR:
	    abio_puts(outFile, bilP_scrollbar_policy_to_string(
				obj_get_hscrollbar_policy(obj)));
	    break;

	case AB_BIL_VSCROLLBAR:
	    abio_puts(outFile, bilP_scrollbar_policy_to_string(
				obj_get_vscrollbar_policy(obj)));
	    break;

	case AB_BIL_IS_HELP_ITEM:
	    abio_puts(outFile, abio_bil_boolean_string(obj_is_help_item(obj)));
	    break;

	case AB_BIL_ITEM_TYPE:
	    abio_puts(outFile, bilP_item_type_to_string(obj_get_item_type(obj)));
	    break;

        case AB_BIL_ACCELERATOR:
	    abio_put_string(outFile, obj_get_accelerator(obj));
	    break;

        case AB_BIL_MNEMONIC:
            abio_put_string(outFile, obj_get_mnemonic(obj));
            break;

	case AB_BIL_TEAR_OFF:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_tearoff(obj)));
	    break;

	case AB_BIL_EXCLUSIVE:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_exclusive(obj)));
	    break;

	case AB_BIL_DECIMAL_POINTS:
                abio_puts(outFile, abio_integer_string(obj_get_decimal_points(obj)));
            break;

  	case AB_BIL_INCREMENT:
                abio_puts(outFile, abio_integer_string(obj_get_increment(obj)));
            break;

	case AB_BIL_MIN_VALUE:
	        abio_puts(outFile, abio_integer_string(obj_get_min_value(obj)));
	    break;

	case AB_BIL_MAX_VALUE:
	        abio_puts(outFile, abio_integer_string(obj_get_max_value(obj)));
	    break;

	case AB_BIL_TEXT_TYPE:
	    abio_puts(outFile, bilP_text_type_to_string(obj_get_text_type(obj)));
	    break;

	case AB_BIL_MAX_LENGTH:
	    abio_puts(outFile, abio_integer_string(obj_get_max_length(obj)));
	    break;

	case AB_BIL_INITIAL_VALUE:
	{
	    STRING	val;
	    int		ival;
	    AB_OBJECT_TYPE 	type;

	    type = obj_get_type(obj);
	    if (type == AB_TYPE_SCALE || type == AB_TYPE_SPIN_BOX)
	    {
		ival = obj_get_initial_value_int(obj);
		abio_puts(outFile, abio_integer_string(ival));
	    } 
	    else if (type == AB_TYPE_TEXT_FIELD || type == AB_TYPE_TEXT_PANE)
	    {
		val = obj_get_initial_value_string(obj);
		if (val != NULL)
		    abio_put_string(outFile, val);
	    }
            break;
        }    

	case AB_BIL_READ_ONLY:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_read_only(obj)));
	    break;

        case AB_BIL_ACTION: 
	{
	    AB_FUNC_TYPE	type = AB_FUNC_UNDEF;

	    type = obj_get_func_type(obj);
	    switch (type)
	    {
		case AB_FUNC_BUILTIN:
		    abio_puts(outFile, bilP_builtin_action_to_string(
			obj_get_func_builtin(obj)));
		    break;
		case AB_FUNC_USER_DEF:
		    abio_puts(outFile, obj_get_func_name(obj));
		    break;
		case AB_FUNC_CODE_FRAG:
		    abio_put_string(outFile, obj_get_func_code(obj));
		    break;
	    }
	    break;
	}

        case AB_BIL_ACTION_TYPE:
	    abio_puts(outFile, bilP_func_type_to_string(obj_get_func_type(obj)));
	    break;

        case AB_BIL_FROM:
	{
            AB_FUNC_TYPE        type = AB_FUNC_UNDEF;

	    if (obj_is_project(obj->info.action.from))
	    {
                abio_puts(outFile, bilP_token_to_string(AB_BIL_APPLICATION));
	    }
	    else
	    {
            	type = obj_get_func_type(obj);
            	switch (type)
            	{                                               
                   case AB_FUNC_BUILTIN:
	           {
		    	ABObj	to_module = NULL, from_module = NULL;
		    	STRING	from_mod_name = NULL;

		    	to_module = obj_get_module(obj->info.action.to);
		    	from_module = obj_get_module(obj->info.action.from);
		    	if (to_module == from_module)
	    	    	{
			    abio_puts(outFile, get_from_name(obj));
		    	}
		    	else  /* It is a cross-module connection */
		    	{
			    from_mod_name = obj_get_name(from_module);
			    abio_puts(outFile, from_mod_name);
			    abio_putc(outFile, '.');
			    abio_puts(outFile, get_from_name(obj));
		    	}
		    	break;
		    }
		    case AB_FUNC_USER_DEF:
		    case AB_FUNC_CODE_FRAG:
		    case AB_FUNC_ON_ITEM_HELP:
		    case AB_FUNC_HELP_VOLUME:
		        abio_puts(outFile, get_from_name(obj));
                        break;
            	}  /* End Switch */
	    }
	    break;
	}  /* End case AB_BIL_FROM */

        case AB_BIL_TO:
        { 
	    AB_FUNC_TYPE	type = AB_FUNC_UNDEF;

	    type = obj_get_func_type(obj);
	    switch (type)
	    {
	      case AB_FUNC_BUILTIN:
		{
		    ABObj       	to_module, from_module; 
		    STRING      	to_mod_name; 

		    to_module = obj_get_module(obj->info.action.to);  
		    from_module = obj_get_module(obj->info.action.from);
		    if (to_module == from_module)  
		    { 
			abio_puts(outFile, get_to_name(obj));
		    }
		    else  /* It is a cross-module connection */ 
		    { 
			to_mod_name = obj_get_name(to_module);
			abio_puts(outFile, to_mod_name); 
			abio_putc(outFile, '.'); 
			abio_puts(outFile, get_to_name(obj)); 
		    }
		}
		break;

	      case AB_FUNC_USER_DEF:
	      case AB_FUNC_CODE_FRAG:
	      case AB_FUNC_ON_ITEM_HELP:
	      case AB_FUNC_HELP_VOLUME:
		break;
	    }
	    break;
	}

        case AB_BIL_WHEN:
	    abio_puts(outFile, bilP_when_to_string(obj_get_when(obj)));
	    break;
	
	case AB_BIL_ARG_TYPE:
	    abio_puts(outFile, bilP_arg_type_to_string(obj_get_arg_type(obj)));
	    break;

	case AB_BIL_ARG_VALUE:
        {
            AB_ARG_TYPE        type = AB_ARG_UNDEF;
 
            if ((type = obj_get_arg_type(obj)) != 0)
	    {
		switch (type)
		{
		    case AB_ARG_FLOAT:
			abio_put_float(outFile, obj_get_arg_float(obj));
			break;
		    case AB_ARG_INT:
			abio_put_integer(outFile, obj_get_arg_int(obj));
			break;
		    case AB_ARG_VOID_PTR:
		    case AB_ARG_STRING:
			if (obj_get_arg_string(obj))
			{
			    abio_put_string(outFile, 
				obj_get_arg_string(obj));
			}
			break;
		}
	    }
	    break;
        }

        case AB_BIL_CHOICE_TYPE:
	    abio_puts(outFile, bilP_choice_type_to_string(obj_get_choice_type(obj))); 
            break; 

        case AB_BIL_SELECTED:
	    abio_puts(outFile, 
		abio_bil_boolean_string(obj_is_initially_selected(obj))); 
            break; 

	case AB_BIL_SELECTION_MODE:
	    abio_puts(outFile, bilP_selection_mode_to_string(obj_get_selection_mode(obj)));
	    break;

	case AB_BIL_DEFAULT:
	    abio_puts(outFile, abio_bil_boolean_string(obj_is_default(obj)));
            break;

	case AB_BIL_CHILDREN:
	{
	    AB_TRAVERSAL    trav;
	    ABObj           child;

	    abio_puts(outFile, abio_bil_object_begin_string());
	    abio_putc(outFile, '\n');
	    abio_set_indent(outFile, 2);
	    for( trav_open(&trav, obj, AB_TRAV_SALIENT_UI_CHILDREN);
        	(child = trav_next(&trav)) != NULL; )
	    {
	        abio_puts(outFile, obj_get_name(child));
	        abio_putc(outFile, '\n');
	    }
	    trav_close(&trav);
	    abio_set_indent(outFile, 1);
	    abio_puts(outFile, abio_bil_object_end_string());

            break;
	}

	case AB_BIL_NORTH_ATTACHMENT: 
	{
	    AB_ATTACH_TYPE	attach_type = AB_ATTACH_UNDEF;
	
	    abio_putc(outFile, '(');
	    attach_type = obj_get_attach_type(obj, AB_CP_NORTH);
	    if (attach_type != AB_ATTACH_UNDEF)
		abio_puts(outFile, bilP_attachment_to_string(attach_type));
	    abio_putc(outFile, ' ');
	    switch (attach_type)
	    {
                case AB_ATTACH_OBJ:
                case AB_ATTACH_ALIGN_OBJ_EDGE:
		    abio_puts(outFile, obj_get_name(obj_get_root( (ABObj)obj_get_attach_value(obj, AB_CP_NORTH))));
		    break;
                case AB_ATTACH_GRIDLINE:
                case AB_ATTACH_CENTER_GRIDLINE:
		    abio_put_integer(outFile, (long)obj_get_attach_value(obj, AB_CP_NORTH));
		    break;
		case AB_ATTACH_POINT:
		case AB_ATTACH_NONE:
		    abio_put_integer(outFile, 0);
		    break;
            }
	    abio_putc(outFile, ' ');
	    if (obj->attachments != NULL)
	        abio_put_integer(outFile, obj_get_attach_offset(obj, AB_CP_NORTH));
	    else
		abio_put_integer(outFile, 0); 
	    abio_putc(outFile, ')');
	    break;
	}

	case AB_BIL_SOUTH_ATTACHMENT:
	{
	    AB_ATTACH_TYPE	attach_type = AB_ATTACH_UNDEF;
	
	    abio_putc(outFile, '(');
	    attach_type = obj_get_attach_type(obj, AB_CP_SOUTH);
	    if (attach_type != AB_ATTACH_UNDEF)
		abio_puts(outFile, bilP_attachment_to_string(attach_type));
	    abio_putc(outFile, ' ');
            switch (attach_type)
            {
                case AB_ATTACH_OBJ:
                case AB_ATTACH_ALIGN_OBJ_EDGE:
                    abio_puts(outFile, obj_get_name(obj_get_root((ABObj)obj_get_attach_value(obj, AB_CP_SOUTH))));
                    break;
                case AB_ATTACH_GRIDLINE:
                case AB_ATTACH_CENTER_GRIDLINE:
                    abio_put_integer(outFile, (long)obj_get_attach_value(obj, AB_CP_SOUTH));
                    break;
                default:
		    abio_put_integer(outFile, 0);
                    break;
            }
	    abio_putc(outFile, ' ');
            if (obj->attachments != NULL)
                abio_put_integer(outFile, obj_get_attach_offset(obj, AB_CP_SOUTH));
            else
		abio_put_integer(outFile, 0);
	    abio_putc(outFile, ')');
	    break;
	}

	case AB_BIL_EAST_ATTACHMENT:
	{
	    AB_ATTACH_TYPE	attach_type = AB_ATTACH_UNDEF;
	
	    abio_putc(outFile, '(');
	    attach_type = obj_get_attach_type(obj, AB_CP_EAST);
	    if (attach_type != AB_ATTACH_UNDEF)
		abio_puts(outFile, bilP_attachment_to_string(attach_type));
	    abio_putc(outFile, ' ');
            switch (attach_type)
            {
                case AB_ATTACH_OBJ:
                case AB_ATTACH_ALIGN_OBJ_EDGE:
                    abio_puts(outFile, obj_get_name(obj_get_root((ABObj)obj_get_attach_value(obj, AB_CP_EAST))));
                    break;
                case AB_ATTACH_GRIDLINE:
                case AB_ATTACH_CENTER_GRIDLINE:
                    abio_put_integer(outFile, (long)obj_get_attach_value(obj, AB_CP_EAST));
                    break;
                default:
		    abio_put_integer(outFile, 0);
                    break;
            }
	    abio_putc(outFile, ' ');
            if (obj->attachments != NULL)
                abio_put_integer(outFile,obj_get_attach_offset(obj, AB_CP_EAST));
            else
		abio_put_integer(outFile, 0);
            abio_putc(outFile, ')');
	    break;
	}

	case AB_BIL_WEST_ATTACHMENT:
	{
	    AB_ATTACH_TYPE	attach_type = AB_ATTACH_UNDEF;
	
            abio_putc(outFile, '(');
	    attach_type = obj_get_attach_type(obj, AB_CP_WEST);
	    if (attach_type != AB_ATTACH_UNDEF)
		abio_puts(outFile, bilP_attachment_to_string(attach_type));
	    abio_putc(outFile, ' ');
            switch (attach_type)
            {
                case AB_ATTACH_OBJ:
                case AB_ATTACH_ALIGN_OBJ_EDGE:
                    abio_puts(outFile, obj_get_name(obj_get_root((ABObj)obj_get_attach_value(obj, AB_CP_WEST))));
                    break;
                case AB_ATTACH_GRIDLINE:
                case AB_ATTACH_CENTER_GRIDLINE:
                    abio_put_integer(outFile, (long)obj_get_attach_value(obj, AB_CP_WEST));
                    break;
                default:
		    abio_put_integer(outFile, 0);
                    break;
            }
	    abio_putc(outFile, ' ');
            if (obj->attachments != NULL)
                abio_put_integer(outFile, obj_get_attach_offset(obj, AB_CP_WEST));
            else
		abio_put_integer(outFile, 0);
            abio_putc(outFile, ')');
	    break;
	}

	case AB_BIL_SB_ALWAYS_VISIBLE:
		/* REMIND: (mcv) This is an obsolete attribute.
		 * Take this case out when bil files have been
		 * converted to new hscroll/vscroll values.
		 */
	    break;

	case AB_BIL_ICON_FILE:
	    abio_put_string(outFile, obj_get_icon(obj));
	    break;

	case AB_BIL_ICON_MASK_FILE:
	    abio_put_string(outFile, obj_get_icon_mask(obj));
	    break;

	case AB_BIL_HELP_LOCATION:
	    if (!obj_is_action(obj))
		abio_put_string(outFile, obj_get_help_location(obj));
	    else
		abio_put_string(outFile, obj_get_func_help_location(obj));
	    break;

	case AB_BIL_HELP_TEXT:
	    abio_put_string(outFile, obj_get_help_text(obj));
	    break;

	case AB_BIL_HELP_VOLUME:
	    if (!obj_is_action(obj))
		abio_put_string(outFile, obj_get_help_volume(obj));
	    else
		abio_put_string(outFile, obj_get_func_help_volume(obj));
	    break;

	case AB_BIL_WORD_WRAP:
            abio_puts(outFile, abio_bil_boolean_string(obj_get_word_wrap(obj)));
            break;

	case AB_BIL_LINE_STYLE:
	    abio_puts(outFile, 
		bilP_line_style_to_string(obj_get_line_style(obj)));
	    break;

	case AB_BIL_ARROW_STYLE:
	    abio_puts(outFile, 
		bilP_arrow_style_to_string(obj_get_arrow_style(obj)));
	    break;
	case AB_BIL_ICONIC:
	    abio_puts(outFile, abio_bil_boolean_string(obj_is_initially_iconic(obj)));
	    break;

	case AB_BIL_ACTIVE:
	    abio_puts(outFile, abio_bil_boolean_string(obj_is_initially_active(obj)));
	    break;

	case AB_BIL_SHOW_VALUE:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_show_value(obj)));
            break; 

	case AB_BIL_FILTER_PATTERN:
	    abio_put_string(outFile, obj_get_filter_pattern(obj)); 
	    break;

	case AB_BIL_OK_LABEL:
	    abio_put_string(outFile, obj_get_ok_label(obj));
	    break;

	case AB_BIL_AUTO_DISMISS:
	    abio_puts(outFile, abio_bil_boolean_string(obj_get_auto_dismiss(obj)));
	    break;

	case AB_BIL_DIRECTORY:
	    abio_put_string(outFile, obj_get_directory(obj));
	    break;

	case AB_BIL_FILE_TYPE_MASK:
	    abio_puts(outFile, bilP_file_type_mask_to_string(obj_get_file_type_mask(obj)));
	    break;

	case AB_BIL_MESSAGE_TYPE:
	    abio_puts(outFile, bilP_msg_type_to_string(obj_get_msg_type(obj)));
	    break;
	
	case AB_BIL_MESSAGE: 
	    abio_put_string(outFile, obj_get_msg_string(obj));
	    break;

	case AB_BIL_SESSION_MGMT:
	{

	    abio_puts(outFile, abio_bil_object_begin_string());
	    abio_putc(outFile, '\n');
	    abio_set_indent(outFile, 2);

	    store_attribute(outFile, obj, AB_BIL_SESSIONING_METHOD, save_type);

	    abio_set_indent(outFile, 1);
	    abio_puts(outFile, abio_bil_object_end_string());

            break;
	}
	case AB_BIL_SESSIONING_METHOD:
	    abio_puts(outFile, 
		bilP_sessioning_method_to_string(obj_get_sessioning_method(obj)));
	break;

	case AB_BIL_I18N:
	{

	    abio_puts(outFile, abio_bil_object_begin_string());
	    abio_putc(outFile, '\n');
	    abio_set_indent(outFile, 2);

	    store_attribute(outFile, obj, AB_BIL_I18N_ENABLED, save_type);

	    abio_set_indent(outFile, 1);
	    abio_puts(outFile, abio_bil_object_end_string());

            break;
	}
	case AB_BIL_I18N_ENABLED:
	    abio_puts(outFile, 
		abio_bil_boolean_string(obj_get_i18n_enabled(obj)));
	    break;

        case AB_BIL_TOOLTALK:
        {
            abio_puts(outFile, abio_bil_object_begin_string());
            abio_putc(outFile, '\n');
            abio_set_indent(outFile, 2);

            store_attribute(outFile, obj, AB_BIL_TOOLTALK_DESKTOP_LEVEL, save_type);

            abio_set_indent(outFile, 1);
            abio_puts(outFile, abio_bil_object_end_string());

            break;
        }
        case AB_BIL_TOOLTALK_DESKTOP_LEVEL:
            abio_puts(outFile,
                bilP_tt_desktop_level_to_string(obj_get_tooltalk_level(obj)));
        break;

	case AB_BIL_CANCEL_BUTTON:
            abio_puts(outFile, 
			abio_bil_boolean_string(obj_has_cancel_button(obj)));
            break;
	    
	case AB_BIL_HELP_BUTTON:
	    if (obj_is_popup_win(obj))
	    {
		ABObj help_b = obj_get_help_act_button(obj);
            	if (help_b != NULL)
                    abio_puts(outFile, obj_get_name(help_b));
		
            }
	    else if (obj_is_message(obj))
	    {
                abio_puts(outFile, 
			abio_bil_boolean_string(obj_has_help_button(obj)));
	    }
            break;
	    
        case AB_BIL_ACTION1_LABEL:
            abio_put_string(outFile, obj_get_action1_label(obj));
            break;

        case AB_BIL_ACTION2_LABEL:
            abio_put_string(outFile, obj_get_action2_label(obj));
            break;

        case AB_BIL_ACTION3_LABEL:
            abio_put_string(outFile, obj_get_action3_label(obj));
            break;

	case AB_BIL_DEFAULT_BUTTON:
	    if (obj_is_popup_win(obj))
	    {
		ABObj default_b = obj_get_default_act_button(obj);
            	if (default_b == NULL)
            	{
                    abio_puts(outFile, bilP_token_to_string(AB_BIL_UNDEF_KEYWORD));
                    break;
            	}   
		else
                    abio_puts(outFile, obj_get_name(default_b));
            }
	    else
	    	abio_puts(outFile,
			bilP_default_button_to_string(obj_get_default_btn(obj)));
	    break;

	case AB_BIL_PANE_MIN_HEIGHT:
 	    abio_puts(outFile, abio_integer_string(obj_get_pane_min(obj)));
	    break;

	case AB_BIL_PANE_MAX_HEIGHT:
 	    abio_puts(outFile, abio_integer_string(obj_get_pane_max(obj)));
	    break;
    }
    abio_putc(outFile, '\n');
    return 0;
}


static int
store_res_file_types(FILE *outFile, ABObj obj)
{
    AB_ARG_CLASS_FLAGS	resFileArgClasses = obj_get_res_file_arg_classes(obj);
    AB_ARG_CLASS	argClass;
    AB_ARG_CLASS_FLAGS	argClassFlag;
    int			i = 0;
    int			classCount = 0;

    abio_puts(outFile, abio_bil_object_begin_string());
    for (i = 0; i < AB_ARG_CLASS_NUM_VALUES; ++i)
    {
	argClass = (AB_ARG_CLASS)i;
	argClassFlag = util_arg_class_to_flag(argClass);
	if ((resFileArgClasses & argClassFlag) != 0)
	{
	    if (classCount > 0)
	    {
	        abio_puts(outFile, " ");
	    }
	    abio_puts(outFile, bilP_arg_class_to_string(argClass));
	    ++classCount;
	}
    }
    abio_puts(outFile, abio_bil_object_end_string());

    return classCount;
}


static int
store_dnd_ops(FILE *outFile, ABDndOpFlags opFlags)
{
    int		flagCount = 0;
    int		numWritten = 0;
    ABDndOpFlags	oneFlag = 0;
    BIL_TOKEN		token = AB_BIL_UNDEF;
    STRING		tokenString = NULL;

    abio_puts(outFile, abio_bil_object_begin_string());
    for (flagCount = 0; flagCount < 3; ++flagCount)
    {
	switch (flagCount)
	{
	    case 0: oneFlag = ABDndOpCopy; break;
	    case 1: oneFlag = ABDndOpLink; break;
	    case 2: oneFlag = ABDndOpMove; break;
	}
        if ((opFlags & oneFlag) != 0)
        {
	    token = bilP_dnd_op_flag_to_token(oneFlag);
	    tokenString = bilP_token_to_string(token);
	    if (numWritten > 0)
	    {
		abio_puts(outFile, " ");
	    }
	    abio_puts(outFile, tokenString);
	    ++numWritten;
        }
    }
    abio_puts(outFile, abio_bil_object_end_string());

    return numWritten;
}


static int
store_dnd_types(FILE *outFile, ABDndTypeFlags typeFlags)
{
    int			flagCount = 0;
    int			numWritten = 0;
    ABDndTypeFlags	oneFlag = 0;
    BIL_TOKEN		token = AB_BIL_UNDEF;
    STRING		tokenString = NULL;

    abio_puts(outFile, abio_bil_object_begin_string());
    for (flagCount = 0; flagCount < 4; ++flagCount)
    {
	switch (flagCount)
	{
	    case 0: oneFlag = ABDndTypeText; break;
	    case 1: oneFlag = ABDndTypeFilename; break;
	    case 2: oneFlag = ABDndTypeUserDef; break;
	    case 3: oneFlag = ABDndTypeAny; break;
	}
        if ((typeFlags & oneFlag) != 0)
        {
	    token = bilP_dnd_type_flag_to_token(oneFlag);
	    tokenString = bilP_token_to_string(token);
	    if (numWritten > 0)
	    {
		abio_puts(outFile, " ");
	    }
	    abio_puts(outFile, tokenString);
	    ++numWritten;
        }
    }
    abio_puts(outFile, abio_bil_object_end_string());

    return numWritten;
}


/*
 * This will write out a project or module definition
 */
static int
bilP_write_tree(
    FILE		*outFile,
    ABObj		root,
    BIL_SAVE_TYPE	save_type
)
{
#ifdef DEBUG
    if (obj_tree_verify(root) < 0)
	util_dabort(1);
#endif
    bilP_write_tree_ui_objs(outFile, root, save_type);
    bilP_write_tree_conn_objs(outFile, root, save_type);

    if (obj_is_project(root) || obj_is_module(root))
    {
	/*
	* This ends the module (module) section of the BIL out file
	*/
	abio_puts(outFile, abio_bil_object_end_string());
	abio_putc(outFile, '\n');
    }

    return 0;
}


static int
bilP_write_tree_ui_objs(
    FILE		*outFile,
    ABObj		root,
    BIL_SAVE_TYPE	save_type
)
{
    AB_TRAVERSAL    trav;
    ABObj           child;

    if (root == NULL)
        return -1;

    if ( !obj_is_sub(root) && obj_is_defined(root)
	 && (obj_is_ui(root) || obj_is_module(root)
	    || obj_is_project(root))
       )
    {
	store_object(outFile, root, save_type);
    }
    for( trav_open(&trav, root, AB_TRAV_SALIENT_CHILDREN);
        (child = trav_next(&trav)) != NULL; )
    {
	if (   obj_is_module(child) 
	    && (   (save_type == BIL_SAVE_FILE_PER_MODULE)
		|| (save_type == BIL_SAVE_ENCAPSULATED)) 
	   )
	{
	    /* don't go into modules... */
	    continue;
	}
	bilP_write_tree_ui_objs(outFile, child, save_type);
    }
    trav_close(&trav);
    return 0;
}





static int
store_action(
    FILE		*outFile,
    ABObj		obj,
    BIL_SAVE_TYPE	save_type
)
{
    AB_OBJECT_TYPE      obj_type;
    BIL_TOKEN           *attrp;

    obj_type = obj_get_type(obj);
    abio_put_keyword(outFile, obj_type);
    abio_putc(outFile, '\n');
    abio_puts(outFile, abio_bil_object_begin_string());
    abio_putc(outFile, '\n');
    abio_set_indent(outFile, 1);

    for (attrp = get_object_attrs(obj); *attrp != -1; attrp++)
                store_attribute(outFile, obj, *attrp, save_type);

    abio_set_indent(outFile, 0);
    abio_puts(outFile, abio_bil_object_end_string());
    abio_putc(outFile, '\n');
    return 0;
}

static int
bilP_write_tree_conn_objs(
    FILE		*outFile,
    ABObj		root,
    BIL_SAVE_TYPE	save_type
)
{
    AB_TRAVERSAL	trav;
    ABObj		action_obj = NULL;
    AB_FUNC_TYPE        type = AB_FUNC_UNDEF;
    BOOL		saveCrossModule = (obj_get_module(root) == NULL);

    for (trav_open(&trav, root, AB_TRAV_ACTIONS_FOR_OBJ);
	(action_obj = trav_next(&trav)) !=NULL; ) 
    {
	type = obj_get_func_type(action_obj);
	switch (type)  
	{
	    case AB_FUNC_BUILTIN:
		/* Check if the connection is within the same module. 
		 * We don't want to write it out if it is a cross-module
		 * connection.
		 */
		if (!util_xor(obj_is_cross_module(action_obj),
				saveCrossModule) )
		{
		    /*is_cross_module and saveCrossModule are logically same*/
		    store_action(outFile, action_obj, save_type);
		}
		break;
	    case AB_FUNC_USER_DEF:
	    case AB_FUNC_CODE_FRAG:
	    case AB_FUNC_ON_ITEM_HELP:
	    case AB_FUNC_HELP_VOLUME:
		store_action(outFile, action_obj, save_type);
		break;
	}
    }
    return 0;
}


#ifdef BOGUS
static int
save_cross_module_actions(
    FILE	*outFile,
    ABObj	root,
    BIL_SAVE_TYPE	save_type
)
{
    AB_TRAVERSAL        mod_trav, act_trav;
    ABObj               mod_obj = NULL, action_obj = NULL;
    AB_FUNC_TYPE        type = AB_FUNC_UNDEF;

    if (!obj_is_project(root))
	return -1;

    for (trav_open(&mod_trav, root, AB_TRAV_MODULES);
        (mod_obj = trav_next(&mod_trav)) !=NULL; )
    {
	for (trav_open(&act_trav, mod_obj, AB_TRAV_ACTIONS);
	    (action_obj = trav_next(&act_trav)) !=NULL; )
	{
	    type = obj_get_func_type(action_obj);
	    if (type == AB_FUNC_BUILTIN)
	    {
		/* Check if this is a cross-module connection */
		if ( obj_get_module(action_obj->info.action.to) != mod_obj)
		    store_action(outFile, action_obj, save_type);
	    }
	}
    }
    return 0;
}
#endif /* BOGUS */

/* This routine returns a list of bil files. The space is
 * malloc'ed here, so it must be freed in the calling routine.
 */
static STRING
create_bil_file_list(
    ABObj	proj
)
{
    AB_TRAVERSAL	trav;
    STRING		bil_list = NULL;
    ABObj		child;
    int			first = 1;
    STRING		file = NULL;
    int			name_len = 0;

    bil_list = util_malloc(MAXPATHLEN * sizeof(char));
    if (bil_list == NULL)
	return bil_list;

    *bil_list = 0;
    for (trav_open_cond(&trav, proj, AB_TRAV_MODULES, obj_is_defined_module);
	(child = trav_next(&trav)) != NULL ; )
    {
	file = obj_get_file(child);
	name_len = name_len + strlen(file) + 2;	/* for \0 and blank */

	if (name_len > MAXPATHLEN)
	    bil_list = (STRING) realloc(bil_list, name_len);

	if (first)
	{
	    first = 0;
	    if (strcpy(bil_list, file) == NULL)
	    {
		if (bil_list) util_free(bil_list);
	  	return NULL;
	    }
	}
	else
	{
	    strcat(bil_list, " ");
	    strcat(bil_list, file);
 	}
    }
    trav_close(&trav);

    return (bil_list);
}


int
bil_save_tree(
    ABObj      		root,
    STRING		file_name,
    STRING		old_proj_dir,
    BIL_SAVE_TYPE	save_type
)
{
    int		return_value = OK;
    STRING	errmsg = NULL;
    FILE	*bilOutFile= NULL;
    int		filesWritten = 0;
    STRING	old_file = NULL;
    int		old_file_len = 0;

    if (save_type == BIL_SAVE_DUMP)
    {
	save_type = BIL_SAVE_ENCAPSULATED;
    }

    if (root == NULL)
    {
	goto epilogue;
    }

    if (file_name == NULL)
    {
	file_name = obj_get_file(root);
    }
    if (file_name == NULL)
    {
	return_value = ERR_CLIENT;
	goto epilogue;
    }

    #ifdef BOGUS
    if (obj_is_project(root) && (save_type == BIL_SAVE_FILE_PER_MODULE))
    {
	/*
	 * Only save the project, itself
	 */
        if (store_project_tree(root, file_name, save_type) < 0)
        {
	    return_value = ERR_OPEN;
	    goto epilogue;
        }
    }
    else
    #endif /* BOGUS */
    {
	/*
	 * Save the whole tree
	 */

  	/* Check root's old filename. If it is not NULL, then
	 * that old file should be opened and any beginning 
	 * comments should be preserved in the new file. If it 
	 * is NULL, then the object (module or project) has 
	 * never been saved, so pass in NULL for old file.
	 */
	if (old_proj_dir != NULL)
	{
	    old_file = obj_get_file(root);

	    /* If the old filename is an absolute path, then
	     * we simply use that to check the old file for
	     * comments.  If it's not an absolute path (e.g.
	     * a module was stored in the same directory as
	     * the project (most likely case) OR the module
	     * was saved to a subdirectory of the project dir,
	     * then we have to cobble together an absolute
	     * path for the old file.
	     */
	    if ((old_file != NULL) && !util_path_is_absolute(old_file))
	    {
		old_file_len = strlen(old_file);
		old_file = (STRING) NULL;
		old_file = (STRING) util_malloc(old_file_len +
				strlen(old_proj_dir) +2);
		sprintf(old_file, "%s/%s", old_proj_dir, obj_get_file(root));
	    }
	}
	else if (save_type == BIL_SAVE_FILE_PER_MODULE)
	{
	    /* If old_proj_dir is NULL and we're saving
	     * a module or project, then that means that
 	     * the old file is located in the current dir-
	     * ectory, so we don't have to cobble together
	     * an absolute path to find it.  If the save_
	     * type is BIL_SAVE_ENCAPSULATED, then old_file
	     * will be NULL, because we're not implementing
	     * saving the comments from the old bip and bil 
	     * files in the bix file at this time.
	     */
	    old_file = obj_get_file(root);
	}

	/* Open the bil output file */
	return_value = abio_open_bil_output(file_name, old_file, &bilOutFile);
	if (return_value != OK)
	{
	    goto epilogue;
	}

	/*
	 * Write out the top part of the tree (or perhaps a module)
	 */
	bilP_write_tree(bilOutFile, root, save_type);

	/*
	 * See if we need to write out the modules, as well.
	 */
	if ((save_type != BIL_SAVE_FILE_PER_MODULE) && (!obj_is_module(root)))
	{
	    ABObj		module = NULL;
	    AB_TRAVERSAL	trav;
	    for (trav_open(&trav, root, AB_TRAV_MODULES);
		(module = trav_next(&trav)) != NULL; )
	    {
		bilP_write_tree(bilOutFile, module, save_type);
	    }
	    trav_close(&trav);
	}

	/* write out end-of-data marker */
	if (save_type == BIL_SAVE_ENCAPSULATED)
	{
	    abio_printf(bilOutFile, "%s\n", 
		bilP_token_to_string(AB_BIL_DATA_END));
	}

	/* Close the bil output file */
	abio_close_output(bilOutFile);
    }

epilogue:
    return return_value;
}


#ifdef BOGUS
static int
store_project_tree(
    ABObj		root,
    STRING		filename,
    BIL_SAVE_TYPE	save_type
)
{
    int		return_value = 0;
    FILE	*bilOutFile= NULL;
    STRING	errmsg = NULL;

    if ( (root != NULL) && (filename != NULL) )
    {
	errmsg = abio_open_bil_output(filename, &bilOutFile);
	if (errmsg != NULL)
        {
	    util_printf_err("%s\n", errmsg);
	    return_value = -1;
	    goto epilogue;
        }
	store_object(bilOutFile, root, save_type);
	save_cross_module_actions(bilOutFile, root, save_type);
	/* This ends the project  file */
	abio_puts(bilOutFile, abio_bil_object_end_string());
	abio_putc(bilOutFile, '\n');
	abio_close_output(bilOutFile);
    }
    else
    {
	util_printf_err(
	    catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 45,
	     "Cannot save project; Either nothing to store or NULL filename."));
    }

epilogue:
    return return_value;
}
#endif /* BOGUS */


#ifdef DEBUG

static int	bad_obj_msg(
			ABObj	obj, 
			STRING	fieldName,
			int	return_value, 
			STRING	file, 
			int	line
		);

#define bad_obj(_obj,_fieldName) \
	    (bad_obj_msg(_obj,_fieldName, -1, __FILE__, __LINE__))
 
/* Check things that obj_verify() can't check, like
 * undefined types and invalid subtypes. Check for
 * attributes that should not be undefined, etc. 
 */
static int
bilP_obj_verify(
    ABObj	obj
)
{
    int			rc = 0;			/* return code */
    AB_OBJECT_TYPE	type = AB_TYPE_UNDEF;
    ABObj		project = NULL;
    char		name[1024];
    *name = 0;

    if (!debugging())
    {
	return 0;
    }
    if ((rc = obj_verify(obj)) < 0)
    {
	return rc;
    }

    project = obj_get_project(obj);

    /* Check object type */
    type = obj_get_type(obj);
    if (type == AB_TYPE_UNDEF)
    {
	return bad_obj(obj, "object type");
    }

    if (obj_is_salient_ui(obj) && util_strempty(obj_get_name(obj)))
    {
	return bad_obj(obj, "name");
    }

    /* Check object subtype */
    switch (type)
    {
	case AB_TYPE_ACTION:
	    if (obj_get_when(obj) == AB_WHEN_UNDEF)
	    {
		return bad_obj(obj, "when");
	    }
	    if (obj_get_from(obj) == NULL)
	    {
		return bad_obj(obj, "from");
	    }
	    if (obj_get_func_type(obj) == AB_FUNC_UNDEF)
	    {
		return bad_obj(obj, "func_type");
	    }
	break;

	case AB_TYPE_BUTTON:
	    if (obj_get_button_type(obj) == AB_BUT_UNDEF)
	    {
		return bad_obj(obj, "button type");
	    }
	    break;

	case AB_TYPE_CONTAINER:
	    if (   (obj_get_container_type(obj) == AB_CONT_UNDEF)
		|| (obj_get_container_type(obj) == AB_CONT_ABSOLUTE) )
	    {
		/* absolute is no longer supported by BIL */
		return bad_obj(obj, "container type");
	    }
	    else 
	    {
		if (   (obj_get_container_type(obj) == AB_CONT_GROUP)
		    && (obj_get_group_type(obj) == AB_GROUP_UNDEF) )
		{
		    return bad_obj(obj, "group type");
		}
	    }
	    break;

	case AB_TYPE_MESSAGE:
	    if (obj_get_msg_type(obj) == AB_MSG_UNDEF)
	    {
		return bad_obj(obj, "message type");
	    }
	    break;

	case AB_TYPE_ITEM:
	    if (obj_get_item_type(obj) == AB_ITEM_FOR_UNDEF)
	    {
		return bad_obj(obj, "item type");
	    }
	    break;

	case AB_TYPE_CHOICE:
	    if (obj_get_choice_type(obj) == AB_CHOICE_UNDEF)
	    {
		return bad_obj(obj, "choice type");
	    }
	    break;

	case AB_TYPE_LABEL:
	    if (obj_get_label_type(obj) == AB_LABEL_UNDEF)
	    {
		return bad_obj(obj, "label type");
	    }
	    break;

	case AB_TYPE_TEXT_FIELD:
	case AB_TYPE_TEXT_PANE:
	    if (obj_get_text_type(obj) == AB_TEXT_UNDEF)
	    {
		return bad_obj(obj, "text type");
	    }
	    break;

	default:
	    break;
    }

    /* Check other attributes */
    switch (type)
    {
	case AB_TYPE_CONTAINER:
	    if ((obj_get_container_type(obj) == AB_CONT_GROUP) ||
		(obj_get_container_type(obj) == AB_CONT_PANED) ||
		(obj_get_container_type(obj) == AB_CONT_RELATIVE))
	    {
		if (check_attachments(obj) < 0)
	        {
		    return bad_obj(obj, "attachment");
	        }
		if (obj_get_container_type(obj) == AB_CONT_GROUP)
		{
		    if ((obj_get_row_align(obj) == AB_ALIGN_UNDEF) ||
			(obj_get_col_align(obj) == AB_ALIGN_UNDEF))
	            {
			return bad_obj(obj, "alignment");
		    }
	        }
	    }
	    break;

	case AB_TYPE_BUTTON:
	    if (check_attachments(obj) < 0)
	    {
	        return bad_obj(obj, "attachment");
	    }
	    if (!util_strempty(obj_get_label(obj)))
	    {
		if (obj_get_label_alignment(obj) == AB_ALIGN_UNDEF)
	        {
	            return bad_obj(obj, "label alignment");
	        }
	    }
	    break;

	case AB_TYPE_LABEL:
            if (check_attachments(obj) < 0)
            {   
                return bad_obj(obj, "attachment");
            }   
	    if (!util_strempty(obj_get_label(obj)))
	    {
		if (obj_get_label_alignment(obj) == AB_ALIGN_UNDEF)
		{
		    return bad_obj(obj, "label alignment");
		}
	    }
	    break;

	case AB_TYPE_CHOICE:
            if (check_attachments(obj) < 0)
            {   
                return bad_obj(obj, "attachment");
            }   
	    if (obj_get_orientation(obj) == AB_ORIENT_UNDEF)
	    {
		return bad_obj(obj, "orientation");
	    }
	    if (!util_strempty(obj_get_label(obj)))
	    {
		if (obj_get_label_position(obj) == AB_CP_UNDEF)
		{
		     return bad_obj(obj, "label position");
		}
	    }
	    break;

        case AB_TYPE_SEPARATOR:
            if (check_attachments(obj) < 0)
            {   
                return bad_obj(obj, "attachment");
            }   
	    if (obj_get_orientation(obj) == AB_ORIENT_UNDEF)
	    {
		return bad_obj(obj, "orientation");
	    }
	    break;

        case AB_TYPE_SCALE:
            if (check_attachments(obj) < 0)
            {   
                return bad_obj(obj, "attachment");
            }   
	    if ((obj_get_orientation(obj) == AB_ORIENT_UNDEF) ||
	    	(obj_get_direction(obj) == AB_DIR_UNDEF))
	    {
		return bad_obj(obj, "orientation");
	    }
	    if (!util_strempty(obj_get_label(obj)))
	    {
		if (obj_get_label_position(obj) == AB_CP_UNDEF)
		{
		    return bad_obj(obj, "label position");
		}
	    }
	    break;

	case AB_TYPE_ACTION:
	    if (obj_get_func_type(obj) == AB_FUNC_UNDEF)
	    {
		return bad_obj(obj, "func type");
	    }

	    switch (obj_get_func_type(obj))
	    {
		case AB_FUNC_BUILTIN:
		    if (obj_get_func_builtin(obj) == AB_STDACT_UNDEF)
		    {
			return bad_obj(obj, "builtin action type");
		    }
		    break;
		case AB_FUNC_USER_DEF:
		    if (obj_get_func_name(obj) == NULL)
		    {
			return bad_obj(obj, "function name");
		    }
		    break;
		case AB_FUNC_CODE_FRAG:
                    if (obj_get_func_code(obj) == NULL)
		    {
			return bad_obj(obj, "user code");
		    }
		    break;
	    }

	    if (obj_get_from(obj) == NULL)
	    {
		return bad_obj(obj, "from");
	    }
	    if (obj_get_func_type(obj) == AB_FUNC_BUILTIN)
            {
                if (obj_get_to(obj) == NULL)
		{
		    return bad_obj(obj, "to");
		}
	    }
	    if (obj_get_when(obj) == AB_WHEN_UNDEF)
	    {
		return bad_obj(obj, "when");
	    }
	    if (obj_get_arg_type(obj) == AB_ARG_UNDEF)
	    {
		return bad_obj(obj, "arg type");
	    }
	    break;

	case AB_TYPE_COMBO_BOX:
            if (check_attachments(obj) < 0)
            {   
		return bad_obj(obj, "attachment");
            }   
	    if (!util_strempty(obj_get_label(obj)))
	    {
		if (obj_get_label_position(obj) == AB_CP_UNDEF)
		{
		    return bad_obj(obj, "label position");
		}
	    }
	    break;

        case AB_TYPE_SPIN_BOX:
            if (check_attachments(obj) < 0)
            {   
		return bad_obj(obj, "attachment");
            }   
            if (!util_strempty(obj_get_label(obj)))
            {
                if (obj_get_label_position(obj) == AB_CP_UNDEF)
		{
		    return bad_obj(obj, "label position");
		}
            }        
            break;

        case AB_TYPE_LIST:
            if (check_attachments(obj) < 0)
            {   
		return bad_obj(obj, "attachment");
            }   
	    if (!util_strempty(obj_get_label(obj)))
	    {
		if (obj_get_label_position(obj) == AB_CP_UNDEF)
		{
		    return bad_obj(obj, "label position");
		}
	    }
	    break;

        case AB_TYPE_TEXT_FIELD:
            if (check_attachments(obj) < 0)
            {   
		return bad_obj(obj, "attachment");
            }   
            if (!util_strempty(obj_get_label(obj)))
            {
                if (obj_get_label_position(obj) == AB_CP_UNDEF)
		{
		    return bad_obj(obj, "label position");
		}
            }        
	    break;

        case AB_TYPE_DIALOG:
            if (obj_get_win_parent(obj) != NULL)
	    {
		if (obj_verify(obj_get_win_parent(obj)) < 0)
		{
		    return bad_obj(obj, "win_parent");
		}
	    }
	    break;

        case AB_TYPE_FILE_CHOOSER:
	    if (obj_get_file_type_mask(obj) == AB_FILE_TYPE_MASK_UNDEF)
	    {
		return bad_obj(obj, "file_type_mask");
	    }
	    break;

	case AB_TYPE_LAYERS:
	case AB_TYPE_TEXT_PANE:
	case AB_TYPE_DRAWING_AREA:
	    if (check_attachments(obj) < 0)
	    {
		return bad_obj(obj, "attachment");
	    }
	    break;

	default:
	    break;
    }

    /*
     * Windows should not have other objects as parents.
     * :win-children attribute handles associating popups with main windows
     */
    if (obj_is_window(obj))
    {
	ABObj	parent = obj_get_parent(obj);
	ABObj	root_window = obj_get_root_window(project);
	ABObj	win_parent = obj_get_win_parent(obj);

	if (!obj_is_module(parent))
	{
	    return bad_obj(obj, "parent");
	}
	if ((win_parent != NULL) && (!obj_is_window(win_parent)))
	{
	    return bad_obj(obj, "win_parent");
	}
    }

    /*
     * messages should be parented to the module
     */
    if (obj_is_message(obj))
    {
	if (!obj_is_module(obj_get_parent(obj)))
	{
	    return bad_obj(obj, "parent");
	}
    }

    /*
     * As a final check, see if we can determine what attributes this
     * object needs.
     */
    if (get_object_attrs(obj) == NULL)
    {
	return bad_obj(obj, "BIL attributes");
    }

    return 0;
}


/*
 * Prints out a message about a bad object and possibly aborts
 */
static int	bad_obj_msg(
			ABObj	obj, 
			STRING	fieldName,
			int	return_value, 
			STRING	file, 
			int	line
)
{
    char	name[1024];
    *name = 0;
    util_dprintf(1, "Bad %s (%s:%d)! %s\n",
	fieldName, file, line, obj_get_safe_name(obj, name, 1024));
    util_dabort(5);
    return return_value;
}



static BOOL
attachment_is_ok(
    ABObj		obj,
    AB_COMPASS_POINT	attach_dir
)
{
    AB_ATTACH_TYPE      attach_type = AB_ATTACH_UNDEF;
    ABObj               attach_obj = NULL;
    BOOL		attachment_ok = TRUE;

    attach_type = obj_get_attach_type(obj, attach_dir);

    switch (attach_type)
    {
	case AB_ATTACH_OBJ:
	case AB_ATTACH_ALIGN_OBJ_EDGE:
	    attach_obj = (ABObj)obj_get_attach_value(obj, attach_dir);
	    if (obj_verify(attach_obj) < 0) 
	        attachment_ok = FALSE;
	break;
    }

    return (attachment_ok);
}


static int
check_attachments(
    ABObj	obj
)
{
    int			iRet = 0;
    ABObj		parent = NULL;

    if (obj == NULL)
	return iRet;

    if (obj_get_parent(obj) != NULL)
	parent = obj_get_root(obj_get_parent(obj));
	   
    if ( obj_is_group_member(obj) &&
	 ((obj_get_group_type(parent) == AB_GROUP_COLUMNS) ||
	  (obj_get_group_type(parent) == AB_GROUP_ROWS) ||
	  (obj_get_group_type(parent) == AB_GROUP_ROWSCOLUMNS))
       )
    {
	if (obj_has_flag(obj_get_module(obj), MappedFlag))
	{
	    if (!attachment_is_ok(obj, AB_CP_NORTH) ||
		!attachment_is_ok(obj, AB_CP_SOUTH) ||
		!attachment_is_ok(obj, AB_CP_EAST)  ||
		!attachment_is_ok(obj, AB_CP_WEST)) 
	    {
		iRet = -1;
	    }   
	}
    }
    else
    {    
	if( !attachment_is_ok(obj, AB_CP_NORTH) ||
            !attachment_is_ok(obj, AB_CP_SOUTH) ||
            !attachment_is_ok(obj, AB_CP_EAST)  ||
            !attachment_is_ok(obj, AB_CP_WEST) )
        {   
	    iRet = -1;   
        }
    }

    return iRet;
}


/* REMIND: move to libAButil */
static int
util_dabort_impl(int debug_abort_level)
{
    if (debug_level() >= debug_abort_level)
    {
	abort();
	return -1;		/* shouldn't execute */
    }
    return 0;
}

#endif /* DEBUG */
