%{
/*
 *	$XConsortium: bil_parse.y /main/4 1995/11/09 12:38:03 rswiston $
 *
 *      @(#)bil_parse.y	1.80 16 Oct 1994
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

#include <stdio.h>
#include <ab_private/bil_parse.h>
#include <ab/util_types.h>
#include "bil_loadattP.h"

#define TOK (bilP_load_get_token())

%}
/*
 * We assume that yacc spits out the symbols in the order they are 
 * declared.
 * We can use AB_BIL_MARKER_FIRST and BIL_MARKER_LAST to determine the
 * range and number of token values.
 */
%token	AB_BIL_MARKER_FIRST
%token	AB_BIL_UNDEF
%token	AB_BIL_UNDEF_KEYWORD	/* actual :undef in file... */
%token	AB_BIL_ABSOLUTE
%token	AB_BIL_ACCELERATOR
%token	AB_BIL_ACCESS_HELP_VOLUME
%token	AB_BIL_ACTION
%token	AB_BIL_ACTION1		/* Message When */
%token	AB_BIL_ACTION2		/* Message When */
%token	AB_BIL_ACTION3		/* Message When */
%token	AB_BIL_ACTION1_BUTTON	/* Message Box default button */
%token	AB_BIL_ACTION2_BUTTON	/* Message Box default button */
%token	AB_BIL_ACTION3_BUTTON	/* Message Box default button */
%token	AB_BIL_ACTION1_LABEL
%token	AB_BIL_ACTION2_LABEL
%token	AB_BIL_ACTION3_LABEL
%token	AB_BIL_ACTION_TYPE
%token	AB_BIL_ACTIVATED
%token	AB_BIL_ACTIVE
%token	AB_BIL_ADVANCED
%token	AB_BIL_AFTER_CREATED
%token	AB_BIL_ALPHANUMERIC
%token	AB_BIL_ALWAYS
%token	AB_BIL_ANY
%token	AB_BIL_APPLICATION
%token	AB_BIL_ARROW_DOWN
%token	AB_BIL_ARROW_LEFT
%token	AB_BIL_ARROW_RIGHT
%token	AB_BIL_ARROW_UP
%token	AB_BIL_ARROW_FLAT_BEGIN
%token	AB_BIL_ARROW_FLAT_END
%token	AB_BIL_ARROW_BEGIN
%token	AB_BIL_ARROW_END
%token	AB_BIL_ARROW_SPLIT
%token	AB_BIL_ARROW_STYLE
%token	AB_BIL_ARG_TYPE
%token	AB_BIL_ARG_VALUE
%token	AB_BIL_ATTACH_ALIGN_OBJ_EDGE
%token	AB_BIL_ATTACH_GRIDLINE
%token	AB_BIL_ATTACH_CENTER_GRIDLINE
%token	AB_BIL_ATTACH_OBJ
%token	AB_BIL_ATTACH_POINT
%token	AB_BIL_AUTO_DISMISS
%token	AB_BIL_BASE_WINDOW
%token	AB_BIL_BASIC
%token	AB_BIL_BEFORE_POST_MENU
%token	AB_BIL_BEFORE_TEXT_CHANGED
%token	AB_BIL_BG_COLOR
%token	AB_BIL_BOLD
%token	AB_BIL_BOOLEAN
%token	AB_BIL_BORDER_FRAME
%token  AB_BIL_BOTTOM
%token  AB_BIL_BOTTOM_TO_TOP
%token  AB_BIL_BROWSE
%token  AB_BIL_BROWSE_MULTIPLE
%token  AB_BIL_BUILTIN
%token	AB_BIL_BUTTON
%token	AB_BIL_BUTTON_PANEL
%token	AB_BIL_BUTTON_TYPE
%token  AB_BIL_CALLBACK
%token  AB_BIL_CALL_FUNCTION
%token	AB_BIL_CANCEL
%token	AB_BIL_CANCEL_BUTTON
%token  AB_BIL_CENTER
%token	AB_BIL_CENTERS
%token	AB_BIL_CHILDREN
%token	AB_BIL_CHOICE
%token	AB_BIL_CHOICE_TYPE
%token	AB_BIL_COLUMN
%token	AB_BIL_COLUMN_ALIGNMENT
%token	AB_BIL_CLASS_NAME
%token	AB_BIL_COMPOSITE
%token	AB_BIL_COMPOUND_TYPE
%token	AB_BIL_COLOR			/* resource class */
%token	AB_BIL_COLOR_CHOOSER
%token	AB_BIL_CONNECTION
%token	AB_BIL_CONNECTION_LIST
%token	AB_BIL_CONTAINER
%token	AB_BIL_CONTAINER_TYPE
%token	AB_BIL_COMBO_BOX
%token	AB_BIL_COMBO_BOX_EDITABLE
%token	AB_BIL_COMBO_BOX_STATIC
%token	AB_BIL_COMBO_BOX_STYLE
%token	AB_BIL_COPY
%token	AB_BIL_DATA_END
%token	AB_BIL_DECIMAL_POINTS
%token	AB_BIL_DEFAULT
%token	AB_BIL_DEFAULT_BUTTON
%token	AB_BIL_DEFINED_STRING
%token	AB_BIL_DESTROYED
%token	AB_BIL_DIALOG
%token	AB_BIL_DIRECTION
%token	AB_BIL_DIRECTORY
%token	AB_BIL_DISABLE
%token	AB_BIL_DOUBLE_CLICKED_ON
%token	AB_BIL_DOUBLE_LINE
%token	AB_BIL_DOUBLE_DASHED_LINE
%token	AB_BIL_DRAG_CURSOR
%token	AB_BIL_DRAG_CURSOR_MASK
%token	AB_BIL_DRAG_ENABLED
%token	AB_BIL_DRAG_OPS
%token	AB_BIL_DRAG_TYPES
%token	AB_BIL_DRAG_TO_ROOT_ALLOWED
%token	AB_BIL_DRAGGED
%token	AB_BIL_DRAGGED_FROM
%token  AB_BIL_DRAWAREA_WIDTH
%token  AB_BIL_DRAWAREA_HEIGHT
%token	AB_BIL_DRAWING_AREA
%token	AB_BIL_DRAWN_BUTTON
%token	AB_BIL_DROP_ENABLED
%token	AB_BIL_DROP_OPS
%token	AB_BIL_DROP_SITE_CHILD_ALLOWED
%token	AB_BIL_DROP_TYPES
%token	AB_BIL_DROPPED_ON
%token  AB_BIL_EAST
%token  AB_BIL_EAST_ATTACHMENT
%token	AB_BIL_ELEMENT
%token	AB_BIL_ENABLE
%token	AB_BIL_EQUAL
%token	AB_BIL_ERROR
%token	AB_BIL_ETCHED_IN
%token	AB_BIL_ETCHED_IN_DASH
%token	AB_BIL_ETCHED_OUT
%token	AB_BIL_ETCHED_OUT_DASH
%token	AB_BIL_EXCLUSIVE
%token	AB_BIL_EXECUTE_CODE
%token	AB_BIL_FALSE
%token	AB_BIL_FG_COLOR
%token	AB_BIL_FILE_CHOOSER
%token	AB_BIL_FILE_TYPE_MASK
%token	AB_BIL_FILENAME
%token	AB_BIL_FILES
%token	AB_BIL_FILTER_PATTERN
%token	AB_BIL_FLOAT
%token	AB_BIL_FONT
%token	AB_BIL_FONT_CHOOSER
%token	AB_BIL_FOOTER
%token	AB_BIL_FROM
%token	AB_BIL_GAUGE
%token	AB_BIL_GEOMETRY			/* resource class */
%token	AB_BIL_GLYPH			/* resource class */
%token	AB_BIL_GLYPH_FILE
%token	AB_BIL_GRAPHIC
%token	AB_BIL_GROUP
%token	AB_BIL_GROUP_TYPE
%token	AB_BIL_HAS_FOOTER
%token	AB_BIL_HATTACH_TYPE
%token	AB_BIL_HCENTER
%token	AB_BIL_HELP_BUTTON
%token	AB_BIL_HELP_LOCATION
%token	AB_BIL_HELP_TEXT
%token	AB_BIL_HELP_VOLUME
%token	AB_BIL_HEIGHT
%token	AB_BIL_HEIGHT_MAX
%token	AB_BIL_HEIGHT_RESIZABLE
%token	AB_BIL_HIDE
%token	AB_BIL_HSCROLL
%token	AB_BIL_HSCROLLBAR
%token	AB_BIL_HSPACING
%token	AB_BIL_HOFFSET
%token	AB_BIL_HORIZONTAL
%token	AB_BIL_I18N
%token	AB_BIL_I18N_ENABLED
%token	AB_BIL_ICON
%token	AB_BIL_ICONIC
%token	AB_BIL_ICON_FILE
%token	AB_BIL_ICON_LABEL
%token	AB_BIL_ICON_MASK_FILE
%token	AB_BIL_IGNORE
%token	AB_BIL_INACTIVE
%token	AB_BIL_INCREMENT
%token	AB_BIL_INFO
%token	AB_BIL_INITIAL_STATE
%token	AB_BIL_INT
%token	AB_BIL_INVISIBLE
%token	AB_BIL_IS_HELP_ITEM
%token	AB_BIL_ITEM
%token	AB_BIL_ITEM_FOR_CHOICE
%token	AB_BIL_ITEM_FOR_COMBO_BOX
%token	AB_BIL_ITEM_FOR_LIST
%token	AB_BIL_ITEM_FOR_MENU
%token	AB_BIL_ITEM_FOR_MENUBAR
%token	AB_BIL_ITEM_FOR_SPIN_BOX
%token	AB_BIL_ITEM_SELECTED
%token	AB_BIL_ITEM_TYPE
%token	AB_BIL_LABEL
%token	AB_BIL_LABEL_ALIGNMENT
%token	AB_BIL_LABEL_POSITION
%token	AB_BIL_LABELS		/* for alignment */
%token	AB_BIL_LABEL_TYPE
%token	AB_BIL_LABEL_STRING	/* resource class */
%token	AB_BIL_LABEL_STYLE
%token	AB_BIL_LAYERS
%token	AB_BIL_LEFT
%token	AB_BIL_LEFT_TO_RIGHT
%token	AB_BIL_LINE_STYLE
%token	AB_BIL_LINK
%token	AB_BIL_LIST
%token	AB_BIL_LITERAL
%token	AB_BIL_LOAD
%token	AB_BIL_MAIN_WINDOW
%token	AB_BIL_MAX_LABEL
%token	AB_BIL_MAX_VALUE
%token	AB_BIL_MENU
%token	AB_BIL_MENU_BAR
%token	AB_BIL_MENU_BUTTON
%token	AB_BIL_MENU_TITLE
%token	AB_BIL_MENU_TYPE
%token	AB_BIL_MESSAGE
%token	AB_BIL_MESSAGE_TYPE
%token	AB_BIL_MIN_LABEL
%token	AB_BIL_MIN_VALUE
%token	AB_BIL_MNEMONIC
%token	AB_BIL_MODULE
%token	AB_BIL_MOVE
%token	AB_BIL_MULTIPLE
%token	AB_BIL_MULTIPLE_SELECTIONS
%token	AB_BIL_NIL
%token	AB_BIL_NONEXCLUSIVE
%token	AB_BIL_NAME
%token	AB_BIL_NEVER
%token	AB_BIL_NONE
%token	AB_BIL_NORTH
%token  AB_BIL_NORTHEAST
%token  AB_BIL_NORTHWEST
%token	AB_BIL_NORTH_ATTACHMENT
%token	AB_BIL_NOTSELECTED
%token	AB_BIL_NUM_COLUMNS
%token	AB_BIL_NUM_ROWS
%token	AB_BIL_NUMERIC
%token	AB_BIL_OK
%token	AB_BIL_OK_BUTTON
%token	AB_BIL_OK_LABEL
%token	AB_BIL_ON_ITEM_HELP
%token	AB_BIL_OPEN
%token	AB_BIL_OPTION
%token	AB_BIL_OPTION_MENU
%token	AB_BIL_ORIENTATION
%token	AB_BIL_OTHER			/* resource class */
%token	AB_BIL_OTHER_STRING		/* resource class */
%token	AB_BIL_PACKING
%token	AB_BIL_PANED
%token	AB_BIL_PANE_MAX_HEIGHT
%token	AB_BIL_PANE_MIN_HEIGHT
%token	AB_BIL_PARAMETER_TYPE
%token	AB_BIL_PIXEL
%token	AB_BIL_PIXMAP
%token	AB_BIL_POPPED_DOWN
%token	AB_BIL_POPPED_UP
%token	AB_BIL_PROCESS_STRING
%token	AB_BIL_PROJECT
%token	AB_BIL_PULLDOWN
%token	AB_BIL_PUSH_BUTTON
%token	AB_BIL_QUESTION
%token	AB_BIL_READ_ONLY 
%token	AB_BIL_REFERENCE_POINT
%token	AB_BIL_REGULAR
%token	AB_BIL_RELATIVE
%token	AB_BIL_REPAINT_NEEDED
%token	AB_BIL_RETURN_TYPE
%token	AB_BIL_RESIZABLE
%token	AB_BIL_RESIZED
%token	AB_BIL_RESOURCE
%token	AB_BIL_RES_FILE_TYPES
%token	AB_BIL_RIGHT
%token	AB_BIL_RIGHT_TO_LEFT
%token	AB_BIL_ROOT_WINDOW
%token	AB_BIL_ROW
%token	AB_BIL_ROWSCOLUMNS
%token	AB_BIL_ROW_ALIGNMENT
%token	AB_BIL_ROW_COLUMN
%token	AB_BIL_SAVE 
%token	AB_BIL_SAVE_AS 
%token	AB_BIL_SB_ALWAYS_VISIBLE 
%token	AB_BIL_SCALE
%token	AB_BIL_SCROLLED_WINDOW 
%token	AB_BIL_SELECT
%token	AB_BIL_SELECTED
%token  AB_BIL_SELECTION_MODE
%token	AB_BIL_SELECTION_BOX
%token	AB_BIL_SELECTION_REQUIRED
%token	AB_BIL_SEPARATOR
%token	AB_BIL_SESSION_RESTORE
%token	AB_BIL_SESSION_SAVE
%token	AB_BIL_SESSION_MGMT
%token	AB_BIL_SESSIONING_METHOD
%token	AB_BIL_SESSIONING_CMDLINE
%token	AB_BIL_SESSIONING_FILE
%token	AB_BIL_SESSIONING_CMDLINE_AND_FILE
%token	AB_BIL_SET_LABEL
%token	AB_BIL_SET_LEFT_FOOTER
%token	AB_BIL_SET_RIGHT_FOOTER
%token	AB_BIL_SET_TEXT
%token	AB_BIL_SET_VALUE
%token	AB_BIL_SHADOW_IN 
%token	AB_BIL_SHADOW_OUT
%token	AB_BIL_SHOW
%token	AB_BIL_SHOW_VALUE
%token	AB_BIL_SINGLE
%token	AB_BIL_SINGLE_LINE
%token	AB_BIL_SINGLE_DASHED_LINE
%token	AB_BIL_SLIDER
%token	AB_BIL_SLIDER_TYPE
%token  AB_BIL_SOUTH
%token  AB_BIL_SOUTHEAST
%token  AB_BIL_SOUTHWEST
%token  AB_BIL_SOUTH_ATTACHMENT
%token	AB_BIL_SPIN_BOX
%token	AB_BIL_STANDARD
%token	AB_BIL_STRING
%token	AB_BIL_MAX_LENGTH
%token	AB_BIL_STYLE_BOLD
%token	AB_BIL_STYLE_NORMAL
%token	AB_BIL_STYLE_3D
%token	AB_BIL_TEAR_OFF
%token	AB_BIL_TERM_PANE
%token	AB_BIL_TEXT
%token	AB_BIL_TEXT_CHANGED
%token	AB_BIL_TEXT_FIELD
%token	AB_BIL_TEXT_PANE
%token	AB_BIL_TEXT_TYPE
%token	AB_BIL_TIGHT
%token	AB_BIL_TITLE
%token	AB_BIL_TITLE_TYPE
%token	AB_BIL_TO
%token	AB_BIL_TOGGLED
%token	AB_BIL_TOOL_BAR
%token	AB_BIL_TOOLTALK
%token	AB_BIL_TOOLTALK_DESKTOP_LEVEL
%token	AB_BIL_TOOLTALK_DO_COMMAND
%token	AB_BIL_TOOLTALK_GET_STATUS
%token	AB_BIL_TOOLTALK_PAUSE_RESUME
%token	AB_BIL_TOOLTALK_QUIT
%token	AB_BIL_TOP
%token	AB_BIL_TOP_TO_BOTTOM
%token	AB_BIL_TRUE
%token	AB_BIL_TYPE
%token	AB_BIL_USER_DATA
%token	AB_BIL_USER_DEF
%token	AB_BIL_INITIAL_VALUE
%token	AB_BIL_VALUE			/* resource class (:value) */
%token	AB_BIL_VALUE_BOOL
%token	AB_BIL_VALUE_CHANGED
%token	AB_BIL_VALUE_FLOAT
%token	AB_BIL_VALUE_IDENT
%token	AB_BIL_VALUE_INT
%token	AB_BIL_VALUE_STRING
%token	AB_BIL_VATTACH_TYPE
%token	AB_BIL_VCENTER
%token	AB_BIL_VENDOR
%token	AB_BIL_VERSION
%token	AB_BIL_VERTICAL
%token	AB_BIL_VISIBLE
%token	AB_BIL_VOFFSET
%token	AB_BIL_VOID
%token	AB_BIL_VSCROLL
%token	AB_BIL_VSCROLLBAR
%token	AB_BIL_VSPACING
%token	AB_BIL_WARNING
%token  AB_BIL_WEST
%token  AB_BIL_WEST_ATTACHMENT
%token	AB_BIL_WHEN
%token	AB_BIL_WHEN_NEEDED
%token	AB_BIL_WIDGET
%token	AB_BIL_WIDTH
%token	AB_BIL_WIDTH_MAX
%token	AB_BIL_WIDTH_RESIZABLE
%token	AB_BIL_WIN_CHILDREN
%token	AB_BIL_WIN_PARENT		/* backwards compat */
%token	AB_BIL_WORD_WRAP
%token	AB_BIL_WORKING
%token	AB_BIL_X
%token	AB_BIL_XMSTRING
%token	AB_BIL_XMSTRING_TBL
%token	AB_BIL_Y
%token	AB_BIL_MARKER_LAST

%start	file
%%

file		: file_contents {bilP_load_end_of_file();}
		;

file_contents	: global_att_or_obj file_contents
		| global_att_or_obj
		;

global_att_or_obj : global_att
		| obj
		;
		
global_att	: AB_BIL_DATA_END {bilP_load_end_of_data();}
		;

obj		: simple_obj {bilP_load_end_of_obj($1/*objClass*/);}
		| list_obj {bilP_load_end_of_obj($1/*objClass*/);}
		;

simple_obj	: simple_obj_class {bilP_load_att_class($1);} name {bilP_load_att_name($3);}  simple_obj_body
		| unnamed_simple_obj_class {bilP_load_att_class($1);} simple_obj_body
		;

list_obj	: list_obj_class {bilP_load_att_class($1);} name {bilP_load_att_name($3);} list_obj_body
		| unnamed_list_obj_class {bilP_load_att_class($1);} list_obj_body
		;

/*
 *  Object classes
 */
simple_obj_class : AB_BIL_ELEMENT{$$=TOK;} 
		 ;

unnamed_simple_obj_class : AB_BIL_CONNECTION{$$=TOK;} ;

list_obj_class	: AB_BIL_MODULE{$$=TOK;}
		| AB_BIL_COMPOSITE{$$=TOK;}
		| AB_BIL_PROJECT{$$=TOK;}
		;

unnamed_list_obj_class: AB_BIL_CONNECTION_LIST{$$=TOK;};

/*
 * Object bodies
 */
simple_obj_body	: '(' atts ')'
		| '(' ')'
		;

list_obj_body	: '(' atts ')'
		| '(' list_of_objs ')'
		| '(' atts list_of_objs ')'
		| '(' ')'
		;

list_of_objs	: list_of_objs obj
		| obj
		;

/*
 * Attribute value
 */
value		: AB_BIL_VALUE_BOOL{$$=TOK;}
		| AB_BIL_VALUE_FLOAT{$$=TOK;}
		| AB_BIL_VALUE_IDENT{$$=TOK;}
		| AB_BIL_VALUE_INT{$$=TOK;}
		| AB_BIL_VALUE_STRING{$$=TOK;}
		| AB_BIL_ANY{$$=TOK;}
		| AB_BIL_COLOR{$$=TOK;}
		| AB_BIL_COPY{$$=TOK;}
		| AB_BIL_FILENAME{$$=TOK;}
		| AB_BIL_GEOMETRY{$$=TOK;}
		| AB_BIL_GLYPH{$$=TOK;}
		| AB_BIL_LABEL_STRING{$$=TOK;}
		| AB_BIL_LINK{$$=TOK;}
		| AB_BIL_MOVE{$$=TOK;}
		| AB_BIL_OTHER{$$=TOK;}
		| AB_BIL_OTHER_STRING{$$=TOK;}
		| AB_BIL_TEXT{$$=TOK;}
		| AB_BIL_USER_DEF{$$=TOK;}
		| AB_BIL_VALUE{$$=TOK;}
	 	;

attach_type	: AB_BIL_ATTACH_POINT {$$=TOK;}
		| AB_BIL_ATTACH_OBJ {$$=TOK;}
		| AB_BIL_ATTACH_ALIGN_OBJ_EDGE {$$=TOK;}
		| AB_BIL_ATTACH_GRIDLINE {$$=TOK;}
		| AB_BIL_ATTACH_CENTER_GRIDLINE {$$=TOK;}
		| AB_BIL_NONE {$$=TOK;}
		| AB_BIL_NIL {$$=TOK;}
		;

value_list	: '('')'
                | '(' list_values ')'
                ;

list_values	: value {bilP_load_list_value($1);}
		| list_values value {bilP_load_list_value($2);}
                ;

attach_list	: '(' attach_type {bilP_load_attachment_type($2);} value {bilP_load_attachment_value($4);} value {bilP_load_attachment_offset($6);} ')' 
		;

name            : AB_BIL_VALUE_IDENT{$$=TOK;}
                ;

when_value	: AB_BIL_ACTION1 {$$=TOK;}
		| AB_BIL_ACTION2 {$$=TOK;}
		| AB_BIL_ACTION3 {$$=TOK;}
		| AB_BIL_ACTIVATED {$$=TOK;}
		| AB_BIL_AFTER_CREATED {$$=TOK;}
		| AB_BIL_BEFORE_POST_MENU {$$=TOK;}
		| AB_BIL_BEFORE_TEXT_CHANGED {$$=TOK;}
		| AB_BIL_CANCEL {$$=TOK;}
		| AB_BIL_DESTROYED {$$=TOK;}
		| AB_BIL_DOUBLE_CLICKED_ON {$$=TOK;}
		| AB_BIL_DRAGGED {$$=TOK;}
		| AB_BIL_DRAGGED_FROM {$$=TOK;}
		| AB_BIL_DROPPED_ON {$$=TOK;}
		| AB_BIL_HIDE {$$=TOK;}
		| AB_BIL_ITEM_SELECTED {$$=TOK;}
		| AB_BIL_OK {$$=TOK;}
		| AB_BIL_POPPED_DOWN {$$=TOK;}
		| AB_BIL_POPPED_UP {$$=TOK;}
		| AB_BIL_REPAINT_NEEDED {$$=TOK;}
		| AB_BIL_RESIZED {$$=TOK;}
        	| AB_BIL_SESSION_RESTORE {$$=TOK;}
        	| AB_BIL_SESSION_SAVE {$$=TOK;}
        	| AB_BIL_SHOW {$$=TOK;}
        	| AB_BIL_TEXT_CHANGED {$$=TOK;}
        	| AB_BIL_TOGGLED {$$=TOK;}
        	| AB_BIL_TOOLTALK_DO_COMMAND {$$=TOK;}
        	| AB_BIL_TOOLTALK_GET_STATUS {$$=TOK;}
        	| AB_BIL_TOOLTALK_PAUSE_RESUME {$$=TOK;}
        	| AB_BIL_TOOLTALK_QUIT {$$=TOK;}
		| AB_BIL_VALUE_CHANGED {$$=TOK;}
		;

/*
 * Attribute definitions
 */
atts		: att
		| atts att
		;

att		: AB_BIL_ACCELERATOR	token	{bilP_load_att_accelerator($2);}
		| AB_BIL_ACTION		token	{bilP_load_att_action($2); }
		| AB_BIL_ACTION_TYPE	token	{bilP_load_att_action_type($2);}
		| AB_BIL_ACTION1_BUTTON	token	{bilP_load_att_action1_button($2);}
		| AB_BIL_ACTION2_BUTTON	token	{bilP_load_att_action2_button($2);}
		| AB_BIL_ACTION1_LABEL	token	{bilP_load_att_action1_label($2);}
		| AB_BIL_ACTION2_LABEL	token	{bilP_load_att_action2_label($2);}
		| AB_BIL_ACTION3_LABEL	token	{bilP_load_att_action3_label($2);}
		| AB_BIL_ARROW_STYLE	token	{bilP_load_att_arrow_style($2);}
		| AB_BIL_ACTIVE		token	{bilP_load_att_active($2); }
		| AB_BIL_ARG_TYPE	token	{bilP_load_att_arg_type($2); }
		| AB_BIL_ARG_VALUE	token 	{bilP_load_att_arg_value($2); }
		| AB_BIL_AUTO_DISMISS	token 	{bilP_load_att_auto_dismiss($2); }
		| AB_BIL_BG_COLOR	token	{bilP_load_att_bg_color($2); }
		| AB_BIL_BUTTON_TYPE	token	{bilP_load_att_button_type($2);}
		| AB_BIL_CANCEL_BUTTON	token	{bilP_load_att_cancel_button($2);}
		| AB_BIL_CHILDREN {bilP_load_att_children_begin();} value_list {bilP_load_att_children_end();}
		| AB_BIL_CLASS_NAME	token	{bilP_load_att_class_name($2);}
		| AB_BIL_COLUMN_ALIGNMENT token {bilP_load_att_col_alignment($2);}
		| AB_BIL_COMBO_BOX_STYLE	token	{bilP_load_att_combo_box_style($2);}
		| AB_BIL_CONTAINER_TYPE	token	{bilP_load_att_container_type($2); }
                | AB_BIL_DECIMAL_POINTS	token   {bilP_load_att_decimal_points($2);}
		| AB_BIL_DEFAULT	token	{bilP_load_att_default($2);}
		| AB_BIL_DEFAULT_BUTTON	token	{bilP_load_att_default_button($2);}
                | AB_BIL_DIRECTION	token   {bilP_load_att_direction($2);}
		| AB_BIL_DIRECTORY	token	{bilP_load_att_directory($2);}
		| AB_BIL_DRAG_CURSOR	token	{bilP_load_att_drag_cursor($2);}
		| AB_BIL_DRAG_CURSOR_MASK	token	{bilP_load_att_drag_cursor_mask($2);}
		| AB_BIL_DRAG_ENABLED	token	{bilP_load_att_drag_enabled($2);}
		| AB_BIL_DRAG_OPS {bilP_load_att_drag_ops_begin(); } value_list {bilP_load_att_drag_ops_end();}
		| AB_BIL_DRAG_TO_ROOT_ALLOWED token	{bilP_load_att_drag_to_root_allowed($2);}
		| AB_BIL_DRAG_TYPES {bilP_load_att_drag_types_begin(); } value_list {bilP_load_att_drag_types_end();}
		| AB_BIL_DRAWAREA_WIDTH token	{bilP_load_att_drawarea_width($2); }
		| AB_BIL_DRAWAREA_HEIGHT token	{bilP_load_att_drawarea_height($2); }
		| AB_BIL_DROP_ENABLED	token	{bilP_load_att_drop_enabled($2);}
		| AB_BIL_DROP_OPS {bilP_load_att_drop_ops_begin(); } value_list {bilP_load_att_drop_ops_end();}
		| AB_BIL_DROP_SITE_CHILD_ALLOWED token	{bilP_load_att_drop_site_child_allowed($2);}
		| AB_BIL_DROP_TYPES {bilP_load_att_drop_types_begin(); } value_list {bilP_load_att_drop_types_end();}
		| AB_BIL_EAST_ATTACHMENT {bilP_load_att_east_attachment_begin(); } attach_list {bilP_load_att_east_attachment_end();}
		| AB_BIL_EXCLUSIVE	token
		| AB_BIL_FG_COLOR	token	{bilP_load_att_fg_color($2); }
		| AB_BIL_FILE_TYPE_MASK token 	{bilP_load_att_file_type_mask($2); }
		| AB_BIL_FILTER_PATTERN token 	{bilP_load_att_filter_pattern($2); }
		| AB_BIL_FROM		token	{bilP_load_att_from($2);}
		| AB_BIL_GROUP_TYPE	token	{bilP_load_att_group_type($2);}
		| AB_BIL_HATTACH_TYPE	token	{bilP_load_att_hattach_type($2);}
		| AB_BIL_HELP_BUTTON	token	{bilP_load_att_help_button($2);}
		| AB_BIL_HELP_LOCATION	token	{bilP_load_att_help_location($2);}
		| AB_BIL_HELP_TEXT	token	{bilP_load_att_help_text($2); }
		| AB_BIL_HELP_VOLUME	token	{bilP_load_att_help_volume($2);}
		| AB_BIL_HEIGHT		token	{bilP_load_att_height($2);}
		| AB_BIL_HEIGHT_MAX	token	{bilP_load_att_height_max($2);}
		| AB_BIL_HSCROLL	token	{bilP_load_att_hscrollbar($2);}
		| AB_BIL_HSCROLLBAR	token	{bilP_load_att_hscrollbar($2);}
		| AB_BIL_HSPACING	token	{bilP_load_att_hspacing($2);}
		| AB_BIL_HOFFSET	token	{bilP_load_att_hoffset($2);}
		| AB_BIL_I18N		simple_obj_body
		| AB_BIL_I18N_ENABLED	token	{bilP_load_att_i18n_enabled($2);}
		| AB_BIL_ICONIC		token	{bilP_load_att_iconic($2);}
		| AB_BIL_ICON_FILE	token	{bilP_load_att_icon($2);}
		| AB_BIL_ICON_LABEL	token	{bilP_load_att_icon_label($2);}
		| AB_BIL_ICON_MASK_FILE	token	{bilP_load_att_icon_mask($2);}
		| AB_BIL_INCREMENT	token	{bilP_load_att_increment($2);}
		| AB_BIL_INITIAL_STATE	token	{bilP_load_att_initial_state($2);}
		| AB_BIL_IS_HELP_ITEM	token	{bilP_load_att_is_help_item($2);}
		| AB_BIL_ITEM_TYPE	token	{bilP_load_att_item_type($2);}
		| AB_BIL_SELECTED	token	{bilP_load_att_selected($2);}
		| AB_BIL_SB_ALWAYS_VISIBLE token {bilP_load_att_sb_always_visible($2);}
		| AB_BIL_LABEL		token	{bilP_load_att_label($2);}
		| AB_BIL_LABEL_ALIGNMENT token	{bilP_load_att_alignment($2); }
		| AB_BIL_LABEL_POSITION	token	{bilP_load_att_label_position($2);}
		| AB_BIL_LABEL_TYPE	token	{bilP_load_att_label_type($2);}
		| AB_BIL_LABEL_STYLE	token	{bilP_load_att_label_emphasis($2);}
		| AB_BIL_LINE_STYLE	token	{bilP_load_att_line_style($2);}
		| AB_BIL_MAX_LABEL	token	{bilP_load_att_max_label($2);}
		| AB_BIL_MAX_VALUE	token	{bilP_load_att_max_value($2);}
		| AB_BIL_MENU		token	{bilP_load_att_menu($2);}
		| AB_BIL_MENU_TITLE	token	{bilP_load_att_menu_title($2);}
		| AB_BIL_MESSAGE	token	{bilP_load_att_message($2);}
		| AB_BIL_MESSAGE_TYPE   token	{bilP_load_att_msg_type($2);}
		| AB_BIL_MIN_LABEL	token	{bilP_load_att_min_label($2);}
		| AB_BIL_MIN_VALUE	token	{bilP_load_att_min_value($2);}
		| AB_BIL_MNEMONIC	token	{bilP_load_att_mnemonic($2);}
		| AB_BIL_FILES {bilP_load_att_files_begin();} value_list {bilP_load_att_files_end();}
		| AB_BIL_SELECTION_MODE	token	{bilP_load_att_selection_mode($2);}
		| AB_BIL_MULTIPLE_SELECTIONS	token	{bilP_load_att_multiple_selections($2);}
		| AB_BIL_NORTH_ATTACHMENT {bilP_load_att_north_attachment_begin();} attach_list {bilP_load_att_north_attachment_end();}
		| AB_BIL_NUM_COLUMNS	token	{bilP_load_att_num_columns($2);}
		| AB_BIL_OK_LABEL	token	{bilP_load_att_ok_label($2);}
		| AB_BIL_ORIENTATION	token	{bilP_load_att_orientation($2);}
		| AB_BIL_WIN_CHILDREN	{bilP_load_att_win_children_begin();} value_list {bilP_load_att_win_children_end();}	

		| AB_BIL_WIN_PARENT	token	{bilP_load_att_win_parent($2);}
		| AB_BIL_PANE_MAX_HEIGHT token	{bilP_load_att_pane_max_height($2);}
		| AB_BIL_PANE_MIN_HEIGHT token	{bilP_load_att_pane_min_height($2);}
		| AB_BIL_PARAMETER_TYPE	token	{bilP_load_att_arg_type($2);}
		| AB_BIL_PROCESS_STRING	token	{bilP_load_att_process_string($2);}
		| AB_BIL_READ_ONLY	token	{bilP_load_att_read_only($2);}
		| AB_BIL_REFERENCE_POINT token	{bilP_load_att_reference_point($2);}
		| AB_BIL_RES_FILE_TYPES {bilP_load_att_res_file_types_begin();} value_list {bilP_load_att_res_file_types_end();}
		| AB_BIL_RESIZABLE 	token	{bilP_load_att_resizable($2);}
		| AB_BIL_RETURN_TYPE	token	{bilP_load_att_arg_type($2);}
		| AB_BIL_ROOT_WINDOW	token	{bilP_load_att_root_window($2);}
		| AB_BIL_ROW_ALIGNMENT	token	{bilP_load_att_row_alignment($2);}
		| AB_BIL_SESSIONING_METHOD	token	{bilP_load_att_sessioning_method($2);}
		| AB_BIL_SESSION_MGMT simple_obj_body
		| AB_BIL_SOUTH_ATTACHMENT {bilP_load_att_south_attachment_begin();} attach_list {bilP_load_att_south_attachment_end();}
		| AB_BIL_TEXT_TYPE	token	{bilP_load_att_text_type($2);}
		| AB_BIL_TO		token	{bilP_load_att_to($2); }
                | AB_BIL_TOOLTALK	simple_obj_body
                | AB_BIL_TOOLTALK_DESKTOP_LEVEL      token   {bilP_load_att_tt_desktop_level($2);}
		| AB_BIL_WIDTH		token	{bilP_load_att_width($2);}
		| AB_BIL_WIDTH_RESIZABLE token	{bilP_load_att_width_resizable($2);}
		| AB_BIL_HEIGHT_RESIZABLE token	{bilP_load_att_height_resizable($2);}
		| AB_BIL_RESOURCE	token value	{bilP_load_att_resource($2);}
		| AB_BIL_NUM_ROWS	token	{bilP_load_att_num_rows($2);}
		| AB_BIL_CHOICE_TYPE   	token	{bilP_load_att_choice_type($2);}
		| AB_BIL_SELECTION_REQUIRED token	{bilP_load_att_selection_required($2);}
		| AB_BIL_BORDER_FRAME	token	{bilP_load_att_border_frame($2);}
		| AB_BIL_HAS_FOOTER	token	{bilP_load_att_has_footer($2);}
		| AB_BIL_MENU_TYPE	token	{bilP_load_att_menu_type($2);}
		| AB_BIL_MAX_LENGTH  	token	{bilP_load_att_max_length($2);}
		| AB_BIL_SHOW_VALUE  	token	{bilP_load_att_show_value($2);}
		| AB_BIL_SLIDER_TYPE  	token	{bilP_load_att_slider_type($2);}
		| AB_BIL_TEAR_OFF	token	{bilP_load_att_tear_off($2);}
		| AB_BIL_TYPE		token	{bilP_load_att_type($2);}
		| AB_BIL_USER_DATA	token	{bilP_load_att_user_data($2);}
		| AB_BIL_INITIAL_VALUE	token	{bilP_load_att_initial_value($2);}
		| AB_BIL_VATTACH_TYPE	token	{bilP_load_att_vattach_type($2);}
		| AB_BIL_VENDOR		token	{bilP_load_att_vendor($2);}
		| AB_BIL_VERSION	token	{bilP_load_att_version($2);}
		| AB_BIL_VISIBLE	token	{bilP_load_att_visible($2); }
		| AB_BIL_VOFFSET	token	{bilP_load_att_voffset($2); }
		| AB_BIL_VSCROLL	token	{bilP_load_att_vscrollbar($2); }
		| AB_BIL_VSCROLLBAR	token	{bilP_load_att_vscrollbar($2); }
		| AB_BIL_VSPACING	token	{bilP_load_att_vspacing($2); }
		| AB_BIL_WEST_ATTACHMENT {bilP_load_att_west_attachment_begin();} attach_list {bilP_load_att_west_attachment_end();} 
		| AB_BIL_WHEN		when_value {bilP_load_att_when($2);}
		| AB_BIL_WIDTH_MAX	token	{bilP_load_att_width_max($2); }
		| AB_BIL_WORD_WRAP	token	{bilP_load_att_word_wrap($2); }
		| AB_BIL_X		token	{bilP_load_att_x($2); }
		| AB_BIL_Y		token	{bilP_load_att_y($2); }
		;

token	: AB_BIL_UNDEF {$$=TOK;}
	| AB_BIL_UNDEF_KEYWORD {$$=TOK;}
	| AB_BIL_ABSOLUTE {$$=TOK;}
	| AB_BIL_ACCELERATOR {$$=TOK;}
	| AB_BIL_ACCESS_HELP_VOLUME {$$=TOK;}
	| AB_BIL_ACTION {$$=TOK;}
	| AB_BIL_ACTION1 {$$=TOK;}
	| AB_BIL_ACTION2 {$$=TOK;}
	| AB_BIL_ACTION3 {$$=TOK;}
	| AB_BIL_ACTION1_BUTTON {$$=TOK;}
	| AB_BIL_ACTION2_BUTTON {$$=TOK;}
	| AB_BIL_ACTION3_BUTTON {$$=TOK;}
	| AB_BIL_ACTION1_LABEL {$$=TOK;}
	| AB_BIL_ACTION2_LABEL {$$=TOK;}
	| AB_BIL_ACTION3_LABEL {$$=TOK;}
	| AB_BIL_ACTION_TYPE {$$=TOK;}
	| AB_BIL_ACTIVATED {$$=TOK;}
	| AB_BIL_ACTIVE {$$=TOK;}
	| AB_BIL_ADVANCED {$$=TOK;}
	| AB_BIL_AFTER_CREATED {$$=TOK;}
	| AB_BIL_ALPHANUMERIC {$$=TOK;}
	| AB_BIL_ALWAYS {$$=TOK;}
	| AB_BIL_ANY {$$=TOK;}
	| AB_BIL_APPLICATION {$$=TOK;}
	| AB_BIL_ARG_TYPE {$$=TOK;}
	| AB_BIL_ARROW_DOWN {$$=TOK;}
	| AB_BIL_ARROW_LEFT {$$=TOK;}
	| AB_BIL_ARROW_RIGHT {$$=TOK;}
	| AB_BIL_ARROW_UP {$$=TOK;}
	| AB_BIL_ARROW_FLAT_BEGIN {$$=TOK;}
	| AB_BIL_ARROW_FLAT_END {$$=TOK;}
	| AB_BIL_ARROW_BEGIN {$$=TOK;}
	| AB_BIL_ARROW_END {$$=TOK;}
	| AB_BIL_ARROW_SPLIT {$$=TOK;}
	| AB_BIL_ATTACH_ALIGN_OBJ_EDGE {$$=TOK;}
	| AB_BIL_ATTACH_GRIDLINE {$$=TOK;}
	| AB_BIL_ATTACH_CENTER_GRIDLINE {$$=TOK;}
	| AB_BIL_ATTACH_OBJ {$$=TOK;}
	| AB_BIL_ATTACH_POINT {$$=TOK;}
	| AB_BIL_AUTO_DISMISS {$$=TOK;}
	| AB_BIL_BASE_WINDOW {$$=TOK;}
	| AB_BIL_BASIC {$$=TOK;}
	| AB_BIL_BEFORE_POST_MENU {$$=TOK;}
	| AB_BIL_BEFORE_TEXT_CHANGED {$$=TOK;}
	| AB_BIL_BG_COLOR {$$=TOK;}
	| AB_BIL_BOLD {$$=TOK;}
	| AB_BIL_BOOLEAN {$$=TOK;}
	| AB_BIL_BORDER_FRAME {$$=TOK;}
	| AB_BIL_BOTTOM {$$=TOK;}
	| AB_BIL_BOTTOM_TO_TOP {$$=TOK;}
	| AB_BIL_BROWSE {$$=TOK;}
	| AB_BIL_BROWSE_MULTIPLE {$$=TOK;}
	| AB_BIL_BUILTIN {$$=TOK;}
	| AB_BIL_BUTTON {$$=TOK;}
	| AB_BIL_BUTTON_PANEL {$$=TOK;}
	| AB_BIL_BUTTON_TYPE {$$=TOK;}
	| AB_BIL_CALLBACK {$$=TOK;}
	| AB_BIL_CALL_FUNCTION {$$=TOK;}
	| AB_BIL_CANCEL {$$=TOK;}
	| AB_BIL_CANCEL_BUTTON {$$=TOK;}
	| AB_BIL_CENTER {$$=TOK;}
	| AB_BIL_CENTERS {$$=TOK;}
	| AB_BIL_CHILDREN {$$=TOK;}
	| AB_BIL_CHOICE {$$=TOK;}
	| AB_BIL_CHOICE_TYPE {$$=TOK;}
	| AB_BIL_COLUMN {$$=TOK;}
	| AB_BIL_COLUMN_ALIGNMENT {$$=TOK;}
	| AB_BIL_COMBO_BOX_EDITABLE {$$=TOK;}
	| AB_BIL_COMBO_BOX_STATIC {$$=TOK;}
	| AB_BIL_CLASS_NAME {$$=TOK;}
	| AB_BIL_COMPOSITE {$$=TOK;}
	| AB_BIL_COMPOUND_TYPE {$$=TOK;}
	| AB_BIL_COLOR {$$=TOK;}
	| AB_BIL_COLOR_CHOOSER {$$=TOK;}
	| AB_BIL_CONNECTION {$$=TOK;}
	| AB_BIL_CONNECTION_LIST {$$=TOK;}
	| AB_BIL_CONTAINER {$$=TOK;}
	| AB_BIL_CONTAINER_TYPE {$$=TOK;}
	| AB_BIL_COMBO_BOX {$$=TOK;}
	| AB_BIL_COPY {$$=TOK;}
	| AB_BIL_DECIMAL_POINTS {$$=TOK;}
	| AB_BIL_DEFAULT {$$=TOK;}
	| AB_BIL_DEFAULT_BUTTON {$$=TOK;}
	| AB_BIL_DEFINED_STRING {$$=TOK;}
	| AB_BIL_DESTROYED {$$=TOK;}
	| AB_BIL_DIALOG {$$=TOK;}
	| AB_BIL_DIRECTORY {$$=TOK;}
	| AB_BIL_DISABLE {$$=TOK;}
	| AB_BIL_DOUBLE_CLICKED_ON {$$=TOK;}
	| AB_BIL_DOUBLE_LINE {$$=TOK;}
	| AB_BIL_DOUBLE_DASHED_LINE {$$=TOK;}
	| AB_BIL_DRAG_CURSOR {$$=TOK;}
	| AB_BIL_DRAG_CURSOR_MASK {$$=TOK;}
	| AB_BIL_DRAG_ENABLED {$$=TOK;}
	| AB_BIL_DRAG_OPS {$$=TOK;}
	| AB_BIL_DRAG_TO_ROOT_ALLOWED {$$=TOK;}
	| AB_BIL_DRAG_TYPES {$$=TOK;}
	| AB_BIL_DRAGGED {$$=TOK;}
	| AB_BIL_DRAGGED_FROM {$$=TOK;}
	| AB_BIL_DRAWAREA_HEIGHT {$$=TOK;}
	| AB_BIL_DRAWAREA_WIDTH {$$=TOK;}
	| AB_BIL_DRAWING_AREA {$$=TOK;}
	| AB_BIL_DRAWN_BUTTON {$$=TOK;}
	| AB_BIL_DROP_ENABLED {$$=TOK;}
	| AB_BIL_DROP_OPS
	| AB_BIL_DROP_SITE_CHILD_ALLOWED {$$=TOK;}
	| AB_BIL_DROP_TYPES
	| AB_BIL_DROPPED_ON {$$=TOK;}
	| AB_BIL_EAST {$$=TOK;}
	| AB_BIL_EAST_ATTACHMENT {$$=TOK;}
	| AB_BIL_ELEMENT {$$=TOK;}
	| AB_BIL_ENABLE {$$=TOK;}
	| AB_BIL_EQUAL {$$=TOK;}
	| AB_BIL_ERROR {$$=TOK;}
	| AB_BIL_ETCHED_IN {$$=TOK;}
	| AB_BIL_ETCHED_IN_DASH {$$=TOK;}
	| AB_BIL_ETCHED_OUT {$$=TOK;}
	| AB_BIL_ETCHED_OUT_DASH {$$=TOK;}
	| AB_BIL_EXCLUSIVE {$$=TOK;}
	| AB_BIL_EXECUTE_CODE {$$=TOK;}
	| AB_BIL_FALSE {$$=TOK;}
	| AB_BIL_FG_COLOR {$$=TOK;}
	| AB_BIL_FILE_TYPE_MASK {$$=TOK;}
	| AB_BIL_FILE_CHOOSER {$$=TOK;}
	| AB_BIL_FILENAME {$$=TOK;}
	| AB_BIL_FILES {$$=TOK;}
	| AB_BIL_FILTER_PATTERN {$$=TOK;}
	| AB_BIL_FLOAT {$$=TOK;}
	| AB_BIL_FONT {$$=TOK;}
	| AB_BIL_FONT_CHOOSER {$$=TOK;}
	| AB_BIL_FOOTER {$$=TOK;}
	| AB_BIL_FROM {$$=TOK;}
	| AB_BIL_GAUGE {$$=TOK;}
	| AB_BIL_GEOMETRY {$$=TOK;}
	| AB_BIL_GLYPH {$$=TOK;}
	| AB_BIL_GLYPH_FILE {$$=TOK;}
	| AB_BIL_GRAPHIC {$$=TOK;}
	| AB_BIL_GROUP {$$=TOK;}
	| AB_BIL_GROUP_TYPE {$$=TOK;}
	| AB_BIL_HAS_FOOTER {$$=TOK;}
	| AB_BIL_HATTACH_TYPE {$$=TOK;}
	| AB_BIL_HCENTER {$$=TOK;}
	| AB_BIL_HELP_BUTTON {$$=TOK;}
	| AB_BIL_HELP_LOCATION {$$=TOK;}
	| AB_BIL_HELP_TEXT {$$=TOK;}
	| AB_BIL_HELP_VOLUME {$$=TOK;}
	| AB_BIL_HEIGHT {$$=TOK;}
	| AB_BIL_HEIGHT_MAX {$$=TOK;}
	| AB_BIL_HEIGHT_RESIZABLE {$$=TOK;}
	| AB_BIL_HIDE {$$=TOK;}
	| AB_BIL_HSCROLL {$$=TOK;}
	| AB_BIL_HSCROLLBAR {$$=TOK;}
	| AB_BIL_HSPACING {$$=TOK;}
	| AB_BIL_HOFFSET {$$=TOK;}
	| AB_BIL_HORIZONTAL {$$=TOK;}
	| AB_BIL_I18N {$$=TOK;}
	| AB_BIL_I18N_ENABLED {$$=TOK;}
	| AB_BIL_ICON {$$=TOK;}
	| AB_BIL_ICONIC {$$=TOK;}
	| AB_BIL_ICON_FILE {$$=TOK;}
	| AB_BIL_ICON_LABEL {$$=TOK;}
	| AB_BIL_ICON_MASK_FILE {$$=TOK;}
	| AB_BIL_IGNORE {$$=TOK;}
	| AB_BIL_INACTIVE {$$=TOK;}
	| AB_BIL_INCREMENT {$$=TOK;}
	| AB_BIL_INFO {$$=TOK;}
	| AB_BIL_INITIAL_STATE {$$=TOK;}
	| AB_BIL_INT {$$=TOK;}
	| AB_BIL_INVISIBLE {$$=TOK;}
	| AB_BIL_ITEM {$$=TOK;}
	| AB_BIL_ITEM_FOR_CHOICE {$$=TOK;}
	| AB_BIL_ITEM_FOR_COMBO_BOX {$$=TOK;}
	| AB_BIL_ITEM_FOR_LIST {$$=TOK;}
	| AB_BIL_ITEM_FOR_MENU {$$=TOK;}
	| AB_BIL_ITEM_FOR_MENUBAR {$$=TOK;}
	| AB_BIL_ITEM_FOR_SPIN_BOX {$$=TOK;}
	| AB_BIL_ITEM_SELECTED {$$=TOK;}
	| AB_BIL_ITEM_TYPE {$$=TOK;}
	| AB_BIL_LABEL {$$=TOK;}
	| AB_BIL_LABEL_ALIGNMENT {$$=TOK;}
	| AB_BIL_LABEL_POSITION {$$=TOK;}
	| AB_BIL_LABELS {$$=TOK;}
	| AB_BIL_LABEL_TYPE {$$=TOK;}
	| AB_BIL_LABEL_STRING {$$=TOK;}
	| AB_BIL_LABEL_STYLE {$$=TOK;}
	| AB_BIL_LAYERS {$$=TOK;}
	| AB_BIL_LEFT {$$=TOK;}
	| AB_BIL_LEFT_TO_RIGHT {$$=TOK;}
	| AB_BIL_LINK {$$=TOK;}
	| AB_BIL_LIST {$$=TOK;}
	| AB_BIL_LITERAL {$$=TOK;}
	| AB_BIL_LOAD {$$=TOK;}
	| AB_BIL_MAIN_WINDOW {$$=TOK;}
	| AB_BIL_MAX_LABEL {$$=TOK;}
	| AB_BIL_MAX_VALUE {$$=TOK;}
	| AB_BIL_MENU {$$=TOK;}
	| AB_BIL_MENU_BAR {$$=TOK;}
	| AB_BIL_MENU_BUTTON {$$=TOK;}
	| AB_BIL_MENU_TITLE {$$=TOK;}
	| AB_BIL_MENU_TYPE {$$=TOK;}
	| AB_BIL_MESSAGE {$$=TOK;}
	| AB_BIL_MESSAGE_TYPE {$$=TOK;}
	| AB_BIL_MIN_LABEL {$$=TOK;}
	| AB_BIL_MIN_VALUE {$$=TOK;}
	| AB_BIL_MODULE {$$=TOK;}
	| AB_BIL_MNEMONIC {$$=TOK;}
	| AB_BIL_MOVE {$$=TOK;}
	| AB_BIL_MULTIPLE {$$=TOK;}
	| AB_BIL_MULTIPLE_SELECTIONS {$$=TOK;}
	| AB_BIL_NIL {$$=TOK;}
	| AB_BIL_NONEXCLUSIVE {$$=TOK;}
	| AB_BIL_NOTSELECTED {$$=TOK;}
	| AB_BIL_NAME {$$=TOK;}
	| AB_BIL_NEVER {$$=TOK;}
	| AB_BIL_NONE {$$=TOK;}
	| AB_BIL_NORTH {$$=TOK;}
	| AB_BIL_NORTHEAST {$$=TOK;}
	| AB_BIL_NORTHWEST {$$=TOK;}
	| AB_BIL_NORTH_ATTACHMENT {$$=TOK;}
	| AB_BIL_NUM_COLUMNS {$$=TOK;}
	| AB_BIL_NUM_ROWS {$$=TOK;}
	| AB_BIL_NUMERIC {$$=TOK;}
	| AB_BIL_OK {$$=TOK;}
	| AB_BIL_OK_BUTTON {$$=TOK;}
	| AB_BIL_OK_LABEL {$$=TOK;}
	| AB_BIL_ON_ITEM_HELP {$$=TOK;}
	| AB_BIL_OPEN {$$=TOK;}
	| AB_BIL_OPTION {$$=TOK;}
	| AB_BIL_OPTION_MENU {$$=TOK;}
	| AB_BIL_ORIENTATION {$$=TOK;}
	| AB_BIL_OTHER {$$=TOK;}
	| AB_BIL_OTHER_STRING {$$=TOK;}
	| AB_BIL_PACKING {$$=TOK;}
	| AB_BIL_PANED {$$=TOK;}
	| AB_BIL_PANE_MAX_HEIGHT {$$=TOK;}
	| AB_BIL_PANE_MIN_HEIGHT {$$=TOK;}
	| AB_BIL_POPPED_DOWN {$$=TOK;}
	| AB_BIL_POPPED_UP {$$=TOK;}
	| AB_BIL_WIN_CHILDREN {$$=TOK;}
	| AB_BIL_WIN_PARENT {$$=TOK;}
	| AB_BIL_PARAMETER_TYPE {$$=TOK;}
	| AB_BIL_PIXEL {$$=TOK;}
	| AB_BIL_PIXMAP {$$=TOK;}
        | AB_BIL_PROCESS_STRING {$$=TOK;}
	| AB_BIL_PROJECT {$$=TOK;}
	| AB_BIL_PULLDOWN {$$=TOK;}
	| AB_BIL_PUSH_BUTTON {$$=TOK;}
	| AB_BIL_QUESTION {$$=TOK;}
	| AB_BIL_READ_ONLY {$$=TOK;}
	| AB_BIL_REFERENCE_POINT {$$=TOK;}
	| AB_BIL_REGULAR {$$=TOK;}
	| AB_BIL_RELATIVE {$$=TOK;}
	| AB_BIL_REPAINT_NEEDED {$$=TOK;}
	| AB_BIL_RESIZABLE {$$=TOK;}
	| AB_BIL_RESIZED {$$=TOK;}
	| AB_BIL_RETURN_TYPE {$$=TOK;}
	| AB_BIL_RES_FILE_TYPES {$$=TOK;}
	| AB_BIL_RESOURCE {$$=TOK;}
	| AB_BIL_RIGHT {$$=TOK;}
	| AB_BIL_RIGHT_TO_LEFT {$$=TOK;}
	| AB_BIL_ROOT_WINDOW {$$=TOK;}
	| AB_BIL_ROW {$$=TOK;}
	| AB_BIL_ROWSCOLUMNS {$$=TOK;}
	| AB_BIL_ROW_ALIGNMENT {$$=TOK;}
	| AB_BIL_ROW_COLUMN {$$=TOK;}
	| AB_BIL_SAVE {$$=TOK;}
	| AB_BIL_SAVE_AS {$$=TOK;}
	| AB_BIL_SB_ALWAYS_VISIBLE {$$=TOK;}
        | AB_BIL_SCALE {$$=TOK;}
	| AB_BIL_SCROLLED_WINDOW {$$=TOK;}
	| AB_BIL_SELECT {$$=TOK;}
	| AB_BIL_SELECTED {$$=TOK;}
	| AB_BIL_SELECTION_BOX {$$=TOK;}
	| AB_BIL_SELECTION_REQUIRED {$$=TOK;}
	| AB_BIL_SEPARATOR {$$=TOK;}
	| AB_BIL_SESSION_MGMT {$$=TOK;}
	| AB_BIL_SESSIONING_CMDLINE {$$=TOK;}
	| AB_BIL_SESSIONING_CMDLINE_AND_FILE {$$=TOK;}
	| AB_BIL_SESSIONING_FILE {$$=TOK;}
	| AB_BIL_SESSIONING_METHOD {$$=TOK;}
	| AB_BIL_SESSION_RESTORE {$$=TOK;}
	| AB_BIL_SESSION_SAVE {$$=TOK;}
	| AB_BIL_SET_LABEL {$$=TOK;}
	| AB_BIL_SET_TEXT {$$=TOK;}
	| AB_BIL_SET_VALUE {$$=TOK;}
	| AB_BIL_SHADOW_IN {$$=TOK;}
	| AB_BIL_SHADOW_OUT {$$=TOK;}
	| AB_BIL_SHOW {$$=TOK;}
	| AB_BIL_SHOW_VALUE {$$=TOK;}
	| AB_BIL_SINGLE {$$=TOK;}
	| AB_BIL_SINGLE_LINE {$$=TOK;}
	| AB_BIL_SINGLE_DASHED_LINE {$$=TOK;}
	| AB_BIL_SLIDER {$$=TOK;}
	| AB_BIL_SLIDER_TYPE {$$=TOK;}
	| AB_BIL_SOUTHEAST {$$=TOK;}
	| AB_BIL_SOUTH {$$=TOK;}
	| AB_BIL_SOUTHWEST {$$=TOK;}
	| AB_BIL_SOUTH_ATTACHMENT {$$=TOK;}
	| AB_BIL_SPIN_BOX {$$=TOK;}
	| AB_BIL_STANDARD {$$=TOK;}
	| AB_BIL_STRING {$$=TOK;}
	| AB_BIL_MAX_LENGTH {$$=TOK;}
	| AB_BIL_TEAR_OFF {$$=TOK;}
	| AB_BIL_TERM_PANE {$$=TOK;}
	| AB_BIL_TEXT {$$=TOK;}
	| AB_BIL_TEXT_CHANGED {$$=TOK;}
	| AB_BIL_TEXT_FIELD {$$=TOK;}
	| AB_BIL_TEXT_PANE {$$=TOK;}
	| AB_BIL_TEXT_TYPE {$$=TOK;}
	| AB_BIL_TIGHT {$$=TOK;}
	| AB_BIL_TITLE {$$=TOK;}
	| AB_BIL_TITLE_TYPE {$$=TOK;}
	| AB_BIL_TO {$$=TOK;}
	| AB_BIL_TOGGLED {$$=TOK;}
	| AB_BIL_TOOL_BAR {$$=TOK;}
	| AB_BIL_TOOLTALK {$$=TOK;}
	| AB_BIL_TOOLTALK_DESKTOP_LEVEL {$$=TOK;}
	| AB_BIL_TOOLTALK_DO_COMMAND {$$=TOK;}
	| AB_BIL_TOOLTALK_GET_STATUS {$$=TOK;}
	| AB_BIL_TOOLTALK_PAUSE_RESUME {$$=TOK;}
	| AB_BIL_TOOLTALK_QUIT {$$=TOK;}
	| AB_BIL_TOP {$$=TOK;}
	| AB_BIL_TOP_TO_BOTTOM {$$=TOK;}
	| AB_BIL_TRUE {$$=TOK;}
	| AB_BIL_TYPE {$$=TOK;}
	| AB_BIL_USER_DATA {$$=TOK;}
	| AB_BIL_USER_DEF {$$=TOK;}
	| AB_BIL_INITIAL_VALUE {$$=TOK;}
	| AB_BIL_VALUE {$$=TOK;}
	| AB_BIL_VALUE_CHANGED {$$=TOK;}
	| AB_BIL_VALUE_BOOL {$$=TOK;}
	| AB_BIL_VALUE_FLOAT {$$=TOK;}
	| AB_BIL_VALUE_IDENT {$$=TOK;}
	| AB_BIL_VALUE_INT {$$=TOK;}
	| AB_BIL_VALUE_STRING {$$=TOK;}
	| AB_BIL_VATTACH_TYPE {$$=TOK;}
	| AB_BIL_VCENTER {$$=TOK;}
	| AB_BIL_VENDOR {$$=TOK;}
	| AB_BIL_VERSION {$$=TOK;}
	| AB_BIL_VERTICAL {$$=TOK;}
	| AB_BIL_VISIBLE {$$=TOK;}
	| AB_BIL_VOFFSET {$$=TOK;}
	| AB_BIL_VOID {$$=TOK;}
	| AB_BIL_VSCROLL {$$=TOK;}
	| AB_BIL_VSCROLLBAR {$$=TOK;}
	| AB_BIL_VSPACING {$$=TOK;}
	| AB_BIL_WARNING {$$=TOK;}
	| AB_BIL_WEST {$$=TOK;}
	| AB_BIL_WEST_ATTACHMENT {$$=TOK;}
	| AB_BIL_WHEN {$$=TOK;}
	| AB_BIL_WHEN_NEEDED {$$=TOK;}
	| AB_BIL_WIDGET {$$=TOK;}
	| AB_BIL_WIDTH {$$=TOK;}
	| AB_BIL_WIDTH_MAX {$$=TOK;}
	| AB_BIL_WIDTH_RESIZABLE {$$=TOK;}
	| AB_BIL_WORD_WRAP {$$=TOK;}
	| AB_BIL_WORKING {$$=TOK;}
	| AB_BIL_X {$$=TOK;}
	| AB_BIL_XMSTRING {$$=TOK;}
	| AB_BIL_XMSTRING_TBL {$$=TOK;}
	| AB_BIL_Y {$$=TOK;}
	;

