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
/* $XConsortium: defs.c /main/3 1995/11/01 16:46:09 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*        THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF        *
*                    AT&T BELL LABORATORIES                    *
*         AND IS NOT TO BE DISCLOSED OR USED EXCEPT IN         *
*            ACCORDANCE WITH APPLICABLE AGREEMENTS             *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*              Unpublished & Not for Publication               *
*                     All Rights Reserved                      *
*                                                              *
*       The copyright notice above does not evidence any       *
*      actual or intended publication of such source code      *
*                                                              *
*               This software was created by the               *
*           Advanced Software Technology Department            *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                    {research,attmail}!dgk                    *
*                                                              *
***************************************************************/
/*
 * Ksh - AT&T Bell Laboratories
 * Written by David Korn
 * This file defines all the  read/write shell global variables
 */

#include	"defs.h"
#include	"jobs.h"
#include	"shlex.h"
#include	"edit.h"
#include	"timeout.h"

struct sh_static	sh;
struct shlex_t		shlex;

/* reserve room for writable state table */
char *sh_lexstates[ST_NONE];

#if defined(SHOPT_VSH) || defined(SHOPT_ESH)
    struct	edit	editb;
#endif	/* SHOPT_VSH||SHOPT_ESH */

struct jobs	job;
time_t		sh_mailchk = 600;

#ifdef SHOPT_MULTIBYTE
/*
 * These are default values.  They can be changed with CSWIDTH
 */

char int_charsize[] =
{
	1, CCS1_IN_SIZE, CCS2_IN_SIZE, CCS3_IN_SIZE,	/* input sizes */
	1, CCS1_OUT_SIZE, CCS2_OUT_SIZE, CCS3_OUT_SIZE	/* output widths */
};
#else
char int_charsize[] =
{
	1, 0, 0, 0,	/* input sizes */
	1, 0, 0, 0	/* output widths */
};
#endif /* SHOPT_MULTIBYTE */

