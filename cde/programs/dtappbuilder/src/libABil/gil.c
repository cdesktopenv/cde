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
 *	$XConsortium: gil.c /main/3 1995/11/06 18:27:05 rswiston $
 */

/************************************************************
*                                                           *
* gil.c - GIL conversion routines                           *
*                                                           *
************************************************************/

#include <stdio.h>
#include <ab/util_types.h>
#include <ab_private/istr.h>
#include "gilP.h"

#define ARRAY_NENTS(array)	(sizeof(array)/sizeof(array[0]))


/*************************************************************************
**									**
**		Type/String Conversion					**
**									**
**************************************************************************/

static int
convert_string_to_int(STRING s_value,
		      ISTRING * table, int num_values, int default_value)
{
    int                 i;
    int                 int_value = default_value;
    ISTRING             is_value = istr_create(s_value);
    if (s_value == NULL)
    {
	istr_destroy(is_value);
	return int_value;
    }
    for (i = 0; i < num_values; ++i)
    {
	if (istr_equal(table[i], is_value))
	{
	    int_value = i;
	    break;
	}
    }
    istr_destroy(is_value);
    return int_value;
}

static int
convert_fill_table(ISTRING * table, int num_values, ISTRING value)
{
    int                 i;
    for (i = 0; i < num_values; ++i)
    {
	table[i] = value;
    }
    return 0;
}



/******************* AB_ALIGNMENT **************************************/

static ISTRING      align_tab[AB_ALIGNMENT_NUM_VALUES];
static BOOL         align_tab_inited = FALSE;
#define align_tab_check_init() (align_tab_inited? 0:align_tab_init())

static int
align_tab_init(void)
{
#define at align_tab
    align_tab_inited = TRUE;
    convert_fill_table(at, ARRAY_NENTS(at), istr_const(NULL));

    /* sorted on string values */
    at[AB_ALIGN_BOTTOM] = istr_const(":bottom-edges");
    at[AB_ALIGN_CENTER] = istr_const(":centers");
    at[AB_ALIGN_HCENTER] = istr_const(":horizontal-centers");
    at[AB_ALIGN_LABELS] = istr_const(":labels");
    at[AB_ALIGN_LEFT] = istr_const(":left-edges");
    at[AB_ALIGN_RIGHT] = istr_const(":right-edges");
    at[AB_ALIGN_TOP] = istr_const(":top-edges");
    at[AB_ALIGN_VCENTER] = istr_const(":vertical-centers");

    return 0;
#undef at
}

AB_ALIGNMENT
gilP_string_to_alignment(STRING align_string)
{
    return (AB_ALIGNMENT) convert_string_to_int(align_string,
				     align_tab, ARRAY_NENTS(align_tab), -1);
}

/******************* AB_ARG_TYPE **************************************/
static ISTRING      arg_type_tab[AB_ARG_TYPE_NUM_VALUES];
static BOOL         arg_type_tab_inited = FALSE;
#define arg_type_tab_check_init() (arg_type_tab_inited? 0:arg_type_tab_init())

static int
arg_type_tab_init(void)
{
#define att arg_type_tab
    arg_type_tab_inited = TRUE;

    att[AB_ARG_BOOLEAN] = istr_const(":boolean");
    att[AB_ARG_CALLBACK] = istr_const(":callback");
    att[AB_ARG_FLOAT] = istr_const(":float");
    att[AB_ARG_INT] = istr_const(":integer");
    att[AB_ARG_LITERAL] = istr_const(":literal");
    att[AB_ARG_NONE] = istr_const(":void");
    att[AB_ARG_STRING] = istr_const(":string");

    return 0;
#undef att
}

AB_ARG_TYPE
gilP_string_to_arg_type(STRING arg_type_string)
{
    return (AB_ARG_TYPE) convert_string_to_int(arg_type_string,
			       arg_type_tab, ARRAY_NENTS(arg_type_tab), -1);
}

/******************* AB_GIL_ATTRIBUTE **************************************/
static ISTRING      gat[AB_GIL_ATTRIBUTE_NUM_VALUES];	/* gil attribute table */
static BOOL         gat_inited = FALSE;
#define gat_check_init() (gat_inited? 0:gat_init())

