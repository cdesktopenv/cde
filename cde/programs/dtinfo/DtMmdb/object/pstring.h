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
/*
 * $XConsortium: pstring.h /main/5 1996/07/18 14:45:25 drk $
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


#ifndef _pstring_h
#define _pstring_h 1

#include "object/primitive.h"
#include "utility/buffer.h"
#include "storage/page_storage.h"

/***************************************
* Primitive string class.
****************************************/

class pstring: public primitive
{

protected:

#ifdef C_API
   static buffer* v_io_buf_ptr;
#else
   static buffer v_io_buf;
#endif

protected:
   void _asciiIn(istream&) ; // get input to v_io_buf

public:
   pstring(pstring&);
   pstring(c_code_t = STRING_CODE);
   pstring(const char* string, int sz, c_code_t = STRING_CODE);
   virtual ~pstring();

   virtual int size() const { return v_sz; };

#ifdef C_API
   char* get(buffer& optional_buffer = *v_io_buf_ptr);
#else
   char* get(buffer& optional_buffer = v_io_buf);
#endif

   MMDB_SIGNATURES(pstring);

   void init_persistent_info(persistent_info*);

/*
// value comparison functions
   virtual Boolean value_EQ(root&, Boolean safe = true) const ;
   virtual Boolean value_LS(root&, Boolean safe = true) const ;
*/

// update function
   virtual Boolean update(pstring& new_value);
   virtual Boolean update(const char* new_value, int new_value_sz);

// print function
   virtual io_status asciiOut(ostream& s) ;
   virtual io_status asciiIn(istream&) ;
   virtual io_status asciiIn(const char* buf, int size) ;

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);

   friend class pstring_handler;

#ifdef C_API
   friend void initialize_MMDB();
   friend void quit_MMDB();
#endif

protected: // a union of disk/memory pointer
   union {
      mmdb_pos_t loc;
      char* p;
   } v_str_ptr;
   unsigned int v_sz;


private:
   void _init(const char*, int sz);

};
   
   
class pstring_handler : public handler
{

protected:

public:
   pstring_handler(const oid_t&, storagePtr = 0);
   pstring_handler(const char* str, int str_sz, storagePtr = 0);
   virtual ~pstring_handler();

   pstring* operator ->();
};


#endif
