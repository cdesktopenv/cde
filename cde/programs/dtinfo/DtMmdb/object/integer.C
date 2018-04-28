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
 * $XConsortium: integer.cc /main/4 1996/06/11 17:24:32 cde-hal $
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


#include "object/integer.h"

integer::integer(c_code_t c_id) : primitive(c_id)
{
      v_intRep = 0;
}

integer::integer(const integer& x) : primitive(x)
{
   v_intRep = x.v_intRep;
}

integer::~integer()
{
}

void integer::set(const int i)
{
   v_intRep = i;
   set_mode(UPDATE, true);
}

int integer::get()
{
   return v_intRep;
}



/*
Boolean integer::copy(int selector, root*& x) const
{
   if ( f_oid.ccode() != INTEGER_CODE || selector != VALUE)
      return false;
   else {
      x = new integer(*this);
      return true;
   }
}
*/

/*
Boolean integer::value_LS(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != INTEGER_CODE ||
          x.my_oid().ccode() != INTEGER_CODE 
        ) 
      )
      return false;

   integer &y = *(integer*)&x;
   if ( v_intRep < y.v_intRep )
      return true;
   else
      return false;
}

Boolean integer::value_EQ(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != INTEGER_CODE || 
          x.my_oid().ccode() != INTEGER_CODE 
        ) 
      ) 
      return false;

   integer &y = *(integer*)&x;
   if ( v_intRep == y.v_intRep )
      return true;
   else
      return false;
}
*/

io_status integer::asciiOut(ostream& out) 
{
   out << v_intRep;
   return done;
}

io_status integer::asciiIn(istream& in) 
{
   if ( ! cc_is_digit(in) )
      throw (stringException("a digit expected"));

   in >> v_intRep; 

   if ( in.get() != '\n' ) {
      throw (stringException("'\\n' expected"));
   }

   set_mode(UPDATE, true);
   return done;
}


int integer::cdr_sizeof()
{
   return primitive::cdr_sizeof() + sizeof(v_intRep);
}

io_status integer::cdrOut(buffer& buf)
{
   primitive::cdrOut(buf);
   buf.put((unsigned int)v_intRep);
   return done;
}

io_status integer::cdrIn(buffer& buf)
{
   primitive::cdrIn(buf);
   buf.get(*(unsigned int*)&v_intRep);
   return done;
}

MMDB_BODIES(integer)
HANDLER_BODIES(integer)

