/* $XConsortium: node_test.h /main/4 1996/07/18 16:34:00 drk $ */

#ifdef REGRESSION_TEST

#ifndef _node_test_h
#define _node_test_h

#include "oliasdb/node_hd.h"
#include "object/random_gen.h"


void print_node(node_smart_ptr& x, ostream& out, Boolean get_data, Boolean get_doc_id);

int compare_node(node_smart_ptr& pattern, info_base* base_ptr);
int compare_SGML_content(istream& in, info_base* base_ptr, Boolean doing_test);
void generate_node_instance(info_base*, random_gen&, ostream& out, 
			    ostream& mixed_out, int min_len, int max_len);



#endif
#endif
