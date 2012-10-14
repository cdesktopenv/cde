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
/* $XConsortium: loc_test.C /main/5 1996/10/26 18:19:23 cde-hal $ */

#ifdef REGRESSION_TEST

#include "oliasdb/loc_test.h"

#define LARGE_BUFSIZ 2048


void print_loc(locator_smart_ptr& x, ostream& out)
{
   out << x.inside_node_locator_str();
   x.node_id().asciiOut(out); out << "\n";
}

int compare_locator(locator_smart_ptr& pattern, info_base* base_ptr)
{
   char pattern_buf[LARGE_BUFSIZ];
   ostringstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_loc(pattern, pattern_out);

   locator_smart_ptr x( base_ptr, pattern.inside_node_locator_str());
   char db_buf[LARGE_BUFSIZ];
   ostringstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
   print_loc(x, db_out);

   return compare_stream(pattern_out, db_out);
}

int loc_test( info_lib* infolib_ptr, const char* base_name, const char* loc )
{
   locator_smart_ptr x( infolib_ptr, base_name, loc );
   print_loc(x, cerr);
   return 0;
}

void generate_loc_instance(random_gen& x, ostream& out)
{
   out << "1002\n";
   out << "3\n";
   x.random_string(out, 15, 15); // locator
   x.random_oid(out, 1001, 0); // id
   x.random_string(out, 8, 255); // label
}

#endif
