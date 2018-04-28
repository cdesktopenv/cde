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
/* $XConsortium: dmpline.c /main/3 1995/11/08 10:50:31 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Write input file and line number for an error message */
void m_dumpline(file, line)
M_WCHAR *file ;
int line ;
{
char buffer[10] ;

m_errline("Line ") ;
sprintf(buffer, "%d", line) ;
m_errline(buffer) ;
if (file)
    {
    char mb_file;

    m_errline(" of File ") ;
    mb_file = MakeMByteString(file);
    m_errline(mb_file) ;
    m_free(mb_file,"multi-byte string");
    }
}
