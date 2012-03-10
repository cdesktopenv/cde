/* $XConsortium: stylesheet_test.h /main/4 1996/07/18 16:34:23 drk $ */

#ifdef REGRESSION_TEST

#ifndef _stylesheet_test_h
#define _stylesheet_test_h

#include "oliasdb/stylesheet_hd.h"
#include "object/random_gen.h"

void print_stylesheet(stylesheet_smart_ptr& x, ostream& out);

int compare_stylesheet(stylesheet_smart_ptr& pattern, info_base* base_ptr);

void generate_stylesheet_instance(random_gen&, ostream& out, int min_len=500, int max_len=5000);


#endif
#endif
