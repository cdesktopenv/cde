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
/* $XConsortium: page_rep.h /main/3 1996/06/11 17:44:37 cde-hal $ */

#ifndef _page_rep
#define _page_rep 1

#include "utility/types.h"

class lru_page;

//////////////////////////////////////////////////
// struct for managing page number/page ptr pair
//////////////////////////////////////////////////
class page_rep 
{

public:
   page_rep(int pnum, lru_page* ptr = 0) {
      f_page_ptr = ptr;
      f_page_num = pnum; 
   };
   virtual ~page_rep() {};

public:
   lru_page* f_page_ptr;
   int       f_page_num;

protected:
};

Boolean page_rep_ls(const void*x, const void* y);
Boolean page_rep_eq(const void*x, const void* y);
void page_rep_del(const void* x);

#endif
