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
/* $XConsortium: whitesp.c /main/3 1995/11/08 09:57:49 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include "basic.h"

LOGICAL m_whitespace(M_WCHAR c);

/* Test if character c is a blank, tab, or carriage return */
LOGICAL m_whitespace(M_WCHAR c)
{
char mb_c[32]; /* arbitrarily large */
int  length;

length = wctomb(mb_c, c);
if (length != 1) return FALSE;

if (*mb_c == ' ' || *mb_c == '\t' || *mb_c == '\n') return(TRUE) ;
else return(FALSE) ;
}
