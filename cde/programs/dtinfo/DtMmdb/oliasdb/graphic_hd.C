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
 * $XConsortium: graphic_hd.cc /main/4 1996/06/11 17:28:57 cde-hal $
 * $XConsortium: graphic_hd.cc /main/4 1996/06/11 17:28:57 cde-hal $
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


#include "oliasdb/graphic_hd.h"

#ifdef C_API
buffer* graphic_smart_ptr::local_graphic_buffer_ptr = 0;
#endif

MMDB_BODIES(graphic)

void
graphic_smart_ptr::init () {
    f_type = 0;
    f_width = 0;
    f_height = 0;
    f_coding = 0;
    f_llx = 0;
    f_lly = 0;
    f_urx = 0;
    f_ury = 0;
}

graphic_smart_ptr::
graphic_smart_ptr(info_lib* lib_ptr, const char* ibase_name, const char* locator) : 
smart_ptr(lib_ptr, ibase_name, GRAPHIC_SET_POS, 
                 managers::query_mgr -> form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                ),
  _converted(false)
{
    init ();
}

graphic_smart_ptr::
graphic_smart_ptr(info_base* base_ptr, const char* locator)
: smart_ptr(base_ptr, GRAPHIC_SET_POS, 
	    managers::query_mgr -> form_pstring_handler(locator),
	    BASE_COMPONENT_INDEX, smart_ptr::SET
	    ),
  _converted(false)
{
   //debug(cerr, locator);
    init ();
}

graphic_smart_ptr::
graphic_smart_ptr(info_base* base_ptr, const oid_t& id)
: smart_ptr(base_ptr -> get_set(GRAPHIC_SET_POS) -> its_store(), id),
  _converted(false)
{
    init ();
}

///////////////////////////////
//
///////////////////////////////

const char* graphic_smart_ptr::locator()
{
  return get_string(BASE_COMPONENT_INDEX);
}

const char* graphic_smart_ptr::file_name()
{
  return get_string(BASE_COMPONENT_INDEX+1);
}

const char* graphic_smart_ptr::version()
{
  return get_string(BASE_COMPONENT_INDEX+2);
}

Boolean graphic_smart_ptr::_convert_to_ints()
{
   pstring_handler* x = (pstring_handler*)get_handler(BASE_COMPONENT_INDEX+3, STRING_CODE);

   char* y = (*x)->get();
   int ct = (*x) -> size();

   int dots = 0;
   for ( int i=0; i<ct; i++ ) 
      if ( y[i] == '.' )
         dots++;

   switch (dots) {
     case 3:
      sscanf(y, "%d.%hu.%hu.%hu", &f_type, &f_coding, &f_width, &f_height);
      break;
   
     case 5:
      sscanf(y, "%d.%hu.%u.%u.%u.%u",
             &f_type, &f_coding, &f_llx, &f_lly, &f_urx, &f_ury);
//debug(cerr, f_llx);
//debug(cerr, f_lly);
//debug(cerr, f_urx);
//debug(cerr, f_ury);
      break;

    default:
      throw(stringException("wrong number of dots"));
  }

   _converted = true;

   delete x;
   return true;
}

#ifdef C_API
#define local_graphic_buffer (*local_graphic_buffer_ptr)
#else
static buffer local_graphic_buffer(LBUFSIZ);
#endif


const char* graphic_smart_ptr::data()
{
  return get_string(BASE_COMPONENT_INDEX+4, local_graphic_buffer);
}

int graphic_smart_ptr::data_size()
{
  return get_string_size(BASE_COMPONENT_INDEX+4);
}

const char* graphic_smart_ptr::title()
{
  mtry
  {
     return get_string(BASE_COMPONENT_INDEX+5);
  }

  mcatch (mmdbException&,e)
  {
     return 0;
  } 
  end_try;
}

int graphic_smart_ptr::type() const
{
   if ( _converted == false )
     // cast away to achieve logical constness
     ((graphic_smart_ptr*)this)->_convert_to_ints();

   return f_type;
}

unsigned short graphic_smart_ptr::coding()
{
   if ( _converted == false )
     _convert_to_ints();

   return f_coding;
}

unsigned short graphic_smart_ptr::width()
{
   if ( _converted == false )
     _convert_to_ints();

   return f_width;
}

unsigned short graphic_smart_ptr::height()
{
   if ( _converted == false )
      _convert_to_ints();

   return f_height;
}

/////////////////////////////////////////////
unsigned int graphic_smart_ptr::llx()
{
   if ( _converted == false )
      _convert_to_ints();

   return f_llx;
}

unsigned int graphic_smart_ptr::lly()
{
   if ( _converted == false )
      _convert_to_ints();

   return f_lly;
}

unsigned int graphic_smart_ptr::urx()
{
   if ( _converted == false )
      _convert_to_ints();

   return f_urx;
}

unsigned int graphic_smart_ptr::ury()
{
   if ( _converted == false )
      _convert_to_ints();

   return f_ury;
}

