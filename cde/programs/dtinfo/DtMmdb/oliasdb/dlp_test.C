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
/* $XConsortium: dlp_test.cc /main/4 1996/07/18 16:02:00 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/dlp_test.h"

int dlp_test( info_lib* infolib_ptr, const char* base_name, const char* oid_str )
{
   oid_t id((char*)oid_str, true, false);

   dlp_smart_ptr* y = new dlp_smart_ptr( infolib_ptr, base_name, id );

   dlp_smart_ptr* z;
   cerr << form("run backward from %s:\n", oid_str);

   do {
      y -> node_id().asciiOut(cerr); cerr <<  "\n";
      z = y->prev();
      delete y;
      y = z;
   } while ( y );


   cerr << form("run forward from %s:\n", oid_str);

   y = new dlp_smart_ptr( infolib_ptr, base_name, id );
  
   do {
      y -> node_id().asciiOut(cerr); cerr <<  "\n";
      z = y->prev();
      delete y;
      y = z;
   } while ( z );


   return 0;
}
#endif
