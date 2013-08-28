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
 * $TOG: buffer.C /main/9 1998/04/17 11:50:56 mgreess $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#include "utility/buffer.h"

/***********************************************************/
// Constructor
/***********************************************************/
/*
buffer::buffer(const int sz)
{
   _alloc(sz);
}
*/

buffer::buffer(buffer& original): v_swap_order(original.v_swap_order)
{
   _alloc(original.v_bufsz);
   memcpy(v_aptr, original.v_aptr, original.v_bufsz);
}

void buffer::_alloc(const int sz)
{
   int void_ptr_size = sizeof(voidPtr);

   v_bufsz = sz;
   if ( sz > 0 ) {

      v_base = ::new char[void_ptr_size+sz];

      memset(v_base, (char)0, void_ptr_size+sz);

      v_align_offset = int(void_ptr_size - long(v_base) % void_ptr_size);
      if (v_align_offset != void_ptr_size)
	v_base += v_align_offset;
      else
	v_align_offset = 0;

      v_allocated = 1;

   } else {
      v_base = 0;
      v_allocated = 0;
   }
   v_aptr = v_eptr = v_base;
}


/***********************************************************/
// Destructor
/***********************************************************/
buffer::~buffer()
{
   if ( v_allocated == 1 ) {
     free((char*)(v_base-v_align_offset));
   }
}

void buffer::reset()
{
   v_eptr = v_base;
   v_aptr = v_base;
   memset(v_base, (char)0, v_bufsz);
}
   
/*
void buffer::set_chunk(char* ptr, const int sz)
{
   bufsz = sz;
   eptr = aptr = base = ptr;
   allocated = 0;
}

void buffer::set_content_sz(const int sz)
{
   eptr = base + sz;
}
*/

/***********************************************************/
// Expand buffer chunk to newsz.
/***********************************************************/
int buffer::expand_chunk(const int newsz)
{
   if ( v_allocated == 0 )
      throw(stringException("expand a buffer with reference memory"));

   int cursz = buf_sz();

   if ( newsz <= cursz ) {
      v_bufsz = newsz;
      return 0;
   }

   int void_ptr_size = sizeof(voidPtr);

//MESSAGE(cerr, "real expand");

   //char* x = (char*)malloc(void_ptr_size+newsz);
   char* x = ::new char [void_ptr_size+newsz];

   int delta = int(void_ptr_size - long(x) % void_ptr_size);
   x += delta ;

   memcpy(x, v_base, cursz);

   memset(x+cursz, char(0), newsz - cursz);

   v_aptr -= long(v_base);
   v_eptr -= long(v_base);

   //free((char*)(v_base-v_align_offset));
   delete (char*)(v_base-v_align_offset);
   v_align_offset = delta;

   v_base = x;
   v_aptr += long(v_base);
   v_eptr += long(v_base);
   v_bufsz = newsz;

   return 0;
}

#define CASTBNDEXCEPT

