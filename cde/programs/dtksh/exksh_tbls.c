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
/* $XConsortium: exksh_tbls.c /main/4 1995/11/01 15:54:33 rswiston $ */
/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */

#include	"name.h" 
#include	"shell.h" 
#include "stdio.h"
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include "exksh.h"
#include "exksh_tbls.h"
#include "exksh_prpar.h"
#include "docall.h"
#include "msgs.h"

int _Delim;
static int strglen;
static int struct_size;

int Pr_tmpnonames = 0;

static const char *Str_close_curly = "}";
static const char *Str_open_curly = "{";

#define MALMEMBERS	(4)	/* initial number of members of malloc'ed array to malloc */

#define UPPER(C) (islower(C) ? toupper(C) : (C))
#define isvarchar(C) (isalnum(C) || ((C) == '_'))

/*
 * Some of our test programs use xk_parse() to parse cdata from command
 * lines.  This has the drawback that anys or externals will get a malloc'ed
 * buffer for their char * on the send side, which would not normally be
 * freed, because xk_free() knows that such any's usually point into the
 * ubuf rather than being malloced.  On the receive side, this would be
 * true even in our test program.  So, on the send side, we keep a table
 * of any any or external char * that is malloc'ed, and xk_free checks
 * this table before free'ing the given item.  After being freed, the
 * stack is decremented for efficiency.
 */

#define ANYTBLINC	(4)

static char **Anytbl = NULL;
static int Nanytbl = 0;
static int Maxanytbl = 0;

struct special {
	char *name;
	int (*free)();
	int (*parse)();
	int (*print)();
};

#define SPEC_FREE	0
#define SPEC_PARSE	1
#define SPEC_PRINT	2

static struct special *Special = NULL;
static int Nspecs = 0;

static char **Dont = NULL;
static int Ndont, Sdont;

int
(*find_special(
        int type,
        char *name ))()
{
	int i;

	if (!Special)
		return(NULL);
	for (i = 0; i < Nspecs; i++) {
		if (strcmp(Special[i].name, name) == 0) {
			switch(type) {
			case SPEC_PRINT:
				return(Special[i].print);
			case SPEC_FREE:
				return(Special[i].free);
			case SPEC_PARSE:
				return(Special[i].parse);
			}
		}
	}
	return(NULL);
}

int
set_special(
        char *name,
        int (*free)(),
        int (*parse)(),
        int (*print)() )
{
	int i;

	for (i = 0; i < Nspecs; i++)
		if (strcmp(Special[i].name, name) == 0)
			break;
	if (i == Nspecs) {
		if (!Special) {
			Special = (struct special *) malloc(sizeof(struct special));
			Special[0].name = strdup(name);
			Nspecs = 1;
		}
		else {
			Special = (struct special *) realloc(Special, (Nspecs + 1) * sizeof(struct special));
			Special[i].name = strdup(name);
			Nspecs++;
		}
	}
	if (!Special)
		return(FAIL);
	Special[i].free = free;
	Special[i].parse = parse;
	Special[i].print = print;
	return(SUCCESS);
}

/*
 * xk_parse:  Takes a pointer to a structure member table, a pointer
 * to a buffer containing an ascii representation of the structure
 * represented by the table pointer, and the number of pointers saved
 * from previous recursive calls to this routine, and parses the
 * buf into p.
 *
 * Increments buf to the last point at which it read a character,
 * and returns SUCCESS or FAIL.
 */