static int
gat_init(void)
{
    convert_fill_table(gat, AB_GIL_ATTRIBUTE_NUM_VALUES, istr_const(NULL));

    gat[AB_GIL_ABBREVIATED] = istr_const(":abbreviated");
    gat[AB_GIL_ACTION] = istr_const(":action");
    gat[AB_GIL_ACTIONS] = istr_const(":actions");
    gat[AB_GIL_ANCHOR_OBJECT] = istr_const(":anchor-object");
    gat[AB_GIL_ANCHOR_POINT] = istr_const(":anchor-point");
    gat[AB_GIL_ARG_TYPE] = istr_const(":arg_type");
    gat[AB_GIL_BACKGROUND_COLOR] = istr_const(":background-color");
    gat[AB_GIL_BUSY_DROP_GLYPH] = istr_const(":busy-drop-glyph");
    gat[AB_GIL_BUTTON_TYPE] = istr_const(":button-type");
    gat[AB_GIL_CHOICES] = istr_const(":choices");
    gat[AB_GIL_CHOICE_DEFAULTS] = istr_const(":choice-defaults");
    gat[AB_GIL_CHOICE_LABEL_TYPES] = istr_const(":choice-label-types");
    gat[AB_GIL_CHOICE_COLORS] = istr_const(":choice-colors");
    gat[AB_GIL_COLUMNS] = istr_const(":columns");
    gat[AB_GIL_COL_ALIGNMENT] = istr_const(":column-alignment");
    gat[AB_GIL_CONNECTIONS] = istr_const(":connections");
    gat[AB_GIL_CONSTANT_WIDTH] = istr_const(":constant-width");
    gat[AB_GIL_DEFAULT_DROP_SITE] = istr_const(":default-drop-site");
    gat[AB_GIL_DND_ACCEPT_CURSOR] = istr_const(":dnd-accept-cursor");
    gat[AB_GIL_DND_ACCEPT_CURSOR_XHOT] = istr_const(":dnd-accept-cursor-xhot");
    gat[AB_GIL_DND_ACCEPT_CURSOR_YHOT] = istr_const(":dnd-accept-cursor-yhot");
    gat[AB_GIL_DND_CURSOR] = istr_const(":dnd-cursor");
    gat[AB_GIL_DND_CURSOR_XHOT] = istr_const(":dnd-cursor-xhot");
    gat[AB_GIL_DND_CURSOR_YHOT] = istr_const(":dnd-cursor-yhot");
    gat[AB_GIL_DRAGGABLE] = istr_const(":draggable");
    gat[AB_GIL_DONE_HANDLER] = istr_const(":done-handler");
    gat[AB_GIL_DROPPABLE] = istr_const(":droppable");
    gat[AB_GIL_DROP_TARGET_WIDTH] = istr_const(":drop-target_width");
    gat[AB_GIL_DRAWING_MODEL] = istr_const(":drawing-model");
    gat[AB_GIL_EVENTS] = istr_const(":events");
    gat[AB_GIL_EVENT_HANDLER] = istr_const(":event-handler");
    gat[AB_GIL_FILE_CHOOSER_FILTER_PATTERN] =
	istr_const(":file-chooser-filter-pattern");
    gat[AB_GIL_FILE_CHOOSER_MATCH_GLYPH] =
	istr_const(":file-chooser-match-glyph");
    gat[AB_GIL_FILE_CHOOSER_MATCH_GLYPH_MASK] =
	istr_const(":file-chooser-match-glyph-mask");
    gat[AB_GIL_FILE_CHOOSER_TYPE] = istr_const(":file-chooser-type");
    gat[AB_GIL_FOREGROUND_COLOR] = istr_const(":foreground-color");
    gat[AB_GIL_FROM] = istr_const(":from");
    gat[AB_GIL_FUNCTION_TYPE] = istr_const(":function_type");
    gat[AB_GIL_GROUP_TYPE] = istr_const(":group-type");
    gat[AB_GIL_HEIGHT] = istr_const(":height");
    gat[AB_GIL_HELP] = istr_const(":help");
    gat[AB_GIL_HOFFSET] = istr_const(":horizontal-offset");
    gat[AB_GIL_HSCROLL] = istr_const(":horizontal-scrollbar");
    gat[AB_GIL_HSPACING] = istr_const(":horizontal-spacing");
    gat[AB_GIL_ICON_FILE] = istr_const(":icon-file");
    gat[AB_GIL_ICON_LABEL] = istr_const(":icon-label");
    gat[AB_GIL_ICON_MASK_FILE] = istr_const(":icon-mask-file");
    gat[AB_GIL_INITIAL_LIST_GLYPHS] = istr_const(":initial-list-glyphs");
    gat[AB_GIL_INITIAL_LIST_VALUES] = istr_const(":initial-list-values");
    gat[AB_GIL_INITIAL_SELECTIONS] = istr_const(":initial-selections");
    gat[AB_GIL_INITIAL_STATE] = istr_const(":initial-state");
    gat[AB_GIL_INITIAL_VALUE] = istr_const(":initial-value");
    gat[AB_GIL_INTERFACES] = istr_const(":interfaces");
    gat[AB_GIL_INTERNATIONAL_DB_BEGIN] = istr_const(":international-db-begin");
    gat[AB_GIL_INTERNATIONAL_DB_END] = istr_const(":international-db-end");
    gat[AB_GIL_LABEL] = istr_const(":label");
    gat[AB_GIL_LABEL_TYPE] = istr_const(":label-type");
    gat[AB_GIL_LABEL_BOLD] = istr_const(":label-bold");
    gat[AB_GIL_LAYOUT_TYPE] = istr_const(":layout-type");
    gat[AB_GIL_MAPPED] = istr_const(":mapped");
    gat[AB_GIL_MAX_TICK_STRING] = istr_const(":max-tick-string");
    gat[AB_GIL_MAX_VALUE] = istr_const(":max-value");
    gat[AB_GIL_MAX_VALUE_STRING] = istr_const(":max-value-string");
    gat[AB_GIL_MEMBERS] = istr_const(":members");
    gat[AB_GIL_MENU] = istr_const(":menu");
    gat[AB_GIL_MENU_HANDLER] = istr_const(":menu-handler");
    gat[AB_GIL_MENU_ITEM_ACCELERATORS] = istr_const(":menu-item-accelerators");
    gat[AB_GIL_MENU_ITEM_COLORS] = istr_const(":menu-item-colors");
    gat[AB_GIL_MENU_ITEM_DEFAULTS] = istr_const(":menu-item-defaults");
    gat[AB_GIL_MENU_ITEM_HANDLERS] = istr_const(":menu-item-handlers");
    gat[AB_GIL_MENU_ITEM_LABELS] = istr_const(":menu-item-labels");
    gat[AB_GIL_MENU_ITEM_LABEL_TYPES] = istr_const(":menu-item-label-types");
    gat[AB_GIL_MENU_ITEM_MENUS] = istr_const(":menu-item-menus");
    gat[AB_GIL_MENU_ITEM_STATES] = istr_const(":menu-item-states");
    gat[AB_GIL_MENU_TITLE] = istr_const(":menu-title");
    gat[AB_GIL_MENU_TYPE] = istr_const(":menu-type");
    gat[AB_GIL_MIN_TICK_STRING] = istr_const(":min-tick-string");
    gat[AB_GIL_MIN_VALUE] = istr_const(":min-value");
    gat[AB_GIL_MIN_VALUE_STRING] = istr_const(":min-value-string");
    gat[AB_GIL_MULTIPLE_SELECTIONS] = istr_const(":multiple-selections");
    gat[AB_GIL_NAME] = istr_const(":name");
    gat[AB_GIL_NORMAL] = istr_const(":normal");
    gat[AB_GIL_NORMAL_DROP_GLYPH] = istr_const(":normal-drop-glyph");
    gat[AB_GIL_ACTIVATE_HANDLER] = istr_const(":notify-handler");
    gat[AB_GIL_ORIENTATION] = istr_const(":orientation");
    gat[AB_GIL_OWNER] = istr_const(":owner");
    gat[AB_GIL_PINNABLE] = istr_const(":pinnable");
    gat[AB_GIL_PINNED] = istr_const(":pinned");
    gat[AB_GIL_READ_ONLY] = istr_const(":read-only");
    gat[AB_GIL_REFERENCE_POINT] = istr_const(":reference-point");
    gat[AB_GIL_REPAINT_PROC] = istr_const(":repaint-proc");
    gat[AB_GIL_RESIZABLE] = istr_const(":resizable");
    gat[AB_GIL_ATT_ROOT_WINDOW] = istr_const(":root_window");
    gat[AB_GIL_ROWS] = istr_const(":rows");
    gat[AB_GIL_ROW_ALIGNMENT] = istr_const(":row-alignment");
    gat[AB_GIL_SCROLLABLE_HEIGHT] = istr_const(":scrollable-height");
    gat[AB_GIL_SCROLLABLE_WIDTH] = istr_const(":scrollable-width");
    gat[AB_GIL_SELECTION_REQUIRED] = istr_const(":selection-required");
    gat[AB_GIL_SETTING_TYPE] = istr_const(":setting-type");
    gat[AB_GIL_SHOW_BORDER] = istr_const(":show-border");
    gat[AB_GIL_SHOW_ENDBOXES] = istr_const(":show-endboxes");
    gat[AB_GIL_SHOW_FOOTER] = istr_const(":show-footer");
    gat[AB_GIL_SHOW_RANGE] = istr_const(":show-range");
    gat[AB_GIL_SHOW_VALUE] = istr_const(":show-value");
    gat[AB_GIL_SCALE_WIDTH] = istr_const(":slider-width");
    gat[AB_GIL_STORED_LENGTH] = istr_const(":stored-length");
    gat[AB_GIL_MAX_LENGTH] = istr_const(":max-length");
    gat[AB_GIL_TEXT_TYPE] = istr_const(":text-type");
    gat[AB_GIL_TICKS] = istr_const(":ticks");
    gat[AB_GIL_TITLE] = istr_const(":title");
    gat[AB_GIL_TO] = istr_const(":to");
    gat[AB_GIL_TYPE] = istr_const(":type");
    gat[AB_GIL_USER_DATA] = istr_const(":user-data");
    gat[AB_GIL_VALUE_LENGTH] = istr_const(":value-length");
    gat[AB_GIL_VALUE_UNDERLINED] = istr_const(":value-underlined");
    gat[AB_GIL_VALUE_X] = istr_const(":value-x");
    gat[AB_GIL_VALUE_Y] = istr_const(":value-y");
    gat[AB_GIL_VOFFSET] = istr_const(":vertical-offset");
    gat[AB_GIL_VSCROLL] = istr_const(":vertical-scrollbar");
    gat[AB_GIL_VSPACING] = istr_const(":vertical-spacing");
    gat[AB_GIL_WHEN] = istr_const(":when");
    gat[AB_GIL_WIDTH] = istr_const(":width");
    gat[AB_GIL_X] = istr_const(":x");
    gat[AB_GIL_Y] = istr_const(":y");

    gat_inited = TRUE;
    return 0;
}

