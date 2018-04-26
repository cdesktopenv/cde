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
// $XConsortium: page_rep.cc /main/3 1996/06/11 17:44:32 cde-hal $

#include "storage/page_rep.h"

void page_rep_del(const void* x)
{
  delete (page_rep*)x;
}

Boolean page_rep_ls(const void* x, const void* y)
{
   if ( ((page_rep*)x) -> f_page_num < ((page_rep*)y) -> f_page_num )
      return true;
   else
      return false;
}

Boolean page_rep_eq(const void* x, const void* y)
{
   if ( ((page_rep*)x) -> f_page_num == ((page_rep*)y) -> f_page_num )
      return true;
   else
      return false;
}


