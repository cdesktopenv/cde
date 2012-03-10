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
/* $XConsortium: inctest.c /main/3 1995/11/08 11:40:05 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"

void m_err1(
#if defined(M_PROTO)
  const char *text, const char *arg
#endif
  ) ;

void m_exit(
#if defined(M_PROTO)
  int status
#endif
  ) ;

void m_inctest(
#if defined(M_PROTO)
  int *count, int limit, char *message
#endif
  ) ;

/* Increment a count and test against a limit */
void m_inctest(count, limit, message)
  int *count ;
  int limit ;
  char *message ;
  {
    if (++*count <= limit) return ;
    m_err1("Internal error: %s exceeded", message) ;
    m_exit(TRUE) ;
    }
