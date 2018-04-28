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
 *	$XConsortium: create_decls.c /main/3 1995/11/06 18:03:55 rswiston $
 *
 *	@(#)create_defs.c	1.20 16 Feb 1994	cde_app_builder/src/abmf
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
 *  create_decls.c - write out local variable definitions for
 *			create procs.
 *
 *  Note: This file actually writes definitions, not declarations:
 *
 *	A declaration does not actually "create" a variable. E.g.:
 *
 *		extern int x;
 *
 *	A definition does "create" the variable. E.g.:
 *
 *		int x;
 *
 *
 *     This file is used to write out the *definitions*.
 */

#include <ab_private/abio.h>
#include <ab_private/objxm.h>
#include "obj_namesP.h"
#include "write_codeP.h"
#include "write_cP.h"
#include "utilsP.h"
#include "argsP.h"
#include "instancesP.h"
#include "create_declsP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

static int write_menu_defs(GenCodeInfo genCodeInfo, ABObj menu);
static int write_menubar_defs(GenCodeInfo genCodeInfo, ABObj menubar);
static int write_text_defs(GenCodeInfo genCodeInfo, ABObj text);
static int write_button_defs(GenCodeInfo genCodeInfo, ABObj obj);
static int write_choice_defs(GenCodeInfo genCodeInfo, ABObj choice);
static int write_combobox_defs(GenCodeInfo genCodeInfo, ABObj choice);
static int write_drawing_area_defs(GenCodeInfo genCodeInfo, ABObj obj);
static int write_list_defs(GenCodeInfo genCodeInfo, ABObj list);
static int write_spinbox_defs(GenCodeInfo genCodeInfo, ABObj spinbox);
static int write_window_defs(GenCodeInfo genCodeInfo, ABObj window);
static int write_file_chooser_defs(GenCodeInfo genCodeInfo, ABObj fchooser);
static int write_termp_defs(GenCodeInfo genCodeInfo, ABObj text);
static int write_define_var(GenCodeInfo genCodeInfo, 
			ISTRING	*struct_field_ptr,
			STRING	type,
			ISTRING	var,
			STRING	value
		);
static int write_define_args_var(GenCodeInfo genCodeInfo, int actual_num_args);
static int write_define_counter_var(GenCodeInfo genCodeInfo);
static int write_define_arg_counter_var(GenCodeInfo genCodeInfo);
static int write_define_display_var(GenCodeInfo genCodeInfo);
static int write_define_drawable_var(GenCodeInfo genCodeInfo);
static int write_define_icon_pixmap_var(GenCodeInfo genCodeInfo);
static int write_define_icon_mask_pixmap_var(GenCodeInfo genCodeInfo);
static int write_define_label_pixmap_var(GenCodeInfo genCodeInfo);
static int write_define_label_xmstr_var(GenCodeInfo genCodeInfo);
static int write_define_acceltext_xmstr_var(GenCodeInfo genCodeInfo);
static int write_define_return_code_var(GenCodeInfo genCodeInfo);
static int write_general_label_defs(GenCodeInfo genCodeInfo, ABObj obj);
static int write_items_names_array_def(GenCodeInfo genCodeInfo, ABObj obj);
static int write_items_strings_array_def(GenCodeInfo genCodeInfo, ABObj obj);
static int write_items_mnemonics_array_def(GenCodeInfo genCodeInfo, ABObj obj);
static int write_items_selected_array_def(GenCodeInfo genCodeInfo, ABObj obj);
static int write_items_widgets_array_def(GenCodeInfo genCodeInfo, ABObj obj);
static int write_single_help_def(GenCodeInfo genCodeInfo, STRING value);
static int write_help_defs(GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_local_vars_for_proc(GenCodeInfo genCodeInfo);

static int write_define_title_str_var(GenCodeInfo genCodeInfo);
static int write_define_value_str_var(GenCodeInfo genCodeInfo);
static int write_define_icon_name_str_var(GenCodeInfo genCodeInfo);
static int write_define_ok_label_xmstr_var(GenCodeInfo genCodeInfo);
static int write_define_directory_xmstr_var(GenCodeInfo genCodeInfo);
static int write_define_pattern_xmstr_var(GenCodeInfo genCodeInfo);

/*************************************************************************
 *************************************************************************
 **                                                                     **
 **       PUBLIC ENTRY POINTS						**
 **									**
 *************************************************************************
 *************************************************************************/

/*
 * Writes out definitions of local variables for the current
 * function (create proc). (No, the name _decls is not really correct).
 */
int
abmfP_write_create_proc_decls(GenCodeInfo genCodeInfo)
{
    int		returnValue= 0;
    int		rc = 0;		/* return code */
    ABObj	obj= genCodeInfo->cur_func.create_obj;

    /*
     * Write out general de
     */
    write_general_label_defs(genCodeInfo, obj);

    /*
     * Write declarations for object
     */
    switch (obj_get_type(obj))
    {
	case AB_TYPE_BUTTON:
	    returnValue = write_button_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_CHOICE:
	    returnValue= write_choice_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_MENU:
	    returnValue= write_menu_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_CONTAINER:
	    switch (obj_get_container_type(obj))
	    {
		case AB_CONT_MENU_BAR:
	            returnValue = write_menubar_defs(genCodeInfo, obj);
		break;
	    }
	break;
	case AB_TYPE_LIST:
	    returnValue= write_list_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_SPIN_BOX:
	    returnValue= write_spinbox_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_COMBO_BOX:
	    returnValue= write_combobox_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_BASE_WINDOW:
	case AB_TYPE_DIALOG:
	    returnValue= write_window_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_DRAWING_AREA:
	    returnValue = write_drawing_area_defs(genCodeInfo,obj);
	break;
	case AB_TYPE_TEXT_FIELD:
	case AB_TYPE_TEXT_PANE:
	    returnValue = write_text_defs(genCodeInfo,obj);
	break;
	case AB_TYPE_FILE_CHOOSER:
	    returnValue = write_file_chooser_defs(genCodeInfo, obj);
	break;
	case AB_TYPE_TERM_PANE:
	    returnValue = write_termp_defs(genCodeInfo, obj);
	break;
    }

    /* Write out help text definitions (if any) */
    returnValue = write_help_defs(genCodeInfo,obj);

    abio_puts(genCodeInfo->code_file, "\n");

    rc = write_assign_local_vars_for_proc(genCodeInfo);
    if (rc > 0)
    {
	abio_puts(genCodeInfo->code_file, "\n");
    }

    return returnValue;
}



/*************************************************************************
 *************************************************************************
 **                                                                     **
 **       OBJECT-SPECIFIC VARIABLE DECLARATIONS				**
 **									**
 *************************************************************************
 *************************************************************************/


static int
write_menu_defs(GenCodeInfo genCodeInfo, ABObj menu)
{
    int num_args = abmfP_get_num_args(menu);

    if (num_args > 0)
    {
    	write_define_args_var(genCodeInfo, num_args);
    	write_define_arg_counter_var(genCodeInfo);
    }
    /*
     * You can have a menu with no items, in which case you want to get the
     * menu pane but don't have any menu items.
     */
    if (obj_get_num_items(menu) != 0)
    {
	if (abmfP_items_have_accelerators(menu))
	    write_define_acceltext_xmstr_var(genCodeInfo);

	else if (abmfP_items_are_homogeneous(menu))
	{
            if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
	    {
	        write_items_names_array_def(genCodeInfo, menu);
	        write_items_strings_array_def(genCodeInfo, menu);
                write_define_counter_var(genCodeInfo);
	        if (abmfP_items_have_mnemonics(menu))
	    	    write_items_mnemonics_array_def(genCodeInfo, menu);
	        write_items_widgets_array_def(genCodeInfo, menu);
	    }
	}
    }
    return 0;
}


static int
write_menubar_defs(GenCodeInfo genCodeInfo, ABObj menubar)
{
    int	num_args = abmfP_get_num_args(menubar);

    write_define_args_var(genCodeInfo, num_args);
    write_define_arg_counter_var(genCodeInfo);

    if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
    {
        /* REMIND: We should check num_items before defining any local 
         * array vars. We're close to FCS, so I don't want to
         * change anything. dunn 30.Apr.95
         */
        write_items_names_array_def(genCodeInfo, menubar);
        write_items_strings_array_def(genCodeInfo, menubar);
        write_define_counter_var(genCodeInfo);
        if (abmfP_items_have_mnemonics(menubar))
            write_items_mnemonics_array_def(genCodeInfo, menubar);
        write_items_widgets_array_def(genCodeInfo, menubar);
    }
    return 0;
}


static int
write_text_defs(GenCodeInfo genCodeInfo, ABObj text)
{
    int num_args = abmfP_comp_get_num_args_of_classes(
			text, ABMF_ARGCLASS_ALL_BUT_WIDGET_REF);
    if (num_args > 0)
    {
        write_define_args_var(genCodeInfo, num_args);
        write_define_arg_counter_var(genCodeInfo);
    }
    if(obj_get_initial_value_string(text) != NULL) {
	write_define_value_str_var(genCodeInfo);
    }
    return 0;
}


static int
write_button_defs(GenCodeInfo genCodeInfo, ABObj obj)
{
    int		num_args = 0;

    if (!obj_has_menu(obj))
    {
	return 0;
    }

    num_args = abmfP_get_num_args(obj);
    if (num_args > 0)
    {
	write_define_args_var(genCodeInfo, num_args);
	write_define_arg_counter_var(genCodeInfo);
    }

    return 0;
}


static int
write_choice_defs(GenCodeInfo genCodeInfo, ABObj choice)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj		item= obj_get_item(choice, 0);
    int			num_items= obj_get_num_items(choice);
    int                 num_args = abmfP_get_num_args(choice);
    int			num_defared_args= num_args + 5; /* for safety */

    if (num_defared_args > 0)
    {
	write_define_args_var(genCodeInfo, num_args);
	write_define_arg_counter_var(genCodeInfo);
    }

    if (num_items > 0)
    {
	abio_printf(codeFile, "Widget\t%s[%d];\n", 
	    abmfP_get_c_array_of_widgets_name(item), num_items);

	if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
	{
	    write_items_names_array_def(genCodeInfo, choice);
	    write_items_strings_array_def(genCodeInfo, choice);
	    write_define_counter_var(genCodeInfo);
            write_items_selected_array_def(genCodeInfo, choice);
	}
    }

    return 0;
}

static int
write_combobox_defs(GenCodeInfo genCodeInfo, ABObj combobox)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               first_item = NULL;
    int                 num_items = 0;

    num_items = obj_get_num_items(combobox);
    first_item= obj_get_item(combobox, 0);

    if (num_items > 0)
    {
        write_define_counter_var(genCodeInfo);
        write_items_names_array_def(genCodeInfo, combobox);
	if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
            write_items_strings_array_def(genCodeInfo, combobox);
    }
    abio_printf(codeFile, "\n");
    return 0;
}


