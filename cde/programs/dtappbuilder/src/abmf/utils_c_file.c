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
 *	$XConsortium: utils_c_file.c /main/3 1995/11/06 18:16:48 rswiston $
 *
 *	@(#)utils_c_file.c	1.25 08 May 1995	cde_app_builder/src/abmf
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
 *  utils_c_file.c - writes dtb_utils.c
 */

#include <ab_private/abio.h>
#include "lib_func_stringsP.h"
#include "write_cP.h"
#include "utils_c_fileP.h"

/*************************************************************************
**                                                                      **
**       Constants (#define and const)					**
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Private Functions (C declarations and macros)			**
**                                                                      **
**************************************************************************/
static int write_lib_func(GenCodeInfo genCodeInfo, LibFunc libFunc);
static int write_all_lib_funcs(GenCodeInfo genCodeInfo, ABObj project);

static int write_all_vars(GenCodeInfo genCodeInfo, ABObj project);
static int write_all_macros(GenCodeInfo genCodeInfo, ABObj project);
static int write_all_types(GenCodeInfo genCodeInfo, ABObj project);
static int write_all_static_func_decls(GenCodeInfo genCodeInfo, ABObj project);
static int write_save_toplevel_widget_var(GenCodeInfo genCodeInfo, ABObj project);
static int write_save_command_var(GenCodeInfo genCodeInfo, ABObj project);
static int write_session_CB_vars(GenCodeInfo genCodeInfo, ABObj project);
static int write_tooltalk_CB_vars(GenCodeInfo genCodeInfo, ABObj project);
static int write_dnd_vars(GenCodeInfo genCodeInfo, ABObj project);
static int write_popup_menu_vars(GenCodeInfo genCodeInfo, ABObj project);
static int write_centering_static_func_decls(GenCodeInfo genCodeInfo, ABObj project);
static int write_msg_static_func_decls( GenCodeInfo genCodeInfo, ABObj project);
static int write_align_static_func_decls(GenCodeInfo genCodeInfo, ABObj project);

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/
/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

int 
abmfP_write_utils_c_file(
			GenCodeInfo	genCodeInfo,
			STRING		fileName,
			ABObj		project
)
{
    char	projName[1024];
    sprintf(projName, "project %s", obj_get_name(project));
    abmfP_write_file_header(
		genCodeInfo, 
		fileName, 
		FALSE,
		projName,
		util_get_program_name(), 
		ABMF_MODIFY_NOT,
		" * CDE Application Builder General Utility Functions"
		);

    abmfP_write_c_system_include(genCodeInfo, "unistd.h");
    abmfP_write_c_system_include(genCodeInfo, "stdlib.h");
    abmfP_write_c_system_include(genCodeInfo, "stdio.h");
    abmfP_write_c_system_include(genCodeInfo, "sys/param.h");
    abmfP_write_c_system_include(genCodeInfo, "sys/stat.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/Xm.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/Form.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/Frame.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/Label.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/MessageB.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/PanedW.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/PushB.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/SashP.h");
    abmfP_write_c_system_include(genCodeInfo, "Xm/RowColumn.h");
    abmfP_write_c_system_include(genCodeInfo, "Dt/Help.h");
    abmfP_write_c_system_include(genCodeInfo, "Dt/HelpDialog.h");
    abmfP_write_c_system_include(genCodeInfo, "Dt/HelpQuickD.h");
    abmfP_write_c_system_include(genCodeInfo, "Dt/Session.h");
    abmfP_write_c_system_include(genCodeInfo, "Dt/Dnd.h");
    abmfP_write_c_local_include(genCodeInfo, "dtb_utils.h");

    write_all_macros(genCodeInfo, project);
    write_all_types(genCodeInfo, project);
    write_all_static_func_decls(genCodeInfo, project);
    write_all_vars(genCodeInfo, project);
    write_all_lib_funcs(genCodeInfo, project);
    return 0;
}


static int
write_all_macros(GenCodeInfo genCodeInfo, ABObj project)
{
    project = project;	/* avoid warning */

    abio_puts(genCodeInfo->code_file, nlstr);
    abio_puts(genCodeInfo->code_file,
	"#ifndef min\n"
	"#define min(a,b) ((a) < (b)? (a):(b))\n"
	"#endif\n\n");

    abio_puts(genCodeInfo->code_file,
	"#ifndef max\n"
	"#define max(a,b) ((a) > (b)? (a):(b))\n"
	"#endif\n\n");

    abio_puts(genCodeInfo->code_file,
        "#ifndef ABS\n"
        "#define ABS(x) ((x) >= 0? (x):(-(x)))\n"
        "#endif\n\n");

    return 0;
}


static int
write_all_types(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile = genCodeInfo->code_file;
    project = project;	/* avoid warning */
    abio_puts(codeFile,
        "typedef struct\n"
        "{\n"
        "    Widget              widget;\n"
        "    DtDndProtocol       protocol;\n"
        "    unsigned char       operations;\n"
        "    Boolean             bufferIsText;\n"
        "    Boolean             allowDropOnRootWindow;\n"
        "    Widget              sourceIcon;\n"
        "    DtbDndDragCallback  callback;\n"
        "    XtCallbackRec       convertCBRec[2];\n"
        "    XtCallbackRec       dragToRootCBRec[2];\n"
        "    XtCallbackRec       dragFinishCBRec[2];\n"
	"} DtbDragSiteRec, *DtbDragSite;\n"
	"\n");

    abio_puts(codeFile,
	"typedef struct\n"
	"{\n"
        "    Widget              widget;\n"
        "    DtDndProtocol       protocols;\n"
        "    unsigned char       operations;\n"
        "    Boolean             textIsBuffer;\n"
        "    Boolean             dropsOnChildren;\n"
        "    Boolean             preservePreviousRegistration;\n"
        "    DtbDndDropCallback  callback;\n"
        "    XtCallbackRec       animateCBRec[2];\n"
        "    XtCallbackRec       transferCBRec[2];\n"
        "} DtbDropSiteRec, *DtbDropSite;\n"
	"\n");

    abio_puts(codeFile,
	"/*\n"
	" * This structure keeps track of widget/menu pairs\n"
	" */\n"
	"typedef struct\n"
	"{\n"
        "    Widget              widget;\n"
        "    Widget              menu;\n"
        "} DtbMenuRefRec, *DtbMenuRef;\n"
	"\n");

    return 0;
}


static int
write_all_static_func_decls(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile= genCodeInfo->code_file;

    write_centering_static_func_decls(genCodeInfo, project);

    if (abmfP_proj_has_message(project))
	write_msg_static_func_decls(genCodeInfo, project);

    /*
     * Write out static functions for aligning objects in groups
     */
    write_align_static_func_decls(genCodeInfo, project);

    /*
     * private path-determing functions
     */
    abio_puts(genCodeInfo->code_file, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE,
	"Private functions used for finding paths");
    abio_printf(codeFile, "%s\n\n", abmfP_lib_determine_exe_dir->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_determine_exe_dir_from_argv->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_determine_exe_dir_from_path->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_path_is_executable->proto);

    /* private popup menu functions */
    abio_printf(codeFile, "%s\n\n", abmfP_lib_popup_menu->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_popup_menu_destroyCB->proto);

    /*
     * static drag functions
     */
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drag_terminate->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drag_button1_motion_handler->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drag_start->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drag_convertCB->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drag_to_rootCB->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drag_finishCB->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drop_animateCB->proto);
    abio_printf(codeFile, "%s\n\n", abmfP_lib_drop_transferCB->proto);

    return 0;
}