int
xk_parse(
        memtbl_t *tbl,
        char **buf,
        char *p,
        int nptr,
        int sub,
        void *pass,
        memtbl_t *(*tbl_find)() )
{
	memtbl_t *ntbl;
	int i = 0;
	int skind, delim_type;
	long val = 0;		/* used for choice selection */
	char *np;
	int delim = _Delim;
	int nmal;		/* number of members malloc'ed arrays */
	char *pp;
	int (*spec_parse)();
        char * errmsg;

	if (tbl == NULL) {
		if (_Prdebug)
                {
                   errmsg=strdup(GETMESSAGE(8,1, 
                       "xk_parse: A NULL 'type' table was specified\n"));
	 	   fprintf(stderr, errmsg);
                   free(errmsg);
                }
		return(FAIL);
	}
	xk_skipwhite(buf);
	/*
	 * If this is supposed to be a pointer, and we have a string that
	 * starts with "P" and a number then we should take the pointer
	 * itself. This is done by stripping off the 'p' and parsing it as a
	 * unsigned long.
	 *
	 * Further, if it starts with an '&', then we want the address of
	 * a variable, use fsym() to find it.
	 */
	if (((tbl->flags & F_TYPE_IS_PTR) || ((tbl->ptr + nptr) > 0)) &&
		(((UPPER((*buf)[0]) == 'P') && isdigit((*buf)[1])) || ((*buf)[0] == '&'))) {
		if ((*buf)[0] == '&') {
			char *start;

			(*buf)++;
			for (start = *buf; isvarchar(*buf[0]); (*buf)++)
				;
			if ((((unsigned long *) p)[0] = fsym(start, -1)) == NULL)
				return(FAIL);
		}
		else {
			(*buf)++;
			RIF(xk_par_int(buf, (long *)p, pass));
		}
		if (Ndont == Sdont) {
			if (Dont)
				Dont = (char **) realloc(Dont, (Sdont + 20) * sizeof(char *));
			else
				Dont = (char **) malloc((Sdont + 20) * sizeof(char *));
			if (!Dont) {
                           errmsg = strdup(GetSharedMsg(DT_ALLOC_FAILURE));
			   ALTPUTS(errmsg);
                           free(errmsg);
			   exit(1);
			}
			Sdont += 20;
		}
		Dont[Ndont++] = ((char **) p)[0];
		return(SUCCESS);
	}
	if (tbl->tname && (spec_parse = find_special(SPEC_PARSE, tbl->tname)))
		return(spec_parse(tbl, buf, p, nptr, sub, pass, tbl_find));
	if (tbl->name && (spec_parse = find_special(SPEC_PARSE, tbl->name)))
		return(spec_parse(tbl, buf, p, nptr, sub, pass, tbl_find));
	nptr += tbl->ptr;
	if (sub > 0 && tbl->subscr > 0) {
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,2, 
                         "xk_parse: Multiple array subscripts are not handled in '%s'\n"));
		   fprintf(stderr, errmsg, tbl->name);
                   free(errmsg);
                }
		return(FAIL);
	}
	/*
	 * If there is exactly one pointer associated with this
	 * member, and no length delimiters, and no subscripts,
	 * or there are multiple pointers,
	 * then malloc space for the structure and call ourself
	 * recursively.
	 */
	if ((nptr > 1 && tbl->delim != 0) || (nptr == 1 && tbl->delim == 0 && tbl->subscr == 0)) {
		if (PARPEEK(buf, ",") || PARPEEK(buf, Str_close_curly)) {
			((char **)p)[0] = NULL;
			if (_Prdebug)
                        {
			   errmsg=strdup(GetSharedMsg(DT_XK_PARSE_SET_NULL));
			   fprintf(stderr, errmsg, tbl->name);
                           free(errmsg);
                        }
			return(SUCCESS);
		}
			if (xk_parpeek(buf, "NULL")) {
				RIF(xk_parexpect(buf, "NULL"));
				((char **)p)[0] = NULL;
				if (_Prdebug)
                                {
			           errmsg=strdup(GetSharedMsg(
                                                 DT_XK_PARSE_SET_NULL));
				   fprintf(stderr, errmsg, tbl->name);
                                   free(errmsg);
                                }
				return(SUCCESS);
			}

		if ((((char **)p)[0] = malloc(tbl->size)) == NULL) {
			return(FAIL);
		}
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,3, 
                        "xk_parse: Setting '%s' to malloc'ed address 0x%x, of size %d\n"));
		   fprintf(stderr,errmsg,tbl->name, ((char **)p)[0], tbl->size);
                   free(errmsg);
                }
		return(xk_parse(tbl, buf, ((char **)p)[0], nptr-1-tbl->ptr, sub, pass, tbl_find));
	}
	/*
	 * If there is exactly one pointer level, or one subscripting level,
	 * and there is a delimiter,
	 * and no subscript, then we are a length delimited malloced array.
	 */
	xk_skipwhite(buf);
	if (tbl->delim != 0 && ((nptr == 1 && tbl->subscr == 0) ||
		(nptr == 0 && tbl->subscr != 0 && tbl->kind != K_STRING))) {

		if (tbl->subscr == 0) {
			if (PARPEEK(buf, ",") || PARPEEK(buf, Str_close_curly)) {
				((char **)p)[0] = NULL;
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,4, 
                                     "xk_parse: Setting malloc'ed array '%s' to NULL\n"));
				   fprintf(stderr, errmsg, tbl->name);
                                   free(errmsg);
                                }
				return(SUCCESS);
			}
			if (xk_parpeek(buf, "NULL")) {
				RIF(xk_parexpect(buf, "NULL"));
				((char **)p)[0] = NULL;
				if (_Prdebug)
                                {
				   errmsg=strdup(GetSharedMsg(
                                                    DT_XK_PARSE_SET_NULL));
				   fprintf(stderr, errmsg, tbl->name);
                                   free(errmsg);
                                }
				return(SUCCESS);
			}
			nmal = MALMEMBERS;
			if ((np = malloc(nmal*tbl->size)) == NULL) {
				return(FAIL);
			}
			((char **)p)[0] = np;
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,5, 
                                "xk_parse: Setting member '%s' to malloc'ed pointer 0x%x, of size %d\n"));
			   fprintf(stderr, errmsg, tbl->name, np, 4*tbl->size);
                           free(errmsg);
                        }
		} else {
			np = p;
		}
		xk_skipwhite(buf);
		RIF(PAREXPECT(buf, Str_open_curly));
		*buf += 1;
		i = 0;
		xk_skipwhite(buf);
		while (PARPEEK(buf, Str_close_curly) == FALSE) {
			if (tbl->subscr == 0 && i >= nmal) {
				nmal += MALMEMBERS;
				if((np = realloc(np, nmal*tbl->size)) == NULL) {
					return(FAIL);
				}
				((char **)p)[0] = np;
				if (_Prdebug) {
				   errmsg=strdup(GetSharedMsg(
                                             DT_XK_PARSE_ARRAY_OVERFLOW));
				   fprintf(stderr, errmsg, tbl->name, nmal, nmal*tbl->size);
                                   free(errmsg);
				}
			} else if (tbl->subscr > 0 && i > tbl->subscr) {
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,6, 
                                          "xk_parse: The array '%s' overflowed at element number %d\n"));
				   fprintf(stderr, errmsg, tbl->name, i);
                                   free(errmsg);
                                }
			}
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,7, 
                                 "xk_parse: Parsing array element [%d] of '%s' into address 0x%x\n"));
			   fprintf(stderr, errmsg, i, tbl->name, &np[i*tbl->size]);
                           free(errmsg);
                        }
			if (i) {
				xk_skipwhite(buf);
				if (PARPEEK(buf, ",") == FALSE) {
					if (PARPEEK(buf, Str_close_curly) == FALSE) {
						RIF(PAREXPECT(buf, ","));
						*buf += 1;
					}
				}
				else {
					RIF(PAREXPECT(buf, ","));
					*buf += 1;
				}
			}
			RIF(xk_parse(tbl, buf, &np[i*tbl->size], nptr ? -1 : 0,
                                     0, (void *)-1, tbl_find));
			i++;
			struct_size = i;
			xk_skipwhite(buf);
		}
		RIF(PAREXPECT(buf, Str_close_curly));
		*buf += 1;
		return(SUCCESS);
	}
	/*
	 * If there is no delimiter, and there are two levels of pointer,
	 * then we are a NULL terminated array of pointers
	 */
	if (tbl->delim == 0 &&
		((nptr == 2 && sub == 0) || (sub == 1 && nptr == 1))) {
		/*
		 * malloc a few members, realloc as needed
		 */
		nmal = MALMEMBERS;
		if ((((char **)p)[0] = malloc(nmal*tbl->size)) == NULL) {
			return(FAIL);
		}
		xk_skipwhite(buf);
		RIF(PAREXPECT(buf, Str_open_curly));
		*buf += 1;
		xk_skipwhite(buf);
		while (PARPEEK(buf, Str_close_curly) == FALSE) {
			if (i >= nmal) {
				nmal += MALMEMBERS;
				if ((((char **)p)[0] = realloc(((char **)p)[0], nmal*tbl->size)) == NULL) {
					return(FAIL);
				}
				if (_Prdebug) {
				   errmsg=strdup(GetSharedMsg(
                                               DT_XK_PARSE_ARRAY_OVERFLOW));
				   fprintf(stderr, errmsg, tbl->name, nmal, nmal*tbl->size);
                                   free(errmsg);
				}
			}
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,8, 
                             "xk_parse: Parsing array element [%d] of '%s'\n"));
			   fprintf(stderr, errmsg, i, tbl->name);
                           free(errmsg);
                        }
			if (i) {
				RIF(PAREXPECT(buf, ","));
				*buf += 1;
			}
			RIF(xk_parse(tbl, buf, &p[i*tbl->size], 
                                     nptr == 2 ? -2 : -1, 0, (void *)-1, 
                                     tbl_find));

			xk_skipwhite(buf);
		}
		RIF(PAREXPECT(buf, Str_close_curly));
		*buf++;
		((char **)p)[i*tbl->size] = NULL;
		return(SUCCESS);
	}

	switch(tbl->kind) {
	case K_CHAR:
		RIF(xk_par_int(buf, &val, pass));
		((unsigned char *)p)[0] = val;
		break;
	case K_SHORT:
		RIF(xk_par_int(buf, &val, pass));
		((ushort *)p)[0] = val;
		break;
	case K_INT:
		RIF(xk_par_int(buf, &val, pass));
		((int *)p)[0] = val;
		break;
	case K_LONG:
		RIF(xk_par_int(buf, (long *)p, pass));
		break;
	case K_STRING:
		if (tbl->subscr) {
			val = tbl->subscr;
			RIF(xk_par_chararr(buf, (char *)p, (int *)&val));
			if (tbl->delim <= 0 && val > -1) {
				p[val] = '\0';
			}
		} else {
			val = 0;
			RIF(xk_par_charstr(buf, (char **)p, (int *)&val));
			/* If this is not a delimited char string,
			 * then it must be null terminated
			 */
			if (tbl->delim <= 0 && val > -1) {
				((char **) p)[0][val] = '\0';
			}
			strglen = val;
		}
		break;
	case K_TYPEDEF:
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		RIF(xk_parse(ntbl, buf, p, nptr, 0, pass, tbl_find));
		return(SUCCESS);
	case K_STRUCT:
		xk_skipwhite(buf);
		RIF(PAREXPECT(buf, Str_open_curly));
		*buf += 1;
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		pp = NULL;
		for (i = 0; ntbl[i].name != NULL; i++) {
			_Delim = xk_get_pardelim(&ntbl[i], p);
			if (ntbl[i].kind >= K_DSHORT) {
				skind = ntbl[i].kind;
				pp = p + ntbl[i].offset;
				struct_size = 0;
			}
			if (ntbl[i].delim) {
				delim_type = ntbl[i].kind;
			}
			if (i && ntbl[i-1].kind < K_DSHORT) {
				xk_skipwhite(buf);
				if (PARPEEK(buf, ",") == FALSE) {
					if (PARPEEK(buf, Str_close_curly) == FALSE) {
						RIF(PAREXPECT(buf, ","));
						*buf += 1;
					}
				}
				else  {
					RIF(PAREXPECT(buf, ","));
					*buf += 1;
				}
			}
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,9, 
                              "xk_parse: Parsing member '%s' into location 0x%x\n"));
			   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                           free(errmsg);
                        }
			if (xk_parse(&ntbl[i], buf, p+ntbl[i].offset, nptr, sub, pass, tbl_find) == FAIL) {
				if (_Prdebug)
                                {
				   errmsg=strdup(GetSharedMsg(
                                                    DT_XK_PARSE_ERROR));
				   fprintf(stderr, errmsg, ntbl[i].name);
                                   free(errmsg);
                                }
				return(FAIL);
			}
		}
		if (pp != NULL) {
			switch(skind) {
				case K_DSHORT:
					if (delim_type == K_STRING)
						((short *)pp)[0] = strglen;
					else
						((short *)pp)[0] = struct_size;
					break;
				case K_DINT:
					if (delim_type == K_STRING)
						((int *)pp)[0] = strglen;
					else
						((int *)pp)[0] = struct_size;
					break;
				case K_DLONG:
					if (delim_type == K_STRING)
						((long *)pp)[0] = strglen;
					else
						((long *)pp)[0] = struct_size;
					break;
				default:
					break;
			}
		}
		xk_skipwhite(buf);
		RIF(PAREXPECT(buf, Str_close_curly));
		*buf += 1;
		break;
	case K_UNION:
		if (strncmp(tbl[-1].name, "ch_", 3) != 0) {
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,10, 
                            "xk_parse: Cannot determine the choice in '%s'\n"));
			   fprintf(stderr, errmsg, tbl->name);
                           free(errmsg);
                        }
			return(FAIL);
		}
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		xk_skipwhite(buf);
		RIF(PAREXPECT(buf, Str_open_curly));
		*buf += 1;
		for (i = 0; ntbl[i].name != NULL; i++) {
			if (xk_parpeek(buf, ntbl[i].name) == TRUE) {
				RIF(xk_parexpect(buf, ntbl[i].name));
				((long *)(p - sizeof(long)))[0] = ntbl[i].choice;
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,11, 
                                       "xk_parse: Parsing union member '%s' into location 0x%x\n"));
				   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                                   free(errmsg);
                                }
				if (xk_parse(&ntbl[i], buf, p, nptr, sub, pass, tbl_find) == FAIL) {
				   if (_Prdebug)
                                   {
				      errmsg=strdup(GetSharedMsg(
                                                     DT_XK_PARSE_ERROR));
				      fprintf(stderr, errmsg, ntbl[i].name);
                                      free(errmsg);
                                   }
				   return(FAIL);
				}
				break;
			}
		}
		xk_skipwhite(buf);
		RIF(PAREXPECT(buf, Str_close_curly));
		*buf += 1;
		break;
	case K_DSHORT:
	case K_DINT:
	case K_DLONG:
		break;
	default:
		return(FAIL);
	}
	return(SUCCESS);
}

