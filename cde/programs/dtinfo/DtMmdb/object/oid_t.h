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
 * $XConsortium: oid_t.h /main/6 1996/08/21 15:52:31 drk $
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


#ifndef _oid_t_h
#define _oid_t_h 1

#ifdef C_API
#include "utility/c_stringstream.h"
#else
#include <sstream>
#endif

#include "utility/funcs.h"
#include "utility/buffer.h"

typedef unsigned short e_code_t;
typedef unsigned short c_code_t;
typedef mmdb_pos_t i_code_t;

#define OID_T_SZ (sizeof(i_code_t))

/*************************************
// class code root
**************************************/

class oid_t 
{
public:
   oid_t() : v_c_code(0), v_i_code(0), v_e_code(0)  {};
   oid_t(c_code_t c, i_code_t i) : v_c_code(c), v_i_code(i), v_e_code(0) {};
   oid_t(const char* source, Boolean ascii_format, Boolean swap_order);
   oid_t(const oid_t& x) : 
      v_c_code(x.v_c_code), v_i_code(x.v_i_code), v_e_code(x.v_e_code) {};
   ~oid_t() {};

// oid_t equal and less test
   Boolean eq(const oid_t&) const;
   Boolean ls(const oid_t&) const;
   bool  operator==(const oid_t& arg) const { return eq(arg); };

// class code, oid type and instance code export functions
   const c_code_t ccode() const { return v_c_code; } ;
   const i_code_t& icode() const { return v_i_code; } ;

   static unsigned hash(const oid_t&);

// in/out functions
   io_status asciiIn(istream&) ;
   io_status asciiOut(ostream&) const;

   io_status _asciiIn(istream&) ; // a version of asciiIn that does not
                                  // eat the trailing '\n'


   friend ostream& operator<<(ostream&, const oid_t&) ;

// compacted disk representation In and Out functions
   int cdr_sizeof();
   io_status cdrOut(buffer&);
   io_status cdrIn(buffer&, c_code_t c_code_to_use = 0);

// out to char strings
   void to_char_string(char* sink, Boolean swap_order) const;

   friend class root;
   friend class oid;
   friend class desc;
   friend class dl_list_cell;
   friend class dl_list;
   friend class compressed_pstring;
   friend class service_mgr_t;
   friend class storage_mgr_t;
   friend class handler;
   friend class smart_ptr;
   friend class template_mgr_t;

protected:
   void become(const oid_t& x) {
      v_e_code = x.v_e_code;
      v_c_code = x.v_c_code;
      v_i_code = x.v_i_code;
   };
   void set_c_code(c_code_t c) {
      v_c_code = c;
   }
   void set_i_code(i_code_t i) {
      v_i_code = i;
   }
   void set_e_code(e_code_t e) {
      v_e_code = e;
   }

protected:
   c_code_t v_c_code; // class code
   i_code_t v_i_code; // instance code
   e_code_t v_e_code; // extended code. Not used within this class.
                      // for using extra space purpose.
};

typedef oid_t* oid_tPtr;

#ifdef C_API
extern oid_t* ground_ptr;
#define ground (*ground_ptr)
#else
extern oid_t ground;
#endif

#endif

