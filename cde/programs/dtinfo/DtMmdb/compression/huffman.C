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
 * $XConsortium: huffman.cc /main/3 1996/06/11 17:15:06 cde-hal $
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


#include "compression/huffman.h"
#include "dstr/heap.h"

////////////////////////////////////////
//
////////////////////////////////////////

htr_node::htr_node(encoding_unit* e, htr_node* lt, htr_node* rt):
   left(lt), right(rt), eu(e), freq(e->freq), parent(0)
{
}

htr_node::htr_node(unsigned long f, htr_node* lt, htr_node* rt):
   left(lt), right(rt), eu(0), freq(f), parent(0)
{
}

htr_node::~htr_node()
{
   delete left;
   delete right;
}

////////////////////////////////////////
//
////////////////////////////////////////
Boolean htr_eq(const void* n1, const void* n2)
{
   if ( ((htr_node*)n1) -> freq == ((htr_node*)n2) -> freq )
     return true;
   else
     return false;
}

Boolean htr_ls(const void* n1, const void* n2)
{
   if ( ((htr_node*)n1) -> freq > ((htr_node*)n2) -> freq )
     return true;
   else
     return false;
}

////////////////////////////////////////
//
////////////////////////////////////////
huff::huff(): compress_agent(HUFFMAN_AGENT_CODE), 
   e_units(0), cts(0), tri(new trie(26)), htr_root(0)
{
}

huff::~huff()
{
   delete tri;
   delete htr_root;
}

void huff::build_tree()
{
   heap htr_node_set(htr_eq, htr_ls, cts);

   htr_node* x ;
   for (int i=0; i<cts; i++ ) {
      if ( e_units[i] ) {
         x = new htr_node(e_units[i]);
         e_units[i] -> leaf_htr_node = x; 
         htr_node_set.insert(x);
      }
   }

   htr_node_set.heapify();

   htr_node *n1, *n2, *n3;
   while ( htr_node_set.count() > 1 ) {

// max is the smallest element. see htr_ls()
      n1 = (htr_node*)htr_node_set.max_elm() ;
      htr_node_set.delete_max() ;

// max is the smallest element. see htr_ls()
      n2 = (htr_node*)htr_node_set.max_elm() ;
      htr_node_set.delete_max() ;

      n3 = new htr_node(n1->freq+n2->freq, n1, n2);

      n1 -> parent = n2 -> parent = n3;

      htr_node_set.insert_heapify(n3);
   }

   htr_root = (htr_node*)htr_node_set.max_elm();
   htr_node_set.delete_max() ;
}

void huff::calculate_code()
{
   htr_node* x ;
   htr_node* parent;

   for (int i=0; i<cts; i++ ) {

      if ( e_units[i] == 0 )
         continue;

      e_units[i] -> code = 0;
      e_units[i] -> bits = 0;

      x = e_units[i] -> leaf_htr_node;

      while ( x ) {
         parent = x -> parent;
       
         if ( parent == 0 )
            break;

         e_units[i] -> code >>= 1;

         if ( parent -> left == x ) {
            e_units[i] -> code |= 0x80000000;
         } else
         if ( parent -> right != x ) {
            debug(cerr, i);
            throw(stringException("huffman tree corrupted"));
         }

         x = parent;
         e_units[i] -> bits++;

         if ( e_units[i] -> bits > BITS_IN(unsigned long) ) {
            debug(cerr, e_units[i] -> bits);
            throw(stringException("huffman tree too deep"));
         }
      }

      e_units[i] -> code >>= ( 32 - e_units[i] -> bits );
//debug(cerr, hex(e_units[i] -> code));
   }
}

ostream& huff::print_alphabet(ostream& out)
{
   unsigned long total_uncmp = 0;
   unsigned long int total_cmp = 0;

   for (int i=0; i<cts; i++ ) {

      if ( e_units[i] == 0 )
         continue;
 
      total_uncmp += (e_units[i] -> word -> size()) * (e_units[i] -> freq); 
      total_cmp += (e_units[i] -> bits) * (e_units[i] -> freq); 

      out << *(e_units[i] -> word) << ":" << e_units[i]->bits << "\n";
   }
   total_cmp = total_cmp / 8 + total_cmp % 8;

/*
   debug(cerr, total_uncmp);
   debug(cerr, total_cmp);

   debug(cerr, 1 - float(total_cmp) / float(total_uncmp) );
*/

   return out;
}

