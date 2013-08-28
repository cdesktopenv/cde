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
 * $XConsortium: trie.cc /main/3 1996/06/11 17:15:26 cde-hal $
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


#include "compression/trie.h"

//trie_node_info::trie_node_info (char c, int f, trie_node* x)

trie_node_info::trie_node_info () : child(0)
{
   info.int_view = 0;
   image.heap = 0;
}

trie_node_info::~trie_node_info ()
{
   delete child;
}


trie_node::trie_node(trie_node_info* parent)
{
#ifdef C_API
   children = new trie_node_info_ptr_t[LANG_ALPHABET_SZ+1];
   for (int i=0; i<LANG_ALPHABET_SZ+1; i++ )
      children[i] = new trie_node_info;

   children[LANG_ALPHABET_SZ] -> child = (trie_node*)parent;
#else
   children[LANG_ALPHABET_SZ].child = (trie_node*)parent;
#endif
}

trie_node::~trie_node()
{
#ifdef C_API
   for (int i=0; i<LANG_ALPHABET_SZ+1; i++ )
      delete children[i];

   delete children;
#else
   children[LANG_ALPHABET_SZ].child = 0;
#endif
}

void trie_node::_print(ostream& out, char* prefix, int prefix_sz)
{
#ifdef C_API
   for ( int i=0; i<LANG_ALPHABET_SZ; i++ ) {
     if ( children[i] -> info.info_view.freq > 0 ) {

        prefix[prefix_sz] = children[i] -> info.info_view.letter;
        for (int j=0; j<prefix_sz; j++)
           out << prefix[j];

        out << char(children[i] -> info.info_view.letter);

        out << ":" << int(children[i] -> info.info_view.freq) << "\n";

        prefix[prefix_sz] = children[i] -> info.info_view.letter;

        if ( children[i] -> child )
           children[i] -> child -> _print(out, prefix, prefix_sz+1);
     }
   }
#else
   for ( int i=0; i<LANG_ALPHABET_SZ; i++ ) {
     if ( children[i].info.info_view.freq > 0 ) {

        prefix[prefix_sz] = children[i].info.info_view.letter;
        for (int j=0; j<prefix_sz; j++)
           out << prefix[j];

        out << char(children[i].info.info_view.letter);

        out << ":" << int(children[i].info.info_view.freq) << "\n";

        prefix[prefix_sz] = children[i].info.info_view.letter;
 
        if ( children[i].child )
           children[i].child -> _print(out, prefix, prefix_sz+1);
     }
   }
#endif
}

//////////////////////////////////////
//
//////////////////////////////////////

trie::trie(int estimatedsz) :
   root(0), max_trie_level(0), total_nodes(1), 
   sorted_freqs(0), alphabet_sz(0),  estimated_sz(estimatedsz)
{
   alphabet = new encoding_unitPtr[ estimated_sz];
   for ( int i=0; i< estimated_sz; alphabet[i++] = 0 );
}

trie::~trie()
{
   delete root;
   delete sorted_freqs;

   for ( unsigned int i=0; i<alphabet_sz; i++ )
      delete alphabet[i];

   delete alphabet;
}

void trie::extend_alphabet()
{
   if ( (int) alphabet_sz >=  estimated_sz ) {
     encoding_unitPtr* new_alphabet = new encoding_unitPtr[2* estimated_sz];

     for ( int k=0; k< estimated_sz; k++ ) {
        new_alphabet[k] = alphabet[k] ;
        new_alphabet[k + estimated_sz] = 0;
     }

     new_alphabet = new_alphabet;
     estimated_sz *= 2;
  }
}

void trie::add(unsigned char* word, int sz, int fq)
{
//cerr << sz << " ";
//for ( int k=0; k<sz; k++ )
//  cerr << word[k];
//cerr << "\n";

   static int j;
   static trie_node* x = 0;
   static trie_node_info* y = 0;

   if ( root == 0 )
      root = new trie_node(0);

   x = root;

   for ( int i=0; i<sz; i++ ) {

      j = word[i];

#ifdef C_API
      y = (x -> children[j]);
#else
      y = &(x -> children[j]);
#endif

      y -> info.info_view.letter = j;

      if ( i<sz-1 && y -> child == 0 ) {
         y -> child = new trie_node(y);
         total_nodes++;
      }

      if ( i == sz-1 ) {

          y -> info.info_view.freq += fq;

          if ( y -> image.eu == 0 ) {

             y -> info.info_view.mark = 1;

             y->image.eu = add_to_alphabet(word, sz, fq);

          } else
             y -> image.eu -> freq += fq;
      }

      x = y -> child;
   }
}
             
encoding_unit* trie::add_to_alphabet(unsigned char* word, int sz, int fq)
{
   extend_alphabet();
   encoding_unit *x = new encoding_unit(new ostring((char*)word, sz), fq);
   alphabet[alphabet_sz++] = x;
   return x;
}

