/* $XConsortium: xmname.c /main/3 1995/11/01 15:59:34 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF          */
/*	UNIX System Laboratories, Inc.			        */
/*	The copyright notice above does not evidence any        */
/*	actual or intended publication of such source code.     */

#define mount   _AST_mount
#include        <ast.h>
#include        <signal.h>
#include        "shtable.h"
#include        "name.h"
#ifdef KSHELL
#   include     "builtins.h"
#   include     "jobs.h"
#   include     "FEATURE/cmds"
#if defined(__STDC__) || defined(__STDPP__)
#   define      bltin(x)        (b_##x)
#else
#   define      bltin(x)        (b_/**/x)
#endif

#else
#   define bltin(x)     0
#endif

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#define NO_AST
#include "dtksh.h"
#undef NO_AST
#include	"dtextra.h"
#include	"xmextra.h"
#include	"msgs.h"

#define VALPTR(x)	((char*)x)

const struct shtable3 dtk_functions[] = {
	DTK_EXTRA_TABLE
	DTK_EXTRA_TABLE2
	DTK_TK_EXTRA_TABLE
	DTK_TK_LIST_TABLE
	DTK_TK_TEXT_TABLE
	{ NULL, 0, NULL }
};

const struct shtable2 dtk_aliases[] = {
	DTK_EXTRA_ALIAS
	DTK_TK_EXTRA_ALIAS
	{ NULL, 0, NULL }
};
