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
/* $XConsortium: extra.c /main/4 1995/11/01 15:54:55 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */


#include	"defs.h"
#include	"shell.h"
#include	"name.h"
#include	"stdio.h"
#include	"msgs.h"


void
env_set(
        char *var )
{
	(void)nv_open(var, sh.var_tree, NV_ASSIGN);
}

void
env_set_gbl(
        char *vareqval )
{
        env_set(vareqval);
}

char *
env_get(
        char *var )
{
	Namval_t *np;
	char *val;

	np = nv_search(var,sh.var_tree,0);
	if (np && (val = nv_getval(np)))
		return(val);
	return(NULL);
}


int
ksh_eval(
        char *cmd )
{
        sh_eval(sfopen(NIL(Sfile_t*),cmd,"s"),0);
        sfsync(sh.outpool);
	return(sh.exitval);
}

void
env_set_var(
        char *var,
        char *val )
{
	int len;
	char tmp[512];
	char *set = &tmp[0];

	if ((len = strlen(var) + strlen(val?val:"") + 2) > sizeof(tmp)) /* 11/06 CHANGED */
		set = malloc(len);
	strcpy(set, var);
	strcat(set, "=");
	strcat(set, val?val:""); 			/* 11/06 CHANGED */
	env_set(set);
	if (set != &tmp[0])
		free(set);
}

void
env_blank(
        char *var )
{
	env_set_var(var, "");
}

void
printerr(
        char *cmd,
        char *msg1,
        char *msg2 )
{
	if (msg1 == NULL)
		msg1 = "";
	if (msg2 == NULL)
		msg2 = "";
        if (cmd && (strlen(cmd) > 0))
	   printf( "%s: %s %s\n", cmd, msg1, msg2);
        else
	   printf( "%s %s\n", msg1, msg2);
}

void
printerrf(
        char *cmd,
        char *fmt,
        char *arg0,
        char *arg1,
        char *arg2,
        char *arg3,
        char *arg4,
        char *arg5,
        char *arg6,
        char *arg7 )
{
	char buf[2048];
	if (arg0 == NULL)
		arg0 = "";
	if (arg1 == NULL)
		arg1 = "";
	if (arg2 == NULL)
		arg2 = "";
	if (arg3 == NULL)
		arg3 = "";
	if (arg4 == NULL)
		arg4 = "";
	if (arg5 == NULL)
		arg5 = "";
	if (arg6 == NULL)
		arg6 = "";
	if (arg7 == NULL)
		arg7 = "";

	sprintf(buf, fmt, arg0, arg1, arg2, arg3,arg4, arg5, arg6, arg7);
        if (cmd && (strlen(cmd) > 0))
	   printf("%s: %s\n", cmd, buf);
        else
	   printf("%s\n", buf);
}
