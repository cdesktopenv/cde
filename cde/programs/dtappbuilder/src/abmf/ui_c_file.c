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
 * $XConsortium: ui_c_file.c /main/3 1995/11/06 18:15:01 rswiston $
 * 
 *      @(#)ui_c_file.c	3.115 21 Mar 1995
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
 * Write a file containing object initialization functions (module_ui.c).
 * These functions will be compiled and linked with the application.
 * 
 * 
 */

#include <string.h>
#include <stdio.h>
#include <ab_private/AB.h>
#include <ab/util_types.h>
#include <ab_private/abio.h>
#include <ab_private/objxm.h>
#include <ab_private/strlist.h>
#include "abmfP.h"
#include "argsP.h"
#include "utilsP.h"
#include "instancesP.h"
#include "cdefsP.h"
#include "create_declsP.h"
#include "write_cP.h"
#include "obj_namesP.h"
#include "ui_header_fileP.h" 	/* write_widget_specific_includes */
#include "ui_c_fileP.h"
#include "lib_func_stringsP.h"
#include "msg_cvt.h"

static int	write_all_obj_clear_procs(
			GenCodeInfo genCodeInfo, ABObj rootObj);
static int	write_all_obj_create_procs(GenCodeInfo genCodeInfo, ABObj rootObj);
static int	write_call_create_proc(
				GenCodeInfo genCodeInfo, 
				ABObj obj);
static int	write_manage_widgets(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj
		);
static int	write_manage_one_tree(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj
		);
static int	write_manage_descendants(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj
		);
static int write_out_mainwin_set_areas(GenCodeInfo genCodeInfo, ABObj obj);
static int write_out_dialog_set_pane_height(GenCodeInfo genCodeInfo, ABObj obj);
static int write_add_widget_ref_resources(GenCodeInfo genCodeInfo, ABObj root);
static BOOL	obj_should_be_managed_when_initialized(ABObj obj);
static BOOL	abmfP_obj_has_unmanaged_child(ABObj obj);
static int	write_action_target_includes(
			GenCodeInfo	genCodeInfo, 
			ABObj		module
		);

static int	write_call_all_user_post_create_procs(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj
		);
static int	write_call_user_post_create_procs(
			GenCodeInfo genCodeInfo, 
			ABObj obj
		);
static int 	write_msg_init_proc(
		    GenCodeInfo	genCodeInfo, 
		    ABObj 	parent
		);
static int 	write_msg_clear_proc(
		    GenCodeInfo genCodeInfo,
		    ABObj       module
		);

static int 	abmfP_obj_get_num_conns_by_when(
		    ABObj	obj,
		    AB_WHEN	when
		);

static ABObj	abmfP_obj_get_conn_by_when(
		    ABObj	obj,
		    AB_WHEN	when,
		    int		which_conn
		);

static StringList abmfP_get_msg_action_list(
		    ABObj   msg_obj
		);


static STRING
strip_spaces_and_dots(char *mname)
{
    static char         new_name[MAXPATHLEN];
    char               *p;

    snprintf(new_name, sizeof(new_name), "%s", mname);
    p = (char *) strrchr(new_name, '.');
    if (p)
	p = (char *) strrchr(p, '.');
    if (p)
	p = (char *) strrchr(p, '.');
    if (p)
	p = (char *) strrchr(p, ' ');
    return new_name;
}


/*
 * Traverse object list and write out Widget decls.
 */
static int
write_all_shared_var_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    File                codeFile = genCodeInfo->code_file;
    AB_TRAVERSAL        trav;
    ABObj               ui_obj = NULL;

    /*
     * Define the widgets in this module
     */
    for (trav_open(&trav, module, AB_TRAV_UI);
	 (ui_obj = trav_next(&trav)) != NULL;)
    {
	if (obj_is_window(ui_obj))
	{
	    abio_printf(codeFile, "\n%s\t%s = \n",
		    abmfP_get_c_struct_type_name(ui_obj),
		    abmfP_get_c_struct_global_name(ui_obj));
	    abio_printf(codeFile, "{\n    False\t /* initialized */\n};\n");
	}
	else if (obj_is_message(ui_obj))
	{
	    abio_printf(codeFile, "%s\t%s;\n",
		    "DtbMessageDataRec",
		    abmfP_get_c_struct_global_name(ui_obj));
	}
    }
    trav_close(&trav);

    return 0;
}


static int
write_call_create_proc(GenCodeInfo genCodeInfo, ABObj obj)
{
    File codeFile= genCodeInfo->code_file;
    char	createProc[1024];
    ABObj	structObj = NULL;
    ABObj	item = NULL;

    strcpy(createProc, abmfP_get_create_proc_name(obj));

    abio_printf(codeFile, "%s(", createProc);
    structObj = abmfP_obj_get_struct_obj(obj);
    if ((structObj != NULL) && (structObj == abmfP_ip_obj(genCodeInfo)))
    {
	abio_printf(codeFile, "instance");
    }
    else
    {
	abio_printf(codeFile, "&(%s)", abmfP_get_c_name(genCodeInfo, obj));
    }

    abio_indent(codeFile);

    if (abmfP_obj_create_proc_has_parent_param(obj))
    {
	STRING	parentName = NULL;

	/*
	if (   (obj == abmfP_ip_obj(genCodeInfo))
	    && (abmfP_parent_param(genCodeInfo) != NULL))
	{
	    parentName = istr_string(abmfP_parent_param(genCodeInfo));
	}
	else
	*/
	{
	    parentName = abmfP_get_widget_parent_name(genCodeInfo, obj);
	}
	abio_puts(codeFile, ",\n");
	abio_printf(codeFile, "%s", parentName);
    }

    /*
     * Write out submenu parameters
     */
    {
	int	num_submenus = abmfP_get_num_cp_submenu_params(obj);
	int	submenu_num = 0;
	ABObj	submenuObj = NULL;

	for (submenu_num = 0; submenu_num < num_submenus; ++submenu_num)
	{
	    submenuObj = abmfP_get_cp_submenu_param_obj(obj, submenu_num);
	    if (submenuObj == NULL)
	    {
		break;
	    }
	    abio_puts(codeFile, ",\n");
	    abio_printf(codeFile, "%s", 
		abmfP_get_c_name(genCodeInfo, submenuObj));
	}
    }

    abio_puts(codeFile, ");\n");
    abio_outdent(codeFile);

    obj_set_was_written(obj, TRUE);

    return 0;
}