static int
write_all_vars(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile = genCodeInfo->code_file;

    write_session_CB_vars(genCodeInfo, project);
    write_tooltalk_CB_vars(genCodeInfo, project);
    write_save_toplevel_widget_var(genCodeInfo, project);
    write_save_command_var(genCodeInfo, project);
    write_popup_menu_vars(genCodeInfo, project);
    write_dnd_vars(genCodeInfo, project);

    /*
     * write path-to-executable var
     */
    abio_puts(codeFile, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE,
        "Directory where the binary for this process whate loaded from");
    abio_puts(codeFile, 
        "static char\t\t\t\t*dtb_exe_dir = (char *)NULL;\n");

    return 0;
}


static int
write_session_CB_vars(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);
    
    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, "\n");

    abio_puts(codeFile, "/*\n");
    abio_puts(codeFile, " * Variable for storing client session save callback\n");
    abio_puts(codeFile, " */\n");
    abio_puts(codeFile, 
        "static DtbClientSessionSaveCB\t\tdtb_client_session_saveCB = NULL;\n");

    if (abmfP_proj_needs_session_restore(project))
    {
        abio_puts(codeFile, "/*\n");
        abio_puts(codeFile, " * Variable for storing client session restore callback\n");
        abio_puts(codeFile, " */\n");
        abio_puts(codeFile, 
	    "static DtbClientSessionRestoreCB\tdtb_client_session_restoreCB = NULL;\n");
    }

    return (0);
}

