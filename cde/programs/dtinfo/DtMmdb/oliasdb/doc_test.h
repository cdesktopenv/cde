/* $XConsortium: doc_test.h /main/4 1996/07/18 16:32:48 drk $ */

#ifndef _doc_test_h
#define _doc_test_h

#ifdef REGRESSION_TEST

#include "oliasdb/doc_hd.h"
#include "object/random_gen.h"

void print_doc(doc_smart_ptr& x, ostream& out, Boolean out_it_oid, 
		Boolean out_tag_list);

int compare_doc(doc_smart_ptr& pattern, info_base* base_ptr);

void generate_doc_instance(random_gen&, ostream& out, int seq_num, int min_tabs = 1, int max_tabs = 20);
#endif


#endif