static int
write_all_call_create_procs(GenCodeInfo genCodeInfo, ABObj root)
{
    AB_TRAVERSAL	trav;
    ABObj		obj;
    int		inst= 0;

    for (trav_open(&trav, root, AB_TRAV_UI | AB_TRAV_MOD_PARENTS_FIRST);
	(obj= trav_next(&trav)) != NULL; )
    {
	if (abmfP_obj_has_create_proc(obj))
	{
	    write_call_create_proc(genCodeInfo, obj);
	    ++inst;
	}
	else if (obj_is_menu_ref(obj))
	{
	    write_call_create_proc(genCodeInfo, obj);
	    ++inst;
	}
    }
    trav_close(&trav);
    return inst;
}


static int
write_add_widget_ref_resources(GenCodeInfo genCodeInfo, ABObj root)
{
    File		codeFile= genCodeInfo->code_file;
    AB_TRAVERSAL	trav;
    ABObj		obj;
    STRING		c_name= NULL;
    ABObj		menu = NULL;

    abio_printf(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE, 
	"Add widget-reference resources.");
    for (trav_open(&trav, root, AB_TRAV_UI | AB_TRAV_MOD_PARENTS_FIRST);
	(obj= trav_next(&trav)) != NULL; )
    {
	menu = obj_get_menu(obj);
	if (obj_is_item(obj) && (menu != NULL))
	{
	    c_name= abmfP_get_c_name(genCodeInfo, obj);
	    abio_printf(codeFile, "XtVaSetValues(%s,\n", c_name);
	    abio_indent(codeFile);
	    abio_printf(codeFile, "XmNsubMenuId, %s,\n",
		abmfP_get_c_name(genCodeInfo, menu));
	    abio_puts(codeFile, "NULL);\n");
	    abio_outdent(codeFile);
	}

	/*
	 * Register popup menus
	 */
	if ((menu != NULL) && (obj_is_root(obj)) && (obj_is_popup(menu)))
	{
	    ABObj menuParent = 
			objxm_comp_get_subobj(obj, AB_CFG_MENU_PARENT_OBJ);
	    abio_printf(codeFile, "%s(%s,\n",
		abmfP_lib_popup_menu_register->name,
		abmfP_get_c_name(genCodeInfo, menu));
		
	    abio_indent(codeFile);
	    abio_printf(codeFile, "%s);\n",
		abmfP_get_c_name(genCodeInfo, menuParent));
	    abio_outdent(codeFile);
	}

	if (abmfP_get_num_args_of_classes(obj, ABMF_ARGCLASS_WIDGET_REF) < 1)
	{
	    continue;
	}

	c_name= abmfP_get_c_name(genCodeInfo, obj);
	abio_printf(codeFile, "\n");
	abmfP_xt_va_list_open_setvalues(genCodeInfo, obj);
	abmfP_obj_spew_args(genCodeInfo, 
		obj, ABMF_ARGCLASS_WIDGET_REF, ABMF_ARGFMT_VA_LIST);
	abmfP_xt_va_list_close(genCodeInfo);
    }
    trav_close(&trav);

    return 0;
}


static int
write_manage_widgets(
			GenCodeInfo	genCodeInfo, 
			ABObj		tree
)
{
    AB_TRAVERSAL	trav;
    ABObj		obj = NULL;

    /*
     * Manage the windows
     */
    for (trav_open(&trav, tree, AB_TRAV_WINDOWS);
	(obj = trav_next(&trav)) != NULL; )
    {
	write_manage_one_tree(genCodeInfo, obj);
    }
    trav_close(&trav);

    return 0;
}


static int
write_manage_one_tree(
			GenCodeInfo	genCodeInfo, 
			ABObj		tree
)
{
    write_manage_descendants(genCodeInfo, tree);
    if (obj_should_be_managed_when_initialized(tree))
    {
        abio_printf(genCodeInfo->code_file, "XtManageChild(%s);\n",
            abmfP_get_c_name(genCodeInfo, tree));
    }

    return 0;
}


/*
 * 
 */
static int
write_manage_descendants(
			GenCodeInfo genCodeInfo, 
			ABObj obj
)
{
    AB_TRAVERSAL	childTrav;
    ABObj		child = NULL;

    for (trav_open(&childTrav, obj, AB_TRAV_CHILDREN);
	(child = trav_next(&childTrav)) != NULL; )
    {
	if (obj_is_ui(child))
	{
            write_manage_descendants(genCodeInfo, child);
	}
    }
    trav_close(&childTrav);

    /* Manage the children
     * Manage them, if it is a composite and has children.
     * Managing the child of a dialog shell will cause the shell to become
     * visible, so don't manage the children of shells that aren't visible
     */
    if (   (!(   ObjWClassIsDialogShell(obj) 
	      && (!obj_is_initially_visible(obj))))
        && (ObjWClassIsCompositeSubclass(obj))
	&& (abmfP_obj_has_field(obj))
	&& (obj_has_child(obj))
       )
    {
	if (   abmfP_obj_has_unmanaged_child(obj)
	    || ObjWClassIsCommand(obj)
	    || ObjWClassIsFileSelectionBox(obj)
	    || ObjWClassIsMainWindow(obj)
	    || ObjWClassIsPanedWindow(obj)
	    || ObjWClassIsScrolledWindow(obj)
	   )
	{
	    /* This object has one or more unmanaged children.
	     * Manage each child individually.
	     *
	     * Or, the widget class has a complex private structure (e.g.,
	     * ScrolledWindow), and we can't rely on XmNchildren.
	     */
	    AB_TRAVERSAL	unmanagedTrav;
	    ABObj		child = NULL;
	    for (trav_open(&unmanagedTrav, obj, AB_TRAV_CHILDREN);
		(child = trav_next(&unmanagedTrav)) != NULL; )
	    {
		if (obj_should_be_managed_when_initialized(child))
		{
		    abio_printf(genCodeInfo->code_file,
			"XtManageChild(%s);\n",
			    abmfP_get_c_name(genCodeInfo, child));
		}
	    }
	    trav_close(&unmanagedTrav);
	}
	else
	{
	    /* All children managed - just manage 'em all 
	     */
            abio_printf(genCodeInfo->code_file, "XtVaGetValues(%s,\n",
	        abmfP_get_c_name(genCodeInfo, obj));
	    abio_indent(genCodeInfo->code_file);
	    abio_puts(genCodeInfo->code_file, 
	      "XmNchildren, &children, XmNnumChildren, &numChildren, NULL);\n");
	    abio_outdent(genCodeInfo->code_file);
            abio_printf(genCodeInfo->code_file,
	        "XtManageChildren(children, numChildren);\n");
	}
    }
    return 0;
}