static int
write_tooltalk_CB_vars(GenCodeInfo genCodeInfo, ABObj project)
{
    File        codeFile;
 
    if (!genCodeInfo || !project ||
	obj_get_tooltalk_level(project) != AB_TOOLTALK_DESKTOP_ADVANCED)
        return (0);

    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile, "\n");
 
    abio_puts(codeFile, "/*\n");
    abio_puts(codeFile, " * Variable for storing ToolTalk Message Quit callback\n");
    abio_puts(codeFile, " */\n");
    abio_puts(codeFile,
        "static DtbTTMsgHandlerCB\t\tdtb_tt_msg_quitCB = NULL;\n");

    abio_puts(codeFile, "/*\n"); 
    abio_puts(codeFile, " * Variable for storing ToolTalk Message Do Command callback\n"); 
    abio_puts(codeFile, " */\n"); 
    abio_puts(codeFile, 
        "static DtbTTMsgHandlerCB\t\tdtb_tt_msg_do_commandCB = NULL;\n");

    abio_puts(codeFile, "/*\n"); 
    abio_puts(codeFile, " * Variable for storing ToolTalk Message Get Status callback\n"); 
    abio_puts(codeFile, " */\n"); 
    abio_puts(codeFile, 
        "static DtbTTMsgHandlerCB\t\tdtb_tt_msg_get_statusCB = NULL;\n");

    abio_puts(codeFile, "/*\n"); 
    abio_puts(codeFile, " * Variable for storing ToolTalk Message Pause/Resume callback\n"); 
    abio_puts(codeFile, " */\n"); 
    abio_puts(codeFile, 
        "static DtbTTMsgHandlerCB\t\tdtb_tt_msg_pause_resumeCB = NULL;\n");
 
    return (0);
}

static int
write_save_toplevel_widget_var(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);
    
    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, "\n");

    abio_puts(codeFile, "/*\n");
    abio_puts(codeFile, " * Variable for storing top level widget\n");
    abio_puts(codeFile, " */\n");
    abio_puts(codeFile, 
        "static Widget\t\tdtb_project_toplevel_widget = (Widget)NULL;\n");

    return (0);
}

static int
write_save_command_var(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);
    
    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE,
        "Variable for storing command used to invoke application");
    abio_puts(codeFile, 
        "static char\t\t\t\t*dtb_save_command_str = (char *)NULL;\n");

    return (0);
}


static int
write_popup_menu_vars(GenCodeInfo genCodeInfo, ABObj project)
{
    abio_puts(genCodeInfo->code_file, nlstr);
    abmfP_write_c_comment(genCodeInfo, FALSE, 
	"Variables that keep track of which menus go with which widgets");
    abio_puts(genCodeInfo->code_file, 
	"static DtbMenuRef\tpopupMenus = NULL;\n"
	"static int\tnumPopupMenus = 0;\n");
    return 0;
}


