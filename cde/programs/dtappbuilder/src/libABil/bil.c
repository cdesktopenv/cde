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
 * $XConsortium: bil.c /main/3 1995/11/06 18:22:40 rswiston $
 * 
 *	@(#)bil.c	1.87 22 May 1995	
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


/************************************************************
*                                                           *
* bil.c - BIL conversion routines                           *
*                                                           *
************************************************************/

#include <string.h>

/*
 * #include "loadP.h"
 */
#include <ab_private/istr.h>
#include "bilP.h"

/*************************************************************************
**
**
**       Function Definitions
**
**
**
**************************************************************************/
/*
BIL_TOKEN
bilP_menu_type_to_token(AB_MENU_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
  
    switch (type)
    {   
    case AB_MENU_PULLDOWN: token = AB_BIL_PULLDOWN;
        break;
    case AB_MENU_POPUP: token = AB_BIL_POPUP;
        break;
    case AB_MENU_OPTION: token = AB_BIL_OPTION;
        break;
    }   
    return (token);
}    
 
AB_MENU_TYPE
bilP_token_to_menu_type(int token)
{
    AB_MENU_TYPE        obj_type = AB_MENU_UNDEF;
 
    switch (token)
    {   
    case AB_BIL_PULLDOWN: obj_type = AB_MENU_PULLDOWN;
        break;
    case AB_BIL_POPUP: obj_type = AB_MENU_POPUP;
        break;
    case AB_BIL_OPTION: obj_type = AB_MENU_OPTION;
        break;
    }   
    return (obj_type);
}
*/

BIL_TOKEN
bilP_packing_to_token(AB_PACKING type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_PACK_NONE: token = AB_BIL_NONE;
	break;
    case AB_PACK_TIGHT: token = AB_BIL_TIGHT;
	break;
    case AB_PACK_EQUAL: token = AB_BIL_EQUAL;
	break;
    }
    return (token);
}

AB_PACKING
bilP_token_to_packing(int type)
{
    AB_PACKING          obj_type = AB_PACK_UNDEF;

    switch (type)
    {
    case AB_BIL_NONE: obj_type = AB_PACK_NONE;
	break;
    case AB_BIL_TIGHT: obj_type = AB_PACK_TIGHT;
	break;
    case AB_BIL_EQUAL: obj_type = AB_PACK_EQUAL;
	break;
    }
    return (obj_type);
}

BIL_TOKEN
bilP_container_type_to_token(AB_CONTAINER_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_CONT_BUTTON_PANEL: token = AB_BIL_BUTTON_PANEL;
	break;
    case AB_CONT_ABSOLUTE: token = AB_BIL_ABSOLUTE;
	break;
    case AB_CONT_FOOTER: token = AB_BIL_FOOTER;
	break;
    case AB_CONT_GROUP: token = AB_BIL_GROUP;
	break;
    case AB_CONT_MAIN_WINDOW: token = AB_BIL_MAIN_WINDOW;
	break;
    case AB_CONT_MENU_BAR: token = AB_BIL_MENU_BAR;
	break;
    case AB_CONT_PANED: token = AB_BIL_PANED;
	break;
    case AB_CONT_RELATIVE: token = AB_BIL_RELATIVE;
	break;
    case AB_CONT_SCROLLED_WINDOW: token = AB_BIL_SCROLLED_WINDOW;
	break;
    case AB_CONT_ROW_COLUMN: token = AB_BIL_ROW_COLUMN;
	break;
    case AB_CONT_TOOL_BAR: token = AB_BIL_TOOL_BAR;
    default:
	break;
    }
    return (token);
}

AB_CONTAINER_TYPE
bilP_token_to_container_type(int type)
{
    AB_CONTAINER_TYPE   obj_type = AB_CONT_UNDEF;
    switch (type)
    {
    case AB_BIL_BUTTON_PANEL:
    case AB_BIL_ACTIVATED: obj_type = AB_CONT_BUTTON_PANEL;
	break;
    case AB_BIL_ABSOLUTE: obj_type = AB_CONT_ABSOLUTE;
	break;
    case AB_BIL_FOOTER: obj_type = AB_CONT_FOOTER;
	break;
    case AB_BIL_GROUP: obj_type = AB_CONT_GROUP;
	break;
    case AB_BIL_MAIN_WINDOW: obj_type = AB_CONT_MAIN_WINDOW;
	break;
    case AB_BIL_MENU_BAR: obj_type = AB_CONT_MENU_BAR;
	break;
    case AB_BIL_PANED: obj_type = AB_CONT_PANED;
	break;
    case AB_BIL_RELATIVE: obj_type = AB_CONT_RELATIVE;
	break;
    case AB_BIL_SCROLLED_WINDOW: obj_type = AB_CONT_SCROLLED_WINDOW;
	break;
    case AB_BIL_ROW_COLUMN: obj_type = AB_CONT_ROW_COLUMN;
	break;
    case AB_BIL_TOOL_BAR: obj_type = AB_CONT_TOOL_BAR;
	break;
    }
    return (obj_type);
}

BIL_TOKEN
bilP_item_type_to_token(AB_ITEM_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_ITEM_FOR_CHOICE: token = AB_BIL_ITEM_FOR_CHOICE;
	break;
    case AB_ITEM_FOR_COMBO_BOX: token = AB_BIL_ITEM_FOR_COMBO_BOX;
	break;
    case AB_ITEM_FOR_LIST: token = AB_BIL_ITEM_FOR_LIST;
	break;
    case AB_ITEM_FOR_MENU: token = AB_BIL_ITEM_FOR_MENU;
	break;
    case AB_ITEM_FOR_MENUBAR: token = AB_BIL_ITEM_FOR_MENUBAR;
	break;
    case AB_ITEM_FOR_SPIN_BOX: token = AB_BIL_ITEM_FOR_SPIN_BOX;
	break;
    }
    return (token);
}

AB_ITEM_TYPE
bilP_token_to_item_type(int type)
{
    AB_ITEM_TYPE        obj_type = AB_ITEM_FOR_UNDEF;

    switch (type)
    {
    case AB_BIL_ITEM_FOR_CHOICE: obj_type = AB_ITEM_FOR_CHOICE;
	break;
    case AB_BIL_ITEM_FOR_COMBO_BOX: obj_type = AB_ITEM_FOR_COMBO_BOX;
	break;
    case AB_BIL_ITEM_FOR_LIST: obj_type = AB_ITEM_FOR_LIST;
	break;
    case AB_BIL_ITEM_FOR_MENU: obj_type = AB_ITEM_FOR_MENU;
	break;
    case AB_BIL_ITEM_FOR_MENUBAR: obj_type = AB_ITEM_FOR_MENUBAR;
	break;
    case AB_BIL_ITEM_FOR_SPIN_BOX: obj_type = AB_ITEM_FOR_SPIN_BOX;
	break;
    }
    return (obj_type);
}

BIL_TOKEN
bilP_label_style_to_token(AB_LABEL_STYLE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_STYLE_NORMAL: token = AB_BIL_STYLE_NORMAL;
	break;
    case AB_STYLE_BOLD: token = AB_BIL_STYLE_BOLD;
	break;
    case AB_STYLE_3D: token = AB_BIL_STYLE_3D;
	break;
    default:
	break;
    }
    return (token);
}

AB_LABEL_STYLE
bilP_token_to_label_style(int type)
{
    AB_LABEL_STYLE      obj_type;

    switch (type)
    {
    case AB_BIL_STYLE_NORMAL: obj_type = AB_STYLE_NORMAL;
	break;
    case AB_BIL_STYLE_BOLD: obj_type = AB_STYLE_BOLD;
	break;
    case AB_BIL_STYLE_3D: obj_type = AB_STYLE_3D;
	break;
    }
    return (obj_type);
}

BIL_TOKEN
bilP_attachment_to_token(AB_ATTACH_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_ATTACH_POINT: token = AB_BIL_ATTACH_POINT;
	break;
    case AB_ATTACH_OBJ: token = AB_BIL_ATTACH_OBJ;
	break;
    case AB_ATTACH_ALIGN_OBJ_EDGE: token = AB_BIL_ATTACH_ALIGN_OBJ_EDGE;
	break;
    case AB_ATTACH_GRIDLINE: token = AB_BIL_ATTACH_GRIDLINE;
	break;
    case AB_ATTACH_CENTER_GRIDLINE: token = AB_BIL_ATTACH_CENTER_GRIDLINE;
	break;
    case AB_ATTACH_NONE: token = AB_BIL_NONE;
	break;
    }
    return (token);
}

AB_ATTACH_TYPE
bilP_token_to_attachment(int type)
{
    AB_ATTACH_TYPE      obj_type = AB_ATTACH_UNDEF;

    switch (type)
    {
    case AB_BIL_ATTACH_POINT: obj_type = AB_ATTACH_POINT;
	break;
    case AB_BIL_ATTACH_OBJ: obj_type = AB_ATTACH_OBJ;
	break;
    case AB_BIL_ATTACH_ALIGN_OBJ_EDGE:
	obj_type = AB_ATTACH_ALIGN_OBJ_EDGE;
	break;
    case AB_BIL_ATTACH_GRIDLINE: obj_type = AB_ATTACH_GRIDLINE;
	break;
    case AB_BIL_ATTACH_CENTER_GRIDLINE: obj_type = AB_ATTACH_CENTER_GRIDLINE;
	break;
    case AB_BIL_NONE: obj_type = AB_ATTACH_NONE;
	break;
    }
    return (obj_type);
}

BIL_TOKEN
bilP_direction_to_token(AB_DIRECTION dir)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (dir)
    {
    case AB_DIR_LEFT_TO_RIGHT: token = AB_BIL_LEFT_TO_RIGHT;
        break;
    case AB_DIR_RIGHT_TO_LEFT: token = AB_BIL_RIGHT_TO_LEFT;
        break;
    case AB_DIR_TOP_TO_BOTTOM: token = AB_BIL_TOP_TO_BOTTOM;
        break;
    case AB_DIR_BOTTOM_TO_TOP: token = AB_BIL_BOTTOM_TO_TOP;
        break;
    }
    return (token);
}
AB_DIRECTION
bilP_token_to_direction(int type)
{
    AB_DIRECTION	dir = AB_DIR_UNDEF;

    switch (type)
    {
    case AB_BIL_LEFT_TO_RIGHT: dir = AB_DIR_LEFT_TO_RIGHT;
        break;
    case AB_BIL_RIGHT_TO_LEFT: dir = AB_DIR_RIGHT_TO_LEFT;
	break;
    case AB_BIL_TOP_TO_BOTTOM: dir = AB_DIR_TOP_TO_BOTTOM;
        break;
    case AB_BIL_BOTTOM_TO_TOP: dir = AB_DIR_BOTTOM_TO_TOP;
	break;
    }
    return (dir);
}

