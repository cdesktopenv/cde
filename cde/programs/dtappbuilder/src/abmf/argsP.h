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
 *	$XConsortium: argsP.h /main/3 1995/11/06 18:02:44 rswiston $
 *
 *	@(#)argsP.h	1.15 01 May 1995	cde_app_builder/src/abmf
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
 * argsP.h - deal with argument lists
 *
 * Nomenclature
 * ------------
 *
 * 	"written" args are written into the C code
 * 	"dumped" arg is dumped into a resource file
 *
 */
#ifndef _ABMF_ARGSP_H_
#define _ABMF_ARGSP_H_

#include "write_codeP.h"


typedef enum
{
    ABMF_ARGFMT_UNDEF = 0,
    ABMF_ARGFMT_ARRAY,
    ABMF_ARGFMT_VA_LIST,
    ABMF_ARG_LIST_FORMAT_NUM_VALUES		/* must be last */
} ABMF_ARG_LIST_FORMAT;


/*
 * Argument "classes"
 */
typedef unsigned char ABMF_ARG_CLASS;
#define	ABMF_ARGCLASS_UNDEF		ABMF_ARGCLASS_NONE
#define	ABMF_ARGCLASS_NONE		((ABMF_ARG_CLASS)0x00)
#define ABMF_ARGCLASS_SENSITIVITY	((ABMF_ARG_CLASS)0x01)
#define ABMF_ARGCLASS_TYPED		((ABMF_ARG_CLASS)0x02)
#define ABMF_ARGCLASS_WIDGET_REF	((ABMF_ARG_CLASS)0x04)
#define ABMF_ARGCLASS_OTHER		((ABMF_ARG_CLASS)0x08)
#define ABMF_ARGCLASS_ALL		((ABMF_ARG_CLASS)0xff)

#define ABMF_ARGCLASS_ALL_BUT_WIDGET_REF \
		(ABMF_ARGCLASS_ALL & ~ABMF_ARGCLASS_WIDGET_REF)


int	abmfP_args_init(void);		/* call first! */

int	abmfP_xt_va_list_open(GenCodeInfo);
int	abmfP_xt_va_list_open_setvalues(GenCodeInfo, ABObj obj);
int	abmfP_xt_va_list_close(GenCodeInfo);

BOOL	abmfP_arg_is_sensitivity(STRING argName);
BOOL	abmfP_arg_is_typed(STRING argName);
BOOL	abmfP_arg_is_widget_ref(STRING argName);
BOOL	abmfP_arg_is_callback(STRING argName);

int	abmfP_get_num_args(ABObj obj);
int	abmfP_get_num_code_file_args(GenCodeInfo genCodeInfo, ABObj obj);
int	abmfP_get_num_res_file_args(GenCodeInfo genCodeInfo, ABObj obj);

int	abmfP_write_arg_val(
		     	GenCodeInfo	genCodeInfo,
		     	BOOL	dumpResource,	/* put in res. file format */
		     	STRING	argName,
		     	void	*argValue,
		     	ABObj	obj		/* obj the arg belongs to */
			);


/*
 * Count args of specific classes (typed, widget-reference, ...)
 */
int	abmfP_get_num_args_of_classes( 
			ABObj		obj, 
			ABMF_ARG_CLASS	classes
			);

int	abmfP_comp_get_num_args_of_classes( 
			ABObj		obj, 
			ABMF_ARG_CLASS	classes
			);

int	abmfP_get_num_code_file_args_of_classes(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj,
			ABMF_ARG_CLASS	classes
			);

int	abmfP_comp_get_num_code_file_args_of_classes(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj,
			ABMF_ARG_CLASS	classes
			);

int	abmfP_get_num_res_file_args_of_classes(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj,
			ABMF_ARG_CLASS	classes
			);

int	abmfP_comp_get_num_res_file_args_of_classes(
			GenCodeInfo	genCodeInfo, 
			ABObj		obj,
			ABMF_ARG_CLASS	classes
			);

BOOL	abmfP_arg_is_spewed(	/* saved out, or ignored completely? */
		GenCodeInfo	genCodeInfo,
		STRING		argName,
		ABObj		obj
	);

BOOL	abmfP_arg_is_dumped(	/* should arg be dumped to resource file? */
		GenCodeInfo	genCodeInfo,
		STRING		argName,
		ABObj		obj
		);

BOOL	abmfP_arg_is_written(	/* should arg be written to the C file? */
		GenCodeInfo	genCodeInfo,
		STRING		argName,
		ABObj		obj
		);

int	abmfP_obj_spew_args(
		GenCodeInfo		genCodeInfo,
		ABObj			obj,
		ABMF_ARG_CLASS		argClassesToSpew,
		ABMF_ARG_LIST_FORMAT	argFormat
		);

AB_ARG_TYPE	abmfP_get_res_type(
			STRING	res_xmname,
			ABObj 	obj	/* may be NULL */
		);

/*****************************************************************
 **								**
 **		INLINE IMPLEMENTATION				**
 **								**
 *****************************************************************/

#define abmfP_get_num_args(obj) \
	(abmfP_get_num_args_of_classes(obj, ABMF_ARGCLASS_ALL))

#define abmfP_get_num_code_file_args(obj) \
	(abmfP_get_num_code_file_args_of_classes(obj, ABMF_ARGCLASS_ALL))

#define abmfP_get_num_res_file_args(obj) \
	(abmfP_get_num_res_file_args_of_classes(obj, ABMF_ARGCLASS_ALL))

#endif /* _ABMF_ARGSP_H_ */

