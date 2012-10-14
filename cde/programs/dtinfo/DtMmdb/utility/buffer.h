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
 * $XConsortium: buffer.h /main/6 1996/08/15 14:21:05 cde-hal $
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


#ifndef _buffer_h
#define _buffer_h 1

#include <ctype.h>
#include <iomanip>
#include "utility/funcs.h"

class buffer 
{

public:
   buffer(const int sz = 1024) : v_swap_order(false) { _alloc(sz); };
   buffer(buffer&);
   virtual ~buffer();

// buffer management functions
   virtual void reset(); // clean the buffer
   int expand_chunk(const int);       // expand buffer chunk
   void set_chunk(char* ptr, const int sz) { 
      v_bufsz = sz;
      v_eptr = v_aptr = v_base = ptr;
      v_allocated = 0;
   };  // set buffer chunk
   void set_content_sz(const int sz) {v_eptr = v_base + sz;} ;

// buffer status functions
   int buf_sz() const { return v_bufsz; };           // chunk length
   int content_sz() const { return v_eptr - v_aptr; }; // content length 
   int remaining_sz() const { return v_bufsz - int(v_eptr - v_aptr); }; // remaining space
   char* get_base() const { return v_base; };  // get 'base'

// get items 
   buffer& get(char& y);                     // get current char
   buffer& get(unsigned short& y);  	     // get a short
   buffer& get(unsigned int& y);       	     // get a unsigned int
   buffer& get(int& y);       	     	     // get an int
   buffer& get(long& y);       	             // get a long
   buffer& get(float & y);       	     // get a float 
   buffer& get(char*, int) ;                 // get a string

   buffer& getusc(int& y);                   // get unsigned char to intr. 
					     // no boundary checking

// put items 
   buffer& put(const char content, Boolean exp_buf = false) ; // append char 
   buffer& put(const unsigned char, Boolean exp_buf = false) ; //append unsigned char 
   buffer& put(const int, Boolean exp_buf = false) ;           // append an int
   buffer& put(const unsigned int, Boolean exp_buf = false) ;  // append a unsigned int
   buffer& put(const long, Boolean exp_buf = false) ; 	       // append a long
   buffer& put(const unsigned short, Boolean exp_buf = false) ; //append a short
   buffer& put(const float y, Boolean exp_buf = false);       // put a float 
   buffer& put(const char*, int, Boolean exp_buf = false) ;   // append a string

   buffer& skip(int i) ;                       // skip i chars

// set and get byte order 
   void set_swap_order(Boolean x) { v_swap_order = x; };
   Boolean get_swap_order() { return v_swap_order; };

   friend Boolean operator ==(buffer&, buffer&);

// show the buffer
   friend ostream& operator <<(ostream&, buffer&);

   friend class abs_storage;
   friend class unixf_storage;
   friend class page_storage;
   friend class mem_storage;
   friend class page_cache_global_part;

protected:
   char v_allocated;
   unsigned int  v_bufsz ;   // allocated chunk size
   char *v_base ;  // base address of the chunk
   char *v_eptr ;  // end address of the buf content
   char *v_aptr ;  // beginning address of the buf content
   int  v_align_offset; // memory align offset

   Boolean v_swap_order;

protected:
   void _alloc(const int sz);
};

typedef buffer* bufferPtr;
   
#endif