static int
write_dnd_vars(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile = genCodeInfo->code_file;
    project = project;	/* avoid warning */
    abio_puts(codeFile,
        "#include <Dt/Dnd.h>"
        "\n"
        "\n"
        "#define DRAG_THRESHOLD  4\n"
        "static Boolean  dragInProgress = False;\n"
        "static int	dragInitialX = -1;\n"
        "static int	dragInitialY = -1;\n"
        "\n");

    return 0;
}


static int
write_centering_static_func_decls(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);
    
    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, "/*\n");
    abio_puts(codeFile, " * Private functions used for dynamic centering of objects\n");
    abio_puts(codeFile, " */\n");
    abio_printf(codeFile, "%s\n", abmfP_lib_center_widget->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_uncenter_widget->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_centering_handler->proto);

    return (0);
}

static int
write_align_static_func_decls(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile;

    if (!genCodeInfo || !project)
	return (0);
    
    codeFile = genCodeInfo->code_file;

    abio_puts(codeFile, "/*\n");
    abio_puts(codeFile, " * Static functions used for dynamic aligning of group objects\n");
    abio_puts(codeFile, " */\n");
    abio_printf(codeFile, "%s\n", abmfP_lib_get_label_widget->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_offset_from_ancestor->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_label_width->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_widest_label->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_widest_value->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_widget_rect->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_greatest_size->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_group_cell_size->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_group_row_col->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_get_group_child->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_children->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_handler->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_expose_handler->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_free_group_info->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_rows->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_cols->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_left->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_right->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_labels->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_vcenter->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_top->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_bottom->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_align_hcenter->proto);

    return (0);
}


