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
 *	$XConsortium: obj.h /main/5 1996/10/29 15:20:51 mustafa $
 *
 *      @(#)obj.h	3.237 04 May 1995
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
 * libABobj/obj.h - definition of a guide UI object
 *
 * The AB_OBJ is a structure which is used to represent all UI objects and
 * actions. The AB_OBJ structure also defines a tree structure that
 * is used to store the objects.
 *
 * For the routines that take or return scoped names, a scoped name may
 * be of the form "name", "module.name", or "module::name", with
 * arbitrary white space around the module/object separator (e.g.,
 * "module ::  name" or "module. name").
 * 
 */
#ifndef _ABOBJ_OBJ_H_
#define _ABOBJ_OBJ_H_

#include <ab_private/AB.h>	/* always the first ab include file */
#include <ab_private/util.h>
#include <ab/util_types.h>
#include <ab_private/istr.h>
#include <ab_private/strlist.h>

#define AB_OBJ_MAX_CHILDREN	2000	/* max children per object */

/*
 * Forward-referenced data types.
 */
typedef	struct _AB_OBJ 			ABObjRec;
typedef	struct _AB_OBJ 			*ABObj;
typedef struct _AB_OBJ			*ABObjPtr;


typedef union _AB_FUNC_VALUE
{
    AB_BUILTIN_ACTION	builtin;	/* AB_FUNC_BUILTIN */
    ISTRING		func_name;	/* AB_FUNC_USERDEF */
    ISTRING		code_frag;	/* AB_FUNC_CODEFRAG */
} AB_FUNC_VALUE;
 

typedef union _AB_ARG_VALUE
{
	int	ival;
	ISTRING	sval;
	float	fval;
} AB_ARG_VALUE;

/*
 * Store Attachment info for object group
 */
typedef struct _AB_ATTACHMENT
{
	AB_ATTACH_TYPE	type;
	void		*value;
	int		offset;
} AB_ATTACHMENT, ABAttachment, *ABAttachPtr;

typedef struct _AB_ATTACHMENT_LIST
{
	AB_ATTACHMENT	north;
	AB_ATTACHMENT	south;
	AB_ATTACHMENT	east;
	AB_ATTACHMENT	west;
} AB_ATTACHMENT_LIST, ABAttachmentList, *ABAttachListPtr;


/*
 * Simple True/False filter for ABObj
 * REMIND: move this to libABobj (these typedefs only)
 */
typedef		BOOL	ABOBJ_TEST_FUNC(ABObj obj);
typedef 	ABOBJ_TEST_FUNC *ABObjTestFunc;


/*
 * Types for code generator
 */
typedef unsigned int			CGenFlags;
struct					CGEN_ANY_DATA_REC;	/* fwd ref */
typedef struct CGEN_ANY_DATA_REC	*CGenAnyData;


/*
 * Types for test mode
 */
typedef unsigned int			TestModeFlags;
struct					TEST_MODE_ANY_DATA_REC;	/* fwd ref */
typedef struct TEST_MODE_ANY_DATA_REC	*TestModeAnyData;

/*
 * for update_clients_with_data methods
 */
typedef int (*UpdateDataFreeFunc)(int update_code, void *update_data);

#define	NoFlags         	0x0000
#define NoCodeGenFlag           0x0001  /* Obj not for generated code 	*/
#define XmConfiguredFlag        0x0002  /* Obj is configured for Motif	*/
#define XmCfgForCodeFlag	0x0004  /* Obj args configured for Code-gen */
#define XmCfgForBuildFlag	0x0008  /* Obj args configured for Building */
#define InstantiatedFlag        0x0010  /* Obj is Instantiated        	*/
#define BuildActionsFlag        0x0020  /* Obj has Building actions   	*/
#define MappedFlag		0x0040 	/* Obj is mapped to screen 	*/
#define AttrChangedFlag         0x0080  /* Obj Attribute Changed     	*/
#define SaveNeededFlag          0x0100  /* Obj changed; Save needed 	*/
#define BeingDestroyedFlag	0x0200	/* Obj to be destroyed       	*/
#define CreateAttrsFlag		0x0400	/* Obj has create-attributes set */
#define PostCreateAttrsFlag	0x0800	/* Obj has create-attributes set */
#define VisibleFlag		0x1000  /* Obj is visible      		*/
#define IconifiedFlag		0x2000	/* Obj (window) is iconified	*/
#define DecorChangedFlag	0x4000  /* Obj Win decorations changed */
#define TestModeWinFlag         0x8000  /* Obj Win modified in Test Mode */


/*
 * These are drag and drop flags
 */ 
typedef BYTE ABDndOpFlags;
#define ABDndOpNone		((BYTE)0x00)
#define	ABDndOpCopy		((BYTE)0x01)
#define	ABDndOpMove		((BYTE)0x02)
#define	ABDndOpLink		((BYTE)0x04)

typedef BYTE ABDndTypeFlags;
#define ABDndTypeNone		((BYTE)0x00)
#define ABDndTypeText		((BYTE)0x01)
#define ABDndTypeFilename	((BYTE)0x02)
#define ABDndTypeUserDef	((BYTE)0x04)
#define ABDndTypeAny		((BYTE)0x08)

typedef enum
{
    AB_REF_UNDEF = 0,
    AB_REF_ATTACH_EAST,
    AB_REF_ATTACH_NORTH,
    AB_REF_ATTACH_SOUTH,
    AB_REF_ATTACH_WEST,
    AB_REF_CHILD,
    AB_REF_CONNECT_FROM,
    AB_REF_CONNECT_TO,
    AB_REF_DEFAULT_BUTTON,
    AB_REF_PARENT,
    AB_REF_ROOT_WINDOW,
    AB_REF_SIBLING_PREV,
    AB_REF_SIBLING_NEXT,
    AB_REF_WIN_PARENT,
    AB_REF_REF_TO,		/* ref to obj */
    AB_OBJ_REF_TYPE_NUM_VALUES	/* must be last */
} AB_OBJ_REF_TYPE;


/*************************************************************************
**									**
**	UI OBJECT DATA TYPE                                   		**
**									**
**************************************************************************/

/*
 *  Actions are stored in a linked list under the project node.
 *  An action defines the destination object and what action to perform
 *  on that object.
 */
/*
 * Extra info for types: AB_TYPE_ACTION
 */
typedef struct _AB_ACTION_INFO
{
    ABObj		from;
    ABObj		to;
    AB_WHEN		when;
    BOOL		auto_named;	 /* we generated name */
    AB_FUNC_TYPE	func_type;
    AB_FUNC_VALUE	func_value;
    ISTRING		func_name_suffix; 
					/* func_name_prefix is from name */
    AB_ARG_TYPE		arg_type;
    AB_ARG_VALUE	arg_value;
    ISTRING		volume_id;	/* Access Help Volume */
    ISTRING		location;	/* Access Help Volume */
} AB_ACTION_INFO;

/*
 * Extra information for types: AB_TYPE_ACTION_LIST
 */
typedef struct _AB_ACTION_LIST_INFO
{
	char	place_holder;	/* this thing really is empty */
} AB_ACTION_LIST_INFO;

/*
 * Extra info for types: AB_BUTTON
 */
typedef struct _AB_BUTTON_INFO
{
	AB_BUTTON_TYPE		type;
        AB_ALIGNMENT		label_alignment;
} AB_BUTTON_INFO;

/*
 * Extra info for types: AB_LABEL
 */
typedef struct _AB_LABEL_INFO
{
	AB_ALIGNMENT		label_alignment;
} AB_LABEL_INFO;

 
/*
 * Extra info for types: AB_TYPE_DRAWING_AREA
 */
typedef struct _AB_DRAWING_AREA_INFO
{
	int			drawarea_width;
	int			drawarea_height;
	AB_SCROLLBAR_POLICY	hscrollbar;
	AB_SCROLLBAR_POLICY	vscrollbar;
        int                     pane_min;
        int                     pane_max;
	ISTRING			menu_title;
} AB_DRAWING_AREA_INFO;

/*
 * Extra info for types: AB_CONTROL_AREA
 */
typedef struct _AB_CONTAINER_INFO
{
	AB_CONTAINER_TYPE	type;
	BOOL			has_border;
	AB_PACKING		packing;
	AB_GROUP_TYPE		group_type;
	AB_ALIGNMENT		col_align;
	int 			num_columns;
	int			hoffset;
	AB_ATTACH_TYPE		hattach_type;
	int			hspacing;
	AB_COMPASS_POINT	ref_point;
	AB_ALIGNMENT		row_align;
	int			num_rows;
	int 			voffset;
	AB_ATTACH_TYPE		vattach_type;
	int			vspacing;
	int			pane_min;
	int			pane_max;
	ISTRING			menu_title;
} AB_CONTAINER_INFO;

/*
 * Extra info for types: AB_TYPE_COMBO_BOX
 */
typedef struct _AB_COMBOBOX_INFO
{
        AB_COMPASS_POINT        label_position;
} AB_COMBO_BOX_INFO;


/*
 * Extra info for types: AB_TYPE_FILE_CHOOSER
 */
typedef struct _AB_FILE_CHOOSER_INFO
{
        ABObj           	win_parent;
	ISTRING			filter_pattern_ABOBJPRIVDDDD3579039821;
	ISTRING			ok_label;
	BOOL			auto_dismiss;
	ISTRING			directory;
	AB_FILE_TYPE_MASK 	file_type_mask;
} AB_FILE_CHOOSER_INFO;

/*
 * Extra info for types: AB_TYPE_MESSAGE
 */
typedef struct _AB_MESSAGE_INFO 
{
        AB_MESSAGE_TYPE		type;
        ISTRING                 msg_string;
	ISTRING			action1_label;
	ISTRING			action2_label;
	ISTRING			action3_label;
	unsigned 		cancel_button : 1;  /* bit field */
	unsigned 		help_button : 1;    /* bit field */
	AB_DEFAULT_BUTTON	default_btn;
} AB_MESSAGE_INFO;

/*
 * Extra info for types: AB_TYPE_MODULE
 */
typedef struct _AB_MODULE_INFO
{
	ISTRING		file_ABOBJPRIVDDDD2382601347;
	ISTRING		stubs_file_ABOBJPRIVDDDD6123725490;
	ISTRING		ui_file_ABOBJPRIVDDDD8421231863;
	StringList	obj_names_list;
	BOOL		write_me;
} AB_MODULE_INFO;

/*
 * Extra info for types: AB_TYPE_MENU_ITEM,  AB_TYPE_CHOICE_ITEM, 
 *			 AB_TYPE_LIST_ITEM,  AB_ITEM_FOR_MENUBAR
 */
typedef struct _AB_ITEM_INFO
{
	AB_ITEM_TYPE	type;
	BOOL		is_initially_selected;
	BOOL		is_help_item;
	ISTRING		accelerator_ABOBJPRIVDDDD0256230318;
	ISTRING		mnemonic_ABOBJPRIVDDDD0903052253;
	AB_LINE_TYPE	line_style;
} AB_ITEM_INFO;

/*
 * Extra info for types: AB_MENU
 */ 
typedef struct _AB_MENU_INFO
{
	AB_MENU_TYPE	type;
	BOOL		tear_off;
	BOOL		exclusive;
} AB_MENU_INFO;


typedef struct _AB_SESSION_MGMT_INFO
{
	AB_SESSIONING_METHOD	sessioning_method;
} AB_SESSION_MGMT_INFO;

typedef struct _AB_TOOLTALK_INFO
{
	AB_TOOLTALK_LEVEL	level;
} AB_TOOLTALK_INFO;

typedef struct _AB_I18N_INFO
{
	BOOL			enabled;
} AB_I18N_INFO;

/*
 * Extra info for types: AB_TYPE_PROJECT
 */
typedef struct _AB_PROJECT_INFO
{
	ISTRING			file_ABOBJPRIVDDDD2192049045;
	ISTRING			stubs_file_ABOBJPRIVDDDD1280681372;
	ISTRING			vendor;
	ISTRING			version;
 	ABObj			root_window;
	StringList		obj_names_list;
	void			*browsers;	/* List of browsers for this project */
	AB_SESSION_MGMT_INFO	session_mgmt;
	AB_TOOLTALK_INFO	tooltalk;
	AB_I18N_INFO		i18n;
	AB_ARG_CLASS_FLAGS	res_file_arg_classes;
	BOOL			is_default;	/* TRUE if not user-specified */
} AB_PROJECT_INFO;

/*
 * Extra info for types: AB_TYPE_CHOICE
 */
typedef struct _AB_CHOICE_INFO
{
	AB_CHOICE_TYPE		type;
	AB_ORIENTATION		orientation;
	int			num_columns;
	BOOL			selection_required;
	int			value_x;
	int			value_y;
	AB_COMPASS_POINT	label_position;
} AB_CHOICE_INFO;

/*
 * Extra info for types: AB_TYPE_SEPARATOR
 */
typedef struct _AB_SEPARATOR_INFO
{
	AB_LINE_TYPE		line_style;
	AB_ORIENTATION		orientation;
} AB_SEPARATOR_INFO;

/*
 * Extra info for types: AB_TYPE_SPIN_BOX
 */
typedef struct _AB_SPIN_BOX_INFO
{
	AB_TEXT_TYPE		type;
	AB_ARROW_STYLE		arrow_style;
        int                     min_value;
        int                     max_value;
	int			increment;
	int			decimal_points;
        int                     initial_value;
        AB_COMPASS_POINT        label_position;
} AB_SPIN_BOX_INFO;

/*
 * Extra information for types: AB_SCALE
 */
typedef struct _AB_SCALE_INFO
{
	AB_ORIENTATION		orientation;
	AB_DIRECTION		direction;
	int			min_value;
	int			max_value;
	int			increment;
	int			decimal_points;
	int			initial_value;
	BOOL			show_value;
        AB_COMPASS_POINT        label_position;
} AB_SCALE_INFO;


/*
 * Exra info for types: AB_TYPE_LIST
 */
typedef struct _AB_LIST_INFO
{
        AB_SELECT_TYPE		selection_mode;
	BOOL			selection_required;
	int			num_rows;
	AB_SCROLLBAR_POLICY	hscrollbar;
	AB_SCROLLBAR_POLICY	vscrollbar;
        AB_COMPASS_POINT 	label_position;
	ISTRING			menu_title;
} AB_LIST_INFO;

/*
 * Extra info for types: AB_TEXT_FIELD, AB_TEXT_PANE
 */
typedef struct _AB_TEXT_INFO
{
	AB_TEXT_TYPE		type;
	int			num_rows;
	int			num_columns;
	int			textpane_width;
	int			textpane_height;
	int			max_length;
	ISTRING			initial_value_string_ABOBJPRIVDDDD2521740590;
	int			initial_value_int;
	BOOL			word_wrap;
	BOOL			has_border;
	AB_COMPASS_POINT	label_position;
	AB_SCROLLBAR_POLICY	hscrollbar;
	AB_SCROLLBAR_POLICY	vscrollbar;
	int			pane_min;
	int			pane_max;
	ISTRING			menu_title;
} AB_TEXT_INFO;

/*
 * Extra info for types: AB_TERM_PANE
 */
typedef struct _AB_TERM_INFO
{
	int			num_rows;
	int			num_columns;
	AB_SCROLLBAR_POLICY	vscrollbar;
	ISTRING			process_string_ABOBJPRIVDDDD3271493068;
        int                     pane_min;
        int                     pane_max;
 	ISTRING			menu_title;
} AB_TERM_INFO;

/*
 * Extra info for types: AB_BASE_WINDOW, AB_POPUP_WINDOW
 */
typedef struct _AB_WINDOW_INFO
{
	ABObj		win_parent;
    	ABObj		default_act_button;
    	ABObj		help_act_button;
	BOOL		resizable;
	BOOL		is_initially_iconic;
	ISTRING		icon_ABOBJPRIVDDDD3403707469;
	ISTRING		icon_mask_ABOBJPRIVDDD93883729879;
	ISTRING		icon_label_ABOBJPRIVDDDD4898216273;
} AB_WINDOW_INFO;

typedef struct _AB_LAYER_INFO
{
        int		pane_min;
        int		pane_max;
} AB_LAYER_INFO;

/*
 * Extra information for all of the object types.
 */
typedef union _AB_OBJ_EXTRA_INFO
{
	AB_ACTION_INFO		action;
	AB_ACTION_LIST_INFO	action_list;
	AB_BUTTON_INFO		button;
	AB_CHOICE_INFO		choice;
	AB_COMBO_BOX_INFO	combo_box;	/* combo box */
	AB_CONTAINER_INFO	container;
	AB_DRAWING_AREA_INFO	drawing_area;
	AB_ITEM_INFO		item;		/* choice,list,menu item */
	AB_LABEL_INFO		label;
        AB_LIST_INFO            list;           /* scrolling list info */
	AB_MENU_INFO		menu;		/* menu info */
	AB_MODULE_INFO		module;		/* module node */
	AB_PROJECT_INFO		project;	/* project node (only one!) */
	AB_SEPARATOR_INFO	separator;	/* separator */
	AB_SPIN_BOX_INFO	spin_box;	/* spin box */
	AB_SCALE_INFO		scale;		/* scale/gauge */
	AB_TERM_INFO		term;		/* term pane */
	AB_TEXT_INFO		text;
	AB_WINDOW_INFO		window;
	AB_FILE_CHOOSER_INFO	file_chooser;
	AB_MESSAGE_INFO		message;
	AB_LAYER_INFO		layer;
} AB_OBJ_EXTRA_INFO;

/*
 * Structure for an AB object containing info about one ui object.
 */
typedef	struct _AB_OBJ
{
	/*
	 * Tree structure
	 */
	ABObj		parent;
	ABObj		next_sibling;
	ABObj		prev_sibling;
	ABObj		first_child;

	/*
	 *  Info common to all objects
	 */
	AB_OBJECT_TYPE	type;
	ISTRING		name_ABOBJPRIVDDDD6281986324;	/* PRIVATE! */
	#ifdef DEBUG
	STRING		debug_name;		/* read-only debugging field */
	unsigned long	debug_last_verify_time;	/* read-only debugging field */
	#endif
	unsigned int	impl_flags_ABOBJPRIVDDDD5707803418;	/* PRIVATE! */
	unsigned long	impl_dnd_flags_ABOBJPRIVDDD7849439167;
	ISTRING		user_data_ABOBJPRIVDDDD1031984606;
	int		x;
	int		y;
	int		width;
	int		width_max;
	int		height;
	int		height_max;
	ISTRING		help_volume_ABOBJPRIVDDDD3479759502;
	ISTRING		help_location_ABOBJPRIVDDDD1652183835;
	ISTRING		help_text_ABOBJPRIVDDDD3129281507;
	ISTRING		drag_cursor_ABOBJPRIVDDDD9147148353;
	ISTRING		drag_cursor_mask_ABOBJPRIVDDDD7624768024;
	ISTRING		bg_color_ABOBJPRIVDDDD7609528164;
	ISTRING		fg_color_ABOBJPRIVDDDD6392057630;
	ISTRING		label_ABOBJPRIVDDDD8127856367;
	AB_LABEL_TYPE	label_type;
	AB_LINE_TYPE	border_frame;
	ISTRING		menu_name_ABOBJPRIVDDDD8412670921;
	ABAttachListPtr	attachments;
	ABObj		part_of;
	ABObj		ref_to;		/*this is a reference to another obj*/

	/* 
	 * These "fields" exist, only through set/get functions:
	 *
	 *	height_is_resizable;
	 *     	is_defined
	 *     	is_read_only
	 *	is_initially_active
	 *	is_initially_iconic
	 *	is_initially_selected
	 *	is_initially_visible
	 *	is_selected;
	 *     	was_written
	 *	width_is_resizable
	 *	drag_initially_enabled
	 *	drag_ops
	 *	drag_to_root_allowed
	 *	drag_types
	 *	drop_initially_enabled
	 *	drop_ops
	 *	drop_types
	 */

	/*
	 * Front-end-specific data (set to null when created)
	 */
	unsigned int		flags;
	void			*ui_handle;	/*xv_handle, Widget, ... */
	ISTRING			class_name_ABOBJPRIVDDDD9513018430;
	void			*ui_args;

	/*
	 * Code-generator specific data
	 */
	CGenFlags		cgen_flags;
	CGenAnyData		cgen_data;

	/*
	 * Handle to browser related data
	 */
	void			*browser_data;

	/*
	 * Handle to project window related data
	 */
	void			*projwin_data;

	/*
	 * Handle to test mode related data (defined in ab/tmodeP.h)
	 */
	TestModeFlags		test_mode_flags;
	TestModeAnyData		test_mode_data;

	/*
	 * Type-specific information
	 */
	AB_OBJ_EXTRA_INFO	info;
} AB_OBJ;

/*
 * This is necessary because some of the methods to return ABObjList
 */
#include <ab_private/obj_list.h>

/*
 * Object methods that deal with lifecycle.
 */
ABObj	obj_create(AB_OBJECT_TYPE obj_type, ABObj parent); /*alloc. and init*/
int	obj_destroy(ABObj obj);			/* deallocate tree */
int	obj_construct(ABObj obj, 
		AB_OBJECT_TYPE type, ABObj parent); /* init to defaults*/
int	obj_destruct(ABObj obj);		/*release data (strings, etc)*/
int	obj_destroy_one(ABObj obj);		/* deallocate one obj */
int	obj_destroy_if_flagged(ABObj obj);	/* check DestroyFlag */
int	obj_tree_destroy_flagged(ABObj tree);	/* check DestroyFlag */
int	obj_init_attachments(ABObj obj);	/* alloc & init attachments */
ABObj	obj_dup(ABObj);				/* returns orphan duplicate */
ABObj	obj_tree_dup(ABObj);
int	obj_dup_child_trees(ABObj to_root, ABObj from_root);

/*
 * Methods dealing with objects that are actually references to other
 * objects.
 *
 * ** Note: these functions allow any parameter to be NULL.
 */
ABObj	obj_create_ref(ABObj actual_obj);	/*create ref to obj*/
ABObj	obj_tree_create_ref(ABObj actual_tree);	/* create tree of refs */
int	obj_create_ref_subtrees(ABObj to_root, ABObj from_root_actual);
int	obj_cvt_to_ref(ABObj, ABObj actual_obj); /*make this obj a ref*/	
ABObj	obj_get_actual_obj(ABObj);		/* could be same obj */
BOOL	obj_is_ref(ABObj);			/* is it */

/*
 * Miscellaneous Object methods
 */
int		obj_update_clients(ABObj);		/* sends update msg */
int		obj_tree_update_clients(ABObj);

int		obj_update_clients_with_data(ABObj,
			int			update_code, 
			void			*update_data,
			UpdateDataFreeFunc	update_data_free_func);

int		obj_tree_update_clients_with_data(ABObj,
			int			update_code, 
			void			*update_data,
			UpdateDataFreeFunc	update_data_free_func);

int		obj_get_child_num(ABObj);	/* what child am I? */
int		obj_set_file(ABObj obj, STRING file);
STRING		obj_get_file(ABObj obj);
ABObj		obj_get_project(ABObj obj);
ABObj		obj_get_module(ABObj obj);
ABObj		obj_get_window(ABObj obj);
int		obj_get_item_num(ABObj);	/* which item am I? */
int		obj_set_type(ABObj obj, AB_OBJECT_TYPE obj_type);
int		obj_set_subtype(ABObj obj, int subtype);
AB_OBJECT_TYPE	obj_get_type(ABObj obj);
int		obj_get_subtype(ABObj obj);
int		obj_set_write_me(ABObj, BOOL write_me);
BOOL		obj_get_write_me(ABObj);
int		obj_move_children(ABObj to, ABObj from);
int		obj_tree_ensure_unique_names_in_modules(ABObj root, 
							int maxnamelen);
int		obj_tree_ensure_unique_names(ABObj root, int maxnamelen);
int		obj_ensure_unique_name(ABObj, ABObj root, int maxnamelen);

STRING		obj_alloc_unique_name(
			ABObj	obj, 
			STRING	name, 
			int	maxNameLen
		);
ISTRING		obj_alloc_unique_name_istr(
			ABObj	obj, 
			ISTRING	name, 
			int	maxNameLen
		);
STRING		obj_get_unique_name(
			ABObj	obj, 
			STRING	name, 
			int	maxnamelen,
			STRING	nameOutBuf
		);

STRING		obj_alloc_unique_name_for_child(
			ABObj	obj,
			STRING	name, 
			int	maxNameLen
		);
ISTRING		obj_alloc_unique_name_istr_for_child(
			ABObj	obj,
			ISTRING	name,
			int	maxnamelen
		);

STRING		obj_get_unique_name_for_child(
			ABObj	obj, 
			STRING	name, 
			int	maxnamelen,
			STRING	nameOutBuf
		);

int		obj_set_name_from_label(ABObj, STRING parent_name);
int		obj_set_name_from_parent(ABObj, STRING suffix);
ABObj		obj_get_parent_of_type(ABObj, AB_OBJECT_TYPE parent_type);
int		obj_get_x(ABObj obj);
int		obj_get_y(ABObj obj);
int		obj_get_width(ABObj obj);
int		obj_get_height(ABObj obj);
int		obj_set_label_type(ABObj, AB_LABEL_TYPE label_type);
AB_LABEL_TYPE	obj_get_label_type(ABObj obj);
int		obj_resize(ABObj, int width, int height);
ABObj		obj_get_root(ABObj);		/* get composite root */
int		obj_move(ABObj, int x, int y);
int		obj_set_geometry(ABObj, int x, int y, int width, int height);
int		obj_get_size(ABObj, int *ptr_x, int *ptr_y);
int		obj_get_position(ABObj, int *ptr_width, int *ptr_height);
int		obj_get_geometry(ABObj,
		    int *ptr_x, int *ptr_y, int *ptr_width, int *ptr_height);
STRING		obj_get_safe_name(ABObj, STRING name, int name_size);
ABObj		obj_get_menubar_help_item(ABObj menubar);


/*
 * "test" queries to see if an operation will be allowed
 *
 * All returns >=0 if allowed, or a negative error code from util_err.h
 */
int		obj_test_resize(ABObj, int width, int height);
int		obj_test_move(ABObj, int x, int y);
int		obj_test_set_geometry(ABObj, 
			int x, int y, int width, int height);
int		obj_test_reparent(ABObj, ABObj new_parent);


/*
 * "Initial" attributes (values when the user's application starts up)
 * (note that boolean values don't use obj_get_... - they use obj_is_... )
 */
int		obj_set_is_initially_active(ABObj, BOOL isit);
BOOL		obj_is_initially_active(ABObj);
int		obj_set_is_initially_iconic(ABObj, BOOL isit);
BOOL		obj_is_initially_iconic(ABObj);
int		obj_set_is_initially_selected(ABObj, BOOL isit);
BOOL		obj_is_initially_selected(ABObj obj);
int		obj_set_is_initially_visible(ABObj, BOOL isit);
BOOL		obj_is_initially_visible(ABObj);
int		obj_get_initial_value_int(ABObj obj);
int		obj_set_initial_value_int(ABObj obj, int ival);
int		obj_set_initial_value_string(ABObj, STRING initial_value);
STRING		obj_get_initial_value_string(ABObj);


/*
 * UI attributes
 */
int		obj_clear_flag(ABObj, unsigned flag);
int		obj_comp_set_flag(ABObj, unsigned flag);
int		obj_comp_clear_flag(ABObj, unsigned flag);
int		obj_set_accelerator(ABObj, STRING accel);
STRING		obj_get_accelerator(ABObj);
int		obj_set_arg_type(ABObj, AB_ARG_TYPE arg_type);
AB_ARG_TYPE	obj_get_arg_type(ABObj);
/* REMIND: removed these methods
int		obj_set_arg_value(ABObj, AB_ARG_VALUE arg_value);
AB_ARG_VALUE	obj_get_arg_value(ABObj);
*/
int		obj_set_arg_float(ABObj, double value);
double		obj_get_arg_float(ABObj obj);
int		obj_set_arg_int(ABObj, int value);
int		obj_get_arg_int(ABObj);
int		obj_set_arg_string(ABObj, STRING value);
STRING		obj_get_arg_string(ABObj);
int		obj_set_bg_color(ABObj, STRING bg_color);
STRING		obj_get_bg_color(ABObj);
int		obj_set_class_name(ABObj, STRING class_name);
STRING		obj_get_class_name(ABObj);
int 		obj_set_col_align(ABObj, AB_ALIGNMENT col_align);
AB_ALIGNMENT 	obj_get_col_align(ABObj);
int		obj_set_container_type(ABObj, AB_CONTAINER_TYPE type);
AB_CONTAINER_TYPE obj_get_container_type(ABObj);
int		obj_set_decimal_points(ABObj, int dec_points);
int		obj_get_decimal_points(ABObj);
int		obj_set_default_act_button(ABObj, ABObj default_b);
ABObj		obj_get_default_act_button(ABObj);
int		obj_set_direction(ABObj, AB_DIRECTION dir);
AB_DIRECTION	obj_get_direction(ABObj);
int		obj_set_drag_cursor(ABObj, STRING filename);
STRING		obj_get_drag_cursor_mask(ABObj);
int		obj_set_drag_cursor_mask(ABObj, STRING filename);
STRING		obj_get_drag_cursor(ABObj);
int		obj_set_drag_initially_enabled(ABObj, BOOL enabled);
BOOL		obj_get_drag_initially_enabled(ABObj);
int		obj_set_drag_ops(ABObj, BYTE ops);
BYTE		obj_get_drag_ops(ABObj);
int		obj_set_drag_to_root_allowed(ABObj, BOOL allowed);
BOOL		obj_get_drag_to_root_allowed(ABObj);
int		obj_set_drag_types(ABObj, BYTE ops);
BYTE		obj_get_drag_types(ABObj);
int             obj_set_drawarea_width(ABObj, int width);
int             obj_get_drawarea_width(ABObj);
int             obj_set_drawarea_height(ABObj, int height);
int             obj_get_drawarea_height(ABObj);
int             obj_set_textpane_width(ABObj obj, int width);
int             obj_set_textpane_height(ABObj obj, int height);
int		obj_set_drop_initially_enabled(ABObj, BOOL enabled);
BOOL		obj_get_drop_initially_enabled(ABObj);
int		obj_set_drop_on_children_is_allowed(ABObj, BOOL allowed);
BOOL		obj_drop_on_children_is_allowed(ABObj);
int		obj_set_drop_ops(ABObj, BYTE ops);
BYTE		obj_get_drop_ops(ABObj);
int		obj_set_drop_types(ABObj, BYTE ops);
BYTE		obj_get_drop_types(ABObj);
int		obj_set_fg_color(ABObj, STRING fg_color);
STRING		obj_get_fg_color(ABObj);
int		obj_get_flag(ABObj, unsigned flag);
int		obj_set_flag(ABObj, unsigned flag);
int             obj_set_func_builtin(ABObj, AB_BUILTIN_ACTION act);
AB_BUILTIN_ACTION obj_get_func_builtin(ABObj);
int		obj_set_func_name(ABObj, STRING func_name);
STRING		obj_get_func_name(ABObj);
int		obj_set_func_code(ABObj, STRING code_frag);
STRING		obj_get_func_code(ABObj);
int		obj_set_func_name_suffix(ABObj, STRING func_name_suffix);
STRING		obj_get_func_name_suffix(ABObj);
int		obj_set_func_type(ABObj, AB_FUNC_TYPE func_type);
AB_FUNC_TYPE	obj_get_func_type(ABObj);
STRING 		obj_get_func_help_location(ABObj obj);
int 		obj_set_func_help_location(ABObj obj, STRING location);
STRING 		obj_get_func_help_volume(ABObj obj);
int 		obj_set_func_help_volume(ABObj obj, STRING volume);
int		obj_set_has_border(ABObj, BOOL has_border);
BOOL		obj_has_border(ABObj);
int		obj_set_height_is_resizable(ABObj, BOOL height_is_resizable);
BOOL		obj_get_height_is_resizable(ABObj);
int		obj_set_help_act_button(ABObj, ABObj default_b);
ABObj		obj_get_help_act_button(ABObj);
BOOL		obj_has_help_data(ABObj);
int		obj_set_help_data(ABObj, 
				STRING help_volume, 
				STRING help_location, 
				STRING help_text);
int		obj_set_help_volume(ABObj,
				STRING help_volume);
int		obj_set_help_location(ABObj,
				STRING help_location);
int		obj_set_help_text(ABObj,
				STRING help_text);
int		obj_get_help_data(ABObj,
				STRING *help_volume, 
				STRING *help_location, 
				STRING *help_text);
STRING		obj_get_help_volume(ABObj);
STRING		obj_get_help_location(ABObj);
STRING		obj_get_help_text(ABObj);
int		obj_set_icon(ABObj, STRING icon);
STRING		obj_get_icon(ABObj);
int		obj_set_icon_label(ABObj, STRING icon_label);
STRING		obj_get_icon_label(ABObj);
int             obj_set_icon_mask(ABObj, STRING icon_mask);
STRING          obj_get_icon_mask(ABObj);
int		obj_set_increment(ABObj, int increment);
int		obj_get_increment(ABObj);
int		obj_set_is_default(ABObj, BOOL is_default);
BOOL		obj_is_default(ABObj);
int		obj_set_is_defined(ABObj, BOOL is_defined);
BOOL		obj_is_defined(ABObj);
int		obj_set_is_help_item(ABObj, BOOL is_help_item);
BOOL		obj_is_help_item(ABObj);
int		obj_select(ABObj);	/* == obj_set_is_selected(TRUE); */
int		obj_unselect(ABObj);	/* == obj_set_is_selected(FALSE); */
int		obj_set_is_selected(ABObj, BOOL is_selected);
BOOL		obj_is_selected(ABObj);
int		obj_set_was_written(ABObj, BOOL was_written);
BOOL		obj_was_written(ABObj);
int		obj_set_item_type(ABObj, AB_ITEM_TYPE type);
AB_ITEM_TYPE	obj_get_item_type(ABObj);
int             obj_set_label(ABObj, STRING label);
STRING          obj_get_label(ABObj);
int		obj_set_label_alignment(ABObj, AB_ALIGNMENT align);
AB_ALIGNMENT	obj_get_label_alignment(ABObj);
int		obj_set_label_position(ABObj, AB_COMPASS_POINT pos);
AB_COMPASS_POINT 
		obj_get_label_position(ABObj);
int		obj_set_group_type(ABObj, AB_GROUP_TYPE type);
AB_GROUP_TYPE	obj_get_group_type(ABObj);
int		obj_set_max_value(ABObj, int max_value);
int		obj_get_max_value(ABObj);
ABObj		obj_get_menu(ABObj);
AB_MENU_TYPE	obj_get_menu_type(ABObj);
int		obj_set_menu_name(ABObj, STRING menu_name);
STRING		obj_get_menu_name(ABObj);
int		obj_set_menu_title(ABObj, STRING menu_title);
STRING		obj_get_menu_title(ABObj);
int		obj_set_min_value(ABObj, int min_value);
int		obj_get_min_value(ABObj);
int             obj_set_mnemonic(ABObj, STRING mnemonic);
STRING          obj_get_mnemonic(ABObj);
int		obj_set_name(ABObj, STRING name);
int		obj_set_name_istr(ABObj, ISTRING name);
int		obj_set_res_file_arg_classes(ABObj, AB_ARG_CLASS_FLAGS);
AB_ARG_CLASS_FLAGS obj_get_res_file_arg_classes(ABObj);
int		obj_set_unique_name(ABObj obj, STRING name);
int		obj_set_unique_name_istr(ABObj obj, ISTRING name);
STRING		obj_get_name(ABObj);
ISTRING		obj_get_name_istr(ABObj);
int		obj_set_num_columns(ABObj, int num_columns);
int		obj_get_num_columns(ABObj);
int		obj_set_num_rows(ABObj, int num_rows);
int		obj_get_num_rows(ABObj);
int             obj_set_orientation(ABObj, AB_ORIENTATION orientation);
AB_ORIENTATION  obj_get_orientation(ABObj);
int             obj_set_process_string(ABObj obj, STRING proc); 
STRING          obj_get_process_string(ABObj obj); 
int		obj_set_selection_mode(ABObj obj, AB_SELECT_TYPE sel);
AB_SELECT_TYPE	obj_get_selection_mode(ABObj obj);	
int		obj_set_selection_required(ABObj, BOOL selection_req);
BOOL		obj_get_selection_required(ABObj);
int		obj_set_parent(ABObj, ABObj parent);
ABObj		obj_get_parent(ABObj);
int		obj_set_read_only(ABObj, BOOL read_only);
BOOL		obj_get_read_only(ABObj);
int		obj_set_resizable(ABObj, BOOL resizable);
BOOL		obj_get_resizable(ABObj);
int		obj_set_user_data(ABObj, STRING user_data);
STRING		obj_get_user_data(ABObj);
int		obj_set_value_x(ABObj, int value_x);
int		obj_get_value_x(ABObj);
int		obj_set_value_y(ABObj, int value_y);
int		obj_get_value_y(ABObj);
int		obj_set_root_window(ABObj obj, ABObj root_window);
ABObj		obj_get_root_window(ABObj obj);
int		obj_set_vendor(ABObj obj, STRING vendor);
STRING		obj_get_vendor(ABObj obj);
int		obj_set_version(ABObj obj, STRING version);
STRING		obj_get_version(ABObj obj);
int		obj_set_hoffset(ABObj obj, int hoffset);
int		obj_get_hoffset(ABObj obj);
int		obj_set_voffset(ABObj obj, int voffset);
int		obj_get_voffset(ABObj obj);
int		obj_set_hspacing(ABObj obj, int hspacing);
int		obj_get_hspacing(ABObj obj);
int		obj_set_vspacing(ABObj obj, int vspacing);
int		obj_get_vspacing(ABObj obj);
int		obj_set_hattach_type(ABObj obj, AB_ATTACH_TYPE type);
AB_ATTACH_TYPE	obj_get_hattach_type(ABObj obj);
int		obj_set_vattach_type(ABObj obj, AB_ATTACH_TYPE type);
AB_ATTACH_TYPE	obj_get_vattach_type(ABObj obj);
int 		obj_set_ref_point(ABObj obj, AB_COMPASS_POINT ref_point);
AB_COMPASS_POINT obj_get_ref_point(ABObj obj);
AB_BUTTON_TYPE	obj_get_button_type(ABObj obj); 
int		obj_set_button_type(ABObj obj, AB_BUTTON_TYPE type);
AB_SCROLLBAR_POLICY obj_get_hscrollbar_policy(ABObj obj);
int		obj_set_hscrollbar_policy(ABObj obj, AB_SCROLLBAR_POLICY hscrollbar);
AB_SCROLLBAR_POLICY obj_get_vscrollbar_policy(ABObj obj);
int		obj_set_vscrollbar_policy(ABObj obj, AB_SCROLLBAR_POLICY vscrollbar);
int		obj_get_max_length(ABObj obj);
int		obj_set_max_length(ABObj obj, int max_len);
AB_TEXT_TYPE 	obj_get_text_type(ABObj obj); 
int		obj_set_text_type(ABObj obj, AB_TEXT_TYPE type); 
AB_PACKING	obj_get_packing(ABObj obj);
int		obj_set_row_align(ABObj obj, AB_ALIGNMENT row_align);
AB_ALIGNMENT	obj_get_row_align(ABObj obj);
AB_CHOICE_TYPE  obj_get_choice_type(ABObj obj);
BOOL		obj_get_tearoff(ABObj obj);
int		obj_set_tearoff(ABObj obj, BOOL tearoff);
BOOL		obj_get_exclusive(ABObj obj);
int		obj_set_from(ABObj, ABObj from);
ABObj		obj_get_from(ABObj);
int		obj_set_to(ABObj, ABObj to);
ABObj		obj_get_to(ABObj);
int		obj_set_when(ABObj, AB_WHEN when);
AB_WHEN		obj_get_when(ABObj);
int		obj_set_width_is_resizable(ABObj, BOOL width_is_resizable);
BOOL		obj_get_width_is_resizable(ABObj);
int		obj_set_win_parent(ABObj obj, ABObj wparent); 
ABObj		obj_get_win_parent(ABObj obj);
BOOL  		obj_has_hscrollbar(ABObj obj);
BOOL  		obj_has_vscrollbar(ABObj obj);
BOOL  		obj_has_scrollbar(ABObj obj);
int		obj_set_filter_pattern(ABObj obj, STRING filter_pattern);
STRING		obj_get_filter_pattern(ABObj obj);
int		obj_set_ok_label(ABObj obj, STRING label);
STRING		obj_get_ok_label(ABObj obj);
int		obj_set_directory(ABObj obj, STRING dir);
STRING		obj_get_directory(ABObj obj);
int		obj_set_auto_dismiss(ABObj obj, BOOL dismiss);
BOOL		obj_get_auto_dismiss(ABObj obj);
int		obj_set_file_type_mask(ABObj obj, AB_FILE_TYPE_MASK ftm);
AB_FILE_TYPE_MASK obj_get_file_type_mask(ABObj obj);
int		obj_set_msg_string(ABObj obj, STRING msg);
STRING		obj_get_msg_string(ABObj obj);
int		obj_set_msg_type(ABObj obj, AB_MESSAGE_TYPE msg_type);
AB_MESSAGE_TYPE obj_get_msg_type(ABObj obj);
int             obj_set_action1_label(ABObj obj, STRING label);
STRING          obj_get_action1_label(ABObj obj);
int             obj_set_action2_label(ABObj obj, STRING label);
STRING          obj_get_action2_label(ABObj obj);
int             obj_set_action3_label(ABObj obj, STRING label);
STRING          obj_get_action3_label(ABObj obj);
int		obj_set_sessioning_method(ABObj obj, 
				AB_SESSIONING_METHOD sessioning_method);
AB_SESSIONING_METHOD obj_get_sessioning_method(ABObj obj);
int             obj_set_default_btn(ABObj obj, AB_DEFAULT_BUTTON button);
AB_DEFAULT_BUTTON obj_get_default_btn(ABObj obj);
int		obj_set_tooltalk_level(ABObj obj, AB_TOOLTALK_LEVEL level);
AB_TOOLTALK_LEVEL obj_get_tooltalk_level(ABObj obj);
int 		obj_set_pane_min( ABObj obj, int val);
int 		obj_get_pane_min( ABObj obj);
int 		obj_set_pane_max( ABObj obj, int val);
int 		obj_get_pane_max( ABObj obj);
int		obj_set_i18n_enabled(ABObj obj, BOOL i18n_enabled);
BOOL		obj_get_i18n_enabled(ABObj obj);
int 		obj_get_num_win_children( ABObj obj);
BOOL		obj_is_defined_module(ABObj obj);


ABAttachment	*obj_get_attachment(ABObj obj,    AB_COMPASS_POINT dir);
AB_ATTACH_TYPE	obj_get_attach_type(ABObj obj,   AB_COMPASS_POINT dir);
void		*obj_get_attach_value(ABObj obj, AB_COMPASS_POINT dir);
int		obj_get_attach_offset(ABObj obj, AB_COMPASS_POINT dir);
int		obj_set_attachment(ABObj obj,    AB_COMPASS_POINT dir,
			AB_ATTACH_TYPE type, void *value, int offset);
int		obj_set_attach_type(ABObj obj,   AB_COMPASS_POINT dir,
			AB_ATTACH_TYPE type);
int		obj_set_attach_value(ABObj obj,  AB_COMPASS_POINT dir,
			void *value);
int		obj_set_attach_offset(ABObj obj, AB_COMPASS_POINT dir,
			int offset);
int		obj_tree_set_flag(ABObj, unsigned flag);
int             obj_tree_clear_flag(ABObj, unsigned flag);
int		obj_set_word_wrap(ABObj obj, BOOL word_wrap);
BOOL		obj_get_word_wrap(ABObj obj);
AB_LINE_TYPE	obj_get_line_style(ABObj	obj);
int		obj_set_line_style(ABObj obj, AB_LINE_TYPE line_style);
AB_ARROW_STYLE	obj_get_arrow_style(ABObj	obj);
int		obj_set_arrow_style(ABObj obj, AB_ARROW_STYLE arrow_style);
int 		obj_set_show_value( ABObj obj, BOOL val);
BOOL 		obj_get_show_value( ABObj obj);
int 		obj_set_show_range_labels( ABObj obj, BOOL val);
BOOL 		obj_get_show_range_labels( ABObj obj);
int 		obj_set_show_endboxes( ABObj obj, BOOL val);
BOOL 		obj_get_show_endboxes( ABObj obj);
BOOL 		obj_has_action1_button(ABObj obj);
BOOL 		obj_has_action2_button(ABObj obj);
BOOL 		obj_has_action3_button(ABObj obj);
BOOL 		obj_has_cancel_button(ABObj obj);
int 		obj_set_cancel_button(ABObj obj, BOOL has_button);
BOOL 		obj_has_help_button(ABObj obj);
int 		obj_set_help_button(ABObj obj, BOOL has_button);

/*
 * Name/object scoping methods (obj_scoped.c)
 *
 * A scoped name can be <name>, <module>::<name>, or <module>.<name>, with
 * spaces allowed around the . or :: .
 * The search_first obj is normally a module or a project.
 *
 * *** SEE COMMENTS IN obj_scoped.c FOR MORE DETAILS! *****
 */
ABObj		obj_scoped_find_by_name(ABObj search_first, STRING scoped_name);


/*
 * "Isa" relationships (isa.c)
 */
BOOL		obj_is_action(ABObj obj);
BOOL		obj_is_action_list(ABObj obj);
BOOL		obj_is_action_target(ABObj);
BOOL		obj_is_base_win(ABObj obj);
BOOL		obj_is_button(ABObj obj);
BOOL		obj_is_cross_module(ABObj obj);
BOOL		obj_is_drawing_area(ABObj obj);
BOOL            obj_is_sub(ABObj obj);
BOOL            obj_is_root(ABObj obj);
BOOL		obj_is_combo_box(ABObj obj);
BOOL            obj_is_control(ABObj obj);
BOOL		obj_is_control_panel(ABObj obj);
BOOL		obj_is_container(ABObj obj);
BOOL		obj_is_file_chooser(ABObj obj);
BOOL		obj_is_group(ABObj obj);
BOOL		obj_is_group_member(ABObj obj);
BOOL		obj_is_module(ABObj obj);
BOOL		obj_is_intra_module(ABObj obj);	/* affects only one module? */
BOOL		obj_is_item(ABObj obj);
BOOL		obj_is_label(ABObj obj);
BOOL		obj_is_layers(ABObj obj);
BOOL		obj_is_list(ABObj obj);
BOOL            obj_is_menu(ABObj obj);
BOOL            obj_is_menubar(ABObj obj);
BOOL		obj_is_menu_button(ABObj obj);
BOOL            obj_is_menu_ref(ABObj obj);
BOOL		obj_is_message(ABObj obj);
BOOL		obj_is_numeric_field(ABObj obj);
BOOL		obj_is_option_menu(ABObj obj);
BOOL		obj_is_pane(ABObj obj);
BOOL		obj_is_paned_win(ABObj obj);
BOOL		obj_is_popup(ABObj obj);
BOOL		obj_is_popup_win(ABObj obj);
BOOL		obj_is_project(ABObj);
BOOL		obj_is_project_action_target(ABObj);
BOOL		obj_is_push_button(ABObj);
BOOL            obj_is_choice(ABObj obj);
BOOL		obj_is_separator(ABObj obj);
BOOL		obj_is_spin_box(ABObj obj);
BOOL		obj_is_scale(ABObj obj);
BOOL		obj_is_term_pane(ABObj obj);
BOOL		obj_is_text(ABObj obj);
BOOL		obj_is_text_field(ABObj obj);
BOOL		obj_is_text_pane(ABObj obj);
BOOL		obj_is_top_level(ABObj obj);
BOOL		obj_is_ui(ABObj obj);
BOOL		obj_is_salient(ABObj);	/* can be manipulated by user */
BOOL		obj_is_salient_ui(ABObj);	/* salient && ui */
BOOL		obj_is_virtual(ABObj obj);
BOOL		obj_is_window(ABObj obj);
BOOL		obj_is_xmconfiged(ABObj obj);
BOOL		obj_is_descendant_of(ABObj obj, ABObj ancestor);
BOOL		obj_is_sibling(ABObj obj, ABObj sibling);

/*
 * Object tree manipulations
 *
 * Note: salient objects are those that can be manipulated directly
 *       by the user (e.g., atomic objects, composite roots)
 */
int		obj_ensure_num_children(ABObj obj, int min_children);
int		obj_append_child(ABObj obj, ABObj child);
int		obj_append_sibling(ABObj obj, ABObj sibling);
int		obj_prepend_sibling(ABObj obj, ABObj sibling);
int		obj_swap_siblings(ABObj child1, ABObj child2);
ABObj		obj_find_action(ABObj root, ABObj action_template);
ABObj		obj_find_by_name(ABObj root, STRING name);
ABObj		obj_find_by_name_and_type(ABObj root, STRING, AB_OBJECT_TYPE);
ABObj		obj_find_by_type(ABObj root, AB_OBJECT_TYPE type);
ABObj		obj_find_child_by_label(ABObj, STRING label);
ABObj		obj_find_child_by_name(ABObj, STRING child_name);
ABObj		obj_find_child_by_type(ABObj, AB_OBJECT_TYPE);
ABObj		obj_find_menu_by_name(ABObj root, STRING menu_name);
ABObj		obj_find_module_by_name(ABObj root, STRING moduleName);
ABObj		obj_scoped_find_or_create_undef(ABObj parent, 
				STRING scoped_name, AB_OBJECT_TYPE type);
ABObj		obj_get_container_child(ABObj);
ABObj		obj_get_child(ABObj obj, int which_child);
ABObj		obj_get_child_cond(ABObj obj, int which_child, 
						BOOL (*fn)(ABObj));
ABObj		obj_get_item(ABObj obj, int which_item);
int		obj_get_num_children(ABObj obj);
int		obj_get_num_children_cond(ABObj obj, BOOL (*fn)(ABObj));
int		obj_get_num_comp_subobjs(ABObj obj);
int		obj_get_num_items(ABObj obj);
int		obj_get_num_siblings(ABObj obj);
int		obj_get_num_salient_children(ABObj);
int		obj_get_num_salient_ui_children(ABObj);
ABObj		obj_get_pane_child(ABObj);
ABObj		obj_get_salient_child(ABObj, int child_num);
ABObj		obj_get_salient_ui_child(ABObj, int child_num);
BOOL		obj_has_child(ABObj obj);
BOOL		obj_has_menu(ABObj obj);
BOOL		obj_has_parent(ABObj obj);
BOOL		obj_has_popup_child(ABObj obj);
int		obj_insert_child(ABObj obj, 
					ABObj newchild, int newchildnum);
int		obj_prepend_child(ABObj obj);
int		obj_unparent(ABObj obj);
int		obj_reparent(ABObj obj, ABObj new_parent);
int		obj_replace(ABObj obj, ABObj replacement);
int		obj_tree_replace(ABObj obj, ABObj replacement);
int		obj_unlink(ABObj obj);
ABObjList	obj_get_refs_to(ABObj obj);

/*
 * General utility functions
 */
AB_OBJECT_TYPE	ab_get_proper_subobj_type(AB_OBJECT_TYPE type);
AB_ITEM_TYPE	ab_get_proper_item_type(AB_OBJECT_TYPE type);
STRING		ab_make_valid_c_ident(STRING name);
BOOL		ab_c_ident_is_ok(STRING ident);
BOOL		ab_ident_is_ok(STRING ident);
STRING		ab_ident_from_file_name(STRING file_name);
STRING		ab_ident_from_name_and_label(STRING name, STRING label);
STRING		obj_scoped_name_get_module_name(STRING scoped_name);
STRING		obj_scoped_name_get_obj_name(STRING scoped_name);
int		obj_scoped_name_split(	/* _buf params may be NULL */
			STRING	scoped_name,
			STRING	module_name_buf,
			int	module_name_buf_size,
			STRING	obj_name_buf,
			int	obj_name_buf_size);

/*
 * Actions
 */
int		obj_add_action(ABObj obj, ABObj action);

/*
 * Debugging functions
 */
int	obj_print(ABObj obj);
int	obj_print_indented(ABObj obj, int spaces, int verbosity);
int	obj_tree_print(ABObj obj);
int	obj_verify(ABObj);		/* < 0 if corrupted */
int	obj_tree_verify(ABObj tree);	/* < 0 if corrupted */


/*************************************************************************
**************************************************************************
**									**
**		Inline definitions					**
**									**
**************************************************************************
*************************************************************************/

#define obj_is_action(obj)	((obj)->type == AB_TYPE_ACTION)
#define obj_is_action_list(obj) ((obj)->type == AB_TYPE_ACTION_LIST)
#define obj_is_module(obj)	((obj)->type == AB_TYPE_MODULE)
#define obj_is_project(obj)	((obj)->type == AB_TYPE_PROJECT)
#define obj_is_salient_ui(obj)	(obj_is_ui(obj) && (obj_is_salient(obj)))
#define obj_is_menu_ref(obj)	(obj_is_menu(obj) && (obj_is_ref(obj)))
#define obj_is_choice_item(obj) (obj->type == AB_TYPE_ITEM && \
				obj->info.item.type == AB_ITEM_FOR_CHOICE)
