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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: sorter.h /main/3 1996/06/11 17:20:52 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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




#ifndef _sorter_h
#define _sorter_h 1

#include <limits.h>
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>
#endif
#include "utility/funcs.h"
#include "utility/atoi_fast.h"
#include "hmphf/buckets.h"

class sorter 
{

public:
   sorter(char* key_file);
   sorter(istream& key_stream);
   ~sorter();

   char** unique_keys() { return v_unique_keys; };
   int no_unique_keys() { return v_no_unique_keys; };

   friend ostream& operator<< (ostream&, sorter&);

protected:
   void _init(istream&);
   void filter_by_hash();
   void filter_a_bucket(bucketPtr bkt);
   void assemble_unique_keys();

   void remove_keys(bucketPtr bkt, char* key, slist_void_ptr_cell* lp);

protected:
   bucketPtr* v_bucket_array;
   int v_max_bucket_sz;
   int v_no_unique_keys;
   char** v_unique_keys;

   atoi_fast b_convertor;

   charPtr*	v_map_table; 
   int*		v_index_table;
   charPtr*	v_check_table; 
   charPtr*	v_dup_table;
};

#endif
