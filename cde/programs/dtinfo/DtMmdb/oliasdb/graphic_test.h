/* $XConsortium: graphic_test.h /main/4 1996/07/18 16:33:04 drk $ */

#ifdef REGRESSION_TEST

#ifndef _graphic_test_h
#define _graphic_test_h


#include "oliasdb/graphic_hd.h"
#include "object/random_gen.h"

void print_graphic(graphic_smart_ptr& x, ostream& out);

int compare_graphic(graphic_smart_ptr& pattern, info_base* base_ptr);
void generate_graphic_instance(info_base*, random_gen&, ostream& out, int min_len, int max_len);

#endif

#endif
