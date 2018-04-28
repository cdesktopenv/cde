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
 * $XConsortium: instances.c /main/4 1996/08/28 20:31:05 mustafa $
 * 
 * @(#)instances.c      3.127 16 Feb 1994       cde_app_builder/src/abmf
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
 * instances.c
 *
 * ALL (and I repeat - ALL) widget creation code is written by
 * this file.
 */

#include <ctype.h>
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <ab_private/AB.h>
#include <ab/util_types.h>
#include <ab_private/util.h>
#include <ab_private/abio.h>
#include <ab_private/trav.h>
#include <ab_private/objxm.h>
#include "abmfP.h"
#include "cdefsP.h"
#include "lib_func_stringsP.h"
#include "write_codeP.h"
#include "write_cP.h"
#include "obj_namesP.h"
#include "argsP.h"
#include "motifdefsP.h"
#include "utilsP.h"
#include "connectP.h"
#include "instancesP.h"
#include "msg_cvt.h"

/* Local functions */
static BOOL     abmfP_subobj_should_be_written(ABObj obj);

static int write_check_null(GenCodeInfo genCodeInfo, ABObj obj);
static int write_check_null_and_return(GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_obj_var(GenCodeInfo genCodeInfo, ABObj obj);
static int write_check_null_and_assign(GenCodeInfo genCodeInfo, ABObj obj);
static int write_end_check_null(GenCodeInfo genCodeInfo, ABObj obj);
static int          write_parent_name(GenCodeInfo genCodeInfo, ABObj obj);
static int write_set_items_glyph_labels(GenCodeInfo genCodeInfo, ABObj obj);
static int write_set_glyph_labels(GenCodeInfo genCodeInfo, ABObj obj);
static int write_set_post_create_args(GenCodeInfo genCodeInfo, ABObj obj);
#ifdef BOGUS
static int write_create_and_attach_all_item_menus(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_create_and_attach_item_menu(
                        GenCodeInfo genCodeInfo, ABObj item);
#endif

static int write_comp_create_all_subobjs(GenCodeInfo genCodeInfo, ABObj obj);
static int write_create_one_item(
			GenCodeInfo	genCodeInfo, 
			ABObj		item,
			STRING		counterVarName
		);
static int write_create_all_items(GenCodeInfo genCodeInfo, ABObj itemsOwner);
static int write_assign_local_vars(GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_local_vars_for_icon(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_local_vars_for_label(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_local_vars_for_acceltext(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_local_vars_for_value(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_assign_local_vars_for_fchooser(
                        GenCodeInfo genCodeInfo, ABObj obj);

static int write_free_local_vars(GenCodeInfo genCodeInfo, ABObj obj);
static int write_free_local_vars_for_label(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_free_local_vars_for_icon(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_free_local_vars_for_acceltext(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_free_local_vars_for_value(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_free_local_vars_for_fchooser(
                        GenCodeInfo genCodeInfo, ABObj obj);
static int write_free_local_xmstring_array(
			GenCodeInfo genCodeInfo, ABObj obj);

static int write_comp_file_chooser(GenCodeInfo genCodeInfo, ABObj fchooser);

static int
write_widget_menu_button(
                         GenCodeInfo genCodeInfo,
                         ABObj obj, int index,
                         int num_submenus,
                         ABObj parent);

static int      write_comp_except_for_actual_obj_tree(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj);

static int      write_comp_except_for_subtree(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj,
                        ABObj           subtree);

static int      write_comp_parents_of_obj(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj);

static void	write_help_setup_for_obj(
			GenCodeInfo	genCodeInfo,
			ABObj		obj);

static void	write_help_setup_for_scrolled_parent(
			GenCodeInfo	genCodeInfo,
			ABObj		obj);

/*
 * Assigns values to the local variables needed to create this
 * object
 */
static int
write_assign_local_vars(GenCodeInfo genCodeInfo, ABObj obj)
{
    write_assign_local_vars_for_icon(genCodeInfo, obj);
    write_assign_local_vars_for_label(genCodeInfo, obj);
    write_assign_local_vars_for_acceltext(genCodeInfo, obj);
    write_assign_local_vars_for_value(genCodeInfo, obj);
    write_assign_local_vars_for_fchooser(genCodeInfo, obj);
    return 0;
}


static int
write_assign_local_vars_for_icon(GenCodeInfo genCodeInfo, ABObj obj)
{
    File	codeFile = genCodeInfo->code_file;
    char	parentName[1024];
    *parentName = 0;

    /*
     * Use the parent as the reference widget to create the pixmaps.
     * This is because this widget doesn't exist, yet!
     */
    if ((obj_get_icon(obj) != NULL) || (obj_get_icon_mask(obj) != NULL))
    {
	/*
        if (obj_is_base_win(obj))
        {
	    sprintf(parentName, "%s()", abmfP_lib_get_toplevel_widget->name);
        }
        else
	*/
        {
	    sprintf(parentName, "%s", abmfP_get_widget_parent_name(genCodeInfo, obj));
	}
    }

    if (obj_get_icon(obj) != NULL)
    {
        abio_printf(codeFile, "%s(%s,\n", 
            abmfP_lib_cvt_image_file_to_pixmap->name, parentName);
        abio_indent(codeFile);
        abio_put_string(codeFile, obj_get_icon(obj));
        abio_printf(codeFile, ", &%s);\n",
            istr_string(abmfP_icon_pixmap_var(genCodeInfo)));
        abio_outdent(codeFile);
        abmfP_icon_pixmap_var_has_value(genCodeInfo) = TRUE;
    }
    if (obj_get_icon_mask(obj) != NULL)
    {
        abio_printf(codeFile, "%s(%s,\n", 
            abmfP_lib_cvt_image_file_to_pixmap->name, parentName);
        abio_indent(codeFile);
        abio_put_string(codeFile, obj_get_icon_mask(obj));
        abio_printf(codeFile, ", &%s);\n",
            istr_string(abmfP_icon_mask_pixmap_var(genCodeInfo)));
        abio_outdent(codeFile);
        abmfP_icon_mask_pixmap_var_has_value(genCodeInfo) = TRUE;
    }

    if ( (obj_get_icon_label(obj) != NULL) &&
    	(abmfP_icon_name_str_var(genCodeInfo) != NULL) &&
    	(!abmfP_icon_name_str_var_has_value(genCodeInfo)) )
    {
        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
        {
    	    char        *label = obj_get_icon_label(obj);
    
    	    abio_printf(codeFile,
    	        "%s = XtNewString(",
    	        istr_string(abmfP_icon_name_str_var(genCodeInfo)));
    
    	    abio_puts(codeFile,
    	          abmfP_catgets_prefix_str(genCodeInfo,
    					obj, label));
    	    abio_put_string(codeFile, label);
    	    abio_puts(codeFile, "));\n");
    	    abmfP_icon_name_str_var_has_value(genCodeInfo) = TRUE;
        }
    }

    return 0;
}


/*
 * Assigns values to local variables to create the label for
 * this object
 */
static
write_assign_local_vars_for_label(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj       wholeObj = NULL;
    ABObj       labelObj = NULL;

    wholeObj = obj_get_root(obj);
    labelObj = objxm_comp_get_subobj(wholeObj, AB_CFG_LABEL_OBJ);

    /*
     * REMIND: fix libABobjXm - the title obj of a popup menu should be
     * the CFG_LABEL_OBJ.
     */
    if ((labelObj == NULL) && obj_is_label(obj) && obj_is_menu(wholeObj))
    {
	labelObj = obj;
    }

    /*
     * If this isn't the label obj, or there is no label, then there's
     * nothing to do.
     */
    if( (labelObj != obj) || (obj_get_label(wholeObj) == NULL)) return 0;

    /*
     * Windows use ascii strings for titles, rather than compound strings
     */
    if (obj_is_window(wholeObj))
    {
	/* Handle window title string */
        if ( (abmfP_title_str_var(genCodeInfo) != NULL)
                    && (!abmfP_title_str_var_has_value(genCodeInfo)) )
        {
            if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
            {
                char        *label = obj_get_label(wholeObj);

                abio_printf(codeFile,
                    "%s = XtNewString(",
                    istr_string(abmfP_title_str_var(genCodeInfo)));

                abio_puts(codeFile,
                      abmfP_catgets_prefix_str(genCodeInfo,
                                                obj, label));
                abio_put_string(codeFile, label);
                abio_puts(codeFile, "));\n");
                abmfP_title_str_var_has_value(genCodeInfo) = TRUE;
            }
        }
    }
    else
    {
        switch (obj_get_label_type(obj))
        {
            case AB_LABEL_STRING:
                if ( (abmfP_label_xmstr_var(genCodeInfo) != NULL)
                    && (!abmfP_label_xmstr_var_has_value(genCodeInfo)) )
                {
                    if (obj_is_item(obj) &&
                        abmfP_label_array_var(genCodeInfo) != NULL &&
                        abmfP_label_array_has_nulls(genCodeInfo))
                    {
                        abio_printf(codeFile,
                            "if (%s[i] != NULL)\n",
                                istr_string(abmfP_label_array_var(genCodeInfo)));
                        abio_indent(codeFile);
                    }
                    abio_printf(codeFile, 
                        "%s = XmStringCreateLocalized(",
                        istr_string(abmfP_label_xmstr_var(genCodeInfo)));

                    if (obj_is_item(obj) &&
                        abmfP_label_array_var(genCodeInfo) != NULL)
                    {
                        abio_printf(codeFile, "%s[i]",
                            istr_string(abmfP_label_array_var(genCodeInfo)));
                    }
                    else
                    {
                        /*
                         * TBD: passing array references for items won't
                         *       work with catgets: need to fix that
                         */
                        /*
                         * TBD: Streamline this and update macros like
                         *      write_str_resource to handle i18n transparently
                         */
                        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
                        {
                            char        *label = obj_get_label(wholeObj);
                        
                            abio_puts(codeFile,
                                      abmfP_catgets_prefix_str(genCodeInfo,
                                                                obj, label));
                            abio_put_string(codeFile, label);
                            abio_puts(codeFile, ")");
                        }
                        else
                        {
                            abio_put_string(codeFile, obj_get_label(wholeObj));
                        }
                    }
                    abio_puts(codeFile, ");\n");
                    if (obj_is_item(obj) &&
                        abmfP_label_array_var(genCodeInfo) != NULL &&
                        abmfP_label_array_has_nulls(genCodeInfo))
                        abio_outdent(codeFile);
                    abmfP_label_xmstr_var_has_value(genCodeInfo) = TRUE;
                }
            break;

            default:
            break;
        }
    }
    return 0;
}

/*
 * Assigns values to local variables to create the Accelerator Text for
 * this object
 */
static
write_assign_local_vars_for_acceltext(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    STRING      accel, acceltext;

    if (obj_is_menu_item(obj) &&
        (accel = obj_get_accelerator(obj)) != NULL)
    {
        if ( (abmfP_acceltext_xmstr_var(genCodeInfo) != NULL)
              && (!abmfP_acceltext_xmstr_var_has_value(genCodeInfo)) )
        {
            acceltext = objxm_accel_to_acceltext(accel);

            abio_printf(codeFile,
                "%s = XmStringCreateLocalized(",
                istr_string(abmfP_acceltext_xmstr_var(genCodeInfo)));

            if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
            {
                abio_puts(codeFile,
                          abmfP_catgets_prefix_str(genCodeInfo,
                                                obj, acceltext));
                abio_put_string(codeFile, acceltext);
                abio_puts(codeFile, ")");
            }
            else
                abio_put_string(codeFile, acceltext);

            abio_puts(codeFile, ");\n");
            abmfP_acceltext_xmstr_var_has_value(genCodeInfo) = TRUE;
        }
    }
    return 0;
}

/*
 * Assigns values to local variables to create the Value for this object
 * (valid only for text fields and text panes).
 */
static
write_assign_local_vars_for_value(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj       wholeObj = obj_get_root(obj);
    ABObj       text_obj = NULL;
    
    if(!obj_is_text(wholeObj)) return 0;
    text_obj = objxm_comp_get_subobj(wholeObj, AB_CFG_OBJECT_OBJ);
    if((obj == text_obj) && (obj_get_initial_value_string(wholeObj) != NULL) )
    {
        if ( (abmfP_value_str_var(genCodeInfo) != NULL)
                    && (!abmfP_value_str_var_has_value(genCodeInfo)) )
        {
            if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
            {
                char        *value = obj_get_initial_value_string(wholeObj);

                abio_printf(codeFile,
                    "%s = XtNewString(",
                    istr_string(abmfP_value_str_var(genCodeInfo)));

                abio_puts(codeFile,
                      abmfP_catgets_prefix_str(genCodeInfo,
                                            obj, value));
                abio_put_string(codeFile, value);
                abio_puts(codeFile, "));\n");
                abmfP_value_str_var_has_value(genCodeInfo) = TRUE;
            }
	}
    }
    return 0;
}

/*
 * Assigns values to local variables to create button label strings for 
 * file choosers
 */
static
write_assign_local_vars_for_fchooser(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj	wholeObj = obj_get_root(obj);
    ABObj	fsb_obj  = NULL;

    if (!obj_is_file_chooser(wholeObj)) 
	return 0;
    fsb_obj = objxm_comp_get_subobj(wholeObj,AB_CFG_OBJECT_OBJ);
    if (obj == fsb_obj)
    {
        if ((obj_get_ok_label(wholeObj) != NULL) &&
	    (abmfP_ok_label_xmstr_var(genCodeInfo) != NULL) &&
	    (!abmfP_ok_label_xmstr_var_has_value(genCodeInfo)))
	{
	    char *ok_label = obj_get_ok_label(wholeObj);

            abio_printf(codeFile,
                "%s = XmStringCreateLocalized(",
                istr_string(abmfP_ok_label_xmstr_var(genCodeInfo)));

            if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
            {
                abio_puts(codeFile,
                          abmfP_catgets_prefix_str(genCodeInfo,
			  obj, ok_label));
                abio_put_string(codeFile, ok_label);
                abio_puts(codeFile, ")");
            }
            else
                abio_put_string(codeFile, ok_label);
            abio_puts(codeFile, ");\n");

            abmfP_ok_label_xmstr_var_has_value(genCodeInfo) = TRUE;
	}

        if ((obj_get_directory(wholeObj) != NULL) &&
	    (abmfP_directory_xmstr_var(genCodeInfo) != NULL) &&
	    (!abmfP_directory_xmstr_var_has_value(genCodeInfo)))      
        {
            char *dirString = obj_get_directory(wholeObj);
 
            abio_printf(codeFile,
                "%s = XmStringCreateLocalized(",
		istr_string(abmfP_directory_xmstr_var(genCodeInfo)));
            abio_put_string(codeFile, dirString);
            abio_puts(codeFile, ");\n");
 
            abmfP_directory_xmstr_var_has_value(genCodeInfo) = TRUE;
        }

        if ((obj_get_filter_pattern(wholeObj) != NULL) &&
            (abmfP_pattern_xmstr_var(genCodeInfo) != NULL) &&
	    (!abmfP_pattern_xmstr_var_has_value(genCodeInfo)))
        {
            char *patString = obj_get_filter_pattern(wholeObj);
 
            abio_printf(codeFile,
                "%s = XmStringCreateLocalized(", 
		istr_string(abmfP_pattern_xmstr_var(genCodeInfo)));
            abio_put_string(codeFile, patString);
            abio_puts(codeFile, ");\n");
 
            abmfP_pattern_xmstr_var_has_value(genCodeInfo) = TRUE;
        }
    }

    return 0;
}

static int 
write_free_local_vars(GenCodeInfo genCodeInfo, ABObj obj)
{
    write_free_local_vars_for_label(genCodeInfo, obj);
    write_free_local_vars_for_icon(genCodeInfo, obj);
    write_free_local_vars_for_acceltext(genCodeInfo, obj);
    write_free_local_vars_for_value(genCodeInfo, obj);
    write_free_local_vars_for_fchooser(genCodeInfo, obj);
    return 0;
}


static int 
write_free_local_vars_for_label(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;

    if (abmfP_label_xmstr_var_has_value(genCodeInfo))
    {
        if (obj_is_item(obj) &&
            abmfP_label_array_var(genCodeInfo) != NULL &&
            abmfP_label_array_has_nulls(genCodeInfo))
        {
            abio_printf(codeFile, "if (%s != NULL)\n", 
                istr_string(abmfP_label_xmstr_var(genCodeInfo)));
            abio_printf(codeFile, "{\n");
            abio_indent(codeFile);
        }
        abio_printf(codeFile, "XmStringFree(%s);\n",
            istr_string(
                abmfP_label_xmstr_var(genCodeInfo)));
        abio_printf(codeFile, "%s = NULL;\n",
            istr_string(
                abmfP_label_xmstr_var(genCodeInfo)));
        if (obj_is_item(obj) &&
            abmfP_label_array_var(genCodeInfo) != NULL &&
            abmfP_label_array_has_nulls(genCodeInfo))
        {
            abio_outdent(codeFile);
            abio_printf(codeFile, "}\n");
        }
        abmfP_label_xmstr_var_has_value(genCodeInfo)= FALSE;
    }
    if (abmfP_title_str_var_has_value(genCodeInfo))
    {
        abio_printf(codeFile, "XtFree(%s);\n",
            istr_string(abmfP_title_str_var(genCodeInfo)));
	abio_printf(codeFile, "%s = NULL;\n",
            istr_string(abmfP_title_str_var(genCodeInfo)));
        abmfP_title_str_var_has_value(genCodeInfo)= FALSE;
    }
    return 0;
}

static int
write_free_local_vars_for_icon(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;

    if (abmfP_icon_name_str_var_has_value(genCodeInfo))
    {
        abio_printf(codeFile, "XtFree(%s);\n",
            istr_string(abmfP_icon_name_str_var(genCodeInfo)));
        abio_printf(codeFile, "%s = NULL;\n",
            istr_string(abmfP_icon_name_str_var(genCodeInfo)));
        abmfP_icon_name_str_var_has_value(genCodeInfo)= FALSE;
    }

    return 0;
}

static int
write_free_local_vars_for_acceltext(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;

    if (abmfP_acceltext_xmstr_var_has_value(genCodeInfo))
    {
        abio_printf(codeFile, "XmStringFree(%s);\n",
            istr_string(
                abmfP_acceltext_xmstr_var(genCodeInfo)));
        abio_printf(codeFile, "%s = NULL;\n",
            istr_string(
                abmfP_acceltext_xmstr_var(genCodeInfo)));
        abmfP_acceltext_xmstr_var_has_value(genCodeInfo)= FALSE;
    }
    return 0;
}

static int
write_free_local_vars_for_value(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;

    if (abmfP_value_str_var_has_value(genCodeInfo))
    {
        abio_printf(codeFile, "XtFree(%s);\n",
            istr_string(abmfP_value_str_var(genCodeInfo)));
        abio_printf(codeFile, "%s = NULL;\n",
            istr_string(abmfP_value_str_var(genCodeInfo)));
        abmfP_value_str_var_has_value(genCodeInfo)= FALSE;
    }
    return 0;
}

static int
write_free_local_vars_for_fchooser(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;

    if (abmfP_ok_label_xmstr_var_has_value(genCodeInfo))
    {
        abio_printf(codeFile, "XmStringFree(%s);\n",
            istr_string(
                abmfP_ok_label_xmstr_var(genCodeInfo)));
        abio_printf(codeFile, "%s = NULL;\n",
            istr_string(
                abmfP_ok_label_xmstr_var(genCodeInfo)));
        abmfP_ok_label_xmstr_var_has_value(genCodeInfo)= FALSE;
    }
    return 0;
}

static int
write_free_local_xmstring_array(
    GenCodeInfo genCodeInfo, 
    ABObj 	obj
)
{
    File        codeFile = genCodeInfo->code_file;
    int		num_items = obj_get_num_items(obj_get_root(obj));

    if (num_items > 0)
    {
        abmfP_write_c_comment(genCodeInfo, FALSE, 
		"Free XmStrings (widget makes private copy of StringList)");
        abio_printf(codeFile, "for (i = 0; i < %d; i++)\n", num_items);
        abio_indent(codeFile);
	abio_printf(codeFile, "XmStringFree(%s[i]);\n\n",
            abmfP_get_c_array_of_xmstrings_name(obj_get_item(obj_get_root(obj),0)));
        abio_outdent(codeFile);
    }
    return 0;
}

/*
 * Determines whether this particular subobj should be written
 * out.  If not, it will probably come into existence out of the
 * creation of another object
 */
static BOOL
abmfP_subobj_should_be_written(ABObj subobj)
{
    BOOL        should_it= TRUE;
    ABObj       compRoot = obj_get_root(subobj);

    if ( ObjWClassIsScrolledWindow(subobj)
         && ( obj_is_list(compRoot) || obj_is_text(compRoot) )
       )
    {
        /* scrolled window containers are created in convenience funcs */
        should_it= FALSE;
    }
    if ( obj_is_file_chooser(compRoot) )
        should_it = FALSE;

    return should_it;
}


static              STRING
abmfP_strip_item_name(char *item_name)
{
    static char         new_name[MAX_NAME_SIZE];
    char               *p;

    snprintf(new_name, sizeof(new_name), "%s", item_name);
    p = (char *) strrchr(new_name, '_');
    if (p != NULL)
        *p = '\0';
    return new_name;
}


/*
 * Write callbacks and actions.
 */
int
abmfP_write_add_callbacks_and_actions(
                        GenCodeInfo     genCodeInfo,
                        ABObj           srcObj
)
{
    File                codeFile = genCodeInfo->code_file;
    AB_TRAVERSAL        trav;
    ABObj               project = obj_get_project(srcObj);
    ABObj               module = obj_get_module(srcObj);
    ABObj               action = NULL;
    ABObj               dest = NULL;
    Arg                *arg = NULL;
    STRING              resource = NULL;
    ABObj               commonSrcObj = obj_get_actual_obj(srcObj);
    ISTRING             istr_resource = NULL;
    ABObj               from = NULL;
    ABObj               to = NULL;
    AB_WHEN             when = AB_WHEN_UNDEF;
    AB_BUILTIN_ACTION   builtin_action = AB_STDACT_UNDEF;
    ABObj               actualSrcSubobj = NULL;
    ABObj               actualTargetSubobj = NULL;
    int                 actionObjCount = 0;
    ABObj               actionObj = NULL;
    ABObj               win_parent, help_btn, help_obj;

    /* 
     * Look in the module and then the project
     */
    for (actionObjCount = 0; actionObjCount < 2; ++actionObjCount)
    {
        switch (actionObjCount)
        {
            case 0:
                actionObj = module;
                break;
            case 1:
                actionObj = project;
                break;
        }
        if (actionObj == NULL)
        {
            continue;
        }
        for (trav_open(&trav, actionObj, AB_TRAV_ACTIONS_FOR_OBJ);
             (action = trav_next(&trav)) != NULL;)
        {

        from = obj_get_from(action);
        if ((from != srcObj) && (from != commonSrcObj))
        {
            continue;
        }

        to = obj_get_to(action);
        when = obj_get_when(action);
        builtin_action = obj_get_func_builtin(action);

        /* post-create procs are not implemented using resources */
        if (from != NULL) switch (when)
        {
        case AB_WHEN_AFTER_CREATED:
                /* not implemented, here */
        break;

        case AB_WHEN_DRAGGED_FROM:
        {
            unsigned    dragOps = obj_get_drag_ops(from);
            unsigned    dragTypes = obj_get_drag_types(from);
            int         numOps = 0;

            actualSrcSubobj = objxm_comp_get_subobj(from, AB_CFG_DND_OBJ);
            abio_printf(codeFile, "%s(%s,\n", 
                abmfP_lib_drag_site_register->name, 
                abmfP_get_c_name(genCodeInfo, actualSrcSubobj));
            abio_indent(codeFile);

            /* protocol */
            abio_printf(codeFile, "%s,\n", abmfP_get_action_name(action));
            abio_printf(codeFile, "%s, ",
                (((dragTypes & ABDndTypeUserDef) != 0)?
                    "DtDND_BUFFER_TRANSFER"
                 :
                     (((dragTypes & ABDndTypeFilename) != 0)?
                         "DtDND_FILENAME_TRANSFER"
                     :
                         "DtDND_TEXT_TRANSFER"
                     )
                ) );

            /* operations */
            numOps = 0;
            abio_printf(codeFile, "(unsigned char)(");
            if ((dragOps & ABDndOpCopy) != 0)
            {
                abio_printf(codeFile, "XmDROP_COPY");
                ++numOps;
            }
            if ((dragOps & ABDndOpLink) != 0)
            {
                abio_printf(codeFile, "%sXmDROP_LINK", 
                        numOps>0?"|":Util_empty_string);
                ++numOps;
            }
            if ((dragOps & ABDndOpMove) != 0)
            {
                abio_printf(codeFile, "%sXmDROP_MOVE", 
                        numOps>0?"|":Util_empty_string);
                ++numOps;
            }
            if (numOps == 0)
            {
                abio_printf(codeFile, "XmDROP_COPY");
                ++numOps;
            }
            abio_printf(codeFile, "),\n");

            /* bufferIsText */
            abio_printf(codeFile, "%s, ",
                ((dragTypes & (ABDndTypeUserDef|ABDndTypeText)) == 0)?
                        "False":"True");
                
            /* allowDropOnRootWindow */
            abio_printf(codeFile, "%s, ",
                (obj_get_drag_to_root_allowed(from)? "True":"False"));

            abio_printf(codeFile, "NULL, NULL, NULL);\n");
            abio_outdent(codeFile);
        }
        break;  /* AB_WHEN_DRAGGED_FROM */

        case AB_WHEN_DROPPED_ON:
        {
            unsigned    dropOps = obj_get_drop_ops(from);
            unsigned    dropTypes = obj_get_drop_types(from);
            int         numProtocols = 0;
            int         numOps = 0;

            actualSrcSubobj = objxm_comp_get_subobj(from, AB_CFG_DND_OBJ);
            abio_printf(codeFile, "%s(%s,\n", 
                abmfP_lib_drop_site_register->name, 
                abmfP_get_c_name(genCodeInfo, actualSrcSubobj));
            abio_indent(codeFile);
            abio_printf(codeFile, "%s,\n", abmfP_get_action_name(action));

            /*** PROTOCOLS ***/
            numProtocols = 0;
            if ((dropTypes & ABDndTypeText) != 0)
            {
                abio_printf(codeFile, "%sDtDND_TEXT_TRANSFER",
                        numProtocols>0?"|":Util_empty_string);
                ++numProtocols;
            }
            if ((dropTypes & ABDndTypeFilename) != 0)
            {
                abio_printf(codeFile, "%sDtDND_FILENAME_TRANSFER",
                        numProtocols>0?"|":Util_empty_string);
                ++numProtocols;
            }
            if ((dropTypes & ABDndTypeUserDef) != 0)
            {
                abio_printf(codeFile, "%sDtDND_BUFFER_TRANSFER",
                        numProtocols>0?"|":Util_empty_string);
                ++numProtocols;
            }
            if (numProtocols == 0)
            {
                abio_printf(codeFile, "DtDND_TEXT_TRANSFER");
            }
            abio_printf(codeFile, ",\n");

            /*** OPERATIONS ***/
            numOps = 0;
            abio_printf(codeFile, "(unsigned char)(");
            if ((dropOps & ABDndOpCopy) != 0)
            {
                abio_printf(codeFile, "XmDROP_COPY");
                ++numOps;
            }
            if ((dropOps & ABDndOpLink) != 0)
            {
                abio_printf(codeFile, "%sXmDROP_LINK", 
                        numOps>0?"|":Util_empty_string);
                ++numOps;
            }
            if ((dropOps & ABDndOpMove) != 0)
            {
                abio_printf(codeFile, "%sXmDROP_MOVE", 
                        numOps>0?"|":Util_empty_string);
                ++numOps;
            }
            if (numOps == 0)
            {
                abio_printf(codeFile, "XmDROP_COPY");
                ++numOps;
            }
            abio_printf(codeFile, "),\n");

            /* dropsOnChildren */
            abio_printf(codeFile, "%s, ",
                (obj_drop_on_children_is_allowed(from)?"True":"False"));
                
            /* preservePreviousRegistration */
            abio_printf(codeFile, "True, ");

            /* dropSiteHandleOut */
            abio_printf(codeFile, "NULL);\n");
            abio_outdent(codeFile);
        }
        break;

        default:
        {
            if (from == commonSrcObj)
            {
                resource = objxm_get_resource_for_when(srcObj, when);
                if (resource == NULL)
                { 
                    /* this when value is unsupported */
                    if (!util_be_silent())
                    {
                        char *src_obj_name = obj_get_name(srcObj);
                        char *when_string  = 
                            util_strsafe(util_when_to_string(when));

                        if(src_obj_name == NULL) {
                            util_printf_err(catgets(Dtb_project_catd, 1, 74,
                               "Ignoring unsupported WHEN - obj:<unnamed> when:%s\n"),
                               when_string);
                        }
                        else {
                            util_printf_err(catgets(Dtb_project_catd, 1, 75,
                               "Ignoring unsupported WHEN - obj:%s when:%s\n"),
                               src_obj_name, when_string);
                        }
                    }
                }
                else
                {
                    istr_resource = objxm_get_res_strname(resource);
                    if ((dest = to) == NULL)
                    {
                        dest = srcObj;
                    }
                    actualSrcSubobj = objxm_comp_get_source_for_when(
                                        srcObj, when);
                    actualTargetSubobj = 
                        objxm_comp_get_target_for_builtin_action(
                                        dest, builtin_action);
		    if (actualTargetSubobj == NULL)
		    {
			/* Although the individual widget is not correct,
			 * this will give us the correct struct.
			 */
			actualTargetSubobj = dest;
		    }
                    abmfP_write_add_callback(
                                                 genCodeInfo,
                                                 istr_string(istr_resource),
                                                 abmfP_get_action_name(action),
                                                 actualSrcSubobj,
                                                 actualTargetSubobj);
                    istr_destroy(istr_resource);
                }
            } /* if (from == commonSourceObj) */
        } /* default: */
        break;
        } /* switch (when) */
        } /* for trav_open(actionObj) */
        trav_close(&trav);
    } /* for actionObjCount */

    return 0;
}


int 
abmfP_write_add_callback(
                         GenCodeInfo genCodeInfo,
                         STRING resource, STRING func_name,
                         ABObj src, ABObj dest
)
{
    File        codeFile = genCodeInfo->code_file;

    if (ObjWClassIsPulldownMenu(src) || ObjWClassIsPopupMenu(src))
    {
        abio_printf(codeFile, "\tXtAddCallback(XtParent(%s),\n\t\t%s, %s,\n",
                abmfP_get_c_name(genCodeInfo, src), resource, func_name);
    }
    else
    {
        abio_printf(codeFile, "\tXtAddCallback(%s,\n\t\t%s, %s,\n",
                abmfP_get_c_name(genCodeInfo, src), resource, func_name);
    }

    /*
    ** if we are unable to obtain the c name for the destination, we are on
    ** a virtual object and should search for the refs to it.
    */
    if (abmfP_get_c_name(genCodeInfo, dest) == NULL)
    {
	ABObjList            refList = abmfP_find_refs_to(dest);
	ABObj                refObj  = NULL;
	int                  numRefs = objlist_get_num_objs(refList);
	int                        i = 0;
	BOOL                 handled = FALSE;
	
	if ((refList != NULL) && (numRefs > 0))
	{
	    for (i = 0; i < numRefs; ++i)
	    {
		refObj = objlist_get_obj(refList, i, NULL);

		if (abmfP_get_c_struct_name(genCodeInfo, refObj) != NULL)
		{
		    abio_printf(codeFile, "\t\t(XtPointer)&%s);\n",
			abmfP_get_c_struct_name(genCodeInfo, refObj));
		    handled = TRUE;
		    break;
		}
	    }
	}

	if (!handled)
	{
	    /*
	    ** is this an acceptable way to recover? possibly not, but the
	    ** resulting code will compile.
	    */
	    util_dprintf(0, "ERROR: abmfP_write_add_callback(): ");
	    util_dprintf(0, "Unable to determine client_data, substituting \"NULL\"\n");
	    abio_printf(codeFile, "\t\t(XtPointer) NULL);\n");
	}
	objlist_destroy(refList);
    }
    else
    {
	abio_printf(codeFile, "\t\t(XtPointer)&%s);\n",
	    abmfP_get_c_struct_name(genCodeInfo, dest));
    }
    abio_outdent(codeFile);
    return 0;
}

int 
abmfP_write_add_callback_with_string(
                         GenCodeInfo    genCodeInfo,
                         STRING         resource, 
                         STRING         func_name,
                         ABObj          src, 
                         STRING         client_data
)
{
    File        codeFile = genCodeInfo->code_file;

    abio_printf(codeFile, "XtAddCallback(%s,\n",
                abmfP_get_c_name(genCodeInfo, src));
    abio_indent(codeFile);
    abio_printf(codeFile, "%s, %s,\n", resource, func_name);
    abio_printf(codeFile, "(XtPointer)%s);\n", client_data);
    abio_outdent(codeFile);
    return 0;
}

/*
 * Write code to convert an array of char* strings to an array of xmstrings
 */
static int
write_convert_strings_to_xmstrings(
                GenCodeInfo     genCodeInfo,
                ABObj           obj)
{
    File        codeFile= genCodeInfo->code_file;
    int         num_items= obj_get_num_items(obj);
    ABObj       first_item= obj_get_item(obj, 0);

    if (num_items < 1)
    {
        return 0;
    }
    abmfP_write_c_comment(genCodeInfo, FALSE, "Convert char* strings to XmStrings");

    if (genCodeInfo->i18n_method != ABMF_I18N_XPG4_API)
    {
        abio_printf(codeFile, "for (i = 0; i < %d; ++i)\n", num_items);
        abmfP_write_c_block_begin(genCodeInfo);

        abio_printf(codeFile, "%s[i] = \n", 
            abmfP_get_c_array_of_xmstrings_name(first_item));
        abio_indent(codeFile);
	abio_puts(codeFile, "XmStringCreateLocalized(\n");
	abio_indent(codeFile);
        abio_printf(codeFile, "%s[i]);\n",
            abmfP_get_c_array_of_strings_name(first_item));

        abio_outdent(codeFile);
        abio_outdent(codeFile);

        abmfP_write_c_block_end(genCodeInfo);
    }
    else
    {
        int     i;

        for (i=0; i < num_items; ++i)
        {
            ABObj       item = obj_get_item(obj, i);
            STRING      label;

            label = obj_get_label(item);

            abio_printf(codeFile, "%s[i++] = XmStringCreateLocalized(", 
                    abmfP_get_c_array_of_xmstrings_name(first_item));

            abio_printf(codeFile, "%s",
                    abmfP_catgets_prefix_str(genCodeInfo, item, label));

            abio_put_string(codeFile, label);

            abio_puts(codeFile, "));\n");
        }
    }

    return 0;
}


static int
write_set_items_glyph_labels(GenCodeInfo genCodeInfo, ABObj obj)
{
    AB_TRAVERSAL        trav;
    ABObj               item = NULL;

    if (!abmfP_obj_has_item_with_glyph_label(obj))
    {
        return 0;
    }

    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
        (item = trav_next(&trav)) != NULL; )
    {
        write_set_glyph_labels(genCodeInfo, item);
    }
    trav_close(&trav);

    return 0;
}


static int
write_set_glyph_labels(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj       rootObj  = obj_get_root(obj);

    if (!obj_is_item(obj))
    {					/* obj is not an item */
	if (abmfP_obj_has_glyph_label(rootObj))
	{
	    ABObj       labelObj = NULL;
	    
	    labelObj = objxm_comp_get_subobj(rootObj, AB_CFG_LABEL_OBJ);
	    if (obj == labelObj)
	    {
		STRING  fileName = obj_get_label(rootObj);

		abio_printf(codeFile, "%s(%s, ",
			    abmfP_lib_set_label_from_image_file->name,
			    abmfP_get_c_name(genCodeInfo, obj));
		abio_put_string(codeFile, fileName);
		abio_puts(codeFile, ");\n");
	    }
	}
    }
    else
    {					/* obj is an item */
	if (abmfP_obj_has_glyph_label(obj))
	{
	    STRING  fileName = obj_get_label(obj);

	    abio_printf(codeFile, "%s(%s, ",
			abmfP_lib_set_label_from_image_file->name,
			abmfP_get_c_name(genCodeInfo, obj));
	    abio_put_string(codeFile, fileName);
	    abio_puts(codeFile, ");\n");
	}
    }

    return 0;
}

static int
write_set_post_create_args(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    int         value;

    write_set_glyph_labels(genCodeInfo, obj);

    switch(obj->type)
    {
        case AB_TYPE_COMBO_BOX:
            /* Bug in Combobox prevents setting width at Create-time */
            if (obj == objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ) &&
                (value = obj_get_width(obj_get_root(obj))) != -1)
            {
                abmfP_write_c_comment(genCodeInfo, FALSE, 
                    "Workaround: ComboBox size resources ignored at Create-time");
                abio_printf(codeFile, "XtVaSetValues(%s, XmNwidth, %d, NULL);\n",
                        abmfP_get_c_name(genCodeInfo, obj), value);
            }
            break;
        default:
            break;
    }
    return 0;
}

/*
 * If useAllCompArgs is TRUE, all the arguments for the composite subobjs
 * will get passed in to the convenience func. Otherwise, only the arguments
 * for this particular object will get passed in.
 */
static int
write_create_widget_by_non_va_conv_func(
                GenCodeInfo     genCodeInfo, 
                ABObj           obj, 
                STRING          conv_func,
                BOOL            useAllCompArgs
)
{
    File        codeFile= genCodeInfo->code_file;
    int         num_args = 0;

    if (obj_was_written(obj))
    {
        return 0;
    }

    write_check_null(genCodeInfo, obj);

    write_assign_local_vars(genCodeInfo, obj);
    num_args = 
        abmfP_get_num_args_of_classes(obj, ABMF_ARGCLASS_ALL_BUT_WIDGET_REF);
    if (num_args > 0)
    {
        abio_printf(codeFile, "%s = 0;\n", 
                istr_string(abmfP_arg_counter_var(genCodeInfo)));
        if (useAllCompArgs)
        {
            ABObj               compRoot = obj_get_root(obj);
            AB_TRAVERSAL        compTrav;
            ABObj               subobj = NULL;
            for (trav_open(&compTrav, compRoot, AB_TRAV_COMP_SUBOBJS);
                (subobj = trav_next(&compTrav)) != NULL; )
            {
                abmfP_obj_spew_args(genCodeInfo, subobj, 
                        ABMF_ARGCLASS_ALL_BUT_WIDGET_REF, ABMF_ARGFMT_ARRAY);
            }
            trav_close(&compTrav);
        }
        else
        {
                abmfP_obj_spew_args(genCodeInfo, obj, 
                        ABMF_ARGCLASS_ALL_BUT_WIDGET_REF, ABMF_ARGFMT_ARRAY);
        }
    }

    write_assign_obj_var(genCodeInfo, obj);
    abio_printf(codeFile, "%s(", conv_func);
    write_parent_name(genCodeInfo, obj);
    abio_indent(codeFile);
    abio_printf(codeFile, "\"%s\"", abmfP_get_widget_name(obj));
    if (num_args > 0)
    { /* ( vi hack */
        abio_printf(codeFile, ", args, %s);\n", 
                istr_string(abmfP_arg_counter_var(genCodeInfo)));
    }
    else
    { /* ( vi hack */
        abio_printf(codeFile, ", NULL, 0);\n");
    }
    abio_outdent(codeFile);
    abio_outdent(codeFile);

    /*
     * Set any post-create resources, if any
     */
    write_set_post_create_args(genCodeInfo, obj);

    /*
     * Set up help callback if necessary
     */
    write_help_setup_for_obj(genCodeInfo, obj);

    write_free_local_vars(genCodeInfo, obj);
    write_end_check_null(genCodeInfo, obj);

    obj_set_was_written(obj, TRUE);
    return 0;
}



/*
 * Create just like your average, ordinary widget
 */
static int
write_create_widget_by_xt_va_create(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj
)
{
    File        codeFile= genCodeInfo->code_file;

    if (obj_was_written(obj))
    {
        return 0;
    }

    write_check_null(genCodeInfo, obj);
    write_assign_local_vars(genCodeInfo, obj);
    write_assign_obj_var(genCodeInfo, obj);
    if (obj_is_item(obj))
        abio_printf(codeFile, "XtVaCreateManagedWidget(");
    else
        abio_printf(codeFile, "XtVaCreateWidget(");

    /* widget name, widget class & parent name */
    abio_printf(codeFile, "\"%s\",\n", abmfP_get_widget_name(obj));
    abio_indent(codeFile);
    abio_printf(codeFile, "%s,\n", obj_get_class_name(obj));
    write_parent_name(genCodeInfo, obj);

    abmfP_obj_spew_args(genCodeInfo, 
        obj, ABMF_ARGCLASS_ALL_BUT_WIDGET_REF, ABMF_ARGFMT_VA_LIST);
    abmfP_xt_va_list_close(genCodeInfo);
    abio_outdent(codeFile);

    /*
     * Set any post-create resources, if any
     */  
    write_set_post_create_args(genCodeInfo, obj);

    /*
     * Set up help callback if necessary
     */
    write_help_setup_for_obj(genCodeInfo, obj);

    write_free_local_vars(genCodeInfo, obj);
    write_end_check_null(genCodeInfo, obj);

    obj_set_was_written(obj, TRUE);
    return 0;
}


static int
write_create_widget_normally(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = -1;
    BOOL        wroteIt = FALSE;

    write_assign_local_vars(genCodeInfo, obj);
    if (ObjWClassIsScrolledWindow(obj))
    {
        wroteIt = TRUE;
        return_value = write_create_widget_by_non_va_conv_func(
                genCodeInfo, obj, "XmCreateScrolledWindow", FALSE);
    }

    if (!wroteIt)
    {
        return_value = write_create_widget_by_xt_va_create(genCodeInfo, obj);
    }
    write_free_local_vars(genCodeInfo, obj);

    return return_value;
}


static int
write_comp_create_menu_button(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    int         return_value = 0;
    int         rc = 0;
    ABObj       menuObj = NULL;
    
    rc = write_create_widget_by_non_va_conv_func(genCodeInfo,
                obj, "DtCreateMenuButton", FALSE);
    return_if_err(rc,rc);

    /*
     * Grab the menu pane from the menu button
     */
    menuObj = obj_find_child_by_type(obj, AB_TYPE_MENU);
    if (menuObj == NULL)
    {
        return_code(-1);
    }

    abio_printf(codeFile, "XtVaGetValues(%s,\n", 
        abmfP_get_c_name(genCodeInfo, obj));
    abio_indent(codeFile);
    abio_printf(codeFile, "XmNsubMenuId, &(%s),\n",
        abmfP_get_c_name(genCodeInfo, menuObj));
    abio_puts(codeFile, "NULL);\n");
    abio_outdent(codeFile);
epilogue:
    return return_value;
}


static int
write_comp_create_button(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = 0;

    if (!obj_has_menu(obj))
    {
        return_value = write_comp_create_all_subobjs(genCodeInfo, obj);
    }
    else
    {
        return_value = write_comp_create_menu_button(genCodeInfo, obj);
    }

    return return_value;
}


/*
 *
 */
static int
write_comp_create_choice(GenCodeInfo genCodeInfo, ABObj obj)
{
   int          rc = 0;         /* return code */
   ABObj        actualObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
   ABObj        itemParentObj = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);
   int          numItems = obj_get_num_items(obj);
   ABObj        labelObj = objxm_comp_get_subobj(obj, AB_CFG_LABEL_OBJ);

    if (obj_get_choice_type(obj) == AB_CHOICE_OPTION_MENU)
    {
        /*
         * for option menus, we need to write out the menu pane
         * before the actual choice object
         *
         * This is a little strange - if the choice object has a label
         * then the "menu" object is a sibling of the actual choice
         * control.  If there is no label, the menu object is a child
         * of the actual control.
         */
        if (itemParentObj != NULL) 
        {
            if (   (actualObj != NULL)
                && (!obj_is_descendant_of(itemParentObj, actualObj)) )
            {
                write_comp_parents_of_obj(genCodeInfo, itemParentObj);
            }
            abmfP_write_create_widgets_for_one_obj(genCodeInfo, itemParentObj);
        }
    }

    write_comp_create_all_subobjs(genCodeInfo, obj);

    return 0;
}

static int
write_comp_file_chooser(GenCodeInfo genCodeInfo, ABObj fchooser)
{
    /* fchooser is the shell that contains a panedWindow which
     * containes the actual file chooser object. We want to
     * generate code for the real file chooser object, so we
     * first have to get the handle to that object.
     */  
    ABObj       fsb_obj = objxm_comp_get_subobj(fchooser, AB_CFG_OBJECT_OBJ);

    write_create_widget_by_non_va_conv_func(
                genCodeInfo, fsb_obj, "XmCreateFileSelectionDialog", FALSE);

    /* can't use get_c_name twice in the same printf! (static buf) */
    abio_printf(genCodeInfo->code_file, "%s = ",
                abmfP_get_c_name(genCodeInfo, fchooser));
    abio_printf(genCodeInfo->code_file, "XtParent(%s);\n",
                abmfP_get_c_name(genCodeInfo, fsb_obj));
 
    abio_printf(genCodeInfo->code_file, "XtVaSetValues(%s,\n",
                abmfP_get_c_name(genCodeInfo, fchooser));

    /* NOTE: As long as all we do in this XtVaSetValues() call is set
     * the fchooser's title, it's o.k. to have the in-line call to catgets()
     * (in the I18N case only, obviously).  If we were to set additional
     * strings here, then we'd need a local variable to hold them so we
     * didn't wind up cascading calls to catgets() and clobbering strings
     * held in its internal buffer.
     */
    abio_indent(genCodeInfo->code_file);
    abmfP_write_arg_val(genCodeInfo, FALSE, XmNtitle,
                (void *)obj_get_label(fchooser), fsb_obj);
    abio_putc(genCodeInfo->code_file, ',');
    abmfP_xt_va_list_close(genCodeInfo);
 
    /*
     * Set up help callback if necessary
     */
    write_help_setup_for_obj(genCodeInfo, fchooser);

    return 0;
}
 


/*
 * Makes sure the scrollbar is written before the term pane
 */
static int
write_comp_create_term_pane(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = 0;
    int         rc = 0;         /* return code */
    ABObj       actualObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    rc = write_comp_except_for_subtree(genCodeInfo, obj, actualObj);
    return_if_err(rc,rc);

    rc = write_comp_create_all_subobjs(genCodeInfo, obj);
    return_if_err(rc,rc);

epilogue:
   return return_value;
}


static int
write_comp_create_text(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = 0;
    ABObj       scrollingObj = objxm_comp_get_subobj(obj, AB_CFG_SCROLLING_OBJ);
    ABObj       objectObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

    if (scrollingObj != NULL)
    {
        write_comp_except_for_subtree(genCodeInfo, obj, scrollingObj);
        write_create_widget_by_non_va_conv_func(
                genCodeInfo, objectObj, "XmCreateScrolledText", TRUE);

        /* can't use get_c_name twice in the same printf! (static buf) */
        abio_printf(genCodeInfo->code_file, "%s = ",
                abmfP_get_c_name(genCodeInfo, scrollingObj));
        abio_printf(genCodeInfo->code_file, "XtParent(%s);\n", 
                abmfP_get_c_name(genCodeInfo, objectObj));

	/* If the TextPane is initially invisible, we must Unmanage the
	 * ScrolledWindow because XmCreateScrolledText automatically
	 * manages it.
	 */
	if (!obj_is_initially_visible(obj))
	{
            abmfP_write_c_comment(genCodeInfo, FALSE,
                    "XmCreateScrolledText automatically manages the ScrolledWindow");
	    abio_printf(genCodeInfo->code_file, "XtUnmanageChild(%s);\n",
		abmfP_get_c_name(genCodeInfo, scrollingObj));
	}
    }
    else
    {
        return_value = write_comp_create_all_subobjs(genCodeInfo, obj);
    }

    return return_value;
}


/*
 *
 */
static int
write_obj_create_base_win(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj       project = obj_get_project(obj); 
    BOOL	wasAppShell = mfobj_has_flags(obj, CGenFlagTreatAsAppShell);
    STRING	className = NULL;

    /* always treat as non-primary shell, so that any other instances
     * created by the application programmer will pick up the proper
     * resources.
     */
    mfobj_clear_flags(obj, CGenFlagTreatAsAppShell);
    write_check_null(genCodeInfo, obj);
    write_assign_local_vars(genCodeInfo, obj);
    write_assign_obj_var(genCodeInfo, obj);

    abio_printf(codeFile, "XtVaCreatePopupShell(");
    abio_indent(codeFile);

    abio_printf(codeFile, "\"%s\",\n", obj_get_name(obj));
    abio_printf(codeFile, "%s,\n", 
	(abmfP_get_root_window(obj_get_project(obj)) == obj)?
	    istr_string(abmfP_topLevelShell)
	:
	    obj_get_class_name(obj));
    abio_printf(codeFile, "parent,\n");

    abmfP_obj_spew_args(genCodeInfo, 
        obj, ABMF_ARGCLASS_ALL_BUT_WIDGET_REF, ABMF_ARGFMT_VA_LIST);
    if (!obj_get_resizable(obj))
    {
	abio_printf(codeFile, 
	    "XmNmwmDecorations, MWM_DECOR_ALL | MWM_DECOR_RESIZEH,\n");
	abio_printf(codeFile,
	    "XmNmwmFunctions, MWM_FUNC_ALL | MWM_FUNC_RESIZE,\n");
    }
    abmfP_xt_va_list_close(genCodeInfo);

    abio_outdent(codeFile);

    /*
     * Set up help callback if necessary
     */
    write_help_setup_for_obj(genCodeInfo, obj);

    write_free_local_vars(genCodeInfo, obj);
    write_end_check_null(genCodeInfo, obj);
    obj_set_was_written(obj, TRUE);
    if (wasAppShell)
    {
	mfobj_set_flags(obj, CGenFlagTreatAsAppShell);
    }
    return 0;
}


/*
 * Writes the code for the actual choice and its items
 */
static int
write_obj_create_for_choice(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = 0;
    ABObj       wholeObj = obj_get_root(obj);
    ABObj       actualObj = objxm_comp_get_subobj(wholeObj, AB_CFG_OBJECT_OBJ);

    if (obj == actualObj)
    {
        if (obj_get_choice_type(wholeObj) == AB_CHOICE_OPTION_MENU)
        {
            return_value = write_create_widget_by_non_va_conv_func(
                genCodeInfo, obj, "XmCreateOptionMenu", FALSE);
        }
        else
        {
            return_value = write_create_widget_by_non_va_conv_func(
                genCodeInfo, obj, "XmCreateRadioBox", FALSE);
        }
    }
    else
    {
        return_value = write_create_widget_by_xt_va_create(genCodeInfo, obj);
    }

    return return_value;
}


/*
 *
 */
static int
write_obj_create_dialog(GenCodeInfo genCodeInfo, ABObj dialog)
{
    return write_create_widget_by_non_va_conv_func(
                genCodeInfo, dialog, "XmCreateDialogShell", FALSE);
}


/*
 * Writes the code for the actual list and its items
 * Assumes: list is the actual list control
 */
static int
write_list(GenCodeInfo genCodeInfo, ABObj list)
{
    ABObj       wholeObj = NULL;
    ABObj       scrollObj = NULL;
    ABObj       item;
    int		num_items= obj_get_num_items(list);
    int		i;

    write_convert_strings_to_xmstrings(genCodeInfo, list);
    genCodeInfo->cur_func.create_obj = list;
    write_create_widget_by_non_va_conv_func(
                genCodeInfo, list, "XmCreateScrolledList", FALSE);

    for(i=0; i < num_items; i++)
    {
	item = obj_get_item(list, i);
	if (obj_is_initially_selected(item))
	    abio_printf(genCodeInfo->code_file, "XmListSelectPos(%s, %d, False);\n",
		abmfP_get_c_name(genCodeInfo, list), i+1);
    }

    wholeObj= obj_get_root(list);
    scrollObj= objxm_comp_get_subobj(wholeObj, AB_CFG_SCROLLING_OBJ);
    if (scrollObj != NULL)
    {
        /* can't use get_c_name twice in the same printf! (static buf) */
        genCodeInfo->cur_func.create_obj = scrollObj;
        abio_printf(genCodeInfo->code_file, "%s = ",
                abmfP_get_c_name(genCodeInfo, scrollObj));
        genCodeInfo->cur_func.create_obj = list;
        abio_printf(genCodeInfo->code_file, "XtParent(%s);\n", 
                abmfP_get_c_name(genCodeInfo, list));

    }
    write_free_local_xmstring_array(genCodeInfo, wholeObj);

    return 0;
}

/*
 * Writes the code for the actual SpinBox and its items
 */
static int
write_spinbox(GenCodeInfo genCodeInfo, ABObj spinbox)
{
    ABObj       wholeObj = obj_get_root(spinbox);

    if (obj_get_text_type(wholeObj) == AB_TEXT_DEFINED_STRING)
        write_convert_strings_to_xmstrings(genCodeInfo, spinbox);

    write_create_widget_by_non_va_conv_func(
                genCodeInfo, spinbox, "DtCreateSpinBox", FALSE);

    if (obj_get_text_type(wholeObj) == AB_TEXT_DEFINED_STRING)
	write_free_local_xmstring_array(genCodeInfo, wholeObj);

    return 0;
}

static int
write_menu(GenCodeInfo genCodeInfo, ABObj menu)
{
    int         return_value = 0;
    if (obj_is_popup(menu))
    {
        return_value = write_create_widget_by_non_va_conv_func(
                        genCodeInfo, menu, "XmCreatePopupMenu", FALSE);
    }
    else
    {
        return_value = write_create_widget_by_non_va_conv_func(
                        genCodeInfo, menu, "XmCreatePulldownMenu", FALSE);
    }
    return return_value;
}


static int
write_menu_bar(GenCodeInfo genCodeInfo, ABObj mbar)
{
    write_create_widget_by_non_va_conv_func(
        genCodeInfo, mbar, "XmCreateMenuBar", FALSE);
    return 0;
}


/*
 * If the object has items, write them out here.
 */
static int
write_create_all_items(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               firstItem= obj_get_item(obj, 0);

    if ((firstItem == NULL) || (obj_was_written(firstItem)))
    {
        return 0;               /* no kids! */
    }

    if (obj_is_list_item(firstItem) ||
        obj_is_spin_box_item(firstItem))
    {
        /* The list items are not widgets and were created as */
        /* XmStrings before the  widget was created, so we ignore */
        /* them here */
    }
    else
    {
        AB_TRAVERSAL    trav;
        ABObj           item = NULL;
        char            widgetArrayName[256] = "";

	/*
	 * Create the title and separator, if this is a menu
	 */
	if (obj_is_menu(obj))
	{
	    ABObj	itemParent = obj_get_parent(firstItem);
	    ABObj	child = NULL;

	    if (itemParent != NULL)
	    {
		/* we'll just create widgets up to the first item */
		for (trav_open(&trav, itemParent, AB_TRAV_CHILDREN);
			(((child = trav_next(&trav)) != NULL) 
			    && (!obj_is_item(child))); )
		{
		    abmfP_write_create_widgets_for_one_obj(genCodeInfo, child);
		}
	    }
	}

        if ((genCodeInfo->i18n_method != ABMF_I18N_XPG4_API) &&
            abmfP_items_are_homogeneous(obj) &&
            !abmfP_items_have_accelerators(obj)) /* Create Items in a Loop */
        {
	    /*
	     * Create the items in a loop
	     */
            strcpy(widgetArrayName,  abmfP_get_c_array_of_widgets_name(firstItem));
            abio_printf(codeFile, "for (i = 0; i < XtNumber(%s); i++)\n",
                    abmfP_get_c_array_of_strings_name(firstItem));
            abio_printf(codeFile, "{\n");
            abio_indent(codeFile);
            write_create_one_item(genCodeInfo, firstItem, "i");
            abio_outdent(codeFile);
            abio_printf(codeFile, "}\n");

	    /*
	     * Since we only generate code for one item, any "dumped" resources
	     * for the other items must be explicitly dumped to the resource
	     * file.
	     */
	    for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ), 
		item = trav_next(&trav);	/* skip first */
		    (item = trav_next(&trav)) != NULL; )
	    {
		abmfP_obj_spew_args(genCodeInfo, item, 
		    ABMF_ARGCLASS_ALL, ABMF_ARGFMT_UNDEF);
	    }
	    trav_close(&trav);

            if (!obj_is_combo_box_item(firstItem))
            {
                /*
                 * Assign the values of the local array into the global
                 * widget variables
                 */
                for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
                     (item = trav_next(&trav)) != NULL; )
                {
                    abio_printf(codeFile, "%s = %s[%d];\n",
                        abmfP_get_c_name(genCodeInfo, item),
                        widgetArrayName,
                        obj_get_item_num(item));
                }
                trav_close(&trav);
            }
        }
        else /* Roll out Creation of each item */
        {
            if (!obj_is_combo_box_item(firstItem))
            {
                for (trav_open(&trav, obj, AB_TRAV_ITEMS_FOR_OBJ);
                    (item = trav_next(&trav)) != NULL; )
                {   
                    abmfP_write_create_widgets_for_one_obj(genCodeInfo, item);
                }
            }
            else
            {
                int     i;

                for (i=0; i < obj_get_num_items(obj); ++i)
                {
                    ABObj       item = obj_get_item(obj, i);

                    abio_printf(codeFile, "\n");
                    write_create_one_item(genCodeInfo, item, NULL);
                }
            }
        }
    }
    write_set_items_glyph_labels(genCodeInfo, obj);

    return 0;
}


#ifdef BOGUS
/*
 * Creates a menu if it has not already been created and attaches it to the
 * parent via the XmNsubMenuId resource.
 */
static int
write_create_and_attach_all_item_menus(GenCodeInfo genCodeInfo, ABObj obj)
{
    ABObj       item = NULL;
    AB_TRAVERSAL        trav;
    int                 num_items = obj_get_num_items(obj);
    int                 num_written = 0;

    if (num_items <= 0)
    {
        return 0;
    }

    abio_puts(genCodeInfo->code_file, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE, "Create and attach the submenus");
    for (trav_open(&trav, obj, AB_TRAV_ITEMS);
        (item = trav_next(&trav)) != NULL; )
    {
        write_create_and_attach_item_menu(genCodeInfo, item);
        ++num_written;
    }
    trav_close(&trav);

    return num_written;
}


/*
 * creates a menu if it has not already been created and attaches it to the
 * parent via the XmNsubMenuId resource.
 */
static int
write_create_and_attach_item_menu(GenCodeInfo genCodeInfo, ABObj item)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               menu_ref = obj_get_menu(item);
    ABObj               menu = obj_get_actual_obj(item);
    char                menuVar[256];
    char                parentVar[256];

    if ((menu_ref == NULL) || (menu == NULL))
    {
        /* this item does not have a menu */
        return 0;
    }

    strncpy(parentVar, abmfP_get_widget_parent_name(genCodeInfo, menu_ref), 256);
    parentVar[255]= 0;
    strncpy(menuVar, abmfP_get_c_name(genCodeInfo, menu_ref), 256);
    menuVar[255] = 0;

    /* call the menu's create function to create this menu_ref */
    abio_puts(codeFile, "\n");

    write_check_null(genCodeInfo, menu_ref);
    abio_printf(codeFile, "%s(\"%s\",\n",
                abmfP_get_create_proc_name(menu),
                abmfP_get_widget_name(menu_ref));
    abio_indent(codeFile);
    abio_printf(codeFile, "%s,\n", parentVar);

    /*
     * Write out return-widget pointer parameters
     */
    {
        AB_TRAVERSAL    trav;
        ABObj           item = NULL;
        int             numItems= obj_get_num_items(menu);
        int             thisItemNum = -1;

        abio_printf(codeFile, "&(%s)", menuVar);
        if (numItems > 0)
        {
            abio_puts(codeFile, ",");
        }
        abio_puts(codeFile, "\n");

        for (trav_open(&trav, menu, AB_TRAV_ITEMS_FOR_OBJ);
            (item = trav_next(&trav)) != NULL; )
        {
            abio_printf(codeFile, "&(%s)",
                abmfP_get_c_name(genCodeInfo, item));
            thisItemNum = obj_get_item_num(item);
            if (thisItemNum < (numItems-1))
            {
                abio_puts(codeFile, ",");
            }
            abio_puts(codeFile, "\n");
        }
        trav_close(&trav);
    }
    abio_printf(codeFile, "%s\n", ");");
    abio_outdent(codeFile);

    abmfP_write_c_block_end(genCodeInfo);

    abio_printf(codeFile, "if (%s == NULL)\n", menuVar);
    abio_indent(codeFile);
    abio_puts(codeFile, "return FALSE;\n"),
    abio_outdent(codeFile);

    /* attach the menu to the item */
    abio_printf(codeFile, "XtVaSetValues(%s,\n", 
                abmfP_get_c_name(genCodeInfo, item));
    abio_indent(codeFile);
    abio_printf(codeFile, "XmNsubMenuId, %s,\n", menuVar);
    abio_puts(codeFile, "NULL);\n");
    abio_outdent(codeFile);

    return TRUE;
}                               /* write_create_and_attach_all_item_menus */
#endif /* BOGUS */


static BOOL
ancestor_is_menu(ABObj obj)
{
    AB_TRAVERSAL        trav;
    AB_OBJ             *ancestor;
    BOOL                isit = FALSE;

    if (!obj_is_choice(obj) || obj_is_group(abmfP_parent(obj)))
        return FALSE;
    for (trav_open(&trav, obj, AB_TRAV_PARENTS);
         (ancestor = trav_next(&trav)) != NULL;)
    {
        if (ObjWClassIsMenuShell(ancestor))
        {
            isit = TRUE;
            break;
        }
    }
    trav_close(&trav);

    return isit;
}

/**********
 * end of MENU STUFF
 *********/


/*
 * Writes creation of all the parents of this object that are
 * in the composite object.
 */
static int
write_comp_parents_of_obj(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj
)
{
#define MAX_ANCESTORS 100
    ABObj       compRoot = obj_get_root(obj);
    ABObj       ancestors[MAX_ANCESTORS];
    ABObj       ancestor = NULL;
    int         numAncestors = 0;
    int         i = 0;

    if (compRoot == NULL)
    {
        return -1;
    }

    for (ancestor = abmfP_parent(obj); 
         ancestor != NULL; ancestor = abmfP_parent(ancestor))
    {
        assert(numAncestors < (MAX_ANCESTORS-1));
        ancestors[numAncestors++] = ancestor;
        if (ancestor == compRoot)
        {
            break;
        }
    }

    for (i = 0; i < numAncestors; ++i)
    {
        ancestor = ancestors[i];
        if (!obj_was_written(ancestor))
        {
            abmfP_write_create_widgets_for_one_obj(genCodeInfo, ancestor);
        }
    }

    return 0;
#undef MAX_ANCESTORS
}


/*
 * Writes out all the widgets in the composite object, except for
 * the actual control object and it's descendants
 */
static int
write_comp_except_for_actual_obj_tree(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj
)
{
    ABObj       actualObj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);
    return write_comp_except_for_subtree(genCodeInfo, obj, actualObj);
}


/*
 * Writes out the subobjs of the object, except for the subtree
 * specified
 *
 * If subtree is NULL, all subobjects are written
 */
static int
write_comp_except_for_subtree(
                        GenCodeInfo     genCodeInfo,
                        ABObj           obj,
                        ABObj           subtree
)
{
    AB_TRAVERSAL        trav;
    ABObj               subobj= NULL;
    ABObj               comp_root= obj_get_root(obj);

    /*
     * Skip the subtree entirely
     */
    if (   (subtree != NULL)
        && (   (obj == subtree)
            || (obj_is_descendant_of(obj, subtree))))
    {
        return 0;
    }

    /*
     * Write out this one widget (maybe)
     */
    if (abmfP_subobj_should_be_written(obj))
    {
        abmfP_write_create_widgets_for_one_obj(genCodeInfo, obj);
    }

    /*
     * Write out each child that is also part of this composite object.
     */
    if (comp_root == NULL)
    {
        return 0;
    }
    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
        (subobj= trav_next(&trav)) != NULL; )
    {
        if ((subobj != obj) && (obj_get_root(subobj) == comp_root) )
        {
            write_comp_except_for_subtree(genCodeInfo, subobj, subtree);
        }
    }
    trav_close(&trav);
    return 0;
}


/*
 * Writes all subobjs associated with a composiite object
 */
static int
write_comp_create_all_subobjs(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = 0;
    int         rc = 0;         /* return code */

    rc = write_comp_except_for_subtree(genCodeInfo, obj, NULL);
    return_if_err(rc,rc);

    rc = write_create_all_items(genCodeInfo, obj);
    return_if_err(rc,rc);

epilogue:
    return return_value;
}


/*
 * If counterVarName is not NULL, it is assumed that this item is
 * being created in a loop
 */
static int
write_create_one_item(
			GenCodeInfo	genCodeInfo, 
			ABObj		item,
			STRING		counterVarName
)
{
    File                codeFile = genCodeInfo->code_file;
    AB_TRAVERSAL	itemTrav;
    ABObj		itemParent = NULL;
    StringListRec	inactiveItemsRec;
    StringList		inactiveItems = &inactiveItemsRec;
    int			selectedIndex = 0;
    int			numInactiveItems = 0;
    ABObj		siblingItem = NULL;
    int			i = 0;
    char		buf[256];
    char		widgetArrayVarName[MAX_NAME_SIZE];
    ABMF_ARG_CLASS	argClasses = 0;
    *buf = 0;
    *widgetArrayVarName = 0;
    strlist_construct(&inactiveItemsRec);

    itemParent = obj_get_parent(obj_get_root(item));
    if (itemParent != NULL)
    {
	itemParent = obj_get_root(itemParent);
    }

    write_assign_local_vars(genCodeInfo, item);

    if (obj_is_combo_box_item(item))
    {
        ABObj parent = objxm_comp_get_subobj(obj_get_parent(item), AB_CFG_PARENT_OBJ);
    	int   num_items = obj_get_num_items(itemParent);
	int   i;

        abio_printf(codeFile, "DtComboBoxAddItem(%s, %s, 0, True);\n",
                abmfP_get_c_name(genCodeInfo, parent),
                istr_string(abmfP_label_xmstr_var(genCodeInfo)));

	/* Determine which item is selected, and if it's any item other
	 * than the first one (the default), write code to select it.
	 */
	for(i = 0; i < num_items; i++)
	    if (obj_is_initially_selected(obj_get_item(itemParent, i)))
		break;

	if (i != 0)
	{
	    abio_printf(codeFile, "if (%s == %d)\n", counterVarName, i);
	    abio_indent(codeFile);
	    abio_printf(codeFile, "DtComboBoxSelectItem(%s, %s);\n",
		abmfP_get_c_name(genCodeInfo, parent),
                istr_string(abmfP_label_xmstr_var(genCodeInfo)));
	    abio_outdent(codeFile);
	}
    }
    else
    {
        switch (obj_get_item_type(item))
        {
            case AB_ITEM_FOR_CHOICE:
            case AB_ITEM_FOR_MENU:
            case AB_ITEM_FOR_MENUBAR:
		util_strncpy(widgetArrayVarName, 
                    abmfP_get_c_array_of_widgets_name(item), MAX_NAME_SIZE);
                abio_printf(codeFile, "%s[i] = ", widgetArrayVarName);
                break;
            default:
                break;
        }

        /*
         * Widget name
         */
        obj_set_was_written(item, TRUE);
        abio_puts(codeFile, "XtVaCreateWidget(");
	if (abmfP_name_array_var_has_value(genCodeInfo))
	{
	    abio_printf(codeFile, "%s[i]", 
		istr_string(abmfP_name_array_var(genCodeInfo)));
	}
	else
	{
            abio_put_string(codeFile, abmfP_get_widget_name(item));
	}
        abio_puts(codeFile, ",\n");
        abio_indent(codeFile);

        /*
         * Class
         */
        abio_printf(codeFile, "%s,\n", obj_get_class_name(item));

        /*
         * Parent
         */
        abio_printf(codeFile, "%s,\n", 
        abmfP_get_c_name(genCodeInfo, abmfP_parent(item)));

        /*
         * Arguments
         */
	argClasses = ABMF_ARGCLASS_ALL_BUT_WIDGET_REF;
	if (counterVarName != NULL)
	{
	    /* don't set sensitivity inside a loop */
	    argClasses &= ~ABMF_ARGCLASS_SENSITIVITY;
	}
        abmfP_obj_spew_args(genCodeInfo, 
                item, argClasses, ABMF_ARGFMT_VA_LIST);
        abmfP_xt_va_list_close(genCodeInfo);

	if ((counterVarName != NULL) && (!util_strempty(widgetArrayVarName)))
	{
	    /*
	     * Set additional arguments
	     * REMIND: This should probably be put into a local array, as well
	     */
	    for (i = 0, trav_open(&itemTrav, itemParent, AB_TRAV_ITEMS);
		    (siblingItem = trav_next(&itemTrav)) != NULL; ++i)
	    {
	        if (!obj_is_initially_active(siblingItem))
	        {
		    sprintf(buf, "%d", i);
		    strlist_add_str(inactiveItems, buf, NULL);
	        }
		if (obj_is_initially_selected(siblingItem))
		    selectedIndex = i;
	    }
	    trav_close(&itemTrav);
    
	    /* Could form: if (i == X)
	     *	       if ((i == X) || (i == Y) || (i == Z)) ...
	     */
	    numInactiveItems = strlist_get_num_strs(inactiveItems);
	    if (numInactiveItems > 0)
	    {
	        abio_printf(codeFile, "if (");
	        for (i = 0; i < numInactiveItems; ++i)
	        {
		    if (i > 0)
		    {
		        abio_printf(codeFile, " || ");
		    }
	            if (numInactiveItems > 1)
	            {
		        abio_printf(codeFile, "(");
	            }
	            abio_printf(codeFile, "%s == %s",
                        counterVarName,
		        strlist_get_str(inactiveItems, i, NULL));
	            if (numInactiveItems > 1)
	            {
		        abio_printf(codeFile, ")");
	            }
	        }
	        abio_printf(codeFile, ")\n");
                abmfP_write_c_block_begin(genCodeInfo);
		abio_printf(codeFile, "XtSetSensitive(%s[%s], False);\n",
			widgetArrayVarName, counterVarName);
                abmfP_write_c_block_end(genCodeInfo);
	    }

	    /* If OptionMenu has an item other than the first(default) selected,
	     * we must generate special code to set this "current" selection.
	     */
	    if (obj_is_option_menu(itemParent) && selectedIndex > 0)
	    {
		abio_printf(codeFile, "if (%s == %d) /* Set selected item */\n",
			counterVarName, selectedIndex);
                abmfP_write_c_block_begin(genCodeInfo);
		abio_printf(codeFile, "XtVaSetValues(%s, XmNmenuHistory, %s[%s], NULL);\n",
			abmfP_get_c_name(genCodeInfo, 
			    objxm_comp_get_subobj(itemParent, AB_CFG_OBJECT_OBJ)),
			    widgetArrayVarName, counterVarName);
                abmfP_write_c_block_end(genCodeInfo);
	    }
		
	} /* counterVarName != NULL */
    }
    write_free_local_vars(genCodeInfo, item);

    strlist_destruct(&inactiveItemsRec);
    return 0;
}

static int
write_widget_file_chooser_shell(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;

    obj_set_was_written(obj, TRUE);
    abio_printf(codeFile, "\t\tXmCreateFileSelectionDialog(");
    write_parent_name(genCodeInfo, obj);
    abio_printf(codeFile, "\t\t\t\"%s\",\n", abmfP_get_widget_name(obj));
    abio_printf(codeFile, "\t\t\tNULL, 0 );\n");

    abio_printf(codeFile, "\tXtVaSetValues(%s,\n", abmfP_get_c_name(genCodeInfo, obj));
    abmfP_obj_spew_args(genCodeInfo, 
        obj, ABMF_ARGCLASS_ALL_BUT_WIDGET_REF, ABMF_ARGFMT_VA_LIST);
    abmfP_xt_va_list_close(genCodeInfo);

    abio_printf(codeFile, "\tif (%s == NULL)\n\t\treturn FALSE;\n\n",
                abmfP_get_c_name(genCodeInfo, obj));

    return TRUE;
}


static int
write_parent_name(GenCodeInfo genCodeInfo, ABObj obj)
{
    abio_printf(genCodeInfo->code_file, "%s,\n",
        abmfP_get_widget_parent_name(genCodeInfo, obj));
    return 0;
}


STRING
abmfP_get_widget_parent_name(GenCodeInfo genCodeInfo, ABObj obj)
{
    static      char parentName[MAX_NAME_SIZE] = "";
    ABObj       parent = abmfP_parent(obj);
    ABObj       wholeObj = obj_get_root(obj);
    ABObj       createObj = abmfP_create_obj(genCodeInfo);
    ABObj       topAncestorObj = NULL;
    BOOL        parentParamExists = abmfP_parent_param_has_value(genCodeInfo);
    BOOL        parentFound = FALSE;
    *parentName = 0;

    /*
     * Don't search above the parent param
     */
    if ((createObj != NULL) && parentParamExists)
    {
        topAncestorObj = abmfP_parent(createObj);
    }

    if (   (genCodeInfo->writing_func) 
        && (createObj == obj)
        && (parentParamExists) )
    {
        util_strncpy(parentName, 
            istr_string(abmfP_parent_param(genCodeInfo)), MAX_NAME_SIZE);
        parentFound = TRUE;
    }

    /*
     * For option menus, the menu pane that holds the choice items
     * is actually parented to the parent of the choice.
     *
     * Don't blame me! Motif did it! :-)
     */
    if ((!parentFound) && (obj_is_choice(wholeObj)))
    {
        if (   (wholeObj != NULL) 
            && (obj_get_choice_type(wholeObj) == AB_CHOICE_OPTION_MENU))
        {
            ABObj itemParentObj = 
                objxm_comp_get_subobj(wholeObj, AB_CFG_PARENT_OBJ);
            if (itemParentObj == obj)
            {
                ABObj   actualObj = objxm_comp_get_subobj(
                                        wholeObj, AB_CFG_OBJECT_OBJ);
                ABObj   widgetParent = NULL;
                
                if (actualObj != NULL)
                {
                    widgetParent = abmfP_parent(actualObj);
                }
                if (widgetParent != NULL)
                {
                    snprintf(parentName, sizeof(parentName), "%s", 
                        abmfP_get_c_name(genCodeInfo, widgetParent));
                    parentFound = TRUE;
                }
            }
        }
    } /* if obj_is_choice */

    /*
     * Look for a parent that exists in the generated code.
     */
    if ((!parentFound) && (parent != NULL))
    {
        ABObj   ancestor= parent;

        while (   ((ancestor != NULL) 
               && (ancestor != topAncestorObj)
               && (!abmfP_subobj_should_be_written(ancestor))) )
        {
            ancestor= abmfP_parent(ancestor);
        }

        /*
         * Menus are not parented to items - parent to the item's parent
         */
        while (   (ancestor != NULL) 
               && (ancestor != topAncestorObj)
               && (obj_is_item(ancestor)) )
        {
            ancestor = abmfP_parent(ancestor);
        }

        if (ancestor != NULL)
        {
            if (ancestor == topAncestorObj)
            {
                util_strncpy(parentName, 
                        istr_string(abmfP_parent_param(genCodeInfo)),
                        MAX_NAME_SIZE);
                parentFound = TRUE;
            }
            else if (obj_is_ui(ancestor))
            {
                util_strncpy(parentName, 
                    abmfP_get_c_name(genCodeInfo, ancestor), MAX_NAME_SIZE);
                parentFound = TRUE;
            }
        }
    }

    return (parentFound? parentName:NULL);
}


static int
child_isnumeric(ABObj child)
{
    return ((child->type == AB_TYPE_TEXT_FIELD)
            && (child->info.text.type == AB_TEXT_NUMERIC));
}


static int
write_check_null(GenCodeInfo genCodeInfo, ABObj obj)
{
    abio_printf(genCodeInfo->code_file, 
        "if (%s == NULL) {\n", abmfP_get_c_name(genCodeInfo, obj));
    abio_indent(genCodeInfo->code_file);
    return 0;
}


static int
write_assign_obj_var(GenCodeInfo genCodeInfo, ABObj obj)
{
    abio_printf(genCodeInfo->code_file,
        "%s =\n", abmfP_get_c_name(genCodeInfo, obj));
    abio_indent(genCodeInfo->code_file);
    return 0;
}


static int
write_check_null_and_assign(GenCodeInfo genCodeInfo, ABObj obj)
{
    write_check_null(genCodeInfo, obj);
    write_assign_local_vars(genCodeInfo, obj);
    write_assign_obj_var(genCodeInfo, obj);
    return 0;
}


static int
write_end_check_null(GenCodeInfo genCodeInfo, ABObj obj)
{
    obj= obj;   /* avoid warning */
    abio_outdent(genCodeInfo->code_file);
    abio_printf(genCodeInfo->code_file, "}\n");
    return 0;
}

/* 
** This single function does all the help setup for a particular object.
** Two types of setup support are required:
**   1) Any object that has help data should have an XmNhelpCallback installed,
**      using standard dtb_ routines to post the proper help dialog, etc.
**   2) Custom Dialogs don't have an XmNhelpCallback resource!  (Don't blame
**      me, blame Motif).  To get help to work for them, the help data must
**      be installed on the uppermost child of the dialog via its 
**      XmNhelpCallback, and the dialog's help button object must have an
**      XmNactivateCallback added to call the proper helpCallback.
*/
static void	
write_help_setup_for_obj(GenCodeInfo genCodeInfo, ABObj	obj)
{
    File	codeFile = genCodeInfo->code_file;
    ABObj	wholeObj = NULL;
    ABObj	helpObj  = NULL;
    ABObj	actualObj  = NULL;
    ABObj	testObj  = NULL;
    ABObj	win_parent = NULL;
    ABObj	help_btn = NULL;
    STRING	help_volume, help_location, help_text;

    /* 
    ** Support for setup case #1 (see comment above)
    **
    ** Get the overall root for this object, and see if it has any help data
    ** to be handled.
    */
    wholeObj = obj_get_root(obj);
    if(obj_has_help_data(wholeObj)) {

	testObj    = obj;
        helpObj    = objxm_comp_get_subobj(wholeObj, AB_CFG_HELP_OBJ);
        actualObj  = objxm_comp_get_subobj(wholeObj, AB_CFG_OBJECT_OBJ);

        /* 
        ** See if the current object is the root object's help object.  If so,
        ** then we need to set up the help callback.  Special case handling
	** is necessary for label-less lists and for textpanes because they are
	** both created in the _ui.c file using XmCreate...() convenience
	** functions that implicitly create the ScrolledWindow parent upon
	** which we need to hang help.
        */
	if((obj_is_list(obj) || obj_is_text_pane(obj)) && (obj == actualObj) ) {
	    ABObj scrollObj = objxm_comp_get_subobj(wholeObj,
			AB_CFG_SCROLLING_OBJ);
	    if(scrollObj == helpObj) {
		write_help_setup_for_scrolled_parent(genCodeInfo,obj);
	    }
	    return;
	}
		
        if(testObj == helpObj) {

            abmfP_write_c_comment(genCodeInfo, FALSE, "Add help callback");

            /* Must fetch help text from message catalog for I18N-ized app */
            if(genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) {
                obj_get_help_data(wholeObj,&help_volume,&help_location,
			&help_text);
                abio_printf(codeFile,"tmp_txt = %s",
                    abmfP_catgets_prefix_str(genCodeInfo,wholeObj,help_text));
                abio_put_string(codeFile,help_text);
                abio_puts(codeFile, ");\n");

                abio_printf(codeFile,
                    "help_data.help_text = XtMalloc(strlen(tmp_txt)+1);\n");
                abio_printf(codeFile,
                    "strcpy(help_data.help_text,tmp_txt);\n");
                abio_puts(codeFile,"\n");
            }

            /* Help callback is wired in the same regardless of I18N-ization */
            abmfP_write_add_callback_with_string(genCodeInfo,"XmNhelpCallback",
                "dtb_help_dispatch", helpObj, "&help_data");
	}
    }
    /*
    ** Support for setup case #2 (see comment above)
    **
    ** If the current object is a push button, examine the
    ** help button attribute of its window parent to see if this object
    ** happens to be the help button for that dialog.  If so, wire in
    ** the help callback.
    */
    if(obj_is_button(obj)) {
        /* Get dialog parent (if any) */
        win_parent = obj_get_parent_of_type(obj,AB_TYPE_DIALOG);
        if(win_parent != (ABObj) NULL) {

           /* Find out which obj is the dialog's help button obj */
           help_btn = obj_get_help_act_button(win_parent);

           /* Is this obj that help button obj?? */
           if(obj == help_btn) {
               /* Yep, so let's wire in help */
               helpObj = objxm_comp_get_subobj(win_parent,AB_CFG_HELP_OBJ);
               abmfP_write_c_comment(genCodeInfo, FALSE, "Add help callback");
               abmfP_write_help_indirect_callback(genCodeInfo,obj,helpObj);
           }
        }
    }
}

static void	
write_help_setup_for_scrolled_parent(GenCodeInfo genCodeInfo, ABObj obj)
{
    File        codeFile = genCodeInfo->code_file;
    ABObj	wholeObj  = obj_get_root(obj);
    STRING	help_volume, help_location, help_text;

    abmfP_write_c_comment(genCodeInfo, FALSE, "Add help callback");

    /* Must fetch help text from message catalog for I18N-ized app */
    if(genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) {
	obj_get_help_data(wholeObj,&help_volume,&help_location,
		&help_text);
	abio_printf(codeFile,"tmp_txt = %s",
	    abmfP_catgets_prefix_str(genCodeInfo,wholeObj,help_text));
	abio_put_string(codeFile,help_text);
	abio_puts(codeFile, ");\n");

	abio_printf(codeFile,
	    "help_data.help_text = XtMalloc(strlen(tmp_txt)+1);\n");
	abio_printf(codeFile,
	    "strcpy(help_data.help_text,tmp_txt);\n");
	abio_puts(codeFile,"\n");
    }


    /* 
    ** Help callback is wired in the same regardless of I18N-ization,
    ** though we have to wire it in directly using XtParent() to get
    ** a handle to the scrolled window that's the parent of the list
    */
    abio_printf(codeFile, "XtAddCallback(XtParent(%s),\n",
                abmfP_get_c_name(genCodeInfo, obj));
    abio_indent(codeFile);
    abio_printf(codeFile, "XmNhelpCallback, dtb_help_dispatch,\n");
    abio_printf(codeFile, "(XtPointer)&help_data);\n");
    abio_outdent(codeFile);
}

/*************************************************************************
 **                                                                     **
 **             PUBLIC FUNCTIONS                                        **
 **                                                                     **
 *************************************************************************/

/*
 *
 */
int
abmfP_write_create_widgets_for_one_obj(GenCodeInfo genCodeInfo, ABObj obj)
{
    int                 return_value= 0;
    File                codeFile = genCodeInfo->code_file;
    int                 iRC= 0;         /* int return code */
    STRING              glyph_file_name = NULL; /* For messages that are
                                                 * glyphs, */
    ABObj               wholeObj = NULL;
    ABObj               actualObj= NULL;
    ABObj               parentObj = NULL;
    ABObj               item= NULL;
    BOOL                objIsActualObj = FALSE;
    BOOL                objIsParentObj = FALSE;
    int                 startingIndent = abio_get_indent(codeFile);
    BOOL                handledIt = FALSE;

    if (   obj_has_flag(obj, NoCodeGenFlag)
        || (obj_was_written(obj)))
    {
        goto epilogue;
    }

    wholeObj = obj_get_root(obj);
    if (wholeObj != NULL)
    {
        actualObj = objxm_comp_get_subobj(wholeObj, AB_CFG_OBJECT_OBJ);
        parentObj = objxm_comp_get_subobj(wholeObj, AB_CFG_PARENT_OBJ);
    }
    objIsActualObj = (actualObj == obj);
    objIsParentObj = (parentObj == obj);

/*
    if ((!handledIt) && obj_is_item(obj))
    {
        handledIt = TRUE;
    }
*/
    if ((!handledIt) && obj_is_choice(wholeObj) && objIsActualObj)
    {
        handledIt = TRUE;
        write_obj_create_for_choice(genCodeInfo, obj);
    }
    if ((!handledIt)  && obj_is_choice(wholeObj) && objIsParentObj)
    {
        handledIt = TRUE;
        write_create_widget_by_non_va_conv_func(
                genCodeInfo, obj, "XmCreatePulldownMenu", FALSE);
    }
    if ((!handledIt) && obj_is_list(obj) && objIsActualObj)
    {
        handledIt = TRUE;
        write_list(genCodeInfo, obj);
    }
/*
    if ((!handledIt) && obj_is_file_chooser(obj))
    {
        handledIt = TRUE;
        write_check_null_and_assign(genCodeInfo, obj);
        iRC= write_widget_file_chooser_shell(genCodeInfo, obj);
        write_free_local_vars(genCodeInfo, obj); 
        write_end_check_null(genCodeInfo, obj);
        return_if_err(iRC,iRC);
        
    }
*/
    if ((!handledIt) && obj_is_menu(obj))
    {
        handledIt = TRUE;
        iRC = write_menu(genCodeInfo, obj);
        return_if_err(iRC,iRC);
    }
    if ((!handledIt) && obj_is_menubar(obj))
    {
        handledIt = TRUE;
        write_menu_bar(genCodeInfo, obj);
    }
    if ((!handledIt) && obj_is_popup_win(obj))
    {
        handledIt = TRUE;
        write_obj_create_dialog(genCodeInfo, obj);
    }
    if ((!handledIt) && (obj_is_base_win(obj)))
    {
        handledIt = TRUE;
        write_obj_create_base_win(genCodeInfo, obj);
    }
    if ((!handledIt) && obj_is_spin_box(obj) && objIsActualObj)
    {
        handledIt = TRUE;
        write_spinbox(genCodeInfo, obj);
    }
    if ((!handledIt) && obj_is_text(obj))
    {
        STRING          convFunc = NULL;
        if (ObjWClassIsTextField(obj))
        {
            convFunc = "XmCreateTextField";
        }
        else if (ObjWClassIsText(obj))
        {
            convFunc = "XmCreateText";
        }
        if (convFunc != NULL)
        {
            handledIt = TRUE;
            write_create_widget_by_non_va_conv_func(
                genCodeInfo, obj, convFunc, FALSE);
        }
    }

    /*
     * Standard widget creation
     */
    if (!handledIt)
    {
        handledIt = TRUE;
        write_create_widget_normally(genCodeInfo, obj);
    } /* !handledIt */

    abio_printf(codeFile, "if (%s == NULL)\n",
                abmfP_get_c_name(genCodeInfo, obj));
    abio_indent(codeFile);
    abio_puts(codeFile, "return -1;\n\n");
    abio_outdent(codeFile);

epilogue:
    abio_set_indent(codeFile, startingIndent);  /* someone may have missed */
    return return_value;
}


int
abmfP_write_create_widgets_for_comp_obj(GenCodeInfo genCodeInfo, ABObj obj)
{
    int         return_value = 0;

    if (!obj_is_root(obj))
    {
        return -1;
    }

    switch (obj_get_type(obj))
    {
        case AB_TYPE_CHOICE:
            return_value = write_comp_create_choice(genCodeInfo, obj);
        break;

        case AB_TYPE_BUTTON:
            return_value = write_comp_create_button(genCodeInfo, obj);
        break;

        case AB_TYPE_TERM_PANE:
            return_value = write_comp_create_term_pane(genCodeInfo, obj);
        break;

        case AB_TYPE_TEXT_PANE:
        case AB_TYPE_TEXT_FIELD:
            return_value = write_comp_create_text(genCodeInfo, obj);
        break;

        case AB_TYPE_FILE_CHOOSER:
            return_value = write_comp_file_chooser(genCodeInfo, obj);
        break;

        default:
            return_value = write_comp_create_all_subobjs(genCodeInfo, obj);
        break;
    }

    return return_value;
}

int
abmfP_write_help_indirect_callback(
        GenCodeInfo genCodeInfo,
        ABObj src_obj, 
        ABObj help_obj
)
{
    File        codeFile = genCodeInfo->code_file;

    abio_printf(codeFile,"XtAddCallback(%s, XmNactivateCallback,\n",
	abmfP_get_c_name(genCodeInfo, src_obj));
    abio_indent(codeFile);
    abio_printf(codeFile,"dtb_call_help_callback, (XtPointer)%s);\n",
	abmfP_get_c_name(genCodeInfo, help_obj));
    abio_outdent(codeFile);
    
    return (0);
}


BOOL
ObjWClassIsCompositeSubclass(ABObj obj)
{
    return (
	ObjWClassIsComposite(obj)
	    || ObjWClassIsConstraint(obj)
	        || ObjWClassIsManagerSubclass(obj)
	    || ObjWClassIsShellSubclass(obj)
    );
}


BOOL
ObjWClassIsManagerSubclass(ABObj obj)
{
    return (   
	ObjWClassIsComposite(obj)
    	 || ObjWClassIsConstraint(obj)
	    || ObjWClassIsManager(obj)
	        || ObjWClassIsBulletinBoard(obj)
	    	    || ObjWClassIsForm(obj)
	    	    || ObjWClassIsSelectionBox(obj)
	    	    || ObjWClassIsMessageBox(obj)
	    		|| ObjWClassIsCommand(obj)
	    		|| ObjWClassIsFileSelectionBox(obj)
	        || ObjWClassIsDrawingArea(obj)
	        || ObjWClassIsFrame(obj)
	        || ObjWClassIsPanedWindow(obj)
	        || ObjWClassIsRowColumn(obj)
		/* Scale is technically a composite, but we'll ignore that
		 * little piece of Motif brain damage
		 */
	        || ObjWClassIsScrolledWindow(obj)
	            || ObjWClassIsMainWindow(obj)
	 /*
	  * Our pseudo-classes
	  */
	 || (ObjWClassIsPulldownMenu(obj))
	 || (ObjWClassIsPopupMenu(obj))
    );
}


BOOL
ObjWClassIsShellSubclass(ABObj obj)
{
    return (
	(ObjWClassIsShell(obj))
	    || (ObjWClassIsOverrideShell(obj))
	        || (ObjWClassIsMenuShell(obj))
	    || (ObjWClassIsWMShell(obj))
	        || (ObjWClassIsVendorShell(obj))
	 	    || (ObjWClassIsTopLevelShell(obj))
	    		|| (ObjWClassIsApplicationShell(obj))
	 	    || (ObjWClassIsTransientShell(obj))
	 		|| (ObjWClassIsDialogShell(obj))
	 || (ObjWClassIsFileSelectionBox(obj))	/* hack! */
    );
}

