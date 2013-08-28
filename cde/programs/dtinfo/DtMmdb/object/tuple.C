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
 * $XConsortium: tuple.cc /main/4 1996/06/11 17:26:06 cde-hal $
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


#include "object/tuple.h"

#define MAX_COMPS 50

tuple::tuple(c_code_t c_cd) : oid_list(c_cd)
{
}

tuple::tuple(int comps, c_code_t c_cd) : oid_list(comps, c_cd)
{
}

tuple::tuple(tuple& x) : oid_list(x)
{
}

tuple::~tuple()
{
}

handler* tuple::get_component(int index) 
{

//MESSAGE(cerr, "in tuple():: get_component()");
//debug(cerr, index);
//debug(cerr, my_oid());
//debug(cerr, int(storage_ptr));


   if (!INRANGE(index, 1, (int) v_sz)) {
      MESSAGE(cerr, "out of range in tuple::get_component()");
      throw( boundaryException(1, v_sz, index) );
   }

   oid_t x = oid_list::operator()(index);

//debug(cerr, x);

//debug(cerr, x.ccode());
//debug(cerr, x.icode());

   handler* y = 0;

   if ( x.icode() != 0 )
      y = new handler(x, storage_ptr);

   return y;
}

Boolean tuple::pinned_insert(int index, const oid_t& val) 
{
//MESSAGE(cerr, "in tuple():: pinned_component()");
//debug(cerr, index);
//debug(cerr, val);
      return oid_list::update_component(index, val);
}

io_status tuple::asciiOut(ostream& out) 
{
   out << "OID_T:\n";
   my_oid().asciiOut(out); 
   out << "\n";
   // debug(cerr, v_sz);

   for ( unsigned int i=1; i<=v_sz; i++ ) {
    
      handler* hd_ptr = get_component(i);

      if ( hd_ptr == 0 )
         continue;


      (*hd_ptr) -> asciiOut(out);


      out << "\n";

      delete hd_ptr;
   }
   return done;
}

io_status tuple::asciiIn(istream& in)
{

/*
MESSAGE(cerr, "in tuple asciiIn()");
my_oid().asciiOut(cerr);
MESSAGE(cerr, "\n");
*/
   int comps; in >> comps; 

   if ( comps > MAX_COMPS ) {
      debug(cerr, comps);
      throw(stringException("exceed MAX_COMPS"));
   }

//debug(cerr, comps);

   int ret = in.get();

   if ( ret != '\n' ) {
      debug(cerr, ret);
      throw(stringException("'\\n' expected"));
   }

   if ( comps > (int) v_sz ) {
MESSAGE(cerr, "tuple asciiIn(): to expand space");
debug(cerr, comps);
debug(cerr, v_sz);
     oid_list::expand_space(comps - v_sz);
     v_sz = comps;
   }


   handler *hd_ptr = 0;

   for ( int i=1; i<=comps; i++ ) {
 
      c_code_t ccode;
      in >> ccode;  

      if ( in.get() != '\n' ) {
         throw(stringException("'\\n' expected"));
      }

      hd_ptr = new handler(ccode, storage_ptr);

      (*hd_ptr) -> asciiIn(in);

      pinned_insert(i, hd_ptr -> its_oid());

      delete hd_ptr;
   }

   set_mode(UPDATE, true);

   return done;
}

MMDB_BODIES(tuple)
HANDLER_BODIES(tuple)
