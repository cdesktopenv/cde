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
 *	$XConsortium: lib_func_stringsP.h /main/3 1995/11/06 18:07:17 rswiston $
 *
 *	@(#)lib_func_stringsP.h	1.25 08 May 1995	cde_app_builder/src/abmf
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
 * lib_lib_stringsP.h - library functions in the form of character strings
 */
#ifndef _ABMF_LIB_FUNC_STRINGSP_H_
#define _ABMF_LIB_FUNC_STRINGSP_H_

#include <ab_private/AB.h>
#include <ab/util_types.h>

typedef struct
{
    STRING	name;		/* name */
    char	proto[512];	/* prototype */
    char	def[4096];	/* actual code */
} LibFuncRec, *LibFunc;


extern LibFunc abmfP_lib_cvt_file_to_pixmap;
extern LibFunc abmfP_lib_create_greyed_pixmap;
extern LibFunc abmfP_lib_set_label_pixmaps;
extern LibFunc abmfP_lib_file_has_extension;
extern LibFunc abmfP_lib_cvt_filebase_to_pixmap;
extern LibFunc abmfP_lib_cvt_image_file_to_pixmap;
extern LibFunc abmfP_lib_set_label_from_image_file;
extern LibFunc abmfP_lib_set_label_from_bitmap_data;
extern LibFunc abmfP_lib_set_label_from_xpm_data;
extern LibFunc abmfP_lib_cvt_resource_from_string;
extern LibFunc abmfP_lib_save_toplevel_widget;
extern LibFunc abmfP_lib_get_toplevel_widget;
extern LibFunc abmfP_lib_remove_sash_focus;
extern LibFunc abmfP_lib_save_command;
extern LibFunc abmfP_lib_get_command;
extern LibFunc abmfP_lib_help_dispatch;
extern LibFunc abmfP_lib_help_back_hdlr;
extern LibFunc abmfP_lib_more_help_dispatch;
extern LibFunc abmfP_lib_do_onitem_help;
extern LibFunc abmfP_lib_show_help_volume_info;
extern LibFunc abmfP_lib_session_save;
extern LibFunc abmfP_lib_get_client_session_saveCB;
extern LibFunc abmfP_lib_set_client_session_saveCB;
extern LibFunc abmfP_lib_session_restore;
extern LibFunc abmfP_lib_set_client_session_restoreCB;
extern LibFunc abmfP_lib_get_client_session_restoreCB;
extern LibFunc abmfP_lib_create_message_dlg;
extern LibFunc abmfP_lib_destroyCB;
extern LibFunc abmfP_lib_MessageBoxGetActionButton;
extern LibFunc abmfP_lib_show_message;
extern LibFunc abmfP_lib_show_modal_message;
extern LibFunc abmfP_lib_modal_dlgCB;
extern LibFunc abmfP_lib_children_center;
extern LibFunc abmfP_lib_children_uncenter;
extern LibFunc abmfP_lib_center;
extern LibFunc abmfP_lib_uncenter;
extern LibFunc abmfP_lib_center_widget;
extern LibFunc abmfP_lib_uncenter_widget;
extern LibFunc abmfP_lib_centering_handler;
extern LibFunc abmfP_lib_get_label_widget;
extern LibFunc abmfP_lib_get_offset_from_ancestor;
extern LibFunc abmfP_lib_get_label_width;
extern LibFunc abmfP_lib_get_widest_label;
extern LibFunc abmfP_lib_get_widest_value;
extern LibFunc abmfP_lib_get_widget_rect;
extern LibFunc abmfP_lib_get_greatest_size;
extern LibFunc abmfP_lib_get_group_cell_size;
extern LibFunc abmfP_lib_get_group_row_col;
extern LibFunc abmfP_lib_get_group_child;
extern LibFunc abmfP_lib_children_align;
extern LibFunc abmfP_lib_align_children;
extern LibFunc abmfP_lib_align_handler;
extern LibFunc abmfP_lib_expose_handler;
extern LibFunc abmfP_lib_free_group_info;
extern LibFunc abmfP_lib_align_rows;
extern LibFunc abmfP_lib_align_cols;
extern LibFunc abmfP_lib_align_left;
extern LibFunc abmfP_lib_align_right;
extern LibFunc abmfP_lib_align_labels;
extern LibFunc abmfP_lib_align_vcenter;
extern LibFunc abmfP_lib_align_top;
extern LibFunc abmfP_lib_align_bottom;
extern LibFunc abmfP_lib_align_hcenter;
extern LibFunc abmfP_lib_get_exe_dir;
extern LibFunc abmfP_lib_determine_exe_dir;
extern LibFunc abmfP_lib_determine_exe_dir_from_argv;
extern LibFunc abmfP_lib_determine_exe_dir_from_path;
extern LibFunc abmfP_lib_path_is_executable;
extern LibFunc abmfP_lib_set_tt_msg_quitCB;
extern LibFunc abmfP_lib_get_tt_msg_quitCB;
extern LibFunc abmfP_lib_tt_msg_quit;
extern LibFunc abmfP_lib_set_tt_msg_do_commandCB;
extern LibFunc abmfP_lib_get_tt_msg_do_commandCB;
extern LibFunc abmfP_lib_tt_msg_do_command;
extern LibFunc abmfP_lib_set_tt_msg_get_statusCB;
extern LibFunc abmfP_lib_get_tt_msg_get_statusCB;
extern LibFunc abmfP_lib_tt_msg_get_status;
extern LibFunc abmfP_lib_set_tt_msg_pause_resumeCB;
extern LibFunc abmfP_lib_get_tt_msg_pause_resumeCB;
extern LibFunc abmfP_lib_tt_msg_pause_resume;
extern LibFunc abmfP_lib_tt_contractCB;
extern LibFunc abmfP_lib_tt_close;
extern LibFunc abmfP_lib_call_help_callback;
extern LibFunc abmfP_lib_popup_menu_register;
extern LibFunc abmfP_lib_popup_menu;
extern LibFunc abmfP_lib_popup_menu_destroyCB;
extern LibFunc abmfP_lib_drag_site_register;
extern LibFunc abmfP_lib_drop_site_register;
extern LibFunc abmfP_lib_drag_terminate;
extern LibFunc abmfP_lib_drag_button1_motion_handler;
extern LibFunc abmfP_lib_drag_start;
extern LibFunc abmfP_lib_drag_convertCB;
extern LibFunc abmfP_lib_drag_to_rootCB;
extern LibFunc abmfP_lib_drag_finishCB;
extern LibFunc abmfP_lib_default_dragCB;
extern LibFunc abmfP_lib_default_dropCB;
extern LibFunc abmfP_lib_drop_animateCB;
extern LibFunc abmfP_lib_drop_transferCB;
#endif /* _ABMF_LIB_FUNC_STRINGSP_H_ */

