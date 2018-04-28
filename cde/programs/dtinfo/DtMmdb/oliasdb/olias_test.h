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
/*
 * $XConsortium: olias_test.h /main/4 1996/07/18 14:49:25 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#ifndef _olias_test_h
#define _olias_test_h 1

#include "oliasdb/mmdb.h"

int select_debug_routine(int argc, char** argv, OLIAS_DB&);

#ifdef DEBUG

int loc_test( info_lib*, const char* base_name, const char* loc );
int toc_test( info_lib*, const char* base_name, const char* oid_id_str);
int graphic_test( info_lib*, const char* base_name, const char* oid_str);

int node_test_loc( info_lib*, const char* base_name, const char* loc );
int node_test_oid( info_lib*, const char* base_name, const char* oid_str );
int dlp_test( info_lib*, const char* base_name, const char* oid_str );
int doc_test_oid( info_lib*, const char* base_name, const char* oid_str );
int doc_test_int( info_lib*, const char* base_name, const char* oid_str );
int doc_test_doc_oid( info_lib*, const char* base_name, const char* oid_str );
int stylesheet_test_loc( info_lib*, const char* base_name, const char* loc );
int stylesheet_test_oid( info_lib*, const char* base_name, const char* oid_str);
void update_stylesheet_test(char* filename, info_base* base_ptr, char* locator);
int cache_test( info_lib*, const char* base_name, int argc, char** argv );
void dump_node_ids(info_base*);

#endif

#ifdef REGRESSION_TEST
int generate_stream(info_base*, char* path, int num_sections, int min_section_len, int max_sec_len);
#endif


#endif