static int
write_drawing_area_defs(GenCodeInfo genCodeInfo, ABObj obj)
{
    int		num_args = 0;
    ABObj	scrollingObj = NULL;

    scrollingObj = objxm_comp_get_subobj(obj, AB_CFG_SCROLLING_OBJ);
    if (scrollingObj == NULL)
    {
	return 0;
    }
    num_args = abmfP_get_num_args(scrollingObj);
    write_define_args_var(genCodeInfo, num_args);
    write_define_arg_counter_var(genCodeInfo);
    return 0;
}


static int
write_list_defs(GenCodeInfo genCodeInfo, ABObj list)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               first_item = NULL;
    ABObj		item= NULL;
    int                 num_args= 0;
    int                 num_items = 0;

    num_items = obj_get_num_items(list);
    first_item= obj_get_item(list, 0);
    num_args += abmfP_get_num_args(list);
    if (num_items > 0)
    {
	num_args+= 2;		/* items array, number of items */
    }
    if (num_args > 0)
    {
	write_define_args_var(genCodeInfo, num_args);
	write_define_arg_counter_var(genCodeInfo);
    }
    if (num_items > 0)
    {
	write_define_counter_var(genCodeInfo);

        if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
	{
            write_items_names_array_def(genCodeInfo, list);
            write_items_strings_array_def(genCodeInfo, list);
	}

	abio_printf(codeFile, "XmString %s[%d];\n",
	    abmfP_get_c_array_of_xmstrings_name(first_item), num_items);
    }
    abio_printf(codeFile, "\n");
    return 0;
}
static int
write_spinbox_defs(GenCodeInfo genCodeInfo, ABObj spinbox)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               first_item = NULL;
    ABObj               item= NULL;
    int                 num_args= 0;
    int                 num_items = 0;

    num_items = obj_get_num_items(spinbox);
    first_item= obj_get_item(spinbox, 0);

    num_args += abmfP_get_num_args(spinbox);
    if (num_items > 0)
    {
        num_args+= 2;           /* items array, number of items */
    }   
    if (num_args > 0)
    {
        write_define_args_var(genCodeInfo, num_args);
        write_define_arg_counter_var(genCodeInfo);
    }   
    if ((num_items > 0) &&
	(obj_get_text_type(spinbox) == AB_TEXT_DEFINED_STRING))
    {
        write_define_counter_var(genCodeInfo);
        write_items_names_array_def(genCodeInfo, spinbox);

        if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
	{
            write_items_strings_array_def(genCodeInfo, spinbox);
	}

        abio_printf(codeFile, "XmString %s[%d];\n",
            abmfP_get_c_array_of_xmstrings_name(first_item), num_items);
    }
    abio_printf(codeFile, "\n");
    return 0;
}