static int
write_out_mainwin_set_areas(GenCodeInfo genCodeInfo, ABObj obj)
{
    ABObj	mainwin, menubar, toolbar, workarea, footer;

    if (!obj_is_base_win(obj))
	return 0;

    mainwin = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_MW_OBJ);
    menubar = objxm_comp_mainwin_get_area(obj, AB_CONT_MENU_BAR);
    toolbar = objxm_comp_mainwin_get_area(obj, AB_CONT_TOOL_BAR);
    footer  = objxm_comp_mainwin_get_area(obj, AB_CONT_FOOTER);
    workarea = objxm_comp_get_subobj(obj, AB_CFG_PARENT_OBJ);

    if (menubar || toolbar)
    {
        abio_printf(genCodeInfo->code_file,
            "XmMainWindowSetAreas(%s,\n", abmfP_get_c_name(genCodeInfo,mainwin));

	abio_indent(genCodeInfo->code_file);

	abio_printf(genCodeInfo->code_file, "%s,\n",
	    menubar? abmfP_get_c_name(genCodeInfo, menubar) : "NULL");

	abio_printf(genCodeInfo->code_file, "%s,\n",
	    toolbar? abmfP_get_c_name(genCodeInfo, toolbar) : "NULL");

	abio_printf(genCodeInfo->code_file, "NULL,\n");
	abio_printf(genCodeInfo->code_file, "NULL,\n");

        abio_printf(genCodeInfo->code_file, "%s);\n",
            workarea? abmfP_get_c_name(genCodeInfo, workarea) : "NULL");

	abio_outdent(genCodeInfo->code_file);
    }
    if (footer)
    {
	abio_printf(genCodeInfo->code_file,
	    "XtVaSetValues(%s,\n", abmfP_get_c_name(genCodeInfo, mainwin));

	abio_indent(genCodeInfo->code_file);

 	abio_printf(genCodeInfo->code_file,
	    "XmNmessageWindow, %s,\n",
		abmfP_get_c_name(genCodeInfo, footer));
	abio_printf(genCodeInfo->code_file, "NULL);\n");

	abio_outdent(genCodeInfo->code_file);
    }
    return 0;
}

static int
write_set_fixed_pane(GenCodeInfo genCodeInfo, ABObj pane)
{

    abio_printf(genCodeInfo->code_file, 
        "XtVaGetValues(%s,\n",
        abmfP_get_c_name(genCodeInfo, pane));
    abio_indent(genCodeInfo->code_file);
    abio_printf(genCodeInfo->code_file, "XmNheight, &pane_height,\n");
    abio_printf(genCodeInfo->code_file, "NULL);\n");
    abio_outdent(genCodeInfo->code_file); 
 
    abio_printf(genCodeInfo->code_file,  
        "XtVaSetValues(%s,\n", 
        abmfP_get_c_name(genCodeInfo, pane)); 
 
    abio_indent(genCodeInfo->code_file);   
 
    abio_printf(genCodeInfo->code_file, 
        "XmNpaneMinimum,   pane_height,\n");
    abio_printf(genCodeInfo->code_file,
        "XmNpaneMaximum,   pane_height,\n");
    abio_printf(genCodeInfo->code_file, "NULL);\n\n"); 
 
    abio_outdent(genCodeInfo->code_file); 

    return 0;
}

static int
write_out_dialog_set_pane_height(GenCodeInfo genCodeInfo, ABObj obj)
{
    ABObj       dialog, panedwin, button_panel, footer;
 
    if (!obj_is_popup_win(obj))
        return 0;

    panedwin = objxm_comp_get_subobj(obj, AB_CFG_WINDOW_PW_OBJ);
    button_panel = objxm_comp_custdlg_get_area(obj, AB_CONT_BUTTON_PANEL);
    footer  = objxm_comp_custdlg_get_area(obj, AB_CONT_FOOTER);

    if (button_panel || footer)
    {
        abmfP_write_c_comment(genCodeInfo, FALSE,
        	"Make Dialog Button-Panel & Footer a fixed height");

	abio_printf(genCodeInfo->code_file, "{\n");
        abio_indent(genCodeInfo->code_file); 

	abio_printf(genCodeInfo->code_file,"Dimension	pane_height;\n\n");

	if (button_panel)
	    write_set_fixed_pane(genCodeInfo, button_panel);

	if (footer)
	    write_set_fixed_pane(genCodeInfo, footer);

        abio_outdent(genCodeInfo->code_file);
	abio_printf(genCodeInfo->code_file, "}\n");
    }
    return 0;
}


/*
 * Don't manage popup windows or menus
 *
 * Where all the children of a widget are to be managed, use 
 * XtManageChildren(). That's supposed to be more efficient than
 * managing each child, individually.
 */
static BOOL
obj_should_be_managed_when_initialized(ABObj obj)
{
    ABObj	wholeObj = obj_get_root(obj);
    ABObj	itemParentObj = NULL;

    if (!abmfP_obj_has_field(obj))
    {
	return FALSE;
    }

    /*
     * Menus are not managed until needed
     */
    if (obj_is_menu(obj))
    {
	return FALSE;
    }

    /*
     * Shells are not managed (Xt error!)
     * The top composite in a dialog is not managed, so that the dialog
     *     will not become visible until needed.
     */
    if (    ObjWClassIsShellSubclass(obj)
	|| (   (abmfP_parent(obj) != NULL) 
	    && (((abmfP_parent(obj))->type == AB_TYPE_DIALOG)
		|| ((abmfP_parent(obj))->type == AB_TYPE_FILE_CHOOSER))
	    && (ObjWClassIsDialogShell(abmfP_parent(obj)))) )
    {
	return FALSE;
    }

    /*
     * The item parent for an option menu is really a menu pane
     */
    if (wholeObj != NULL)
    {
	itemParentObj = objxm_comp_get_subobj(wholeObj, AB_CFG_PARENT_OBJ);
    }
    if (   obj_is_menu(obj)
	|| (obj_is_option_menu(wholeObj) && (obj == itemParentObj)) )
    {
	return FALSE;
    }

    if (!obj_is_initially_visible(obj))
	return FALSE;

    return TRUE;
}


static BOOL
abmfP_obj_has_unmanaged_child(ABObj obj)
{
    AB_TRAVERSAL	trav;
    ABObj		child = NULL;
    BOOL		doesIt = FALSE;

    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	(child = trav_next(&trav)) != NULL; )
    {
	/* menus don't show up in XmNchildren list, so we need to ignore them
	 */
	if (   abmfP_obj_has_field(child)
	    && (!obj_is_menu(child))
	    && (!ObjWClassIsMenuShell(child))
	    && (!ObjWClassIsPulldownMenu(child))
	    && (!ObjWClassIsPopupMenu(child))
	    && (!obj_should_be_managed_when_initialized(child)) )
	{
	    /* this is an unmanaged child */
	    doesIt = TRUE;
	    break;
	}
    }
    trav_close(&trav);

    return doesIt;
}


/* 
 * Write out one object clear proc
 */
