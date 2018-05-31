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
 * $XConsortium: memory_pool.h /main/3 1996/06/11 17:17:31 cde-hal $
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



#ifndef _memory_pool
#define _memory_pool 1

#include "utility/funcs.h"
#include "dstr/dlist_void_ptr_cell.h"
#include "dstr/dlist.h"
#include "dstr/void_ptr_array.h"

#define MAX_CHUNK_SZ 128

class chunk_carrier;

class chunk_manage_record_t : public dlist_cell
{

public:
   chunk_carrier* chunk_carrier_ptr;

   chunk_manage_record_t( chunk_carrier* chk_carr) :
      chunk_carrier_ptr(chk_carr) {};
   virtual ~chunk_manage_record_t() {};
};

typedef chunk_manage_record_t* chunk_manage_recordPtr;


class chunk_carrier
{

public:
   chunk_carrier(int chunk_sz, int chunks);
   virtual ~chunk_carrier();

   dlist* init_ptrs();

protected:
   int alloc_sz;
   int chunk_sz;
   int max_chunks;
   char* carrier_ptr;

   friend class fix_chunk_pool;
   friend class memory_pool;
};


class fix_chunk_pool 
{

public:
   fix_chunk_pool(int chunk_sz);
   virtual ~fix_chunk_pool();

// return char*
   virtual char* alloc();

// free a char*
   virtual void free(char*); 

protected:
   int chunk_sz;
   int chunks;
   dlist chunk_carrier_list;
   dlist free_chunk_list;

   void init_one_chunk_carrier();
};


typedef void_ptr_array vm_pool_array_t;

class memory_pool
{

public:
   memory_pool(int max_alloc_size = MAX_CHUNK_SZ);
   virtual ~memory_pool();

// return char*
   virtual char* alloc(size_t sz); 

// free a char*
   virtual void free(char*); 

protected:
   int max_alloc_size_from_pool;
   vm_pool_array_t vm_pool_vector; 
};

#ifdef C_API
#define g_memory_pool *g_memory_pool_ptr
extern memory_pool* g_memory_pool_ptr;
#endif

#endif
