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
 *	$XConsortium: gil_loadattP.h /main/3 1995/11/06 18:29:01 rswiston $
 *
 *	@(#)gil_loadattP.h	1.9 20 Jan 1995	cde_app_builder/src/libABil
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
 * loadatt.h - load attribute (GIL) header file
 */
#ifndef _ABIL_LOADATT_H_
#define _ABIL_LOADATT_H_

#include <ab_private/obj.h>
#include "gilP.h"

typedef struct
{
	BOOL			bools[AB_OBJ_MAX_CHILDREN];
	int			count;
} BOOL_ARRAY;

typedef struct
{
	AB_OBJECT_STATE		states[AB_OBJ_MAX_CHILDREN];
	int			count;
} INITIAL_STATE_ARRAY;

typedef struct
{
	ISTRING	strings[AB_OBJ_MAX_CHILDREN];
	int	count;
} ISTRING_ARRAY;

typedef struct
{
	AB_LABEL_TYPE	label_types[AB_OBJ_MAX_CHILDREN];
	int		count;
} LABEL_TYPE_ARRAY;

int     istr_array_init(ISTRING_ARRAY *array);
int     istr_array_uninit(ISTRING_ARRAY *array);

int	gilP_load_attribute_value(FILE *inFile, 
			ABObj obj, AB_GIL_ATTRIBUTE attr, ABObj root);
int	gilP_load_bools(FILE *inFile, BOOL_ARRAY *bools);
int	gilP_load_label_types(FILE *inFile, 
			LABEL_TYPE_ARRAY *label_types);
int	gilP_load_initial_states(FILE *inFile, 
			INITIAL_STATE_ARRAY *states);
int	gilP_load_string(FILE *inFile, ISTRING *string);
int	gilP_load_strings(FILE *inFile, ISTRING_ARRAY *strings);
int	gilP_load_name(FILE *inFile, ISTRING *name);
int	gilP_load_names(FILE *inFile, ISTRING_ARRAY *names);
int	gilP_load_handler(FILE *inFile, ISTRING *handler);
int	gilP_load_handlers(FILE *inFile, ISTRING_ARRAY *handlers);
int	gilP_load_file_names(FILE *inFile, ISTRING_ARRAY *names);
int	gilP_load_file_name(FILE *inFile, ISTRING *name);

#endif /* _ABIL_LOADATT_H_ */

