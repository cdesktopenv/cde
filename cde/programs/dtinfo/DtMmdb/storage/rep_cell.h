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
 * $XConsortium: rep_cell.h /main/4 1996/08/21 15:53:50 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#ifndef _rep_cell_h
#define _rep_cell_h 1

#ifdef C_API
#include "utility/c_stream.h"
#else
#include <sstream>
#endif

#include "dstr/dlist.h"
#include "utility/debug.h"


enum position_t { ACTIVE, INACTIVE, NOWHERE };

class rep_cell : public dlist_cell 
{
public:
   rep_cell() : pos(NOWHERE) {};
   virtual ~rep_cell() ;

   position_t get_position() { return pos; };
   void       set_position(position_t x) { pos = x; };

protected:
   position_t pos;

};

class lru_cell_void_dt : public rep_cell
{

protected:

public:
   void* dt;

   lru_cell_void_dt(void* x = 0) : dt(x) {};
   virtual ~lru_cell_void_dt();
};


#endif