// self modifying buf ptr after taking an encoding unit.
encoding_unit* huff::get_e_unit(unsigned char*& buf, int len) 
{
   encoding_unit* x = tri -> parse(buf, len) ;

//debug(cerr, *(x -> word));

   buf += x -> word -> size();
   return x;
}

int total_uncomp = 0;
int total_comp = 0;

void huff::compress(const buffer& uncompressed, buffer& compressed) 
{
//debug(cerr, *(buffer*)&uncompressed);
   if ( compressed.buf_sz() < uncompressed.buf_sz() )
      compressed.expand_chunk(uncompressed.buf_sz());


   unsigned short total_bits = 0;

   int uncmp_sz = uncompressed.content_sz();
   unsigned char* buf = (unsigned char*)uncompressed.get_base();


   unsigned int code_buf = 0;
   unsigned int rem_long = 0;
   int rem_bits = 0;

   encoding_unit *e_ptr = 0;

   while ( uncmp_sz > 0 ) {

       //e_ptr = get_e_unit(buf, uncmp_sz);

       e_ptr = tri -> parse(buf, uncmp_sz);

       buf += e_ptr -> word -> size();
       uncmp_sz -= e_ptr -> word -> size();

       if ( rem_bits + e_ptr -> bits > 32 ) {

          code_buf = e_ptr -> code;          // shift bits to the higher end

          rem_long <<= 32-rem_bits;

          rem_bits += e_ptr -> bits - 32;    // new rem_bits

          code_buf >>= rem_bits;             // get padding part

          rem_long |= code_buf;              // padding

          compressed.put( rem_long );

// save remaining (rem_bits + e_ptr -> bits - 32)  bits to rem_bits.

          rem_long = e_ptr -> code & (~0L >> (32 - rem_bits));

       } else {
          rem_long <<= e_ptr -> bits;
          rem_long |=  e_ptr -> code;
          rem_bits +=  e_ptr -> bits;
//debug(cerr, hex(rem_long));
       }

       total_bits += e_ptr -> bits;
       total_bits &= 0x1f; // take the mod on 32
   }

   if ( rem_bits > 0 ) {
      rem_long <<= 32 - rem_bits;
//MESSAGE(cerr, "PUT");
//debug(cerr, hex(rem_long));
      compressed.put( rem_long );
   }

//debug(cerr, total_bits);
   compressed.put(char(total_bits));

//   total_uncomp += uncompressed.content_sz();
//   total_comp += compressed.content_sz();

/*
   debug(cerr, total_uncomp);
   debug(cerr, total_comp);
         
   debug(cerr, 
         1-float(compressed.content_sz()-1)/float(uncompressed.content_sz())
        );
*/
}

void huff::decompress(buffer& compressed, buffer& uncompressed)
{
   char* buf_base = uncompressed.get_base();
   char* str;
   int str_len;

   char rem_bits;

   int ct = (compressed.content_sz() - 1) >> 2;

   unsigned int c;

   int bits_bound = 32;

   htr_node *node_ptr = htr_root;

   do {
      compressed.get(c); ct--;

      if ( ct == 0 ) {
         compressed.get(rem_bits);
//debug(cerr, int(rem_bits));
         bits_bound = rem_bits ; 
      }

      for ( int i=0;i<bits_bound; i++ ) {
         if ( node_ptr -> left == 0 && node_ptr -> right == 0 ) {
//for ( int j=0; j<node_ptr -> eu -> word -> size(); j++ ) {
// cerr << (node_ptr -> eu -> word -> get())[j];
//}

            str_len = node_ptr -> eu -> word -> size();
            str = node_ptr -> eu -> word -> get();

            if ( str_len == 1 ) {

                *buf_base = str[0];
                buf_base++;

//                uncompressed.put((node_ptr -> eu -> word -> get())[0]);
            } else {


                memcpy(buf_base, str, str_len);
                buf_base += str_len;


/*
                uncompressed.put( node_ptr -> eu -> word -> get(), 
                                  node_ptr -> eu -> word -> size() 
                                );
*/

            }
            node_ptr = htr_root;
         }

         if ( c & 0x80000000 )
            node_ptr = node_ptr -> left;
         else
            node_ptr = node_ptr -> right;
   
         c <<= 1;
      }


   } while ( ct>0 );

//debug(cerr, buf_base-uncompressed.get_base());
   uncompressed.set_content_sz(buf_base-uncompressed.get_base());

   if ( rem_bits > 0 )
      uncompressed.put( node_ptr -> eu -> word -> get(), 
                        node_ptr -> eu -> word -> size() 
                      );
}

