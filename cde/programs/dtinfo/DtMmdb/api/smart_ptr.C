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
 * $XConsortium: smart_ptr.cc /main/4 1996/06/11 17:11:33 cde-hal $
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



#include "api/smart_ptr.h"
#include "object/compressed_pstring.h"


smart_ptr::smart_ptr(info_lib* lib_ptr,
                     const char* ibase_name, 
                     int composite_pos,
                     const handler& query_hd,
                     int index_selector,
                     composite_tag tag
                    ) 
{
   info_base* ibase = lib_ptr -> get_info_base(ibase_name);

   if ( ibase == 0 ) {
       throw(stringException(
          form("unknown base %s in %s\n", ibase_name, 
               lib_ptr -> get_info_lib_path()
            )));
   }
//debug(cerr, ibase -> num_of_docs());

   switch (tag) {
     case SET:
      {
      cset_handlerPtr set_ptr = ibase -> get_set(composite_pos);

      if ( set_ptr == 0 )
         throw(stringException("NULL set ptr"));

      _init((*set_ptr)->get_first_oid(query_hd, index_selector),
            set_ptr -> its_store()
           );
      }
      break;
     case LIST:
      {
      dl_list_handlerPtr list_ptr = ibase -> get_list(composite_pos);

      if ( list_ptr == 0 )
         throw(stringException("NULL set ptr"));

      _init((*list_ptr)->get_first_oid(query_hd, index_selector),
            list_ptr -> its_store()
           );
      }
      break;
     default:
      throw(stringException("unknown tag"));
   }

}

smart_ptr::smart_ptr(info_base* ibase, 
                     int composite_pos,
                     const handler& query_hd,
                     int index_selector,
                     composite_tag tag
                    ) 
{
   switch (tag) {
     case SET:
      {
      cset_handlerPtr set_ptr = ibase -> get_set(composite_pos);

      if ( set_ptr == 0 )
         throw(stringException("NULL set ptr"));

      _init((*set_ptr)->get_first_oid(query_hd, index_selector),
            set_ptr -> its_store()
           );
      }
      break;
     case LIST:
      {
      dl_list_handlerPtr list_ptr = ibase -> get_list(composite_pos);

      if ( list_ptr == 0 )
         throw(stringException("NULL set ptr"));

      _init((*list_ptr)->get_first_oid(query_hd, index_selector),
            list_ptr -> its_store()
           );
      }
      break;
     default:
      throw(stringException("unknown tag"));
   }

}

/* inlined */
/*
smart_ptr::smart_ptr()
{
}

smart_ptr::~smart_ptr()
{
}
*/

void smart_ptr::_init(const oid_t& id, const abs_storage* x)
{
//MESSAGE(cerr, "smart_ptr::_init()");
   obj_id.become(id);
//obj_id.asciiOut(cerr); cerr << "\n";

   store = (abs_storage*)x;
   obj_ptr = 0;

   handler::operator->(); 

//obj_id.asciiOut(cerr); cerr << "\n";
//debug(cerr, int(obj_ptr));
//MESSAGE(cerr, "smart_ptr::_init() done");
}

   
handler* smart_ptr::get_handler(int i, c_code_t code)
{
   handler* x =  _get_component(i); 
   x -> operator->();                      

   if ( x -> its_oid().ccode() != code )   
       throw(stringException("invalid node data class code"));

   return x;
}

int smart_ptr::get_int(int i)
{
   handler* x = get_handler(i, INTEGER_CODE);
   int y= (*(integer_handler*)x) -> get();
   delete x;

   return y;
}

const char* smart_ptr::get_string(int i, buffer& buf)
{
  handler* x = _get_component(i);

  x -> operator->(); // this will bring the its_oid field up-to-date

  char* str;

  switch ( x -> its_oid().ccode() ) {
     case STRING_CODE:
       {
       str = (*(pstring_handler*)x) -> get(buf);
       break;
       }
     case COMPRESSED_STRING_CODE:
       {
       str = (*(compressed_pstring_handler*)x) -> get(buf);
       break;
       }

     default:
       throw(stringException("invalid node data class code"));
  }

  delete x;

  return str;
}

const char* smart_ptr::get_string(int i)
{
  handler* x = _get_component(i);

  x -> operator->(); // this will bring the its_oid field up-to-date

  char* str;

  switch ( x -> its_oid().ccode() ) {
     case STRING_CODE:
       {
       str = (*(pstring_handler*)x) -> get();
       break;
       }
     case COMPRESSED_STRING_CODE:
       {
       str = (*(compressed_pstring_handler*)x) -> get();
       break;
       }

     default:
       throw(stringException("invalid node data class code"));
  }

  delete x;

  return str;
}


int smart_ptr::get_string_size(int i)
{
  handler* x = _get_component(i);
  x -> operator->(); // this will bring the its_oid field up-to-date

  int y ;

  switch ( x -> its_oid().ccode() ) {
     case STRING_CODE:
       y = (*(pstring_handler*)x) -> size();
       break;

     case COMPRESSED_STRING_CODE:
       y = (*(compressed_pstring_handler*)x) -> size();
       break;

     default:
       throw(stringException("invalid node data class code"));
  }

  delete x;

  return y;
}

oid_t smart_ptr::get_oid(int i)
{
   handler* x = get_handler(i, OID_CODE);
   oid_t y = (*(oid_handler*)x) -> my_coid();
   delete x;

   return y;
}

void smart_ptr::update_oid(int i, const oid_t& x)
{
   static char buf[64];

   handler* z = get_handler(i, OID_CODE);
   oid_handler* y = (oid_handler*)z;

   snprintf(buf, sizeof(buf), "%d.%d\n", x.ccode(), (int)x.icode());
   istringstream in(buf);

   (*y) -> asciiIn(in);

   delete y;
}

void smart_ptr::update_string(int i, istream& in)
{
  handler* x = _get_component(i);

  x -> operator->(); // this will bring the its_oid field up-to-date

  io_status ok;
  switch ( x -> its_oid().ccode() ) {
     case STRING_CODE:
       ok =(*(pstring_handler*)x) -> asciiIn(in);
       break;

     case COMPRESSED_STRING_CODE:
       ok =(*(compressed_pstring_handler*)x) -> _asciiIn(in);
       break;

     default:
       throw(stringException("invalid node data class code"));
  }

  if(ok) { ; }
  delete x;
}

void smart_ptr::update_string(int i, const char* buf, int size)
{
  handler* x = _get_component(i);

  x -> operator->(); // this will bring the its_oid field up-to-date

  switch ( x -> its_oid().ccode() ) {
     case STRING_CODE:
       (*(pstring_handler*)x) -> update(buf, size);
       break;

     case COMPRESSED_STRING_CODE:
       (*(compressed_pstring_handler*)x) -> _asciiIn(buf, size);
       break;

     default:
       throw(stringException("invalid node data class code"));
  }
  delete x;
}

