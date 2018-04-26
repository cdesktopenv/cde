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
 * $XConsortium: imp_bucket.cc /main/4 1996/07/18 14:32:05 drk $
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



#include "dynhash/imp_bucket.h"

imp_bucket::imp_bucket() : k(19), rotate(0)
{
}

imp_bucket::~imp_bucket()
{
}

Boolean imp_bucket::empty()
{
   return ( count() == 0 ) ? true : false;
}

/************************************************/
// insert 
/************************************************/
Boolean imp_bucket::insert(data_t* data)
{
   insert_as_tail(data);
   return true;
}

/************************************************/
// remove 
/************************************************/
data_t* imp_bucket::remove_all()
{
   data_t* x = (data_t*)v_head;
   v_head = v_tail = 0;
   return x;
}

/*******************************/
// compute the hash value
/*******************************/
int imp_bucket::h(int key, int prime, int M)
{
   return ( abs( k * key ) % prime + rotate ) % M ;
}

data_t* imp_bucket::operator()(long ind)
{
   if ( ind == 0 ) {
      MESSAGE(cerr, "imp_bucket::operator(): zero index value");
      throw(boundaryException(1, MAXINT, ind));
   }

   return (data_t*)ind;
}


/***********************************************/
// print operation
/***********************************************/
ostream& operator<<(ostream& out, imp_bucket& bt)
{
   long ind = bt.first();
   while ( ind != 0 ) {
      out << *bt(ind) << "\n";
      bt.next(ind);
   }
   return out;
}

ostream& imp_bucket::asciiOut(ostream& out, print_func_ptr_t print_f)
{
   long ind = first();
   while ( ind != 0 ) {
      (*this)(ind) -> asciiOut(out, print_f);
      next(ind);
   }
   return out;
}
