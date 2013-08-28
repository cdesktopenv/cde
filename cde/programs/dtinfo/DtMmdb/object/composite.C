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
 * $XConsortium: composite.cc /main/5 1996/07/18 14:40:37 drk $
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


#include "object/composite.h"

composite::composite(c_code_t c_id) : root(c_id)
{
   //init_persistent_info();
   v_sz = 0;
}
   
composite::composite(const composite& x) : root(x) 
{ 
   //init_persistent_info();
   v_sz = x.v_sz; 
}

int composite::first() const
{
   if ( v_sz == 0 )
      return 0;
   else 
      return 1;
}

void composite::next(int& index) const
{
   if INRANGE( index, 1, (int) v_sz - 1 )
      index ++;
   else 
      index = 0;
}

handler* composite::get_component(int) 
{
   throw(stringException("composite::get_component() can't be called"));
   return 0;
}

void composite::set_mode(obj_mode_t mode, Boolean option)
{
   root::set_mode(mode, option);
}


Boolean composite::test_io_mode(int mode)
{
   if ( storage_ptr )
      return storage_ptr -> io_mode(mode);
   else
      return false;
}


int composite::cdr_sizeof()
{
   return root::cdr_sizeof() + sizeof(v_sz);
}

io_status composite::cdrOut(buffer& buf)
{
   root::cdrOut(buf);
   buf.put(v_sz);
   return done;
}
  
io_status composite::cdrIn(buffer& buf)
{
   root::cdrIn(buf);
   buf.get(v_sz);
   return done;
}

MMDB_BODIES(composite)


NEW_AND_DELETE_BODIES(composite)


composite_handler::composite_handler(const oid_t& v_oid, storagePtr _store):
handler(v_oid, _store)
{
}

composite_handler::~composite_handler()
{
}

composite* composite_handler::operator ->()
{
   return (composite*)handler::operator->();
}

handler* composite_handler::_get_component(int i)
{
  handler* x = 0;

  if ( i == THIS ) 
     x = this;
  else {
      x = ((composite*)handler::operator->()) -> get_component(i);
  }

  if ( x == 0 ) 
     throw(stringException("NULL component handler ptr"));

  return x;
}

