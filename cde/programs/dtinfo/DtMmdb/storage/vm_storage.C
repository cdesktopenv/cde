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
/* $XConsortium: vm_storage.cc /main/4 1996/07/18 16:05:10 drk $ */

#include "storage/vm_storage.h"
#include "utility/mmdb_exception.h"

vm_storage::vm_storage( char* _path, char* _name, rep_policy* rep_p):
  abs_storage( _path, _name, MEM_STORAGE_CODE, rep_p ), f_array(20)
{
}

vm_storage::~vm_storage()
{
   remove();
}

void vm_storage::remove() 
{
   for ( int i=0; i<f_array.no_elmts(); i++ ) {
     delete (buffer*)f_array[i];
     f_array.insert(0, i) ;
   }
   f_array.reset_elmts(0);
}

int vm_storage::appendString(mmdb_pos_t loc, const char*, int len,
                            Boolean flush_opt) 
{
   return -1;
}

int vm_storage::readString(mmdb_pos_t loc, char* base, int len, int str_offset) 
{
//MESSAGE(cerr, "vm: read");
//debug(cerr, loc);
//debug(cerr, len);
//debug(cerr, str_offset);

   char* x;
   int y;
   int ok = get_str_ptr(loc, x, y);

   if ( ok == 0 && str_offset+len <= y ) {
      memcpy(base, x+str_offset, len);
//MESSAGE(cerr, "========");
      return 0;
   } else
      return -1;
}

int vm_storage::insertString(mmdb_pos_t& loc, const char* base, int len, Boolean flush)
{
//MESSAGE(cerr, "vm: insert");
//debug(cerr, len);


   char* x;
   int ok = allocString (loc, len, x, 0);
//debug(cerr, loc);
//MESSAGE(cerr, "========");

   if ( ok == 0 ) {
      return updateString(loc, base, len, 0);
   } else
      return -1;
}

int vm_storage::get_str_ptr(mmdb_pos_t loc, char*& x, int& len)
{
//MESSAGE(cerr, "vm: get_str_ptr");
//debug(cerr, loc);

   if ( loc >= f_array.no_elmts() ) {
      debug(cerr, loc);
      debug(cerr, f_array.no_elmts());
      return -1;
   }

   buffer* b = (buffer*)f_array[(int)loc];

   if ( b == 0 ) {
     MESSAGE(cerr, "null pointer");
     return -1;
   }

   x = b -> get_base();
   len = b -> buf_sz();
//debug(cerr, (void*)x);
//debug(cerr, len);
//MESSAGE(cerr, "========");

   return 0;
}

int vm_storage::updateString(mmdb_pos_t loc, const char* base, int len, 
                    int string_ofst, Boolean flush) 
{
//MESSAGE(cerr, "updateString");
//debug(cerr, loc);
//debug(cerr, len);

   if ( loc >= f_array.no_elmts() ) {
      debug(cerr, loc);
      debug(cerr, f_array.no_elmts());
      return -1;
   }

   buffer* b = (buffer*)f_array[(int)loc];

   if ( b == 0 ) {
     MESSAGE(cerr, "null pointer");
     return -1;
   }


   if ( b -> buf_sz() < len + string_ofst )
      b -> expand_chunk(len + string_ofst);

/*
for (int i=0; i<len; i++)
  if ( isascii(base[i]) )
  cerr << base[i];
  else
  cerr << int(base[i]);
cerr << "\n";
MESSAGE(cerr, "========");
*/

   char* x = b -> get_base();
   memcpy(x+string_ofst, base, len);
   return 0;
}

int vm_storage::deleteString (mmdb_pos_t loc, Boolean flush) 
{
   return 0;
}

int vm_storage::allocString (mmdb_pos_t& loc, int len, char*& x, int mode)
{
//MESSAGE(cerr, "vm: allocate");
//debug(cerr, len);

   int c = f_array.no_elmts();

   if ( c >= f_array.count() ) {
      f_array.expandWith(10);
   }

   buffer* b = new buffer(len==0 ? 1 : len);
   f_array.insert(b, c) ;

   f_array.reset_elmts(c+1);

   x = b -> get_base();
   loc = c;
//debug(cerr, (void*)x);
//debug(cerr, loc);
//MESSAGE(cerr, "========");

   return 0;
}

Boolean vm_storage::io_mode(int mode)
{
   return true;
}

