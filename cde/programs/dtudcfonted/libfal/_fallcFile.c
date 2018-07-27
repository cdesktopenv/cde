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
/* lcFile.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:36 	*/
/* $XConsortium: _fallcFile.c /main/1 1996/04/08 15:16:48 cde-fuj $ */
/*
 *
 * Copyright IBM Corporation 1993
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
#include <stdio.h>
#include <ctype.h>
#include "_fallibint.h"
#include "_fallcPubI.h"
#include <X11/Xos.h>

/************************************************************************/

#define	iscomment(ch)	((ch) == '#' || (ch) == '\0')
#define isreadable(f)	((access((f), R_OK) != -1) ? 1 : 0)
/*
#define	isspace(ch)	((ch) == ' ' || (ch) == '\t' || (ch) == '\n')
*/

static int
parse_line(char *line, char **argv, int argsize)
{
    int argc = 0;
    char *p = line;

    while(argc < argsize){
	while(isspace(*p)){
	    ++p;
	}
	if(*p == '\0'){
	    break;
	}
	argv[argc++] = p;
	while(! isspace(*p) && *p != '\0'){
	    ++p;
	}
	if(*p == '\0'){
	    break;
	}
	*p++ = '\0';
    }

    return argc;
}

#ifndef XLOCALEDIR
#define XLOCALEDIR "/usr/lib/X11/locale"
#endif

static void
xlocaledir(char *path)
{
    char *dir, *p = path;
    int len;

    dir = getenv("XLOCALEDIR");
    if(dir != NULL){
	len = strlen(dir);
	strcpy(p, dir);
	p[len++] = ':';
	p += len;
    }
    strcpy(p, XLOCALEDIR);
}

static int
parse_path(char *path, char **argv, int argsize)
{
    char *p = path;
    int i, n;

    while((p = strchr(p, ':')) != NULL){
	*p = ' ';	/* place space on delimter */
    }
    n = parse_line(path, argv, argsize);
    if(n == 0){
	return 0;
    }
    for(i = 0; i < n; ++i){
	int len;
	p = argv[i];
	len = strlen(p);
	if(p[len - 1] == '/'){
	    /* eliminate slash */
	    p[len - 1] = '\0';
	}
    }
    return n;
}

enum { LtoR, RtoL };

static char *
_XlcResolveName(char *lc_name, char *file_name, int direction)/*mapping direction*/
{
    FILE *fp;
    char buf[BUFSIZE], *name = NULL;

    fp = fopen(file_name, "r");
    if(fp == (FILE *)NULL){
	return NULL;
    }

    while(fgets(buf, BUFSIZE, fp) != NULL){
	char *p = buf;
	int n;
	char *args[2], *from, *to;
	while(isspace(*p)){
	    ++p;
	}
	if(iscomment(*p)){
	    continue;
	}
	n = parse_line(p, args, 2);		/* get first 2 fields */
	if(n != 2){
	    continue;
	}
	if(direction == LtoR){
	    from = args[0], to = args[1];	/* left to right */
	}else{
	    from = args[1], to = args[0];	/* right to left */
	}
	if(! strcmp(from, lc_name)){
	    name = Xmalloc(strlen(to) + 1);
	    if(name != NULL){
		strcpy(name, to);
	    }
	    break;
	}
    }
    if(fp != (FILE *)NULL){
	fclose(fp);
    }
    return name;
}

/*
#define	isupper(ch)	('A' <= (ch) && (ch) <= 'Z')
#define	tolower(ch)	((ch) - 'A' + 'a')
*/
static char *
lowercase(char *dst, char *src)
{
    char *s, *t;

    for(s = src, t = dst; *s; ++s, ++t){
	*t = isupper(*s) ? tolower(*s) : *s;
    }
    *t = '\0';
    return dst;
}

