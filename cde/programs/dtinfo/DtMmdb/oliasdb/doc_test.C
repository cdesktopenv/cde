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
/* $XConsortium: doc_test.cc /main/4 1996/07/18 16:02:13 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/doc_test.h"

#define LARGE_BUFSIZ 2048


void print_doc(doc_smart_ptr& x, ostream& out, Boolean out_it_oid, Boolean out_tag_list)
{
   if ( out_it_oid == true )
     out << "its_oid=" << x.its_oid();

   out << "short_title=" << x.short_title();
   out << "long_title=" << x.long_title();
   out << "seq_num=" << x.seq_num();


   if ( x.license_terms()) {
     const char* w = x.license_terms();
     int u = x.license_terms_size();
     for (int i=0; i<u; i++ )
        out << w[i];
     out << "\n";
   }

   out << "license_terms_size=" <<  x.license_terms_size();

   if ( out_tag_list == true ) {
      cerr << "tag list of the doc";
      short_list_handler* z = x.tab_list();
      (*z) -> asciiOut(cerr); cerr << "\n";
      delete z;
   }
}

int compare_doc(doc_smart_ptr& pattern, info_base* base_ptr)
{
   char pattern_buf[LARGE_BUFSIZ];
   ostringstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_doc(pattern, pattern_out, false, false);

   doc_smart_ptr x( base_ptr, pattern.seq_num());
   char db_buf[LARGE_BUFSIZ];
   ostringstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
   print_doc(x, db_out, false, false);

   return compare_stream(pattern_out, db_out);
}

int doc_test_oid( info_lib* infolib_ptr, const char* base_name, const char* oid_str )
{
   oid_t id((char*)oid_str, true, false);
   doc_smart_ptr x( infolib_ptr, base_name, id);
   print_doc(x, cerr, true, true);

   return 0;
}

int doc_test_doc_oid( info_lib* infolib_ptr, const char* base_name, const char* oid_str )
{
   oid_t id((char*)oid_str, true, false);
   doc_smart_ptr x(id, infolib_ptr, base_name);
   print_doc(x, cerr, true, true);

   return 0;
}

int 
doc_test_int(info_lib* infolib_ptr, 
             const char* base_name, 
             const char* seq_num_str)
{

   doc_smart_ptr x( infolib_ptr, base_name, atoi(seq_num_str));
   print_doc(x, cerr, true, true);

   return 0;
}

void generate_doc_instance(random_gen& x, ostream& out, int seq_num, int min_tabs, int max_tabs)
{
   out << "1000\n";
   out << "6\n";
   x.random_oid(out, 1001, 0); // id
   x.random_string(out, 10, 20); // title
   x.random_string(out, 10, 40); // title
   x.random_integer(out, seq_num); // seq number

   out << "105\n";
   out << "#\n";

   int tabs = x.pick_a_value(min_tabs, max_tabs);

   for ( int i=0; i<tabs; i++ )
      x.random_string(out, 10, 40); // tab node locator
     
   out << "#\n";

   x.random_string(out, 10, 100); // license term
}

#endif
