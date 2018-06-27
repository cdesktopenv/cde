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
/* $XConsortium: exksh_prpar.c /main/4 1995/11/01 15:54:12 rswiston $ */

/*	Copyright (c) 1991, 1992 UNIX System Laboratories, Inc. */
/*	All Rights Reserved     */

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF    */
/*	UNIX System Laboratories, Inc.			*/
/*	The copyright notice above does not evidence any       */
/*	actual or intended publication of such source code.    */


#include        "name.h"
#include        "shell.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include "exksh.h"
#include "stdio.h"
#include "exksh_prpar.h"
#include "symbolic.h"
#include "msgs.h"



static int xk_uppercase( 
                        char *s) ;
static int getsize( 
                        int fd,
                        int *psize) ;



#define OBJ_END	100

struct symlist Val_list;

unsigned int Pr_format = PRSYMBOLIC|PRMIXED|PRNAMES;


/*
 * Takes a pointer to a pointer to a character buffer, and skips any
 * whitespace, as defined by isspace(section 3).  Increments the
 * buf parameter to the first non-whitespace character.
 * Returns SUCCESS, there are no known ways for it to fail.
 */

int
xk_skipwhite(
        char **buf )
{
	while (isspace(**buf))
		(*buf)++;
	return(SUCCESS);
}

int
xk_backskip(
        char **buf,
        int *n )
{
	*n = 0;
	while (isspace(**buf)) {
		(*buf)--;
		*n++;
	}
	if ((*buf)[0] == '\\')
		return(TRUE);
	else
		return(FALSE);
}

/*
 * Takes a pointer to a character buffer and a string.  Sees if
 * the str is present as the first part of the buffer (minus any
 * whitespace), and if so increments the buffer past the string.
 * If not, returns FAIL without incrementing buffer (except perhaps
 * by eating leading whitespace).
 */

int
xk_parexpect(
        char **buf,
        char *str )
{
	RIF(xk_skipwhite(buf));
	if (strncmp(*buf, str, strlen(str)) == 0) {
		*buf += strlen(str);
	} else {
		return(FAIL);
	}
	return(SUCCESS);
}

/*
 * Takes a pointer to a char buffer, and a string.  Returns
 * TRUE if the string appears immediately (after skipping whitespace).
 * or FALSE otherwise.
 */

int
xk_parpeek(
        char **buf,
        char *str )
{
	RIF(xk_skipwhite(buf));
	if (strncmp(*buf, str, strlen(str)) == 0)
		return(TRUE);
	else
		return(FALSE);
}

int
xk_prin_int(
        memtbl_t *tbl,
        char **buf,
        unsigned long *old_v )

{
	int i, printed = 0;
	struct symlist *sym;
	unsigned long v;

	switch (tbl->kind) {
	case K_CHAR:
		v = *((unsigned char *) old_v);
		break;
	case K_SHORT:
		v = *((unsigned short *) old_v);
		break;
	case K_INT:
		v = *((unsigned int *) old_v);
		break;
	default:
		v = *old_v;
	}
	**buf = '\0';
	if ((Pr_format & PRSYMBOLIC) && ((Val_list.syms != NULL) || ((sym = fsymbolic(tbl)) != NULL))) {
		if (Val_list.syms != NULL)
			sym = &Val_list;
		if (sym->isflag) {
			if (v == 0) {
				*buf += lsprintf(*buf, "0");
				return(SUCCESS);
			}
			for (i = 0; i < sym->nsyms; i++) {
				if (sym->syms[i].addr & v) {
					if (Pr_format & PRMIXED_SYMBOLIC) {
						if (Pr_format & PRDECIMAL)
							*buf += lsprintf(*buf, "%s%s(%d)", printed ? "|" : "", sym->syms[i].str, sym->syms[i].addr);
						else
							*buf += lsprintf(*buf, "%s%s(0x%x)", printed ? "|" : "", sym->syms[i].str, sym->syms[i].addr);
					}
					else
						*buf += lsprintf(*buf, "%s%s", printed ? "|" : "", sym->syms[i].str);
					v &= ~(sym->syms[i].addr);
					printed++;
				}
			}
			if (v) {
				if (Pr_format & PRMIXED_SYMBOLIC) {
					if (Pr_format & PRDECIMAL)
						*buf += lsprintf(*buf, "%sNOSYMBOLIC(%d)", printed ? "|" : "", v);
					else
						*buf += lsprintf(*buf, "%sNOSYMBOLIC(0x%x)", printed ? "|" : "", v);
				}
				else {
					if (Pr_format & PRDECIMAL)
						*buf += lsprintf(*buf, "%s%d", printed ? "|" : "", v);
					else
						*buf += lsprintf(*buf, "%s0x%x", printed ? "|" : "", v);
				}
			}
			return(SUCCESS);
		}
		else {
			for (i = 0; i < sym->nsyms; i++) {
				if (sym->syms[i].addr == v) {
					if (Pr_format & PRMIXED_SYMBOLIC) {
						if (Pr_format & PRDECIMAL)
							*buf += lsprintf(*buf, "%s(%d)", sym->syms[i].str, v);
						else
							*buf += lsprintf(*buf, "%s(0x%x)", sym->syms[i].str, v);
					}
					else
						*buf += lsprintf(*buf, "%s", sym->syms[i].str);
					return(SUCCESS);
				}
			}
		}
	}
	if (Pr_format & PRHEX)
		*buf += lsprintf(*buf, "0x%x", v);
	else if (Pr_format & PRDECIMAL)
		*buf += lsprintf(*buf, "%d", v);
	else
		*buf += lsprintf(*buf, "%d(0x%x)", v, v);
	return(SUCCESS);
}