/************************************************************************/
char *
_fallcFileName(XLCd lcd, char *category)
{
    char lc_name[BUFSIZE];
    char cat[BUFSIZE], dir[BUFSIZE];
    int i, n;
    char *args[256];
    char *file_name = NULL;

    if(lcd == (XLCd)NULL){
	return NULL;
    }

    if(! _fallcResolveLocaleName(XLC_PUBLIC(lcd, siname), lc_name,
			       NULL, NULL, NULL)){
	return NULL;
    }

    lowercase(cat, category);
    xlocaledir(dir);
    n = parse_path(dir, args, 256);
    for(i = 0; i < n; ++i){
	char buf[BUFSIZE], *name;
	sprintf(buf, "%s/%s.dir", args[i], cat);
	name = _XlcResolveName(lc_name, buf, RtoL);
	if(name == NULL){
	    continue;
	}
	if(*name == '/'){
	    /* supposed to be absolute path name */
	    file_name = name;
	}else{
	    sprintf(buf, "%s/%s", args[i], name);
	    Xfree(name);
	    file_name = Xmalloc(strlen(buf) + 1);
	    if(file_name == NULL){
		break;
	    }
	    strcpy(file_name, buf);
	}
	if(isreadable(file_name)){
	    break;
	}
	Xfree(file_name);
	file_name = NULL;
	/* Then, try with next dir */
    }
    return file_name;
}

/************************************************************************/
#ifndef LOCALE_ALIAS
#define LOCALE_ALIAS    "locale.alias"
#endif

int
_fallcResolveLocaleName(
    char *lc_name,
    char *full_name,
    char *language,
    char *territory,
    char *codeset)
{
    char dir[BUFSIZE], buf[BUFSIZE], *name = NULL;
    int i, n;
    char *args[256];

    xlocaledir(dir);
    n = parse_path(dir, args, 256);
    for(i = 0; i < n; ++i){
	sprintf(buf, "%s/%s", args[i], LOCALE_ALIAS);
	name = _XlcResolveName(lc_name, buf, LtoR);
	if(name != NULL){
	    break;
	}
    }

    if(name != NULL){
	snprintf(buf, sizeof(buf), "%s", name);
	Xfree(name);
    }else{
	snprintf(buf, sizeof(buf), "%s", lc_name);
    }
    if(full_name != NULL){
	strcpy(full_name, buf);
    }

    if(language || territory || codeset){
	char *ptr, *name_p;
	/*
	 * Decompose locale name
	 */
	if(language) *language = '\0';
	if(territory) *territory = '\0';
	if(codeset) *codeset = '\0';

	name_p = buf;
	ptr = language;
	while (1) {
	    if (*name_p == '_') {
		if (ptr)
		    *ptr = '\0';
		ptr = territory;
	    } else if (*name_p == '.') {
		if (ptr)
		    *ptr = '\0';
		ptr = codeset;
	    } else {
		if (ptr)
		    *ptr++ = *name_p;
		if (*name_p == '\0')
		    break;
	    }
	    name_p++;
	}
    }

    return (buf[0] != '\0') ? 1 : 0;
}

/************************************************************************/
#ifndef	LOCALE_DIR
#define	LOCALE_DIR	"locale.dir"
#endif

int
_fallcResolveDBName(char *lc_name, char *file_name)
{
    char dir[BUFSIZE], buf[BUFSIZE], *name = NULL;
    int i, n;
    char *args[256];

    xlocaledir(dir);
    n = parse_path(dir, args, 256);
    for(i = 0; i < n; ++i){
	sprintf(buf, "%s/%s", args[i], LOCALE_DIR);
	name = _XlcResolveName(lc_name, buf, RtoL);
	if(name != NULL){
	    break;
	}
    }
    if(name == NULL){
	return 0;
    }
    strcpy(buf, name);
    Xfree(name);
    if(file_name != NULL){
	strcpy(file_name, buf);
    }
    return 1;
}

/************************************************************************/
int
_fallcResolveI18NPath(char *path_name)
{
    if(path_name != NULL){
	xlocaledir(path_name);
    }
    return 1;
}
