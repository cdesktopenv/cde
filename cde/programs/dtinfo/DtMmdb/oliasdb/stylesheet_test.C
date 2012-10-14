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
/* $XConsortium: stylesheet_test.cc /main/4 1996/07/18 16:03:49 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/stylesheet_test.h"

#define LARGE_BUFSIZ 2048

void print_stylesheet(stylesheet_smart_ptr& x, ostream& out)
{
   out << "name=" << x.name();
   out << "online_data_size=" << x.online_data_size();
   out << "online_data=" << x.online_data();
   out << "hardcopy_data_size=" << x.hardcopy_data_size();
   out << "hardcopy_data_size=" << x.hardcopy_data();
}

int compare_stylesheet(stylesheet_smart_ptr& pattern, info_base* base_ptr)
{
   char pattern_buf[LARGE_BUFSIZ];
   ostringstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_stylesheet(pattern, pattern_out);

   char loc[BUFSIZ];
   strcpy(loc, pattern.name());

   stylesheet_smart_ptr x( base_ptr, loc );
   char db_buf[LARGE_BUFSIZ];
   ostringstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
   print_stylesheet(x, db_out);

   return compare_stream(pattern_out, db_out);
}

void update_stylesheet_test(char* filename, info_base* base_ptr, char* locator)
{
   char buf[LBUFSIZ];
   char st_buf[LBUFSIZ];
   fstream in(filename, ios::in);
   while (in) {
      in.getline(buf, LBUFSIZ);
      strcat(st_buf, buf);
   }
   stylesheet_smart_ptr st(base_ptr, locator);
   st.update_online_data(st_buf, strlen(st_buf));
}

int stylesheet_test_loc( info_lib* infolib_ptr, const char* base_name, const char* loc )
{
   stylesheet_smart_ptr x( infolib_ptr, base_name, loc );
   cerr << form("info of stylesheet object with loc %s:\n", loc);
   print_stylesheet(x, cerr);
   return 0;
}

int stylesheet_test_oid( info_lib* infolib_ptr, const char* base_name, const char* oid_str)
{
   oid_t id((char*)oid_str, true, false);
   stylesheet_smart_ptr x( infolib_ptr, base_name, id );

   cerr << form("info of stylesheet object with id : ");
   debug(cerr, id);
   debug(cerr, x.name());
   debug(cerr, x.online_data_size());
   debug(cerr, x.online_data());
   debug(cerr, x.hardcopy_data_size());
   debug(cerr, x.hardcopy_data());

   x.its_oid().asciiOut(cerr); cerr << "\n";

   return 0;
}

void generate_stylesheet_instance(random_gen& x, ostream& out, int min_len, int max_len)
{
   out << "1011\n";
   out << "3\n";
   x.random_string(out, 15, 15); // name
   x.random_string(out, min_len, max_len); // sheet1
   x.random_string(out, min_len, max_len); // sheet2
}

#endif