static int
write_window_defs(GenCodeInfo genCodeInfo, ABObj window)
{
    File	codeFile= genCodeInfo->code_file;

    abio_printf(codeFile, "Display	*display= (parent == NULL? NULL:XtDisplay(parent));\n");

    if (obj_is_popup_win(window))
    {
        int		num_args = abmfP_get_num_args(window);
	if (num_args > 0)
	{
            write_define_args_var(genCodeInfo, num_args);
	    write_define_arg_counter_var(genCodeInfo);
	}
    }

    if (obj_get_icon(window) != NULL)
    {
        write_define_icon_pixmap_var(genCodeInfo);
    }
    if (obj_get_icon_mask(window) != NULL)
    {
        write_define_icon_mask_pixmap_var(genCodeInfo);
    }
    if (obj_get_icon_label(window) != NULL)
    {
        write_define_icon_name_str_var(genCodeInfo);
    }
    if (obj_get_label(window) != NULL) 
    {
	write_define_title_str_var(genCodeInfo);
    }

    return 0;
}

int
write_file_chooser_defs(GenCodeInfo genCodeInfo, ABObj fchooser)
{
    ABObj       fsb_obj = objxm_comp_get_subobj(fchooser, AB_CFG_OBJECT_OBJ);
    int         num_args = 0;

    num_args = abmfP_get_num_args(fsb_obj);
    if (num_args > 0)
    {
        write_define_arg_counter_var(genCodeInfo);
        write_define_args_var(genCodeInfo, num_args);
    }
    if (obj_get_ok_label(fchooser) != NULL)
	write_define_ok_label_xmstr_var(genCodeInfo);

    if (obj_get_directory(fchooser) != NULL)
	write_define_directory_xmstr_var(genCodeInfo);

    if (obj_get_filter_pattern(fchooser) != NULL)
	write_define_pattern_xmstr_var(genCodeInfo);

    return 0;
}