#define obj_is_combo_box_item(obj) (obj->type == AB_TYPE_ITEM && \
				obj->info.item.type == AB_ITEM_FOR_COMBO_BOX)
#define obj_is_list_item(obj) 	(obj->type == AB_TYPE_ITEM && \
				obj->info.item.type == AB_ITEM_FOR_LIST)
#define obj_is_menu_item(obj)	(obj->type == AB_TYPE_ITEM && \
            			 obj->info.item.type == AB_ITEM_FOR_MENU)
#define obj_is_menubar_item(obj) (obj->type == AB_TYPE_ITEM && \
            			 obj->info.item.type == AB_ITEM_FOR_MENUBAR)
#define obj_is_spin_box_item(obj) (obj->type == AB_TYPE_ITEM && \
            			 obj->info.item.type == AB_ITEM_FOR_SPIN_BOX)

/* #define obj_get_label(obj) \
		(istr_string((obj)->label_ABOBJPRIVDDDD8127856367)) */
/* #define obj_get_name(obj) \
		(istr_string((obj)->name_ABOBJPRIVDDDD6281986324)) */
/* #define obj_get_parent(obj)	((obj)->parent) */
#define obj_set_parent(obj,par)	(obj_reparent(obj, par))
#define obj_get_type(obj)   	((obj)->type)
#define obj_has_child(obj)  	(obj_get_num_children(obj) > 0)
#define obj_has_parent(obj) 	((obj)->parent != NULL)
#define obj_prepend_child(obj, child) (obj_insert_child(obj, child, 0))
#define obj_get_root(obj)   (ABObj)((obj)->part_of == NULL? (obj):(obj)->part_of)

