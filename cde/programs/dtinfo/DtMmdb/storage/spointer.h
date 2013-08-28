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
/* $XConsortium: spointer.h /main/6 1996/07/18 16:35:10 drk $ */

#ifndef _spointer_h
#define _spointer_h 1

////////////////////////////////////////////////////////////
// Macros to manipulate page number and item number components.
//
//  Variable definitions:
//	x: compacted representation of page id and item 
//         index in the page;
//      y: page size;
//      u: page id;
//      v: item index in the page;
////////////////////////////////////////////////////////////
#define PAGE_ID(x, y)  ( unsigned(x) >> pos_of_LSB(y) )
#define PAGE_IDX(x, y)  ( unsigned(x) & ~(~0 << pos_of_LSB(y)) )
#define FORM_PAGE_REF(u, v, y) ( ((u) << pos_of_LSB(y)) | PAGE_IDX((v), (y)) )


////////////////////////////////////////////////////////////
// .  page class. group of pages are under LRU management;
// .  use buffer to hold page content (implementation);
// .  LRU implementation is made possible by defining 'lru' 
//    as one of the base class.
////////////////////////////////////////////////////////////


#define HEADER_MASK 0xf

struct header_t 
{
#ifdef MMDB_BIG_ENDIAN
   unsigned spointer    : 14;
   unsigned length      : 14;
   unsigned is_object   : 1;
   unsigned updated	: 1;
   unsigned first_recd  : 1;
   unsigned deleted     : 1;
#else
   unsigned deleted     : 1;
   unsigned first_recd  : 1;
   unsigned updated     : 1;
   unsigned is_object   : 1;
   unsigned length      : 14;
   unsigned spointer    : 14;
#endif
};

class spointer_t 
{

public:

   enum spointer_mode { IS_OBJECT=8, 
                        UPDATED=4,
                        FIRST_RECD=2, 
                        DELETED=1 
                      };

   spointer_t(char* page_base, int slot_num);
   spointer_t(char* page_base, int slot_num, int ofst, int len); 
   ~spointer_t();

   void set_mode(spointer_mode, Boolean);
   Boolean get_mode(spointer_mode);
   void add_mode(int m) { 
      header.int_view |= (HEADER_MASK & m); 
      set_mode(UPDATED, true);
   };
   Boolean test_mode(unsigned int m) {
      return ( (header.int_view & HEADER_MASK & m) == m ) ? true : false;
   };

   mmdb_pos_t forward_ptr() { return fwd_ptr; };
   int string_leng() {
      return int(header.bit_view.length);
   };

   int string_ofst() {
      return int(header.bit_view.spointer);
   };

   void set_string_leng(int l) {
      header.bit_view.length = l;
      set_mode(UPDATED, true);
   };
   void set_string_ofst(int ofst) {
      header.bit_view.spointer = ofst; 
      set_mode(UPDATED, true);
   };

   void set_forward_ptr(mmdb_pos_t ptr) { 
      fwd_ptr = ptr; 
      set_mode(UPDATED, true);
   };

   void swap_order() {
#ifdef __osf__
      ORDER_SWAP_INT(fwd_ptr);
#else
      ORDER_SWAP_LONG(fwd_ptr);
#endif
      ORDER_SWAP_UINT(header.int_view);
      swapped = true;
   };

   friend class page;
   friend class page_storage;

protected:
   mmdb_pos_t fwd_ptr;
   union {
      header_t bit_view;
      unsigned int int_view;
   } header;

   char* page_image;
   Boolean swapped;
};

#endif
