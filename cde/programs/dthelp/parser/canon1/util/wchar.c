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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: wchar.c /main/3 1995/11/08 09:57:35 rswiston $ */
/*
	       Copyright 1992 Hewlett-Packard Co.
*/

#include "basic.h"
#include "trie.h"
#include "cont.h"


/* compare two wide character strings */
#if defined(M_PROTO)
int w_strcmp(const M_WCHAR *string1, const M_WCHAR *string2)
#else
int w_strcmp(string1, string2)
M_WCHAR *string1;
M_WCHAR *string2;
#endif
{
M_WCHAR c1, c2, null;

if (string1 == string2) return 0;

null = (M_WCHAR) 0;
if (!string1) string1 = &null;
if (!string2) string2 = &null;

while (1)
    {
    c1 = *string1++;
    c2 = *string2++;
    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
    if (!c1) return 0;
    }
}


/* copy a wide character string */
#if defined(M_PROTO)
M_WCHAR *w_strcpy(M_WCHAR *string1, const M_WCHAR *string2)
#else
M_WCHAR *w_strcpy(string1, string2)
M_WCHAR *string1;
M_WCHAR *string2;
#endif
{
M_WCHAR *string1start;

string1start = string1;

while (*string1++ = *string2++);

return string1start;
}


/* copy a wide character string, stopping after "max" moves */
#if defined(M_PROTO)
M_WCHAR *w_strncpy(M_WCHAR *string1, const M_WCHAR *string2, int max)
#else
M_WCHAR *w_strncpy(string1, string2, max)
M_WCHAR *string1;
M_WCHAR *string2;
int      max;
#endif
{
M_WCHAR *string1start;

string1start = string1;

while ((--max >= 0) && (*string1++ = *string2++));

return string1start;
}


/* get the length of a wide character string */
#if defined(M_PROTO)
int w_strlen(const M_WCHAR *string)
#else
int w_strlen(string)
M_WCHAR *string;
#endif
{
int length;

length = 0;
if (string)
    while (*string++) length++;

return length;
}


/* find wide character "chr" in wide string "string" */
#if defined(M_PROTO)
M_WCHAR *w_strchr(M_WCHAR *string, const M_WCHAR chr)
#else
M_WCHAR *w_strchr(string, chr)
M_WCHAR *string, chr;
#endif
{
M_WCHAR *where;

where = string;

while (*where)
    {
    if (*where == chr) return where;
    where++;
    }

if (!chr)
    return where;
else
    return NULL;
}


/* find wide character "chr" in wide string "string" */
#if defined(M_PROTO)
M_WCHAR *w_strstr(M_WCHAR *string1, M_WCHAR *string2)
#else
M_WCHAR *w_strstr(string1, string2)
M_WCHAR *string1, *string2;
#endif
{
M_WCHAR *where1, *where2;

if (!*string2) return string1;

where1 = string1;
where2 = string2;

while (*where1)
    {
    if (*where1 == *where2)
	{
	string1 = where1;

	while (*where1 && *where2 && (*where1 == *where2))
	    {
	    where1++;
	    where2++;
	    }
	
	if (!*where2) return string1;
	if (!*where1) return NULL;

	where1 = string1;
	where2 = string2;
	}
    where1++;
    }

return NULL;
}


/* make a multi-byte string from a wide character string */
#if defined(M_PROTO)
char *MakeMByteString(const M_WCHAR *wc_string)
#else
char *MakeMByteString(wc_string)
M_WCHAR *wc_string;
#endif
{
char   *mb_string;
int     length, retVal;
M_WCHAR wc;
static M_WCHAR empty[] = {0};

if (!wc_string)
    wc_string = empty;

/* Do a little work to compensate for m_malloc allocating sizeof(M_WCHAR)
 * bytes for each increment in the size requested.
*/
length = (w_strlen(wc_string) * MB_CUR_MAX) + 1;
length = (length + sizeof(M_WCHAR) - 1) / sizeof(M_WCHAR);
mb_string = (char *) m_malloc(length, "multi-byte string");

length = 0;
while (wc = *wc_string++)
    {
    if ((retVal = wctomb(&mb_string[length], wc)) > 0)
	length += retVal;
    }
mb_string[length] = 0;

return mb_string;
}


/* make a wide character string from a multi-byte string */
#if defined(M_PROTO)
M_WCHAR *MakeWideCharString(const char *mb_string)
#else
M_WCHAR *MakeWideCharString(mb_string)
char *mb_string;
#endif
{
M_WCHAR *wc_string, *wc_stringStart;
int      length, incr;
char     c;

if (!mb_string)
    mb_string = "";

length = strlen(mb_string);
wc_stringStart = wc_string =
    (M_WCHAR *) m_malloc(length + 1, "wide character string");

length = 0;
while (mb_string[length])
    {
    if ((incr = mbtowc(wc_string, &mb_string[length], MB_CUR_MAX)) < 0)
	{
	char badOne[2], buffer[32];

	badOne[0] = mb_string[length];
	badOne[1] = 0;
	sprintf(buffer, "0x%x", mb_string[length]);
	m_err2("invalid multibyte character found: '%c' (%s)", badOne, buffer);
	incr = 1;
	}
    length += incr;
    wc_string++;
    }
*wc_string = (M_WCHAR) 0;

return wc_stringStart;
}


/* Get-wide-char procedure */
int mb_getwc(m_ptr)
void *m_ptr;
{
int  c;
M_WCHAR wc;
char badch[2];
char mbyte[32]; /* make this bigger than any possible multi-byte char */
int  length;

length = 0;
if ((c = getc((FILE *) m_ptr)) == EOF) return(EOF);

while (1)
    {
    mbyte[length++] = c;
    mbyte[length]   = 0;
    if (mblen(mbyte,length) != -1) break; /* hurray! */
    if (length == MB_CUR_MAX)
	{ /* reached max without a hit */
	m_error("An invalid multi-byte character was found in the input");
	c = ' ';
	length = 1;
	break;
	}
    if ((c = getc((FILE *) m_ptr)) == EOF)
	{ /* huh? */
	m_error("End-of-file found in within a multi-byte character");
	return(EOF);
	}
    }
mbtowc(&wc,mbyte,length);

return((int) wc);
}
