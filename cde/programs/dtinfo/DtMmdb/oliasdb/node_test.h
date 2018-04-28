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
