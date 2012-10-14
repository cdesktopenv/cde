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
 * $XConsortium: oid_t.cc /main/4 1996/07/18 14:44:11 drk $
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


#include "object/oid_t.h"

#ifdef C_API
oid_t* ground_ptr = 0;
#else
oid_t ground(c_code_t(0), i_code_t(0));
#endif


/* inline
oid_t::oid_t()
{
}
*/

/* inline
oid_t::oid_t(c_code_t c, i_code_t i) :
v_c_code(c), v_i_code(i), v_e_code(0)
{
}
*/

/* inline
oid_t::oid_t(const oid_t& x)
{
   v_e_code = x.v_e_code;
   v_c_code = x.v_c_code;
   v_i_code = x.v_i_code;
}
*/

oid_t::oid_t(const char* source, Boolean ascii_format, Boolean swap_order)
{
   if ( source == 0 )
       throw(stringException("NULL string pointer"));

   if ( ascii_format == false ) {

// 4/28/93.  do not save c_code in an oid_t.
      v_c_code = 1;
      memcpy((char*)&v_i_code, source, sizeof(v_i_code));

      if ( swap_order == true )
#ifdef __osf__
         ORDER_SWAP_INT(v_i_code);
#else
         ORDER_SWAP_LONG(v_i_code);
#endif

   } else {
      istringstream in((char*)source);

      in >> v_c_code;

      if ( in.get() == '\n') {
         v_i_code = 0;
      } else {
         in >> v_i_code;
      }
   }

/*
MESSAGE(cerr, "constr oid_t using a string");
debug(cerr, c_code);
debug(cerr, i_code);
*/
}

/* inline
oid_t::~oid_t()
{
}
*/

/* inline
void oid_t::become(const oid_t& x)
{
   v_e_code = x.v_e_code;
   v_c_code = x.v_c_code;
   v_i_code = x.v_i_code;
}
*/

Boolean oid_t::eq(const oid_t& x) const
{
   if ( v_i_code == x.v_i_code ) {
      if ( v_c_code == 1 || x.v_c_code == 1 )
         return true;
      else {
        if ( v_c_code == x.v_c_code )
          return true;
        else
          return false;
      }
   } else
      return false;
}

Boolean oid_t::ls(const oid_t & x) const
{
   if ( v_c_code != 1 && x.v_c_code != 1 && v_c_code >= x.v_c_code )
      return false;
   else
   if ( v_i_code >= x.v_i_code )
      return false;
   else
      return true;
}

// Static function to compute a hash code for an oid_t
unsigned oid_t::hash(const oid_t& id)
{
    return unsigned(id.icode()) + unsigned(id.ccode());
}

io_status oid_t::asciiOut(ostream& out) const
{
   out << v_c_code << "." ; 

// for compacted disk store. need to revisit. not working.
//   out << 0 << "." ;

   out << v_i_code;
   return done;
}

io_status oid_t::asciiIn(istream& in) 
{
   _asciiIn(in);

   if ( in.get() != '\n' ) {
       throw(stringException("'\\n' expected"));
   }

   return done;
}

io_status oid_t::_asciiIn(istream& in) 
{
   if ( ! cc_is_digit(in) )
      throw (stringException("a digit is expected"));

   if ( v_c_code == 0 )
      in >> v_c_code;
   else {
      c_code_t y;
      in >> y;
      if ( v_c_code != y )
         return fail;
   }

   int mid = in.get();

   if ( mid == '\n') {
      v_i_code = 0;
      in.putback(char(mid));
   } else {

      if ( mid != '.' ) {
          debug(cerr, v_c_code);
          debug(cerr, mid);
          throw(stringException("'.' expected"));
      }
    
      if ( ! cc_is_digit(in) )
         throw (stringException("a digit expected"));

      in >> v_i_code;

   }

   return done;
}

ostream& operator<<(ostream& out, const oid_t& id) 
{
   id.asciiOut(out);
   return out;
}

void oid_t::to_char_string(char* sink, Boolean swap_order) const
{
   if ( sink == 0 )
       throw(stringException("NULL string pointer"));

////////////////////////////////////////////
// 4/28/93.  Do not save c_code in an oid_t.
////////////////////////////////////////////
/*
   //bcopy((char*)&c_code, sink, sizeof(c_code));
   memcpy(sink, (char*)&c_code, sizeof(c_code));

   //bcopy((char*)&i_code, 
   //      sink+sizeof(c_code), sizeof(i_code));
   memcpy(
         sink+sizeof(c_code), 
         (char*)&i_code, 
         sizeof(i_code));
*/

   if ( swap_order == true ) {
      i_code_t x = v_i_code;
#ifdef __osf__
      ORDER_SWAP_INT(x);
#else
      ORDER_SWAP_LONG(x);
#endif
      memcpy(sink, (char*)&x, sizeof(x));
   } else 
      memcpy(sink, (char*)&v_i_code, sizeof(v_i_code));
}


int oid_t::cdr_sizeof()
{
   //return sizeof(c_code) + sizeof(i_code);
// 4/30/93. save no c_code
   return sizeof(v_i_code);
}

io_status oid_t::cdrOut(buffer& buf)
{
// 4/30/93. save no c_code
//   buf.put(c_code);
   buf.put(v_i_code);
   return done;
}
   
io_status oid_t::cdrIn(buffer& buf, c_code_t c_code_to_use)
{
// 4/30/93. save no c_code
//   buf.get(v_c_code);
   buf.get(v_i_code);
   v_c_code = c_code_to_use;
   return done;
}


