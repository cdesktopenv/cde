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
/* $XConsortium: strerror.c /main/3 1996/06/19 17:18:42 drk $ */
/* strerror.c -
   ANSI C strerror() function.

      Written by James Clark (jjc@jclark.com).
*/

#include "config.h"

#ifdef STRERROR_MISSING
#include <stdio.h>

char *strerror(n)
int n;
{
     extern int sys_nerr;
     extern char *sys_errlist[];
     static char buf[sizeof("Error ") + 1 + 3*sizeof(int)];

     if (n >= 0 && n < sys_nerr && sys_errlist[n] != 0)
	  return sys_errlist[n];
     else {
	  sprintf(buf, "Error %d", n);
	  return buf;
     }
}

#endif /* STRERROR_MISSING */
/*
Local Variables:
c-indent-level: 5
c-continued-statement-offset: 5
c-brace-offset: -5
c-argdecl-indent: 0
c-label-offset: -5
End:
*/
