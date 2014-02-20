/*
 * $TOG: BitField.hh /main/5 1998/04/17 11:35:57 mgreess $
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
#if defined(SVR4) || defined(hpux) || defined(linux) || defined(__NetBSD__)
#include <limits.h>
#else
#include <sys/limits.h>
#endif

const int bytes_per_word = sizeof(unsigned long);
const int bits_per_byte  = CHAR_BIT ;
const int bits_per_word = bytes_per_word * bits_per_byte ;

typedef unsigned long BitHandle ;

class BitField 
{
public:
  BitField(unsigned long initial_bits = CHAR_BIT * sizeof(unsigned long) - 1) ; 
  ~BitField();

  void		return_bit(unsigned long bit); // give back previously
					       // allocated bit
  BitHandle	get_handle();		       // allocate a bit for future use
  void		free_handle(BitHandle);	       // return bit for other use

  /* -------- bit operations -------- */
  bool	is_set(BitHandle);
  bool	set(BitHandle);
  bool	unset(BitHandle);


  void		save();

#ifdef DEBUG
  bool	is_used(BitHandle);
  ostream      &dump(ostream &);
#endif

private:
  /* -------- internal functions that do the real work -------- */
  void check_allocation (unsigned long word);
  bool	i_is_set(BitHandle);
  bool	i_set(BitHandle);
  bool	i_unset(BitHandle);


  /* -------- suplementary (inline) functions -------- */
  bool	word_has_zero_bit(unsigned long);
  bool	byte_has_zero_bit(unsigned char byte);
  unsigned char	get_byte(int byte, unsigned long word);


private:
  unsigned long		f_num_words ; // number of allocated machine words
  unsigned long	       *f_words ;     // array of words for bits
  unsigned long	       *f_bits_in_use ;	// maps bits already allocated
};

// the assert(is_used(handle))  is commented out as OutlineLists do not
// allocate separate data handles for thier child lists - jbm

inline bool
BitField::is_set(BitHandle handle)
{
  assert(handle);
//  assert(is_used(handle));
  return i_is_set(handle);
}

inline bool
BitField::set(BitHandle handle)
{
  assert(handle);
//  assert(is_used(handle));
  return i_set(handle);
}

inline bool
BitField::unset(BitHandle handle)
{
  assert(handle);
//  assert(is_used(handle));
  return i_unset(handle);
}


inline bool
BitField::word_has_zero_bit(unsigned long word)
{
  return !((word & ULONG_MAX) == ULONG_MAX);
}

inline bool
BitField::byte_has_zero_bit(unsigned char byte)
{
  return !((byte & UCHAR_MAX) == UCHAR_MAX) ;
}

// return a byte within a word

inline unsigned char
BitField::get_byte(int byte, unsigned long word)
{
  return (word >> (byte * bits_per_byte)) & UCHAR_MAX ;
}

#ifdef DEBUG
inline
ostream &
operator << (ostream &o, BitField &bf)
{
  return bf.dump(o);
}

#endif
