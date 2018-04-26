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
 *	$XConsortium: write_cP.h /main/3 1995/11/06 18:18:07 rswiston $
 *
 *	@(#)write_cP.h	1.6 16 Feb 1994	cde_app_builder/src/abmf
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
 * write_cP.h - generic utilities for writing C output
 */
#ifndef _ABMF_WRITE_CP_H_
#define _ABMF_WRITE_CP_H_

#include "abmfP.h"
#include "write_codeP.h"

typedef enum
{
    ABMF_MODIFY_UNDEF = 0,
    ABMF_MODIFY_NOT,
    ABMF_MODIFY_USER_SEGS,
    ABMF_MODIFY_UNMARKED,
    ABMF_MODIFY_ANY,
    ABMF_MODIFY_TYPE_NUM_VALUES
} ABMF_MODIFY_TYPE;

int abmfP_write_c_block_begin(GenCodeInfo genCodeInfo);

int abmfP_write_c_block_end(GenCodeInfo genCodeInfo);

int abmfP_write_c_comment(
		GenCodeInfo	genCodeInfo,
		BOOL		oneLiner,
		STRING		comment
);

int abmfP_write_c_func_decl(
		GenCodeInfo	genCodeInfo,
		BOOL	is_static,
		STRING	return_type,
		STRING	func_name,
		...			/* "type", "name", ..., NULL */
		);

int abmfP_write_c_func_begin(
		GenCodeInfo	genCodeInfo,
		BOOL	is_static,
		STRING	return_type,
		STRING	func_name,
		...			/* "type", "name", ..., NULL */
		);

int abmfP_write_c_func_end(
		GenCodeInfo, 
		STRING return_value
		);

int abmfP_write_c_include(GenCodeInfo, STRING file_name);

/* wriutes #include "blah" */
int abmfP_write_c_local_include(GenCodeInfo, STRING file_name);

/* writes #include <blah> */
int abmfP_write_c_system_include(GenCodeInfo, STRING file_name);

int abmfP_write_xm_callback_decl(
		GenCodeInfo, 
		BOOL	is_static,
		STRING	func_name
		);

int abmfP_write_xm_callback_begin(
		GenCodeInfo, 
		BOOL	is_static,
		STRING	func_name
		);

int abmfP_write_tooltalk_callback_decl(
                GenCodeInfo,
                BOOL    is_static,
                STRING  func_name
                );

int abmfP_write_session_save_callback_decl(
                GenCodeInfo,
                BOOL    is_static,
                STRING  func_name
                );

int abmfP_write_session_restore_callback_decl(
                GenCodeInfo,
                BOOL    is_static,
                STRING  func_name
                );

int abmfP_write_tooltalk_callback_begin(
                GenCodeInfo,
                BOOL    is_static,
                STRING  func_name
                );


int abmfP_write_clear_proc_decl(
		GenCodeInfo,
		ABObj	obj
		);

int abmfP_write_msg_clear_proc_decl(
		GenCodeInfo,
		ABObj	obj
		);

int abmfP_write_clear_proc_begin(
		GenCodeInfo,
		ABObj	obj
		);


int abmfP_write_create_proc_decl(
		GenCodeInfo,
		ABObj	obj
		);

int abmfP_write_create_proc_begin(
		GenCodeInfo,
		ABObj	obj
		);


int abmfP_write_init_proc_decl(
		GenCodeInfo,
		ABObj	obj
		);

int	abmfP_write_init_proc_begin(
		GenCodeInfo,
		ABObj	obj
		);

int	abmfP_write_file_header(
		GenCodeInfo		genCodeInfo,
		STRING			fileName,
		BOOL			openIfdef,
		STRING			gennedFrom,
		STRING			gennedBy,
		ABMF_MODIFY_TYPE	modifyType,
		STRING			description
	);

int	abmfP_write_file_footer(
			GenCodeInfo	genCodeInfo,
			STRING		fileName,
			BOOL		closeIfdef
	);

/* types */
extern STRING	abmfP_str_bool;
extern STRING	abmfP_str_int;
extern STRING	abmfP_str_string;
extern STRING	abmfP_str_void;
extern STRING	abmfP_str_void_ptr;
extern STRING	abmfP_str_widget;
extern STRING	abmfP_str_xtpointer;

/* common values */
extern STRING	abmfP_str_empty;	/* "" */
extern STRING	abmfP_str_null;		/* "NULL" */
extern STRING	abmfP_str_zero;		/* "0" */

#endif /* _ABMF_WRITE_CP_H_ */