/*************************************************************************
 *************************************************************************
 **                                                                     **
 **       Write General-Purpose variable definitions			**
 **									**
 **									**
 **    	These are used by the object-specific write_<obj>_def() 	**
 **	functions.							**
 *************************************************************************
 *************************************************************************/

#define make_istr_var_name(var, const_name) \
	static ISTRING	var = NULL; \
	if ((var) == NULL) \
	{ \
	    (var) = istr_const(const_name); \
	}
	 
/*
 * Utility for writing out a generic local variable definition.
 */
static int
write_define_var(GenCodeInfo genCodeInfo, 
			ISTRING	*struct_field_ptr,
			STRING	type,
			ISTRING	var,
			STRING	value
)
{
#define struct_field (*struct_field_ptr)
    File	codeFile = genCodeInfo->code_file;

    if (struct_field != NULL)
    {
	return 0;
    }

    abio_printf(codeFile, "%s\t%s",
		(type), istr_string(var));
    if (value != NULL)
    {
	abio_printf(codeFile, " = %s", value);
    }
    abio_puts(codeFile, ";\n");
    struct_field = istr_dup(var);

    return 0;
#undef struct_field
}

/*
 * Variable: Arg args[n];
 */
static int
write_define_args_var(GenCodeInfo genCodeInfo, int actual_num_args)
{
    make_istr_var_name(varArgs, "args");

    if (abmfP_args_var(genCodeInfo) == NULL)
    {
	File	codeFile = genCodeInfo->code_file;
	int	num_extra_args = 5;
        int	def_num_args = actual_num_args + num_extra_args;

	abmfP_args_var(genCodeInfo) = istr_dup(varArgs);
	abio_printf(codeFile, "Arg\targs[%d];", def_num_args);
	abio_printf(codeFile, "\t/* need %d args (add %d to be safe) */",
		actual_num_args, num_extra_args);
	abio_puts(codeFile, nlstr);
    }
    return 0;
}


