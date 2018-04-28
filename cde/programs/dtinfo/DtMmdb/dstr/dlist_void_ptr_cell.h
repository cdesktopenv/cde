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
/* $XConsortium: dlist_void_ptr_cell.h /main/3 1996/06/11 17:17:00 cde-hal $ */


#ifndef _dlist_void_ptr_cell_h
#define _dlist_void_ptr_cell_h 1

#include "utility/types.h"
#include "dstr/dlist_cell.h"

// doubly-linked list cell class, voidPtr as data ptr.

class dlist_void_ptr_cell : public dlist_cell {

public:
   dlist_void_ptr_cell(voidPtr vp) : data(vp) {};
   virtual ~dlist_void_ptr_cell() {};

   voidPtr void_ptr() { return data; };

protected:
   voidPtr data;
};

typedef dlist_void_ptr_cell *dlist_void_ptr_cellPtr;

#endif
