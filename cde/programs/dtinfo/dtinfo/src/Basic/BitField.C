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
 * $XConsortium: BitField.C /main/4 1996/10/04 11:26:36 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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

#define C_BitField
#define L_Basic

#include <Prelude.h>

// NOTE: run-time speedup...set this up to use masks on the handle rather than
// using div and mod to caluclate the word and bit offset - jbm, djb


// /////////////////////////////////////////////////////////////////////////
// bitmask sets up run-time computed bitmasks at startup initialization
// /////////////////////////////////////////////////////////////////////////

class bitmask
{
public:
  bitmask();
  unsigned char operator [](int);
private:
  unsigned char f_masks[bits_per_byte] ;
};
bitmask::bitmask()
{
  for (int i = 0 ; i < bits_per_byte; i++)
    f_masks[i] = 1 << i ;
}
inline
unsigned char
bitmask::operator[](int i)
{
  return f_masks[i] ;
}

static bitmask mask;

BitField::BitField(unsigned long initial_bits)
{
  f_num_words = (initial_bits - 1)/(bytes_per_word * bits_per_byte) + 1 ;
  ON_DEBUG (printf ("BitField allocated %ld words for %ld bits\n",
		    f_num_words, initial_bits));

  f_words = new unsigned long [ f_num_words ] ;
  f_bits_in_use = new unsigned long [ f_num_words ] ;

  // zero bit arrays 
  for (int i = 0 ; i < f_num_words; i++){
    f_words[i] = 0 ;
  }
  // reserve first (0) bit
  //  - a handle of zero is a likely source of errors - jbm 
  f_bits_in_use[0] = 1 ;
}

BitField::~BitField()
{
  delete f_words ;
  delete f_bits_in_use ;
}

void
BitField::check_allocation (unsigned long word)
{
  if (word >= f_num_words)
    {
      unsigned long *new_words = new unsigned long [word + 1];
      unsigned long *new_use   = new unsigned long [word + 1];

      int i;
      for (i = 0 ; i < f_num_words ; i++)
	{
	  // copy old info 
	  new_words[i] = f_words[i] ;
	  new_use[i] = f_bits_in_use[i];
	}

      delete f_words;
      f_words = new_words;
      delete f_bits_in_use;
      f_bits_in_use = new_use;
      f_num_words = word + 1;

      // zero out new memory
      for (; i < f_num_words; i++)
	{
	  new_words[i] = 0;
	  new_use[i] = 0;
	}
    }
}


BitHandle
BitField::get_handle()
{

  // find first free bit

  // first find first free word
  int word;
  for (word = 0 ; word < f_num_words ; word++ )
    if ( word_has_zero_bit(f_bits_in_use[word]) )
      break;

  if (word == f_num_words)
    {
      check_allocation (word);
#if 0
    // have to allocate more 
    unsigned long *new_words = new unsigned long [f_num_words + 1] ;
    unsigned long *new_use   = new unsigned long [f_num_words + 1] ;

    for (int i = 0 ; i < f_num_words ; i++ ){
      // copy old info 
      new_words[i] = f_words[i] ;
      new_use[i] = f_bits_in_use[i] ;
    }

    // zero out newest memory
    new_words[f_num_words] = 0 ;
    new_use[f_num_words] = 0 ;

    delete f_words ;
    f_words = new_words ;
    delete f_bits_in_use ;
    f_bits_in_use = new_use ;
    f_num_words++ ;
#endif
    return get_handle();	// NOTE: recursive and function exit
  }


  // now find first free byte in word 
  int byte;
  for (byte = 0 ; byte < bytes_per_word ; byte++ )
    if (byte_has_zero_bit(get_byte(byte, f_bits_in_use[word])))
      break;
  
  unsigned char the_byte = (unsigned char)
    (f_bits_in_use[word] >> (byte * bits_per_byte)) & 0xFF ;

  // now check byte for zero bit
  int bit;
  for (bit = 0 ; bit < bits_per_byte ; bit++ )
    if ((the_byte & mask[bit]) == 0)
      break ;

  // mark the bit as in use 
  f_bits_in_use[word] |= 1 << ((byte * bits_per_byte) + bit) ;
  
  return (word * bits_per_word) + (byte * bits_per_byte) + bit ;
}

void
BitField::free_handle(BitHandle handle)
{
  assert(handle);

  // same as code for unset 

  unsigned long word = handle / bits_per_word ;

  unsigned long mask = 1 << (handle % bits_per_word) ;
  f_bits_in_use[word] &= ~0L ^ mask ;

  i_unset(handle);	// zero the bit
  unsigned long *save = f_words; 
  f_words = f_bits_in_use;
  i_unset(handle); // free the bit
  f_words = save;
}

// /////////////////////////////////////////////////////////////////////////
// i_is_set - internal is set
// /////////////////////////////////////////////////////////////////////////

bool
BitField::i_is_set(BitHandle handle)
{
  assert(handle);
  unsigned long word = handle / bits_per_word ;
  check_allocation (word);

  unsigned long mask = 1 << (handle % bits_per_word) ;

  return (f_words[word] & mask) != 0 ;
}


// /////////////////////////////////////////////////////////////////////////
// i_set - internal set
// /////////////////////////////////////////////////////////////////////////

bool
BitField::i_set(BitHandle handle)
{
  assert(handle);
  unsigned long word = handle / bits_per_word ;
  check_allocation (word);

  unsigned long mask = 1 << (handle % bits_per_word) ;

  ON_DEBUG (printf ("BitField: setting bit %ld (in word %ld)\n", handle, word));

  f_words[word] |= mask ;

  return 1 ;
}
// /////////////////////////////////////////////////////////////////////////
// i_unset - internal unset
// /////////////////////////////////////////////////////////////////////////

bool
BitField::i_unset(BitHandle handle)
{
  assert(handle);
  unsigned long word = handle / bits_per_word ;
  check_allocation (word);

  unsigned long mask = 1 << (handle % bits_per_word) ;

  f_words[word] &= ~0L ^ mask ;

  return 0 ;
}

#if 0
void
BitField::save()
{
  f_buffer.write(f_num_words);
  for (int i = 0 ; i < f_num_words ; i++){
    f_buffer.write(f_words[i]);
    f_buffer.write(f_bits_in_use[i]);
  }
}

void
BitField::restore()
{
  f_buffer.read(f_num_words);
  f_words = 		new unsigned long [f_num_words] ;
  f_bits_in_use = 	new unsigned long [f_num_words] ;

  for (int i = 0 ; i < f_num_words; i++ ){
    f_buffer.read(f_words[i]);
    f_buffer.read(f_bits_in_use[0]);
  }
    
}
#endif

#ifdef DEBUG

bool
BitField::is_used(BitHandle handle)
{
  unsigned long word = handle / bits_per_word ;

  unsigned long mask = 1 << (handle % bits_per_word) ;

  return (f_bits_in_use[word] & mask) != 0 ;
}


ostream &
BitField::dump(ostream &o)
{
  int i ;
  o << "use:\t" ;
  for (i = 0 ; i < f_num_words ; i++ )
    o << hex << f_bits_in_use[i] << "\t" ;
  o << endl;

  o << "flag:\t" ;
  for (i = 0 ; i < f_num_words ; i++ )
    o << hex << f_words[i] << "\t" ;
  o << endl;
  return o ;
}
#endif