/*
 * var: int i = 0;
 */
static int
write_define_counter_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varCounter, "i");
	write_define_var(genCodeInfo, &abmfP_counter_var(genCodeInfo),
		abmfP_str_int, varCounter, abmfP_str_zero);
    return 0;
}

/*
 * var: int n = 0;
 */
static int
write_define_arg_counter_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varArgCounter, "n");
        write_define_var(genCodeInfo, &abmfP_arg_counter_var(genCodeInfo),
                abmfP_str_int, varArgCounter, abmfP_str_zero);
    return 0;
}


/*
 * Variable: Display *display;
 */
static int
write_define_display_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varDisplay, "display");
	write_define_var(genCodeInfo, &abmfP_display_var(genCodeInfo),
		"Display *", varDisplay, abmfP_str_null);
    return 0;
}


/*
 * var: Drawable drawable = 0;
 */
static int
write_define_drawable_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varDrawable, "drawable");
	write_define_var(genCodeInfo, &abmfP_drawable_var(genCodeInfo),
		"Drawable", varDrawable, abmfP_str_zero);
    return 0;
}


/*
 * var: Pixmap icon_pixmap = 0;
 */
static int
write_define_icon_pixmap_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varIconPixmap, "icon_pixmap");
	write_define_var(genCodeInfo, &abmfP_icon_pixmap_var(genCodeInfo),
		"Pixmap", varIconPixmap, abmfP_str_zero);
    return 0;
}


/*
 * var: Pixmap icon_mask_pixmap = 0;
 */
static int
write_define_icon_mask_pixmap_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varIconPixmap, "icon_mask_pixmap");
	write_define_var(genCodeInfo, &abmfP_icon_mask_pixmap_var(genCodeInfo),
		"Pixmap", varIconPixmap, abmfP_str_zero);
    return 0;
}


/*
 * var: Pixmap label_pixmap = 0;
 */
static int
write_define_label_pixmap_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varLabelPixmap, "label_pixmap");
	write_define_var(genCodeInfo, &abmfP_label_pixmap_var(genCodeInfo),
		"Pixmap", varLabelPixmap, abmfP_str_zero);
    return 0;
}


/*
 * var: XmString label_xmstring = NULL;
 */
static int
write_define_label_xmstr_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varLabelXmStr, "label_xmstring");
	write_define_var(genCodeInfo, &abmfP_label_xmstr_var(genCodeInfo),
		"XmString", varLabelXmStr, abmfP_str_null);
    return 0;
}

/*
 * var: XmString acceltext_xmstring = NULL;
 */
static int
write_define_acceltext_xmstr_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varAccelTextXmStr, "acceltext_xmstring");
    write_define_var(genCodeInfo, &abmfP_acceltext_xmstr_var(genCodeInfo),
                "XmString", varAccelTextXmStr, abmfP_str_null);
    return 0;
}


/*
 * var: int return_code = 0;
 */
static int
write_define_return_code_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varReturnCode, "return_code");
	write_define_var(genCodeInfo, &abmfP_return_code_var(genCodeInfo),
		abmfP_str_int, varReturnCode, abmfP_str_zero);
    return 0;
}


/*
 * var: String title_str = NULL;
 */
static int
write_define_title_str_var(GenCodeInfo genCodeInfo)
{
    /* We only want to do this if the app is being I18N-ized */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
    {
        make_istr_var_name(varTitleString, "title_str");
	write_define_var(genCodeInfo, &abmfP_title_str_var(genCodeInfo),
		"String", varTitleString, abmfP_str_null);
    }
    return 0;
}

/*
 * var: String value_str = NULL;
 */
