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
 * $XConsortium: data_t.h /main/3 1996/06/11 17:19:11 cde-hal $
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


#ifndef _record_h
#define _record_h

#include "utility/types.h"
#include "utility/funcs.h"
#include "utility/buffer.h"
#include "dstr/memory_pool.h"
#include "utility/atoi_pearson.h"
#include "utility/atoi_larson.h"
#include "dstr/dlist_cell.h"

/****************************/
// record class. A record
// contains a key and a void*
// pointer         
/****************************/

class data_t : public dlist_cell
{

protected:

#ifdef C_API
   static atoi_pearson* pearson_convertor_ptr;
   static atoi_larson* larson_convertor_ptr;
#define pearson_convertor (*pearson_convertor_ptr)
#define larson_convertor (*larson_convertor_ptr)
#else
   static atoi_pearson pearson_convertor;
   static atoi_larson larson_convertor;
#endif

public:
   enum flag_type { INT=0, STRING=1, VOID=2 };

   voidPtr dt;     // rest of information in the record
   flag_type flag;
   union {
      int int_key;
      char* str_key;
   } key;

   data_t(): dt(0), flag(data_t::VOID) {} ;
   data_t(data_t&) ;
   data_t(int int_key, voidPtr d = 0) : dt(d), flag(data_t::INT)
   { key.int_key = int_key; };

   data_t(const char* char_ptr_key, int char_ptr_key_sz, voidPtr d = 0) ;
   virtual ~data_t() ;

   data_t& operator=(data_t&);
   int operator==(data_t&);

   virtual int bucket_num(int k, int p, int M ) ;
   virtual int slot_num(int k, int rotate, int prime, int M ) ;

   ostream& asciiOut(ostream& out, print_func_ptr_t print_f);

   Boolean binaryIn(buffer&);
   Boolean binaryOut(buffer&);

   void* operator new( size_t );
   void operator delete( void* );

   friend ostream& operator <<(ostream& o, data_t& d);
   friend istream& operator >>(istream& i, data_t& d);

#ifdef C_API
   friend void initialize_MMDB();
   friend void quit_MMDB();
#endif

};

typedef data_t* data_tPtr;

#endif