static int
write_obj_clear_proc(GenCodeInfo genCodeInfo, ABObj obj)
{
    File	file= genCodeInfo->code_file;
    abmfP_write_clear_proc_begin(genCodeInfo, obj);
    abio_printf(file,
	"memset((void *)(%s), 0, sizeof(*%s));\n",
	abmfP_instance_ptr_var_name,
	abmfP_instance_ptr_var_name
	);
    abmfP_write_c_func_end(genCodeInfo, "0");
    return 0;
}


/*
 * Write out one object initialization proc
 */
static int
write_obj_init_proc(GenCodeInfo genCodeInfo, ABObj parent)
{
    int			return_value= 0;
    File                codeFile = genCodeInfo->code_file;
    ABObj               obj;
    ABObj               whole_obj = NULL;
    int                 inst = FALSE;
    AB_TRAVERSAL        trav;

    abmfP_gencode_enter_func(genCodeInfo);
    genCodeInfo->cur_func.ip_obj= abmfP_obj_get_struct_obj(parent);
    genCodeInfo->cur_func.create_obj= abmfP_obj_get_struct_obj(parent);
    abmfP_parent_param(genCodeInfo) = istr_const(abmfP_parent_param_name);
    abmfP_parent_param_has_value(genCodeInfo) = TRUE;

    abmfP_write_init_proc_begin(genCodeInfo, parent);

    abio_printf(codeFile, "WidgetList\tchildren = NULL;\n");
    abio_printf(codeFile, "int\t\tnumChildren = 0;\n");
    abio_printf(codeFile, "if (%s->initialized)\n", 
	abmfP_instance_ptr_var_name);
    abmfP_write_c_block_begin(genCodeInfo);
    abio_puts(codeFile, "return 0;\n");
    abmfP_write_c_block_end(genCodeInfo);
    abio_printf(codeFile, "%s->initialized = True;\n\n",
	abmfP_instance_ptr_var_name);

    inst= write_all_call_create_procs(genCodeInfo, parent);
    write_add_widget_ref_resources(genCodeInfo, parent);

    /*
     * Write out code to layout groups
     */
    abio_printf(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE, 
	"Call utility functions to do group layout");
    for (trav_open(&trav, parent, AB_TRAV_GROUPS);
	 (obj = trav_next(&trav)) != NULL;)
    {
        if (abmfP_obj_needs_align_handler(obj))
        {
	    ABObj	oobj = objxm_comp_get_subobj(obj, AB_CFG_OBJECT_OBJ);

            abio_printf(codeFile, "\n");

            abio_printf(genCodeInfo->code_file, 
		"%s(%s,\n\t\t%s,\n\t\t%s,\n\t\t%s,\n\t\t%d, %d, %d, %d, %d);\n",
		abmfP_lib_children_align->name,
		abmfP_get_c_name(genCodeInfo, oobj),
		abmfP_obj_get_group_type(obj),
		abmfP_obj_get_row_align_type(obj),
		abmfP_obj_get_col_align_type(obj),
		0,
		obj_get_num_rows(obj),
		obj_get_num_columns(obj),
		obj_get_hoffset(obj),
		obj_get_voffset(obj));
        }
    }
    trav_close(&trav);

    abio_printf(codeFile, "\n");
    if (obj_is_base_win(parent))
        write_out_mainwin_set_areas(genCodeInfo, parent);
    else if (obj_is_popup_win(parent))
        write_out_dialog_set_pane_height(genCodeInfo, parent);

    abio_puts(codeFile, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE, 
	"Manage the tree, from the bottom up.");
    write_manage_widgets(genCodeInfo, parent);
    if (obj_is_popup_win(parent))
    {
	ABObj	pwObj = objxm_comp_get_subobj(parent, AB_CFG_WINDOW_PW_OBJ);
	
	if (pwObj)
	{
	    abmfP_write_c_comment(genCodeInfo, FALSE,
		"Turn off traversal for invisible sash in dialog's PanedWindow");
	    abio_printf(codeFile, "%s(%s);\n",
		abmfP_lib_remove_sash_focus->name,	
		abmfP_get_c_name(genCodeInfo, pwObj));
	}
    }

    /*
     * Add Callbacks
     */
    abio_printf(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE, 
	"Add User and Connection callbacks");
    for (trav_open(&trav, parent, AB_TRAV_UI);
	 (obj = trav_next(&trav)) != NULL;)
    {
	if ( ((abmfP_get_window_parent(obj) == parent) || (obj == parent))
	    && (abmfP_write_add_callbacks_and_actions(genCodeInfo, obj) < 0)
	   )
	{
	    return_code(-1);
	}
    }
    trav_close(&trav);

    abmfP_write_c_func_end(genCodeInfo, "0");

epilogue:
    abmfP_gencode_exit_func(genCodeInfo);
    return return_value;
}


static int
write_all_obj_init_procs(GenCodeInfo genCodeInfo, ABObj module)
{
    ABObj               obj;
    AB_TRAVERSAL        trav;
    BOOL		MsgClearWritten = FALSE;

    abmfP_tree_set_written(module, FALSE);
    for (trav_open(&trav, module, AB_TRAV_SALIENT_UI);
	 (obj = trav_next(&trav)) != NULL;)
    {
/*
	if (obj_is_message(obj) && !MsgClearWritten)
	{
	    write_msg_clear_proc(genCodeInfo, module); 
	    MsgClearWritten = TRUE;
	}
	else if (obj_is_window(obj))
	{
	    write_obj_clear_proc(genCodeInfo, obj);
	}
*/

	if (obj_is_window(obj))
	{
	    write_obj_clear_proc(genCodeInfo, obj);
	    write_obj_init_proc(genCodeInfo, obj);
	}
	else if (obj_is_message(obj))
	{
	    write_msg_init_proc(genCodeInfo, obj);
	}
    }
    trav_close(&trav);
    abmfP_tree_set_written(module, FALSE);
    return 0;
}

static int
write_create_proc_decls(GenCodeInfo genCodeInfo, ABObj module)
{
    File                codeFile = genCodeInfo->code_file;
    AB_TRAVERSAL        trav;
    ABObj               obj;
    char               *parent = NULL;

    abmfP_tree_set_written(module, FALSE);
    abio_puts(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE,
	"Widget create procedure decls");

    for (trav_open(&trav, module, AB_TRAV_UI);
	 (obj = trav_next(&trav)) != NULL;)
    {
	if (!abmfP_obj_has_create_proc(obj))
	{
	    continue;
	}
	abmfP_write_create_proc_decl(genCodeInfo, obj);
    }
    trav_close(&trav);
    abio_puts(codeFile, "\n");
    return 0;
}


static void
set_parent_for_submenu(ABObj obj)
{
    ABObj               child = NULL;

    if (obj_has_child(obj))
    {
	child = obj_get_child(obj, 0);
	if (!obj_has_parent(child))
	{
	    obj_append_child(obj, child);
	}
    }
}