static int
write_define_value_str_var(GenCodeInfo genCodeInfo)
{
    /* We only want to do this if the app is being I18N-ized */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
    {
        make_istr_var_name(varValueString, "value_str");
	    write_define_var(genCodeInfo, &abmfP_value_str_var(genCodeInfo),
		"String", varValueString, abmfP_str_null);
    }
    return 0;
}
/*
 * var: String icon_name_str = NULL;
 */
static int
write_define_icon_name_str_var(GenCodeInfo genCodeInfo)
{
    /* We only want to do this if the app is being I18N-ized */
    if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
    {
        make_istr_var_name(varIconNameString, "icon_name_str");
	    write_define_var(genCodeInfo, &abmfP_icon_name_str_var(genCodeInfo),
		"String", varIconNameString, abmfP_str_null);
    }
    return 0;
}

/*
 * var: XmString oklabel_xmstring = NULL;
 */
static int
write_define_ok_label_xmstr_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varOkLabelXmStr, "oklabel_xmstring");
    write_define_var(genCodeInfo, &abmfP_ok_label_xmstr_var(genCodeInfo),
                "XmString", varOkLabelXmStr, abmfP_str_null);
    return 0;
}

/*
 * var: XmString dir_xmstring = NULL;
 */
static int
write_define_directory_xmstr_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varDirXmStr, "dir_xmstring");
    write_define_var(genCodeInfo, &abmfP_directory_xmstr_var(genCodeInfo),
                "XmString", varDirXmStr, abmfP_str_null);
    return 0;
}

/*
 * var: XmString pattern_xmstring = NULL;
 */
static int
write_define_pattern_xmstr_var(GenCodeInfo genCodeInfo)
{
    make_istr_var_name(varPatternXmStr, "pattern_xmstring");
    write_define_var(genCodeInfo, &abmfP_pattern_xmstr_var(genCodeInfo),
                "XmString", varPatternXmStr, abmfP_str_null);
    return 0;
}


/*
 * Creates the local variable(s) necessary for the type of label the
 * object has.
 */
static int
write_general_label_defs(GenCodeInfo genCodeInfo, ABObj obj)
{
    File	codeFile = genCodeInfo->code_file;
    BOOL	hasGlyphLabel = FALSE;
    BOOL	hasStringLabel = FALSE;

    if (   abmfP_obj_has_glyph_label(obj)
	|| abmfP_obj_has_item_with_glyph_label(obj))
    {
	hasGlyphLabel = TRUE;
    }
    if (   abmfP_obj_has_string_label(obj)
	|| abmfP_obj_has_item_with_string_label(obj))
    {
	hasStringLabel = TRUE;
    }

    if (hasGlyphLabel)
    {
	write_define_return_code_var(genCodeInfo);
	write_define_display_var(genCodeInfo);
	write_define_label_pixmap_var(genCodeInfo);
	write_define_drawable_var(genCodeInfo);
    }
    if (hasStringLabel)
    {
	write_define_label_xmstr_var(genCodeInfo);
    }

    return 0;
}


/*
 * Defines an array of widget names.
 * Works for any object with items.
 */
static int
write_items_names_array_def(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    int                 num_items= obj_get_num_items(obj);
    int			this_item_num = 0;
    ABObj		item= NULL;
    STRING		strings_array_var_name = NULL;
    AB_TRAVERSAL        trav;
    STRING		widgetName = NULL;

    if (num_items < 1)
    {
	return 0;
    }
    item = obj_get_item(obj, 0);
    strings_array_var_name = abmfP_get_c_array_of_names_name(item);
    abmfP_name_array_var(genCodeInfo) = istr_create(strings_array_var_name);
    abmfP_name_array_var_has_value(genCodeInfo) = TRUE;
    abio_printf(codeFile, "static String\t%s[] =\n", strings_array_var_name);
    abio_puts(codeFile, "{\n");
    abio_indent(codeFile);
    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ), this_item_num = 0;
	 (item = trav_next(&trav)) != NULL; ++this_item_num)
    {
	if ((widgetName = abmfP_get_widget_name(item)) == NULL)
	{
	    abio_printf(codeFile, "NULL");
	}
	else
	{
	    abio_put_string(codeFile, widgetName);
	}
	if (this_item_num < (num_items-1))
	{
	    abio_puts(codeFile, ", ");
	}
    }
    trav_close(&trav);
    abio_puts(codeFile, "\n");
    abio_outdent(codeFile);
    abio_puts(codeFile, "};\n");
    return 0;
}

