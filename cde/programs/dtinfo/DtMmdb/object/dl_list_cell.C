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
 * $XConsortium: dl_list_cell.cc /main/4 1996/06/11 17:24:22 cde-hal $
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


#include "object/dl_list_cell.h"

dl_list_cell::dl_list_cell(c_code_t c_id) : composite(c_id)
{
      v_forward_ptr.become(ground);
      v_backward_ptr.become(ground);
}

dl_list_cell::dl_list_cell(const dl_list_cell& x) : 
composite(x), 
v_backward_ptr(x.v_backward_ptr), 
v_forward_ptr(x.v_forward_ptr)
{
}

dl_list_cell::~dl_list_cell()
{
}

void dl_list_cell::set_backward_ptr(const oid_t& x)
{
   v_backward_ptr.become(x);
   set_mode(UPDATE, true);
}

void dl_list_cell::set_forward_ptr(const oid_t& x)
{
   v_forward_ptr.become(x);
   set_mode(UPDATE, true);
}

oid_t& dl_list_cell::get_backward_ptr()
{
   return v_backward_ptr;
}

oid_t& dl_list_cell::get_forward_ptr()
{
   return v_forward_ptr;
}


/*
boolean dl_list_cell::value_LS(root& x, boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != DL_LIST_CELL_CODE ||
          x.my_oid().ccode() != DL_LIST_CELL_CODE 
        ) 
      )
      return false;

   dl_list_cell &y = *(dl_list_cell*)&x;
   if ( v_backward_ptr.ls(y.v_backward_ptr) == true ||
        ( v_backward_ptr.eq(y.v_backward_ptr) == true &&
          v_forward_ptr.ls(y.v_forward_ptr) == true 
        )
      )
      return true;
   else
      return false;
}

boolean dl_list_cell::value_EQ(root& x, boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != DL_LIST_CELL_CODE || 
          x.my_oid().ccode() != DL_LIST_CELL_CODE 
        ) 
      ) 
      return false;

   dl_list_cell &y = *(dl_list_cell*)&x;
   if ( v_backward_ptr.eq(y.v_backward_ptr) == true )
      return true;
   else
      return false;
}
*/

io_status dl_list_cell::asciiOut(ostream& out) 
{
   out << "OID_T:\n";
   my_oid().asciiOut(out);
   out << "\n";

   v_backward_ptr.asciiOut(out);
   out << "\n";
   v_forward_ptr.asciiOut(out);
   return done;
}

io_status dl_list_cell::asciiIn(istream& in) 
{
   v_backward_ptr.asciiIn(in);
   v_forward_ptr.asciiIn(in);
   set_mode(UPDATE, true);
   return done;
}


int dl_list_cell::cdr_sizeof()
{
   return composite::cdr_sizeof() + v_backward_ptr.cdr_sizeof() + 
          v_forward_ptr.cdr_sizeof();
}

io_status dl_list_cell::cdrOut(buffer& buf)
{
   composite::cdrOut(buf);
   v_backward_ptr.cdrOut(buf);
   v_forward_ptr.cdrOut(buf);
   return done;
}

io_status dl_list_cell::cdrIn(buffer& buf)
{
   composite::cdrIn(buf);
   v_backward_ptr.cdrIn(buf);
   v_forward_ptr.cdrIn(buf);
   return done;
}

MMDB_BODIES(dl_list_cell)
HANDLER_BODIES(dl_list_cell)

