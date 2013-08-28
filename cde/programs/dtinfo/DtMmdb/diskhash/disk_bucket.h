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
 * $XConsortium: disk_bucket.h /main/4 1996/06/11 17:16:05 cde-hal $
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


#ifndef _disk_bucket_h
#define _disk_bucket_h

#include "storage/page.h"
#include "storage/page_storage.h"


class disk_bucket 
{

public:
   disk_bucket(int bucket_num, page_storage* store);
   virtual ~disk_bucket();

// update
   //void set_k(int new_k) { v_k = new_k; };
   //void set_r(int new_rotate) { v_r = new_rotate; } ;
   void set_overflow(Boolean ov) { v_overflowed = ov; } ;

   //int k() { return v_k; };
   //int r() { return v_r; };
   Boolean overflow() { return v_overflowed; };

// update
   int insert(data_t*);
   Boolean remove(int ind);
   void remove_all();

   Boolean member(data_t& v, int& slot_num);

   int first() { int ind = 0; next(ind); return ind; };
   data_t* operator()(int ind);
   void next(int& ind) { ind = ( ind >= count() ) ? 0 : ind+1; };

   int count() { return MAX(0, bucket_page() -> count() - 2); };
   Boolean empty() { return ( count() == 0 ) ? true : false; };
   int bnum() { return v_bucket_num; };

   ostream& asciiOut(ostream& out, print_func_ptr_t print_f);
   friend ostream& operator<<(ostream&, disk_bucket&);

protected:
   void init_params();
   void sync_params();
   page* bucket_page();

protected:
   int v_bucket_num;
   page_storage* v_key_store;

   Boolean  v_overflowed;
   //unsigned int v_k;
   //unsigned int v_r;

   buffer& buf;
};

typedef disk_bucket* disk_bucketPtr;

#endif