AB_FUNC_TYPE
bilP_token_to_func_type(int type)
{
    AB_FUNC_TYPE        obj_type = AB_FUNC_UNDEF;

    switch (type)
    {
    case AB_BIL_BUILTIN: obj_type = AB_FUNC_BUILTIN;
	break;
    case AB_BIL_CALL_FUNCTION: obj_type = AB_FUNC_USER_DEF;
	break;
    case AB_BIL_EXECUTE_CODE: obj_type = AB_FUNC_CODE_FRAG;
	break;
    case AB_BIL_ACCESS_HELP_VOLUME: obj_type = AB_FUNC_HELP_VOLUME;
	break;
    case AB_BIL_ON_ITEM_HELP: obj_type = AB_FUNC_ON_ITEM_HELP;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_func_type_to_token(AB_FUNC_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
    case AB_FUNC_BUILTIN: token = AB_BIL_BUILTIN;
	break;
    case AB_FUNC_USER_DEF: token = AB_BIL_CALL_FUNCTION;
	break;
    case AB_FUNC_CODE_FRAG: token = AB_BIL_EXECUTE_CODE;
	break;
    case AB_FUNC_ON_ITEM_HELP: token = AB_BIL_ON_ITEM_HELP;
	break;
    case AB_FUNC_HELP_VOLUME: token = AB_BIL_ACCESS_HELP_VOLUME;
	break;
    }
    return token;
}

BOOL
bilP_token_to_bool(BIL_TOKEN token)
{
    BOOL                boolVal = FALSE;

    switch (token)
    {
    case AB_BIL_FALSE: boolVal = FALSE;
	break;
    case AB_BIL_TRUE: boolVal = TRUE;
	break;
    }

    return boolVal;
}

BIL_TOKEN
bilP_bool_to_token(BOOL boolVal)
{
    return (boolVal ? AB_BIL_TRUE : AB_BIL_FALSE);
}

AB_BUILTIN_ACTION
bilP_token_to_builtin_action(int type)
{				/* REMIND: will need to add more */
    AB_BUILTIN_ACTION   obj_type = AB_STDACT_UNDEF;

    switch (type)
    {
    case AB_BIL_DISABLE: obj_type = AB_STDACT_DISABLE;
	break;
    case AB_BIL_ENABLE: obj_type = AB_STDACT_ENABLE;
	break;
    case AB_BIL_HIDE: obj_type = AB_STDACT_HIDE;
	break;
    case AB_BIL_SET_LABEL: obj_type = AB_STDACT_SET_LABEL;
	break;
    case AB_BIL_SET_TEXT: obj_type = AB_STDACT_SET_TEXT;
	break;
    case AB_BIL_SET_VALUE: obj_type = AB_STDACT_SET_VALUE;
	break;
    case AB_BIL_SHOW: obj_type = AB_STDACT_SHOW;
	break;
    case AB_BIL_UNDEF:
    case AB_BIL_UNDEF_KEYWORD:
	obj_type = AB_STDACT_UNDEF;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_builtin_action_to_token(AB_BUILTIN_ACTION action)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (action)
    {
	case AB_STDACT_DISABLE: token = AB_BIL_DISABLE;
	    break;
	case AB_STDACT_ENABLE: token = AB_BIL_ENABLE;
	    break;
	case AB_STDACT_HIDE: token = AB_BIL_HIDE;
	    break;
	case AB_STDACT_SET_LABEL: token = AB_BIL_SET_LABEL;
	    break;
	case AB_STDACT_SET_TEXT: token = AB_BIL_SET_TEXT;
	    break;
	case AB_STDACT_SET_VALUE: token = AB_BIL_SET_VALUE;
	    break;
	case AB_STDACT_SHOW: token = AB_BIL_SHOW;
	    break;
	case AB_STDACT_UNDEF: token = AB_BIL_UNDEF_KEYWORD;
    }
    return token;
}

AB_COMPASS_POINT
bilP_token_to_compass_point(int type)
{
    AB_COMPASS_POINT    obj_type = AB_CP_UNDEF;

    switch (type)
    {
    case AB_BIL_UNDEF: 
    case AB_BIL_UNDEF_KEYWORD: obj_type = AB_CP_UNDEF;
	break;
    case AB_BIL_NORTH: obj_type = AB_CP_NORTH;
	break;
    case AB_BIL_NORTHEAST: obj_type = AB_CP_NORTHEAST;
	break;
    case AB_BIL_EAST: obj_type = AB_CP_EAST;
	break;
    case AB_BIL_SOUTHEAST: obj_type = AB_CP_SOUTHEAST;
	break;
    case AB_BIL_SOUTH: obj_type = AB_CP_SOUTH;
	break;
    case AB_BIL_SOUTHWEST: obj_type = AB_CP_SOUTHWEST;
	break;
    case AB_BIL_WEST: obj_type = AB_CP_WEST;
	break;
    case AB_BIL_NORTHWEST: obj_type = AB_CP_NORTHWEST;
	break;
    case AB_BIL_CENTER: obj_type = AB_CP_CENTER;
	break;
    default:
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_compass_point_to_token(AB_COMPASS_POINT cp)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (cp)
    {
    case AB_CP_UNDEF: token = AB_BIL_UNDEF_KEYWORD;
	break;
    case AB_CP_NORTH: token = AB_BIL_NORTH;
	break;
    case AB_CP_NORTHEAST: token = AB_BIL_NORTHEAST;
	break;
    case AB_CP_EAST: token = AB_BIL_EAST;
	break;
    case AB_CP_SOUTHEAST: token = AB_BIL_SOUTHEAST;
	break;
    case AB_CP_SOUTH: token = AB_BIL_SOUTH;
	break;
    case AB_CP_SOUTHWEST: token = AB_BIL_SOUTHWEST;
	break;
    case AB_CP_WEST: token = AB_BIL_WEST;
	break;
    case AB_CP_NORTHWEST: token = AB_BIL_NORTHWEST;
	break;
    case AB_CP_CENTER: token = AB_BIL_CENTER;
	break;
    default:
	break;
    }
    return token;
}

ABDndOpFlags		
bilP_token_to_dnd_op_flag(BIL_TOKEN token)
{
    ABDndOpFlags	flag = 0;

    switch (token)
    {
	case AB_BIL_COPY: flag = ABDndOpCopy; break;
	case AB_BIL_MOVE: flag = ABDndOpMove; break;
	case AB_BIL_LINK: flag = ABDndOpLink; break;
    }
    return flag;
}


BIL_TOKEN
bilP_dnd_op_flag_to_token(BYTE opFlag)
{
    BIL_TOKEN	token = AB_BIL_UNDEF;

    switch (opFlag)
    {
	case ABDndOpCopy: token = AB_BIL_COPY; break;
	case ABDndOpLink: token = AB_BIL_LINK; break;
	case ABDndOpMove: token = AB_BIL_MOVE; break;
    }

    return token;
}

ABDndTypeFlags		
bilP_token_to_dnd_type_flag(BIL_TOKEN token)
{
    ABDndTypeFlags	typeFlag = 0;
    switch (token)
    {
	case AB_BIL_ANY:	typeFlag = ABDndTypeAny; break;
	case AB_BIL_FILENAME:	typeFlag = ABDndTypeFilename; break;
	case AB_BIL_TEXT: 	typeFlag = ABDndTypeText; break;
	case AB_BIL_USER_DEF:	typeFlag = ABDndTypeUserDef; break;
    }
    return typeFlag;
}

BIL_TOKEN		
bilP_dnd_type_flag_to_token(BYTE typeFlag)
{
    BIL_TOKEN	token = AB_BIL_UNDEF;
    switch (typeFlag)
    {
	case ABDndTypeAny:	token = AB_BIL_ANY; break;
	case ABDndTypeFilename:	token = AB_BIL_FILENAME; break;
	case ABDndTypeText:	token = AB_BIL_TEXT; break;
	case ABDndTypeUserDef:	token = AB_BIL_USER_DEF; break;
    }
    return token;
}

AB_CHOICE_TYPE
bilP_token_to_choice_type(int type)
{
    AB_CHOICE_TYPE      obj_type = AB_CHOICE_UNDEF;

    switch (type)
    {
    case AB_BIL_EXCLUSIVE: obj_type = AB_CHOICE_EXCLUSIVE;
	break;
    case AB_BIL_NONEXCLUSIVE: obj_type = AB_CHOICE_NONEXCLUSIVE;
	break;
    case AB_BIL_OPTION_MENU: obj_type = AB_CHOICE_OPTION_MENU;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_choice_type_to_token(AB_CHOICE_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
    case AB_CHOICE_EXCLUSIVE: token = AB_BIL_EXCLUSIVE;
	break;
    case AB_CHOICE_NONEXCLUSIVE: token = AB_BIL_NONEXCLUSIVE;
	break;
    case AB_CHOICE_OPTION_MENU: token = AB_BIL_OPTION_MENU;
	break;
    }
    return token;
}

AB_ORIENTATION
bilP_token_to_orientation(int type)
{
    AB_ORIENTATION      obj_type = AB_ORIENT_UNDEF;

    switch (type)
    {
    case AB_BIL_HORIZONTAL: obj_type = AB_ORIENT_HORIZONTAL;
	break;
    case AB_BIL_VERTICAL: obj_type = AB_ORIENT_VERTICAL;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_orientation_to_token(AB_ORIENTATION orientation)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (orientation)
    {
    case AB_ORIENT_UNDEF: token = AB_BIL_UNDEF_KEYWORD;
	break;
    case AB_ORIENT_HORIZONTAL: token = AB_BIL_HORIZONTAL;
	break;
    case AB_ORIENT_VERTICAL: token = AB_BIL_VERTICAL;
	break;
    }
    return token;
}

AB_ARG_CLASS
bilP_token_to_arg_class(int token)
{
    AB_ARG_CLASS	argClass = AB_ARG_CLASS_UNDEF;
    switch (token)
    {
	case AB_BIL_COLOR:	argClass = AB_ARG_CLASS_COLOR; 
	break;
	case AB_BIL_GEOMETRY:	argClass = AB_ARG_CLASS_GEOMETRY;
	break;
	case AB_BIL_GLYPH:	argClass = AB_ARG_CLASS_GLYPH;
	break;
	case AB_BIL_LABEL_STRING: argClass = AB_ARG_CLASS_NON_GLYPH_LABEL;
	break;
	case AB_BIL_OTHER:	argClass = AB_ARG_CLASS_OTHER;
	break;
	case AB_BIL_OTHER_STRING: argClass = AB_ARG_CLASS_OTHER_STRING;
	break;
	case AB_BIL_VALUE:	  argClass = AB_ARG_CLASS_VALUE;
	break;
    }
    return argClass;
}

BIL_TOKEN
bilP_arg_class_to_token(AB_ARG_CLASS argClass)
{
    BIL_TOKEN	token = AB_BIL_UNDEF;
    switch (argClass)
    {
	case AB_ARG_CLASS_COLOR:	token = AB_BIL_COLOR;
	break;
	case AB_ARG_CLASS_GEOMETRY:	token = AB_BIL_GEOMETRY;
	break;
	case AB_ARG_CLASS_GLYPH:	token = AB_BIL_GLYPH;
	break;
	case AB_ARG_CLASS_NON_GLYPH_LABEL:	token = AB_BIL_LABEL_STRING;
	break;
	case AB_ARG_CLASS_OTHER:	token = AB_BIL_OTHER;
	break;
	case AB_ARG_CLASS_OTHER_STRING:	token = AB_BIL_OTHER_STRING;
	break;
	case AB_ARG_CLASS_VALUE:	token = AB_BIL_VALUE;
	break;
    }
    return token;
}

AB_ARG_TYPE
bilP_token_to_arg_type(int type)
{
    AB_ARG_TYPE         obj_type = AB_ARG_UNDEF;

    switch (type)
    {
    case AB_BIL_INT: obj_type = AB_ARG_INT;
	break;
    case AB_BIL_FLOAT: obj_type = AB_ARG_FLOAT;
	break;
    case AB_BIL_STRING: obj_type = AB_ARG_STRING;
	break;
    case AB_BIL_VOID: obj_type = AB_ARG_VOID_PTR;
	break;
    case AB_BIL_BOOLEAN: obj_type = AB_ARG_BOOLEAN;
	break;
    case AB_BIL_CALLBACK: obj_type = AB_ARG_CALLBACK;
	break;
    case AB_BIL_LITERAL: obj_type = AB_ARG_LITERAL;
	break;
    case AB_BIL_XMSTRING: obj_type = AB_ARG_XMSTRING;
	break;
    case AB_BIL_XMSTRING_TBL: obj_type = AB_ARG_XMSTRING_TBL;
	break;
    case AB_BIL_PIXEL: obj_type = AB_ARG_PIXEL;
	break;
    case AB_BIL_PIXMAP: obj_type = AB_ARG_PIXMAP;
	break;
    case AB_BIL_FONT: obj_type = AB_ARG_FONT;
	break;
    case AB_BIL_WIDGET: obj_type = AB_ARG_WIDGET;
	break;
    case AB_BIL_UNDEF_KEYWORD: case AB_BIL_UNDEF: obj_type = AB_ARG_UNDEF;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_arg_type_to_token(AB_ARG_TYPE arg_type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (arg_type)
    {
    case AB_ARG_BOOLEAN: token = AB_BIL_BOOLEAN;
	break;
    case AB_ARG_CALLBACK: token = AB_BIL_CALLBACK;
	break;
    case AB_ARG_FLOAT: token = AB_BIL_FLOAT;
	break;
    case AB_ARG_INT: token = AB_BIL_INT;
	break;
    case AB_ARG_LITERAL: token = AB_BIL_LITERAL;
	break;
    case AB_ARG_STRING: token = AB_BIL_STRING;
	break;
    case AB_ARG_XMSTRING: token = AB_BIL_XMSTRING;
	break;
    case AB_ARG_XMSTRING_TBL: token = AB_BIL_XMSTRING_TBL;
	break;
    case AB_ARG_PIXEL: token = AB_BIL_PIXEL;
	break;
    case AB_ARG_PIXMAP: token = AB_BIL_PIXMAP;
	break;
    case AB_ARG_FONT: token = AB_BIL_FONT;
	break;
    case AB_ARG_WIDGET: token = AB_BIL_WIDGET;
	break;
    case AB_ARG_VOID_PTR: token = AB_BIL_VOID;
	break;
    case AB_ARG_UNDEF: token = AB_BIL_UNDEF_KEYWORD;
	break;
    }
    return token;
}

AB_TEXT_TYPE
bilP_token_to_text_type(int type)
{
    AB_TEXT_TYPE        obj_type = AB_TEXT_UNDEF;

    switch (type)
    {
    case AB_BIL_ALPHANUMERIC: obj_type = AB_TEXT_ALPHANUMERIC;
	break;
    case AB_BIL_NUMERIC: obj_type = AB_TEXT_NUMERIC;
	break;
    case AB_BIL_DEFINED_STRING: obj_type = AB_TEXT_DEFINED_STRING;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_text_type_to_token(AB_TEXT_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
    case AB_TEXT_ALPHANUMERIC: token = AB_BIL_ALPHANUMERIC;
	break;
    case AB_TEXT_NUMERIC: token = AB_BIL_NUMERIC;
	break;
    case AB_TEXT_DEFINED_STRING: token = AB_BIL_DEFINED_STRING;
	break;
    }
    return token;
}

AB_BUTTON_TYPE
bilP_token_to_button_type(int type)
{
    AB_BUTTON_TYPE      obj_type = AB_BUT_UNDEF;

    switch (type)
    {
    case AB_BIL_PUSH_BUTTON: obj_type = AB_BUT_PUSH;
	break;
    case AB_BIL_DRAWN_BUTTON: obj_type = AB_BUT_DRAWN;
	break;
    case AB_BIL_MENU_BUTTON: obj_type = AB_BUT_MENU;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_button_type_to_token(AB_BUTTON_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
    case AB_BUT_PUSH: token = AB_BIL_PUSH_BUTTON;
	break;
    case AB_BUT_DRAWN: token = AB_BIL_DRAWN_BUTTON;
	break;
    case AB_BUT_MENU: token = AB_BIL_MENU_BUTTON;
	break;
    }
    return token;
}

AB_LABEL_TYPE
bilP_token_to_label_type(int type)
{
    AB_LABEL_TYPE       obj_type = AB_LABEL_UNDEF;

    switch (type)
    {
        case AB_BIL_ARROW_DOWN: obj_type = AB_LABEL_ARROW_DOWN;
            break;
        case AB_BIL_ARROW_LEFT: obj_type = AB_LABEL_ARROW_LEFT;
            break;
        case AB_BIL_ARROW_RIGHT: obj_type = AB_LABEL_ARROW_RIGHT;
            break;
        case AB_BIL_ARROW_UP: obj_type = AB_LABEL_ARROW_UP;
            break;
	case AB_BIL_STRING: obj_type = AB_LABEL_STRING;
	    break;
	case AB_BIL_GRAPHIC:
	case AB_BIL_ICON: obj_type = AB_LABEL_GLYPH;
	    break;
	case AB_BIL_SEPARATOR: obj_type = AB_LABEL_SEPARATOR;
	    break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_label_type_to_token(AB_LABEL_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
	case AB_LABEL_ARROW_DOWN: token = AB_BIL_ARROW_DOWN;
	    break;
	case AB_LABEL_ARROW_LEFT: token = AB_BIL_ARROW_LEFT;
	    break;
	case AB_LABEL_ARROW_RIGHT: token = AB_BIL_ARROW_RIGHT;
	    break;
	case AB_LABEL_ARROW_UP: token = AB_BIL_ARROW_UP;
	    break;
	case AB_LABEL_STRING: token = AB_BIL_STRING;
	    break;
	case AB_LABEL_GLYPH: token = AB_BIL_GRAPHIC;
	    break;
	case AB_LABEL_SEPARATOR: token = AB_BIL_SEPARATOR;
	break;
    }
    return token;
}

AB_GROUP_TYPE
bilP_token_to_group_type(int type)
{
    AB_GROUP_TYPE       obj_type = AB_GROUP_UNDEF;

    switch (type)
    {
    case AB_BIL_ROW: obj_type = AB_GROUP_ROWS;
	break;
    case AB_BIL_COLUMN: obj_type = AB_GROUP_COLUMNS;
	break;
    case AB_BIL_IGNORE: obj_type = AB_GROUP_IGNORE;
	break;
    case AB_BIL_ROWSCOLUMNS: obj_type = AB_GROUP_ROWSCOLUMNS;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_group_type_to_token(AB_GROUP_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
    case AB_GROUP_ROWS:
	token = AB_BIL_ROW;
	break;
    case AB_GROUP_COLUMNS: token = AB_BIL_COLUMN;
	break;
    case AB_GROUP_IGNORE: token = AB_BIL_IGNORE;
	break;
    case AB_GROUP_ROWSCOLUMNS: token = AB_BIL_ROWSCOLUMNS;
	break;
    }
    return token;
}

AB_ALIGNMENT
bilP_token_to_alignment(int type)
{
    AB_ALIGNMENT        obj_type = AB_ALIGN_UNDEF;

    switch (type)
    {
    case AB_BIL_TOP: obj_type = AB_ALIGN_TOP;
	break;
    case AB_BIL_RIGHT: obj_type = AB_ALIGN_RIGHT;
	break;
    case AB_BIL_BOTTOM: obj_type = AB_ALIGN_BOTTOM;
	break;
    case AB_BIL_LEFT: obj_type = AB_ALIGN_LEFT;
	break;
    case AB_BIL_CENTER: obj_type = AB_ALIGN_CENTER;
	break;
    case AB_BIL_VCENTER: obj_type = AB_ALIGN_VCENTER;
	break;
    case AB_BIL_HCENTER: obj_type = AB_ALIGN_HCENTER;
	break;
    case AB_BIL_LABELS: obj_type = AB_ALIGN_LABELS;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_alignment_to_token(AB_ALIGNMENT align)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (align)
    {
    case AB_ALIGN_TOP: token = AB_BIL_TOP;
	break;
    case AB_ALIGN_RIGHT: token = AB_BIL_RIGHT;
	break;
    case AB_ALIGN_BOTTOM: token = AB_BIL_BOTTOM;
	break;
    case AB_ALIGN_LEFT: token = AB_BIL_LEFT;
	break;
    case AB_ALIGN_CENTER: token = AB_BIL_CENTER;
	break;
    case AB_ALIGN_HCENTER: token = AB_BIL_HCENTER;
	break;
    case AB_ALIGN_VCENTER: token = AB_BIL_VCENTER;
	break;
    case AB_ALIGN_LABELS: token = AB_BIL_LABELS;
	break;
    default:
	break;
    }
    return token;
}

AB_OBJECT_TYPE
bilP_token_to_object_type(int type)
{
    AB_OBJECT_TYPE      obj_type = AB_TYPE_UNKNOWN;

    switch (type)
    {
    case AB_BIL_ACTION: obj_type = AB_TYPE_ACTION;
	break;
    case AB_BIL_BASE_WINDOW: obj_type = AB_TYPE_BASE_WINDOW;
	break;
    case AB_BIL_BUTTON: obj_type = AB_TYPE_BUTTON;
	break;
    case AB_BIL_CHOICE: obj_type = AB_TYPE_CHOICE;
	break;
    case AB_BIL_COMBO_BOX: obj_type = AB_TYPE_COMBO_BOX;
	break;
    case AB_BIL_FILE_CHOOSER: obj_type = AB_TYPE_FILE_CHOOSER;
	break;
    case AB_BIL_MESSAGE: obj_type = AB_TYPE_MESSAGE;
	break;
    case AB_BIL_CONTAINER: obj_type = AB_TYPE_CONTAINER;
	break;
    case AB_BIL_DIALOG: obj_type = AB_TYPE_DIALOG;
	break;
    case AB_BIL_DRAWING_AREA: obj_type = AB_TYPE_DRAWING_AREA;
	break;
    case AB_BIL_ITEM: obj_type = AB_TYPE_ITEM;
	break;
    case AB_BIL_LABEL: obj_type = AB_TYPE_LABEL;
	break;
    case AB_BIL_LAYERS: obj_type = AB_TYPE_LAYERS;
	break;
    case AB_BIL_LIST: obj_type = AB_TYPE_LIST;
	break;
    case AB_BIL_MENU: obj_type = AB_TYPE_MENU;
	break;
    case AB_BIL_MODULE: obj_type = AB_TYPE_MODULE;
	break;
    case AB_BIL_PROJECT: obj_type = AB_TYPE_PROJECT;
	break;
    case AB_BIL_SEPARATOR: obj_type = AB_TYPE_SEPARATOR;
	break;
    case AB_BIL_SCALE: obj_type = AB_TYPE_SCALE;
	break;
    case AB_BIL_SPIN_BOX: obj_type = AB_TYPE_SPIN_BOX;
	break;
    case AB_BIL_TERM_PANE: obj_type = AB_TYPE_TERM_PANE;
	break;
    case AB_BIL_TEXT_FIELD: obj_type = AB_TYPE_TEXT_FIELD;
	break;
    case AB_BIL_TEXT_PANE: obj_type = AB_TYPE_TEXT_PANE;
	break;
    }
    return obj_type;
}

BIL_TOKEN
bilP_object_type_to_token(AB_OBJECT_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (type)
    {
    case AB_TYPE_ACTION: token = AB_BIL_ACTION;
	break;
    case AB_TYPE_BASE_WINDOW: token = AB_BIL_BASE_WINDOW;
	break;
    case AB_TYPE_BUTTON: token = AB_BIL_BUTTON;
	break;
    case AB_TYPE_CHOICE: token = AB_BIL_CHOICE;
	break;
    case AB_TYPE_COMBO_BOX: token = AB_BIL_COMBO_BOX;
	break;
    case AB_TYPE_FILE_CHOOSER: token = AB_BIL_FILE_CHOOSER;
	break;
    case AB_TYPE_MESSAGE: token = AB_BIL_MESSAGE;
	break;
    case AB_TYPE_CONTAINER: token = AB_BIL_CONTAINER;
	break;
    case AB_TYPE_DIALOG: token = AB_BIL_DIALOG;
	break;
    case AB_TYPE_DRAWING_AREA: token = AB_BIL_DRAWING_AREA;
	break;
    case AB_TYPE_ITEM: token = AB_BIL_ITEM;
	break;
    case AB_TYPE_LABEL: token = AB_BIL_LABEL;
	break;
    case AB_TYPE_LAYERS: token = AB_BIL_LAYERS;
	break;
    case AB_TYPE_LIST: token = AB_BIL_LIST;
	break;
    case AB_TYPE_MENU: token = AB_BIL_MENU;
	break;
    case AB_TYPE_MODULE: token = AB_BIL_MODULE;
	break;
    case AB_TYPE_PROJECT: token = AB_BIL_PROJECT;
	break;
    case AB_TYPE_SEPARATOR: token = AB_BIL_SEPARATOR;
	break;
    case AB_TYPE_SCALE: token = AB_BIL_SCALE;
	break;
    case AB_TYPE_SPIN_BOX: token = AB_BIL_SPIN_BOX;
	break;
    case AB_TYPE_TERM_PANE: token = AB_BIL_TERM_PANE;
	break;
    case AB_TYPE_TEXT_FIELD: token = AB_BIL_TEXT_FIELD;
	break;
    case AB_TYPE_TEXT_PANE: token = AB_BIL_TEXT_PANE;
	break;
    }
    return token;
}

AB_WHEN
bilP_token_to_when(int type)
{
    AB_WHEN             when = AB_WHEN_UNDEF;

    switch (type)
    {
    case AB_BIL_ACTION1: when = AB_WHEN_ACTION1;
	break;
    case AB_BIL_ACTION2: when = AB_WHEN_ACTION2;
	break;
    case AB_BIL_ACTION3: when = AB_WHEN_ACTION3;
	break;
    case AB_BIL_ACTIVATED: when = AB_WHEN_ACTIVATED;
	break;
    case AB_BIL_AFTER_CREATED: when = AB_WHEN_AFTER_CREATED;
	break;
    case AB_BIL_BEFORE_POST_MENU: when = AB_WHEN_BEFORE_POST_MENU;
	break;
    case AB_BIL_BEFORE_TEXT_CHANGED: when = AB_WHEN_BEFORE_TEXT_CHANGED;
	break;
    case AB_BIL_CANCEL: when = AB_WHEN_CANCEL;
	break;
    case AB_BIL_DESTROYED: when = AB_WHEN_DESTROYED;
	break;
    case AB_BIL_DOUBLE_CLICKED_ON: when = AB_WHEN_DOUBLE_CLICKED_ON;
	break;
    case AB_BIL_DRAGGED: when = AB_WHEN_DRAGGED;
	break;
    case AB_BIL_DRAGGED_FROM: when = AB_WHEN_DRAGGED_FROM;
	break;
    case AB_BIL_DROPPED_ON: when = AB_WHEN_DROPPED_ON;
	break;
    case AB_BIL_HIDE: when = AB_WHEN_HIDDEN;
	break;
    case AB_BIL_ITEM_SELECTED: when = AB_WHEN_ITEM_SELECTED;
	break;
    case AB_BIL_OK: when = AB_WHEN_OK;
	break;
    case AB_BIL_POPPED_DOWN: when = AB_WHEN_POPPED_DOWN;
	break;
    case AB_BIL_POPPED_UP: when = AB_WHEN_POPPED_UP;
	break;
    case AB_BIL_REPAINT_NEEDED: when = AB_WHEN_REPAINT_NEEDED;
	break;
    case AB_BIL_RESIZED: when = AB_WHEN_AFTER_RESIZED;
	break;
    case AB_BIL_SESSION_RESTORE: when = AB_WHEN_SESSION_RESTORE;
	break;
    case AB_BIL_SESSION_SAVE: when = AB_WHEN_SESSION_SAVE;
	break;
    case AB_BIL_SHOW: when = AB_WHEN_SHOWN;
	break;
    case AB_BIL_TEXT_CHANGED: when = AB_WHEN_TEXT_CHANGED;
	break;
    case AB_BIL_TOGGLED: when = AB_WHEN_TOGGLED;
	break;
    case AB_BIL_TOOLTALK_DO_COMMAND: when = AB_WHEN_TOOLTALK_DO_COMMAND;
	break;
    case AB_BIL_TOOLTALK_GET_STATUS: when = AB_WHEN_TOOLTALK_GET_STATUS;
	break;
    case AB_BIL_TOOLTALK_PAUSE_RESUME: when = AB_WHEN_TOOLTALK_PAUSE_RESUME;
	break;
    case AB_BIL_TOOLTALK_QUIT: when = AB_WHEN_TOOLTALK_QUIT;
	break;
    case AB_BIL_VALUE_CHANGED: when = AB_WHEN_VALUE_CHANGED;
	break;
    }
    return when;
}

BIL_TOKEN
bilP_when_to_token(AB_WHEN ab_when)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    switch (ab_when)
    {
    case AB_WHEN_UNDEF: token = AB_BIL_UNDEF_KEYWORD;
	break;
    case AB_WHEN_ACTION1: token = AB_BIL_ACTION1;
	break;
    case AB_WHEN_ACTION2: token = AB_BIL_ACTION2;
	break;
    case AB_WHEN_ACTION3: token = AB_BIL_ACTION3;
	break;
    case AB_WHEN_ACTIVATED: token = AB_BIL_ACTIVATED;
	break;
    case AB_WHEN_AFTER_CREATED: token = AB_BIL_AFTER_CREATED;
	break;
    case AB_WHEN_AFTER_RESIZED: token = AB_BIL_RESIZED;
	break;
    case AB_WHEN_BEFORE_POST_MENU: token = AB_BIL_BEFORE_POST_MENU;
	break;
    case AB_WHEN_BEFORE_TEXT_CHANGED: token = AB_BIL_BEFORE_TEXT_CHANGED;
	break;
    case AB_WHEN_CANCEL: token = AB_BIL_CANCEL;
	break;
    case AB_WHEN_DESTROYED: token = AB_BIL_DESTROYED;
	break;
    case AB_WHEN_DOUBLE_CLICKED_ON: token = AB_BIL_DOUBLE_CLICKED_ON;
	break;
    case AB_WHEN_DRAGGED: token = AB_BIL_DRAGGED;
	break;
    case AB_WHEN_DRAGGED_FROM: token = AB_BIL_DRAGGED_FROM;
	break;
    case AB_WHEN_DROPPED_ON: token = AB_BIL_DROPPED_ON;
	break;
    case AB_WHEN_HIDDEN: token = AB_BIL_HIDE;
	break;
    case AB_WHEN_ITEM_SELECTED: token = AB_BIL_ITEM_SELECTED;
	break;
    case AB_WHEN_OK: token = AB_BIL_OK;
	break;
    case AB_WHEN_POPPED_DOWN: token = AB_BIL_POPPED_DOWN;
	break;
    case AB_WHEN_POPPED_UP: token = AB_BIL_POPPED_UP;
	break;
    case AB_WHEN_REPAINT_NEEDED: token = AB_BIL_REPAINT_NEEDED;
	break;
    case AB_WHEN_SESSION_RESTORE: token = AB_BIL_SESSION_RESTORE;
	break;
    case AB_WHEN_SESSION_SAVE: token = AB_BIL_SESSION_SAVE;
	break;
    case AB_WHEN_SHOWN: token = AB_BIL_SHOW;
	break;
    case AB_WHEN_TEXT_CHANGED: token = AB_BIL_TEXT_CHANGED;
	break;
    case AB_WHEN_TOGGLED: token = AB_BIL_TOGGLED;
	break;
    case AB_WHEN_TOOLTALK_DO_COMMAND: token = AB_BIL_TOOLTALK_DO_COMMAND;
   	break;
    case AB_WHEN_TOOLTALK_GET_STATUS: token = AB_BIL_TOOLTALK_GET_STATUS;
	break;
    case AB_WHEN_TOOLTALK_PAUSE_RESUME: token = AB_BIL_TOOLTALK_PAUSE_RESUME;
        break;
    case AB_WHEN_TOOLTALK_QUIT: token = AB_BIL_TOOLTALK_QUIT;
	break;
    case AB_WHEN_VALUE_CHANGED: token = AB_BIL_VALUE_CHANGED;
	break;
    }
    return token;
}

typedef struct
{
    BIL_TOKEN	token;
    ISTRING	string;
} BilTokenIndexEntryRec, *BilTokenIndexEntry;

static ISTRING          	bil_token_table[BIL_TOKEN_NUM_VALUES];
static BilTokenIndexEntryRec	bil_token_table_index[BIL_TOKEN_NUM_VALUES];
static BOOL             	bil_token_table_inited = FALSE;
#define bil_token_table_check_init() \
	(bil_token_table_inited? 0:bil_token_table_init())

/* 
 * Compares to entries (for qsort)
 */
static long
bil_token_entry_compare(const void *leftEntry, const void *rightEntry)
{
    return ((long)(((BilTokenIndexEntry)leftEntry)->string))
		- ((long)(((BilTokenIndexEntry)rightEntry)->string));
}


/*
 * Compares a key to an entry (for bsearch)
 */
static long
bil_token_key_compare(const void *voidIstr, const void *voidEntry)
{
    return ((long)((ISTRING)(voidIstr)))
		- ((long)(((BilTokenIndexEntry)voidEntry)->string));
}


static int
bil_token_table_init()
{
#define btt bil_token_table
#define off ((int)BIL_TOKEN_MIN_VALUE)	/* offset to 1st token */

    int                 i;
    int			rc = 0;		/* return code */
    BIL_TOKEN		curToken = AB_BIL_UNDEF;
    bil_token_table_inited = TRUE;
    for (i = 0; i < BIL_TOKEN_NUM_VALUES; ++i)
    {
	bil_token_table[i] = NULL;
    }

    /* AB_BIL_UNDEF should not be here (returns NULL) */
    btt[AB_BIL_UNDEF_KEYWORD - off] = istr_const(":undef");
    btt[AB_BIL_ABSOLUTE - off] = istr_const(":absolute");
    btt[AB_BIL_ACCELERATOR - off] = istr_const(":accelerator");
    btt[AB_BIL_ACCESS_HELP_VOLUME - off] = istr_const(":access-help-volume");
    btt[AB_BIL_ACTION - off] = istr_const(":action");
    btt[AB_BIL_ACTION1 - off] = istr_const(":action1");
    btt[AB_BIL_ACTION2 - off] = istr_const(":action2");
    btt[AB_BIL_ACTION3 - off] = istr_const(":action3");
    btt[AB_BIL_ACTION1_BUTTON - off] = istr_const(":action1-button");
    btt[AB_BIL_ACTION2_BUTTON - off] = istr_const(":action2-button");
    btt[AB_BIL_ACTION3_BUTTON - off] = istr_const(":action3-button");
    btt[AB_BIL_ACTION1_LABEL - off] = istr_const(":action1-label");
    btt[AB_BIL_ACTION2_LABEL - off] = istr_const(":action2-label");
    btt[AB_BIL_ACTION3_LABEL - off] = istr_const(":action3-label");
    btt[AB_BIL_ACTION_TYPE - off] = istr_const(":action-type");
    btt[AB_BIL_ACTIVATED - off] = istr_const(":activate");
    btt[AB_BIL_ACTIVE - off] = istr_const(":active");
    btt[AB_BIL_ADVANCED - off] = istr_const(":advanced");
    btt[AB_BIL_AFTER_CREATED - off] = istr_const(":after-create");
    btt[AB_BIL_ALPHANUMERIC - off] = istr_const(":alphanumeric");
    btt[AB_BIL_ALWAYS - off] = istr_const(":always");
    btt[AB_BIL_ANY - off] = istr_const(":any");
    btt[AB_BIL_APPLICATION- off] = istr_const(":application");
    btt[AB_BIL_ARG_TYPE - off] = istr_const(":arg-type");
    btt[AB_BIL_ARG_VALUE - off] = istr_const(":arg");
    btt[AB_BIL_ARROW_DOWN - off] = istr_const(":arrow-down");
    btt[AB_BIL_ARROW_LEFT - off] = istr_const(":arrow-left");
    btt[AB_BIL_ARROW_RIGHT - off] = istr_const(":arrow-right");
    btt[AB_BIL_ARROW_UP - off] = istr_const(":arrow-up");
    btt[AB_BIL_ARROW_STYLE - off] = istr_const(":arrow-style");
    btt[AB_BIL_ARROW_FLAT_BEGIN - off] = istr_const(":arrow-flat-begin");
    btt[AB_BIL_ARROW_FLAT_END - off] = istr_const(":arrow-flat-end");
    btt[AB_BIL_ARROW_BEGIN - off] = istr_const(":arrow-begin");
    btt[AB_BIL_ARROW_END - off] = istr_const(":arrow-end");
    btt[AB_BIL_ARROW_SPLIT - off] = istr_const(":arrow-split");
    btt[AB_BIL_ATTACH_ALIGN_OBJ_EDGE - off] = istr_const(":obj-edge");
    btt[AB_BIL_ATTACH_GRIDLINE - off] = istr_const(":grid-line");
    btt[AB_BIL_ATTACH_CENTER_GRIDLINE - off] = istr_const(":center-grid-line");
    btt[AB_BIL_ATTACH_OBJ - off] = istr_const(":obj");
    btt[AB_BIL_ATTACH_POINT - off] = istr_const(":point");
    btt[AB_BIL_AUTO_DISMISS - off] = istr_const(":auto-dismiss");
    btt[AB_BIL_BASE_WINDOW - off] = istr_const(":base-window");
    btt[AB_BIL_BASIC - off] = istr_const(":basic");
    btt[AB_BIL_BEFORE_POST_MENU - off] = 
				istr_const(":before-post-menu");
    btt[AB_BIL_BEFORE_TEXT_CHANGED - off] = 
				istr_const(":before-text-changed");
    btt[AB_BIL_BG_COLOR - off] = istr_const(":bg-color");
    btt[AB_BIL_BOLD - off] = istr_const(":bold");
    btt[AB_BIL_BOOLEAN - off] = istr_const(":boolean");
    btt[AB_BIL_BORDER_FRAME - off] = istr_const(":border-frame");
    btt[AB_BIL_BOTTOM - off] = istr_const(":bottom");
    btt[AB_BIL_BOTTOM_TO_TOP - off] = istr_const(":bottom-to-top");
    btt[AB_BIL_BROWSE - off] = istr_const(":browse");
    btt[AB_BIL_BROWSE_MULTIPLE - off] = istr_const(":browse-multiple");
    btt[AB_BIL_BUILTIN - off] = istr_const(":builtin");
    btt[AB_BIL_BUTTON - off] = istr_const(":button");
    btt[AB_BIL_BUTTON_PANEL - off] = istr_const(":button-panel");
    btt[AB_BIL_BUTTON_TYPE - off] = istr_const(":button-type");
    btt[AB_BIL_CALLBACK - off] = istr_const(":callback");
    btt[AB_BIL_CALL_FUNCTION - off] = istr_const(":call-function");
    btt[AB_BIL_CANCEL - off] = istr_const(":cancel");
    btt[AB_BIL_CANCEL_BUTTON - off] = istr_const(":cancel-button");
    btt[AB_BIL_CENTER - off] = istr_const(":center");
    btt[AB_BIL_CENTERS - off] = istr_const(":centers");
    btt[AB_BIL_CHILDREN - off] = istr_const(":children");
    btt[AB_BIL_CHOICE - off] = istr_const(":choice");
    btt[AB_BIL_CHOICE_TYPE - off] = istr_const(":choice-type");
    btt[AB_BIL_COLOR - off] = istr_const(":color");
    btt[AB_BIL_COLUMN - off] = istr_const(":column");
    btt[AB_BIL_COLUMN_ALIGNMENT - off] = istr_const(":column-alignment");
    btt[AB_BIL_CLASS_NAME - off] = istr_const(":class-name");
    btt[AB_BIL_COMPOSITE - off] = istr_const(":composite");
    btt[AB_BIL_COLOR_CHOOSER - off] = istr_const(":color-chooser");
    btt[AB_BIL_COMBO_BOX - off] = istr_const(":combobox");
    btt[AB_BIL_COMBO_BOX_EDITABLE - off] = istr_const(":editable");
    btt[AB_BIL_COMBO_BOX_STATIC - off] = istr_const(":static");
    btt[AB_BIL_COMBO_BOX_STYLE - off] = istr_const(":combobox-style");
    btt[AB_BIL_CONNECTION - off] = istr_const(":connection");
    btt[AB_BIL_CONNECTION_LIST - off] = istr_const(":connection-list");
    btt[AB_BIL_CONTAINER - off] = istr_const(":container");
    btt[AB_BIL_CONTAINER_TYPE - off] = istr_const(":container-type");
    btt[AB_BIL_COPY - off] = istr_const(":copy");
    btt[AB_BIL_DATA_END - off] = istr_const(":bil-data-end");
    btt[AB_BIL_DECIMAL_POINTS - off] = istr_const(":decimal-points");
    btt[AB_BIL_DEFAULT - off] = istr_const(":default");
    btt[AB_BIL_DEFAULT_BUTTON - off] = istr_const(":default-button");
    btt[AB_BIL_DEFINED_STRING - off] = istr_const(":defined-string");
    btt[AB_BIL_DESTROYED - off] = istr_const(":destroy");
    btt[AB_BIL_DIALOG - off] = istr_const(":dialog");
    btt[AB_BIL_DIRECTORY - off] = istr_const(":directory");
    btt[AB_BIL_DIRECTION - off] = istr_const(":direction");
    btt[AB_BIL_DISABLE - off] = istr_const(":disable");
    btt[AB_BIL_DOUBLE_CLICKED_ON - off] = istr_const(":double-click");
    btt[AB_BIL_DOUBLE_LINE - off] = istr_const(":double-line");
    btt[AB_BIL_DOUBLE_DASHED_LINE - off] = istr_const(":double-dashed-line");
    btt[AB_BIL_DRAG_CURSOR - off] = istr_const(":drag-cursor");
    btt[AB_BIL_DRAG_CURSOR_MASK - off] = istr_const(":drag-cursor-mask");
    btt[AB_BIL_DRAG_ENABLED - off] = istr_const(":drag-enabled");
    btt[AB_BIL_DRAG_OPS - off] = istr_const(":drag-ops");
    btt[AB_BIL_DRAG_TO_ROOT_ALLOWED - off] = istr_const(":drag-to-root-allowed");
    btt[AB_BIL_DRAG_TYPES - off] = istr_const(":drag-types");
    btt[AB_BIL_DRAGGED - off] = istr_const(":dragged");
    btt[AB_BIL_DRAGGED_FROM - off] = istr_const(":dragged-from");
    btt[AB_BIL_DRAWAREA_WIDTH - off] = istr_const(":drawarea-width");
    btt[AB_BIL_DRAWAREA_HEIGHT - off] = istr_const(":drawarea-height");
    btt[AB_BIL_DRAWING_AREA - off] = istr_const(":drawing-area");
    btt[AB_BIL_DRAWN_BUTTON - off] = istr_const(":drawn-button");
    btt[AB_BIL_DROP_ENABLED - off] = istr_const(":drop-enabled");
    btt[AB_BIL_DROP_SITE_CHILD_ALLOWED - off] = 
					istr_const(":drop-site-child-allowed");
    btt[AB_BIL_DROP_OPS - off] = istr_const(":drop-ops");
    btt[AB_BIL_DROP_TYPES - off] = istr_const(":drop-types");
    btt[AB_BIL_DROPPED_ON - off] = istr_const(":dropped-on");
    btt[AB_BIL_EAST - off] = istr_const(":east");
    btt[AB_BIL_EAST_ATTACHMENT - off] = istr_const(":east-attachment");
    btt[AB_BIL_ELEMENT - off] = istr_const(":element");
    btt[AB_BIL_ENABLE - off] = istr_const(":enable");
    btt[AB_BIL_EQUAL - off] = istr_const(":equal");
    btt[AB_BIL_ERROR - off] = istr_const(":error");
    btt[AB_BIL_ETCHED_IN - off] = istr_const(":etched-in");
    btt[AB_BIL_ETCHED_IN_DASH - off] = istr_const(":etched-in-dash");
    btt[AB_BIL_ETCHED_OUT - off] = istr_const(":etched-out");
    btt[AB_BIL_ETCHED_OUT_DASH - off] = istr_const(":etched-out-dash");
    btt[AB_BIL_EXCLUSIVE - off] = istr_const(":exclusive");
    btt[AB_BIL_EXECUTE_CODE - off] = istr_const(":execute-code");
    btt[AB_BIL_FALSE - off] = istr_const(":false");
    btt[AB_BIL_FG_COLOR - off] = istr_const(":fg-color");
    btt[AB_BIL_FILE_CHOOSER - off] = istr_const(":file-chooser");
    btt[AB_BIL_FILE_TYPE_MASK - off] = istr_const(":file-type-mask");
    btt[AB_BIL_FILENAME - off] = istr_const(":filename");
    btt[AB_BIL_FILES - off] = istr_const(":files");
    btt[AB_BIL_FILTER_PATTERN - off] = istr_const(":filter-pattern");
    btt[AB_BIL_FLOAT - off] = istr_const(":float");
    btt[AB_BIL_FONT - off] = istr_const(":font");
    btt[AB_BIL_FONT_CHOOSER - off] = istr_const(":font-chooser");
    btt[AB_BIL_FOOTER - off] = istr_const(":footer");
    btt[AB_BIL_FROM - off] = istr_const(":from");
    btt[AB_BIL_GAUGE - off] = istr_const(":gauge");
    btt[AB_BIL_GEOMETRY - off] = istr_const(":geometry");
    btt[AB_BIL_GLYPH - off] = istr_const(":glyph");
    btt[AB_BIL_GLYPH_FILE - off] = istr_const(":glyph-file");
    btt[AB_BIL_GRAPHIC - off] = istr_const(":graphic");
    btt[AB_BIL_GROUP - off] = istr_const(":group");
    btt[AB_BIL_GROUP_TYPE - off] = istr_const(":group-type");
    btt[AB_BIL_HATTACH_TYPE - off] = istr_const(":hattach_type");
    btt[AB_BIL_HAS_FOOTER - off] = istr_const(":has-footer");
    btt[AB_BIL_HCENTER - off] = istr_const(":hcenter");
    btt[AB_BIL_HELP_BUTTON - off] = istr_const(":help-button");
    btt[AB_BIL_HELP_LOCATION - off] = istr_const(":help-location");
    btt[AB_BIL_HELP_TEXT - off] = istr_const(":help-text");
    btt[AB_BIL_HELP_VOLUME - off] = istr_const(":help-volume");
    btt[AB_BIL_HEIGHT - off] = istr_const(":height");
    btt[AB_BIL_HEIGHT_MAX - off] = istr_const(":height-max");
    btt[AB_BIL_HEIGHT_RESIZABLE - off] = istr_const(":height-resizable");
    btt[AB_BIL_HIDE - off] = istr_const(":hide");
    btt[AB_BIL_HSCROLL - off] = istr_const(":hscroll");
    btt[AB_BIL_HSCROLLBAR - off] = istr_const(":hscrollbar");
    btt[AB_BIL_HSPACING - off] = istr_const(":hspacing");
    btt[AB_BIL_HOFFSET - off] = istr_const(":hoffset");
    btt[AB_BIL_HORIZONTAL - off] = istr_const(":horizontal");
    btt[AB_BIL_I18N - off] = istr_const(":internationalization");
    btt[AB_BIL_I18N_ENABLED - off] = istr_const(":enabled");
    btt[AB_BIL_ICON - off] = istr_const(":icon");
    btt[AB_BIL_ICONIC - off] = istr_const(":iconic");
    btt[AB_BIL_ICON_FILE - off] = istr_const(":icon-file");
    btt[AB_BIL_ICON_LABEL - off] = istr_const(":icon-label");
    btt[AB_BIL_ICON_MASK_FILE - off] = istr_const(":icon-mask-file");
    btt[AB_BIL_IGNORE - off] = istr_const(":ignore");
    btt[AB_BIL_INACTIVE - off] = istr_const(":inactive");
    btt[AB_BIL_INCREMENT - off] = istr_const(":increment");
    btt[AB_BIL_INFO - off] = istr_const(":info");
    btt[AB_BIL_INITIAL_STATE - off] = istr_const(":initial-state");
    btt[AB_BIL_INVISIBLE - off] = istr_const(":invisible");
    btt[AB_BIL_INT - off] = istr_const(":int");
    btt[AB_BIL_IS_HELP_ITEM - off] = istr_const(":is-help-item");
    btt[AB_BIL_ITEM - off] = istr_const(":item");
    btt[AB_BIL_ITEM_FOR_CHOICE - off] = istr_const(":item-for-choice");
    btt[AB_BIL_ITEM_FOR_COMBO_BOX - off] = istr_const(":item-for-combobox");
    btt[AB_BIL_ITEM_FOR_LIST - off] = istr_const(":item-for-list");
    btt[AB_BIL_ITEM_FOR_MENU - off] = istr_const(":item-for-menu");
    btt[AB_BIL_ITEM_FOR_MENUBAR - off] = istr_const(":item-for-menubar");
    btt[AB_BIL_ITEM_FOR_SPIN_BOX - off] = istr_const(":item-for-spinbox");
    btt[AB_BIL_ITEM_SELECTED - off] = istr_const(":item-selected");
    btt[AB_BIL_ITEM_TYPE - off] = istr_const(":item-type");
    btt[AB_BIL_LABEL - off] = istr_const(":label");
    btt[AB_BIL_LABEL_ALIGNMENT - off] = istr_const(":label-alignment");
    btt[AB_BIL_LABEL_POSITION - off] = istr_const(":label-position");
    btt[AB_BIL_LABELS - off] = istr_const(":labels");
    btt[AB_BIL_LABEL_STRING - off] = istr_const(":label-string");
    btt[AB_BIL_LABEL_STYLE - off] = istr_const(":label-style");
    btt[AB_BIL_LABEL_TYPE - off] = istr_const(":label-type");
    btt[AB_BIL_LAYERS - off] = istr_const(":layers");
    btt[AB_BIL_LEFT - off] = istr_const(":left");
    btt[AB_BIL_LEFT_TO_RIGHT - off] = istr_const(":left-to-right");
    btt[AB_BIL_LINE_STYLE - off] = istr_const(":line-style");
    btt[AB_BIL_LINK - off] = istr_const(":link");
    btt[AB_BIL_LIST - off] = istr_const(":list");
    btt[AB_BIL_LITERAL - off] = istr_const(":literal");
    btt[AB_BIL_LOAD - off] = istr_const(":load");
    btt[AB_BIL_MAIN_WINDOW - off] = istr_const(":main-window");
    btt[AB_BIL_MAX_LABEL - off] = istr_const(":max-label");
    btt[AB_BIL_MAX_VALUE - off] = istr_const(":max-value");
    btt[AB_BIL_MENU - off] = istr_const(":menu");
    btt[AB_BIL_MENU_BAR - off] = istr_const(":menu-bar");
    btt[AB_BIL_MENU_BUTTON - off] = istr_const(":menu-button");
    btt[AB_BIL_MENU_TITLE - off] = istr_const(":menu-title");
    btt[AB_BIL_MENU_TYPE - off] = istr_const(":menu-type");
    btt[AB_BIL_MESSAGE - off] = istr_const(":message");
    btt[AB_BIL_MESSAGE_TYPE - off] = istr_const(":message-type");
    btt[AB_BIL_MIN_LABEL - off] = istr_const(":min-label");
    btt[AB_BIL_MIN_VALUE - off] = istr_const(":min-value");
    btt[AB_BIL_MNEMONIC - off] = istr_const(":mnemonic");
    btt[AB_BIL_MODULE - off] = istr_const(":module");
    btt[AB_BIL_MOVE - off] = istr_const(":move");
    btt[AB_BIL_MULTIPLE - off] = istr_const(":multiple");
    btt[AB_BIL_MULTIPLE_SELECTIONS - off] = istr_const(":multiple-selections");
    btt[AB_BIL_NAME - off] = istr_const(":name");
    btt[AB_BIL_NEVER - off] = istr_const(":never");
    btt[AB_BIL_NIL - off] = istr_const(":nil");
    btt[AB_BIL_NONE - off] = istr_const(":none");
    btt[AB_BIL_NONEXCLUSIVE - off] = istr_const(":nonexclusive");
    btt[AB_BIL_NORTH - off] = istr_const(":north");
    btt[AB_BIL_NORTHEAST - off] = istr_const(":northeast");
    btt[AB_BIL_NORTHWEST - off] = istr_const(":northwest");
    btt[AB_BIL_NORTH_ATTACHMENT - off] = istr_const(":north-attachment");
    btt[AB_BIL_NOTSELECTED - off] = istr_const(":not-selected");
    btt[AB_BIL_NUM_COLUMNS - off] = istr_const(":num-columns");
    btt[AB_BIL_NUM_ROWS - off] = istr_const(":num-rows");
    btt[AB_BIL_NUMERIC - off] = istr_const(":numeric");
    btt[AB_BIL_OK - off] = istr_const(":ok");
    btt[AB_BIL_OK_BUTTON - off] = istr_const(":ok-button");
    btt[AB_BIL_OK_LABEL - off] = istr_const(":ok-label");
    btt[AB_BIL_ON_ITEM_HELP - off] = istr_const(":on-item-help");
    btt[AB_BIL_OPEN - off] = istr_const(":open");
    btt[AB_BIL_OPTION - off] = istr_const(":option");
    btt[AB_BIL_OPTION_MENU - off] = istr_const(":option-menu");
    btt[AB_BIL_ORIENTATION - off] = istr_const(":orientation");
    btt[AB_BIL_OTHER - off] = istr_const(":other");
    btt[AB_BIL_OTHER_STRING - off] = istr_const(":other-string");
    btt[AB_BIL_PACKING - off] = istr_const(":packing");
    btt[AB_BIL_PANED - off] = istr_const(":panedwindow");
    btt[AB_BIL_PANE_MAX_HEIGHT - off] = istr_const(":pane-max-height");
    btt[AB_BIL_PANE_MIN_HEIGHT - off] = istr_const(":pane-min-height");
    btt[AB_BIL_PARAMETER_TYPE - off] = istr_const(":parameter-type");
    btt[AB_BIL_PIXEL - off] = istr_const(":pixel");
    btt[AB_BIL_PIXMAP - off] = istr_const(":pixmap");
    btt[AB_BIL_POPPED_DOWN - off] = istr_const(":popdown");
    btt[AB_BIL_POPPED_UP - off] = istr_const(":popup");
    btt[AB_BIL_PROCESS_STRING - off] = istr_const(":process-string");
    btt[AB_BIL_PROJECT - off] = istr_const(":project");
    btt[AB_BIL_PULLDOWN - off] = istr_const(":pulldown");
    btt[AB_BIL_PUSH_BUTTON - off] = istr_const(":push-button");
    btt[AB_BIL_QUESTION - off] = istr_const(":question");
    btt[AB_BIL_READ_ONLY - off] = istr_const(":read-only");
    btt[AB_BIL_REFERENCE_POINT - off] = istr_const(":reference-point");
    btt[AB_BIL_REGULAR - off] = istr_const(":regular");
    btt[AB_BIL_RELATIVE - off] = istr_const(":relative");
    btt[AB_BIL_REPAINT_NEEDED - off] = istr_const(":repaint-needed");
    btt[AB_BIL_RES_FILE_TYPES - off] = istr_const(":res-file-types");
    btt[AB_BIL_RESIZABLE - off] = istr_const(":resizable");
    btt[AB_BIL_RESIZED - off] = istr_const(":resize");
    btt[AB_BIL_RESOURCE - off] = istr_const(":resource");
    btt[AB_BIL_RETURN_TYPE - off] = istr_const(":return-type");
    btt[AB_BIL_RIGHT - off] = istr_const(":right");
    btt[AB_BIL_RIGHT_TO_LEFT - off] = istr_const(":right-to-left");
    btt[AB_BIL_ROOT_WINDOW - off] = istr_const(":root-window");
    btt[AB_BIL_ROW - off] = istr_const(":row");
    btt[AB_BIL_ROWSCOLUMNS - off] = istr_const(":rowscolumns");
    btt[AB_BIL_ROW_ALIGNMENT - off] = istr_const(":row-alignment");
    btt[AB_BIL_ROW_COLUMN - off] = istr_const(":row-column");
    btt[AB_BIL_SAVE - off] = istr_const(":save");
    btt[AB_BIL_SAVE_AS - off] = istr_const(":save-as");
    btt[AB_BIL_SB_ALWAYS_VISIBLE - off] = istr_const(":sb-always-visible");
    btt[AB_BIL_SCALE - off] = istr_const(":scale");
    btt[AB_BIL_SCROLLED_WINDOW - off] = istr_const(":scrolled-window");
    btt[AB_BIL_SELECT - off] = istr_const(":select");
    btt[AB_BIL_MULTIPLE - off] = istr_const(":multiple");
    btt[AB_BIL_SELECTED - off] = istr_const(":selected");
    btt[AB_BIL_SELECTION_BOX - off] = istr_const(":selection-box");
    btt[AB_BIL_SELECTION_MODE - off] = istr_const(":selection-mode");
    btt[AB_BIL_SELECTION_REQUIRED - off] = istr_const(":selection-required");
    btt[AB_BIL_SEPARATOR - off] = istr_const(":separator");
    btt[AB_BIL_SESSION_MGMT - off] = istr_const(":session-management");
    btt[AB_BIL_SESSION_RESTORE - off] = istr_const(":session-restore");
    btt[AB_BIL_SESSION_SAVE - off] = istr_const(":session-save");
    btt[AB_BIL_SESSIONING_METHOD - off] = istr_const(":sessioning-method");
    btt[AB_BIL_SESSIONING_CMDLINE - off] = istr_const(":cmdline");
    btt[AB_BIL_SESSIONING_CMDLINE_AND_FILE - off] = istr_const(":cmdline-file");
    btt[AB_BIL_SESSIONING_FILE - off] = istr_const(":file");
    btt[AB_BIL_SET_LABEL - off] = istr_const(":set-label");
    btt[AB_BIL_SET_TEXT - off] = istr_const(":set-text");
    btt[AB_BIL_SET_VALUE - off] = istr_const(":set-value");
    btt[AB_BIL_SHADOW_IN - off] = istr_const(":shadow-in");
    btt[AB_BIL_SHADOW_OUT - off] = istr_const(":shadow-out");
    btt[AB_BIL_SHOW - off] = istr_const(":show");
    btt[AB_BIL_SHOW_VALUE - off] = istr_const(":show-value");
    btt[AB_BIL_SINGLE - off] = istr_const(":single");
    btt[AB_BIL_SINGLE_LINE - off] = istr_const(":single-line");
    btt[AB_BIL_SINGLE_DASHED_LINE - off] = istr_const(":single-dashed-line");
    btt[AB_BIL_SLIDER - off] = istr_const(":slider");
    btt[AB_BIL_SLIDER_TYPE - off] = istr_const(":slider-type");
    btt[AB_BIL_SOUTHEAST - off] = istr_const(":southeast");
    btt[AB_BIL_SOUTH - off] = istr_const(":south");
    btt[AB_BIL_SOUTHWEST - off] = istr_const(":southwest");
    btt[AB_BIL_SOUTH_ATTACHMENT - off] = istr_const(":south-attachment");
    btt[AB_BIL_SPIN_BOX - off] = istr_const(":spinbox");
    btt[AB_BIL_STANDARD - off] = istr_const(":standard");
    btt[AB_BIL_STRING - off] = istr_const(":string");
    btt[AB_BIL_MAX_LENGTH - off] = istr_const(":max-length");
    btt[AB_BIL_STYLE_BOLD - off] = istr_const(":bold");
    btt[AB_BIL_STYLE_NORMAL - off] = istr_const(":normal");
    btt[AB_BIL_STYLE_3D - off] = istr_const(":3D");
    btt[AB_BIL_TEAR_OFF - off] = istr_const(":tear-off");
    btt[AB_BIL_TERM_PANE - off] = istr_const(":term-pane");
    btt[AB_BIL_TEXT - off] = istr_const(":text");
    btt[AB_BIL_TEXT_CHANGED - off] = istr_const(":text-changed");
    btt[AB_BIL_TEXT_FIELD - off] = istr_const(":text-field");
    btt[AB_BIL_TEXT_PANE - off] = istr_const(":text-pane");
    btt[AB_BIL_TEXT_TYPE - off] = istr_const(":text-type");
    btt[AB_BIL_TIGHT - off] = istr_const(":tight");
    btt[AB_BIL_TITLE - off] = istr_const(":title");
    btt[AB_BIL_TITLE_TYPE - off] = istr_const(":title-type");
    btt[AB_BIL_TO - off] = istr_const(":to");
    btt[AB_BIL_TOGGLED - off] = istr_const(":toggle");
    btt[AB_BIL_TOOL_BAR - off] = istr_const(":tool-bar");
    btt[AB_BIL_TOOLTALK - off] = istr_const(":tooltalk");
    btt[AB_BIL_TOOLTALK_DESKTOP_LEVEL - off] = istr_const(":desktop-level");
    btt[AB_BIL_TOOLTALK_DO_COMMAND - off] = istr_const(":tooltalk-do-command");
    btt[AB_BIL_TOOLTALK_GET_STATUS - off] = istr_const(":tooltalk-get-status");
    btt[AB_BIL_TOOLTALK_PAUSE_RESUME - off] = istr_const(":tooltalk-pause-resume");
    btt[AB_BIL_TOOLTALK_QUIT - off] = istr_const(":tooltalk-quit");
    btt[AB_BIL_TOP - off] = istr_const(":top");
    btt[AB_BIL_TOP_TO_BOTTOM - off] = istr_const(":top-to-bottom");
    btt[AB_BIL_TRUE - off] = istr_const(":true");
    btt[AB_BIL_TYPE - off] = istr_const(":type");
    btt[AB_BIL_USER_DATA - off] = istr_const(":user-data");
    btt[AB_BIL_USER_DEF - off] = istr_const(":user-def");
    btt[AB_BIL_INITIAL_VALUE - off] = istr_const(":initial-value");
    btt[AB_BIL_VALUE - off] = istr_const(":value");
    btt[AB_BIL_VALUE_CHANGED - off] = istr_const(":value-changed");
    btt[AB_BIL_VALUE_FLOAT - off] = istr_const(":value-float");
    btt[AB_BIL_VALUE_IDENT - off] = istr_const(":value-ident");
    btt[AB_BIL_VALUE_INT - off] = istr_const(":value-int");
    btt[AB_BIL_VALUE_STRING - off] = istr_const(":value-string");
    btt[AB_BIL_VATTACH_TYPE - off] = istr_const(":vattach_type");
    btt[AB_BIL_VCENTER - off] = istr_const(":vcenter");
    btt[AB_BIL_VENDOR - off] = istr_const(":vendor");
    btt[AB_BIL_VERSION - off] = istr_const(":version");
    btt[AB_BIL_VERTICAL - off] = istr_const(":vertical");
    btt[AB_BIL_VISIBLE - off] = istr_const(":visible");
    btt[AB_BIL_VOFFSET - off] = istr_const(":voffset");
    btt[AB_BIL_VOID - off] = istr_const(":void");
    btt[AB_BIL_VSCROLL - off] = istr_const(":vscroll");
    btt[AB_BIL_VSCROLLBAR - off] = istr_const(":vscrollbar");
    btt[AB_BIL_VSPACING - off] = istr_const(":vspacing");
    btt[AB_BIL_WARNING - off] = istr_const(":warning");
    btt[AB_BIL_WEST - off] = istr_const(":west");
    btt[AB_BIL_WEST_ATTACHMENT - off] = istr_const(":west-attachment");
    btt[AB_BIL_WHEN - off] = istr_const(":when");
    btt[AB_BIL_WHEN_NEEDED - off] = istr_const(":when-needed");
    btt[AB_BIL_WIDGET - off] = istr_const(":widget");
    btt[AB_BIL_WIDTH - off] = istr_const(":width");
    btt[AB_BIL_WIDTH_MAX - off] = istr_const(":width-max");
    btt[AB_BIL_WIDTH_RESIZABLE - off] = istr_const(":width-resizable");
    btt[AB_BIL_WIN_CHILDREN - off] = istr_const(":win-children");
    btt[AB_BIL_WIN_PARENT - off] = istr_const(":win-parent");
    btt[AB_BIL_WORD_WRAP - off] = istr_const(":word-wrap");
    btt[AB_BIL_WORKING - off] = istr_const(":working");
    btt[AB_BIL_X - off] = istr_const(":x");
    btt[AB_BIL_XMSTRING - off] = istr_const(":xmstring");
    btt[AB_BIL_XMSTRING_TBL - off] = istr_const(":xmstring-table");
    btt[AB_BIL_Y - off] = istr_const(":y");

    /*
     * Build b-searchable index from ISTRING back to enum
     */
    memset(bil_token_table_index, 0, sizeof(bil_token_table_index));
    for (i = 0; i < BIL_TOKEN_NUM_VALUES; ++i)
    {
	bil_token_table_index[i].string = btt[i];
	bil_token_table_index[i].token = (BIL_TOKEN)(i + off);
    }
    qsort(bil_token_table_index, 
		BIL_TOKEN_NUM_VALUES, 
		sizeof(bil_token_table_index[0]),
		bil_token_entry_compare);

    return 0;
#undef off
#undef btt
}


STRING
bilP_token_to_string(BIL_TOKEN token)
{
    bil_token_table_check_init();
    if ((token < BIL_TOKEN_MIN_VALUE) || (token > BIL_TOKEN_MAX_VALUE))
    {
	return NULL;
    }
    return istr_string(bil_token_table[((int) token) - BIL_TOKEN_MIN_VALUE]);

}

BIL_TOKEN
bilP_string_to_token(STRING strToken)
{
    ISTRING             istrToken = istr_create(strToken);
    register long	midDiff;
    register long	midIndex; 
    long		minIndex, maxIndex;
    bil_token_table_check_init();

    /*
     * Perform binary search for ISTRING value
     */
    minIndex = 0;
    maxIndex = BIL_TOKEN_NUM_VALUES-1;
    while (minIndex < maxIndex)
    {
	midIndex = ((minIndex + maxIndex)>>1);
	midDiff = (((long)bil_token_table_index[midIndex].string)
			- ((long)istrToken));
	if (midDiff < 0)
	{
	    /* mid is too small - take upper half */
	    minIndex = midIndex + 1;
	}
	else if (midDiff > 0)
	{
	    /* mid is too large - take bottom half */
	    maxIndex = midIndex - 1;
	}
	else
	{
	    return bil_token_table_index[midIndex].token;
	}
    }
    /* After exiting loop, min=max */
    return (istr_equal(istrToken, bil_token_table_index[minIndex].string)?
		bil_token_table_index[minIndex].token
	    : 
		AB_BIL_UNDEF);
}

STRING
bilP_compass_point_to_string(AB_COMPASS_POINT compass_point)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_compass_point_to_token(compass_point))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_text_type_to_string(AB_TEXT_TYPE text_type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_text_type_to_token(text_type))
    {
	string = bilP_token_to_string(token);
    }
    return string;
}

STRING
bilP_container_type_to_string(AB_CONTAINER_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_container_type_to_token(type))
    {
	string = bilP_token_to_string(token);
    }
    return string;
}

STRING
bilP_packing_to_string(AB_PACKING pack)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_packing_to_token(pack))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_alignment_to_string(AB_ALIGNMENT align)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_alignment_to_token(align))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

/*
STRING
bilP_menu_type_to_string(AB_MENU_TYPE menu_type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_menu_type_to_token(menu_type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}
*/

STRING
bilP_label_type_to_string(AB_LABEL_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_label_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_group_type_to_string(AB_GROUP_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_group_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_button_type_to_string(AB_BUTTON_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_button_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_orientation_to_string(AB_ORIENTATION type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_orientation_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_direction_to_string(AB_DIRECTION type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_direction_to_token(type))
    {
        string = (bilP_token_to_string(token));
    }   
    return string;
}

STRING
bilP_item_type_to_string(AB_ITEM_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_item_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_choice_type_to_string(AB_CHOICE_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_choice_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_label_style_to_string(AB_LABEL_STYLE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_label_style_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_func_type_to_string(AB_FUNC_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_func_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_builtin_action_to_string(AB_BUILTIN_ACTION type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_builtin_action_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_when_to_string(AB_WHEN type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_when_to_token(type))
    {
	string = bilP_token_to_string(token);
    }
    return string;
}

STRING
bilP_arg_class_to_string(AB_ARG_CLASS argClass)
{
    BIL_TOKEN           token;
    STRING              string = NULL;
    if (token = bilP_arg_class_to_token(argClass))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_arg_type_to_string(AB_ARG_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_arg_type_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

STRING
bilP_attachment_to_string(AB_ATTACH_TYPE type)
{
    BIL_TOKEN           token;
    STRING              string = NULL;

    if (token = bilP_attachment_to_token(type))
    {
	string = (bilP_token_to_string(token));
    }
    return string;
}

BIL_TOKEN
bilP_initial_state_to_token(AB_OBJECT_STATE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_STATE_ACTIVE: token = AB_BIL_ACTIVE;
	break;
    case AB_STATE_ICONIC: token = AB_BIL_ICONIC;
	break;
    case AB_STATE_INACTIVE: token = AB_BIL_INACTIVE;
	break;
    case AB_STATE_INVISIBLE: token = AB_BIL_INVISIBLE;
	break;
    case AB_STATE_NOTSELECTED: token = AB_BIL_NOTSELECTED;
	break;
    case AB_STATE_OPEN: token = AB_BIL_OPEN;
	break;
    case AB_STATE_SELECTED: token = AB_BIL_SELECTED;
	break;
    case AB_STATE_VISIBLE: token = AB_BIL_VISIBLE;
	break;
    }
    return (token);
}

AB_OBJECT_STATE
bilP_token_to_initial_state(int token)
{
    AB_OBJECT_STATE     obj_type = AB_STATE_UNDEF;

    switch (token)
    {
    case AB_BIL_ACTIVE: obj_type = AB_STATE_ACTIVE;
	break;
    case AB_BIL_ICONIC: obj_type = AB_STATE_ICONIC;
	break;
    case AB_BIL_INACTIVE: obj_type = AB_STATE_INACTIVE;
	break;
    case AB_BIL_INVISIBLE: obj_type = AB_STATE_INVISIBLE;
	break;
    case AB_BIL_NOTSELECTED: obj_type = AB_STATE_INVISIBLE;
	break;
    case AB_BIL_OPEN: obj_type = AB_STATE_OPEN;
	break;
    case AB_BIL_SELECTED: obj_type = AB_STATE_SELECTED;
	break;
    case AB_BIL_VISIBLE: obj_type = AB_STATE_VISIBLE;
	break;
    }
    return (obj_type);
}

STRING
bilP_initial_state_to_string(AB_OBJECT_STATE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_initial_state_to_token(type))
	string = bilP_token_to_string(token);
    return (string);
}

BIL_TOKEN
bilP_border_frame_to_token(AB_LINE_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (type)
    {
    case AB_LINE_NONE: token = AB_BIL_NONE;
	break;
    case AB_LINE_SHADOW_IN: token = AB_BIL_SHADOW_IN;
	break;
    case AB_LINE_SHADOW_OUT: token = AB_BIL_SHADOW_OUT;
	break;
    case AB_LINE_ETCHED_IN: token = AB_BIL_ETCHED_IN;
	break;
    case AB_LINE_ETCHED_OUT: token = AB_BIL_ETCHED_OUT;
	break;
    }
    return (token);
}

AB_LINE_TYPE
bilP_token_to_border_frame(int token)
{
    AB_LINE_TYPE      type = AB_LINE_UNDEF;

    switch (token)
    {
    case AB_BIL_NONE: type = AB_LINE_NONE;
	break;
    case AB_BIL_SHADOW_IN: type = AB_LINE_SHADOW_IN;
	break;
    case AB_BIL_SHADOW_OUT: type = AB_LINE_SHADOW_OUT;
	break;
    case AB_BIL_ETCHED_IN: type = AB_LINE_ETCHED_IN;
	break;
    case AB_BIL_ETCHED_OUT: type = AB_LINE_ETCHED_OUT;
	break;
    }
    return (type);
}

STRING
bilP_border_frame_to_string(AB_LINE_TYPE type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_border_frame_to_token(type))
	string = bilP_token_to_string(token);
    return (string);
}


BIL_TOKEN
bilP_line_style_to_token(AB_LINE_TYPE line_style)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (line_style)
    {
    case AB_LINE_NONE: token = AB_BIL_NONE;
	break;
    case AB_LINE_ETCHED_IN: token = AB_BIL_ETCHED_IN;
	break;
    case AB_LINE_ETCHED_OUT: token = AB_BIL_ETCHED_OUT;
	break;
    case AB_LINE_ETCHED_IN_DASH: token = AB_BIL_ETCHED_IN_DASH;
	break;
    case AB_LINE_ETCHED_OUT_DASH: token = AB_BIL_ETCHED_OUT_DASH;
	break;
    case AB_LINE_SINGLE_LINE: token = AB_BIL_SINGLE_LINE;
	break;
    case AB_LINE_DOUBLE_LINE: token = AB_BIL_DOUBLE_LINE;
	break;
    case AB_LINE_SINGLE_DASHED_LINE: token = AB_BIL_SINGLE_DASHED_LINE;
	break;
    case AB_LINE_DOUBLE_DASHED_LINE: token = AB_BIL_DOUBLE_DASHED_LINE;
	break;
    }
    return (token);
}

AB_LINE_TYPE
bilP_token_to_line_style(int token)
{
    AB_LINE_TYPE      line_style = AB_LINE_UNDEF;

    switch (token)
    {
    case AB_BIL_NONE: line_style = AB_LINE_NONE;
	break;
    case AB_BIL_ETCHED_IN: line_style = AB_LINE_ETCHED_IN;
	break;
    case AB_BIL_ETCHED_OUT: line_style = AB_LINE_ETCHED_OUT;
	break;
    case AB_BIL_ETCHED_IN_DASH: line_style = AB_LINE_ETCHED_IN_DASH;
	break;
    case AB_BIL_ETCHED_OUT_DASH: line_style = AB_LINE_ETCHED_OUT_DASH;
	break;
    case AB_BIL_SINGLE_LINE: line_style = AB_LINE_SINGLE_LINE;
	break;
    case AB_BIL_DOUBLE_LINE: line_style = AB_LINE_DOUBLE_LINE;
	break;
    case AB_BIL_SINGLE_DASHED_LINE: line_style = AB_LINE_SINGLE_DASHED_LINE;
	break;
    case AB_BIL_DOUBLE_DASHED_LINE: line_style = AB_LINE_DOUBLE_DASHED_LINE;
	break;
    }
    return (line_style);
}

STRING
bilP_line_style_to_string(AB_LINE_TYPE line_style)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_line_style_to_token(line_style))
	string = bilP_token_to_string(token);
    return (string);
}

BIL_TOKEN
bilP_arrow_style_to_token(AB_ARROW_STYLE arrow_style)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (arrow_style)
    {
    case AB_ARROW_FLAT_BEGIN: token = AB_BIL_ARROW_FLAT_BEGIN;
	break;
    case AB_ARROW_FLAT_END: token = AB_BIL_ARROW_FLAT_END;
	break;
    case AB_ARROW_BEGIN: token = AB_BIL_ARROW_BEGIN;
	break;
    case AB_ARROW_END: token = AB_BIL_ARROW_END;
	break;
    case AB_ARROW_SPLIT: token = AB_BIL_ARROW_SPLIT;
	break;
    }
    return (token);
}

AB_ARROW_STYLE
bilP_token_to_arrow_style(int token)
{
    AB_ARROW_STYLE      arrow_style = AB_ARROW_UNDEF;

    switch (token)
    {
    case AB_BIL_ARROW_FLAT_BEGIN: arrow_style = AB_ARROW_FLAT_BEGIN;
	break;
    case AB_BIL_ARROW_FLAT_END: arrow_style = AB_ARROW_FLAT_END;
	break;
    case AB_BIL_ARROW_BEGIN: arrow_style = AB_ARROW_BEGIN;
	break;
    case AB_BIL_ARROW_END: arrow_style = AB_ARROW_END;
	break;
    case AB_BIL_ARROW_SPLIT: arrow_style = AB_ARROW_SPLIT;
	break;
    }
    return (arrow_style);
}

STRING
bilP_arrow_style_to_string(AB_ARROW_STYLE arrow_style)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_arrow_style_to_token(arrow_style))
	string = bilP_token_to_string(token);
    return (string);
}

BIL_TOKEN
bilP_selection_mode_to_token(AB_SELECT_TYPE selection_mode)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (selection_mode)
    {
    case AB_SELECT_SINGLE: token = AB_BIL_SINGLE;
        break;
    case AB_SELECT_BROWSE: token = AB_BIL_BROWSE;
        break;
    case AB_SELECT_MULTIPLE: token = AB_BIL_MULTIPLE;
        break;
    case AB_SELECT_BROWSE_MULTIPLE: token = AB_BIL_BROWSE_MULTIPLE;
        break;
    }
    return (token);
}

AB_SELECT_TYPE
bilP_token_to_selection_mode(int token)
{
    AB_SELECT_TYPE	selection_mode = AB_SELECT_UNDEF;
 
    switch (token)
    {
    case AB_BIL_SINGLE: selection_mode = AB_SELECT_SINGLE;
        break;
    case AB_BIL_BROWSE: selection_mode = AB_SELECT_BROWSE;
        break;
    case AB_BIL_MULTIPLE: selection_mode = AB_SELECT_MULTIPLE;
        break;
    case AB_BIL_BROWSE_MULTIPLE: selection_mode = AB_SELECT_BROWSE_MULTIPLE;
        break;
    }
    return (selection_mode);
}

STRING
bilP_selection_mode_to_string(AB_SELECT_TYPE selection_mode)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_selection_mode_to_token(selection_mode))
        string = bilP_token_to_string(token);
    return (string);
}

BIL_TOKEN
bilP_scrollbar_policy_to_token(AB_SCROLLBAR_POLICY sb_policy)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (sb_policy)
    {
 	case AB_SCROLLBAR_ALWAYS: token = AB_BIL_ALWAYS;
            break;
	case AB_SCROLLBAR_NEVER: token = AB_BIL_NEVER;
            break;
	 case AB_SCROLLBAR_WHEN_NEEDED: token = AB_BIL_WHEN_NEEDED;
            break;
    }
    return (token);
}

AB_SCROLLBAR_POLICY
bilP_token_to_scrollbar_policy(BIL_TOKEN token)
{
    AB_SCROLLBAR_POLICY      sb_policy = AB_SCROLLBAR_UNDEF;
 
    switch (token)
    { 
        case AB_BIL_ALWAYS: sb_policy = AB_SCROLLBAR_ALWAYS; 
            break;
        case AB_BIL_NEVER: sb_policy = AB_SCROLLBAR_NEVER;  
            break; 
         case AB_BIL_WHEN_NEEDED: sb_policy = AB_SCROLLBAR_WHEN_NEEDED; 
            break; 
    } 
    return (sb_policy);
}

STRING
bilP_scrollbar_policy_to_string(AB_SCROLLBAR_POLICY sb_policy)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_scrollbar_policy_to_token(sb_policy))
        string = bilP_token_to_string(token);
    return (string);
}


