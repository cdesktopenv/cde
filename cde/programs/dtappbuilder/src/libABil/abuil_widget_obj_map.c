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
 *	$XConsortium: abuil_widget_obj_map.c /main/3 1995/11/06 18:22:10 rswiston $
 *
 *	@(#)abuil_widget_obj_map.c	1.24 31 Jan 1995	
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
 * This file contains the mapping between uil widgets and ABObj's
 */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "abuil_loadP.h"

/*
 * Function prototypes
 */
static ABObj	objp_type_button(char *, ABObj, ABObj);
static ABObj	childp_type_button(char *, ABObj, ABObj);
static ABObj	objp_type_choice(char *, ABObj, ABObj);
static ABObj	childp_type_choice(char *, ABObj, ABObj);
static ABObj	objp_type_compound(char *, ABObj, ABObj);
static ABObj	childp_type_compound(char *, ABObj, ABObj);
static ABObj	objp_type_container(char *, ABObj, ABObj);
static ABObj	childp_type_container(char *, ABObj, ABObj);
static ABObj	objp_type_dialog(char *, ABObj, ABObj);
static ABObj	childp_type_dialog(char *, ABObj, ABObj);
static ABObj	objp_type_drawing_area(char *, ABObj, ABObj);
static ABObj	childp_type_drawing_area(char *, ABObj, ABObj);
static ABObj	objp_type_label(char *, ABObj, ABObj);
static ABObj	childp_type_label(char *, ABObj, ABObj);
static ABObj	objp_type_list(char *, ABObj, ABObj);
static ABObj	childp_type_list(char *, ABObj, ABObj);
static ABObj	objp_type_menu(char *, ABObj, ABObj);
static ABObj	childp_type_menu(char *, ABObj, ABObj);
static ABObj	objp_type_separator(char *, ABObj, ABObj);
static ABObj	childp_type_separator(char *, ABObj, ABObj);
static ABObj	objp_type_scale(char *, ABObj, ABObj);
static ABObj	childp_type_scale(char *, ABObj, ABObj);
static ABObj	objp_type_text_field(char *, ABObj, ABObj);
static ABObj	childp_type_text_field(char *, ABObj, ABObj);
static ABObj	objp_type_text_pane(char *, ABObj, ABObj);
static ABObj	childp_type_text_pane(char *, ABObj, ABObj);
static ABObj	objp_type_unknown(char *, ABObj, ABObj);
static ABObj	childp_type_unknown(char *, ABObj, ABObj);


static WidgetABObjMap
widget_abobj_map[] =
{
    { "XmArrowButton",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmArrowButtonGadget",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmBulletinBoard",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_ABSOLUTE,
		objp_type_container, childp_type_container },
    { "XmBulletinBoardDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_CUSTOM,
		objp_type_dialog, childp_type_dialog },
    { "XmCascadeButton",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmCascadeButtonGadget",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmCommand",
		AB_TYPE_UNKNOWN,	(unsigned int)0,
		objp_type_compound, childp_type_compound },
    { "XmDialogShell",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_CUSTOM,
		objp_type_dialog, childp_type_dialog },
    { "XmDrawingArea",
		AB_TYPE_DRAWING_AREA,	(unsigned int)0,
		objp_type_drawing_area, childp_type_drawing_area },
    { "XmDrawnButton",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmErrorDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmFileSelectionBox",
		AB_TYPE_FILE_CHOOSER,	(unsigned int) 0,
		objp_type_compound, childp_type_compound },
    { "XmFileSelectionDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmForm",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_RELATIVE,
		objp_type_container, childp_type_container },
    { "XmFormDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_CUSTOM,
		objp_type_dialog, childp_type_dialog },
    { "XmFrame",
		AB_TYPE_UNKNOWN,	(unsigned int)AB_TYPE_UNKNOWN,
		objp_type_unknown, childp_type_unknown },
    { "XmInformationDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmLabel",
		AB_TYPE_LABEL,	(unsigned int)0,
		objp_type_label, childp_type_label },
    { "XmLabelGadget",
		AB_TYPE_LABEL,	(unsigned int)0,
		objp_type_label, childp_type_label },
    { "XmList",
		AB_TYPE_LIST,	(unsigned int)0,
		objp_type_list, childp_type_list },
    { "XmMainWindow",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_MAIN_WINDOW,
		objp_type_container, childp_type_container },
    { "XmMenuBar",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_MENU_BAR,
		objp_type_container, childp_type_container },
    { "XmMenuShell",
		AB_TYPE_UNKNOWN,	(unsigned int)0,
		objp_type_unknown, childp_type_unknown },
    { "XmMessageBox",
		AB_TYPE_MESSAGE,	(unsigned int)0,
		objp_type_compound, childp_type_compound },
    { "XmMessageDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmOptionMenu",
		AB_TYPE_CHOICE,	(unsigned int)AB_CHOICE_OPTION_MENU,
		objp_type_choice, childp_type_choice },
    { "XmPanedWindow",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_PANED,
		objp_type_container, childp_type_container },
    { "XmPopupMenu",
		AB_TYPE_MENU,	(unsigned int)AB_MENU_POPUP,
		objp_type_menu, childp_type_menu },
    { "XmPromptDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmPulldownMenu",
		AB_TYPE_MENU,	(unsigned int)AB_MENU_PULLDOWN,
		objp_type_menu, childp_type_menu },
    { "XmPushButton",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmPushButtonGadget",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmQuestionDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmRadioBox",
		AB_TYPE_CHOICE,	(unsigned int)AB_CHOICE_EXCLUSIVE,
		objp_type_choice, childp_type_choice },
    { "XmRowColumn",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_ROW_COLUMN,
		objp_type_container, childp_type_container },
    { "XmScale",
		AB_TYPE_SCALE,	(unsigned int)0,
		objp_type_scale, childp_type_scale },
    { "XmScrollBar",
		AB_TYPE_UNKNOWN,	(unsigned int)AB_TYPE_UNKNOWN,
		objp_type_unknown, childp_type_unknown },
    { "XmScrolledList",
		AB_TYPE_LIST,	(unsigned int)0,
		objp_type_list, childp_type_list },
    { "XmScrolledText",
		AB_TYPE_TEXT_PANE,	(unsigned int)AB_TEXT_ALPHANUMERIC,
		objp_type_text_pane, childp_type_text_pane },
    { "XmScrolledWindow",
		AB_TYPE_UNKNOWN,	(unsigned int)AB_TYPE_UNKNOWN,
		objp_type_unknown, childp_type_unknown },
    { "XmSelectionBox",
		AB_TYPE_UNKNOWN,	(unsigned int)0,
		objp_type_compound, childp_type_compound },
    { "XmSelectionDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmSeparator",
		AB_TYPE_SEPARATOR,	(unsigned int)0,
		objp_type_separator, childp_type_separator },
    { "XmSeparatorGadget",
		AB_TYPE_SEPARATOR,	(unsigned int)0,
		objp_type_separator, childp_type_separator },
    { "XmTearOffButton",
		AB_TYPE_BUTTON,	(unsigned int)0,
		objp_type_button, childp_type_button },
    { "XmTemplateDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmText",
		AB_TYPE_TEXT_PANE,	(unsigned int)AB_TEXT_ALPHANUMERIC,
		objp_type_text_pane, childp_type_text_pane },
    { "XmTextField",
		AB_TYPE_TEXT_FIELD,	(unsigned int)AB_TEXT_ALPHANUMERIC,
		objp_type_text_field, childp_type_text_field },
    { "XmToggleButton",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmToggleButtonGadget",
		AB_TYPE_BUTTON,	(unsigned int)AB_BUT_PUSH,
		objp_type_button, childp_type_button },
    { "XmWarningDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
    { "XmWorkArea",
		AB_TYPE_CONTAINER,	(unsigned int)AB_CONT_ROW_COLUMN,
		objp_type_container, childp_type_container },
    { "XmWorkingDialog",
		AB_TYPE_DIALOG,	(unsigned int)AB_DLG_BUILTIN,
		objp_type_dialog, childp_type_dialog },
};

#ifdef TEST

main(
    int		argc,
    char	**argv
)
{
    int	i;

    for (i = 0; i < XtNumber(widget_abobj_map); i++)
    {
	int		j;
	WidgetABObjMap	*this_map = &(widget_abobj_map[i]);

	printf("%s	%d	%d\n", this_map->widget_name,
		this_map->obj_type, this_map->sub_type);

    }
}

#endif /* TEST */


extern WidgetABObjMap *
abuilP_entry_for_uil_widget(
    sym_widget_entry_type	*uil_widget
)
{
    int			b_type	= uil_widget->header.b_type;

    if (b_type > sym_k_error_object)
    {
	int	i;
	STRING		widget_class_name = uil_widget_names[b_type];

	for (i = 0; i < XtNumber(widget_abobj_map); i++)
	    if (!strcmp(widget_abobj_map[i].widget_name, widget_class_name))
		return(&(widget_abobj_map[i]));
    }
    return((WidgetABObjMap *)NULL);
}




/*
 * objp_type_button:
 */
static ABObj
objp_type_button(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent) || obj_is_menu(parent))
    {
	obj_set_is_initially_active(object, TRUE);
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_button:
 */
static ABObj
childp_type_button(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_choice:
 */
static ABObj
objp_type_choice(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent))
    {
	obj_set_class_name(object, "xmRowColumnWidgetClass");
	obj_set_num_columns(object, 1);
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_choice:
 */
static ABObj
childp_type_choice(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    /*
     * Skip menu child of option menu
     */
    if (obj_get_subtype(object) == AB_CHOICE_OPTION_MENU &&
	obj_is_menu(child))
	return(NULL);

    obj_set_type(child, AB_TYPE_ITEM);
    obj_set_subtype(child, AB_ITEM_FOR_CHOICE);
    return(child);
}

/*
 * objp_type_compound:
 */
static ABObj
objp_type_compound(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    return(object);
}

/*
 * childp_type_compound:
 */
static ABObj
childp_type_compound(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_container:
 */
static ABObj
objp_type_container(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    if (obj_get_subtype(object) != (int)AB_CONT_MENU_BAR)
    {
	if (obj_is_module(parent))
	    obj_set_type(object, AB_TYPE_BASE_WINDOW);
	else if (!obj_is_module(obj_get_parent(parent)))
	{
	    /* Only time we have control panels is when
	     * it is a child of the main window or dialog
	     * which is always rooted at the module
	     */
	    obj_set_subtype(object, AB_CONT_GROUP);
	    obj_set_group_type(object, AB_GROUP_ROWSCOLUMNS);
	}
    }

    /*
     * REMIND: jit
     * Temporary until we support all types of containers
    obj_set_class_name(object, "xmBulletinBoardWidgetClass");
     */

    return(object);
}

/*
 * childp_type_container:
 */
static ABObj
childp_type_container(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    if (obj_get_subtype(object) == (int)AB_CONT_MENU_BAR)
    {
	obj_set_type(child, AB_TYPE_ITEM);
	obj_set_subtype(child, (int)AB_ITEM_FOR_MENUBAR);
	if (obj_get_label(child) == (STRING)NULL)
	    obj_set_label(child, obj_get_name(child));
    }
    return(child);
}

/*
 * objp_type_dialog:
 */
static ABObj
objp_type_dialog(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    /*
     * REMIND: jit
     * Temporary until we support all types of containers
     */
    obj_set_class_name(object, "xmDialogShellWidgetClass");

    return(object);
}

/*
 * childp_type_dialog:
 */
static ABObj
childp_type_dialog(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_drawing_area:
 */
static ABObj
objp_type_drawing_area(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    return(object);
}

/*
 * childp_type_drawing_area:
 */
static ABObj
childp_type_drawing_area(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_label:
 */
static ABObj
objp_type_label(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent))
    {
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_label:
 */
static ABObj
childp_type_label(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_list:
 */
static ABObj
objp_type_list(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent))
    {
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_list:
 */
static ABObj
childp_type_list(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_menu:
 */
static ABObj
objp_type_menu(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    obj_set_menu_name(parent, obj_get_name(object));
    obj_reparent(object, obj_get_module(object));
    return(object);
}

/*
 * childp_type_menu:
 */
static ABObj
childp_type_menu(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    obj_set_type(child, AB_TYPE_ITEM);
    obj_set_subtype(child, AB_ITEM_FOR_MENU);
    if (obj_get_label(child) == (STRING)NULL)
	obj_set_label(child, obj_get_name(child));
    return(child);
}

/*
 * objp_type_separator:
 */
static ABObj
objp_type_separator(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent))
    {
	obj_set_line_style(object, AB_LINE_ETCHED_IN);
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_separator:
 */
static ABObj
childp_type_separator(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_scale:
 */
static ABObj
objp_type_scale(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent))
    {
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_scale:
 */
static ABObj
childp_type_scale(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_text_field:
 */
static ABObj
objp_type_text_field(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    ABObj	ret_val = (ABObj)NULL;

    if (obj_is_container(parent))
    {
	ret_val = object;
    }
    return(ret_val);
}

/*
 * childp_type_text_field:
 */
static ABObj
childp_type_text_field(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_text_pane:
 */
static ABObj
objp_type_text_pane(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    return(object);
}

/*
 * childp_type_text_pane:
 */
static ABObj
childp_type_text_pane(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    return(child);
}

/*
 * objp_type_unknown:
 */
static ABObj
objp_type_unknown(
    char *	class_name,
    ABObj	parent,
    ABObj	object
)
{
    return(object);
}

/*
 * childp_type_unknown:
 */
static ABObj
childp_type_unknown(
    char *	class_name,
    ABObj	object,
    ABObj	child
)
{
    if (strcmp(class_name, "XmFrame") == 0)
    {
	if (obj_is_container(child))
	    child->info.container.has_border = TRUE;
    }
    else if (strcmp(class_name, "XmMenuShell") == 0)
    {
	if (obj_is_container(child))
	    obj_set_type(object, AB_TYPE_MENU);
    }
    else if (strcmp(class_name, "XmScrolledWindow") == 0)
    {
	if (   (obj_set_hscrollbar_policy(child, AB_SCROLLBAR_NEVER) < 0)
	    || (obj_set_vscrollbar_policy(child, AB_SCROLLBAR_WHEN_NEEDED) < 0)
	   )
	{
	    fprintf(stderr,
		"Non (drawing area, list) below scrolled window.\n");
	}
    }
    return(child);
}