int
xk_par_int(
        char **buf,
        long *v,
        struct envsymbols *env )
{
	int ret, base;
	char *p, *q, *pp;
	char nbuf[512];

	xk_skipwhite(buf);
	strncpy(nbuf, *buf, sizeof(nbuf)-1);
	if (strchr(nbuf, '|') == NULL) {
		for (p = nbuf; *p && *p != ' ' && *p != ',' && *p != ']'
		&& *p != '{' && *p != '}' && *p != '/' && *p != '@'
		&& *p != ':' && *p != '.' && *p != 13 && *p != 10 && *p != 11
		&& *p != 12 && *p != 9; p++)
			;
		*p = '\0';
	}
	else {
		for (p = nbuf; *p && *p != ','
		&& *p != '{' && *p != '}' && *p != '/' && *p != '@'
		&& *p != ':' && *p != '.' ; p++)
			;
		*p = '\0';

	}
	ret = strlen(nbuf);
	if (ret == 0)
		return(OBJ_END);
	*v = 0;

	if ((p = strchr(nbuf, '"')) != NULL) {
		return(FAIL);
	}
	if ((p = strchr(nbuf, '+')) != NULL) {
		char *qq;
		long v1, v2;

		*p = '\0';
		v1 = v2 = 0;
		qq = nbuf;
		p++;
		xk_par_int(&qq, &v1, env);
		xk_par_int(&p, &v2, env);
		*v = v1 + v2;
		*buf += ret;
		return(SUCCESS);
	}
	if ((p = strchr(&nbuf[1], '-')) != NULL) {
		long v1, v2;
		char *qq;

		*p = '\0';
		v1 = v2 = 0;
		qq = nbuf;
		p++;
		xk_par_int(&qq, &v1, env);
		xk_par_int(&p, &v2, env);
		*v = v1 - v2;
		*buf += ret;
		return(SUCCESS);
	}
	for (p = strtok(nbuf, " |\t\n"); p; p = strtok(NULL, " |\t\n")) {
		for (pp = p; *pp && *pp != ' ' && *pp != ','
		&& *pp != '{' && *pp != '}' && *pp != '/' && *pp != '@'
		&& *pp != ':' && *pp != '.' && *pp != 13 &&
		*pp != 11 && *pp != 12 && *pp != 9; pp++)
			;
		*pp = '\0';
		if ((pp = strchr(p, '#')) != NULL) {
			base = strtol(p, &p, 10);
			if (p != pp)
				return(FAIL);
			p++;
		}
		else
			base = 0;
		xk_skipwhite(&p);
		if (*p == '\0')
			continue;
		if (isdigit(*p) || *p == '-') {
			*v |= strtoul(p, (char **)NULL, base);
		}
		else {
			unsigned long val;

			/* knock out commentary parenthesized things */
			if ((q = strchr(p, '(' /*)*/ )) != NULL)
				*q = '\0';
			/* Search through available names for partial match */
			if (!fdef(p, &val)) {
				return(FAIL);
			}
			else
				*v |= val;
		}
	}
	*buf += ret;
	return(SUCCESS);
}


