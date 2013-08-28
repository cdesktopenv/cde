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
/* $XConsortium: graphic_test.cc /main/4 1996/07/18 16:02:34 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/graphic_test.h"

#define LARGE_BUFSIZ 2048


void print_graphic(graphic_smart_ptr& x, ostream& out, Boolean out_misc)
{
   out << "locator=" << x.locator() << "\n";
   out << "filename=" << x.file_name() << "\n";
   out << "version=" << x.version() << "\n";

   const char* title = x.title();
   if ( title )
      out << "title=" << x.title() << "\n";
   else
      out << "no title" << "\n";

   out << "data_size=" << x.data_size() << "\n";

   int l = x.data_size();
   const char* str = x.data();

   for ( int i=0; i<l; i++ )
      out << str[i];

   out << "\n";

   if ( out_misc == true ) {
      out << "type=" << x.type() << "\n";
      out << "coding=" << x.coding() << "\n";
      out << "width=" << x.width() << "\n";
      out << "height=" << x.height() << "\n";

      out << "llx=" << x.llx() << "\n";
      out << "lly=" << x.lly() << "\n";
      out << "urx=" << x.urx() << "\n";
      out << "ury=" << x.ury() << "\n";
   }
}

int compare_graphic(graphic_smart_ptr& pattern, info_base* base_ptr)
{
   char pattern_buf[LARGE_BUFSIZ];
   ostringstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_graphic(pattern, pattern_out, false);

   char loc[BUFSIZ];
   int len = MIN(strlen(pattern.locator()), BUFSIZ -1);
   *((char *) memcpy(loc, pattern.locator(), len) + len) = '\0';

   graphic_smart_ptr x( base_ptr, loc );
   char db_buf[LARGE_BUFSIZ];
   ostringstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
   print_graphic(x, db_out, false);

   return compare_stream(pattern_out, db_out);
}

int graphic_test(info_lib* infolib_ptr, const char* base_name, const char* node_id_str )
{
   graphic_smart_ptr x( infolib_ptr, base_name, node_id_str );
   cerr << form("info. about graphic %s:\n", node_id_str);
   print_graphic(x, cerr, true);
   return 0;
}

void generate_graphic_instance(info_base* b_ptr, random_gen& x, ostream& out, int min_len, int max_len)
{

   out << "1009\n";
   out << "6\n";
   x.random_string(out, 15, 15); // locator
   x.random_string(out, 0, 0);   // 
   x.random_string(out, 2, 2);   // version
   x.random_string(out, 10, 30);   // misc info

   handler * y = (b_ptr -> get_obj_dict()).get_handler(
        form("%s.%s", b_ptr -> get_base_name(), "sgml.dict")
                                                      );
   x.random_string_to_be_compressed(out, min_len, max_len, y -> its_oid()); // graphic
   x.random_string(out, 10, 30);   // title
}

#endif
