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
 * $XConsortium: pstring.C /main/6 1996/09/16 14:26:44 mgreess $
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


#include "object/pstring.h"

#ifdef C_API
buffer* pstring::v_io_buf_ptr;
#define v_io_buf (*v_io_buf_ptr)
#else
buffer pstring::v_io_buf(LBUFSIZ);
#endif
   
void pstring::init_persistent_info(persistent_info* x)
{
   root::init_persistent_info(x);
   if ( get_mode(OLD_OBJECT) == false ) {
     char* y = 0;
     if ( storage_ptr )
       storage_ptr -> insertString(v_str_ptr.loc, y, 0);
   }
}

pstring::pstring(c_code_t c_id) : primitive(c_id),
   v_sz(0)
{
   //
   // CDExc21900
   // Make sure the entire union is initialized for 64bit architectures.
   //
   memset((char*) &v_str_ptr, 0, sizeof(v_str_ptr));
}

pstring::pstring(const char* x, int leng, c_code_t c_id) : 
primitive(c_id)
{
   _init(x, leng);
}

pstring::pstring(pstring& x) : primitive(x)
{
   _init((const char*)x.get(), x.v_sz);
}

void pstring::_init(const char* x, int leng) 
{
   v_sz = leng;

   if ( v_sz == 0 ) {
     v_str_ptr.loc = 0;
     return;
   }

   v_str_ptr.p = new char[leng+1]; 
   memcpy(v_str_ptr.p, x, leng);
   v_str_ptr.p[leng] = 0;
}

pstring::~pstring()
{
   if ( get_mode(PERSISTENT) == false && NULL != v_str_ptr.p ) 
      delete v_str_ptr.p;
}


// /////////////////////////////////////
// should not delete what get() returns!
// /////////////////////////////////////
char* pstring::get(buffer& string_buffer)
{
//MESSAGE(cerr, "get()");
   if ( get_mode(PERSISTENT) == true ) {

/*
MESSAGE(cerr, "persistent");
debug(cerr, v_sz);
debug(cerr, v_str_ptr.loc);
debug(cerr, f_oid);
*/

      string_buffer.reset();
      string_buffer.expand_chunk(v_sz+1);

      char* this_v_str_ptr = string_buffer.get_base();

      storage_ptr -> 
           readString(v_str_ptr.loc, this_v_str_ptr, v_sz);


/*
for (int i=0; i<v_sz; i++)
{
  cerr << int(this_v_str_ptr[i]) << " " ;
}
  cerr << "\n";
*/


      this_v_str_ptr[v_sz] = 0;

      string_buffer.set_content_sz(v_sz);

      return this_v_str_ptr;

   } else {

      return v_str_ptr.p;

   }
}


/*
Boolean pstring::value_LS(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != STRING_CODE ||
          x.my_oid().ccode() != STRING_CODE
        ) 
      ) 
     return false;

   Boolean ok;
   char* this_v_str_ptr;
   char* x_v_str_ptr;

   if ( get_mode(PERSISTENT) == true ) {
      this_v_str_ptr = new char[v_sz+1];
      storage_ptr -> readString(v_str_ptr.loc, this_v_str_ptr, v_sz);
   } else
      this_v_str_ptr = v_str_ptr.p;
      
   pstring &casted_x = *(pstring*)&x;

   if ( casted_x.get_mode(PERSISTENT) == true ) {
      x_v_str_ptr = new char[v_sz+1];
      storage_ptr -> 
          readString(casted_x.v_str_ptr.loc, x_v_str_ptr, casted_x.v_sz);
   } else
      x_v_str_ptr = casted_x.v_str_ptr.p;

   int min_len = MIN(v_sz, casted_x.v_sz);

   for ( int i=0; i<min_len; i++ ) {
         if ( this_v_str_ptr[i] < x_v_str_ptr[i] ) {
            ok = true;
            break;
          }

         if ( this_v_str_ptr[i] > x_v_str_ptr[i] ) {
            ok = false;
            break;
          }
    }

    if ( i == min_len && v_sz < casted_x.v_sz )
       ok = true;
    else
       ok = false;

    if ( get_mode(PERSISTENT) == true )
       delete this_v_str_ptr;

    if ( casted_x.get_mode(PERSISTENT) == true )
       delete x_v_str_ptr;

    return ok;
}

Boolean pstring::value_EQ(root& x, Boolean safe) const
{
   if ( safe == true &&
        ( f_oid.ccode() != STRING_CODE ||
           x.my_oid().ccode() != STRING_CODE
        ) 
      )
     return false;

   pstring &casted_x = *(pstring*)&x;

   if ( v_sz != casted_x.v_sz )
     return false;

   Boolean ok;
   char* this_v_str_ptr;
   char* x_v_str_ptr;

   if ( get_mode(PERSISTENT) == true ) {
      this_v_str_ptr = new char[v_sz+1];
      storage_ptr -> readString(v_str_ptr.loc, this_v_str_ptr, v_sz);
   } else
      this_v_str_ptr = v_str_ptr.p;
      
   if ( casted_x.get_mode(PERSISTENT) == true ) {
      x_v_str_ptr = new char[v_sz+1];
      storage_ptr -> 
          readString(casted_x.v_str_ptr.loc, x_v_str_ptr, casted_x.v_sz);
   } else
      x_v_str_ptr = casted_x.v_str_ptr.p;

   //ok = ( bcmp(this_v_str_ptr, x_v_str_ptr, v_sz) == 0 ) ? true : false;
   ok = ( memcmp(this_v_str_ptr, x_v_str_ptr, v_sz) == 0 ) ? true : false;

   if ( get_mode(PERSISTENT) == true )
      delete this_v_str_ptr;

   if ( casted_x.get_mode(PERSISTENT) == true )
      delete x_v_str_ptr;

    return ok;
}
*/

