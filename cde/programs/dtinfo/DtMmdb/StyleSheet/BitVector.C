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
// $TOG: BitVector.C /main/4 1998/04/17 11:47:51 mgreess $


#include "BitVector.h"
#include "Debug.h"
#include "StyleSheetExceptions.h"
   
#define wordWithMSBSet (0x1 << (WORD_SIZE-1))
#define BIT_TEST(x, y)  ( ((x) & (y)) == (y) )
#define RESET_BIT(x, y)  x &= (~(y))
#define SET_BIT(x, y)    x |= (y)

unsigned int posRecord::operator ==(const posRecord&)
{
   MESSAGE(cerr, "posRecord::operator ==() should not be called");
   throw(CASTBEEXCEPT badEvaluationException());
   return 0;
}


BitVector::BitVector(int bits, unsigned int initValue) :
   f_bits(bits), f_words(bits/WORD_SIZE+1), f_positionArray(0)
{
   f_array = new unsigned int[f_words];
   setAllBitsTo(initValue);
}

void BitVector::setAllBitsTo(unsigned int initValue) 
{
   unsigned int fill = ( initValue == 0 ) ? 0x0 : ~0x0;

   for ( unsigned int i=0; i<f_words; i++ )
     f_array[i]=fill;
}

BitVector::~BitVector()
{
   delete f_array;
   delete f_positionArray;
}

void BitVector::setTo(BitVector& v)
{
   if ( f_words != v.f_words ) {
     delete f_array;
     f_array = new unsigned int[v.f_words];
   }
   f_bits = v.f_bits;
   f_words = v.f_words;

   for ( unsigned int i=0; i<f_words; i++ )
     f_array[i]=v.f_array[i];
}

void BitVector::setBitTo(int i, unsigned int x)
{
   unsigned int wordIndex = i / WORD_SIZE;
   unsigned int bitIndex = i % WORD_SIZE;

   if ( x == 1 ) {
      if ( wordIndex < f_words - 1 ) 
         SET_BIT(f_array[wordIndex], (0x1 << bitIndex));
      else
         SET_BIT(f_array[wordIndex], 
                 (0x1 << (WORD_SIZE - f_bits % WORD_SIZE + bitIndex))
                );
   } else {
      if ( wordIndex < f_words - 1 ) 
         RESET_BIT(f_array[wordIndex], (0x1 << bitIndex));
      else
         RESET_BIT(f_array[wordIndex], 
                 (0x1 << (WORD_SIZE - f_bits % WORD_SIZE + bitIndex))
                );
    }
}

void BitVector::recordPositions(unsigned int PTPos, unsigned int BITPos)
{
   if ( f_positionArray == 0 )
      f_positionArray = new positionArrayT;

   f_positionArray -> append(posRecord(PTPos, BITPos));
}

unsigned int BitVector::getBit(int i)
{
   unsigned int wordIndex = i / WORD_SIZE;
   unsigned int bitIndex = i % WORD_SIZE;

   if ( wordIndex < f_words - 1 ) 
         return BIT_TEST((int)f_array[wordIndex], (0x1 << bitIndex)) ? 1 : 0;
   else
         return BIT_TEST((int)f_array[wordIndex],
                 (0x1 << (WORD_SIZE - f_bits % WORD_SIZE + bitIndex))
                        ) ? 1 : 0;
}

BitVector& BitVector::operator &=(BitVector& b)
{
   for ( unsigned int i=0; i<f_words; i++ )
     f_array[i] &= b.f_array[i];

   return *this;
}

BitVector& BitVector::operator ^=(BitVector& b)
{
   for ( unsigned int i=0; i<f_words; i++ )
     f_array[i] ^= b.f_array[i];

   return *this;
}

BitVector& BitVector::operator |=(BitVector& b)
{
   for ( unsigned int i=0; i<f_words; i++ )
     f_array[i] |= b.f_array[i];

   return *this;
}

BitVector& BitVector::shiftRightOneBit()
{
   unsigned int msb = 0;
   unsigned int lsb = 0;

   for ( unsigned int i=0; i<f_words; i++ ) {
     lsb = ( BIT_TEST(f_array[i], 0x1) ) ? 0x1 : 0x0;
     f_array[i] = f_array[i] >> 1;   
     f_array[i] |= msb;
     msb = lsb;
   }

   SET_BIT(f_array[0], wordWithMSBSet);

   return *this;
}

BitVector& BitVector::shiftLeftOneBit()
{
   unsigned int msb = 0;
   unsigned int lsb = 0;


   for ( int i=f_words-1; i>=0; i++ ) {
     msb = (BIT_TEST((int)f_array[i], wordWithMSBSet)) ? wordWithMSBSet : 0x0;
     f_array[i] = f_array[i] << 1;   
     f_array[i] |= lsb;
     lsb = msb;
   }

   return *this;
}

ostream& operator<<(ostream& out, BitVector& bv)
{
   for ( int i=bv.f_bits-1; i>=0; i-- ) {
      out << bv.getBit(i) ;
   }
   out << "\n";

   posRecord x;
   if ( bv.f_positionArray ) {
      positionArrayIteratorT l_positionNext(*bv.f_positionArray);
      while (++l_positionNext) {
         x = l_positionNext.key();
         out << x.pathTermPos << "." << x.bitPos << " ";
      }
   }

   out << "\n";

   return out;
}