BIL_TOKEN
bilP_msg_type_to_token(AB_MESSAGE_TYPE msg_type)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (msg_type)
    {
 	case AB_MSG_ERROR: token = AB_BIL_ERROR;
            break;
	case AB_MSG_INFORMATION: token = AB_BIL_INFO;
            break;
	 case AB_MSG_QUESTION: token = AB_BIL_QUESTION;
            break;
	 case AB_MSG_WARNING: token = AB_BIL_WARNING;
            break;
	 case AB_MSG_WORKING: token = AB_BIL_WORKING;
            break;
	default:
	    break;
    }
    return (token);
}

AB_MESSAGE_TYPE
bilP_token_to_msg_type(BIL_TOKEN token)
{
    AB_MESSAGE_TYPE      msg_type = AB_MSG_UNDEF;
 
    switch (token)
    { 
        case AB_BIL_ERROR: msg_type = AB_MSG_ERROR; 
            break;
        case AB_BIL_INFO: msg_type = AB_MSG_INFORMATION;  
            break; 
         case AB_BIL_QUESTION: msg_type = AB_MSG_QUESTION; 
            break; 
         case AB_BIL_WARNING: msg_type = AB_MSG_WARNING; 
            break; 
         case AB_BIL_WORKING: msg_type = AB_MSG_WORKING; 
            break; 
	default:
	    break;
    } 
    return (msg_type);
}