AB_GIL_ATTRIBUTE
gilP_string_to_att(STRING s_att)
{
    AB_GIL_ATTRIBUTE    att = AB_GIL_UNDEF;
    ISTRING             istr_att = istr_create(s_att);


    att = (AB_GIL_ATTRIBUTE) convert_string_to_int(
						   istr_string(istr_att),
			    gat, AB_GIL_ATTRIBUTE_NUM_VALUES, AB_GIL_UNDEF);

    istr_destroy(istr_att);
    return att;
}

/*************************************************************************
**									**
**		AB_BUILTIN_ACTION					**
**									**
**************************************************************************/

static BOOL         builtin_action_table_inited = FALSE;
static ISTRING      builtin_action_table[AB_BUILTIN_ACTION_NUM_VALUES] = {NULL};

#define check_builtin_action_table_init() \
	(builtin_action_table_inited? 0:builtin_action_table_init())

static int
builtin_action_table_init(void)
{
#define bat builtin_action_table
    builtin_action_table_inited = TRUE;
    convert_fill_table(builtin_action_table,
		       AB_BUILTIN_ACTION_NUM_VALUES,
		       istr_const("?when?"));

    bat[AB_STDACT_UNDEF] = istr_const("Undef");
    bat[AB_STDACT_DISABLE] = istr_const("Disable");
    bat[AB_STDACT_ENABLE] = istr_const("Enable");
    /* bat[AB_STDACT_GET_LABEL]=	 istr_const("GetLabel"); */
    /* bat[AB_STDACT_GET_LEFT_FOOTER]=  istr_const("GetLeftFooter"); */

    /*
     * bat[AB_STDACT_GET_RIGHT_FOOTER]= istr_const("GetRightFooter");*?
     * bat[AB_STDACT_GET_VALUE_NUMBER]= istr_const("GetValueNumber");
     */
    /* bat[AB_STDACT_GET_VALUE_STRING]= istr_const("GetValueString"); */
    bat[AB_STDACT_HIDE] = istr_const("Hide");
    /* bat[AB_STDACT_LOAD_TEXT_FILE]=	 istr_const("LoadTextFile"); */
    bat[AB_STDACT_SET_LABEL] = istr_const("SetLabel");
    bat[AB_STDACT_SHOW] = istr_const("Show");
    /* bat[AB_STDACT_SET_LEFT_FOOTER]=  istr_const("SetLeftFooter"); */
    /* bat[AB_STDACT_SET_RIGHT_FOOTER]= istr_const("SetRightFooter"); */
    /* bat[AB_STDACT_SET_SELECTED]=	 istr_const("SetSelected"); */
    bat[AB_STDACT_SET_VALUE] = istr_const("SetValueNumber");

    /*
     * Must handle this another way - can't have dup. entries
     */
    /* bat[AB_STDACT_SET_VALUE]= istr_const("SetValueString"); */

    return 0;
#undef bat
}