/****************
 *
 * Build an array of item labels
 * This works for any object with items
 *
 ****************/
static int
write_items_strings_array_def(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    int                 num_items= obj_get_num_items(obj);
    int			this_item_num = 0;
    ABObj		item= NULL;
    STRING		label_array_var_name = NULL;
    AB_TRAVERSAL        trav;

    if (num_items < 1)
    {
	return 0;
    }
    item= obj_get_item(obj, 0);
    label_array_var_name = abmfP_get_c_array_of_strings_name(item);
    abmfP_label_array_var(genCodeInfo) = istr_create(label_array_var_name);
    abio_printf(codeFile, "static String\t%s[] =\n",
		label_array_var_name);
    abio_puts(codeFile, "{\n");
    abio_indent(codeFile);
    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ), this_item_num = 0;
	 (item = trav_next(&trav)) != NULL; ++this_item_num)
    {
	if (obj_get_label(item) == NULL)
	{
	    abio_printf(codeFile, "NULL");
	    abmfP_label_array_has_nulls(genCodeInfo) = TRUE;
	}
	else
	{
	    abio_put_string(codeFile, obj_get_label(item));
	}
	if (this_item_num < (num_items-1))
	{
	    abio_puts(codeFile, ", ");
	}
    }
    trav_close(&trav);
    abio_puts(codeFile, "\n");
    abio_outdent(codeFile);
    abio_puts(codeFile, "};\n");
    return 0;
}

static int
write_items_mnemonics_array_def(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    int                 num_items= obj_get_num_items(obj);
    int                 this_item_num = 0;
    ABObj               item= NULL;
    STRING		mnemonic_array_var_name = NULL;
    STRING		mnemonic;
    AB_TRAVERSAL        trav;

    if (num_items < 1)
    {
        return 0;
    }
    item= obj_get_item(obj, 0);
    mnemonic_array_var_name = abmfP_get_c_array_of_mnemonics_name(item);
    abmfP_mnemonic_array_var(genCodeInfo) = 
		istr_create(mnemonic_array_var_name); 
    abio_printf(codeFile, "static String\t%s[] =\n", mnemonic_array_var_name);
    abio_puts(codeFile, "{\n");
    abio_indent(codeFile);
    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ), this_item_num = 0;
         (item = trav_next(&trav)) != NULL; ++this_item_num)
    {
        if ((mnemonic = obj_get_mnemonic(item)) == NULL)
        {
            abio_printf(codeFile, "NULL");
        }
        else
        {
            abio_put_string(codeFile, mnemonic);
        }
        if (this_item_num < (num_items-1))
        {
            abio_puts(codeFile, ", ");
        }
    }
    trav_close(&trav);
    abio_puts(codeFile, "\n");
    abio_outdent(codeFile);
    abio_puts(codeFile, "};\n");
    return 0;
}


/*
 * Writes out an array of Booleans that determine for each item whether
 * or not it is selected.
 *
 * Works for any object with items
 */
static int
write_items_selected_array_def(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    int                 num_items= obj_get_num_items(obj);
    ABObj		item= NULL;
    int			this_item_num = 0;
    AB_TRAVERSAL        trav;

    if (num_items < 1)
    {
	return 0;
    }
    item= obj_get_item(obj, 0);
    abio_printf(codeFile, "Boolean\t%s[] =\n",
		abmfP_get_c_array_of_selected_bools_name(item));
    abio_puts(codeFile, "{\n");
    abio_indent(codeFile);
    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ), this_item_num = 0;
	 (item = trav_next(&trav)) != NULL; ++this_item_num)
    {
	abio_printf(codeFile, "%s", 
		obj_is_initially_selected(item)? "True":"False");
	if (this_item_num < (num_items-1))
	{
	    abio_puts(codeFile, ", ");
	}
    }
    trav_close(&trav);
    abio_puts(codeFile, "\n");
    abio_outdent(codeFile);
    abio_puts(codeFile, "};\n");
    return 0;
}


/****************
 *
 * Declare an array of item widgets
 * This works for any object with items
 *
 ****************/
