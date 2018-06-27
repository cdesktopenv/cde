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
/* $TOG: docall.c /main/7 1998/04/17 11:22:59 mgreess $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */

#include "stdio.h"
#include "exksh.h" /* which includes sys/types.h */
#include "docall.h"
#include <sys/param.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include "struct.h"
#include "misc.h"
#include "exksh_tbls.h"
#include "basetbl.h"
#include "msgs.h"

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

static int allprint( 
                        unsigned long *pargs,
                        memtbl_t *tbls) ;
static pp_usage( void ) ;
static int call_postprompt( 
                        char * argv0,
                        unsigned long *pargs,
                        memtbl_t *tbls,
                        int *freeit) ;
static long get_prdebug( void ) ;
static long set_prdebug( 
                        long n) ;
static int myprompt( 
                        char *prompt) ;



struct memtbl Null_tbl = { NULL };

static char use[] = "0x%x";
static char use2[] = "%s=0x%x";

int Xk_errno = 0;

int Xkdebug = 0;

char xk_ret_buffer[100];
char *xk_ret_buf = xk_ret_buffer;
struct Bfunction xk_prdebug = { get_prdebug, set_prdebug };

int
do_field_get(
        int argc,
        char **argv )
{
	char buf[BIGBUFSIZ], *p, *bufstart;
	char *fld, *type, *ptr, *ptr2, **pptr2;
	memtbl_t tbl[2], *tbl2;
	int i;
	char *targvar = NULL;
	char fail = 0, always_ptr;
        char * errmsg;

	always_ptr = 0;
	for (i = 1; (i < argc) && argv[i] != NULL && argv[i][0] == '-'; i++) {
		switch(argv[i][1]) {
		   case 'p':
			always_ptr = 1;
			break;
		   case 'v':
			targvar = argv[++i];
			break;
		}
	}

        if ((i + 1) >= argc)
        {
           XK_USAGE(argv[0]);
        }

	type = argv[i++];
	if (!isdigit(argv[i][0]))
		always_ptr = 1;
	ptr = (char *) getaddr(argv[i++]);
	tbl[1] = Null_tbl;
	if (!type || !ptr || (parse_decl(argv[0], tbl, type, 1) == FAIL)) {
		if (!type || !ptr)
                {
		   XK_USAGE(argv[0]);
                }
		else
                {
		   errmsg = strdup(GETMESSAGE(4,1, 
                                   "Cannot parse the structure named '%s'; it may not have been defined"));
		   printerrf(argv[0], errmsg, type, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL);
                   free(errmsg);
                   return(SH_FAIL);
                }
	}
	if ((always_ptr || !IS_SIMPLE(tbl)) && !tbl->ptr && !(tbl->flags & F_TYPE_IS_PTR))
		tbl->ptr = 1;
	else while (tbl->ptr > 1) {
		ptr = *((void **) ptr);
		tbl->ptr--;
	}
	Pr_tmpnonames = 1;
	p = buf;
	if (targvar) {
		strcpy(p, targvar);
		p += strlen(p);
		*p++ = '=';
		bufstart = p;
	}
	else
		bufstart = buf;
	while ((i < argc) && (fld = argv[i++])) {
		if (p != bufstart)
			*p++ = targvar ? ' ' : '\n';
		tbl2 = tbl;
		ptr2 = ptr;
		pptr2 = &ptr2;
		if (!C_PAIR(fld, '.', '\0'))
			tbl2 = ffind(tbl, fld, (char **)&pptr2);
		if (!tbl2) {
                        errmsg = strdup(GetSharedMsg(DT_BAD_FIELD_NAME));
			printerrf(argv[0], errmsg, fld, type,
                                  NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			fail = 1;
			break;
		}
		if (XK_PRINT(tbl2, &p, (char *)pptr2, 0, 0, NULL, 
                              all_tbl_find) == FAIL) 
                {
                        errmsg=strdup(GETMESSAGE(4,2, 
                            "Cannot print the field '%s' in structure '%s'"));
			printerrf(argv[0], errmsg, fld, type,
                                  NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			fail = 1;
			break;
		}
	}
	if (!fail) {
		*p = '\0';
		if (targvar)
			env_set(buf);
		else
			ALTPUTS(buf);
	}
	Pr_tmpnonames = 0;
	return(fail ? SH_FAIL : SH_SUCC);
}

static int
allprint(
        unsigned long *pargs,
        memtbl_t *tbls )
{
	char buf[BIGBUFSIZ], *p;
	int i;
        char * errmsg;

	for (i = 0; tbls[i].name; i++) {
		errmsg = strdup(GETMESSAGE(4,3, "Argument %d (type %s):\n\t"));
		printf(errmsg, i + 1, tbls[i].name);
                free(errmsg);
		p = buf;
		XK_PRINT(tbls + i, &p, (char *)(pargs + i), 0, 0, NULL, 
                         all_tbl_find);
		ALTPUTS(buf);
	}
}

static
pp_usage( void )
{
        char * errmsg;

	errmsg = strdup(GETMESSAGE(4,4, 
                 "Please enter p(rint), s(end), q(uit) or field=value\n"));
	printf(errmsg);
        free(errmsg);
}

static int
call_postprompt(
        char * argv0 ,
        unsigned long *pargs,
        memtbl_t *tbls,
        int *freeit )
{
	char buf[BUFSIZ];
        char * errmsg;
        char * quitStr, *printStr, *sendStr, *promptStr;
        int returnVal = 0;

        quitStr = strdup(GETMESSAGE(4,5, "q"));
        printStr = strdup(GETMESSAGE(4,6, "p"));
        sendStr = strdup(GETMESSAGE(4,7, "s"));
	promptStr = strdup(GETMESSAGE(4,8, "Postprompt: "));

	for ( ; ; ) {
		myprompt(promptStr);
                strcpy(buf, quitStr);

		*buf = '\0';
		fgets(buf, sizeof(buf), stdin);
    		if (strlen(buf) && buf[strlen(buf)-1] == '\n')
      		  buf[strlen(buf)-1] = '\0';

		if (xk_Strncmp(buf, quitStr, 2) == 0)
		{
			errmsg=strdup(GETMESSAGE(4,9, 
                                   "Warning: command was not executed\n"));
			printf(errmsg);
                        free(errmsg);
			returnVal = 0;
                        break;
		}
		else if (xk_Strncmp(buf, printStr, 2) == 0)
			allprint(pargs, tbls);
		else if (xk_Strncmp(buf, sendStr, 2) == 0)
                {
			returnVal = 1;
                        break;
                }
		else if (!strchr(buf, '=') || 
                 (asl_set(argv0,tbls, buf, (unsigned char **)pargs) == SH_FAIL))
                {
			pp_usage();
                }
	}

        free(quitStr);
        free(printStr);
        free(sendStr);
        free(promptStr);
        return(returnVal);
}

#define ZERORET		0
#define NONZERO		1
#define NONNEGATIVE	2

/* In shell, 0 is success so, ZERORET means direct return, NONZERO means
** return the opposite of its truth value and NONNEGATIVE means return
** true if the value IS negative (since FALSE is success)
*/
#define CALL_RETURN(RET) return(SET_RET(RET), ((ret_type == ZERORET) ? (RET) : ((ret_type == NONZERO) ? !(RET) : ((RET) < 0))))
#define EARLY_RETURN(RET) return(SET_RET(RET))
#define SET_RET(RET) (((int) sprintf(xk_ret_buffer, use, (RET))), (int) (xk_ret_buf = xk_ret_buffer), RET)

int
do_call(
        int argc,
        char **argv )
{
	void *pargs[MAX_CALL_ARGS];
	memtbl_t tblarray[MAX_CALL_ARGS];
	char freeit[MAX_CALL_ARGS];
	unsigned long (*func)();
	char *p;
	char dorun, promptflag;
	unsigned char freeval, ret_type;
	int i, j, ret;
        char * msg;
        char * errbuf;
        char * errmsg;

	promptflag = 0;
	freeval = 1;
	ret_type = ZERORET;
	dorun = 1;
	if (argc == 1) {
		errmsg = strdup(GetSharedMsg(DT_NO_FUNC_NAME));
		printerr(argv[0], errmsg, NULL);
                free(errmsg);
                xk_usage(argv[0]);
		EARLY_RETURN(1);
	}

	for (j = 1; (j < argc) && argv[j][0] == '-'; j++) {
		for (i = 1; argv[j][i]; i++) {
			switch(argv[j][i]) {
			case 'F':
				/* Do not free */
				freeval = 0;
				break;
			case 'r':
				/* reverse sense of return value */
				ret_type = NONZERO;
				break;
			case 'n':
				/* Non-negative return value is okay */
				ret_type = NONNEGATIVE;
				break;
			default:
                                errmsg =strdup(GetSharedMsg(DT_UNKNOWN_OPTION));
				printerrf(argv[0], errmsg,
                                          argv[j], NULL, NULL, NULL,
                                          NULL, NULL, NULL, NULL);
                                free(errmsg);
                                xk_usage(argv[0]);
				EARLY_RETURN(1);
			}
		}
	}
	if (j >= argc) {
		errmsg = strdup(GetSharedMsg(DT_NO_FUNC_NAME));
		printerr(argv[0], errmsg, NULL);
                free(errmsg);
                xk_usage(argv[0]);
		CALL_RETURN(1);
	}
	memset(tblarray, '\0', MAX_CALL_ARGS * sizeof(memtbl_t));
	memset(pargs, '\0', MAX_CALL_ARGS * sizeof(void *));
	memset(freeit, '\0', MAX_CALL_ARGS * sizeof(char));
	func = (unsigned long (*)()) fsym(argv[j], -1);
	if (!func && ((argv[j][0] != '0') || (UPP(argv[j][1]) != 'X') || !(func = (unsigned long (*)()) strtoul(argv[j], &p, 16)) || *p)) {
		errmsg = strdup(GETMESSAGE(4,10, 
                                  "Unable to locate the function '%s'"));
		printerrf(argv[0], errmsg, 
                         argv[j], NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                free(errmsg);
		CALL_RETURN(1);
	}
	j++;
	for (i = 0; (i < MAX_CALL_ARGS) && (j < argc) && argv[j]; j++, i++) {
		char *val;
		char type[100];

		if (C_PAIR(argv[j], '+', '?')) {
			promptflag = 1;
			continue;
		}
		else if (C_PAIR(argv[j], '+', '+')) {
			j++;
			break;
		}
		if (argv[j][0] == '@') {
			if (!(val = strchr(argv[j] + 1, ':'))) {
				dorun = 0;
				ret = -1;
				break;
			}
			strncpy(type, argv[j] + 1, val - argv[j] - 1);
			type[val - argv[j] - 1] = '\0';
			val++;
			if (parse_decl(argv[0], tblarray + i, type, 1) == FAIL)
                        {
				dorun = 0;
				ret = -1;
				break;
			}
			else {
				if (!strparse(tblarray + i, 
                                        (char **)(pargs + i), val)) 
                                {
                                        errmsg=strdup(GETMESSAGE(4,11, 
                                            "The value descriptor '%s' does not match the definition for structure '%s'"));
					printerrf(argv[0], errmsg,
                                                 val, type, NULL, NULL, NULL,
                                                 NULL, NULL, NULL);
                                        free(errmsg);
					dorun = 0;
					ret = -1;
					break;
				}
				else
					freeit[i] = freeval;
			}
		}
		else if (isdigit(argv[j][0])) {
			char *p;

			p = argv[j];
			tblarray[i] = T_unsigned_long[0];
			xk_par_int(&p, pargs + i, NULL);
		}
		else if (strcmp(argv[j], (char *) "NULL") == 0) {
			tblarray[i] = T_unsigned_long[0];
			pargs[i] = NULL;
		}
		else {
			pargs[i] = (void *) argv[j];
			tblarray[i] = T_string_t[0];
		}
	}
	/* Process special arguments */
	while (j < argc) {
		asl_set(argv[0], tblarray, argv[j], (unsigned char **)pargs);
		j++;
	}
	if (dorun) {
		if (!promptflag || 
                    call_postprompt(argv[0], (unsigned long *)pargs, tblarray, 
                                     (int *)freeit))
                {
			ret = (*func)(pargs[0], pargs[1], pargs[2], pargs[3], 
                                      pargs[4], pargs[5], pargs[6], pargs[7], 
                                      pargs[8], pargs[9], pargs[10], pargs[11],
                                      pargs[12], pargs[13], pargs[14]);
                }
		else
			ret = 0;
		Xk_errno = errno;
	}
	for (i = 0; i < MAX_CALL_ARGS; i++) {
		if (pargs[i] && freeit[i])
                {
			/* There is no recourse for failure */
			XK_FREE(tblarray + i, (char *)(pargs + i), 0, 0, 
                                all_tbl_find);
                }
	}
	CALL_RETURN(ret);
}

int _Prdebug;

static long
get_prdebug( void )
{
	return(_Prdebug);
}

static long
set_prdebug(
        long n )
{
	_Prdebug = n;
}


int
asl_set(
        char *argv0,
        memtbl_t *tblarray,
        char *desc,
        unsigned char **pargs )
{
	char *ptr;
	char *val;
	memtbl_t *tbl;
	memtbl_t usetbl[2];
	char op;
	char field[80], *fldp = field;
	unsigned long intval, i, newval;
	unsigned long top, bottom;
        char * errmsg;

	if ((val = strchr(desc, '=')) == NULL)
		return(SH_FAIL);
	if (ispunct(val[-1]) && (val[-1] != ']')) {
		op = val[-1];
		strncpy(field, desc, val - desc - 1);
		field[val - desc - 1] = '\0';
		val++;
	}
	else {
		op = '\0';
		strncpy(field, desc, val - desc);
		field[val - desc] = '\0';
		val++;
	}
	if (isdigit(fldp[0])) {
		top = bottom = strtoul(fldp, &fldp, 0) - 1;
		if (*fldp == '.')
			fldp++;
	}
	else {
		top = 9;
		bottom = 0;
	}
	usetbl[1] = Null_tbl;
	for (i = bottom; i <= top; i++) {
		usetbl[0] = tblarray[i];
		ptr = (char *) (pargs + i);
		if (tbl = ffind(usetbl, fldp, &ptr))
			break;
	}
	if (!tbl || (i > top)) {
		errmsg=strdup(GETMESSAGE(4,12, "Cannot locate the field '%s'"));
		printerrf(argv0, errmsg, fldp, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL);
                free(errmsg);
		return(SH_FAIL);
	}
	if (!op || !(tbl->flags & F_SIMPLE)) 
        {
	   if (XK_PARSE(tbl, &val, ptr, 0, 0, NULL, all_tbl_find) < 0)
           {
	      errmsg = strdup(GETMESSAGE(4,13, 
                     "Cannot set the following value for the field '%s': %s"));
	      printerrf(argv0, errmsg, val, NULL,
                                  NULL, NULL, NULL, NULL, NULL, NULL);
              free(errmsg);
           }
	}
	else {
		xk_par_int(&val, &newval, NULL);
		switch (tbl->size) {
		case sizeof(long):
			intval = ((unsigned long *) ptr)[0];
			break;
		case sizeof(short):
			intval = ((unsigned short *) ptr)[0];
			break;
		case sizeof(char):
			intval = ((unsigned char *) ptr)[0];
			break;
		default:
			if (tbl-size == sizeof(int))
			{
			    intval = ((unsigned int *) ptr)[0];
			    break;
			}
		}
		switch(op) {
		case '+':
			intval += newval;
			break;
		case '-':
			intval -= newval;
			break;
		case '*':
			intval *= newval;
			break;
		case '/':
			intval /= newval;
			break;
		case '%':
			intval %= newval;
			break;
		case '&':
			intval &= newval;
			break;
		case '|':
			intval |= newval;
			break;
		case '^':
			intval ^= newval;
			break;
		}
		switch (tbl->size) {
		case sizeof(long):
			((unsigned long *) ptr)[0] = intval;
			break;
		case sizeof(short):
			((unsigned short *) ptr)[0] = intval;
			break;
		case sizeof(char):
			((unsigned char *) ptr)[0] = intval;
			break;
		default:
			if (tbl->size == sizeof(int))
			{
			    ((unsigned int *) ptr)[0] = intval;
			    break;
			}
		}
	}
	return(SH_SUCC);
}

int
do_field_comp(
        int argc,
        char **argv )
{
	char *val, *type;
	void *ptr, *ptr2, **pptr2, *nuptr;
	memtbl_t tbl[2], *tbl2;
	unsigned int i;
	unsigned char always_ptr;
	char pr1[5 * BUFSIZ], pr2[5 * BUFSIZ], *p1, *p2;
        char * errbuf;
        char * msg;
        char * errmsg;

	i = 1;
	if (argc > 1 && C_PAIR(argv[i], '-', 'p')) {
		i++;
		always_ptr = 1;
	}
	else
		always_ptr = 0;

        if ((i + 2) > argc)
        {
           XK_USAGE(argv[0]);
        }

	type = argv[i++];
	if (!isdigit(argv[i][0]))
		always_ptr = 1;
	ptr = getaddr(argv[i++]);
	tbl[1] = Null_tbl;
	if (!type || !ptr || (parse_decl(argv[0], tbl, type, 1) == FAIL))
        {
		XK_USAGE(argv[0]);
        }
	if ((always_ptr || !IS_SIMPLE(tbl)) && !tbl->ptr && !(tbl->flags & F_TYPE_IS_PTR))
		tbl->ptr = 1;
	else while (tbl->ptr > 1) {
		ptr = *((void **) ptr);
		tbl->ptr--;
	}
	for ( ; (i < argc) && argv[i]; i++) {
		tbl2 = tbl;
		ptr2 = ptr;
		pptr2 = &ptr2;
		if (val = strchr(argv[i], '=')) {
			*val++ = '\0';
			tbl2 = ffind(tbl, argv[i], (char **)&pptr2);
			if (!tbl2) {
                           errmsg = strdup(GetSharedMsg(DT_BAD_FIELD_NAME));
			   printerrf(argv[0], errmsg, argv[i],
                                     type, NULL, NULL, NULL, NULL, NULL, NULL);
                           free(errmsg);
			   return(SH_FAIL);
			}
			val[-1] = '=';
		}
		else
			val = argv[i];
		p1 = pr1;
		p2 = pr2;
		Pr_tmpnonames = 1;
		XK_PRINT(tbl2, &p1, (char *)pptr2, 0, 0, NULL, all_tbl_find);
		if (XK_PARSE(tbl2, &val, (char *)&nuptr, 0, 0, NULL, 
                    all_tbl_find) < 0) 
                {
			errmsg=strdup(GETMESSAGE(4,15, 
                                  "Cannot parse the following expression: %s"));
			printerrf(argv[0], errmsg, argv[i],
                                  NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        free(errmsg);
			return(SH_FAIL);
		}
		XK_PRINT(tbl2, &p2, (char *)&nuptr, 0, 0, NULL, all_tbl_find);
		XK_FREE(tbl2, (char *)&nuptr, 0, 0, all_tbl_find);
		Pr_tmpnonames = 0;
		if (strcmp(pr1, pr2)) {
			if (env_get((char *) "PRCOMPARE"))
                        {
				errmsg=strdup(GETMESSAGE(4,16, 
                                        "The following comparision failed: '%s'\n\tActual:  %s\n\tCompare: %s"));
				printerrf(argv[0], errmsg,
                                          argv[i], pr1, pr2, NULL, NULL, NULL,
                                          NULL, NULL);
                                free(errmsg);
                        }
			return(SH_FAIL);
		}
	}
	return(SH_SUCC);
}

static int
myprompt(
        char *prompt )
{
	fprintf(stderr,prompt);
}


#if 0
/* This needs a functional proto, and needs to be extern'ed in docall.h */
unsigned long
strprint(va_alist)
va_dcl
{
	va_list ap;
	char *arg;
	char *variable = NULL;
	memtbl_t tbl;
	char *p;
	char buf[5 * BUFSIZ];
	char *name;
	void *val;
	char always_ptr;
	int nonames = 0;
	int ret;

	va_start(ap);
	always_ptr = 0;
	while ((arg = (char *) va_arg(ap, unsigned long)) && (arg[0] == '-')) {
		int i;

		for (i = 1; arg[i]; i++) {
			switch (arg[i]) {
			case 'v':
				variable = va_arg(ap, char *);
				i = strlen(arg) - 1;
				break;
			case 'p':
				always_ptr = 1;
				break;
			case 'N':
				nonames = 1;
			}
		}
	}
	name = arg;
	if (!arg) {
		printerr(argv[0], "Insufficient arguments", NULL);
		va_end(ap);
		return(SH_FAIL);
	}
	val = (void *) va_arg(ap, unsigned long);
	va_end(ap);
	if (parse_decl("strprintf", &tbl, name, 1) == FAIL)
		return(SH_FAIL);
	if (variable)
		p = buf + lsprintf(buf, "%s=", variable);
	else
		p = buf;
	if ((always_ptr || !IS_SIMPLE(&tbl)) && !tbl.ptr && !(tbl.flags & F_TYPE_IS_PTR))
		tbl.ptr = 1;
	if (!val && (tbl.ptr || (tbl.flags & F_TYPE_IS_PTR))) {
		printerr(argv[0], "NULL value argument to strprint", NULL);
		return(SH_FAIL);
	}
	if (always_ptr && (tbl.flags & F_TYPE_IS_PTR))
		val = *((void **) val);
	else while (tbl.ptr > 1) {
		val = *((void **) val);
		tbl.ptr--;
	}
	Pr_tmpnonames = nonames;
	ret = XK_PRINT(&tbl, &p, (void *) &val, 0, 0, NULL, all_tbl_find);
	Pr_tmpnonames = 0;
	if (ret == FAIL)
		return(SH_FAIL);
	if (variable)
		env_set(buf);
	else
		ALTPUTS(buf);
	return(SH_SUCC);
}
#endif
