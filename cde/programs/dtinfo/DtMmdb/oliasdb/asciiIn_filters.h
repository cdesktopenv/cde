/* $XConsortium: asciiIn_filters.h /main/4 1996/06/11 17:26:27 cde-hal $ */

#ifndef _filters_h
#define _filters_h 1

#include "utility/funcs.h"
#include "api/info_lib.h"

char* get_oid(info_lib*, char* base_nm, char* locator_line); // with byte count and \t
char* get_oid_2(info_lib*, char* base_str, char* locator); // without byte count and \t


#endif
