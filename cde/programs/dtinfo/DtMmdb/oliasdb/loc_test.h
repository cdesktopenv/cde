/* $XConsortium: loc_test.h /main/4 1996/07/18 16:33:21 drk $ */
#ifdef REGRESSION_TEST

#ifndef _loc_test_h
#define _loc_test_h

#include "oliasdb/locator_hd.h"
#include "object/random_gen.h"


int compare_locator(locator_smart_ptr& pattern, info_base* base_ptr);
int loc_test(info_lib* infolib_ptr, const char* base_name, const char* loc );
void generate_loc_instance(random_gen&, ostream& out);


#endif
#endif
