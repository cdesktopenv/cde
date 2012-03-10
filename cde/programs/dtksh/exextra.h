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
