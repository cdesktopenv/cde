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
 * $XConsortium: slist.h /main/4 1996/07/18 14:30:30 drk $
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



#ifndef _slist_h
#define _slist_h 1

#include <stdarg.h>
#include "utility/debug.h"
#include "dstr/slist_cell.h"

// single-linked list class

class slist {

public:
   slist(slist_cell* x = 0);
   virtual ~slist();

// append a slist. tail_list becomes part of this list. no copy
// is performed. tail_list -> head and tail_list -> tail 
// are set to NULL.  Also, tail_list -> ct = 0.
   void append(slist* tail_list);

// generalization of append(). arguments should be of type slist*. 
// this is returned
   slist* concate_with(slist* ...);


// update functions
   void insert_as_tail(slist_cell* x) ;

   void delete_head() ;
   void delete_tail() ;

// empty the list without free cells
   void empty_list() { v_head = v_tail = 0; v_ct = 0;} ;

// status function
   int count() { return v_ct; }; // number of cells in the list
   slist_cell* get_head() { return v_head; };
   slist_cell* get_tail() { return v_tail; };

   long first();   // 0 if the list is empty
   void next(long & index);
   long last();    // 0 if the list is empty

protected:
   int v_ct;            // cell in the list
   slist_cell *v_head;  // head pointer
   slist_cell *v_tail;  // tail pointer

};

typedef slist *slistPtr;

#endif
