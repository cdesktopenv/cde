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
 * $XConsortium: strtokx.C /main/4 1996/04/21 19:09:46 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <codelibs/nl_hack.h>
#include <codelibs/mbstring.h>
#include <codelibs/stringx.h>

char *
strtokx(char *&ptr, const char *sep)
{
    if (ptr == NULL)
	return NULL;

    // find the beginning of the token
    char *ret = ptr;
    while (*ret != '\0' && _mb_schr(sep, *ret) != NULL)
	ADVANCE(ret);

    // find the end of the token
    char *end = ret;
    while (*end != '\0' && _mb_schr(sep, *end) == NULL)
	ADVANCE(end);

    ptr = end;

    // If this isn't the last token, advance pointer and terminate
    // current token.
    if (*end != '\0')
    {
	ADVANCE(ptr);
	WCHAR('\0', end);
    }

    if (*ret == '\0')
	return NULL;

    return ret;
}
