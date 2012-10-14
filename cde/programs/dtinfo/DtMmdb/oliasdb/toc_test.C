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
/* $XConsortium: toc_test.cc /main/4 1996/07/18 16:04:09 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/toc_test.h"

#define LARGE_BUFSIZ 2048


void print_toc(toc_smart_ptr& x, ostream& out)
{
   x.toc_node_oid().asciiOut(out); out << "\n";
   x.parent().asciiOut(out); out << "\n";

   out << "children of the toc: ";
   oid_list_handler* y = x.children();
   (*y) -> asciiOut(out); out << "\n";

   delete y;

   out << "subtree node count: ";
   out << x.subtree_size() << "\n";
}

int compare_toc(toc_smart_ptr& pattern, info_base* base_ptr)
{
   char pattern_buf[LARGE_BUFSIZ];
   ostringstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_toc(pattern, pattern_out);

   toc_smart_ptr x(base_ptr, pattern.toc_node_oid());
   char db_buf[LARGE_BUFSIZ];
   ostringstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
   print_toc(x, db_out);

   return compare_stream(pattern_out, db_out);
}

int 
toc_test(info_lib* infolib_ptr, const char* base_name, const char* node_id_str )
{
   oid_t id((char*)node_id_str, true, false);
   toc_smart_ptr x( infolib_ptr, base_name, id );
   print_toc(x, cerr);
   return 0;
}

void generate_toc_instance(random_gen& x, ostream& out, int instance_num, int min_nodes, int max_nodes)
{
   out << "1003\n";
   out << "4\n";
   x.random_oid(out, 1001, instance_num); // toc node id
   x.random_oid(out, 1001, 0); // parent node id
   x.random_integer(out); // 

   out << "106\n";
   out << "#\n";

   int nodes = x.pick_a_value(min_nodes, max_nodes);

   for ( int i=0; i<nodes; i++ )
      x.random_oid(out, 1001, 0); // id

   out << "#\n";
}

#endif
