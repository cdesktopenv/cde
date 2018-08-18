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
/* SetLocale.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:31:29 	*/
/* $XConsortium: _falSetLocale.c /main/1 1996/04/08 15:15:08 cde-fuj $ */

/*
 * Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
 *                      and Nippon Telegraph and Telephone Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON, NTT Software, and NTT
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. OMRON, NTT Software,
 * and NTT make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OMRON, NTT SOFTWARE, AND NTT, DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OMRON, NTT SOFTWARE, OR NTT, BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Authors: Li Yuhong		OMRON Corporation
 *		 Tetsuya Kato		NTT Software Corporation
 *		 Hiroshi Kuribayashi	OMRON Corporation
 *
 */
/*

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#include "_fallibint.h"
#include "_fallcint.h"
#include <X11/Xlocale.h>
#include <X11/Xos.h>

#ifdef X_LOCALE

#define MAXLOCALE	64	/* buffer size of locale name */

char *
_falsetlocale(int category, const char *name)
{
    static char *xsl_name;
    char *old_name;
    XrmMethods methods;
    XPointer state;

    if (category != LC_CTYPE && category != LC_ALL)
	return NULL;
    if (!name) {
	if (xsl_name)
	    return xsl_name;
	return "C";
    }
    if (!*name)
	name = getenv("LC_CTYPE");
    if (!name || !*name)
	name = getenv("LANG");
    if (!name || !*name)
	name = "C";
    old_name = xsl_name;
    xsl_name = (char *)name;
    methods = _falrmInitParseInfo(&state);
    xsl_name = old_name;
    if (!methods)
	return NULL;
    name = (*methods->lcname)(state);
    xsl_name = Xmalloc(strlen(name) + 1);
    if (!xsl_name) {
	xsl_name = old_name;
	(*methods->destroy)(state);
	return NULL;
    }
    strcpy(xsl_name, name);
    if (old_name)
	Xfree(old_name);
    (*methods->destroy)(state);
    return xsl_name;
}

#else /* X_LOCALE */

/*
 * _fallcMapOSLocaleName is an implementation dependent routine that derives
 * the LC_CTYPE locale name as used in the sample implementation from that
 * returned by setlocale.
 * Should match the code in Xt ExtractLocaleName.
 */

char *
_fallcMapOSLocaleName(char *osname, char *siname)
{
#if defined(hpux) || defined(CSRG_BASED) || defined(sun) || defined(SVR4)

#ifdef hpux
#define SKIPCOUNT 2
#define STARTCHAR ':'
#define ENDCHAR ';'
#elif !defined(sun) || defined(SVR4)
#define STARTCHAR '/'
#define ENDCHAR '/'
#endif

    char           *start;
    char           *end;
    int             len;

    start = osname;
#ifdef SKIPCOUNT
    int = n;
    for (n = SKIPCOUNT;
	 --n >= 0 && start && (start = strchr (start, STARTCHAR));
	 start++)
	;
    if (!start)
	start = osname;
#endif
#ifdef STARTCHAR
    if (start && (start = strchr (start, STARTCHAR))) {
	start++;
#endif
	if (end = strchr (start, ENDCHAR)) {
	    len = end - start;
	    strncpy(siname, start, len);
	    *(siname + len) = '\0';
#ifdef WHITEFILL
	    for (start = siname; start = strchr(start, ' '); )
		*start++ = '-';
#endif
	    return siname;
#ifdef STARTCHAR
	}
#endif
    }
#ifdef WHITEFILL
    if (strchr(osname, ' ')) {
	strcpy(siname, osname);
	for (start = siname; start = strchr(start, ' '); )
	    *start++ = '-';
	return siname;
    }
#endif
#undef STARTCHAR
#undef ENDCHAR
#undef WHITEFILL
#endif
    return osname;
}

#endif  /* X_LOCALE */