STRING
bilP_msg_type_to_string(AB_MESSAGE_TYPE msg_type) 
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_msg_type_to_token(msg_type))
        string = bilP_token_to_string(token);
    return (string);
}


BIL_TOKEN
bilP_file_type_mask_to_token(AB_FILE_TYPE_MASK ftm)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (ftm)
    {
 	case AB_FILE_REGULAR: token = AB_BIL_REGULAR;
            break;
	case AB_FILE_DIRECTORY: token = AB_BIL_DIRECTORY;
            break;
	 case AB_FILE_ANY: token = AB_BIL_ANY;
            break;
	default:
	    break;
    }
    return (token);
}

AB_FILE_TYPE_MASK
bilP_token_to_file_type_mask(BIL_TOKEN token)
{
    AB_FILE_TYPE_MASK      ftm = AB_FILE_TYPE_MASK_UNDEF;
 
    switch (token)
    { 
        case AB_BIL_REGULAR: ftm = AB_FILE_REGULAR; 
            break;
        case AB_BIL_DIRECTORY: ftm = AB_FILE_DIRECTORY;  
            break; 
         case AB_BIL_ANY: ftm = AB_FILE_ANY; 
            break; 
	default:
	    break;
    } 
    return (ftm);
}

STRING
bilP_file_type_mask_to_string(AB_FILE_TYPE_MASK ftm) 
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_file_type_mask_to_token(ftm))
        string = bilP_token_to_string(token);
    return (string);
}