io_status pstring::asciiOut(ostream& out) 
{
/*
MESSAGE(cerr, "pstring::asciiOut():");
debug(cerr, v_sz);
debug(cerr, (void*)this);
my_oid().asciiOut(cerr);
cerr << "\n";
*/

   const char* x = get();

   out << v_sz << '\t';

   for ( unsigned int i=0; i<v_sz; i++ )
      out << x[i];

    return done;
}

io_status pstring::asciiIn(istream& in) 
{
/*
MESSAGE(cerr, "pstring::asciiIn():");
debug(cerr, (void*)this);
my_oid().asciiOut(cerr);
cerr << "\n";
*/

   _asciiIn(in);
   pstring::update(v_io_buf.get_base(), v_io_buf.content_sz());
   return done;
}

io_status pstring::asciiIn(const char* buf, int size) 
{
   v_io_buf.reset();
   v_io_buf.expand_chunk(size);
   memcpy(v_io_buf.get_base(), buf, size);
   v_io_buf.set_content_sz(size);
   return done;
}

void pstring::_asciiIn(istream& in) 
{
   if ( ! cc_is_digit(in) )
      throw (stringException("a digit expected"));

   int len;
   in >> len;

   int tab = in.get(); // expect a '\t'

   if ( tab != '\t' ) {
      debug(cerr, len);
      debug(cerr, tab);
      throw(stringException("'\\t' expected"));
   }

   v_io_buf.reset();
   v_io_buf.expand_chunk(len);

   if (  len > 0 && 
         ( !in.read(v_io_buf.get_base(), len) || in.gcount() != len ) 
      ) {
      debug(cerr, len);
      debug(cerr, v_io_buf.get_base());
      throw(stringException("pstring::asciiIn(): read failed"));
   }

   int ret = in.get(); // expect a '\n'

   if ( ret != '\n' ) {
      debug(cerr, ret);
      throw(stringException(form("'\\n' expected. %c seen. (count=%d)", ret, len)));
   }

   v_io_buf.set_content_sz(len);
}

Boolean pstring::update(pstring& new_value)
{
   return update((const char*)new_value.get(), new_value.v_sz);
}

Boolean pstring::update(const char* new_value, int new_value_sz)
{

/*
MESSAGE(cerr, "pstring:: update value:");
debug(cerr, v_str_ptr.loc);
debug(cerr, f_oid);
debug(cerr, v_sz);

debug(cerr, new_value_sz);
for (int i=0; i<new_value_sz; i++)
{
  cerr << int(new_value[i]) << " " ;
}
  cerr << "\n";
*/



   int old_sz = v_sz;
   v_sz = new_value_sz;

   if ( get_mode(PERSISTENT) == true ) {
//MESSAGE(cerr, "persist case");

      if ( v_str_ptr.loc == 0 ) {
         storage_ptr -> insertString(v_str_ptr.loc, new_value, v_sz);
      } else {
         storage_ptr -> updateString(v_str_ptr.loc, new_value, v_sz); 
      }

   } else  {

      if ( old_sz < new_value_sz ) {
         delete v_str_ptr.p;
         v_str_ptr.p = new char[v_sz+1];
      } 
      memcpy(v_str_ptr.p, new_value, v_sz);
      *(v_str_ptr.p + v_sz) = '\0';
   }

   set_mode(UPDATE, true);

//debug(cerr, this -> get());

   return true;
}


int pstring::cdr_sizeof()
{
   return primitive::cdr_sizeof() + 
          sizeof(v_str_ptr.loc) + 
          sizeof(v_sz);
}

io_status pstring::cdrOut(buffer& buf)
{
   primitive::cdrOut(buf);
   buf.put(v_str_ptr.loc);
   buf.put(v_sz);
   return done;
}

io_status pstring::cdrIn(buffer& buf)
{
   primitive::cdrIn(buf);
   buf.get(v_str_ptr.loc);
   buf.get(v_sz);
   return done;
}

MMDB_BODIES(pstring)

pstring_handler::pstring_handler(const oid_t& v_oid, storagePtr _store) :
handler(v_oid, _store)
{
}

pstring_handler::pstring_handler(const char* str, int sz, storagePtr _store) :
handler(STRING_CODE, _store)
{
   (this -> operator->()) -> update(str, sz);
}

pstring_handler::~pstring_handler()
{
}

pstring* pstring_handler::operator ->()
{
   return (pstring*)handler::operator->();
}


