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
 *	$XConsortium: bil_loadattP.h /main/3 1995/11/06 18:24:55 rswiston $
 *
 *	@(#)bil_loadattP.h	1.47 22 Nov 1994	
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
 * billdatt.h - load attribute (BIL) header file
 */
#ifndef _ABIL_BILLDATT_H_
#define _ABIL_BILLDATT_H_

#include "bilP.h"

extern ISTRING bilP_get_string();
extern STRING bilP_load_att_class(BIL_TOKEN);
extern STRING bilP_load_att_class_name(BIL_TOKEN);
extern STRING bilP_load_att_children_begin(void);
extern STRING bilP_load_att_children_end(void);
extern STRING bilP_load_att_container_type(BIL_TOKEN);
extern STRING bilP_load_att_decimal_points(BIL_TOKEN);
extern STRING bilP_load_att_default(BIL_TOKEN);
extern STRING bilP_load_att_drag_cursor(BIL_TOKEN);
extern STRING bilP_load_att_drag_cursor_mask(BIL_TOKEN);
extern STRING bilP_load_att_drag_enabled(BIL_TOKEN);
extern STRING bilP_load_att_drag_ops_begin(void);
extern STRING bilP_load_att_drag_ops_end(void);
extern STRING bilP_load_att_drag_to_root_allowed(BIL_TOKEN);
extern STRING bilP_load_att_drag_types_begin(void);
extern STRING bilP_load_att_drag_types_end(void);
extern STRING bilP_load_att_drawarea_width(BIL_TOKEN);
extern STRING bilP_load_att_drawarea_height(BIL_TOKEN);
extern STRING bilP_load_att_drop_enabled(BIL_TOKEN);
extern STRING bilP_load_att_drop_ops_begin(void);
extern STRING bilP_load_att_drop_ops_end(void);
extern STRING bilP_load_att_drop_site_child_allowed(BIL_TOKEN);
extern STRING bilP_load_att_drop_types_begin(void);
extern STRING bilP_load_att_drop_types_end(void);
extern STRING bilP_load_att_files_begin(void);
extern STRING bilP_load_att_files_end(void);
extern STRING bilP_load_att_icon_label(BIL_TOKEN);
extern STRING bilP_load_att_increment(BIL_TOKEN);
extern STRING bilP_load_att_initial_state(BIL_TOKEN valueToken);
extern STRING bilP_load_att_item_type(BIL_TOKEN);
extern STRING bilP_load_att_accelerator(BIL_TOKEN);
extern STRING bilP_load_att_alignment(BIL_TOKEN);
extern STRING bilP_load_att_col_alignment(BIL_TOKEN);
extern STRING bilP_load_att_res_file_types_begin(void);
extern STRING bilP_load_att_res_file_types_end(void);
extern STRING bilP_load_att_row_alignment(BIL_TOKEN);
extern STRING bilP_load_att_bg_color(BIL_TOKEN);
extern STRING bilP_load_att_button_type(BIL_TOKEN);
extern STRING bilP_load_att_fg_color(BIL_TOKEN);
extern STRING bilP_load_att_file_chooser_type(BIL_TOKEN);
extern STRING bilP_load_att_group_type(BIL_TOKEN);
extern STRING bilP_load_att_hattach_type(BIL_TOKEN);
extern STRING bilP_load_att_vattach_type(BIL_TOKEN);
extern STRING bilP_load_att_height(BIL_TOKEN);
extern STRING bilP_load_att_height_max(BIL_TOKEN);
extern STRING bilP_load_att_height_resizable(BIL_TOKEN);
extern STRING bilP_load_att_help_text(BIL_TOKEN);
extern STRING bilP_load_att_hscrollbar(BIL_TOKEN);
extern STRING bilP_load_att_hspacing(BIL_TOKEN);
extern STRING bilP_load_att_hoffset(BIL_TOKEN);
extern STRING bilP_load_att_icon(BIL_TOKEN);
extern STRING bilP_load_att_icon_mask(BIL_TOKEN);
extern STRING bilP_load_att_label(BIL_TOKEN);
extern STRING bilP_load_att_label_emphasis(BIL_TOKEN);
extern STRING bilP_load_att_label_type(BIL_TOKEN);
extern STRING bilP_load_att_max_label(BIL_TOKEN);
extern STRING bilP_load_att_max_value(BIL_TOKEN);
extern STRING bilP_load_att_menu(BIL_TOKEN);
extern STRING bilP_load_att_menu_title(BIL_TOKEN);
extern STRING bilP_load_att_menu_type(BIL_TOKEN);
extern STRING bilP_load_att_min_label(BIL_TOKEN);
extern STRING bilP_load_att_min_value(BIL_TOKEN);
extern STRING bilP_load_att_mnemonic(BIL_TOKEN);
extern STRING bilP_load_att_multiple_selections(BIL_TOKEN);
extern STRING bilP_load_att_name(BIL_TOKEN);
extern STRING bilP_load_att_num_columns(BIL_TOKEN);
extern STRING bilP_load_att_orientation(BIL_TOKEN);
extern STRING bilP_load_att_win_parent(BIL_TOKEN);
extern STRING bilP_load_att_reference_point(BIL_TOKEN);
extern STRING bilP_load_att_resource(BIL_TOKEN);
extern STRING bilP_load_att_read_only(BIL_TOKEN);
extern STRING bilP_load_att_root_window(BIL_TOKEN);
extern STRING bilP_load_att_selection_mode(BIL_TOKEN);
extern STRING bilP_load_att_selection_required(BIL_TOKEN);
extern STRING bilP_load_att_choice_type(BIL_TOKEN);
extern STRING bilP_load_att_has_footer(BIL_TOKEN);
extern STRING bilP_load_att_max_length(BIL_TOKEN);
extern STRING bilP_load_att_tear_off(BIL_TOKEN);
extern STRING bilP_load_att_text_type(BIL_TOKEN);
extern STRING bilP_load_att_initial_value(BIL_TOKEN);
extern STRING bilP_load_att_vendor(BIL_TOKEN);
extern STRING bilP_load_att_version(BIL_TOKEN);
extern STRING bilP_load_att_visible(BIL_TOKEN);
extern STRING bilP_load_att_voffset(BIL_TOKEN);
extern STRING bilP_load_att_vscrollbar(BIL_TOKEN);
extern STRING bilP_load_att_vspacing(BIL_TOKEN);
extern STRING bilP_load_att_num_rows(BIL_TOKEN);
extern STRING bilP_load_att_selected(BIL_TOKEN);
extern STRING bilP_load_att_type(BIL_TOKEN);
extern STRING bilP_load_att_user_data(BIL_TOKEN);
extern STRING bilP_load_att_width(BIL_TOKEN);
extern STRING bilP_load_att_width_max(BIL_TOKEN);
extern STRING bilP_load_att_width_resizable(BIL_TOKEN);
extern STRING bilP_load_att_x(BIL_TOKEN);
extern STRING bilP_load_att_y(BIL_TOKEN);
extern STRING bilP_load_list_value(BIL_TOKEN);
extern STRING bilP_load_att_is_help_item(BIL_TOKEN);
extern STRING bilP_load_att_border_frame(BIL_TOKEN);
extern STRING bilP_load_att_resizable(BIL_TOKEN);
extern STRING bilP_load_att_sb_always_visible(BIL_TOKEN);
extern STRING bilP_load_att_label_position(BIL_TOKEN);
extern STRING bilP_load_att_process_string(BIL_TOKEN valueToken);
extern STRING bilP_load_att_east_attachment_begin(void);
extern STRING bilP_load_att_east_attachment_end(void);
extern STRING bilP_load_att_west_attachment_begin(void);
extern STRING bilP_load_att_west_attachment_end(void);
extern STRING bilP_load_att_north_attachment_begin(void);
extern STRING bilP_load_att_north_attachment_end(void);
extern STRING bilP_load_att_south_attachment_begin(void);
extern STRING bilP_load_att_south_attachment_end(void);
extern STRING bilP_load_attachment_type(BIL_TOKEN);
extern STRING bilP_load_attachment_value(BIL_TOKEN);
extern STRING bilP_load_attachment_offset(BIL_TOKEN);
extern STRING bilP_load_att_label_position(BIL_TOKEN);
extern STRING bilP_load_att_help_volume( BIL_TOKEN);
extern STRING bilP_load_att_help_location( BIL_TOKEN);
extern STRING bilP_load_att_word_wrap(BIL_TOKEN);
extern STRING bilP_load_att_line_style(BIL_TOKEN);
extern STRING bilP_load_att_arrow_style(BIL_TOKEN);
extern STRING bilP_load_att_active(BIL_TOKEN );
extern STRING bilP_load_att_iconic(BIL_TOKEN );
extern STRING bilP_load_att_combo_box_style(BIL_TOKEN);
extern STRING bilP_load_att_show_value(BIL_TOKEN);
extern STRING bilP_load_att_slider_type( BIL_TOKEN);
extern STRING bilP_load_selection_mode(BIL_TOKEN);
extern STRING bilP_load_att_ok_label(BIL_TOKEN );
extern STRING bilP_load_att_auto_dismiss(BIL_TOKEN);
extern STRING bilP_load_att_directory(BIL_TOKEN);
extern STRING bilP_load_att_direction(BIL_TOKEN);
extern STRING bilP_load_att_filter_pattern(BIL_TOKEN);
extern STRING bilP_load_att_file_type_mask(BIL_TOKEN);
extern STRING bilP_load_att_message(BIL_TOKEN);
extern STRING bilP_load_att_msg_type(BIL_TOKEN);
extern STRING bilP_load_att_sessioning_method(BIL_TOKEN);
extern STRING bilP_load_att_tt_desktop_level(BIL_TOKEN);
extern STRING bilP_load_att_action1_button(BIL_TOKEN valueToken);
extern STRING bilP_load_att_action1_label(BIL_TOKEN valueToken);
extern STRING bilP_load_att_action2_button(BIL_TOKEN valueToken);
extern STRING bilP_load_att_action2_label(BIL_TOKEN valueToken);
extern STRING bilP_load_att_action3_label(BIL_TOKEN valueToken);
extern STRING bilP_load_att_cancel_button(BIL_TOKEN valueToken);
extern STRING bilP_load_att_help_button(BIL_TOKEN valueToken);
extern STRING bilP_load_att_default_button(BIL_TOKEN valueToken);
extern STRING bilP_load_att_decimal_points(BIL_TOKEN valueToken);
extern STRING bilP_load_att_increment(BIL_TOKEN valueToken);
extern STRING bilP_load_att_pane_max_height(BIL_TOKEN valueToken);
extern STRING bilP_load_att_pane_min_height(BIL_TOKEN valueToken);
extern STRING bilP_load_att_i18n_enabled(BIL_TOKEN valueToken);
extern STRING bilP_load_att_win_children_begin(void);
extern STRING bilP_load_att_win_children_end(void);

#endif /* _ABIL_BILLDATT_H_ */