BIL_TOKEN
bilP_sessioning_method_to_token(AB_SESSIONING_METHOD sessioning_method)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (sessioning_method)
    {
 	case AB_SESSIONING_NONE: token = AB_BIL_NONE;
            break;
	case AB_SESSIONING_CMDLINE: token = AB_BIL_SESSIONING_CMDLINE;
            break;
	 case AB_SESSIONING_FILE: token = AB_BIL_SESSIONING_FILE;
            break;
	 case AB_SESSIONING_CMDLINE_AND_FILE: token = AB_BIL_SESSIONING_CMDLINE_AND_FILE;
            break;
	default:
	    break;
    }
    return (token);
}

AB_SESSIONING_METHOD
bilP_token_to_sessioning_method(BIL_TOKEN token)
{
    AB_SESSIONING_METHOD      sessioning_method = AB_SESSIONING_UNDEF;
 
    switch (token)
    { 
        case AB_BIL_NONE: sessioning_method = AB_SESSIONING_NONE; 
            break;
        case AB_BIL_SESSIONING_CMDLINE: sessioning_method = AB_SESSIONING_CMDLINE;  
            break; 
         case AB_BIL_SESSIONING_FILE: sessioning_method = AB_SESSIONING_FILE; 
            break; 
         case AB_BIL_SESSIONING_CMDLINE_AND_FILE: sessioning_method = AB_SESSIONING_CMDLINE_AND_FILE; 
            break; 
	default:
	    break;
    } 
    return (sessioning_method);
}

