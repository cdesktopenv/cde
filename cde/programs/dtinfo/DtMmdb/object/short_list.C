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
 * $XConsortium: short_list.cc /main/4 1996/06/11 17:25:56 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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



#include "object/short_list.h"

short_list::short_list(rootPtr t, c_code_t c_cd) : 
oid_list(c_cd), v_tempt(t)
{
}

short_list::short_list(short_list& x) : oid_list(x)
{
   v_tempt = x.v_tempt;
}

short_list::~short_list()
{
}

//
//Boolean short_list::value_LS(root& x, Boolean safe) const
//{
//   if ( safe == true &&
//        ( my_oid().ccode() != SHORT_LIST_CODE ||
//          x.my_oid().ccode() != SHORT_LIST_CODE  
//        ) 
//      )
//      return false;
//
//   short_list &y = *(short_list*)&x;
//
//   if ( sz != y.sz )
//      return false;
//
///************************************************/
//// Note: the components are indexed from 1 to sz
///************************************************/
//
//   for ( int i=1; i<=sz; i++ ) {
//
//      handler* rootPtr1 = get_component(i);
//      handler* rootPtr2 = y.get_component(i);
//
//      if ( (*rootPtr1) -> value_LS( *rootPtr2 ) == true )
//         return true;
//      if ( (*rootPtr1) -> value_EQ( *rootPtr2 ) == false )
//         return false;
//   }
//   return false;
//}
//
//Boolean short_list::value_EQ(root& x, Boolean safe) const
//{
//   if ( safe == true &&
//        ( my_oid().ccode() != SHORT_LIST_CODE ||
//          x.my_oid().ccode() != SHORT_LIST_CODE  
//        ) 
//      )
//      return false;
//
//   short_list &y = *(short_list*)&x;
//
//   if ( sz != y.sz )
//      return false;
//
///************************************************/
//// Note: the components are indexed from 1 to sz
///************************************************/
//
//   for ( int i=1; i<sz; i++ ) {
//
//      handler* rootPtr1 = get_component(i);
//      handler* rootPtr2 = y.get_component(i);
//
//      if ( (*rootPtr1) -> value_EQ( *rootPtr2 ) == false )
//         return false;
//   }
//   return true;
//}


handler* short_list::get_component(int index) 
{
  if ( !INRANGE(index, 1, (int) v_sz) ) {
     MESSAGE(cerr, "out of range in short_list::get_component()");
     throw(boundaryException(1, v_sz, index));
  }

  oid_t x = oid_list::operator()(index);

  handler *y = 0;

  if ( x.icode() != 0 )
     y = new handler(x, storage_ptr);

  return y;
}

Boolean short_list::pinned_insert(int index, oid_t& val) 
{
   return oid_list::update_component(index, val);
}

io_status short_list::asciiOut(ostream& out) 
{
   out << "OID_T:\n";
   my_oid().asciiOut(out);
   out << "\n";

   for ( unsigned int i=1; i<=v_sz; i++ ) {

      handler* hd_ptr = get_component(i);


      (*hd_ptr) -> asciiOut(out);


      out << "\n";

      delete hd_ptr;
   }
   return done;
}

#define OID_BUFFER_SIZE 40

io_status short_list::asciiIn(istream& in)
{
   if ( in.get() != LIST_MARK_CHAR ) {
      throw(formatException("short_list::asciiIn(): LIST_MARK_CHAR"));
   }

   if ( in.get() != '\n' ) {
      throw(formatException("short_list::asciiIn(): \n"));
   }

   handler* hd_ptr = 0;

#ifndef C_API
   oid_t oid_buffer[OID_BUFFER_SIZE]; 
   int oid_buffer_level = 0;
   int ct = 0;
#endif

   char c;
   while ( (c = in.get()) != LIST_MARK_CHAR ) {

      in.putback(c);

      c_code_t ccode;
      in >> ccode;  in.get();

      hd_ptr = new handler (oid_t(ccode, 0), storage_ptr);


      (*hd_ptr) -> asciiIn(in);

#ifndef C_API
      if ( oid_buffer_level == OID_BUFFER_SIZE ) {
        ct = oid_list::count();
        oid_list::expand_space(OID_BUFFER_SIZE);
        for ( int i=0; i<OID_BUFFER_SIZE; i++ ) {
           oid_list::update_component(ct+i+1, oid_buffer[i]);
        }
        oid_buffer_level = 0;
      } 

      oid_buffer[oid_buffer_level] = hd_ptr -> its_oid();
      oid_buffer_level++;
      
#else

      oid_list::insert_component(hd_ptr -> its_oid());
#endif

      delete hd_ptr;
   }

   if ( in.get() != '\n' ) {
      throw(formatException("short_list::asciiIn(): \n"));
   }


#ifndef C_API
   if ( oid_buffer_level > 0 ) {
      ct = oid_list::count();
      oid_list::expand_space(oid_buffer_level);
      for ( int i=0; i<oid_buffer_level; i++ ) {
        oid_list::update_component(ct+i+1, oid_buffer[i]);
      }
   }
#endif

   return done;
}

rootPtr short_list::get_template()
{
   return v_tempt ;
}

MMDB_BODIES(short_list)
HANDLER_BODIES(short_list)