static int
write_all_lib_funcs(GenCodeInfo genCodeInfo, ABObj project)
{
    File	codeFile= genCodeInfo->code_file;

    abio_puts(codeFile, "\n");
    abmfP_write_c_comment(genCodeInfo, FALSE,
		"Application Builder utility funcs");

    write_lib_func(genCodeInfo, abmfP_lib_cvt_file_to_pixmap);
    write_lib_func(genCodeInfo, abmfP_lib_set_label_from_bitmap_data);
    write_lib_func(genCodeInfo, abmfP_lib_set_label_pixmaps);
    write_lib_func(genCodeInfo, abmfP_lib_file_has_extension);
    write_lib_func(genCodeInfo, abmfP_lib_cvt_filebase_to_pixmap);
    write_lib_func(genCodeInfo, abmfP_lib_cvt_image_file_to_pixmap);
    write_lib_func(genCodeInfo, abmfP_lib_set_label_from_image_file);
    write_lib_func(genCodeInfo, abmfP_lib_cvt_resource_from_string);
    write_lib_func(genCodeInfo, abmfP_lib_create_greyed_pixmap);

    write_lib_func(genCodeInfo, abmfP_lib_save_toplevel_widget);
    write_lib_func(genCodeInfo, abmfP_lib_get_toplevel_widget);

    write_lib_func(genCodeInfo, abmfP_lib_remove_sash_focus);

    write_lib_func(genCodeInfo, abmfP_lib_save_command);
    write_lib_func(genCodeInfo, abmfP_lib_get_command);


    write_lib_func(genCodeInfo, abmfP_lib_help_dispatch);
    write_lib_func(genCodeInfo, abmfP_lib_more_help_dispatch);
    write_lib_func(genCodeInfo, abmfP_lib_help_back_hdlr);
    write_lib_func(genCodeInfo, abmfP_lib_do_onitem_help);
    write_lib_func(genCodeInfo, abmfP_lib_show_help_volume_info);
    write_lib_func(genCodeInfo, abmfP_lib_call_help_callback);

    /*
     * The session save callback is written out always
     */
    write_lib_func(genCodeInfo, abmfP_lib_session_save);
    write_lib_func(genCodeInfo, abmfP_lib_get_client_session_saveCB);

    if (abmfP_proj_needs_session_save(project))
    {
        write_lib_func(genCodeInfo, abmfP_lib_set_client_session_saveCB);
    }

    if (abmfP_proj_needs_session_restore(project))
    {
        write_lib_func(genCodeInfo, abmfP_lib_session_restore);
        write_lib_func(genCodeInfo, abmfP_lib_set_client_session_restoreCB);
        write_lib_func(genCodeInfo, abmfP_lib_get_client_session_restoreCB);
    }

    /*
     * Write out ToolTalk functions if needed
     */
    switch(obj_get_tooltalk_level(project))
    {
        case AB_TOOLTALK_DESKTOP_ADVANCED:
            write_lib_func(genCodeInfo, abmfP_lib_set_tt_msg_quitCB);
            write_lib_func(genCodeInfo, abmfP_lib_get_tt_msg_quitCB);
            write_lib_func(genCodeInfo, abmfP_lib_tt_msg_quit);
            write_lib_func(genCodeInfo, abmfP_lib_set_tt_msg_do_commandCB);
            write_lib_func(genCodeInfo, abmfP_lib_get_tt_msg_do_commandCB);
            write_lib_func(genCodeInfo, abmfP_lib_tt_msg_do_command);
            write_lib_func(genCodeInfo, abmfP_lib_set_tt_msg_get_statusCB);
            write_lib_func(genCodeInfo, abmfP_lib_get_tt_msg_get_statusCB);
            write_lib_func(genCodeInfo, abmfP_lib_tt_msg_get_status);
            write_lib_func(genCodeInfo, abmfP_lib_set_tt_msg_pause_resumeCB);
            write_lib_func(genCodeInfo, abmfP_lib_get_tt_msg_pause_resumeCB);
            write_lib_func(genCodeInfo, abmfP_lib_tt_msg_pause_resume);
            write_lib_func(genCodeInfo, abmfP_lib_tt_contractCB);
            /* fall through */
        case AB_TOOLTALK_DESKTOP_BASIC:
            write_lib_func(genCodeInfo, abmfP_lib_tt_close);
            break;
        default:
            break;
    }
	    

    /* Write out Message posting utilities */
    if (abmfP_proj_has_message(project))
    {
	write_lib_func(genCodeInfo, abmfP_lib_create_message_dlg);
	write_lib_func(genCodeInfo, abmfP_lib_destroyCB);
	write_lib_func(genCodeInfo, abmfP_lib_MessageBoxGetActionButton);
	write_lib_func(genCodeInfo, abmfP_lib_show_message);
	write_lib_func(genCodeInfo, abmfP_lib_show_modal_message);
	write_lib_func(genCodeInfo, abmfP_lib_modal_dlgCB);
    }

    /* Write out centering routines */
    write_lib_func(genCodeInfo, abmfP_lib_children_center);
    write_lib_func(genCodeInfo, abmfP_lib_children_uncenter);
    write_lib_func(genCodeInfo, abmfP_lib_center);
    write_lib_func(genCodeInfo, abmfP_lib_uncenter);
    write_lib_func(genCodeInfo, abmfP_lib_center_widget);
    write_lib_func(genCodeInfo, abmfP_lib_uncenter_widget);
    write_lib_func(genCodeInfo, abmfP_lib_centering_handler);

    /* Write out align routines */
    write_lib_func(genCodeInfo, abmfP_lib_get_label_widget);
    write_lib_func(genCodeInfo, abmfP_lib_get_offset_from_ancestor);
    write_lib_func(genCodeInfo, abmfP_lib_get_label_width);
    write_lib_func(genCodeInfo, abmfP_lib_get_widest_label);
    write_lib_func(genCodeInfo, abmfP_lib_get_widest_value);
    write_lib_func(genCodeInfo, abmfP_lib_get_widget_rect);
    write_lib_func(genCodeInfo, abmfP_lib_get_greatest_size);
    write_lib_func(genCodeInfo, abmfP_lib_get_group_cell_size);
    write_lib_func(genCodeInfo, abmfP_lib_get_group_row_col);
    write_lib_func(genCodeInfo, abmfP_lib_get_group_child);
    write_lib_func(genCodeInfo, abmfP_lib_children_align);
    write_lib_func(genCodeInfo, abmfP_lib_align_children);
    write_lib_func(genCodeInfo, abmfP_lib_align_handler);
    write_lib_func(genCodeInfo, abmfP_lib_expose_handler);
    write_lib_func(genCodeInfo, abmfP_lib_free_group_info);
    write_lib_func(genCodeInfo, abmfP_lib_align_rows);
    write_lib_func(genCodeInfo, abmfP_lib_align_cols);
    write_lib_func(genCodeInfo, abmfP_lib_align_left);
    write_lib_func(genCodeInfo, abmfP_lib_align_right);
    write_lib_func(genCodeInfo, abmfP_lib_align_labels);
    write_lib_func(genCodeInfo, abmfP_lib_align_vcenter);
    write_lib_func(genCodeInfo, abmfP_lib_align_top);
    write_lib_func(genCodeInfo, abmfP_lib_align_bottom);
    write_lib_func(genCodeInfo, abmfP_lib_align_hcenter);

    /* Write out path-finding routines */
    write_lib_func(genCodeInfo, abmfP_lib_get_exe_dir);
    write_lib_func(genCodeInfo, abmfP_lib_determine_exe_dir);
    write_lib_func(genCodeInfo, abmfP_lib_determine_exe_dir_from_argv);
    write_lib_func(genCodeInfo, abmfP_lib_determine_exe_dir_from_path);
    write_lib_func(genCodeInfo, abmfP_lib_path_is_executable);

    /* write out popup menu routines */
    write_lib_func(genCodeInfo, abmfP_lib_popup_menu_register);
    write_lib_func(genCodeInfo, abmfP_lib_popup_menu);
    write_lib_func(genCodeInfo, abmfP_lib_popup_menu_destroyCB);

    /* Write out drag and drop routines */
    write_lib_func(genCodeInfo, abmfP_lib_drag_site_register);
    write_lib_func(genCodeInfo, abmfP_lib_drop_site_register);
    write_lib_func(genCodeInfo, abmfP_lib_drag_terminate);
    write_lib_func(genCodeInfo, abmfP_lib_drag_button1_motion_handler);
    write_lib_func(genCodeInfo, abmfP_lib_drag_start);
    write_lib_func(genCodeInfo, abmfP_lib_drag_convertCB);
    write_lib_func(genCodeInfo, abmfP_lib_drag_to_rootCB);
    write_lib_func(genCodeInfo, abmfP_lib_drag_finishCB);
    write_lib_func(genCodeInfo, abmfP_lib_drop_animateCB);
    write_lib_func(genCodeInfo, abmfP_lib_drop_transferCB);

    return 0;
}


static int
write_lib_func(GenCodeInfo genCodeInfo, LibFunc libFunc)
{
    abio_puts(genCodeInfo->code_file, libFunc->def);
    abio_puts(genCodeInfo->code_file, "\n\n\n");
    return 0;
}


static int
write_msg_static_func_decls(
    GenCodeInfo genCodeInfo,
    ABObj project
)
{
    File        codeFile;

    if (!genCodeInfo || !project)
        return (0);

    codeFile = genCodeInfo->code_file;
 
    abio_puts(codeFile, "/*\n");
    abio_puts(codeFile, " * Static functions used for messages.\n");
    abio_puts(codeFile, " */\n");
    abio_printf(codeFile, "%s\n", abmfP_lib_destroyCB->proto);
    abio_printf(codeFile, "%s\n", abmfP_lib_modal_dlgCB->proto);
 
    return (0);
}