void trie::add_letters(unsigned char* letters, int sz)
{
/*
MESSAGE(cerr, "trie::add_letters()");
cerr << sz << " ";
for ( int k=0; k<sz; k++ )
  cerr << letters[k];
cerr << "\n";
*/

   static int j;
   static trie_node_info* y = 0;

   static char buf[1];
   static ostring *z;

   if ( root == 0 )
      root = new trie_node(0);

   for ( int i=0; i<sz; i++ ) {

      j = letters[i];

#ifdef C_API
      y = (root -> children[j]);
#else
      y = &(root -> children[j]);
#endif
      y -> info.info_view.freq ++;

      if ( y -> image.eu == 0 ) {

         y -> info.info_view.letter = j;

         buf[0] = char(j);
         z = new ostring(buf, 1);
         y -> info.info_view.mark = 1;

         extend_alphabet();

         alphabet[alphabet_sz++] = y->image.eu =new encoding_unit(z, 1);
      } else
         y -> image.eu -> freq++;
   }
}

ostream& operator <<(ostream& out, trie& tr)
{
   static char char_buf[100];

   if ( tr.root == 0 )
      return out;

   tr.root -> _print(out, char_buf, 0);
   return out;
}


void update_index(int ind, void* x)
{
   ((trie_node_info*)x) -> image.heap = ind;
}

void trie::_find_leaf(trie_node* z, int& j)
{
   trie_node_info* x = 0;
   ostring *y;

   for ( int i=0; i<LANG_ALPHABET_SZ; i++ ) {
#ifdef C_API
      x = (z -> children[i]);
#else
      x = &(z -> children[i]);
#endif

      if ( x -> info.info_view.freq > 0 ) {

         if ( x -> child == 0 ) { 

            x -> info.info_view.mark = 1;
            y = get_word(x);
            alphabet[j++] =x->image.eu =new encoding_unit(y, x -> info.info_view.freq);

         } else
            _find_leaf(x -> child, j);

      }
   }
}

encoding_unit** trie::get_alphabet(unsigned int& a_sz)
{
   a_sz = alphabet_sz;
   return alphabet;
}

ostring* trie::get_word(trie_node_info* leaf)
{
   static char buf[128];
   buf[127] = 0;

   int i = 127;

   trie_node_info* x = leaf;

   if ( x == 0 ) 
      throw(stringException("null leaf node pointer"));

   while ( x ) {

      buf[--i] = x -> info.info_view.letter;

      x = &x[LANG_ALPHABET_SZ - x->info.info_view.letter];
      x = (trie_node_info*)(x -> child);
   }

//debug(cerr, buf+i);
   return new ostring(buf+i, 127-i);
}

Boolean trie::travers_to(char* str, int len, 
                         trie_node*& node, trie_node_info*& node_info
                        )
{
//MESSAGE(cerr, "travers_to BEG");
   if ( root == 0 )
      return false;
      

   node = root;
   int i, j;

   for ( i=0; i<len; i++ ) {
      j = str[i];

//debug(cerr, str[i]);

#ifdef C_API
      node_info = (node -> children[j]);
#else
      node_info = &(node -> children[j]);
#endif

      if ( node_info -> info.info_view.freq == 0 ) {
//MESSAGE(cerr, "BREAK");
        break;
      } else
        node = node_info -> child;
   }
//MESSAGE(cerr, "travers_to RET");

   return  ( len == i ) ? true : false;
}

void trie::collect_freqs(trie_node* rt, int level)
{
   for ( int i= 0; i<LANG_ALPHABET_SZ; i++ ) {
#ifdef C_API
      if ( rt -> children[i] -> info.info_view.freq > 0 ) {

         sorted_freqs -> insert(rt -> children[i]);

         if ( rt -> children[i] -> child )
            collect_freqs(rt -> children[i] -> child, level+1);
      }
#else
       if ( rt -> children[i].info.info_view.freq > 0 ) {

         sorted_freqs -> insert(&(rt -> children[i]));

         if ( rt -> children[i].child )
            collect_freqs(rt -> children[i].child, level+1);
      }
#endif
   }
}

encoding_unit* trie::parse(unsigned char* str, int len)
{
   if ( root == 0 )
      return 0;

   trie_node* node = root;
   trie_node_info* cell = 0;
   trie_node_info* last_hit = 0;
   int i=0;

   while ( node && i < len ) {

//debug(cerr, str[i]);
#ifdef C_API
      cell = (node -> children[str[i++]]);
#else
      cell = &(node -> children[str[i++]]);
#endif

      if ( cell -> info.info_view.mark == 1 ) 
         last_hit = cell;

      node = cell -> child;
   }

   if ( last_hit == 0 ) {
      debug(cerr, len);
      debug(cerr, str[i]);
      debug(cerr, int(str[i]));
      throw(stringException("parse(): string not in trie"));
   }

   if ( last_hit -> image.eu == 0 )
      throw(stringException("parse(): encoding unit not found"));

   return last_hit -> image.eu;
}

///////////////////////////////////////
//
///////////////////////////////////////
Boolean trie_node_ls(voidPtr n1, voidPtr n2)
{
   if ( ((trie_node_info*)n1) -> info.info_view.freq < ((trie_node_info*)n2) -> info.info_view.freq )
      return true;
   else
      return false;
}

Boolean trie_node_eq(voidPtr n1, voidPtr n2)
{
   if ( ((trie_node_info*)n1) -> info.info_view.freq == ((trie_node_info*)n2) -> info.info_view.freq )
      return true;
   else
      return false;
}


