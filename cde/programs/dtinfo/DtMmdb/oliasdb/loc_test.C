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
   ostrstream pattern_out(pattern_buf, LARGE_BUFSIZ, ios::out);
   print_loc(pattern, pattern_out);

   locator_smart_ptr x( base_ptr, pattern.inside_node_locator_str());
   char db_buf[LARGE_BUFSIZ];
   ostrstream db_out(db_buf, LARGE_BUFSIZ, ios::out);
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