int
xk_get_delim(
        memtbl_t *tbl,
        char *p )
{
	memtbl_t *dtbl = &tbl[tbl->delim];
        char * errmsg;

	if (tbl->delim == 0) {
		return(-1);
	}
	if (_Prdebug)
        {
	   errmsg=strdup(GETMESSAGE(8,12, 
                 "xk_get_delim: The delimiter for field '%s' is field '%s'\n"));
	   fprintf(stderr, errmsg, tbl->name, dtbl->name);
           free(errmsg);
        }
	p += dtbl->offset;
	switch (dtbl->kind) {
	case K_DLONG:
	case K_LONG:
		return(((long *)p)[0]);
	case K_CHAR:
		return(((char *)p)[0]);
	case K_DSHORT:
	case K_SHORT:
		return(((short *)p)[0]);
	case K_DINT:
	case K_INT:
		return(((int *)p)[0]);
	default:
	   if (_Prdebug)
           {
	      errmsg=strdup(GETMESSAGE(8,13, 
                  "xk_get_delim: Cannot find a delimiter value in '%s'\n"));
	      fprintf(stderr, errmsg, tbl->name);
              free(errmsg);
           }
	   return(0);
	}
}


int
xk_get_pardelim(
        memtbl_t *tbl,
        char *p )
{
	memtbl_t *dtbl = &tbl[tbl->delim];
        char * errmsg;

	if (tbl->delim == 0) {
		return(-1);
	}
	if (_Prdebug)
        {
	   errmsg=strdup(GETMESSAGE(8,14, 
              "xk_get_pardelim: The delimiter for field '%s' is field '%s'\n"));
	   fprintf(stderr, errmsg, tbl->name, dtbl->name);
           free(errmsg);
        }
	p += dtbl->offset;
	switch (dtbl->kind) {
	   case K_DLONG:
		return(-1);
	   case K_LONG:
		return(((long *)p)[0]);
	   case K_DSHORT:
		return(-1);
	   case K_CHAR:
		return(((char *)p)[0]);
	   case K_SHORT:
		return(((short *)p)[0]);
	   case K_DINT:
		return(-1);
	   case K_INT:
		return(((int *)p)[0]);
	   default:
	   if (_Prdebug)
           {
	      errmsg=strdup(GETMESSAGE(8,15, 
                 "xk_get_pardelim: Cannot find a delimiter value in '%s'\n"));
	      fprintf(stderr, errmsg, tbl->name);
              free(errmsg);
           }
	   return(0);
	}
}