static int
item_has_children(ABObj obj)
{
    return (obj_is_menu_item(obj) && obj_has_child(obj));
}


static void
check_for_submenus(ABObj obj)
{
    AB_TRAVERSAL        trav;
    ABObj               child;

    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	 (child = trav_next(&trav)) != NULL;)
    {
	if (item_has_children(child))
	{
	    set_parent_for_submenu(child);
	    check_for_submenus(child);
	}
    }
    trav_close(&trav);
}


static void
assign_parent_to_submenus(ABObj project)
{
    AB_TRAVERSAL        trav;
    ABObj               obj;

    for (trav_open(&trav, project, AB_TRAV_UI);
	 (obj = trav_next(&trav)) != NULL;)
    {
	if (obj_is_menu(obj))
	    check_for_submenus(obj);
    }
}


int
write_obj_create_proc(GenCodeInfo genCodeInfo, ABObj obj)
{
    File                codeFile = genCodeInfo->code_file;
    ABObj               top_obj = NULL;
    ABObj		ip_obj = abmfP_obj_get_struct_obj(obj);
    ABObj               help_obj = NULL;
    int			num_post_create_procs = 0;
    STRING 		help_volume, help_location, help_text;

    abmfP_gencode_enter_func(genCodeInfo);
    abmfP_create_obj(genCodeInfo) = obj;
    abmfP_ip_obj(genCodeInfo) = ip_obj;

    if (abmfP_obj_create_proc_has_parent_param(obj))
    {
	abmfP_parent_param(genCodeInfo) = istr_const(abmfP_parent_param_name);
	abmfP_parent_param_has_value(genCodeInfo) = TRUE;
    }

    abmfP_write_create_proc_begin(genCodeInfo, obj);
    abmfP_write_create_proc_decls(genCodeInfo);
    abmfP_write_create_widgets_for_comp_obj(genCodeInfo, obj);
    write_call_all_user_post_create_procs(genCodeInfo, obj);

    if (abmfP_obj_needs_centering_handler(obj))
    {
        abio_printf(genCodeInfo->code_file, "\t%s(%s, %s);\n",
		abmfP_lib_center->name,
		abmfP_get_c_name(genCodeInfo, obj),
		abmfP_obj_get_centering_type(obj));
    }

    abmfP_write_c_func_end(genCodeInfo, "0");

    obj_set_was_written(obj, TRUE);
    abmfP_obj_set_subobjs_written(obj, TRUE);
    abmfP_obj_set_items_written(obj, TRUE);

epilogue:
    abmfP_gencode_exit_func(genCodeInfo);
    return (OK);
}


static int
write_call_all_user_post_create_procs(
			GenCodeInfo genCodeInfo, 
			ABObj		obj
)
{
    int			return_value = 0;
    int 		total_post_create_procs = 0;
    ABObj		item = NULL;
    AB_TRAVERSAL	itemTrav;

    total_post_create_procs += 
	    abmfP_obj_get_num_conns_by_when(obj, AB_WHEN_AFTER_CREATED);
    for (trav_open(&itemTrav, obj, AB_TRAV_ITEMS_FOR_OBJ);
	(item = trav_next(&itemTrav)) != NULL; )
    {
        total_post_create_procs += 
	    abmfP_obj_get_num_conns_by_when(item, AB_WHEN_AFTER_CREATED);
    }
    /* don't close trav, yet */

    if (total_post_create_procs > 0)
    {
	abio_puts(genCodeInfo->code_file, "\n");
        abmfP_write_c_comment(genCodeInfo, FALSE,
		"Call user (Post-)Create procs");
        for (trav_reset(&itemTrav);
	    (item = trav_next(&itemTrav)) != NULL; )
        {
            write_call_user_post_create_procs(genCodeInfo, item);
	}

	/*
	 * Call object's last, after all the subobjecs completely inited
	 */
        write_call_user_post_create_procs(genCodeInfo, obj);
    }
    trav_close(&itemTrav);	/* close it, now */

    return return_value;
}


static int
write_call_user_post_create_procs(GenCodeInfo genCodeInfo, ABObj subObj)
{
    int 	return_value = 0;
    int		num_post_create_procs = 0;
    ABObj	action = NULL;
    ABObj	compObj = obj_get_root(subObj);
    ABObj	sourceObj = objxm_comp_get_source_for_when(
				compObj, AB_WHEN_AFTER_CREATED);
    ABObj	targetObj = NULL;
    char	targetStructPtrName[MAX_NAME_SIZE];
    char	sourceName[MAX_NAME_SIZE];
    *targetStructPtrName = 0;
    *sourceName = 0;

        num_post_create_procs = 
	    abmfP_obj_get_num_conns_by_when(compObj, AB_WHEN_AFTER_CREATED);
        if (num_post_create_procs > 0)
        {
	    int 	i = 0;
    
	    for (i = 0; i < num_post_create_procs; ++i)
	    {
	        action = abmfP_obj_get_conn_by_when(
				compObj, AB_WHEN_AFTER_CREATED, i);
		targetObj = obj_get_to(action);

		/*
		 * source widget
		 */
		util_strncpy(sourceName, abmfP_get_c_name(
				genCodeInfo, sourceObj), MAX_NAME_SIZE);

		if (targetObj == NULL)
		{
		    snprintf(targetStructPtrName, sizeof(targetStructPtrName), "%s", abmfP_str_null);
		}
		else
		{
		    util_strncpy(targetStructPtrName,
			abmfP_get_c_struct_ptr_name(genCodeInfo, targetObj),
			MAX_NAME_SIZE);
		}

                abio_printf(genCodeInfo->code_file,
                  "%s(%s, (XtPointer)%s, (XtPointer)%s);\n",
                    abmfP_get_action_name(action),
                    sourceName,
		    targetStructPtrName,
                    abmfP_get_c_struct_ptr_name(genCodeInfo, sourceObj));
	    }
        }
    return return_value;
}


/*
 * Writes out parents before children
 */
static int
write_all_obj_create_procs(GenCodeInfo genCodeInfo, ABObj rootObj)
{
    int			return_value = 0;
    int			rc = 0;		/* return code */
    AB_TRAVERSAL        trav;
    ABObj               obj = NULL;

    for (trav_open(&trav, rootObj, AB_TRAV_UI | AB_TRAV_MOD_PARENTS_FIRST);
	 (obj = trav_next(&trav)) != NULL;)
    {
	if (!abmfP_obj_has_create_proc(obj))
	{
	    continue;
	}
	abio_printf(genCodeInfo->code_file, "\n\n");
	if ((rc = write_obj_create_proc(genCodeInfo, obj)) < 0)
	{
	    return_value = rc;
	    break;
	}
    }
    trav_close(&trav);

    return return_value;
}


