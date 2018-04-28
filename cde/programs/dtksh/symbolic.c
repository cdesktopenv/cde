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
/* $XConsortium: symbolic.c /main/4 1995/11/01 15:56:54 rswiston $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */

#include        "name.h"
#include        "shell.h"
#include "stdio.h"
#include "exksh.h" /* which includes sys/types.h */
#include "struct.h" /* which includes sys/types.h */
#include <string.h>
#include "msgs.h"

struct symlist *Symlist = NULL;
int Nsymlist;

/*
** There is an implicit dirty trick going on here.  It is effective,
** efficient and will work anywhere but it is tricky.  A memtbl has
** strings in it.  The fact that a byte-by-byte comparison is being
** done on a memtbl means that pointers are being compared.  This
** means that EVERY UNIQUE MEMTBL SHOULD HAVE SOME UNIQUE FIELD (i.e.
** in this case, the string for the name field).  If somebody uses
** an algorithm in do_struct() that saves string space (by seeing if
** the same string is lying around) this code will break and an ID
** field will be necessary to maintain uniqueness.
*/


struct symlist *
fsymbolic(
        struct memtbl *tbl )
{
	int i;

	for (i = 0; i < Nsymlist; i++)
		if (memcmp(tbl, &Symlist[i].tbl, sizeof(struct memtbl)) == 0)
			return(Symlist + i);
	return(NULL);
}

int
do_symbolic(
        int argc,
        char **argv )
{
	int i, nsyms, isflag;
	unsigned long j;
	struct symarray syms[50];
	struct memtbl *tbl;
	char *p;
	char *type = NULL;
        char * errmsg;

	nsyms = 0;
	isflag = 0;
	for (i = 1; (i < argc) && argv[i]; i++) {
		if (argv[i][0] == '-') {
			for (j = 1; argv[i][j]; j++) {
				switch(argv[i][j]) {
				case 'm':
					isflag = 1;
					break;
				case 't':
					if (argv[i][j + 1])
						type = argv[i] + j + 1;
					else
						type = argv[++i];
					j = strlen(argv[i]) - 1;
					break;
				}
			}
		}
		else {
			syms[nsyms++].str = argv[i];
			if (nsyms == 50)
				break;
		}
	}
	if ((type == NULL) || (!nsyms)) {
		XK_USAGE(argv[0]);
	}
	if (p = strchr(type, '.')) {
		*p = '\0';
		if ((tbl = all_tbl_search(type, 0)) == NULL) {
			*p = '.';
                        errmsg = strdup(GetSharedMsg(DT_UNDEF_TYPE));
			printerrf(argv[0], errmsg, type, NULL, NULL,
                                  NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			return(SH_FAIL);
		}
		if ((tbl = ffind(tbl, p + 1, NULL)) == NULL) {
                        errmsg=strdup(GETMESSAGE(13,1, 
                           "Unable to locate a field named '%s' for the type '%s'"));
			printerrf(argv[0], errmsg, p + 1, type, NULL,
                                  NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			*p = '.';
			return(SH_FAIL);
		}
		*p = '.';
	}
	else if ((tbl = all_tbl_search(type, 0)) == NULL) {
                errmsg = strdup(GetSharedMsg(DT_UNDEF_TYPE));
		printerrf(argv[0], errmsg, type, NULL, NULL,
                          NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		return(SH_FAIL);
	}
		
	for (i = 0; i < nsyms; i++) {
		if (!fdef(syms[i].str, &j)) {
                        errmsg=strdup(GETMESSAGE(13,2, 
                               "The name '%s' has not been defined"));
			printerrf(argv[0], errmsg, syms[i].str,
                                  NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			return(SH_FAIL);
		}
		syms[i].str = strdup(syms[i].str);
		syms[i].addr = j;
	}
	add_symbolic(isflag, tbl, syms, nsyms);
	return(SH_SUCC);
}

int
add_symbolic(
        int isflag,
        struct memtbl *tbl,
        struct symarray *syms,
        int nsyms )
{
	struct symlist *symptr;

	if ((symptr = fsymbolic(tbl)) == NULL) {
		if (!Symlist)
			Symlist = (struct symlist *) malloc((Nsymlist + 1) * sizeof(struct symlist));
		else
			Symlist = (struct symlist *) realloc(Symlist, (Nsymlist + 1) * sizeof(struct symlist));
		if (!Symlist)
			return(SH_FAIL);
		symptr = Symlist + Nsymlist;
		Nsymlist++;
	}
	else
		free(symptr->syms);
	symptr->tbl = *tbl;
	symptr->nsyms = nsyms;
	symptr->isflag = isflag;
	symptr->syms = (struct symarray *) malloc(nsyms * sizeof(struct symarray));
	memcpy(symptr->syms, syms, nsyms * sizeof(struct symarray));
}
