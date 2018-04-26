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
 * $XConsortium: bset.h /main/4 1996/06/11 17:16:30 cde-hal $
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


#ifndef _bset_h
#define _bset_h 1

#include  "dstr/set.h"
//#include  "storage/page_storage.h"

//**************************************************************
//
// a set implemented based on binary search tree data structure.
//
//**************************************************************

Boolean void_ls(const void* o1, const void* o2);
Boolean void_eq(const void* o1, const void* o2);

class bset;

struct bsetnode 
{
   void* v_element;
   bsetnode* v_left;
   bsetnode* v_right;

   bsetnode(void* x) : v_element(x), v_left(0), v_right(0)
   {};
   virtual ~bsetnode() { delete v_left; delete v_right; };
};

class bset : public set {

public:
   bset(cmp_func_ptr_t eq = void_eq, cmp_func_ptr_t ls = void_ls);
   virtual ~bset();

   virtual Boolean insert(void* element); // return false if element 
                                          // is already there
   virtual void* remove(void* element);
   virtual void* member(const void* element);

   virtual void* smaller_member(const void* element);
   virtual void* larger_member(const void* element);

// print elements as int.
   virtual ostream& asciiOut(ostream&);

   virtual void unlink_elements(); // unlink those v_elments
   virtual void del_elements(app_func_ptr_t func);
   virtual void apply(app_func_ptr_t);

protected:

   virtual void* _member(const void* element, bsetnode* rt);
   virtual Boolean _insert(void* element, bsetnode* rt);
   virtual void* _move(bsetnode* target, bsetnode* source);
   virtual void* _remove_min(void* element, bsetnode*& rt);
   virtual void*_remove(void* element, bsetnode*& rt);
   virtual void _del_elements(bsetnode* rt, app_func_ptr_t func);
   virtual void _apply(bsetnode* rt, app_func_ptr_t f);
   virtual void _unlink_elements(bsetnode* rt); 

   virtual void _smaller_member(const void* element, bsetnode* rt, void*& answer);
   virtual void* _larger_member(const void* element, bsetnode* rt);
   Boolean gt(const void* x, const void* y);

   virtual void _asciiOut(ostream&, bsetnode* rt);

protected:
   bsetnode* v_setroot;

};

typedef bset* bsetPtr;

#endif