STRING
bilP_sessioning_method_to_string(AB_SESSIONING_METHOD sessioning_method) 
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;

    if (token = bilP_sessioning_method_to_token(sessioning_method))
        string = bilP_token_to_string(token);
    return (string);
}

BIL_TOKEN
bilP_tt_desktop_level_to_token(AB_TOOLTALK_LEVEL tt_level)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (tt_level)
    {
        case AB_TOOLTALK_NONE: token = AB_BIL_NONE;
            break;
        case AB_TOOLTALK_DESKTOP_BASIC: token = AB_BIL_BASIC;
            break;
        case AB_TOOLTALK_DESKTOP_ADVANCED: token = AB_BIL_ADVANCED;
            break;
        default:
            break;
    }
    return (token);
}

AB_TOOLTALK_LEVEL
bilP_token_to_tt_desktop_level(BIL_TOKEN token)
{
    AB_TOOLTALK_LEVEL	tt_level= AB_TOOLTALK_UNDEF;
 
    switch (token)
    {
        case AB_BIL_NONE: tt_level= AB_TOOLTALK_NONE;
            break;
        case AB_BIL_ADVANCED: tt_level = AB_TOOLTALK_DESKTOP_ADVANCED;
            break;
        case AB_BIL_BASIC: tt_level = AB_TOOLTALK_DESKTOP_BASIC;
            break;
        default:
            break;
    }
    return (tt_level);
}