#define obj_select(obj)		(obj_set_is_selected(obj, TRUE))
#define obj_unselect(obj)	(obj_set_is_selected(obj, FALSE))

/*
 * These 4 set macros only set the obj fields, period. If the object is
 * already created, then you should use obj_set_geometry().
 */
#define obj_set_x(obj,newx)	((obj)->x=(newx))
#define obj_set_y(obj,newy)	((obj)->y=(newy))
#define obj_set_width(obj,newwidth)	((obj)->width=(newwidth))
#define obj_set_height(obj,newheight)	((obj)->height=(newheight))

#define obj_get_x(obj)		((obj)->x)
#define obj_get_y(obj)		((obj)->y)
#define obj_get_width(obj)	((obj)->width)
#define obj_get_height(obj)	((obj)->height)
#define obj_get_ref_to(obj)	((obj)->ref_to)
#define obj_set_ref_to(obj,ref)	((obj)->ref_to = (ref))
/* #define obj_get_label_type(obj)	((obj)->label_type) */
#define	obj_get_width_resizable(obj)	((obj)->width_resizable)
#define	obj_get_height_resizable(obj)	((obj)->height_resizable)
#define obj_get_border_frame(obj)	((obj)->border_frame)
#define obj_set_border_frame(obj,b)	((obj)->border_frame=(b))
#define obj_has_border_frame(obj)	(obj_get_border_frame((obj)) != AB_LINE_NONE)
#define obj_has_label(obj)	(!util_strempty(obj_get_label((obj))))

