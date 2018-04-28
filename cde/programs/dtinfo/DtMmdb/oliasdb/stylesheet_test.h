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
