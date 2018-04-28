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
 * $XConsortium: oid.cc /main/4 1996/06/11 17:24:56 cde-hal $
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


#include "object/oid.h"

oid::oid(c_code_t c_id) : primitive(c_id)
{
}

oid::oid(const oid_t& x) : primitive(OID_CODE), v_oid(x)
{
}

oid::oid(const oid& x) : primitive(x), v_oid(x.v_oid)
{
}

/* inline
void oid::set(const oid_t& x) 
{
   v_oid.become(x);
}
*/

/*
Boolean oid::value_EQ(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( x.my_oid().c_code != OID_CODE ||
          f_oid.c_code != OID_CODE  
        )
      )
      return false;
   
   return v_oid.eq(((oid*)&x) -> v_oid);
}

Boolean oid::value_LS(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( x.my_oid().c_code != OID_CODE ||
          f_oid.c_code != OID_CODE  
        )
      )
      return false;
   
   return v_oid.ls(((oid*)&x) -> v_oid);
}
*/

io_status oid::asciiIn(istream& in)
{
   set_mode(UPDATE, true);
   return ( v_oid.asciiIn(in) );
}

io_status oid::asciiOut(ostream& out) 
{
   v_oid.asciiOut(out);
   return done;
}

/* inline
oid_t& oid::my_coid() 
{
   return v_oid;
}
*/


int oid::cdr_sizeof()
{
   return primitive::cdr_sizeof() + v_oid.cdr_sizeof() + 
          sizeof(v_oid.v_c_code);
}

io_status oid::cdrOut(buffer& buf)
{
   primitive::cdrOut(buf);
   v_oid.cdrOut(buf);
   buf.put(v_oid.v_c_code);
   return done;
}

io_status oid::cdrIn(buffer& buf)
{
   primitive::cdrIn(buf);
   v_oid.cdrIn(buf);
   buf.get(v_oid.v_c_code);
   return done;
}

MMDB_BODIES(oid)
HANDLER_BODIES(oid)

