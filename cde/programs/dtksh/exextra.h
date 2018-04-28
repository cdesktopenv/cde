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
/* $XConsortium: exextra.h /main/3 1995/11/01 15:53:50 rswiston $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */

#ifndef _Dtksh_exextra_h
#define _Dtksh_exextra_h


#define EXKSH_EXTRA_TABLE \
	{ "call", NV_BLTIN|BLT_ENV|BLT_SPC, do_call }, \
	{ "field_comp", NV_BLTIN|BLT_ENV|BLT_SPC, do_field_comp }, \
	{ "field_get", NV_BLTIN|BLT_ENV|BLT_SPC, do_field_get }, \
	{ "sizeof", NV_BLTIN|BLT_ENV|BLT_SPC, do_sizeof }, \
	{ "findsym", NV_BLTIN|BLT_ENV|BLT_SPC, do_findsym }, \
	{ "finddef", NV_BLTIN|BLT_ENV|BLT_SPC, do_finddef }, \
	{ "deflist", NV_BLTIN|BLT_ENV|BLT_SPC, do_deflist }, \
	{ "define", NV_BLTIN|BLT_ENV|BLT_SPC, do_define }, \
	{ "structlist", NV_BLTIN|BLT_ENV|BLT_SPC, do_structlist }, \
	{ "deref", NV_BLTIN|BLT_ENV|BLT_SPC, do_deref }, \
	{ "struct", NV_BLTIN|BLT_ENV|BLT_SPC, do_struct }, \
	{ "typedef", NV_BLTIN|BLT_ENV|BLT_SPC, do_typedef }, \
	{ "symbolic", NV_BLTIN|BLT_ENV|BLT_SPC, do_symbolic }, \


#define EXKSH_EXTRA_VAR \
	"RET",	NV_NOFREE|NV_RDONLY, (char*)(&xk_ret_buffer[0]), \
	"PRDEBUG",	NV_NOFREE|NV_INTEGER, (char*)(&xk_prdebug),

#define EXKSH_EXTRA_ALIAS \
	"args",		NV_NOFREE|NV_EXPORT, "setargs \"$@\"",

#endif /* _Dtksh_exextra_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