static int
write_items_widgets_array_def(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    int                 num_items= obj_get_num_items(obj);
    ABObj		item= NULL;

    if (num_items < 1)
    {
	return 0;
    }
    item= obj_get_item(obj, 0);
    abio_printf(codeFile, "Widget\t%s[%d];\n",
		abmfP_get_c_array_of_widgets_name(item),
		num_items);
    return 0;
}


/* Convenience routine used on a per-help-attribute basis */
static int
write_single_help_def(GenCodeInfo genCodeInfo, STRING value)
{

    File	codeFile = genCodeInfo->code_file;

    if( ((value != 0) && (*value != 0))  ) {
        abio_put_string(codeFile, value);
	abio_puts(codeFile, ",\n");
    }
    else {
        abio_printf(codeFile, "\"\",\n");
    }
	    
    return 0;
}

/* 
 * Write out declarations associated with help attributes 
 */
static int
write_help_defs(GenCodeInfo genCodeInfo, ABObj obj)
{
    STRING 	help_volume, help_location, help_text;
    File	codeFile = genCodeInfo->code_file;

    if(obj_has_help_data(obj)) {
        obj_get_help_data(obj,&help_volume,&help_location,&help_text);

	/*
	** If this is an I18N-ized app, we can't compile in the help text,
	** but must instead load it at runtime via catgets().  We still need
	** the static HelpData info, but we just force the help text part to
	** be a null string (for now).
	*/
	if(genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) {
	    abio_puts(codeFile,"static DtbObjectHelpDataRec\thelp_data = {\n");
	    abio_indent(codeFile);
	    abio_puts(codeFile,"\"\",\n");
	    write_single_help_def(genCodeInfo,help_volume);
	    write_single_help_def(genCodeInfo,help_location);
	    abio_outdent(codeFile);
	    abio_puts(codeFile,"};\n");
	    abio_puts(codeFile,"char *tmp_txt;\n");
	}
	else {
	    abio_puts(codeFile,"static DtbObjectHelpDataRec\thelp_data = {\n");
	    abio_indent(codeFile);
	    write_single_help_def(genCodeInfo,help_text);
	    write_single_help_def(genCodeInfo,help_volume);
	    write_single_help_def(genCodeInfo,help_location);
	    abio_outdent(codeFile);
	    abio_puts(codeFile,"};\n");
	}
    }

    return 0;
}



/*************************************************************************
 *************************************************************************
 **                                                                     **
 **       General-purpose private functions				**
 **									**
 *************************************************************************
 *************************************************************************/


/*
 * Assigns values to local variables that need them.
 */
static int
write_assign_local_vars_for_proc(GenCodeInfo genCodeInfo)
{
    File	codeFile = genCodeInfo->code_file;
    ABObj	activeObj = NULL;
    int		num_written = 0;

    if (abmfP_create_obj(genCodeInfo) != NULL)
    {
	activeObj = abmfP_create_obj(genCodeInfo);
    }
    else if (abmfP_ip_obj(genCodeInfo) != NULL)
    {
	activeObj = abmfP_ip_obj(genCodeInfo);
    }

    if (   (abmfP_display_var(genCodeInfo) != NULL)
        && (!abmfP_display_var_has_value(genCodeInfo)) )
    {
	++num_written;
	abio_printf(codeFile, "%s = XtDisplay(%s);\n",
		istr_string(abmfP_display_var(genCodeInfo)),
		abmfP_get_widget_parent_name(genCodeInfo, activeObj));
	abmfP_display_var_has_value(genCodeInfo) = TRUE;
    }

    if (    (abmfP_drawable_var(genCodeInfo) != NULL)
	&&  (!abmfP_drawable_var_has_value(genCodeInfo)) )
    {
	++num_written;
	abio_printf(codeFile, "%s = DefaultRootWindow(%s);\n",
		istr_string(abmfP_drawable_var(genCodeInfo)),
		istr_string(abmfP_display_var(genCodeInfo)) );
	abmfP_drawable_var_has_value(genCodeInfo) = TRUE;
    }

    return num_written;
}

static int
write_termp_defs(GenCodeInfo genCodeInfo, ABObj text)
{
    int num_args = abmfP_comp_get_num_args_of_classes(
                        text, ABMF_ARGCLASS_ALL_BUT_WIDGET_REF);
    if (num_args > 0)
    {
        write_define_args_var(genCodeInfo, num_args);
        write_define_arg_counter_var(genCodeInfo);
    }
    return 0;
}