AB_BUILTIN_ACTION
gilP_string_to_builtin_action(STRING action)
{
    AB_BUILTIN_ACTION   act = AB_STDACT_UNDEF;

    act = (AB_BUILTIN_ACTION) 
        convert_string_to_int(action,
			 builtin_action_table, AB_BUILTIN_ACTION_NUM_VALUES,
					      AB_STDACT_UNDEF);
    if ((act == AB_STDACT_UNDEF) && util_streq(action, "SetValueString"))
    {
	act = AB_STDACT_SET_VALUE;
    }

    return act;
}


/******************* AB_BUTTON_TYPE **************************************/
static ISTRING      button_type_tab[AB_BUTTON_TYPE_NUM_VALUES];
static BOOL         button_type_tab_inited = FALSE;
#define button_type_tab_check_init() \
		(button_type_tab_inited? 0:button_type_tab_init())

static int
button_type_tab_init(void)
{
#define btt button_type_tab
    button_type_tab_inited = TRUE;

    btt[AB_BUT_PUSH] = istr_const(":normal");

    return 0;
#undef btt
}

AB_BUTTON_TYPE
gilP_string_to_button_type(STRING type_string)
{
    return (AB_BUTTON_TYPE) convert_string_to_int(type_string,
			 button_type_tab, ARRAY_NENTS(button_type_tab), -1);
}

