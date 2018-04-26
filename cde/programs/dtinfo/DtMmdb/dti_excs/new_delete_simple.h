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
/* $XConsortium: new_delete_simple.h /main/3 1996/06/11 16:52:04 cde-hal $ */


#ifndef _new_delete_simple_h
#define _new_delete_simple_h 1


#define NEW_AND_DELETE_SIGNATURES(class_name) \
   void* operator new( size_t ); \
   void* operator new( size_t, void* ); \
   void operator delete( void* ) 

#ifdef C_API
#include <stdlib.h>

#define NEW_AND_DELETE_BODIES_SIMPLE(class_name) \
\
void* class_name::operator new( size_t sz )\
{\
   return (void*)malloc(sz); \
}\
\
void* class_name::operator new( size_t sz, void* ptr )\
{\
   return (void*)ptr; \
}\
\
void class_name::operator delete( void* p )\
{\
   ::operator delete(p); \
}


#else
#include <new>
using namespace std;
#endif


#endif


