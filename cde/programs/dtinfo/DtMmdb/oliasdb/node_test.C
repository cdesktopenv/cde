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
/* $XConsortium: node_test.cc /main/4 1996/07/18 16:03:28 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/node_test.h"

#define LARGE_BUFSIZ 2048

void print_node(node_smart_ptr& x, ostream& out, Boolean get_data, Boolean get_doc_id)
{
   out << "locator=" << x.locator() << "\n";
   out << "long_title=" << x.long_title() << "\n";
   out << "short_title=" << x.short_title() << "\n";

   if ( get_data == true ) {
      out << "data_size=" << x.data_size() << "\n";
      out << "data=" << x.data() << "\n";
   }

   out << "toc_node_loc=" << x.toc_node_loc() << "\n";

   if ( get_data == true ) {
      out << "doc_id=" << x.doc_id() << "\n";
   }

   out << "stylesheet_id=" << x.stylesheet_id() << "\n";
}

int compare_node(node_smart_ptr& pattern, info_base* base_ptr)
{
   char pattern_buf[LARGE_BUFSIZ];
   ostringstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_node(pattern, pattern_out, false, false);

   char loc[BUFSIZ];
   int len = MIN(strlen(pattern.locator()), BUFSIZ - 1);
   *((char *) memcpy(loc, pattern.locator(), len) + len) = '\0';

   node_smart_ptr x( base_ptr, loc );
   char db_buf[LARGE_BUFSIZ];
   ostringstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
   print_node(x, db_out, false, false);

   return compare_stream(pattern_out, db_out);
}


int compare_SGML_content(istream& in, info_base* base_ptr, Boolean doingTest)
{
   int ok = 0;

   int loc_size;
   char loc[BUFSIZ];
   in >> loc_size; in.get(); 

   for ( int i=0; i<loc_size; i++ )
      loc[i] = (char)in.get();

   loc[i] = 0;
/*
MESSAGE(cerr, "compare_SGML_content():");
debug(cerr, loc);
debug(cerr, loc_size);
*/

   in.get();

   char* db_buf;
   int data_size ;
   ostringstream* db_out;

   if ( doingTest ) {
      node_smart_ptr x( base_ptr, loc );
      data_size = x.data_size();
   
      db_buf = new char[data_size+1];
      db_out = new ostringstream(db_buf, data_size+1, ios::out);
   
      const char* z = x.data();
      for ( int i=0; i<data_size; i++ )
         db_out -> put(z[i]);
   }

   in >> data_size; in.get(); 

   char* pattern_buf = new char[data_size+1];
   ostringstream pattern_out(pattern_buf, data_size+1, ios::out);

   for ( i=0; i<data_size; i++ )
      pattern_out.put((char)in.get());

   in.get();

   if ( doingTest ) {
      ok = compare_stream(pattern_out, *db_out);
      delete db_out;
      delete db_buf;
   }

   delete pattern_buf;

   return ok;
}

int node_test_loc( info_lib* infolib_ptr, const char* base_name, const char* loc )
{
   node_smart_ptr x( infolib_ptr, base_name, loc );

   cerr << form("info of node with loc %s:\n", loc);
   x.its_oid().asciiOut(cerr); cerr << "\n";
   print_node(x, cerr, true, true);

   return 0;
}

int node_test_oid( info_lib* infolib_ptr, const char* base_name, const char* oid_str)
{
   oid_t id((char*)oid_str, true, false);
   node_smart_ptr x( infolib_ptr, base_name, id );

   cerr << form("info of node with oid string %s:\n", oid_str);
   print_node(x, cerr, true, true);

   return 0;
}

/*
1001
7
6
15      y3THnmB0oYn87oL
6
19      Component Test Book
6
19      Component Test Book
11
503.16387
0
6
15      y3THnmB0oYn87oL
7
0.0
7
1011.212994
*/

void generate_node_instance(info_base* b_ptr, random_gen& x, ostream& out, 
	ostream& mixed_out, int min_len, int max_len)
{
   out << "1001\n";
   out << "7\n";

   const char* loc = x.get_random_string(15, 15); // locator

   out << "6\n15\t" << loc << "\n";

   x.random_string(out, 10, 50); // short title
   x.random_string(out, 10, 100); // long title

//fprintf(stderr, "%d\n", (void*)(b_ptr -> get_base_name()));

   handler * y = (b_ptr -> get_obj_dict()).get_handler(
	form("%s.%s", b_ptr -> get_base_name(), "sgml.dict")
						      );

   x.random_string_to_be_compressed(out, 0, 0, y -> its_oid()); // content
   x.random_string(out, 15, 15); // locator
   x.random_oid(out); 
   x.random_oid(out); 

   mixed_out << "1010\n";
   mixed_out << "15\t" << loc << "\n"; // locator

   x.random_string(mixed_out, min_len, max_len, false); // content
}

/*
void generate_sgml_instance(random_gen& x, ostream& out, int min_len, int max_len)
{
   out << "1010\n";
   x.random_string(out, 15, 15); // locator
   x.random_string(out, min_len, max_len); // content
}
*/
#endif
