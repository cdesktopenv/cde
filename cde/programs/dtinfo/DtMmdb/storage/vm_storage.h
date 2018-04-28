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
/* $XConsortium: vm_storage.h /main/4 1996/07/18 16:35:41 drk $ */

#ifndef _vm_storage_h
#define _vm_storage_h 1

#include "dstr/void_ptr_array.h"
#include "storage/abs_storage.h"

class vm_storage : public abs_storage
{
protected:
   void_ptr_array f_array;

public:
   vm_storage(char* path, char* name, rep_policy* rep_p = 0);
   ~vm_storage() ;

   void remove();

   // i/o functions
   int readString (mmdb_pos_t loc, char* base, int len, 
                  int str_offset = 0) ;
   int insertString(mmdb_pos_t& loc, const char* base, int len, Boolean flush = false) ;
   int get_str_ptr(mmdb_pos_t loc, char*&, int& len);

   int updateString(mmdb_pos_t loc, const char* base, int len, 
                    int string_ofst = 0, Boolean flush = false) ;
   int deleteString (mmdb_pos_t loc, Boolean flush = false) ;
   int allocString (mmdb_pos_t& loc, int len, char*&, int mode = 0);

   int appendString(mmdb_pos_t loc, const char*, int len,
                            Boolean flush_opt = false) ;

// status functions
   Boolean io_mode(int mode) ;

   friend class storage_mgr_t;

protected:
};

#endif
