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
/* $XConsortium: c_api_pure_virtual.cc /main/5 1996/07/18 16:01:43 drk $ */

#include <stdio.h>
#include <stdlib.h>


extern "C" {

#if defined(hpux)
char __pure_virtual_called()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
   return 0;
}
#else
#if defined(SVR4) && defined(SC3)
void _pure_error_()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
}
#else
#ifdef _AIX
void __PureVirtualCalled()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
}
#else

void __pure_virtual_called()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
}

#endif
#endif
#endif

}
