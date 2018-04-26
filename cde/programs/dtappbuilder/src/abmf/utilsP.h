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
 * $XConsortium: utilsP.h /main/3 1995/11/06 18:16:34 rswiston $
 * 
 * @(#)utilsP.h	3.43 16 Feb 1994	cde_app_builder/src/abmf
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

#ifndef _ABMF_UTILSP_H_
#define _ABMF_UTILSP_H_

#include <ab_private/obj.h>
#include "abmfP.h"

/*
 * Pointer to functions.
 */
typedef int         (*PFI) ();
typedef char       *(*PFS) ();
typedef void        (*PFV) ();

extern STRING nlstr;	/* "\n" - keep this identifier short! */
extern STRING nlstr2;	/* "\n\n" - keep this identifier short! */

/*
 * Miscellaneous
 */
ABObj		abmfP_get_root_window(ABObj project);
STRING          abmfP_backup_file(STRING fileName);
int             abmfP_filecopy(STRING file1, STRING file2);
STRING		abmfP_convert_wclass_ptr_to_name(WidgetClass wclass);


/*
 * strings
 */
STRING          abmfP_capitalize_first_char(char *str);
STRING          abmfP_uncapitalize_first_char(char *str);
STRING		abmfP_get_define_from_file_name(STRING fileName);
BOOL		abmfP_file_name_is_bitmap(STRING fileName);
BOOL		abmfP_file_name_is_xpm(STRING fileName);
STRING		abmfP_cvt_file_name_to_bitmap_data_vars(
			STRING	fileName,
			STRING	*widthVarPtr,
			STRING	*heightVarPtr,
			STRING	*bitsVarPtr);
STRING		abmfP_cvt_file_name_to_xpm_data_var(STRING fileName);

/* 
 * object functions
 */
ABObj	abmfP_parent(ABObj obj); /* same abmfP_obj_find_codegen_parent */
ABObj		abmfP_obj_find_codegen_parent(ABObj obj);
BOOL		abmfP_obj_create_proc_has_parent_param(ABObj obj);
BOOL		abmfP_obj_create_proc_has_name_param(ABObj obj);
BOOL		abmfP_obj_create_proc_has_instance_param(ABObj obj);
BOOL		abmfP_obj_create_proc_has_submenu_params(ABObj);
int		abmfP_get_num_cp_submenu_params(ABObj);
ABObj		abmfP_get_cp_submenu_param_obj(ABObj, int which);
STRING		abmfP_get_cp_submenu_param_name(ABObj, int which);
STRING		abmfP_get_cp_submenu_param_type_name(ABObj, int which);
STRING		abmfP_obj_get_create_proc_instance_param_type(ABObj);
int		abmfP_obj_get_num_create_proc_return_widgets(ABObj);
STRING		abmfP_obj_get_create_proc_return_widget_name(
				ABObj obj, int which_param);
BOOL		abmfP_obj_has_clear_proc(ABObj obj);
BOOL		abmfP_obj_has_create_proc(ABObj obj);
BOOL		abmfP_obj_has_init_proc(ABObj obj);
BOOL		abmfP_obj_has_menu(ABObj);
BOOL		abmfP_obj_has_item_with_menu(ABObj);
BOOL		abmfP_module_has_menu_struct(ABObj obj);
int		abmfP_get_num_substruct_fields(ABObj obj);
ABObj		abmfP_obj_get_struct_obj(ABObj obj);
ABObj		abmfP_obj_get_substruct_obj(ABObj obj);
BOOL		abmfP_objs_in_same_struct(ABObj obj1, ABObj obj2);
BOOL		abmfP_objs_in_same_substruct(ABObj obj1, ABObj obj2);
int             abmfP_assign_widget_names(ABObj tree);
ABObj           abmfP_get_parent_of_type(ABObj obj, AB_OBJECT_TYPE parent_type);
ABObj           abmfP_get_window_parent(ABObj obj);
int             abmfP_obj_is_duplicate(ABObj obj1, ABObj obj2);
ABObj           abmfP_get_actual_control(ABObj obj);
int             abmfP_tree_set_written(ABObj root, BOOL written);
int             abmfP_obj_set_subobjs_written(ABObj root, BOOL written);
int             abmfP_obj_set_items_written(ABObj root, BOOL written);
BOOL		abmfP_obj_has_glyph_label(ABObj obj);
BOOL		abmfP_obj_has_item_with_glyph_label(ABObj obj);
BOOL		abmfP_obj_has_string_label(ABObj obj);
BOOL		abmfP_obj_has_item_with_string_label(ABObj obj);
ABObj		abmfP_find_callback_scope(ABObj project, STRING func_name);
BOOL		abmfP_proj_needs_session_save(ABObj proj);
BOOL		abmfP_proj_needs_session_restore(ABObj proj);
BOOL		abmfP_obj_needs_centering_handler(ABObj obj);
STRING		abmfP_obj_get_centering_type(ABObj obj);
BOOL		abmfP_obj_needs_align_handler(ABObj obj);
STRING		abmfP_obj_get_group_type(ABObj obj);
STRING		abmfP_obj_get_row_align_type(ABObj obj);
STRING		abmfP_obj_get_col_align_type(ABObj obj);
BOOL		abmfP_items_are_homogeneous(ABObj obj);
BOOL		abmfP_items_have_mnemonics(ABObj obj);
BOOL		abmfP_items_have_accelerators(ABObj obj);
BOOL 		abmfP_proj_has_message(ABObj proj);
 

/*
 * Memory
 */
void*		abmfP_calloc(size_t n, size_t size);
void*		abmfP_malloc(size_t size);
void*		abmfP_realloc(void *buf, unsigned int size);
int		abmfP_free(void *buf);


/*
** refs
*/
ABObjList	abmfP_find_refs_to(ABObj obj);


/*************************************************************************
 **									**
 **			INLINE IMPLEMENTATION				**
 **									**
 *************************************************************************/

#define abmfP_free(ptr) util_free(ptr)

#define abmfP_obj_has_clear_proc(obj) \
	(abmfP_obj_is_struct_obj(obj))

#define abmfP_obj_has_init_proc(obj) \
	(abmfP_obj_is_struct_obj(obj))

/*
 * shortcut for abmfP_obj_find_codegen_parent. Checks to see if it
 * can avoid the function call, first (which is most of the time).
 */
#define abmfP_parent(obj) \
	((obj)->parent == NULL? \
	    NULL \
	: \
	    obj_has_flag((obj)->parent, NoCodeGenFlag)? \
		abmfP_obj_find_codegen_parent(obj) \
	    : \
		((obj)->parent) \
	)

#endif /* _ABMF_UTILSP_H_ */
