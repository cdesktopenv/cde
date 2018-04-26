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
 * $XConsortium: transaction.C /main/5 1996/08/21 15:51:19 drk $
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


#include "api/transaction.h"
#include "dstr/bset.h"
#include "mgrs/misc.h"

transaction* g_transac = 0;

//////////////////////////////////////
// application functions
//////////////////////////////////////
inline void begin_trans(const void* x) 
{
   ((page_storage*)x) -> begin_trans();
}

inline void end_trans(const void* x) 
{
   ((page_storage*)x) -> commit_trans();
}

inline void roll_back(const void* x) 
{
   ((page_storage*)x) -> roll_back();
}

inline void psync(const void* x) 
{
   ((page_storage*)x) -> sync();
}

inline void commit_object(const void* x)
{
   name_oid_t *y = (name_oid_t*)x;

   if (y==0)
      throw(stringException("null pointer"));

   handler z(y->v_oid, y->v_store);
   z.commit();
}

inline void set_updated_false_f(const void* x)
{
   name_oid_t *y = (name_oid_t*)x;
   if (y==0)
      throw(stringException("null pointer"));
   handler z(y->v_oid, y->v_store);
   z -> set_mode(UPDATE, false);
}

//////////////////////////////////
//
//
//////////////////////////////////
transaction::transaction() :
   v_store_array(), v_updated_objects(oid_storage_eq, oid_storage_ls)
{
}

transaction::~transaction() 
{
}

void transaction::book(abs_storage* x)
{
   if ( v_store_array.member(x) == 0 ) {
      ((page_storage*)x) -> begin_trans();
      v_store_array.insert(x);
   }
}

void transaction::book(oid_t& id, abs_storage* x)
{
   char _dummy[1]; _dummy[0] = 0;
   name_oid_t* y = new name_oid_t(_dummy, id, x);

   if ( v_updated_objects.insert(y) == false )
      delete y;

   if ( v_store_array.member(x) == 0 ) {
      debug(cerr, id);
      debug(cerr, x -> my_path());
      debug(cerr, x -> my_name());
      throw(stringException("store is not in transaction mode"));
   }
}

void transaction::begin()
{
   v_store_array.apply(begin_trans);
   g_transac = this;
//MESSAGE(cerr, "g_tr in transaction.C");
//debug(cerr, int(g_transac));
}

void transaction::end()
{
//MESSAGE(cerr, "transaction::end()");

   v_updated_objects.apply(commit_object);
   v_store_array.apply(end_trans);

   v_updated_objects.del_elements(delete_name_oid_rec_f);

   g_transac = 0;
//MESSAGE(cerr, "transaction::end() done");
}

void transaction::abort()
{
   v_updated_objects.del_elements(delete_name_oid_rec_f);
   g_transac = 0;
}

void transaction::rollback()
{
   v_updated_objects.apply(set_updated_false_f);
   v_store_array.apply(roll_back);
}

void transaction::sync()
{
   v_updated_objects.apply(commit_object);
   v_store_array.apply(psync);

   g_transac = 0;
}

