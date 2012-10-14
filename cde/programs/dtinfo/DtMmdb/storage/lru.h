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
 * $XConsortium: lru.h /main/5 1996/08/21 15:53:46 drk $
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


#ifndef _lru_h
#define _lru_h 1

#ifdef C_API
#include "utility/c_stream.h"
#else
#include <sstream>
#endif

#include "utility/debug.h"
#include "dstr/dlist.h"
#include "storage/rep_policy.h"

class lru: public rep_policy {

protected:
   dlist active_list;
   dlist inactive_list;

protected:
   void lower_last(rep_cell*& replaced);

public:
   lru(int a_size, int i_size, Boolean remove_cells) ;
   virtual ~lru() ;

// promotes the cell x to the active list.
// In the first version, the replaced is the cell dumped from the
// active list. It is set to zero if no dumping takes place.
// Boolean = true: replaced is saved in the inactive list

   virtual Boolean promote(rep_cell& x, rep_cell*& replaced);
   virtual Boolean promote(rep_cell& x);

// put x to the last of position_t list. Only from
//    somewhere in active list -> active list's last
//       or
//    somewhere in active list -> inactive list's last
//
// not possible:
//    somewhere in inactive list -> active list's last
//

   virtual Boolean derank(rep_cell& x, position_t);

// walk through all cells in either the active or inactive list, 
   long first(position_t = ACTIVE);
   rep_cell* operator()(long index, position_t);
   rep_cell* operator()(long index) { return operator()(index, ACTIVE); };
   void next(long & index, position_t = ACTIVE);

// the last cell in either of the list
   long last(position_t = ACTIVE);

   Boolean remove(rep_cell& x); // remove a cell x
   void remove();		// remove all cells
   void forget();		// let go all cells, do nothing on them

// counts
   int active_elmts()   { return active_list.count();   };
   int inactive_elmts() { return inactive_list.count(); }; 

   dlist& get_active_list()   { return active_list;   };
   dlist& get_inactive_list() { return inactive_list; }; 
};

typedef lru *lruPtr;

#endif
