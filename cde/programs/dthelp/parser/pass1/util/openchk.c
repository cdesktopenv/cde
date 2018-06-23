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
/* $XConsortium: openchk.c /main/3 1995/11/08 10:34:41 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"

extern FILE *m_errfile ;

void m_exit(int status);

void m_openchk(FILE **ptr, char *name, char *mode);

/* Open a file and check that the open succeeded */
void m_openchk(FILE **ptr, char *name, char *mode)
  {
    *ptr = fopen(name, mode) ;
    if (*ptr) return ;
    fprintf(stderr, "Unable to open %s\n", name) ;
    if (m_errfile) fprintf(m_errfile, "Unable to open %s\n", name) ;
    m_exit(TRUE) ;
    }