/*
 * xk_print:  Takes a pointer to a structure member table, a pointer
 * to a buffer big enough to hold an ascii representation of the structure
 * represented by the table pointer, and a pointer to a structure to
 * be filled in, and the number of pointers saved
 * from previous recursive calls to this routine, and prints the
 * buf into p.
 *
 * Increments buf to the last point at which it wrote a character,
 * and returns SUCCESS or FAIL.
 */


int
xk_print(
        memtbl_t *tbl,
        char **buf,
        char *p,
        int nptr,
        int sub,
        void *pass,
        memtbl_t *(*tbl_find)() )
{
	memtbl_t *ntbl;
	int i;
	long val;		/* used for choice selection */
	char *np;
	int delim = _Delim;
	int (*spec_print)();
        char * errmsg;

	if (p == NULL) {
		*buf += lsprintf(*buf, "NULL");
		return(SUCCESS);
	}
	if (tbl == NULL) {
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,16, 
                       "xk_print: A NULL 'type' table was specified\n"));
		   fprintf(stderr, errmsg);
                   free(errmsg);
                }
		return(FAIL);
	}
	if (tbl->tname && (spec_print = find_special(SPEC_PRINT, tbl->tname)))
		return(spec_print(tbl, buf, p, nptr, sub, pass, tbl_find));
	if (tbl->name && (spec_print = find_special(SPEC_PRINT, tbl->name)))
		return(spec_print(tbl, buf, p, nptr, sub, pass, tbl_find));
	nptr += tbl->ptr;
	if (sub > 0 && tbl->subscr > 0) {
	   if (_Prdebug)
           {
	      errmsg=strdup(GETMESSAGE(8,17, 
              "xk_print: Multiple array subscripts are not handled in '%s'\n"));
	      fprintf(stderr, errmsg, tbl->name);
              free(errmsg);
           }
	   return(FAIL);
	}
	/*
	 * If there is exactly one pointer associated with this
	 * member, and no length delimiters, and no subscripts,
	 * or there are multiple pointers,
	 * then dereference the structure and call ourself
	 * recursively.
	 */
	if ((nptr > 1 && tbl->delim != 0) || (nptr == 1 && tbl->delim == 0 && tbl->subscr == 0)) {
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,18, 
                          "xk_print: Dereferencing '%s' to address 0x%x\n"));
		   fprintf(stderr, errmsg, tbl->name, ((char **)p)[0]);
                   free(errmsg);
                }
		return(xk_print(tbl, buf, ((char **)p)[0], nptr-1-tbl->ptr, sub, pass, tbl_find));
	}
	/*
	 * If there is exactly one pointer level, or one subscripting level,
	 * and there is a delimiter,
	 * and no subscript, then we are a length delimited array.
	 */
	if (tbl->delim != 0 && ((nptr == 1 && tbl->subscr == 0) ||
		nptr == 0 && tbl->subscr != 0 && tbl->kind != K_STRING)) {

		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,19, 
                             "xk_print: The delimiter for '%s' is %d\n"));
		   fprintf(stderr, errmsg, tbl->name, delim);
                   free(errmsg);
                }
		if (tbl->subscr == 0) {
			np = ((char **)p)[0];
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,20, 
                             "xk_print: Using the pointer 0x%x as an array\n"));
			   fprintf(stderr, errmsg, np);
                           free(errmsg);
                        }
		} else {
			np = p;
		}
		if (np == NULL) {
		 	*buf += lsprintf(*buf, "NULL");
			return(SUCCESS);
		}
		*buf += lsprintf(*buf, Str_open_curly);
		for (i = 0; i < delim; i++) {
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,21, 
                                  "xk_print: Printing array level %d of member '%s' at location 0x%x\n"));
			   fprintf(stderr, errmsg, i, tbl->name, &np[i*tbl->size]);
                           free(errmsg);
                        }
			if (i)
				*buf += lsprintf(*buf, ", ");
			RIF(xk_print(tbl, buf, &np[i*tbl->size], nptr ? -1 : 0,
                            0, (void *)-1, tbl_find));
		}
		*buf += lsprintf(*buf, Str_close_curly);
		return(SUCCESS);
	}
	/*
	 * If there is no delimiter, and there are two levels of pointer,
	 * then we are a NULL terminated array.
	 */
	if (tbl->delim == 0 &&
		((nptr == 2 && sub == 0) || (sub == 1 && nptr == 1))) {
		*buf += lsprintf(*buf, Str_open_curly);
		for (i = 0; ((char **)p)[i*tbl->size] != NULL; i++) {
			if (i)
				*buf += lsprintf(*buf, ", ");
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,22, 
                               "xk_print: Printing array level %d of member '%s'\n"));
			   fprintf(stderr, errmsg, i, tbl->name);
                           free(errmsg);
                        }
			RIF(xk_print(tbl, buf, ((char **)p)[i*tbl->size], 
                                     nptr-(!sub), 0, (void *)-1, tbl_find));
		}
		*buf += lsprintf(*buf, Str_close_curly);
		return(SUCCESS);
	}

	if (!Pr_tmpnonames && (Pr_format & PRNAMES)) {
		switch(tbl->kind) {
		case K_CHAR:
		case K_SHORT:
		case K_INT:
		case K_LONG:
		case K_STRING:
			*buf += lsprintf(*buf, "%s=", tbl->name);
		}
	}
	switch(tbl->kind) {
	case K_CHAR:
	case K_SHORT:
	case K_INT:
	case K_LONG:
		RIF(xk_prin_int(tbl, buf, (unsigned long *)p));
		break;
	case K_STRING:
		if (delim > 0) {
			if (tbl->subscr) {
				RIF(xk_prin_hexstr(buf, (char *)p, delim));
			} else {
				RIF(xk_prin_hexstr(buf, ((char **)p)[0], delim));
			}
		} else {
			if (tbl->subscr) {
				RIF(xk_prin_nts(buf, (char *)p));
			} else {
				RIF(xk_prin_nts(buf, ((char **)p)[0]));
			}
		}
		break;
	case K_TYPEDEF:
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		return(xk_print(ntbl, buf, p, nptr, 0, pass, tbl_find));
	case K_STRUCT:
		*buf += lsprintf(*buf, Str_open_curly);
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		for (i = 0; ntbl[i].name != NULL; i++) {
			_Delim = xk_get_delim(&ntbl[i], p);
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,23, 
                              "xk_print: Printing member '%s' at location 0x%x\n"));
			   fprintf(stderr, errmsg, ntbl[i].name, p+ntbl[i].offset);
                           free(errmsg);
                        }
			RIF(xk_print(&ntbl[i], buf, p+ntbl[i].offset, nptr, sub, pass, tbl_find));
			if (ntbl[i].kind < K_DSHORT && ntbl[i+1].name != NULL)
				*buf += lsprintf(*buf, ", ");
		}
		*buf += lsprintf(*buf, Str_close_curly);
		break;
	case K_UNION:
		if (strncmp(tbl[-1].name, "ch_", 3) != 0) {
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,24, 
                            "xk_print: Cannot determine the choice in '%s'\n"));
			   fprintf(stderr, errmsg, tbl->name);
                           free(errmsg);
                        }
			return(FAIL);
		}
		val = *((long *)(p - sizeof(long)));
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		*buf += lsprintf(*buf, Str_open_curly);
		for (i = 0; ntbl[i].name != NULL; i++) {
			if (ntbl[i].choice == val) {
				*buf += lsprintf(*buf, "%s ", ntbl[i].name);
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,25, 
                                      "xk_print: Printing union member '%s' into location 0x%x\n"));
				   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                                   free(errmsg);
                                }
				RIF(xk_print(&ntbl[i], buf, p, nptr, sub, pass, tbl_find));
				break;
			}
		}
		*buf += lsprintf(*buf, Str_close_curly);
		break;
	case K_DSHORT:
	case K_DINT:
	case K_DLONG:
		break;
	default:
		return(FAIL);
	}
	return(SUCCESS);
}

