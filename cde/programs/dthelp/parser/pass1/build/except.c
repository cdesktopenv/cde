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
/* $XConsortium: except.c /main/3 1995/11/08 10:01:32 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Except.c contains procedures related to exceptions for program BUILD */

#include "build.h"

/* Add an exception (inclusion or exclusion) for the current element */
void addex(void)
  {
    excount++ ;
    *nextex = (EXCEPTION *) m_malloc(sizeof(EXCEPTION), "exception") ;
    if (exlist) exlist->next = *nextex ;
    exlist = *nextex ;
    (*nextex)->next = NULL ;
    (*nextex)->nextptr = NULL ;
    (*nextex)->element = ntrelt(name)->eltno ;
    nextex = &(*nextex)->nextptr ;
    }
