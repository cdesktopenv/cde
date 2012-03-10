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