/*
 * xk_free:  Takes a pointer to a structure member table, and
 * free any malloc'ec elements in it at all levels.
 * Returns SUCCESS or FAIL.
 *
 * Contains an optimization that if a structure or union contains a
 * type that is a simple type and nptr is zero, does not do a recursive call.
 */

int
xk_free(
        memtbl_t *tbl,
        char *p,
        int nptr,
        int sub,
        memtbl_t *(*tbl_find)() )
{
	memtbl_t *ntbl;
	int i;
	long val;		/* used for choice selection */
	char *np;
	int delim = _Delim;
	int (*spec_free)();
        char * errmsg;

	if (tbl == NULL) {
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,26, 
                       "xk_free: A NULL 'type' table was specified\n"));
		   fprintf(stderr, errmsg);
                   free(errmsg);
                }
		return(FAIL);
	}
	if (tbl->tname && (spec_free = find_special(SPEC_FREE, tbl->tname)))
		return(spec_free(tbl, p, nptr, sub, tbl_find));
	if (tbl->name && (spec_free = find_special(SPEC_FREE, tbl->name)))
		return(spec_free(tbl, p, nptr, sub, tbl_find));
	nptr += tbl->ptr;
	if ((tbl->flags & F_TYPE_IS_PTR) || (nptr > 0)) {
		for (i = Ndont - 1; i >= 0; i--) {
			if (Dont[i] == ((char **) p)[0]) {
				for ( ; i < Ndont - 1; i++)
					Dont[i] = Dont[i + 1];
				Ndont--;
				return(SUCCESS);
			}
		}
	}
	if (sub > 0 && tbl->subscr > 0) {
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,27, 
                       "xk_free: Multiple array subscripts are not handled in '%s'\n"));
		   fprintf(stderr, errmsg, tbl->name);
                   free(errmsg);
                }
		return(FAIL);
	}
	/*
	 * If there is exactly one pointer associated with this
	 * member, and no length delimiters, and no subscripts,
	 * or there are multiple pointers,
	 * then recursively call ourselves on the structure, then
	 * free the structure itself.
	 */
	if ((nptr > 1 && tbl->delim != 0) || (nptr == 1 && tbl->delim == 0 && tbl->subscr == 0)) {
		if (((char **)p)[0] != NULL) {
			RIF(xk_free(tbl, ((char **)p)[0], nptr-1-tbl->ptr, sub, tbl_find));
			free(((char **)p)[0]);
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,28, 
                                  "xk_free: The member '%s' at location 0x%x was freed and set to NULL\n"));
			   fprintf(stderr, errmsg, tbl->name, ((char **)p)[0]);
                           free(errmsg);
                        }
			((char **)p)[0] = NULL;
		} else {
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,29, 
                              "xk_free: The member '%s' is NULL; no free occurred\n"));
			   fprintf(stderr, errmsg, tbl->name);
                           free(errmsg);
                        }
		}
		return(SUCCESS);
	}
	/*
	 * If there is exactly one pointer level, or one subscripting level,
	 * and there is a delimiter,
	 * and no subscript, then we are a length delimited malloced array.
	 * Free each element, then free the whole array.
	 */
	if (tbl->delim != 0 && ((nptr == 1 && tbl->subscr == 0) ||
		nptr == 0 && tbl->subscr != 0 && tbl->kind != K_STRING)) {
		if (_Prdebug)
                {
		   errmsg=strdup(GETMESSAGE(8,30, 
                            "xk_free: The delimiter for '%s' is %d\n"));
		   fprintf(stderr, errmsg, tbl->name, delim);
                   free(errmsg);
                }
		if (tbl->subscr == 0)
			np = ((char **)p)[0];
		else
			np = p;
		for (i = 0; i < delim; i++) {
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,31, 
                                "xk_free: Freeing array element [%d] of '%s' at address 0x%x\n"));
			   fprintf(stderr, errmsg, i, tbl->name, &np[i*tbl->size]);
                           free(errmsg);
                        }
			RIF(xk_free(tbl, &np[i*tbl->size], nptr ? -1 : 0, 0, tbl_find));
		}
		if (tbl->subscr == 0) {
			if (np != NULL) {
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,32, 
                                       "xk_free: Freeing pointer to array of '%s' at location 0x%x and setting to NULL\n"));
				   fprintf(stderr, errmsg, tbl->name, np);
                                   free(errmsg);
                                }
				free(np);
				if (tbl->subscr == 0)
					((char **)p)[0] = NULL;
			} else if (_Prdebug) {
			   errmsg=strdup(GETMESSAGE(8,33, 
                                 "xk_free: The pointer to array of '%s'is NULL; no free occurred\n"));
			   fprintf(stderr, errmsg, tbl->name);
                           free(errmsg);
                        }
		}
		return(SUCCESS);
	}

	switch(tbl->kind) {
	case K_DSHORT:
	case K_SHORT:
	case K_DINT:
	case K_INT:
	case K_DLONG:
	case K_LONG:
		break;
	case K_STRING:
		if (!tbl->subscr) {
			if (((char **)p)[0] != NULL) {
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,34, 
                                      "xk_free: Freeing string '%s' at location 0x%x, and setting to NULL\n"));
				   fprintf(stderr, errmsg, tbl->name, p);
                                   free(errmsg);
                                }
				free(((char **)p)[0]);
				((char **)p)[0] = NULL;
			} else if (_Prdebug) {
			   errmsg=strdup(GETMESSAGE(8,35, 
                                 "xk_free: The string '%s' is NULL; no free occurred\n"));
			   fprintf(stderr, errmsg, tbl->name, p);
                           free(errmsg);
                        }
		}
		break;
	case K_TYPEDEF:
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		return(xk_free(ntbl, p, nptr, 0, tbl_find));
	case K_STRUCT:
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		for (i = 0; ntbl[i].name != NULL; i++) {
			if ((ntbl[i].flags & F_SIMPLE) && nptr+ntbl[i].ptr == 0) {
				if (_Prdebug)
                                {
				   errmsg=strdup(GetSharedMsg(
                                                  DT_XK_FREE_NO_MEMBER));
				   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                                   free(errmsg);
                                }
				continue;
			}
			_Delim = xk_get_delim(&ntbl[i], p);
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,36, 
                            "xk_free: Freeing member '%s' at location 0x%x\n"));
			   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                           free(errmsg);
                        }
			if (xk_free(&ntbl[i], p+ntbl[i].offset, nptr, sub, tbl_find) == FAIL) {
			   if (_Prdebug)
                           {
			      errmsg=strdup(GETMESSAGE(8,37, 
                                 "xk_free: A failure occurred while freeing the '%s' member\n"));
			      fprintf(stderr, errmsg, ntbl[i].name);
                              free(errmsg);
                           }
			   return(FAIL);
			}
		}
		break;
	case K_UNION:
		if (strncmp(tbl[-1].name, "ch_", 3) != 0) {
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,38, 
                             "xk_free: Cannot determine the choice in '%s'\n"));
			   fprintf(stderr, errmsg, tbl->name);
                           free(errmsg);
                        }
			return(FAIL);
		}
		val = *((long *)(p - sizeof(long)));
		ntbl = tbl_find(tbl->tname, tbl->tbl, tbl->id);
		for (i = 0; ntbl[i].name != NULL; i++) {
			if (ntbl[i].choice == val) {
				if ((ntbl[i].flags & F_SIMPLE) && nptr+ntbl[i].ptr == 0) {
					if (_Prdebug)
                                        {
					   errmsg=strdup(GetSharedMsg(
                                                      DT_XK_FREE_NO_MEMBER));
					   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                                           free(errmsg);
                                        }
					continue;
				}
				if (_Prdebug)
                                {
				   errmsg=strdup(GETMESSAGE(8,39, 
                                      "xk_free: Freeing union member '%s' at location 0x%x\n"));
				   fprintf(stderr, errmsg, ntbl[i].name, p + ntbl[i].offset);
                                   free(errmsg);
                                }
				RIF(xk_free(&ntbl[i], p, nptr, sub, tbl_find));
				break;
			}
		}
		if (ntbl[i].name == NULL && _Prdebug)
			if (_Prdebug)
                        {
			   errmsg=strdup(GETMESSAGE(8,40, 
                               "xk_free: There is no legal union choice for '%s' (value is 0x%x); no free occurred\n"));
			   fprintf(stderr, errmsg, tbl->name, val);
                           free(errmsg);
                        }
		break;
	default:
		return(FAIL);
	}
	return(SUCCESS);
}
