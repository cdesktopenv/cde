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
/* $XConsortium: scan.c /main/3 1995/11/08 10:05:59 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Scan.c contains scanner procedures for program ELTDEF */

#include <stdio.h>
#include <string.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"

#include "dtdext.h"

#include "eltdef.h"
#define M_CONDEF
#include "context.h"
#define M_DELIMDEF
#include "delim.h"

/* Reads a name token */
void getname(first)
int first;
{
M_WCHAR *p, wus;
int c, cttype;
LOGICAL cname;

int ret = mbtowc(&wus, "_", 1);
(void) ret;

cname = (LOGICAL) (curcon == INPARAM || curcon == INVALUE);
*(p = name) = (M_WCHAR) first;
if (! cname) *p = m_ctupper(*p);
while (TRUE)
    {
    c = getachar();
    if (c == EOF) break;
    *++p = (M_WCHAR) c;
    cttype = m_cttype(*p);
    if (! cname)
	{
	if (cttype == M_NONNAME) break;
	}
    else
	{
	if ((cttype != M_NMSTART) && (cttype != M_DIGIT) && (*p != wus))
	    break;
	}
    if (p >= name + M_NAMELEN)
	{
	*(name + M_NAMELEN) = M_EOS;
	m_error("Element name too long");
	}
    if (! cname) *p = m_ctupper(*p);
    }
ungetachar(c);
*p = M_EOS;
}

/* Reads the next token and returns it to the main procedure */
int scan(M_NOPAR)
{
int c;
int n;
static char unexp[] = "c";
M_WCHAR wus;

int ret = mbtowc(&wus, "_", 1);
(void) ret;

while (TRUE)
    {
    while ((n = gettoken(&c, COMCON)) == STARTCOMMENT)
	while ((n = gettoken(&c, INCOM)) != ENDCOMMENT)
	    if (c == EOF)
		{
		m_error("EOF occurred within comment");
		done();
		exit(TRUE);
		}
    ungetachar(c);
    n = gettoken(&c, curcon);
    if (n)
	{
	if (n != LIT && n != LITA) return(n);
	if (litproc(n))
	    {
	    if (scantrace)
		{
		char *mbyte;

		mbyte = MakeMByteString(literal);
		printf("literal '%s'\n", mbyte);
		m_free(mbyte, "multi-byte string");
		}
	    return(LITERAL);
	    }
	}
    if (c == EOF) return(ENDFILE);
    if (m_newcon(curcon - 1, TEXT - 1))
	{
	textchar = (M_WCHAR) c;
	return(TEXT);
	}
    if (m_whitespace((M_WCHAR) c)) continue;
    if (m_newcon(curcon - 1, NAME - 1))
	{
	if (curcon == INPARAM || curcon == INVALUE)
	    {
	    /* Check for C identifier */
	    if (m_letter((M_WCHAR) c) || c == wus)
		{
		getname(c);
		return(NAME);
		}
	    }
	else
	    {
	    /* Check for SGML name */
	    if ((m_cttype(c) == M_NMSTART) ||
	        (m_cttype(c) != M_NONNAME && curcon == VALUE))
		{
		getname(c);
		return(NAME);
		}
	    }
	}
    if (curcon != ERROR)
	{
	m_mberr1("Unexpected character: '%s'", unexp);
	}
    } /* End while */
}   /* End scan */

#include "scanutil.c"

#if defined(sparse)
#include "sparse.c"
#endif
