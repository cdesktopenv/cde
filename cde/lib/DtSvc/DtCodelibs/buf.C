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
/*
 * File:	buf.C $TOG: buf.C /main/5 1998/01/21 18:01:14 mgreess $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <codelibs/nl_hack.h>
#include "buf.h"

void
_SHXbuf::start_token()
{
    vec[vec.size()] = (char *)buf.size();
    is_pattern = FALSE;
    _new_token = FALSE;
}

void
_SHXbuf::quote(Quote q)
{
    _quote ^= q;
    if (_new_token)
	start_token();
}

void
_SHXbuf::append(int const ch, char flag)
{
    flag |= _quote;

    if (ch == '\0' && !_new_token)
	_new_token = TRUE;
    else if (_new_token)
	start_token();

    if (flag == NOQUOTE)
	if (ch == '*' || ch == '?' || ch == '[')
	    if (glob)
		is_pattern = TRUE;
	    else
		flag |= SINGLEQUOTE;

    if (ch <= 0xFF)
    {
	// 8-bit char
	flags[buf.size()] = flag;
	buf[buf.size()] = (unsigned char)ch;
    }
    else
    {
	// multibyte char
	long sz = buf.size();

	for (int i=0; i<MB_CUR_MAX; i++) flags[sz+i] = flag;
	buf.reset(sz + MB_CUR_MAX);
	char *cp = &buf.elt(sz);
	WCHAR(ch, cp);
    }

    // expand token into filename(s) if appropriate
    if (ch == '\0' && (is_pattern || completion))
	if (!(flag & EXPANDQUOTE))
	    filegen();
}

void
_SHXbuf::append(char const *cp, char flag)
{
    wchar_t __nlh_char[1];

    do
	append((int)CHARAT(cp), flag);
    while (CHARADV(cp) != '\0');
}

void
_SHXbuf::reset(boolean g, boolean comp)
{
    glob = (boolean) !!g;
    completion = (boolean ) !!comp;
    is_pattern = FALSE;
    _new_token = TRUE;
    _quote = NOQUOTE;
    buf.reset(0);
    flags.reset(0);
    vec.reset(0);
}

// Convert the subscripts that are stored in vec[] into
// char pointers.  The only legal operation on _SHXbuf after
// calling vector is to call reset.
char **
_SHXbuf::vector()
{
    int i;
    for (i = 0; i < vec.size(); i++)
	vec[i] = &buf[long(vec[i])];

    vec[i] = NULL;

    return vec.getarr();
}