/******************* AB_CHOICE_TYPE **************************************/
AB_CHOICE_TYPE
gilP_string_to_choice_type(STRING s_choice_type)
{
    return AB_CHOICE_EXCLUSIVE;
}


/******************* AB_COMPASS_POINT *********************************/
static ISTRING      compass_tab[AB_COMPASS_POINT_NUM_VALUES];
static BOOL         compass_tab_inited = FALSE;
#define cp_tab_check_init() (compass_tab_inited? 0:compass_tab_init())

static int
compass_tab_init(void)
{
#define cpt compass_tab
    compass_tab_inited = TRUE;
    convert_fill_table(cpt, ARRAY_NENTS(cpt), istr_const(NULL));

    /* sorted in clockwise order */
    cpt[AB_CP_NORTH] = istr_const(":north");
    cpt[AB_CP_NORTHEAST] = istr_const(":north-east");
    cpt[AB_CP_EAST] = istr_const(":east");
    cpt[AB_CP_SOUTHEAST] = istr_const(":south-east");
    cpt[AB_CP_SOUTH] = istr_const(":south");
    cpt[AB_CP_SOUTHWEST] = istr_const(":south-west");
    cpt[AB_CP_WEST] = istr_const(":west");
    cpt[AB_CP_NORTHWEST] = istr_const(":north-west");
    cpt[AB_CP_CENTER] = istr_const(":center");

    return 0;
#undef at
}