//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////

MMDB_BODIES(huff)

int huff::cdr_sizeof()
{
   return pstring::cdr_sizeof();
}

io_status huff::cdrOut(buffer& buf)
{
//MESSAGE(cerr, "huff::cdrOut");
//debug(cerr, my_oid());
   static buffer v_out_buf(LBUFSIZ);
   int i;

   if ( cts > 0 ) {
//MESSAGE(cerr, "huff::cdrOut: dict out");
      int sz = sizeof(int);
      for ( i=0; i<cts; i++ ) {
         sz += ( e_units[i] -> word -> size() +
                        sizeof(unsigned int) +
                        sizeof(char)
                      );
      }
   
      v_out_buf.expand_chunk(sz);
   
      v_out_buf.put(cts);
   
      int word_sz;
   
      for ( i=0; i<cts; i++ ) {
         word_sz = e_units[i] -> word -> size();
         v_out_buf.put(char(word_sz));
   
         v_out_buf.put(e_units[i] -> word -> get(), word_sz);
         v_out_buf.put(e_units[i] -> freq);
      }
   
      pstring::update(v_out_buf.get_base(), v_out_buf.content_sz());
   }

   return pstring::cdrOut(buf);
}

// format:
//     entries_int
//     (len_byte word_chars freq_int)+
//
io_status huff::cdrIn(buffer& buf)
{
   static buffer v_in_buf(0);

   pstring::cdrIn(buf);

   if ( pstring::size() > 0 ) {

      v_in_buf.set_chunk(pstring::get(), pstring::size());
      v_in_buf.set_content_sz(pstring::size());
     
      v_in_buf.get(cts);
   
      char word_buf[BUFSIZ];
      char word_sz;
      unsigned int word_freq;
      //ostring *z = 0;
   
      for ( int i=0; i<cts; i++ ) {
   
         v_in_buf.get(word_sz);
         v_in_buf.get(word_buf, int(word_sz));
         v_in_buf.get(word_freq);
   
/*
         z = new ostring((char*)word_buf, word_sz);
         extend_alphabet();
         alphabet[alphabet_sz++] = new encoding_unit(z, word_freq);
*/

         tri -> add_to_alphabet((unsigned char*)word_buf, word_sz, word_freq);
      }

   e_units = tri -> get_alphabet(cts);

   build_tree();
   calculate_code();
   delete tri; tri = 0;

//print_alphabet(cerr);

   }

   return done;
}

trie* alphabet = 0;

void trie_add_wrap(unsigned char* buf, int len, int action_num)
{
   switch ( action_num ) {
      case 1:
         alphabet -> add(buf, len);
         break;
      case 2: 
         alphabet -> add_letters(buf, len);
         break;

      default:
         debug(cerr, action_num);
         throw(stringException("unknown action number"));
   }
}


io_status huff::build_dict(lex_func_t f_lex, getchar_func_t f_getchar)
{
MESSAGE(cerr, "get to huff build dict");
   fill_buf_func = f_getchar;

   alphabet = tri;

   lex_action_func = trie_add_wrap;
   
   if ( (*f_lex)() != 0 )
      throw(stringException("huff::asciiIn(): Parsing input failed"));

   e_units = tri -> get_alphabet(cts);

//debug(cerr, *tri);

   build_tree();
   calculate_code();

//print_alphabet(cerr);

   set_mode(UPDATE, true);

   return done;
}

