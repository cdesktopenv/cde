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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