AB_COMPASS_POINT
gilP_string_to_compass_point(STRING compass_str)
{
    return (AB_COMPASS_POINT) convert_string_to_int(compass_str,
				    compass_tab, ARRAY_NENTS(compass_tab), -1);
}


/******************* AB_FILE_CHOOSER_TYPE *********************************/

/*
 * CDE app builder doesn't really distinguish the file chooser types 
 */

int
gilP_string_to_file_chooser_type(STRING s_type)
{
    return 0;
}


/******************* AB_GROUP_TYPE **************************************/
AB_GROUP_TYPE
gilP_string_to_group_type(STRING s_group)
{
    AB_GROUP_TYPE	groupType = AB_GROUP_UNDEF;


    if (util_streq(s_group, ":column"))
    {
	groupType = AB_GROUP_COLUMNS;
    }
    else if (util_streq(s_group, ":row"))
    {
	groupType = AB_GROUP_ROWS;
    }
    else if (util_streq(s_group, ":row-column"))
    {
	groupType = AB_GROUP_ROWSCOLUMNS;
    }
    else if (util_streq(s_group, "nil"))
    {
	groupType = AB_GROUP_IGNORE;
    }

    return groupType;
}


/******************* AB_LABEL_TYPE **************************************/
static ISTRING      label_type_tab[AB_LABEL_TYPE_NUM_VALUES];
static BOOL         label_type_tab_inited = FALSE;
#define label_type_tab_check_init() \
		(label_type_tab_inited? 0:label_type_tab_init())

static int
label_type_tab_init(void)
{
#define ltt label_type_tab
    label_type_tab_inited = TRUE;

    ltt[AB_LABEL_ARROW_DOWN] = istr_const(":arrow-down");
    ltt[AB_LABEL_ARROW_LEFT] = istr_const(":arrow-left");
    ltt[AB_LABEL_ARROW_RIGHT] = istr_const(":arrow-right");
    ltt[AB_LABEL_ARROW_UP] = istr_const(":arrow_up");
    ltt[AB_LABEL_DRAWN] = istr_const(":drawn");
    ltt[AB_LABEL_GLYPH] = istr_const(":glyph");
    ltt[AB_LABEL_STRING] = istr_const(":string");

    return 0;
#undef ltt
}

AB_LABEL_TYPE
gilP_string_to_label_type(STRING type_string)
{
    return (AB_LABEL_TYPE) convert_string_to_int(type_string,
			   label_type_tab, ARRAY_NENTS(label_type_tab), -1);
}

/******************* AB_MENU_TYPE **************************************/
AB_MENU_TYPE
gilP_string_to_menu_type(STRING s_menu_type)
{
    AB_MENU_TYPE	menuType = AB_MENU_UNDEF;

    if (util_streq(s_menu_type, ":command"))
    {
	menuType = AB_MENU_PULLDOWN;
    }
    else if (util_streq(s_menu_type, ":exclusive"))
    {
	menuType = AB_MENU_OPTION;
    }

    return menuType;
}


/******************* AB_OBJECT_STATE **************************************/

