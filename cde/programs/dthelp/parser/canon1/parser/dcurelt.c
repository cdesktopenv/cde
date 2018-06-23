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
/* $XConsortium: dcurelt.c /main/3 1995/11/08 09:35:06 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Displays current element after some error messages */
void m_dispcurelt(M_WCHAR *file, int line)
{
char *mb_parent;

m_errline("Current element is ") ;
mb_parent = MakeMByteString(m_parent(0));
m_errline(mb_parent) ;
m_free(mb_parent,"multi-byte string");
m_errline(" begun on ") ;
m_dumpline(file, line) ;
m_errline(".\n") ;
}
