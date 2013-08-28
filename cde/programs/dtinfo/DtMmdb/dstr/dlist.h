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
 * $XConsortium: dlist.h /main/4 1996/07/18 14:29:10 drk $
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



#ifndef _dlist_h
#define _dlist_h 1

#include "utility/debug.h"
#include "utility/types.h"
#include "dstr/dlist_cell.h"

// doubly-linked list class

class dlist {

public:
   dlist(Boolean remove_cells = true);
   virtual ~dlist(); 

// append a dlist. x becomes part of this list. no copy
// is performed. tail_list -> head and tail_list -> tail 
// are set to NULL.  Also, tail_list -> ct = 0.
   void append(dlist* tail_list);

// update functions
   void insert_before(dlist_cell* ref,  dlist_cell* x) ; //insert before ref
   void insert_after(dlist_cell* ref, dlist_cell* x) ; // insert after ref
   void insert_as_head(dlist_cell* x);  // insert x at head
   void insert_as_tail(dlist_cell* x);  // insert x at tail

   void delete_cell(dlist_cell*) ;
   void remove() ; // remove all cells (call delete on each cell)

// empty the list without free cells
   void empty_list() { v_head = v_tail = 0; v_ct = 0;} ;

// status function
   int count() { return v_ct; }; // number of cells in the list
   dlist_cell* get_head() { return v_head; };
   dlist_cell* get_tail() { return v_tail; };

   long first();   // 0 if the list is empty
   void next(long& index);
   long last();    // 0 if the list is empty

protected:
   int v_ct;            // cell in the list
   dlist_cell *v_head;  // head pointer
   dlist_cell *v_tail;  // tail pointer
   int remove_cells_when_done;
};

typedef dlist *dlistPtr;

#endif
