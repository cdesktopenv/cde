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
/*
 * File:	stringio.C $XConsortium: stringio.C /main/5 1996/06/21 17:36:19 ageorge $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
#include <codelibs/nl_hack.h>
#include "stringio.h"

int
_StringIO::doit(int commit)
{
    wchar_t ch;
    int cu = curr;
    char *ccp;
    wchar_t __nlh_char[1];

    for (; cu >= 0; cu--)
    {
	ccp = ptr[cu];
	ch = ccp ? CHARAT(ccp) : '\0';
	if (ch != '\0')
	    break;
    }
    if (commit)
    {
	old_curr = curr;
	if ((curr = cu) >= 0)
	{
	    old_ccp = (const char *)ccp;
	    ADVANCE(ccp);
	    ptr[curr] = ccp;
	}
    }
    return ch;
}
