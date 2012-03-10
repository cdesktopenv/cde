/* $XConsortium: toc_test.h /main/4 1996/07/18 16:34:47 drk $ */

#ifdef REGRESSION_TEST

#ifndef _toc_test_h
#define _toc_test_h

#include "oliasdb/toc_hd.h"
#include "object/random_gen.h"


int compare_toc(toc_smart_ptr& pattern, info_base* base_ptr);
void generate_toc_instance(random_gen&, ostream& out, int instance_num, int min_nodes = 1, int max_nodes = 20);


#endif
#endif