AB_OBJECT_STATE
gilP_string_to_object_state(STRING s_state)
{
    AB_OBJECT_STATE	state = AB_STATE_UNDEF;

    if (util_streq(s_state, ":active"))
    {
	state = AB_STATE_ACTIVE;
    }
    else if (util_streq(s_state, ":iconic"))
    {
	state = AB_STATE_ICONIC;
    }
    else if (util_streq(s_state, ":inactive"))
    {
	state = AB_STATE_INACTIVE;
    }
    else if (util_streq(s_state, ":invisible"))
    {
	state = AB_STATE_INVISIBLE;
    }
    else if (util_streq(s_state, ":notselected"))
    {
	state = AB_STATE_NOTSELECTED;
    }
    else if (util_streq(s_state, ":open"))
    {
	state = AB_STATE_OPEN;
    }
    else if (util_streq(s_state, ":selected"))
    {
	state = AB_STATE_SELECTED;
    }
    else if (util_streq(s_state, ":visible"))
    {
	state = AB_STATE_VISIBLE;
    }

    return state;
}

/*************************************************************************
**									**
**		AB_OBJECT_TYPE						**
**									**
**************************************************************************/

/*
 * REMIND: move type conversion stuff to guide
 */
static BOOL         obj_type_table_inited = FALSE;
static ISTRING      obj_type_table[AB_OBJECT_TYPE_NUM_VALUES + 1] = {NULL};

#define check_obj_type_table_init() \
	(obj_type_table_inited? 0:obj_type_table_init())

static int
obj_type_table_init(void)
{
#define ott obj_type_table
    obj_type_table_inited = TRUE;
    convert_fill_table(obj_type_table,
		       AB_OBJECT_TYPE_NUM_VALUES, istr_const("?type?"));

    /*
     * REMIND: what is type :stack? Is it a setting stack or layers?
     * REMIND: what about file chooser?
     */
    /* sorted by enum name */
    ott[AB_TYPE_BASE_WINDOW] = istr_const(":base-window");
    ott[AB_TYPE_BUTTON] = istr_const(":button");
    ott[AB_TYPE_CHOICE] = istr_const(":setting");
    /* ott[AB_TYPE_CHOICE]=	istr_const(":stack"); duplicate */
    ott[AB_TYPE_CONTAINER] = istr_const(":control-area");
    /*ott[AB_TYPE_CONTAINER] = istr_const(":group");*/
    ott[AB_TYPE_DIALOG] = istr_const(":popup-window");
    ott[AB_TYPE_DRAWING_AREA] = istr_const(":canvas-pane");
    ott[AB_TYPE_FILE_CHOOSER] = istr_const(":file-chooser");
    ott[AB_TYPE_LABEL] = istr_const(":drop-target");
    ott[AB_TYPE_LIST] = istr_const(":scrolling-list");
    ott[AB_TYPE_MENU] = istr_const(":menu");
    ott[AB_TYPE_SCALE] = istr_const(":gauge");
    /* ott[AB_TYPE_SCALE]=	istr_const(":slider"); duplicate */
    ott[AB_TYPE_TERM_PANE] = istr_const(":term-pane");
    ott[AB_TYPE_TEXT_FIELD] = istr_const(":text-field");
    /*ott[AB_TYPE_TEXT_FIELD] = istr_const(":message");*/
    ott[AB_TYPE_TEXT_PANE] = istr_const(":text-pane");

    return 0;
#undef ott
}

AB_OBJECT_TYPE
gilP_string_to_object_type(STRING string_type)
{
    AB_OBJECT_TYPE      objType = AB_TYPE_UNDEF;

    objType = (AB_OBJECT_TYPE) convert_string_to_int(
						     string_type,
		obj_type_table, AB_OBJECT_TYPE_NUM_VALUES, AB_TYPE_UNDEF);

    if (objType == AB_TYPE_UNDEF)
    {
	if (util_streq(string_type, ":stack"))
	{
	    objType = AB_TYPE_CHOICE;
	}
	else if (util_streq(string_type, ":group"))
	{
	    objType = AB_TYPE_CONTAINER;
	}
	else if (util_streq(string_type, ":message"))
	{
	    objType = AB_TYPE_TEXT_FIELD;
	}
	else if (util_streq(string_type, ":slider"))
	{
	    objType = AB_TYPE_SCALE;
	}
    }

    return objType;
}