STRING
bilP_tt_desktop_level_to_string(AB_TOOLTALK_LEVEL tt_level)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;
 
    if (token = bilP_tt_desktop_level_to_token(tt_level))
        string = bilP_token_to_string(token);
    return (string);
}

bilP_default_button_to_token(AB_DEFAULT_BUTTON button)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;

    switch (button)
    {
        case AB_DEFAULT_BTN_ACTION1: token = AB_BIL_ACTION1_BUTTON;
            break;
        case AB_DEFAULT_BTN_ACTION2: token = AB_BIL_ACTION2_BUTTON;
            break;
        case AB_DEFAULT_BTN_ACTION3: token = AB_BIL_ACTION3_BUTTON;
            break;
        case AB_DEFAULT_BTN_CANCEL: token = AB_BIL_CANCEL_BUTTON;
            break;
        case AB_DEFAULT_BTN_NONE: token = AB_BIL_NONE;
            break;
        default:
            break;
    }
    return (token);
}

AB_DEFAULT_BUTTON
bilP_token_to_default_button(BIL_TOKEN token)
{
    AB_DEFAULT_BUTTON      button = AB_DEFAULT_BTN_UNDEF;

    switch (token)
    {
        case AB_BIL_ACTION1_BUTTON: button = AB_DEFAULT_BTN_ACTION1;
            break;
        case AB_BIL_ACTION2_BUTTON: button = AB_DEFAULT_BTN_ACTION2;
            break;
        case AB_BIL_ACTION3_BUTTON: button = AB_DEFAULT_BTN_ACTION3;
            break;
         case AB_BIL_CANCEL_BUTTON: button = AB_DEFAULT_BTN_CANCEL;
            break;
         case AB_BIL_NONE: button = AB_DEFAULT_BTN_NONE;
            break;
        default:
            break;
    }    
    return (button);
}

STRING   
bilP_default_button_to_string(AB_DEFAULT_BUTTON button)
{
    BIL_TOKEN           token = AB_BIL_UNDEF;
    STRING              string = NULL;
 
    if (token = bilP_default_button_to_token(button))
        string = bilP_token_to_string(token);
    return (string);
}