/*
 * 
 */
static int
write_includes(GenCodeInfo genCodeInfo, ABObj module)
{
    File	codeFile = genCodeInfo->code_file;
    int		num_written = 0;
    AB_TRAVERSAL	trav;
    ABObj		obj = NULL;
    BOOL		includeGlyphFiles = FALSE;
    STRING		glyphFile = NULL;
    StringListRec	glyphIncludesRec;
    StringList		glyphIncludes= &(glyphIncludesRec);
    strlist_construct(glyphIncludes);

    strlist_set_is_unique(glyphIncludes, TRUE);
    ++num_written;
    abmfP_tree_write_widget_specific_includes(codeFile, module);
    abmfP_write_c_local_include(genCodeInfo, 
	abmfP_get_utils_header_file_name(module));
    abmfP_write_c_local_include(genCodeInfo, 
	abmfP_get_project_header_file_name(module));

    write_action_target_includes(genCodeInfo, module);

    abmfP_write_c_local_include(genCodeInfo,
	abmfP_get_ui_header_file_name(module));

    if (includeGlyphFiles)
    {
        for (trav_open(&trav, module, AB_TRAV_SALIENT_UI); 
		(obj = trav_next(&trav)) != NULL; )
        {
            ++num_written;
	    if (abmfP_obj_has_glyph_label(obj))
	    {
	        glyphFile = obj_get_label(obj);
	        if (!strlist_str_exists(glyphIncludes, glyphFile))
	        {
	            strlist_add_str(glyphIncludes, glyphFile, NULL);
	            abmfP_write_c_local_include(genCodeInfo, glyphFile);
	        }
	    }
        }
        trav_close(&trav);	/* finally close it */
    }

    strlist_destruct(glyphIncludes);
    return num_written;
}


static int
write_action_target_includes(GenCodeInfo genCodeInfo, ABObj module)
{
    int		numIncludes = 0;
    ABObj	project = obj_get_project(module);
    ABObj	action = NULL;
    AB_TRAVERSAL	trav;
    ABObj	fromObj = NULL;
    ABObj	fromModule = NULL;
    ABObj	toObj = NULL;
    ABObj	toModule = NULL;
    STRING	headerFile = NULL;
    StringListRec	targetIncludes;
    strlist_construct(&targetIncludes);

    /*
     * REMIND: this should be AB_TRAV_ACTIONS_FOR_OBJ.
     *		Currently, there's a bug that puts cross-module
     *		connection under the module, rather than the project
     */
    for (trav_open(&trav, project, AB_TRAV_ACTIONS);
	(action = trav_next(&trav)) != NULL; )
    {
	fromObj = obj_get_from(action);
	fromModule = NULL;
	if (fromObj != NULL)
	{
	    fromModule = obj_get_module(fromObj);
	}
	toObj = obj_get_to(action);
	toModule = NULL;
	if (toObj != NULL)
	{
	    toModule= obj_get_module(toObj);
	}

	if ((fromModule != NULL) && (fromModule == module)
	    && (toModule != NULL) && (toModule != module))
	{
	    /* cross-module connection from this module */
	    headerFile = abmfP_get_ui_header_file_name(toModule);
	    if (!strlist_str_exists(&targetIncludes, headerFile))
	    {    
		strlist_add_str(&targetIncludes, headerFile, NULL);
		abmfP_write_c_local_include(genCodeInfo, headerFile);
	    }

	}
    }
    trav_close(&trav);

    strlist_destruct(&targetIncludes);
    return numIncludes;
}



/*
 * Write the object initialization function file.
 */
int
abmfP_write_ui_c_file(
		      GenCodeInfo genCodeInfo,
		      STRING codeFileName,
		      ABObj module
)
{
    File                codeFile = genCodeInfo->code_file;
    char		moduleName[1024];
    ABObj               obj = NULL;
    char               *errmsg = NULL;
    *moduleName = 0;

    /*
     * Write file header.
     */
    sprintf(moduleName, "module %s", util_strsafe(obj_get_name(module)));
    abmfP_write_file_header(
		genCodeInfo, 
		codeFileName,
		FALSE,
		moduleName,
		util_get_program_name(), 
		ABMF_MODIFY_NOT,
	" * Contains: user module object initialize and create functions"
		);


    /*
     * Write includes.
     */
    write_includes(genCodeInfo, module);

    /*
     * REMIND: include header files from other modules
     */
    abio_puts(codeFile, "\n");

    assign_parent_to_submenus(module);

    /*
     * Write the file!
     */
    write_all_shared_var_decls(genCodeInfo, module);
    write_create_proc_decls(genCodeInfo, module);
    write_all_obj_init_procs(genCodeInfo, module);
    write_all_obj_create_procs(genCodeInfo, module);

    return OK;
}


static ABObj	connFromObj = NULL;
static AB_WHEN	connWhen = AB_WHEN_UNDEF;

static BOOL
action_from_pred(ABObj obj)
{
   return (   (obj_get_from(obj) == connFromObj) 
	   && (obj_get_when(obj) == connWhen) );
}


static int 
abmfP_obj_get_num_conns_by_when(
		ABObj	obj,
		AB_WHEN	when
)
{
    ABObj	project = obj_get_project(obj);
    int		numConns = 0;
    if (project == NULL)
    {
	return -1;
    }

    connFromObj = obj_get_actual_obj(obj); connWhen = when;
    numConns = trav_count_cond(project, AB_TRAV_ACTIONS, action_from_pred);
    connFromObj = NULL; connWhen = AB_WHEN_UNDEF;
    return numConns;
}


static ABObj	
abmfP_obj_get_conn_by_when(
		    ABObj	obj,
		    AB_WHEN	when,
		    int		which_conn
)
{
    ABObj		project = obj_get_project(obj);
    AB_TRAVERSAL	trav;
    ABObj		action = NULL;
    if (project == NULL)
    {
	return NULL;
    }

    connFromObj = obj_get_actual_obj(obj); connWhen = when;
    trav_open_cond(&trav, project, AB_TRAV_ACTIONS, action_from_pred);
    trav_goto(&trav, which_conn);
    action = trav_obj(&trav);
    trav_close(&trav);
    connFromObj = NULL; connWhen = AB_WHEN_UNDEF;

    return action;
}

