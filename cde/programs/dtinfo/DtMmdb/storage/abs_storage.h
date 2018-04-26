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
 * $XConsortium: abs_storage.h /main/6 1996/07/18 14:52:29 drk $
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


#ifndef _abs_storage_h
#define _abs_storage_h 1

#include "storage/rep_policy.h"
#include "utility/buffer.h"
#include "utility/types.h"
#include "object/root.h"
#include "storage/version.h"

/*****************************************
// abstract storage class
// this class provides a set of common
// member function signatures and a policy
// pointer for all derived storage classes. 
*****************************************/

typedef enum
{
        mmdb_big_endian = 1,
        mmdb_little_endian = 2
} mmdb_byte_order_t;


class abs_storage : public root 
{
public:
   int index_num;

   abs_storage( char* path, char* name, 
                c_code_t c_id, 
                rep_policy* rep_p = 0
              );
   virtual ~abs_storage() ;

   void sync() {};
   virtual void remove() = 0; // wipe all contents

   // i/o functions
   virtual int readString (mmdb_pos_t loc, char* base, int len, 
                  int str_offset = 0) = 0;
   virtual int insertString(mmdb_pos_t& loc, const char* base, int len, Boolean flush = false) = 0;
   virtual int appendString(mmdb_pos_t loc, const char*, int len, 
			    Boolean flush_opt = false) = 0;


   virtual int get_str_ptr(mmdb_pos_t loc, char*&, int& len
                          )  = 0;

   virtual int updateString(mmdb_pos_t loc, const char* base, int len, 
                    int string_ofst = 0, Boolean flush = false) = 0;
   virtual int deleteString (mmdb_pos_t loc, Boolean flush = false) = 0;
   virtual int allocString (mmdb_pos_t& loc, int len, 
                            char*&, int mode = 0) = 0;
// status functions
   virtual const char* my_path() { return path; };
   virtual const char* my_name() { return name; };
   virtual Boolean io_mode(int mode) = 0;

// return byte order of the machine with one of enums in MMDB_BYTE_ORDER
   int byte_order();
   virtual Boolean swap_order() { return v_swap_order; };

   mm_version& get_db_version() { return f_version; };

   friend class storage_mgr_t;

protected:
   char path[PATHSIZ];
   char name[PATHSIZ];
   rep_policy *policy;
   Boolean v_swap_order;

   mm_version f_version;
};

typedef abs_storage* storagePtr;

#endif