/******************* AB_ORIENTATION **************************************/
AB_ORIENTATION
gilP_string_to_orientation(STRING s_orient)
{
    AB_ORIENTATION	orient = AB_ORIENT_UNDEF;

    if (util_streq(s_orient, ":horizontal"))
    {
	orient = AB_ORIENT_HORIZONTAL;
    }
    else if (util_streq(s_orient, ":vertical"))
    {
	orient = AB_ORIENT_VERTICAL;
    }

    return orient;
}


/******************* AB_TEXT_TYPE **************************************/
AB_TEXT_TYPE
gilP_string_to_text_type(STRING s_text_type)
{
    AB_TEXT_TYPE	textType = AB_TEXT_UNDEF;

    if (util_streq(s_text_type, ":alphanumeric"))
    {
	textType = AB_TEXT_ALPHANUMERIC;
    }
    else if (util_streq(s_text_type, ":multiline"))
    {
	textType = AB_TEXT_ALPHANUMERIC;
    }
    else if (util_streq(s_text_type, ":numeric"))
    {
	textType = AB_TEXT_NUMERIC;
    }

    return textType;
}

/*************************************************************************
**									**
**		AB_WHEN							**
**									**
**************************************************************************/

/*
 * REMIND: move when stuff to libguide
 */
static ISTRING      when_table[AB_WHEN_NUM_VALUES];
static BOOL         when_table_inited = FALSE;
#define check_when_table_init() \
		if (!when_table_inited) {when_table_init();}

static int
when_table_init(void)
{
#define wt when_table
    int                 i = 0;

    when_table_inited = TRUE;
    convert_fill_table(when_table, AB_WHEN_NUM_VALUES,
		       istr_const("?when?"));

    /* wt[AB_WHEN_ANY_EVENT]=		istr_const("AnyEvent"); */
    /* wt[AB_WHEN_BUTTON1_PRESS]=	istr_const("Button1Press"); */
    /* wt[AB_WHEN_BUTTON2_PRESS]=	istr_const("Button2Press"); */
    /* wt[AB_WHEN_BUTTON3_PRESS]=	istr_const("Button3Press"); */
    wt[AB_WHEN_AFTER_CREATED] = istr_const("Create");
    wt[AB_WHEN_DESTROYED] = istr_const("Destroy");
    /* wt[AB_WHEN_DONE]=		istr_const("Done"); */
    wt[AB_WHEN_DROPPED_ON] = istr_const("DroppedUpon");
    /* wt[AB_WHEN_DOUBLE_CLICK]=	istr_const("DoubleClick"); */
    /* wt[AB_WHEN_PTR_ENTER]=	istr_const("PointerEnter"); */
    /* wt[AB_WHEN_PTR_EXIT]=		istr_const("PointerExit"); */
    /* wt[AB_WHEN_KEY_DOWN]=		istr_const("KeyPress"); */
    wt[AB_WHEN_ACTIVATED] = istr_const("Notify");
    wt[AB_WHEN_HIDDEN] = istr_const("Popdown");
    wt[AB_WHEN_SHOWN] = istr_const("Popup");
    wt[AB_WHEN_REPAINT_NEEDED] = istr_const("Repaint");
    wt[AB_WHEN_AFTER_RESIZED] = istr_const("Resize");
    /* wt[AB_WHEN_SELECT]=		istr_const("Select"); */
    wt[AB_WHEN_UNDEF] = istr_const("Undef");
    /* wt[AB_WHEN_UNSELECT]=		istr_const("Unselect"); */

    return 0;
#undef wt
}



AB_WHEN
gilP_string_to_when(STRING when_string)
{
    return (AB_WHEN) convert_string_to_int(
					   when_string,
			     when_table, AB_WHEN_NUM_VALUES, AB_WHEN_UNDEF);
}


int
gil_init(void)
{
    static BOOL		gil_inited = FALSE;

    if (gil_inited)
    {
	return 0;
    }

    align_tab_check_init();
    arg_type_tab_check_init();
    gat_check_init();
    check_builtin_action_table_init();
    button_type_tab_check_init();
    cp_tab_check_init();
    label_type_tab_check_init();
    check_obj_type_table_init();
    check_when_table_init();

    gil_inited = TRUE;

    return 0;
}

