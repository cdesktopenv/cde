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
 * $XConsortium: trie.h /main/3 1996/06/11 17:15:31 cde-hal $
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


#ifndef _trie_h
#define _trie_h 1

#include "compression/code.h"
#include "dstr/heap.h"

#define MAX_LEVELS 50
#define LANG_ALPHABET_SZ 256

class trie_node;

struct info_t {
   unsigned freq:	23;
   unsigned letter:	8;
   unsigned mark:	1;
};

class trie_node_info 
{
   trie_node* child;
   union {
      struct info_t info_view;
      int int_view;
   } info;
   union {
      encoding_unit* eu;
      int heap;
      int pos;
   } image;

public:
   //trie_node_info (char letter = 0, int freq = 0, trie_node* child = 0);
   trie_node_info ();
   ~trie_node_info ();

   friend class trie_node;
   friend class trie;
   friend Boolean trie_node_ls(voidPtr n1, voidPtr n2);
   friend Boolean trie_node_eq(voidPtr n1, voidPtr n2);
   friend void update_index(int ind, void* x);
};

typedef trie_node_info* trie_node_info_ptr_t;

class trie_node 
{

protected:
#ifdef C_API
   trie_node_info_ptr_t* children; 
#else
   trie_node_info children[LANG_ALPHABET_SZ+1]; 
         // children[LANG_ALPHABET_SZ+1].child encodes the parent
#endif


public:
   trie_node(trie_node_info* parent);
   ~trie_node();

   void _print(ostream& out, char* prefix, int prefix_sz);

   friend class trie;
};

class trie 
{

protected:
   trie_node* root;
   int max_trie_level;
   int total_nodes;
   int level_sz[MAX_LEVELS];

   heap* sorted_freqs;

   encoding_unit** alphabet;
   unsigned int alphabet_sz;
   int  estimated_sz;

protected:
   void collect_freqs(trie_node* rt, int level);
   ostring* get_word(trie_node_info* x);

   void _find_leaf(trie_node* z, int& j);
   void extend_alphabet();

public:
   trie(int estimated_alphabet_sz = 400) ;
   virtual ~trie() ;

   virtual void add(unsigned char* word, int len, int freq = 1) ;
   virtual void add_letters(unsigned char* letters, int len) ;
   virtual encoding_unit* add_to_alphabet(unsigned char* word, int sz, int fq);

   virtual encoding_unit** get_alphabet(unsigned int& alphabet_sz);

   virtual Boolean travers_to(char* str, int sz, 
                              trie_node*& node, trie_node_info*& node_info
                             );

// take the longest substring from str and returns its
// encoding_unit.
   virtual encoding_unit* parse(unsigned char* str, int len);

   friend ostream& operator <<(ostream& out, trie& tr);
};


#endif
