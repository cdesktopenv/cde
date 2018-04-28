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
 *      $XConsortium: abobj_set.h /main/3 1995/11/06 17:17:42 rswiston $
 *
 *	@(#)abobj_set.h	1.53 02 Feb 1995	
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * abobj_set.h 
 */
#ifndef _ABOBJ_SET_H_
#define _ABOBJ_SET_H_

#include <ab_private/obj.h>


/*
 * Routines to instantiate attribute changes in the Motif
 * instance of the objects
 */
extern void	abobj_instantiate_changes(
		    ABObj	obj
		);

extern void	abobj_tree_instantiate_changes(
		    ABObj	root
		);

/*
 * Routines to set attribute changes in objects
 */
extern void	abobj_set_accelerator(
		    ABObj	obj,
		    STRING	accel
		);
extern void     abobj_set_background_color(
                    ABObj       obj,
                    STRING      colorname
                );

extern void     abobj_set_choice_type(
                    ABObj       obj,
                    AB_CHOICE_TYPE type
                );

extern void	abobj_set_direction(
		    ABObj	obj,
		    AB_DIRECTION	dir
		);

extern void     abobj_set_drawarea_size(
                    ABObj       obj,
                    int		width,
                    int		height
                );

extern void     abobj_set_foreground_color(
                    ABObj       obj,
                    STRING      colorname
                );

extern void     abobj_set_icon(
                    ABObj       obj,
		    STRING	icon,
		    STRING	icon_mask,
                    STRING      icon_label
                );
 
extern void     abobj_set_initial_state(
                    ABObj       obj,
                    AB_OBJECT_STATE istate
                );
 
extern void     abobj_set_initial_value(
                    ABObj       obj,
                    STRING      strval,
		    int		intval
                );

extern void     abobj_set_label(
                    ABObj       	obj,
		    AB_LABEL_TYPE 	label_type,
                    STRING      	label
                );

extern void	abobj_set_label_alignment(
		    ABObj		obj,
		    AB_ALIGNMENT	align
		);

extern void	abobj_set_label_position(
		    ABObj		obj,
		    AB_COMPASS_POINT	pos
		);

extern void     abobj_set_group_type(
                    ABObj       	obj,
		    AB_GROUP_TYPE 	group_type
                );

extern void     abobj_set_row_alignment(
                    ABObj       	obj,
		    AB_ALIGNMENT 	align
                );

extern void     abobj_set_col_alignment(
                    ABObj       	obj,
		    AB_ALIGNMENT 	align
                );

extern void     abobj_set_row_attach_type(
                    ABObj       	obj,
		    AB_ATTACH_TYPE 	type
                );

extern void     abobj_set_col_attach_type(
                    ABObj       	obj,
		    AB_ATTACH_TYPE 	type
                );

extern void     abobj_set_row_offset(
                    ABObj       	obj,
		    int 		offset
                );

extern void     abobj_set_col_offset(
                    ABObj       	obj,
		    int 		offset
                );

extern void	abobj_set_border_frame(
		    ABObj	obj,
		    AB_LINE_TYPE btype
		);

extern void	abobj_set_line_style(
		    ABObj	obj,
		    AB_LINE_TYPE type
		);

extern void	abobj_set_arrow_style(
		    ABObj	obj,
		    AB_ARROW_STYLE astyle
		);

extern void	abobj_set_button_type(
		    ABObj	obj,
		    AB_BUTTON_TYPE type
		);
 
extern void	abobj_set_name(
		    ABObj	obj,
		    STRING	name
		);

extern void	abobj_set_selection_mode(
		    ABObj	obj,
		    AB_SELECT_TYPE select
		);

extern void	abobj_set_show_value(
		    ABObj	obj,
		    BOOL	show_val
		);

extern void	abobj_set_size_policy(
		    ABObj	obj,
		    BOOL	resizable
		);

extern STRING	abobj_construct_item_name(
		    STRING	prefix,
		    STRING	namebase,
		    STRING	suffix
		);

extern void	abobj_set_item_name(
		    ABObj	iobj,
		    ABObj	module,
		    STRING	basename,
		    STRING	label
		);

extern void	abobj_set_menu_name(
		    ABObj	obj,
		    STRING	menuname
		);

extern void	abobj_set_menu_title(
		    ABObj	obj,
		    STRING	menu_title	
		);

extern void     abobj_set_mnemonic( 
                    ABObj       obj, 
                    STRING      mnemonic
		); 

extern void	abobj_set_orientation(
		    ABObj	obj,
		    AB_ORIENTATION orient
		);

extern void	abobj_set_num_columns(
		    ABObj	obj,
		    int		num_cols
		);

extern void	abobj_set_num_rows(
		    ABObj	obj,
		    int		num_rows
		);

extern void	abobj_set_read_only(
		    ABObj	obj,
		    BOOL	readonly
		);

extern void     abobj_set_resize_mode(
                    ABObj       obj,
                    BOOL        resizable
                );

extern void	abobj_set_scrollbar_state(
		    ABObj		obj,
		    AB_SCROLLBAR_POLICY	scrolling
		);

extern void     abobj_set_pixel_width(
                    ABObj       obj,
                    int		width,
		    int	        border_frame_w
                );
extern void     abobj_set_pixel_height(
                    ABObj       obj,
                    int		height,
		    int	        border_frame_w
                );
extern void	abobj_set_pixel_size(
		    ABObj       obj,
                    int         height,
		    int		width,
		    int		border_frame_w
		);
extern void     abobj_set_text_size(
                    ABObj       obj,
                    int         height,
                    int         width
		);
extern void	abobj_set_max_length(
		    ABObj	obj,
		    int		max_len
		);

extern void	abobj_set_win_parent(
		    ABObj	obj,
		    ABObj	win_parent
		);

extern void	abobj_set_word_wrap(
		    ABObj	obj,
		    BOOL	wrap
		);

extern void	abobj_set_xy(
		    ABObj	obj,
		    int		x,
		    int		y
		);

extern void 	abobj_set_active(
		    ABObj       obj,
		    BOOL        active
		);

extern void 	abobj_set_visible(
		    ABObj       obj,
		    BOOL        visible
		);

extern void	 abobj_set_iconic(
		    ABObj       obj,
		    BOOL        iconic
		);

extern void 	abobj_set_selected(
		    ABObj       obj,
		    BOOL        selected
		);

extern void 	abobj_set_tearoff(
		    ABObj       obj,
		    BOOL        tearoff   
		);

extern void	abobj_set_text_type(
		    ABObj	obj,
		    AB_TEXT_TYPE ttype
		);

extern void	abobj_set_decimal_points(
		    ABObj	obj,
		    int		dec_points
		);

extern void	abobj_set_default_act_button(
		    ABObj	obj,
		    ABObj	button
		);

extern void	abobj_set_help_act_button(
		    ABObj	obj,
		    ABObj	button
		);

extern void	abobj_set_increment(
		    ABObj	obj,
		    int		incr
		);

extern void 	abobj_set_min_max_values( 
		    ABObj       obj, 
		    int         min, 
		    int		max
		);

extern int 	abobj_set_save_needed(
		    ABObj       obj,
		    BOOL	set
		); 

extern void 	abobj_disable_save_needed(
		); 

extern void 	abobj_enable_save_needed(
		); 

extern BOOL 	abobj_save_needed_enabled(
		); 

extern void 	abobj_set_pattern_type(
		    ABObj               obj,
		    AB_FILE_TYPE_MASK   fmtype
		);

extern void 	abobj_set_directory(
		    ABObj       obj,
		    STRING      val
		);

extern void 	abobj_set_filter_pattern(
		    ABObj       obj,
		    STRING      val
		);

extern void 	abobj_set_ok_label(
		    ABObj       obj,
		    STRING      val
		);

extern void 	abobj_set_auto_dismiss(
		    ABObj       obj,
		    BOOL        val
		);

extern void	abobj_set_sessioning_method(
		    ABObj	proj,
		    AB_SESSIONING_METHOD ss_mthd
		);

extern void 	abobj_set_pane_min(
		    ABObj       obj,
		    int         value
		);

extern void 	abobj_set_pane_max(
		    ABObj       obj,
		    int         value
		);

extern void	abobj_set_attachment(
		    ABObj		obj,
		    AB_COMPASS_POINT	dir,
		    ABAttachment	*attach
		);

extern void	abobj_set_i18n_enabled(
		    ABObj	proj,
		    BOOL	i18n_enabled
		);

#endif /* _ABOBJ_SET_H_ */
