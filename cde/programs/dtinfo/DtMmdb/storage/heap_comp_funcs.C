/*
 * $XConsortium: heap_comp_funcs.cc /main/3 1996/06/11 17:33:36 cde-hal $
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


#include "storage/heap_comp_funcs.h"

//memory_pool fbytes_t::heap_cell_space_pool;

#ifndef C_API
memory_pool g_memory_pool;
#endif

Boolean fbytes_t_ls(const void* x, const void* y)
{
   if ( ((fbytes_t*)x) -> free_bytes < ((fbytes_t*)y) -> free_bytes )
      return true;
   else
      return false;
}

Boolean fbytes_t_eq(const void* x, const void* y)
{
   if ( ((fbytes_t*)x) -> free_bytes == ((fbytes_t*)y) -> free_bytes )
      return true;
   else
      return false;
}

void* fbytes_t::operator new( size_t x )
{
   //return (void*)heap_cell_space_pool.alloc(x);
   return (void*)g_memory_pool.alloc(x);
}

void fbytes_t::operator delete( void* ptr )
{
   //heap_cell_space_pool.free((char*)ptr);
   g_memory_pool.free((char*)ptr);
}