int
xk_prin_nts(
        char **buf,
        char *str )
{
	return(xk_prin_charstr(buf, (unsigned char *)str, 
               str ? strlen(str) : 0));
}

int
xk_prin_charstr(
        char **buf,
        unsigned char *str,
        int len )
{
	int i;

	if (str == NULL)
		*buf += lsprintf(*buf, "NULL");
	else {
		*buf += lsprintf(*buf, "\"");
		for (i = 0; i < len; i++) {
			if (str[i] == '"') {
				*buf += lsprintf(*buf, "\\\"");
			} else if (isprint(str[i])) {
				*buf += lsprintf(*buf, "%c", str[i]);
			} else {
				switch (str[i]) {
				case '\n':
					*buf += lsprintf(*buf, "\\n");
					break;
				case '\t':
					*buf += lsprintf(*buf, "\\t");
					break;
				case '\b':
					*buf += lsprintf(*buf, "\\b");
					break;
				case '\v':
					*buf += lsprintf(*buf, "\\v");
					break;
				case '\f':
					*buf += lsprintf(*buf, "\\f");
					break;
				case '\r':
					*buf += lsprintf(*buf, "\\r");
					break;
				case '\0':
					*buf += lsprintf(*buf, "\\00");
					break;
				default:
					*buf += lsprintf(*buf, "\\%x", (unsigned int)str[i]);
					break;
				}
			}
		}
		*buf += lsprintf(*buf, "\"");
	}
	return(SUCCESS);
}

int
xk_prin_hexstr(
        char **buf,
        char *str,
        int len )
{
	int i;
	unsigned char tempc;

	if (str == NULL)
		*buf += lsprintf(*buf, "NULL");
	else {
		*buf += lsprintf(*buf, "%s", "0x");
		for (i = 0; i < len; i++) {
			tempc = str[i];
			if (str[i] & 0xf0) {
				*buf += lsprintf(*buf, "%x", tempc);
			}
			else
				*buf += lsprintf(*buf, "0%x", tempc);
		}
	}
	return(SUCCESS);
}

#define MALSIZ 16	/* initial size of string to malloc */

int
xk_par_chararr(
        char **buf,
        char *str,
        int *len )
{
	return(xk_par_charstr(buf, &str, len));
}

#define CHAR_QUOTED	0
#define CHAR_HEXSTR	1
#define	CHAR_FILE	2

int
xk_par_nts(
        char **buf,
        char **str )
{
	int temp = 0;

	RIF(xk_par_charstr(buf, str, &temp));
	if (temp >= 0)
		str[0][temp] = '\0';
	return(SUCCESS);
}

