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
/* $XConsortium: ifdata.c /main/3 1995/11/08 10:19:44 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* Ifdata.c contains functions used by the interface to access the
   single item of interface-determined data stored on the parse stack. */

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Retrieve the interface data stored with the current element's nth parent */
void *m_getdata(n, flag)
  int n ;
  LOGICAL *flag ;
  {
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && n >= 0 ;
         stackptr = stackptr->oldtop, n--) {
      if (! n) {
        *flag = TRUE ;
        return(stackptr->ifdata) ;
        }
      }
    *flag = FALSE ;
    return(NULL) ;
    }

/* Store interface data for the current element's nth parent */
LOGICAL m_putdata(data, n)
  void *data ;
  int n ;
  {
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && n >= 0 ;
         stackptr = stackptr->oldtop, n--) {
      if (! n) {
        stackptr->ifdata = data ;
        return(TRUE) ;
        }
      }
    return(FALSE) ;
    }

