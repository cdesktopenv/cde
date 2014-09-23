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
 *	$XConsortium: util_convert.c /main/4 1995/11/06 18:53:08 rswiston $
 *
 * @(#)util_convert.c	1.25 23 Oct 1994	cde_app_builder/src/
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
 *  util_convert.c - general utilities for converting to/from AB types
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>
#include <ab_private/util.h>
#include <ab/util_types.h>
#include <ab_private/istr.h>

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

/*************************************************************************
**									**
**		Type/String Conversion					**
**									**
**************************************************************************/

static STRING
convert_int_to_string(int value, 
	ISTRING *table, int num_values, ISTRING default_value)
{
	if ((value < 0) || (value >= num_values))
	{
		return istr_string(default_value);
	}
	return istr_string(table[value]);
}

static int
convert_string_to_int(STRING s_value, 
	ISTRING *table, int num_values, int default_value)
{
	int	i;
	int	int_value= default_value;
	ISTRING	is_value= istr_create(s_value);
	if (s_value == NULL)
	{
		istr_destroy(is_value);
		return int_value;
	}
	for (i= 0; i < num_values; ++i)
	{
		if (istr_equal(table[i], is_value))
		{
			int_value= i;
			break;
		}
	}
	istr_destroy(is_value);
	return int_value;
}

static int
convert_fill_table(ISTRING *table, int num_values, ISTRING value)
{
	int	i;
	for (i= 0; i < num_values; ++i)
	{
		table[i]= value;
	}
	return 0;
}


/*************************************************************************
**									**
**		AB_BUILTIN_ACTION					**
**									**
**************************************************************************/

static BOOL	builtin_action_table_inited= FALSE;
static ISTRING	builtin_action_table[AB_BUILTIN_ACTION_NUM_VALUES]= {NULL};

#define check_builtin_action_table_init() \
	(builtin_action_table_inited? 0:builtin_action_table_init())

static int
builtin_action_table_init()
{
#define bat builtin_action_table
	builtin_action_table_inited= TRUE;
	convert_fill_table(builtin_action_table,
					AB_BUILTIN_ACTION_NUM_VALUES, 
					istr_const("?when?"));

	bat[AB_STDACT_UNDEF]=		 istr_const("Undef");
	bat[AB_STDACT_DISABLE]=	  	 istr_const("Disable");
	bat[AB_STDACT_ENABLE]=		 istr_const("Enable");
	bat[AB_STDACT_HIDE]=		 istr_const("Hide");
	bat[AB_STDACT_SET_LABEL]=	 istr_const("SetLabel");
	bat[AB_STDACT_SET_TEXT]=	 istr_const("SetText");
	bat[AB_STDACT_SET_VALUE]= 	 istr_const("SetValue");
	bat[AB_STDACT_SHOW]=		 istr_const("Show");

	return 0;
#undef bat
}


AB_BUILTIN_ACTION
util_string_to_builtin_action(STRING action)
{
	return (AB_BUILTIN_ACTION)convert_string_to_int(action,
			builtin_action_table, AB_BUILTIN_ACTION_NUM_VALUES,
			AB_STDACT_UNDEF);
}


STRING
util_builtin_action_to_string(AB_BUILTIN_ACTION action)
{
	check_builtin_action_table_init();
	return convert_int_to_string(action, 
		builtin_action_table, AB_BUILTIN_ACTION_NUM_VALUES,
		istr_const("?action?"));
}

/*************************************************************************
**									**
**		AB_OBJECT_TYPE						**
**									**
**************************************************************************/
/*
 * REMIND: move type conversion stuff to guide 
 */
static BOOL	obj_type_table_inited= FALSE;
static ISTRING obj_type_table[AB_OBJECT_TYPE_NUM_VALUES+1]= {NULL};

#define check_obj_type_table_init() \
	(obj_type_table_inited? 0:obj_type_table_init())

static int
obj_type_table_init()
{
#define ott obj_type_table
    obj_type_table_inited= TRUE;
    convert_fill_table(obj_type_table, 
	AB_OBJECT_TYPE_NUM_VALUES, istr_const("?type?"));

    ott[AB_TYPE_ACTION]=	istr_const(":action");
    ott[AB_TYPE_ACTION_LIST]=	istr_const(":action-list");
    ott[AB_TYPE_BASE_WINDOW]=	istr_const(":base-window");
    ott[AB_TYPE_BUTTON]=	istr_const(":button");
    ott[AB_TYPE_CHOICE]=	istr_const(":choice");
    ott[AB_TYPE_COMBO_BOX]=	istr_const(":combobox");
    ott[AB_TYPE_FILE_CHOOSER]=	istr_const(":file-chooser");
    ott[AB_TYPE_MESSAGE]=	istr_const(":message");
    ott[AB_TYPE_CONTAINER]=	istr_const(":container");
    ott[AB_TYPE_DIALOG]=	istr_const(":dialog");
    ott[AB_TYPE_DRAWING_AREA]= 	istr_const(":drawing-area");
    ott[AB_TYPE_FILE]=		istr_const(":file");
    ott[AB_TYPE_MODULE]=	istr_const(":module");
    ott[AB_TYPE_ITEM]=		istr_const(":item");
    ott[AB_TYPE_LABEL]=		istr_const(":label");
    ott[AB_TYPE_LAYERS]=	istr_const(":layers");
    ott[AB_TYPE_LIST]=		istr_const(":list");
    ott[AB_TYPE_MENU]=		istr_const(":menu");
    ott[AB_TYPE_PROJECT]=	istr_const(":project");
    ott[AB_TYPE_SEPARATOR]=	istr_const(":separator");
    ott[AB_TYPE_SPIN_BOX]=	istr_const(":spinbox");
    ott[AB_TYPE_SCALE]=		istr_const(":scale");
    ott[AB_TYPE_TERM_PANE]=	istr_const(":term-pane");
    ott[AB_TYPE_TEXT_FIELD]=	istr_const(":text-field");
    ott[AB_TYPE_TEXT_PANE]=	istr_const(":text-pane");
    ott[AB_TYPE_UNKNOWN]=	istr_const(":unknown-type");

    return 0;
#undef ott
}

AB_OBJECT_TYPE
util_string_to_object_type(STRING string_type)
{
	check_obj_type_table_init();
	return (AB_OBJECT_TYPE)convert_string_to_int(
		string_type,
		obj_type_table, AB_OBJECT_TYPE_NUM_VALUES, AB_TYPE_UNKNOWN);
}


STRING
util_object_type_to_string(AB_OBJECT_TYPE type)
{
	check_obj_type_table_init();
	return convert_int_to_string(type,
		obj_type_table, AB_OBJECT_TYPE_NUM_VALUES, 
		istr_const("?type?"));
}

/*****************************************************************
**								**
**		AB_OBJECT_TYPE - for browser			**
**								**
******************************************************************/
/*
 * REMIND: move type conversion stuff to guide 
 */
static BOOL	browser_obj_type_table_inited= FALSE;
static ISTRING browser_obj_type_table[AB_OBJECT_TYPE_NUM_VALUES+1]= {NULL};

#define check_browser_obj_type_table_init() \
	(browser_obj_type_table_inited? 0:browser_obj_type_table_init())

static int
browser_obj_type_table_init()
{
#define bott browser_obj_type_table
    browser_obj_type_table_inited= TRUE;
    convert_fill_table(browser_obj_type_table, 
	AB_OBJECT_TYPE_NUM_VALUES, istr_const("?type?"));

    bott[AB_TYPE_ACTION]=	istr_const("ACTION");
    bott[AB_TYPE_ACTION_LIST]=	istr_const("ACTION-LIST");
    bott[AB_TYPE_BASE_WINDOW]=	istr_const("BASE-WINDOW");
    bott[AB_TYPE_BUTTON]=	istr_const("BUTTON");
    bott[AB_TYPE_CHOICE]=	istr_const("CHOICE");
    bott[AB_TYPE_COMBO_BOX]=	istr_const("COMBO-BOX");
    bott[AB_TYPE_FILE_CHOOSER]=	istr_const("file-chooser");
    bott[AB_TYPE_MESSAGE]=	istr_const("message");
    bott[AB_TYPE_CONTAINER]=	istr_const("CONTAINER");
    bott[AB_TYPE_DIALOG]=	istr_const("DIALOG");
    bott[AB_TYPE_DRAWING_AREA]= istr_const("DRAWING-AREA");
    bott[AB_TYPE_MODULE]=	istr_const("MODULE");
    bott[AB_TYPE_ITEM]=		istr_const("ITEM");
    bott[AB_TYPE_LABEL]=	istr_const("LABEL");
    bott[AB_TYPE_LAYERS]=	istr_const("LAYERS");
    bott[AB_TYPE_LIST]=		istr_const("LIST");
    bott[AB_TYPE_MENU]=		istr_const("MENU");
    bott[AB_TYPE_PROJECT]=	istr_const("PROJECT");
    bott[AB_TYPE_SEPARATOR]=	istr_const("SEPARATOR");
    bott[AB_TYPE_SPIN_BOX]=	istr_const("SPIN-BOX");
    bott[AB_TYPE_SCALE]=	istr_const("SCALE");
    bott[AB_TYPE_TERM_PANE]=	istr_const("TERM-PANE");
    bott[AB_TYPE_TEXT_FIELD]=	istr_const("TEXT-FIELD");
    bott[AB_TYPE_TEXT_PANE]=	istr_const("TEXT-PANE");
    bott[AB_TYPE_UNKNOWN]=	istr_const("UNKNOWN-TYPE");

    return 0;
#undef bott
}

AB_OBJECT_TYPE
util_browser_string_to_object_type(STRING string_type)
{
	check_browser_obj_type_table_init();
	return (AB_OBJECT_TYPE)convert_string_to_int(
		string_type,
		browser_obj_type_table, AB_OBJECT_TYPE_NUM_VALUES, AB_TYPE_UNKNOWN);
}


STRING
util_object_type_to_browser_string(AB_OBJECT_TYPE type)
{
	check_browser_obj_type_table_init();
	return convert_int_to_string(type,
		browser_obj_type_table, AB_OBJECT_TYPE_NUM_VALUES, 
		istr_const("?type?"));
}

/*************************************************************************
**									**
**		AB_WHEN							**
**									**
**************************************************************************/
/*
 * REMIND: move when stuff to libguide
 */
static ISTRING	when_table[AB_WHEN_NUM_VALUES];
static BOOL	when_table_inited= FALSE;

static int
when_table_init()
{
#define wt when_table
	int	i= 0;

	when_table_inited= TRUE;
	convert_fill_table(when_table, AB_WHEN_NUM_VALUES, 
			istr_const("?when?"));

	wt[AB_WHEN_ACTION1]=		istr_const("Action1Activated");
	wt[AB_WHEN_ACTION2]=		istr_const("Action2Activated");
	wt[AB_WHEN_ACTION3]=		istr_const("Action3Activated");
	wt[AB_WHEN_ACTIVATED]=		istr_const("Activate");
	wt[AB_WHEN_AFTER_CREATED]=	istr_const("AfterCreate");
	wt[AB_WHEN_AFTER_RESIZED]=	istr_const("AfterResize");
	wt[AB_WHEN_BEFORE_POST_MENU]=	istr_const("BeforePostMenu");
	wt[AB_WHEN_BEFORE_TEXT_CHANGED]=istr_const("BeforeTextChanged");
	wt[AB_WHEN_CANCEL]=		istr_const("CancelActivated");
	wt[AB_WHEN_DESTROYED]=		istr_const("Destroy");
	wt[AB_WHEN_DRAGGED]=		istr_const("Dragged");
	wt[AB_WHEN_DRAGGED_FROM]=	istr_const("DraggedFrom");
	wt[AB_WHEN_DOUBLE_CLICKED_ON]=	istr_const("DoubleClick");
	wt[AB_WHEN_DROPPED_ON]=		istr_const("DroppedOn");
	wt[AB_WHEN_HIDDEN]=		istr_const("Hidden");
	wt[AB_WHEN_ITEM_SELECTED]=	istr_const("ItemSelected");
	wt[AB_WHEN_OK]=			istr_const("OkActivated");
	wt[AB_WHEN_REPAINT_NEEDED]=	istr_const("NeedRepaint");
	wt[AB_WHEN_POPPED_DOWN]=	istr_const("Popdown");
	wt[AB_WHEN_POPPED_UP]=		istr_const("Popup");
	wt[AB_WHEN_SESSION_RESTORE]=	istr_const("SessionRestore");
	wt[AB_WHEN_SESSION_SAVE]=	istr_const("SessionSave");
	wt[AB_WHEN_SHOWN]=		istr_const("Shown");
	wt[AB_WHEN_TEXT_CHANGED]=	istr_const("TextChanged");
	wt[AB_WHEN_TOOLTALK_DO_COMMAND]=istr_const("ToolTalkDoCommand");
	wt[AB_WHEN_TOOLTALK_GET_STATUS]=istr_const("ToolTalkGetStatus");
	wt[AB_WHEN_TOOLTALK_PAUSE_RESUME]=istr_const("ToolTalkPauseResume");
	wt[AB_WHEN_TOOLTALK_QUIT]=istr_const("ToolTalkQuit");
	wt[AB_WHEN_VALUE_CHANGED]=istr_const("ValueChanged");
	wt[AB_WHEN_UNDEF]=		istr_const("Undef");

	return 0;
#undef wt
}

#define check_when_table_init() \
		if (!when_table_inited) {when_table_init();}


AB_WHEN
util_string_to_when(STRING when_string)
{
	check_when_table_init();
	return (AB_WHEN)convert_string_to_int(
		when_string,
		when_table, AB_WHEN_NUM_VALUES, AB_WHEN_UNDEF);
}


STRING
util_when_to_string(AB_WHEN when)
{
	check_when_table_init();
	return convert_int_to_string(when,
		when_table, AB_WHEN_NUM_VALUES, istr_const("?when?"));
}

/******************* AB_ALIGNMENT **************************************/

static ISTRING	align_tab[AB_ALIGNMENT_NUM_VALUES];
static BOOL	align_tab_inited= FALSE;
#define align_tab_check_init() (align_tab_inited? 0:align_tab_init())

static int
align_tab_init()
{
#define at align_tab
    align_tab_inited= TRUE;
    convert_fill_table(at, ARRAY_SIZE(at), istr_const(NULL));

    at[AB_ALIGN_TOP]=		istr_const(":top");
    at[AB_ALIGN_LEFT]=		istr_const(":left");
    at[AB_ALIGN_RIGHT]=		istr_const(":right");
    at[AB_ALIGN_BOTTOM]=	istr_const(":bottom");
    at[AB_ALIGN_CENTER]=	istr_const(":centers");
    at[AB_ALIGN_LABELS]=	istr_const(":labels");

    return 0;
#undef at
}

AB_ALIGNMENT
util_string_to_alignment(STRING align_string)
{
    align_tab_check_init();
    return (AB_ALIGNMENT)convert_string_to_int(align_string,
		align_tab, ARRAY_SIZE(align_tab), -1);
}

STRING
util_alignment_to_string(AB_ALIGNMENT align)
{
    align_tab_check_init();
    return convert_int_to_string(align,
		align_tab, ARRAY_SIZE(align_tab), istr_const(NULL));
}

/******************* AB_LABEL_TYPE **************************************/
static ISTRING	label_type_tab[AB_LABEL_TYPE_NUM_VALUES];
static BOOL	label_type_tab_inited= FALSE;
#define label_type_tab_check_init() \
		(label_type_tab_inited? 0:label_type_tab_init())

static int
label_type_tab_init()
{
#define ltt label_type_tab
	label_type_tab_inited= TRUE;

	ltt[AB_LABEL_ARROW_DOWN]= istr_const(":arrow-down");
	ltt[AB_LABEL_ARROW_LEFT]= istr_const(":arrow-left");
	ltt[AB_LABEL_ARROW_RIGHT]= istr_const(":arrow-right");
	ltt[AB_LABEL_ARROW_UP]= istr_const(":arrow_up");
	ltt[AB_LABEL_DRAWN]= istr_const(":drawn");
	ltt[AB_LABEL_GLYPH]= istr_const(":glyph");
	ltt[AB_LABEL_STRING]= istr_const(":string");

	return 0;
#undef ltt
}

AB_LABEL_TYPE
util_string_to_label_type(STRING type_string)
{
	label_type_tab_check_init();
	return (AB_LABEL_TYPE)convert_string_to_int(type_string,
		label_type_tab, ARRAY_SIZE(label_type_tab), -1);
}

STRING
util_label_type_to_string(AB_LABEL_TYPE label_type)
{
    label_type_tab_check_init();
    return convert_int_to_string(label_type,
		label_type_tab, ARRAY_SIZE(label_type_tab), NULL);
}

/******************* AB_BUTTON_TYPE **************************************/
static ISTRING	button_type_tab[AB_BUTTON_TYPE_NUM_VALUES];
static BOOL	button_type_tab_inited= FALSE;
#define button_type_tab_check_init() \
		(button_type_tab_inited? 0:button_type_tab_init())

static int
button_type_tab_init()
{
#define btt button_type_tab
    button_type_tab_inited= TRUE;

    btt[AB_BUT_PUSH]= istr_const(":normal");

    return 0;
#undef btt
}

AB_BUTTON_TYPE
util_string_to_button_type(STRING type_string)
{
    button_type_tab_check_init();
    return (AB_BUTTON_TYPE)convert_string_to_int(type_string,
		button_type_tab, ARRAY_SIZE(button_type_tab), -1);
}

STRING
util_button_type_to_string(AB_BUTTON_TYPE type)
{
    button_type_tab_check_init();
    return convert_int_to_string(type,
		button_type_tab, ARRAY_SIZE(button_type_tab), NULL);
}


/******************* AB_ARG_CLASS **************************************/
static ISTRING	arg_class_tab[AB_ARG_CLASS_NUM_VALUES];
static BOOL	arg_class_tab_inited= FALSE;
#define arg_class_tab_check_init() \
		(arg_class_tab_inited? 0:arg_class_tab_init())

static int
arg_class_tab_init()
{
#define att arg_class_tab
    arg_class_tab_inited= TRUE;

    att[AB_ARG_CLASS_UNDEF]=		istr_const(":undef");
    att[AB_ARG_CLASS_ACCELERATOR]=	istr_const(":attachment");
    att[AB_ARG_CLASS_COLOR]=		istr_const(":color");
    att[AB_ARG_CLASS_GEOMETRY]=		istr_const(":geometry");
    att[AB_ARG_CLASS_GLYPH]=		istr_const(":value");
    att[AB_ARG_CLASS_NON_GLYPH_LABEL]=	istr_const(":label-alignment");
    att[AB_ARG_CLASS_OTHER_STRING]=	istr_const(":label-glyph");
    att[AB_ARG_CLASS_OTHER]=		istr_const(":other");
    att[AB_ARG_CLASS_OTHER]=		istr_const(":other");

    return 0;
#undef att
}

AB_ARG_CLASS
util_string_to_arg_class(STRING arg_class_string)
{
    arg_class_tab_check_init();
    return (AB_ARG_CLASS)convert_string_to_int(arg_class_string,
		arg_class_tab, ARRAY_SIZE(arg_class_tab), -1);
}

STRING
util_arg_class_to_string(AB_ARG_CLASS arg_class)
{
    arg_class_tab_check_init();
    return convert_int_to_string(arg_class,
		arg_class_tab, ARRAY_SIZE(arg_class_tab), NULL);
}


AB_ARG_CLASS_FLAGS
util_arg_class_to_flag(AB_ARG_CLASS arg_class)
{
    AB_ARG_CLASS_FLAGS	class_flag = AB_ARG_CLASS_FLAGS_NONE;

    if ((arg_class < 1) || (arg_class > AB_ARG_CLASS_NUM_VALUES))
    {
	return (AB_ARG_CLASS_FLAGS)0;
    }
    class_flag = (AB_ARG_CLASS_FLAGS)(0x01<<(((unsigned)arg_class)-1));

    assert(arg_class == util_flag_to_arg_class(class_flag));
    return class_flag;
}


AB_ARG_CLASS
util_flag_to_arg_class(AB_ARG_CLASS_FLAGS arg_class_flag)
{
    int			i = 0;
    AB_ARG_CLASS	arg_class = AB_ARG_CLASS_UNDEF;

    for (i = 1; i < AB_ARG_CLASS_NUM_VALUES; ++i)
    {
	if ((arg_class_flag & 0x01) != 0)
	{
	    break;
	}
	arg_class_flag >>= 1;
    }
    if (i < AB_ARG_CLASS_NUM_VALUES)
    {
	arg_class = (AB_ARG_CLASS)i;
    }

    return arg_class;
}


/******************* AB_ARG_TYPE **************************************/
static ISTRING	arg_type_tab[AB_ARG_TYPE_NUM_VALUES];
static BOOL	arg_type_tab_inited= FALSE;
#define arg_type_tab_check_init() (arg_type_tab_inited? 0:arg_type_tab_init())

static int
arg_type_tab_init()
{
#define att arg_type_tab
    arg_type_tab_inited= TRUE;

    att[AB_ARG_BOOLEAN]=	istr_const(":boolean");
    att[AB_ARG_CALLBACK]=	istr_const(":callback");
    att[AB_ARG_FLOAT]=		istr_const(":float");
    att[AB_ARG_INT]=		istr_const(":integer");
    att[AB_ARG_LITERAL]=	istr_const(":literal");
    att[AB_ARG_STRING]=		istr_const(":string");
    att[AB_ARG_VOID_PTR]=	istr_const(":void");

    return 0;
#undef att
}

AB_ARG_TYPE
util_string_to_arg_type(STRING arg_type_string)
{
    arg_type_tab_check_init();
    return (AB_ARG_TYPE)convert_string_to_int(arg_type_string,
		arg_type_tab, ARRAY_SIZE(arg_type_tab), -1);
}

STRING
util_arg_type_to_string(AB_ARG_TYPE arg_type)
{
    arg_type_tab_check_init();
    return convert_int_to_string(arg_type,
		arg_type_tab, ARRAY_SIZE(arg_type_tab), NULL);
}

/******************* AB_TEXT_TYPE **************************************/
AB_TEXT_TYPE
util_string_to_text_type(STRING s_text_type)
{
	return AB_TEXT_ALPHANUMERIC;
}

STRING
util_text_type_to_string(AB_TEXT_TYPE tt)
{
	return "text-type-not-implemented";
}

/******************* AB_CHOICE_TYPE **************************************/
AB_CHOICE_TYPE
util_string_to_choice_type(STRING s_choice_type)
{
	return AB_CHOICE_EXCLUSIVE;
}

STRING
util_choice_type_to_string(AB_CHOICE_TYPE choice_type)
{
	return ":choice-type-not-implemented";
}

/******************* AB_ORIENTATION **************************************/
AB_ORIENTATION
util_string_to_orientation(STRING s_orient)
{
	return AB_ORIENT_HORIZONTAL;
}

STRING
util_orientation_to_string(AB_ORIENTATION orient)
{
	return "orientation-not-implemented";
}

/******************* AB_MENU_TYPE **************************************/
AB_MENU_TYPE
util_string_to_menu_type(STRING s_menu_type)
{
	return AB_MENU_PULLDOWN;
}

STRING
util_menu_type_to_string(AB_MENU_TYPE type)
{
	return "menu-type-not-implemented";
}

/******************* AB_GROUP_TYPE **************************************/
AB_GROUP_TYPE
util_string_to_group_type(STRING s_group)
{
	return AB_GROUP_IGNORE;
}

STRING
util_group_type_to_string(AB_GROUP_TYPE group)
{
	return "group-type-not-implemented";
}

/******************* AB_OBJECT_STATE **************************************/
AB_OBJECT_STATE
util_string_to_object_state(STRING s_state)
{
	return AB_STATE_ACTIVE;
}

STRING
util_object_state_to_string(AB_OBJECT_STATE state)
{
	return "object-state-not-implemented";
}

/******************* AB_CONTAINER_TYPE **************************************/
 
static ISTRING  container_tab[AB_CONTAINER_TYPE_NUM_VALUES];
static BOOL     container_tab_inited= FALSE;
#define container_tab_check_init() (container_tab_inited? 0:container_tab_init())
 
static int
container_tab_init()
{
#define cont container_tab
    container_tab_inited= TRUE;
    convert_fill_table(cont, ARRAY_SIZE(cont), istr_const(NULL));
 
    cont[AB_CONT_ABSOLUTE]= istr_const(":absolute");
    cont[AB_CONT_GROUP]= istr_const(":group");
    cont[AB_CONT_MAIN_WINDOW]= istr_const(":main_window");
    cont[AB_CONT_MENU_BAR]= istr_const(":menu_bar");
    cont[AB_CONT_PANED]= istr_const(":paned");
    cont[AB_CONT_RELATIVE]= istr_const(":relative");
    cont[AB_CONT_SCROLLED_WINDOW]= istr_const(":scrolled_window");
    cont[AB_CONT_ROW_COLUMN]= istr_const(":row_column");
    return 0;
#undef cont
}
 
AB_CONTAINER_TYPE
util_string_to_container_type(STRING container_string)
{
    container_tab_check_init();
    return
(AB_CONTAINER_TYPE)convert_string_to_int(container_string,
                container_tab, ARRAY_SIZE(container_tab),
-1);
}

STRING
util_container_type_to_string(AB_CONTAINER_TYPE type)
{
    container_tab_check_init();
    return convert_int_to_string(type,
		container_tab, ARRAY_SIZE(container_tab), istr_const(NULL));
}


/******************* AB_ITEM_TYPE **************************************/
 
static ISTRING  item_tab[AB_ITEM_TYPE_NUM_VALUES];
static BOOL     item_tab_inited= FALSE;
#define item_tab_check_init() (item_tab_inited? 0:item_tab_init())
 
static int
item_tab_init()
{
#define it item_tab
    item_tab_inited= TRUE;
    convert_fill_table(it, ARRAY_SIZE(it), istr_const(NULL));
 
    it[AB_ITEM_FOR_CHOICE]= istr_const(":choice_item");
    it[AB_ITEM_FOR_LIST]= istr_const(":list_item");
    it[AB_ITEM_FOR_MENU]= istr_const(":menu_item");
    return 0;
#undef it
}
 
AB_ITEM_TYPE
util_string_to_item_type(STRING item_string)
{
    item_tab_check_init();
    return
(AB_ITEM_TYPE)convert_string_to_int(item_string,
                item_tab, ARRAY_SIZE(item_tab), -1);
}

STRING
util_item_type_to_string(AB_ITEM_TYPE type)
{
    item_tab_check_init();
    return convert_int_to_string(type,
		item_tab, ARRAY_SIZE(item_tab), istr_const(NULL));
}

/******************* AB_LABEL_ALIGN_TYPE **************************************/
 
/*
static ISTRING  lalign_tab[AB_LABEL_ALIGN_TYPE_NUM_VALUES];
static BOOL     lalign_tab_inited= FALSE;
#define lalign_tab_check_init() (lalign_tab_inited? 0:lalign_tab_init())
 
static int
lalign_tab_init()
{
#define lat lalign_tab
    lalign_tab_inited= TRUE;
    convert_fill_table(lat, ARRAY_SIZE(lat), istr_const(NULL));
 
    lat[AB_LABEL_ALIGN_RIGHT]= istr_const(":right");
    lat[AB_LABEL_ALIGN_CENTER]= istr_const(":center");
    lat[AB_LABEL_ALIGN_LEFT]= istr_const(":left");
    return 0;
#undef lat
}
 
AB_LABEL_ALIGN_TYPE
ab_string_to_label_align_type(STRING lalign_string)
{
    lalign_tab_check_init();
    return
(AB_LABEL_ALIGN_TYPE)convert_string_to_int(lalign_string,
                lalign_tab, ARRAY_SIZE(lalign_tab), -1);
}

STRING
ab_label_align_type_to_string(AB_LABEL_ALIGN_TYPE type)
{
    lalign_tab_check_init();
    return convert_int_to_string(type,
		lalign_tab, ARRAY_SIZE(lalign_tab), istr_const(NULL));
}
*/


STRING
util_cvt_bool_to_string(BOOL bool_value, STRING buf, int buf_size)
{
    if (buf_size < 1)
    {
	goto epilogue;
    }

    if (bool_value)
    {
	strncpy(buf, "True", buf_size);
    }
    else
    {
	strncpy(buf, "False", buf_size);
    }
    buf[buf_size-1]= 0;

epilogue:
    return buf;
}


/*
 * returns identifier-legal string that can be used for file names,
 * et cetera.
 */
STRING	
util_os_type_to_ident(AB_OS_TYPE osType)
{
    STRING	typeString = NULL;

    switch (osType)
    {
	case AB_OS_AIX: typeString = "aix"; break;
	case AB_OS_HPUX: typeString = "hpux"; break;
	case AB_OS_SUNOS: typeString = "sunos"; break;
	case AB_OS_UNIXWARE: typeString = "unixware"; break;
	case AB_OS_UXP: typeString = "uxp"; break;
        case AB_OS_OSF1: typeString = "osf1"; break;
        case AB_OS_LNX: typeString = "linux"; break;
        case AB_OS_FBSD: typeString = "freebsd"; break;
        case AB_OS_NBSD: typeString = "netbsd"; break;
        case AB_OS_OBSD: typeString = "openbsd"; break;
    }

    return typeString;
}


/*
 * Returns a nicely formatted OS name (even on not-so-nice OSes!)
 */
STRING
util_os_type_to_string(AB_OS_TYPE osType)
{
    STRING	nameString = NULL;

    switch (osType)
    {
	case AB_OS_AIX: nameString = "AIX"; break;
	case AB_OS_HPUX: nameString = "HP-UX"; break;
	case AB_OS_SUNOS: nameString = "SunOS"; break;
	case AB_OS_UNIXWARE: nameString = "UnixWare"; break;
	case AB_OS_UXP: nameString = "UXP"; break;
        case AB_OS_OSF1: nameString = "OSF/1"; break;
        case AB_OS_LNX: nameString = "Linux"; break;
        case AB_OS_FBSD: nameString = "FreeBSD"; break;
        case AB_OS_NBSD: nameString = "NetBSD"; break;
        case AB_OS_OBSD: nameString = "OpenBSD"; break;
    }

    return nameString;
}

