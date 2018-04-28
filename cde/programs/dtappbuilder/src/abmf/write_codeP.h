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
 *	$XConsortium: write_codeP.h /main/3 1995/11/06 18:18:40 rswiston $
 *
 *	@(#)write_codeP.h	1.18 16 Feb 1994	cde_app_builder/src/abmf
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
 * write_codeP.h - write code in entire files, projects, et cetera
 */
#ifndef _ABMF_WRITE_CODEP_H_
#define _ABMF_WRITE_CODEP_H_

#include <ab_private/strlist.h>
#include "utilsP.h"
#include "abmfP.h"
#include "msg_file.h"

/*
 * Info about a function being written
 *
 * If a local variable exists, the _var member will have a value.
 * (e.g. display_var = "display"). If, in the generated code, it
 * has been assigned a value, _has_value will be TRUE.
 */
typedef struct
{
    ABObj	create_obj;	/* func is to create this obj */
    ABObj	ip_obj;		/* object referenced by instance pointer */

    /* local vars - if ISTRING is not NULL, variable exists */
    ISTRING	acceltext_xmstr_var;
    BOOL	acceltext_xmstr_var_has_value;
    ISTRING	arg_counter_var;
    BOOL	arg_counter_var_has_value;
    ISTRING	args_var;
    BOOL	args_var_has_value;
    ISTRING	counter_var;
    BOOL	counter_var_has_value;
    ISTRING	display_var;
    BOOL	display_var_has_value;
    ISTRING	drawable_var;
    BOOL	drawable_var_has_value;
    ISTRING	icon_pixmap_var;
    BOOL	icon_pixmap_var_has_value;
    ISTRING	icon_mask_pixmap_var;
    BOOL	icon_mask_pixmap_var_has_value;
    ISTRING	label_array_var;
    BOOL	label_array_has_nulls;
    ISTRING	label_pixmap_var;
    BOOL	label_pixmap_var_has_value;
    ISTRING	label_str_var;
    BOOL	label_str_var_has_value;
    ISTRING	label_xmstr_var;
    BOOL	label_xmstr_var_has_value;
    ISTRING	mnemonic_array_var;
    BOOL	mnemonic_array_var_has_value;
    ISTRING	name_array_var;
    BOOL	name_array_var_has_value;
    ISTRING	name_param;
    BOOL	name_param_has_value;
    ISTRING	parent_param;	
    BOOL	parent_param_has_value;
    ISTRING	return_code_var;
    BOOL	return_code_var_has_value;
    StringListRec	submenu_params;
    StringListRec	submenu_param_types;
    BOOL	submenu_params_has_value;
    ISTRING	title_str_var;
    BOOL	title_str_var_has_value;
    ISTRING	value_str_var;
    BOOL	value_str_var_has_value;
    ISTRING	icon_name_str_var;
    BOOL	icon_name_str_var_has_value;
    ISTRING	ok_label_xmstr_var;
    BOOL	ok_label_xmstr_var_has_value;
    ISTRING	directory_xmstr_var;
    BOOL	directory_xmstr_var_has_value;
    ISTRING	pattern_xmstr_var;
    BOOL	pattern_xmstr_var_has_value;
} GenFuncInfoRec, *GenFuncInfo;


/* 
 * Parameters for code generation and status of generation in progress
 */
typedef struct
{
    /* ** NOTE: "write" arg = write to C file
     * **       "dump" arg = dump to resource file
     */
    File		code_file;		/* C code */
    File		resource_file;		/* dumped resources */
    BOOL		prototype_funcs;	/* write prototypes? */
    AB_ARG_CLASS_FLAGS	dumped_resources;	/* resources to "dump" */
    ABMF_I18N_METHOD	i18n_method;		/* intl'zation method */
    STRING		msg_src_file_name;	/* i18n msg src file name */
    MsgFile		msg_file_obj;		/* ADT for Msg Src File */
    BOOL		writing_func;	/* currently writing a func? */
    GenFuncInfoRec	cur_func;	/* func currently being generated */
} GenCodeInfoRec, *GenCodeInfo;


/*
 * Get/Set fields
 */
#define abmfP_cur_func(genCodeInfo) (&((genCodeInfo)->cur_func))

#define abmfP_create_obj(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->create_obj)

#define abmfP_ip_obj(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->ip_obj)

#define abmfP_args_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->args_var)

#define abmfP_args_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->args_var_has_value)

#define abmfP_counter_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->counter_var)

#define abmfP_counter_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->counter_var_has_value)

#define abmfP_arg_counter_var(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->arg_counter_var)

#define abmfP_arg_counter_var_has_value(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->arg_counter_var_has_value)

#define abmfP_display_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->display_var)

#define abmfP_display_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->display_var_has_value)

#define abmfP_drawable_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->drawable_var)

#define abmfP_drawable_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->drawable_var_has_value)

#define abmfP_icon_pixmap_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->icon_pixmap_var)

#define abmfP_icon_pixmap_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->icon_pixmap_var_has_value)

#define abmfP_icon_mask_pixmap_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->icon_mask_pixmap_var)

#define abmfP_icon_mask_pixmap_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->icon_mask_pixmap_var_has_value)

#define abmfP_label_pixmap_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->label_pixmap_var)

#define abmfP_label_pixmap_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->label_pixmap_var_has_value)

#define abmfP_label_str_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->label_str_var)

#define abmfP_label_str_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->label_str_var_has_value)

#define abmfP_label_array_var(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->label_array_var)

#define abmfP_label_array_has_nulls(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->label_array_has_nulls)

#define abmfP_mnemonic_array_var(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->mnemonic_array_var)

#define abmfP_mnemonic_array_var_has_value(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->mnemonic_array_var_has_value)

#define abmfP_name_array_var(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->name_array_var)

#define abmfP_name_array_var_has_value(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->name_array_var_has_value)

#define abmfP_label_xmstr_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->label_xmstr_var)

#define abmfP_label_xmstr_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->label_xmstr_var_has_value)

#define abmfP_acceltext_xmstr_var(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->acceltext_xmstr_var)

#define abmfP_acceltext_xmstr_var_has_value(genCodeInfo) \
                (abmfP_cur_func(genCodeInfo)->acceltext_xmstr_var_has_value)

#define abmfP_name_param(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->name_param)

#define abmfP_name_param_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->name_param_has_value)

#define abmfP_parent_param(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->parent_param)

#define abmfP_parent_param_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->parent_param_has_value)

#define abmfP_return_code_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->return_code_var)

#define abmfP_return_code_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->return_code_var_has_value)

#define abmfP_submenu_params(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->submenu_params)

#define abmfP_submenu_param_types(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->submenu_param_types)

#define abmfP_submenu_params_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->submenu_params_has_value)

#define abmfP_title_str_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->title_str_var)

#define abmfP_title_str_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->title_str_var_has_value)

#define abmfP_value_str_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->value_str_var)

#define abmfP_value_str_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->value_str_var_has_value)

#define abmfP_icon_name_str_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->icon_name_str_var)

#define abmfP_icon_name_str_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->icon_name_str_var_has_value)

#define abmfP_ok_label_xmstr_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->ok_label_xmstr_var)

#define abmfP_ok_label_xmstr_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->ok_label_xmstr_var_has_value)

#define abmfP_directory_xmstr_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->directory_xmstr_var)

#define abmfP_directory_xmstr_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->directory_xmstr_var_has_value)

#define abmfP_pattern_xmstr_var(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->pattern_xmstr_var)

#define abmfP_pattern_xmstr_var_has_value(genCodeInfo) \
		(abmfP_cur_func(genCodeInfo)->pattern_xmstr_var_has_value)

/*
 * Functions
 */
int	abmfP_gencode_construct(GenCodeInfo genCodeInfo);
int	abmfP_gencode_destruct(GenCodeInfo genCodeInfo);
int	abmfP_gencode_enter_func(GenCodeInfo genCodeInfo);
int	abmfP_gencode_exit_func(GenCodeInfo genCodeInfo);

/*
 * A struct obj has a C structure defined for it to contain all of its
 * children
 */
BOOL	abmfP_obj_is_struct_obj(ABObj obj);
BOOL	abmfP_obj_is_substruct_obj(ABObj obj);
BOOL	abmfP_obj_could_be_substruct_obj(ABObj obj);

/*
 * Does the object have a field of its own in a structure?
 */
BOOL	abmfP_obj_has_field(ABObj obj);
BOOL	abmfP_obj_has_struct_field(ABObj obj);
BOOL	abmfP_obj_has_substruct_field(ABObj obj);

/*
 * Routines to write out magic comments
 */
int	abmfP_write_user_start_comment(GenCodeInfo, STRING desc);
int	abmfP_write_user_long_start_comment(GenCodeInfo, STRING desc);
int	abmfP_write_user_end_comment(GenCodeInfo, STRING desc);
int	abmfP_write_user_seg(GenCodeInfo, STRING desc, STRING contents);
int	abmfP_write_user_long_seg(GenCodeInfo, STRING desc);
int	abmfP_write_user_code_seg(GenCodeInfo, STRING contents);
int	abmfP_write_user_funcs_seg(GenCodeInfo);
int	abmfP_write_user_struct_fields_seg(GenCodeInfo);
int	abmfP_write_user_type_and_var_seg(GenCodeInfo);
int	abmfP_write_user_var_seg(GenCodeInfo);
int	abmfP_write_user_var_and_code_seg(GenCodeInfo, STRING contents);
int	abmfP_write_user_long_var_and_code_seg(GenCodeInfo);
int	abmfP_write_user_header_seg(GenCodeInfo);
int	abmfP_write_user_file_top_seg(GenCodeInfo);
int	abmfP_write_user_file_bottom_seg(GenCodeInfo);

/*
 * Data
 */
extern char *const abmfP_cmt_user_code_start;
extern char *const abmfP_cmt_user_code_end;


/*************************************************************************
 **									**
 **		INLINE DEFINITION					**
 **									**
 *************************************************************************/

#endif /* _ABMF_WRITE_CODEP_H_ */