int
xk_par_charstr(
        char **buf,
        char **str,
        int *len )
{
	int i;
	char delim;
	int didmalloc = FALSE, getmode;
	char cbuf[3];	/* conversion buffer for hex strings */
	char filename[128];
        char * errmsg;

	RIF(xk_skipwhite(buf));
	if (xk_parpeek(buf, "NULL")) {
		RIF(xk_parexpect(buf, "NULL"));
		*str = NULL;
		*len = -1;
		return(SUCCESS);
	}
	/* this is pure internal feature, no error setting */
	if (**buf == '<') {
		char *p;
		FILE *fp;
		char gbuf[BUFSIZ];
		int line;
		int size;

		(*buf)++;
		xk_skipwhite(buf);
		for (p = &filename[0];
		     **buf != ',' && **buf != /* { */ '}' && **buf != ' ' &&
			**buf != '\t' && p < &filename[sizeof(filename)];
			*p++ = *(*buf)++)
			;
		*p++ = '\0';
		if ((fp = fopen(filename, "r")) == NULL) {
                        errmsg=strdup(GETMESSAGE(7,1, 
                             "Unable to open the file '%s'; verify that it exists and is readable"));
			lsprintf(gbuf, errmsg, filename);
			fprintf(stderr, gbuf);
                        free(errmsg);
			return(FAIL);
		}
		if (*len == 0) {
			if (getsize(fileno(fp), &size) == FAIL) {
                                errmsg=strdup(GETMESSAGE(7,2, 
                                     "Unable to access the file '%s'; verify you have permission to access it"));
				lsprintf(gbuf, errmsg, filename);
				fprintf(stderr, gbuf);
                                free(errmsg);
				return(FAIL);
			}
			*len = size/2 + 1;
			if ((*str = malloc(*len)) == NULL) {
				return(FAIL);
			}
		}
		line = i = 0;
		while (fgets(gbuf, sizeof(gbuf), fp) != NULL) {
			line++;
			p = gbuf;
			/* eat any leading 0x */
			if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
				p += 2;
			}
			for ( ; *p && *p != '\n'; ) {
				if (i > *len - 1) {
					fclose(fp);
					return(FAIL);
				}
				if (!isxdigit(*p)) {
					p++;
					continue;
				}
				if (!isxdigit(p[1])) {
					fclose(fp);
					return(FAIL);
				}
				cbuf[0] = p[0];
				cbuf[1] = p[1];
				cbuf[2] = '\0';
				str[0][i++] = (char)strtol(cbuf, (char **)NULL, 16);
				p += 2;
				xk_skipwhite(&p);
			}
		}
		*len = i;
		fclose(fp);
		return(SUCCESS);
	} else if (!ispunct(**buf)) {
		getmode = CHAR_HEXSTR;
		if ((*buf)[0] == '0' && ((*buf)[1] == 'x' || (*buf)[1] == 'X'))
			(*buf) += 2;
	} else {
		delim = *((*buf)++);
		getmode = CHAR_QUOTED;
	}
	if (*len == 0) {
		if ((*str = malloc(MALSIZ)) == NULL) {
			return(FAIL);
		}
		didmalloc = TRUE;
		*len = MALSIZ;
	}
	i = 0;
	while ((*buf)[0] != '\0' && ((getmode == CHAR_QUOTED && (*buf)[0] != delim) ||
	       (getmode == CHAR_HEXSTR && (isxdigit((*buf)[0]))) ||
	       (getmode == CHAR_HEXSTR && (isspace((*buf)[0]))))) {
	       /* NOTE: must always leave 1 additional byte for a null
		* termination, because could be called by xk_par_nts!
		*/
		if (i >= *len - 1) {
			if (didmalloc == FALSE) {
				return(FAIL);
			} else {
				if ((*str = realloc(*str, *len + MALSIZ)) == NULL) {
					return(FAIL);
				}
				*len += MALSIZ;
			}
		}
		if (getmode == CHAR_QUOTED) {
			if ((*buf)[0] == '\\') {
				(*buf)++;
				switch ((*buf)[0]) {
				case 't':
					str[0][i++] = '\t';
					(*buf)++;
					break;
				case 'v':
					str[0][i++] = '\v';
					(*buf)++;
					break;
				case 'f':
					str[0][i++] = '\f';
					(*buf)++;
					break;
				case 'n':
					str[0][i++] = '\n';
					(*buf)++;
					break;
				case 'r':
					str[0][i++] = '\r';
					(*buf)++;
					break;
				case 'b':
					str[0][i++] = '\b';
					(*buf)++;
					break;
				case '0':
					str[0][i++] = (char)strtol(*buf, buf, 8);
					break;
				case 's':
					(*buf)++;
					break;
				default:
					str[0][i++] = *(*buf)++;
				}
			} else
				str[0][i++] = *(*buf)++;
		} else {
			if (!isxdigit((*buf)[1])) {
				return(FAIL);
			}
			cbuf[0] = (*buf)[0];
			cbuf[1] = (*buf)[1];
			cbuf[2] = '\0';
			str[0][i++] = (char)strtol(cbuf, (char **)NULL, 16);
			(*buf) += 2;
			xk_skipwhite(buf);
		}
	}
	if (getmode == CHAR_QUOTED)
		(*buf)++;	/* eat the trailing quote */
	/*
	 * NOTE: We leave a malloced buffer the same size rather
	 * than realloc()'ing it to be the exact size in order
	 * to save time and avoid malloc arena fragmentation
	 */
	*len = i;
	return(SUCCESS);
}

/* Case Ignoring String Functions. */

static int
xk_uppercase(
        char *s )
{
	while (*s) {
		if (islower(*s))
			*s = toupper(*s);
		s++;
	}
}

int
xk_Strncmp(
        char *s1,
        char *s2,
        int len )
{
	int diff, i;

	for (i=0; i < len && s1[i] != '\0' && s2[i] != '\0'; i++)
		if ((diff = tolower(s1[i]) - tolower(s2[i])) != 0)
			return (diff);
	return(i == len ? 0 : s1[i] - s2[i]);
}

static int
getsize(
        int fd,
        int *psize )
{
	struct stat stat;

	if (fstat(fd, &stat) == FAIL)
		return(FAIL);
	*psize = stat.st_size;
	return(SUCCESS);
}