/***********************************************************/
// Get sz chars to the array x. x is supposed allocated
/***********************************************************/
buffer& buffer::get(char *x, int sz)
{
   if ( sz + v_aptr > v_eptr ) {
      MESSAGE(cerr, "buffer::get(): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(sz + v_aptr) ));
   }

   memcpy(x, v_aptr, sz);
   v_aptr += sz;
   return *this;
}

/***********************************************************/
// skip i chars.
/***********************************************************/
buffer& buffer::skip(int i)
{
   if ( i + v_aptr > v_eptr ) 
      MESSAGE(cerr, "buffer::skip(): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(i + v_aptr) ));

   v_aptr += i;
   return *this;
}


/***********************************************************/
// Get a char to y.
/***********************************************************/
buffer& buffer::get(char& y)
{
   if ( v_aptr > v_eptr) {
      MESSAGE(cerr, "buffer::get(char&): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(1+v_aptr)));
   }

   y = *v_aptr;
   v_aptr++;
   return *this;
}

buffer& buffer::getusc(int& y)
{
   y = *(unsigned char*)v_aptr;
   v_aptr++;
   return *this;
}

/***********************************************************/
// Get an integer to y. option can be ASCII or BINARY.
/***********************************************************/
buffer& buffer::get(int& y)
{
   unsigned int x;
   get(x);
   y = (int)x;
   return *this;
}

buffer& buffer::get(unsigned int& y)
{
   if ( v_aptr + sizeof(y) > v_eptr ) {
      MESSAGE(cerr, "buffer::get(int&): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(sizeof(unsigned)+v_aptr)));
   }

   get((char*)&y, sizeof(y));

#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_UINT(y);
#endif

   return *this;
}

/***********************************************************/
// Get a long to y. option can be ASCII or BINARY.
/***********************************************************/
buffer& buffer::get(long& y)
{
//MESSAGE(cerr, "WARNING: buffer::get(long& y) +++++++++++++++++++++++");
   if ( v_aptr + sizeof(y) > v_eptr ) {
      MESSAGE(cerr, "buffer::get(long&): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(sizeof(long)+v_aptr)));
   }

   get((char*)&y, sizeof(y));

#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_LONG(y);
#endif

   return *this;
}

/***********************************************************/
// Get a float to y. 
/***********************************************************/
buffer& buffer::get(float& y)
{
   if ( v_aptr + sizeof(y) > v_eptr ) {
      MESSAGE(cerr, "buffer::get(float &): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(sizeof(float)+v_aptr)));
   }

   get((char*)&y, sizeof(y));

#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_FLOAT(y);
#endif

   return *this;
}

buffer& buffer::get(unsigned short& y)
{
   if ( v_aptr + sizeof(y) > v_eptr ) {
      MESSAGE(cerr, "buffer::get(float &): underflow");
      throw ( CASTBNDEXCEPT boundaryException( long(v_aptr), long(v_eptr), long(sizeof(float)+v_aptr)));
   }

   get((char*)&y, sizeof(y));

#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_USHORT(y);
#endif

   return *this;
}

/***********************************************************/
// Put a char to buffer.
/***********************************************************/
buffer& buffer::put(const char content, Boolean exp_buf) 
{
   //return put((char*)&content, sizeof(content));

   if ( (int) v_bufsz == content_sz() )
   {
      if ( exp_buf == true )
        expand_chunk(v_bufsz + 10);
      else {
         MESSAGE( cerr, "buffer::put(const char): overflow");
         throw ( CASTBNDEXCEPT boundaryException(content_sz(), v_bufsz, 1) );
      }
   }

   *v_eptr = content;
   v_eptr++;
   return *this;
}

buffer& buffer::put(const unsigned char content, Boolean exp_buf) 
{
   return put((char*)&content, sizeof(content), exp_buf);
}

/***********************************************************/
// Put a unsigned int to buffer.
/***********************************************************/
buffer& buffer::put(const int content, Boolean exp_buf)
{
   return put((unsigned int)content, exp_buf);
}

buffer& buffer::put(const unsigned int content, Boolean exp_buf)
{
#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_UINT(content);
#endif

   return put((char*)&content, sizeof(content), exp_buf);
}

/***********************************************************/
// Put a long to buffer.
/***********************************************************/
buffer& buffer::put(const long content, Boolean exp_buf)
{
//MESSAGE(cerr, "WARNING: buffer::put(long& y) =====================");
#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_LONG(content);
#endif

   return put((char*)&content, sizeof(content), exp_buf);
}

/***********************************************************/
// Put a unsigned short to buffer.
/***********************************************************/
buffer& buffer::put(const unsigned short content, Boolean exp_buf)
{
#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_USHORT(content);
#endif

   return put((char*)&content, sizeof(content), exp_buf);
}

/***********************************************************/
// Put a float to buffer.
/***********************************************************/
buffer& buffer::put(const float content, Boolean exp_buf)
{
#ifdef PORTABLE_DB
   if ( v_swap_order == true )
      ORDER_SWAP_FLOAT(content);
#endif

   return put((char*)&content, sizeof(content), exp_buf);
}

/***********************************************************/
// Put sz chars to buffer.
/***********************************************************/
buffer& buffer::put(const char* content, int sz, Boolean exp_buf) 
{
   if ( sz > (int)(v_bufsz - content_sz()) ) {
      if ( exp_buf == true ) 
         expand_chunk(v_bufsz + sz);
      else {
         MESSAGE( cerr, "buffer::put(char*, int): overflow");
         throw ( CASTBNDEXCEPT boundaryException(content_sz(), v_bufsz, sz) );
      } 
   } 
      
   //memcpy(v_eptr, content, sz);

//debug(cerr, int(v_base));
//debug(cerr, int(v_eptr));

   for ( int i=0; i<sz; i++ ) {
     v_eptr[i] = content[i];
//debug(cerr, int(v_eptr[i]));
   }

   v_eptr += sz;
   return *this;
}

Boolean operator ==(buffer&x, buffer& y)
{
   if ( x.content_sz() != y.content_sz() ) {
      debug(cerr, x.content_sz());
      debug(cerr, y.content_sz());
   }

   unsigned char* x_buf = (unsigned char*)x.get_base();
   unsigned char* y_buf = (unsigned char*)y.get_base();

   for ( int i=0; i<x.content_sz(); i++ ) {
       if ( x_buf[i] != y_buf[i] ) {
          debug(cerr, i);
          debug(cerr, x_buf[i]);
          debug(cerr, y_buf[i]);
          //return false;
       }
   }

   return true;
}

/***********************************************************/
// show content
/***********************************************************/

ostream& operator<<(ostream& s, buffer& b)
{

   //debug(s, b.bufsz);
   //debug(s, b.content_sz());
   //debug(s, b.base);
   //debug(s, b.eptr);
   //debug(s, b.aptr);

   int x = b.v_eptr - b.v_base ;

   int i;
   for ( i = 0; i < x; i++ ) {
      s << b.v_base[i];

/*
      if ( isprint(b.v_base[i]) )
         cout << b.v_base[i];
      else
         cout << int(b.v_base[i]);
*/
   }

MESSAGE(cerr, "buffer=");
   for ( i = 0; i < x; i++ ) {
      cout << int(b.v_base[i]) << " ";
   }
cout << endl;

   return s;
}

/*
void buffer::cdrIn(buffer& buf)
{
   buf.get(v_bufsz);
   buf.get(v_base, v_bufsz);
}

void buffer::cdrOut(buffer& buf)
{
   buf.put(v_bufsz);
   buf.put(v_base, v_bufsz);
}
*/