#define obj_has_scrollbar(obj)	\
	(obj_has_hscrollbar(obj) || obj_has_vscrollbar(obj))

#define	obj_set_flag(obj, f)			\
	( ((obj)->flags) |= ((unsigned int)(f)))

#define obj_clear_flag(obj, f) \
	( ((obj)->flags) &= ~((unsigned int)(f)) )

#define	obj_has_flag(obj, f)	\
	( (((obj)->flags) & ((unsigned int)(f))) != 0 )

/*
 * destroy! (these functions set the client pointer to NULL).
 *    Major problems have occurred because references to old objects
 *    were kept around, and not set to NULL.  So, please don't mess.
 */
int obj_destroy_impl(ABObj *objPtr);
int obj_destroy_one_impl(ABObj *objPtr);
int obj_destroy_if_flagged_impl(ABObj *objPtr);
int obj_tree_destroy_flagged_impl(ABObj *treePtr);
#define obj_destroy(obj)		(obj_destroy_impl(&(obj)))
#define obj_destroy_one(obj)		(obj_destroy_one_impl(&(obj)))
#define obj_destroy_if_flagged(obj)	(obj_destroy_if_flagged_impl(&(obj)))
#define obj_tree_destroy_flagged(tree) (obj_tree_destroy_flagged_impl(&(tree)))

#endif /* _ABOBJ_OBJ_H_ */
