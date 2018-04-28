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
