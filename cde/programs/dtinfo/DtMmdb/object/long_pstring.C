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
 * $XConsortium: long_pstring.cc /main/4 1996/06/11 17:24:42 cde-hal $
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


#include "object/long_pstring.h"


/****************************************************/
// long_pstring member funcions: CONS/DEST
/****************************************************/

long_pstring::long_pstring(c_code_t c_id) : pstring(c_id), v_str_index(0)
{
    set_mode(SWAP_ALLOWED, false); 
}
   

long_pstring::long_pstring(char* x, int leng, c_code_t c_id) : pstring(x, leng, c_id), v_str_index(0)
{
}

long_pstring::long_pstring(long_pstring& x) : pstring(x)
{
   v_str_index = 0;
}

long_pstring::~long_pstring()
{
   delete v_str_index;
}

/****************************************************/
// long_pstring member funcions: OTHERS
/****************************************************/
Boolean 
long_pstring::extract(int left, int right, char* sink)
{
//debug(cerr, left);
//debug(cerr, right);
//debug(cerr, int(v_str_ptr.loc));

   if ( left > right || right - left > (int) v_sz ) {
      throw(boundaryException(left, right, v_sz));
   }
  
   if ( get_mode(PERSISTENT) == true ) {

   if ( v_str_index == 0 && 
        get_mode(OLD_OBJECT) == true && v_sz > PAGSIZ 
      )
      v_str_index = new chunks_index(storage_ptr, v_str_ptr.loc);

      if ( v_str_index ) {

         str_index_record_t* anchor = v_str_index -> chunk_location(left);

//debug(cerr, left - anchor -> str_offset);
         storage_ptr -> 
             readString(anchor -> loc, sink, 
                        right-left, left - anchor -> str_offset
                       );

      } else 

//debug(cerr, left);
         storage_ptr -> readString(v_str_ptr.loc, sink, right-left, left);


   } else {
      memcpy(sink, v_str_ptr.p + left, right - left);
   }

//for ( int i=0;i<right-left; i++)
//  cerr << int(sink[i]) << " ";
//cerr << "\n";

   return true;
}


/*
Boolean long_pstring::value_LS(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != LONG_STRING_CODE ||
          x.my_oid().ccode() != LONG_STRING_CODE
        ) 
      ) 
     return false;

   return pstring::value_LS(x, false);
}

Boolean long_pstring::value_EQ(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != LONG_STRING_CODE ||
           x.my_oid().ccode() != LONG_STRING_CODE
        ) 
      )
     return false;
   
   return pstring::value_EQ(x, false);
}
*/

io_status long_pstring::asciiIn(istream& in) 
{
  if ( pstring::asciiIn(in) == done ) {
     v_str_index = ( get_mode(PERSISTENT) == true && v_sz > PAGSIZ ) ?
                 new chunks_index(storage_ptr, v_str_ptr.loc) : 
                 0;
     return done;
  } else
     return fail; 
}

MMDB_BODIES(long_pstring)
HANDLER_BODIES(long_pstring)