static StringList
abmfP_get_msg_action_list(
    ABObj   msg_obj
)
{
    ABObj	module = NULL;
    ABObj       action = NULL;
    ABObj       fromObj = NULL; 
    AB_TRAVERSAL        trav;
    StringList	callback_funcs = NULL;

    if (!obj_is_message(msg_obj))
	return NULL;

    callback_funcs = strlist_create();

    module = obj_get_module(msg_obj);
    for (trav_open(&trav, module, AB_TRAV_ACTIONS);
	(action = trav_next(&trav)) != NULL; )
    { 
	fromObj = obj_get_from(action);
	if (fromObj == msg_obj)
	{
	    strlist_add_str(callback_funcs,
		abmfP_get_action_name(action),
		(void *) obj_get_when(action));
	}
    }

    if (strlist_get_num_strs(callback_funcs) == 0)
    {
	strlist_destroy(callback_funcs);
	callback_funcs = NULL;
    }

    return (callback_funcs);
}

/*
 * Write out one message object initialization proc
 */
static int
write_msg_init_proc(GenCodeInfo genCodeInfo, ABObj parent)
{
    int			return_value = 0;
    File                codeFile = genCodeInfo->code_file;
    STRING              i18n_str = (STRING) NULL;
    STRING              dialogType = (STRING) NULL;
    STRING              default_btn = (STRING) NULL;
    STRING              str = (STRING) NULL;
    StringList          callback_funcs = (StringList) NULL;
    STRING              callback_func = (STRING) NULL;
    STRING              okCB = (STRING) NULL, 
			action2CB = (STRING) NULL, 
			action3CB = (STRING) NULL, 
			cancelCB = (STRING) NULL;
    AB_WHEN             when = AB_WHEN_UNDEF;
    int			i = 0;
    STRING		help_text = (STRING) NULL,
			help_volume = (STRING) NULL,
			help_location = (STRING) NULL;

    abmfP_gencode_enter_func(genCodeInfo);
    genCodeInfo->cur_func.ip_obj = abmfP_obj_get_struct_obj(parent);
    genCodeInfo->cur_func.create_obj = abmfP_obj_get_struct_obj(parent);
    abmfP_parent_param(genCodeInfo) = (ISTRING) NULL;
    abmfP_parent_param_has_value(genCodeInfo) = FALSE;

    abmfP_write_init_proc_begin(genCodeInfo, parent);

    /* Write out one local variable for help I18N */
    if ( obj_has_help_data(parent) && 
	(genCodeInfo->i18n_method == ABMF_I18N_XPG4_API))
    {
	abio_puts(codeFile, "char	*tmp_txt;\n");
    }

    abio_printf(codeFile, "if (%s->initialized)\n", 
	abmfP_instance_ptr_var_name);
    abmfP_write_c_block_begin(genCodeInfo);
    abio_puts(codeFile, "return 0;\n");
    abmfP_write_c_block_end(genCodeInfo);
    abio_printf(codeFile, "%s->initialized = True;\n\n",
	abmfP_instance_ptr_var_name);

    /* Set the Message Dialog Type */
    switch (obj_get_msg_type(parent))
    {
        case AB_MSG_ERROR:
            dialogType = "XmDIALOG_ERROR";
            break;
        case AB_MSG_INFORMATION:
            dialogType = "XmDIALOG_INFORMATION";
            break;
        case AB_MSG_QUESTION:
            dialogType = "XmDIALOG_QUESTION";
            break;
        case AB_MSG_WARNING:
            dialogType = "XmDIALOG_WARNING";
            break;
        case AB_MSG_WORKING:
            dialogType = "XmDIALOG_WORKING";
            break;
        default:
            break;
    }
    abio_printf(codeFile, "%s->type = ", abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n", dialogType);

    /* Set Message Dialog Title */
    abio_printf(codeFile, "%s->title = ", abmfP_instance_ptr_var_name);
    str = obj_get_label(parent);
    if (!util_strempty(str))
    {
        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) 
	{
	    i18n_str = abmfP_catgets_prefix_str(genCodeInfo, parent, str);
            abio_printf(codeFile, "XmStringCreateLocalized(%s", i18n_str);
	    abio_put_string(codeFile, str);
	    abio_puts(codeFile, "));\n");
	}
	else
	{
	    abio_puts(codeFile, "XmStringCreateLocalized(");
	    abio_put_string(codeFile, str);
	    abio_puts(codeFile, ");\n");
	}
    }
    else
    {
        abio_puts(codeFile, "(XmString) NULL;\n");
    }

    /* Set Message Dialog message string */
    abio_printf(codeFile, "%s->message = ", abmfP_instance_ptr_var_name);
    str = obj_get_msg_string(parent);
    if (!util_strempty(str))
    {
        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) 
        {
            i18n_str = abmfP_catgets_prefix_str(genCodeInfo, parent, str);
            abio_printf(codeFile, "XmStringCreateLocalized(%s", i18n_str);
            abio_put_string(codeFile, str);
            abio_puts(codeFile, "));\n");  
	}
	else
	{
            abio_puts(codeFile, "XmStringCreateLocalized("); 
            abio_put_string(codeFile, str); 
            abio_puts(codeFile, ");\n"); 
	}
    }
    else
    {
        abio_puts(codeFile, "(XmString) NULL;\n");
    }

    /* Get callback function names */
    callback_funcs = abmfP_get_msg_action_list(parent);
    if (callback_funcs != NULL)
    {
        for (i = 0; i < strlist_get_num_strs(callback_funcs); i++)
        {
            callback_func = strlist_get_str(callback_funcs, i, (void**)&when);
            switch (when)
            {
                case AB_WHEN_ACTION1:
                    okCB = callback_func;
                    break;
                case AB_WHEN_ACTION2:
                    action2CB = callback_func;
                    break;
                case AB_WHEN_ACTION3:
                    action3CB = callback_func;
                    break;
                case AB_WHEN_CANCEL:
                    cancelCB = callback_func;
                    break;
            }
        }
    }


    /* Set the Action1 button (Ok button) label */
    abio_printf(codeFile, "%s->action1_label = ", abmfP_instance_ptr_var_name);
    str = obj_get_action1_label(parent);
    if (!util_strempty(str))
    {
        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) 
        {
            i18n_str = abmfP_catgets_prefix_str(genCodeInfo, parent, str);
            abio_printf(codeFile, "XmStringCreateLocalized(%s", i18n_str);
            abio_put_string(codeFile, str);
            abio_puts(codeFile, "));\n");   
	}
	else
	{
            abio_puts(codeFile, "XmStringCreateLocalized(");
            abio_put_string(codeFile, str);
            abio_puts(codeFile, ");\n");
	}
    }
    else
    {
        abio_puts(codeFile, "(XmString) NULL;\n");
    }
    
    /* Set the Action1 button callback (Ok callback) */
    abio_printf(codeFile, "%s->action1_callback = ", abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n", okCB? okCB : "(XtCallbackProc) NULL");

    /* Set the Action2 button label (Extra button) */
    abio_printf(codeFile, "%s->action2_label = ", abmfP_instance_ptr_var_name);
    str = obj_get_action2_label(parent);
    if (!util_strempty(str))
    {
        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
        {
            i18n_str = abmfP_catgets_prefix_str(genCodeInfo, parent, str);
            abio_printf(codeFile, "XmStringCreateLocalized(%s", i18n_str);
            abio_put_string(codeFile, str);
            abio_puts(codeFile, "));\n");
	}
	else
	{
            abio_puts(codeFile, "XmStringCreateLocalized("); 
            abio_put_string(codeFile, str); 
            abio_puts(codeFile, ");\n"); 
	}
    }
    else
    {
        abio_puts(codeFile, "(XmString) NULL;\n");
    }

    /* Set the Action2 button callback (Extra button) */
    abio_printf(codeFile,"%s->action2_callback = ",abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n", 
		action2CB? action2CB : "(XtCallbackProc) NULL");

    /* Set the Action3 button label (Extra button) */
    abio_printf(codeFile, "%s->action3_label = ", abmfP_instance_ptr_var_name);
    str = obj_get_action3_label(parent);
    if (!util_strempty(str))
    {
        if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API)
        {
            i18n_str = abmfP_catgets_prefix_str(genCodeInfo, parent, str);
            abio_printf(codeFile, "XmStringCreateLocalized(%s", i18n_str);
            abio_put_string(codeFile, str);
            abio_puts(codeFile, "));\n");
        }
        else
        {
            abio_puts(codeFile, "XmStringCreateLocalized(");
            abio_put_string(codeFile, str);
            abio_puts(codeFile, ");\n");
        }
    }   
    else
    {  
        abio_puts(codeFile, "(XmString) NULL;\n");
    }   
 
    /* Set the Action3 button callback (Extra button) */
    abio_printf(codeFile,"%s->action3_callback = ",abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n",
                action3CB? action3CB : "(XtCallbackProc) NULL");
 
    /* Set the Cancel button boolean */
    abio_printf(codeFile, "%s->cancel_button = ", abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n",
                obj_has_cancel_button(parent)? "True":"False");

    /* Set the Cancel button callback */
    abio_printf(codeFile, "%s->cancel_callback = ", abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n", cancelCB? cancelCB:"(XtCallbackProc) NULL");

    /* Set the Help button boolean */
    abio_printf(codeFile, "%s->help_button = ", abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n",
                obj_has_help_button(parent)? "True":"False");

    /* Set Object Help Data */
    if (obj_has_help_data(parent))
    {
	obj_get_help_data(parent, &help_volume, &help_location, &help_text);

	/* Set the help text field */
	if (genCodeInfo->i18n_method == ABMF_I18N_XPG4_API) 
	{
	    i18n_str = abmfP_catgets_prefix_str(genCodeInfo,parent,help_text);
            abio_printf(codeFile,"tmp_txt = %s", i18n_str);
            abio_put_string(codeFile, help_text);
            abio_puts(codeFile, ");\n");
            abio_printf(codeFile,
                "%s->help_data.help_text = XtMalloc(strlen(tmp_txt)+1);\n",
		abmfP_instance_ptr_var_name);
            abio_printf(codeFile,
                "strcpy(%s->help_data.help_text, tmp_txt);\n",
		abmfP_instance_ptr_var_name);
            abio_puts(codeFile,"\n");
        }
	else
	{
	    abio_printf(codeFile, "%s->help_data.help_text = ",
		abmfP_instance_ptr_var_name);
	    abio_put_string(codeFile, help_text);
	    abio_puts(codeFile, ";\n");
	}

	/* Set the help volume field */
	if (!util_strempty(obj_get_help_volume(parent)))
	{
	    abio_printf(codeFile, "%s->help_data.help_volume = ",
                abmfP_instance_ptr_var_name);
            abio_put_string(codeFile, help_volume);
            abio_puts(codeFile, ";\n");
	}
	else
	{
	    abio_printf(codeFile,
                "%s->help_data.help_volume = \"\";\n",
                abmfP_instance_ptr_var_name);
	}

        /* Set the help location field */
        if (!util_strempty(obj_get_help_location(parent))) 
        { 
            abio_printf(codeFile, "%s->help_data.help_locationID = ",
                abmfP_instance_ptr_var_name); 
            abio_put_string(codeFile, help_location); 
            abio_puts(codeFile, ";\n"); 
        }
	else
	{
            abio_printf(codeFile,
                "%s->help_data.help_locationID = \"\";\n",
                abmfP_instance_ptr_var_name);
	}
    }
    else
    {
	abio_printf(codeFile, 
		"%s->help_data.help_text = (char *) NULL;\n",
		abmfP_instance_ptr_var_name);
	abio_printf(codeFile, 
		"%s->help_data.help_volume = \"\";\n",
		abmfP_instance_ptr_var_name);
	abio_printf(codeFile, 
		"%s->help_data.help_locationID = \"\";\n",
		abmfP_instance_ptr_var_name);
    }

    /* Set the default button */
    switch(obj_get_default_btn(parent))
    {
        case AB_DEFAULT_BTN_ACTION1:
            default_btn = "DTB_ACTION1_BUTTON";
            break;
        case AB_DEFAULT_BTN_ACTION2:
            default_btn = "DTB_ACTION2_BUTTON";
            break;
        case AB_DEFAULT_BTN_ACTION3:
            default_btn = "DTB_ACTION3_BUTTON";
            break;
        case AB_DEFAULT_BTN_CANCEL:
            default_btn = "DTB_CANCEL_BUTTON";
            break;
        case AB_DEFAULT_BTN_NONE:
            default_btn = "DTB_NONE";
            break;
        default:
            break;
    }
    abio_printf(codeFile, "%s->default_button = ", abmfP_instance_ptr_var_name);
    abio_printf(codeFile, "%s;\n", default_btn);
    
    abmfP_write_c_func_end(genCodeInfo, "0");

epilogue:
    abmfP_gencode_exit_func(genCodeInfo);
    return return_value;
}

/* 
 * Write out the generic message clear proc 
 */
static int
write_msg_clear_proc(
    GenCodeInfo	genCodeInfo, 
    ABObj	module
)
{
    File        file = genCodeInfo->code_file;
    int		iRet = 0;

    iRet = abmfP_write_c_func_begin(
            genCodeInfo,
            FALSE,                                 /* Is static */
            abmfP_str_int,                         /* return type */
            abmfP_get_msg_clear_proc_name(module), /* function name */
            "DtbMessageData",                     /* Argument type */
            abmfP_instance_ptr_var_name,           /* Argument name */
            NULL);

    abio_printf(file,
        "memset((void *)(%s), 0, sizeof(*%s));\n",
        abmfP_instance_ptr_var_name,
        abmfP_instance_ptr_var_name);

    abmfP_write_c_func_end(genCodeInfo, "0");

    return iRet;
}
