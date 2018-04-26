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
 *	$XConsortium: util_types.h /main/4 1995/11/06 18:54:42 rswiston $
 *
 * @(#)util_types.h	3.69 15 Feb 1994	cde_app_builder/src/libAButil
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

#ifndef _AB_UTIL_TYPES_H
#define _AB_UTIL_TYPES_H
/*
 * util_types.h - defines all scalar types used in the CDE app builder
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#define AB_VERSION_PREFIX	":bil-version"


/*************************************************************************
**************************************************************************
**									**
**		GENERAL/MISCELLANEOUS TYPES				**
**									**
**************************************************************************
**************************************************************************/

#define OK	(0)
#define	ERROR	(-1)

#define	AB_NO_SUBTYPE	-1

extern const int	Util_major_version;
extern const int	Util_minor_version;

#include <X11/Xmd.h>    /* For BOOL and BYTE typedefs */
typedef unsigned char	UCHAR;
typedef unsigned int	UINT;
typedef unsigned long	ULONG;
typedef char *		STRING;

#ifndef FALSE
#define	FALSE	((BOOL)(0x00))
#endif
#ifndef TRUE
#define	TRUE	((BOOL)(0x01))
#endif

/*************************************************************************
**************************************************************************
**									**
**			APP-BUILDER-SPECIFIC TYPES			**
**									**
**************************************************************************
**************************************************************************/

/*
 * Argument type used in the resfile.
 */
typedef enum
{
	AB_ARG_UNDEF = 0,
	AB_ARG_BOOLEAN,
	AB_ARG_CALLBACK,
	AB_ARG_FLOAT,
	AB_ARG_INT,
	AB_ARG_LITERAL,		/* e.g., #define values, written out "as is" */
	AB_ARG_NONE,		/* no argument */
	AB_ARG_STRING,
	AB_ARG_XMSTRING,
	AB_ARG_XMSTRING_TBL,
	AB_ARG_PIXEL,
	AB_ARG_PIXMAP,
	AB_ARG_FONT,
	AB_ARG_WIDGET,
	AB_ARG_VOID_PTR,
	AB_ARG_VARIABLE,
	AB_ARG_MNEMONIC,
	AB_ARG_TYPE_NUM_VALUES	/* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_ARG_TYPE;

/*
 * Resource "classes"
 *
 * The classes are mutually exclusive.
 */
typedef enum
{
    AB_ARG_CLASS_UNDEF = 0,
    AB_ARG_CLASS_ACCELERATOR,		/* items */
    AB_ARG_CLASS_COLOR,			/* any color-related arg */
    AB_ARG_CLASS_GEOMETRY,		/* includes layout, attachments */
    AB_ARG_CLASS_GLYPH,			/* any glyph (including icons) */
    AB_ARG_CLASS_NON_GLYPH_LABEL,	/* any label not of type glyph */
    AB_ARG_CLASS_OTHER_STRING,		/* any string of no other class */
    AB_ARG_CLASS_OTHER,			/* any of no other class */
    AB_ARG_CLASS_VALUE,			/* e.g., text field, scale, scrollbar*/
    AB_ARG_CLASS_NUM_VALUES	/* must be last */
} AB_ARG_CLASS;

typedef unsigned char AB_ARG_CLASS_FLAGS;	/* bitmap of arg classes */
#define AB_ARG_CLASS_FLAGS_NONE ((AB_ARG_CLASS_FLAGS)0x0000)
#define AB_ARG_CLASS_FLAGS_ALL	((AB_ARG_CLASS_FLAGS)~0x0000)


/*
 * Function type used in the resfile
 */
typedef enum
{
	AB_FUNC_UNDEF = 0,
	AB_FUNC_BUILTIN,	/* ab-defined action (e.g., setvalue) */
	AB_FUNC_USER_DEF,	/* function name (user-defined function) */
	AB_FUNC_CODE_FRAG,	/* actual code entered by user */
	AB_FUNC_ON_ITEM_HELP,	/* dtb_do_on_item_help() */
	AB_FUNC_HELP_VOLUME,	/* dtb_show_help_volume_info() */
	AB_FUNC_TYPE_NUM_VALUES	/* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_FUNC_TYPE;



/*
 * Orientation scalar
 */
typedef enum
{
	AB_ORIENT_UNDEF = 0,
	AB_ORIENT_HORIZONTAL,
	AB_ORIENT_VERTICAL,
	AB_ORIENTATION_NUM_VALUES /* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_ORIENTATION;

/*
 * Types for the AB_GROUP_TYPE attribute.
 */
typedef enum
{
	AB_GROUP_UNDEF = 0,
	AB_GROUP_IGNORE,
	AB_GROUP_COLUMNS,
	AB_GROUP_ROWS,
	AB_GROUP_ROWSCOLUMNS,
	AB_GROUP_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_GROUP_TYPE;

/*
 * Types for AB_LINE_TYPE (Used for Separator Object &
 * framing objects)
 */
typedef enum
{
	AB_LINE_UNDEF = 0,
	AB_LINE_NONE,
	AB_LINE_SHADOW_IN,
	AB_LINE_SHADOW_OUT,
	AB_LINE_ETCHED_IN,
	AB_LINE_ETCHED_OUT,
	AB_LINE_ETCHED_IN_DASH,
	AB_LINE_ETCHED_OUT_DASH,
	AB_LINE_SINGLE_LINE,
	AB_LINE_DOUBLE_LINE,
	AB_LINE_SINGLE_DASHED_LINE,
	AB_LINE_DOUBLE_DASHED_LINE,
	AB_LINE_TYPE_NUM_VALUES
} AB_LINE_TYPE;


/*
 * 
 */
typedef enum
{
	AB_ALIGN_UNDEF = 0,
	AB_ALIGN_TOP,
	AB_ALIGN_RIGHT,
	AB_ALIGN_BOTTOM,
	AB_ALIGN_LEFT,
	AB_ALIGN_CENTER,	/* center verticaly and horizontally */
	AB_ALIGN_VCENTER,	/* center vertically */
	AB_ALIGN_HCENTER,	/* center horizontally */
	AB_ALIGN_LABELS,
	AB_ALIGNMENT_NUM_VALUES	/* must be last! */
} AB_ALIGNMENT;

typedef enum
{
	AB_DIR_UNDEF = 0,
	AB_DIR_LEFT_TO_RIGHT,
	AB_DIR_RIGHT_TO_LEFT,
	AB_DIR_TOP_TO_BOTTOM,
	AB_DIR_BOTTOM_TO_TOP,
	AB_DIR_NUM_VALUES /* must be last! */
} AB_DIRECTION;


/*
 * Types for the AB_{ANCHOR,REFERENCE}_POINT attributes.
 */
typedef enum
{
	AB_CP_UNDEF = 0,
	AB_CP_NORTH,
	AB_CP_NORTHEAST,
	AB_CP_EAST,
	AB_CP_SOUTHEAST,
	AB_CP_SOUTH,
	AB_CP_SOUTHWEST,
	AB_CP_WEST,
	AB_CP_NORTHWEST,
	AB_CP_CENTER,
	AB_COMPASS_POINT_NUM_VALUES /* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_COMPASS_POINT;


/*
 * Layout Attachment types
 */
typedef enum
{
	AB_ATTACH_UNDEF = 0,
        AB_ATTACH_POINT,
        AB_ATTACH_OBJ,
        AB_ATTACH_ALIGN_OBJ_EDGE, 
        AB_ATTACH_GRIDLINE,
        AB_ATTACH_CENTER_GRIDLINE,
        AB_ATTACH_NONE,
        AB_ATTACHMENT_NUM_VALUES
} AB_ATTACH_TYPE;

/*
 * Intitial state types.
 */
typedef enum
{
	AB_STATE_UNDEF = 0,
	AB_STATE_ACTIVE,
	AB_STATE_ICONIC,
	AB_STATE_INACTIVE,
	AB_STATE_INVISIBLE,
	AB_STATE_NOTSELECTED,
	AB_STATE_OPEN,
	AB_STATE_SELECTED,
	AB_STATE_VISIBLE,
	AB_OBJECT_STATE_NUM_VALUES /* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_OBJECT_STATE;


/*
 * 
 */
typedef enum
{
	AB_PACK_UNDEF = 0,
        AB_PACK_NONE,
        AB_PACK_TIGHT,
        AB_PACK_EQUAL,
        AB_PACKING_NUM_VALUES
} AB_PACKING;
 
/*
 * file types
 */
typedef enum
{
        AB_FILE_UNDEF = 0,
	AB_FILE_BIL_COMPLETE,
	AB_FILE_BIL_PROJECT,
	AB_FILE_BIL_MODULE,
        AB_FILE_C,
        AB_FILE_GIL_INTERFACE,
        AB_FILE_GIL_PROJECT,
        AB_FILE_UIL,
        AB_FILE_TYPE_NUM_VALUES
} AB_FILE_TYPE;


/*
 * Action triggers ("when" values)
 *
 */
typedef enum
{
        AB_WHEN_UNDEF = 0,
	AB_WHEN_ACTION1,		/* MessageBox Ok Button */
	AB_WHEN_ACTION2,		/* MessageBox Extra Button */
	AB_WHEN_ACTION3,		/* MessageBox Extra Button */
        AB_WHEN_ACTIVATED,
        AB_WHEN_AFTER_CREATED,		/* immediately after create */
        AB_WHEN_AFTER_RESIZED,
        AB_WHEN_BEFORE_POST_MENU,
	AB_WHEN_BEFORE_TEXT_CHANGED,
	AB_WHEN_CANCEL,			/* MessageBox Cancel Button */
        AB_WHEN_DESTROYED,
        AB_WHEN_DOUBLE_CLICKED_ON,
        AB_WHEN_DRAGGED,
        AB_WHEN_DRAGGED_FROM,
        AB_WHEN_DROPPED_ON,
	AB_WHEN_HIDDEN,
        AB_WHEN_ITEM_SELECTED,
	AB_WHEN_OK,
        AB_WHEN_POPPED_DOWN,
        AB_WHEN_POPPED_UP,
        AB_WHEN_REPAINT_NEEDED,
        AB_WHEN_SESSION_RESTORE,
        AB_WHEN_SESSION_SAVE,
	AB_WHEN_SHOWN,
	AB_WHEN_TEXT_CHANGED,
	AB_WHEN_TOGGLED,
	AB_WHEN_TOOLTALK_DO_COMMAND,
	AB_WHEN_TOOLTALK_GET_STATUS,
	AB_WHEN_TOOLTALK_PAUSE_RESUME,
	AB_WHEN_TOOLTALK_QUIT,
	AB_WHEN_VALUE_CHANGED,
        AB_WHEN_NUM_VALUES
} AB_WHEN;
 
/*
 * Builtin app builder actions
 */
typedef enum
{
        AB_STDACT_UNDEF = 0,
        AB_STDACT_DISABLE,
        AB_STDACT_ENABLE,
        AB_STDACT_HIDE,
        AB_STDACT_SET_LABEL,
        AB_STDACT_SET_TEXT,
        AB_STDACT_SET_VALUE,
        AB_STDACT_SHOW,
        AB_BUILTIN_ACTION_NUM_VALUES
} AB_BUILTIN_ACTION;
 
/*
 * Horizontal and vertical scrollbar settings
 */
typedef enum
{
        AB_SCROLLBAR_UNDEF = 0,
        AB_SCROLLBAR_ALWAYS,
        AB_SCROLLBAR_NEVER,
        AB_SCROLLBAR_WHEN_NEEDED,
        AB_SCROLLBAR_NUM_VALUES /* number of valid values - MUST BE LAST */
        /* ANSI: no comma after last enum item! */
} AB_SCROLLBAR_POLICY;

/*
 * Tooltalk Handling Level types
 */
typedef enum
{
	AB_TOOLTALK_UNDEF,
	AB_TOOLTALK_NONE,
	AB_TOOLTALK_DESKTOP_BASIC,
	AB_TOOLTALK_DESKTOP_ADVANCED,
	AB_TOOLTALK_NUM_VALUES /* number of valid values - LAST */
} AB_TOOLTALK_LEVEL;

/*
 * Sessioning method.
 * Used in Project ABObj.
 */
typedef enum
{
    	AB_SESSIONING_UNDEF = 0,
    	AB_SESSIONING_NONE,
    	AB_SESSIONING_CMDLINE,
    	AB_SESSIONING_FILE,
    	AB_SESSIONING_CMDLINE_AND_FILE,
        AB_SESSIONING_NUM_VALUES /* number of valid values - MUST BE LAST */
        /* ANSI: no comma after last enum item! */
} AB_SESSIONING_METHOD;


/*************************************************************************
**************************************************************************
**									**
**			OBJECT TYPES AND SUBTYPES			**
**									**
**************************************************************************
**************************************************************************/


/*
 * Basic object types
 */
typedef enum
{
	AB_TYPE_UNDEF = 0,
	AB_TYPE_ACTION,
	AB_TYPE_ACTION_LIST,
	AB_TYPE_BASE_WINDOW,
	AB_TYPE_BUTTON,
	AB_TYPE_CHOICE,
	AB_TYPE_COMBO_BOX,
	AB_TYPE_CONTAINER,
	AB_TYPE_DIALOG,
	AB_TYPE_DRAWING_AREA,
	AB_TYPE_FILE,	    /*generic file, usually, a file in a proj*/
	AB_TYPE_MODULE,
	AB_TYPE_ITEM,		/* item in a list, menu, or choice */
	AB_TYPE_LABEL,		/* static text */
	AB_TYPE_LAYERS,
	AB_TYPE_LIST,
	AB_TYPE_MENU,
	AB_TYPE_PROJECT,
	AB_TYPE_SEPARATOR,
	AB_TYPE_SPIN_BOX,
	AB_TYPE_SCALE,
	AB_TYPE_TERM_PANE,
	AB_TYPE_TEXT_FIELD,
	AB_TYPE_TEXT_PANE,
	AB_TYPE_UNKNOWN,
	AB_TYPE_FILE_CHOOSER,
	AB_TYPE_MESSAGE,
	AB_OBJECT_TYPE_NUM_VALUES /* number of valid values (MUST BE LAST!) */
}               AB_OBJECT_TYPE;
#define AB_TYPE_INTERFACE AB_TYPE_MODULE /* INTERFACE WILL GO AWAY */


/*
 * Compound object sub-types
 */
typedef enum
{
	AB_COMP_UNDEF = 0,
	AB_COMP_FILE_CHOOSER,
	AB_COMP_MESSAGE,
	AB_COMPOUND_TYPE_NUM_VALUES	/* must be last! */
} AB_COMPOUND_TYPE;


/*
 * Container subtypes
 */
typedef enum
{
	AB_CONT_UNDEF = 0,
        AB_CONT_ABSOLUTE,
	AB_CONT_BUTTON_PANEL,
	AB_CONT_FOOTER,
        AB_CONT_GROUP,
	AB_CONT_MAIN_WINDOW,
        AB_CONT_MENU_BAR,
	AB_CONT_PANED,
	AB_CONT_RELATIVE,
	AB_CONT_SCROLLED_WINDOW,
	AB_CONT_ROW_COLUMN,
	AB_CONT_TOOL_BAR,
        AB_CONTAINER_TYPE_NUM_VALUES	/* must be last! */
} AB_CONTAINER_TYPE;


typedef enum
{
	AB_DLG_UNDEF = 0,
	AB_DLG_BUILTIN,
	AB_DLG_CUSTOM,
	AB_DIALOG_TYPE_NUM_VALUES	/* must be last! */
} AB_DIALOG_TYPE;


/*
 * Button subtypes.
 */
typedef enum
{
	AB_BUT_UNDEF = 0,
	AB_BUT_PUSH,
	AB_BUT_DRAWN,
   	AB_BUT_MENU,
	AB_BUTTON_TYPE_NUM_VALUES	/* must be last! */
}	AB_BUTTON_TYPE;


/*
 * subtype for: ITEM
 */
typedef enum
{
	AB_ITEM_FOR_UNDEF = 0,
	AB_ITEM_FOR_CHOICE,
     	AB_ITEM_FOR_COMBO_BOX,
	AB_ITEM_FOR_LIST,
	AB_ITEM_FOR_MENU,
        AB_ITEM_FOR_MENUBAR,
 	AB_ITEM_FOR_SPIN_BOX,
	AB_ITEM_TYPE_NUM_VALUES	/* must be last */
} AB_ITEM_TYPE;

/*
 * Menu types.
 */
typedef enum
{
	AB_MENU_UNDEF = 0,
	AB_MENU_PULLDOWN, 
	AB_MENU_POPUP,
	AB_MENU_OPTION,
	AB_MENU_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_MENU_TYPE;


/*
 * Setting types.
 */
typedef enum
{
	AB_CHOICE_UNDEF = 0,
	AB_CHOICE_EXCLUSIVE,
	AB_CHOICE_NONEXCLUSIVE,
	AB_CHOICE_OPTION_MENU,
	AB_CHOICE_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */
	/* ANSI: no comma after last enum item! */
} AB_CHOICE_TYPE;

typedef enum
{
	AB_SELECT_UNDEF = 0,
	AB_SELECT_SINGLE,
	AB_SELECT_BROWSE,
	AB_SELECT_MULTIPLE,
	AB_SELECT_BROWSE_MULTIPLE,
	AB_SELECT_TYPE_NUM_VALUES
        /* ANSI: no comma after last enum item! */

} AB_SELECT_TYPE;

/*
 * subtype for: TEXT_FIELD, TEXT_PANE, SPIN_BOX
 */
typedef enum
{
	AB_TEXT_UNDEF = 0,
	AB_TEXT_ALPHANUMERIC,
	AB_TEXT_NUMERIC,
 	AB_TEXT_DEFINED_STRING,
	AB_TEXT_TYPE_NUM_VALUES	/* must be last! */
} AB_TEXT_TYPE;


/*
 * Label subtypes.
 */
typedef enum
{
	AB_LABEL_UNDEF = 0,
	AB_LABEL_ARROW_DOWN,
	AB_LABEL_ARROW_LEFT,
	AB_LABEL_ARROW_RIGHT,
	AB_LABEL_ARROW_UP,
	AB_LABEL_DRAWN,
	AB_LABEL_GLYPH,
	AB_LABEL_STRING,
	AB_LABEL_SEPARATOR,
	AB_LABEL_TYPE_NUM_VALUES /* number of valid values - MUST BE LAST */ 
} AB_LABEL_TYPE;

typedef enum
{
	AB_STYLE_UNDEF = 0,
	AB_STYLE_NORMAL,
	AB_STYLE_BOLD,
	AB_STYLE_3D,
	AB_LABEL_STYLE_NUM_VALUES	/* must be last */
} AB_LABEL_STYLE;

/*
 * Arrow styles for SpinBox
 */
typedef enum
{
	AB_ARROW_UNDEF = 0,
	AB_ARROW_FLAT_BEGIN,
	AB_ARROW_FLAT_END,
	AB_ARROW_BEGIN,
	AB_ARROW_END,
	AB_ARROW_SPLIT,
	AB_ARROW_STYLE_NUM_VALUES	/* must be last */
} AB_ARROW_STYLE;

#define ab_label_is_arrow(lt)	\
		((lt) == AB_LABEL_ARROW_DOWN || \
		 (lt) == AB_LABEL_ARROW_UP   || \
		 (lt) == AB_LABEL_ARROW_RIGHT || \
		 (lt) == AB_LABEL_ARROW_LEFT )
/*
 * Scale subtypes.
 */
typedef enum
{
        AB_SCALE_UNDEF = 0,
        AB_SCALE_SCALE,
        AB_SCALE_GAUGE,
        AB_SCALE_NUM_VALUES       /* must be last! */
} AB_SCALE_TYPE;

/*
 * File type masks for the File Chooser
 */
typedef enum
{
        AB_FILE_TYPE_MASK_UNDEF = 0,
        AB_FILE_REGULAR,
        AB_FILE_DIRECTORY,
        AB_FILE_ANY,
        AB_FILE_TYPE_MASK_NUM_VALUES       /* must be last! */
} AB_FILE_TYPE_MASK;

/*
 * Message Box types
 */
typedef enum
{
        AB_MSG_UNDEF = 0,
        AB_MSG_ERROR,
        AB_MSG_INFORMATION,
	AB_MSG_QUESTION,
	AB_MSG_WARNING,
	AB_MSG_WORKING,
        AB_MSG_TYPE_NUM_VALUES       /* must be last! */
} AB_MESSAGE_TYPE;

/*
 * Default Button.
 */
typedef enum
{
        AB_DEFAULT_BTN_UNDEF = 0,
        AB_DEFAULT_BTN_ACTION1,
        AB_DEFAULT_BTN_ACTION2,
        AB_DEFAULT_BTN_ACTION3,
        AB_DEFAULT_BTN_CANCEL,
	AB_DEFAULT_BTN_NONE,
        AB_DEFAULT_BTN_NUM_VALUES       /* must be last! */
}       AB_DEFAULT_BUTTON;


/*************************************************************************
**									**
**		Type converters						**
**									**
**************************************************************************/

AB_ARG_TYPE		util_string_to_arg_type(
			    STRING arg_type
			);
STRING			util_arg_type_to_string(
			    AB_ARG_TYPE arg_type
			);

AB_ARG_CLASS		util_string_to_arg_class(
			    STRING		arg_class_string
			);
STRING			util_arg_class_to_string(
			    AB_ARG_CLASS	arg_class
			);
AB_ARG_CLASS_FLAGS	util_arg_class_to_flag(
			    AB_ARG_CLASS	arg_class
			);
AB_ARG_CLASS		util_flag_to_arg_class(
			    AB_ARG_CLASS_FLAGS	arg_flag
			);

AB_LABEL_TYPE		util_string_to_label_type(
			    STRING label_type
			);
STRING			util_label_type_to_string(
			    AB_LABEL_TYPE label_type
			);

AB_OBJECT_STATE		util_string_to_object_state(
			    STRING state
			);
STRING			util_object_state_to_string(
			    AB_OBJECT_STATE state
			);

AB_OBJECT_TYPE		util_string_to_object_type(
			    STRING type
			);
STRING			util_object_type_to_string(
			    AB_OBJECT_TYPE type
			);

AB_ORIENTATION		util_string_to_orientation(
			    STRING orientation
			);
STRING			util_orientation_to_string(
			    AB_ORIENTATION orientation
			);

AB_MENU_TYPE		util_string_to_menu_type(
			    STRING menu_type
			);
STRING			util_menu_type_to_string(
			    AB_MENU_TYPE menu_type
			);

AB_BUTTON_TYPE		util_string_to_button_type(
			    STRING button_type
			);
STRING			util_button_type_to_string(
			    AB_BUTTON_TYPE button_type
			);

AB_ALIGNMENT		util_string_to_alignment(
			    STRING alignment
			);
STRING			util_alignment_to_string(
			    AB_ALIGNMENT alignment
			);

AB_GROUP_TYPE		util_string_to_group_type(
			    STRING group_type
			);
STRING			util_group_type_to_string(
			    AB_GROUP_TYPE group_type
			);

AB_CHOICE_TYPE		util_string_to_choice_type(
			    STRING choice_type
			);
STRING			util_choice_type_to_string(
			    AB_CHOICE_TYPE choice_type
			);

AB_TEXT_TYPE		util_string_to_text_type(
			    STRING text_type
			);
STRING			util_text_type_to_string(
			    AB_TEXT_TYPE text_type
			);

AB_BUILTIN_ACTION	util_string_to_builtin_action(
			    STRING action
			);
STRING			util_builtin_action_to_string(
			    AB_BUILTIN_ACTION action
			);

AB_WHEN			util_string_to_when(
			    STRING when
			);
STRING			util_when_to_string(
			    AB_WHEN when
			);

STRING			util_object_type_to_browser_string(
			    AB_OBJECT_TYPE type
			);
AB_OBJECT_TYPE		util_browser_string_to_object_type(
			    STRING string_type
			);

AB_CONTAINER_TYPE	util_string_to_container_type(
			    STRING container_string
			);
STRING			util_container_type_to_string(
			    AB_CONTAINER_TYPE type
			);

AB_ITEM_TYPE		util_string_to_item_type(
			    STRING item_string
			);
STRING			util_item_type_to_string(
			    AB_ITEM_TYPE type
			);

#endif /* _AB_UTIL_TYPES_H */
