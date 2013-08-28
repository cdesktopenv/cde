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
/* $XConsortium: BitVector.h /main/4 1996/08/21 15:49:59 drk $ */

#ifndef _BitVector_h
#define _BitVector_h 1

#include <sstream>
#include <ostream>
using namespace std;

#ifndef CDE_NEXT

#else
//#include <StyleSheet/cde_next.h>
#include "dti_cc/CC_Slist.h"
#endif

#define WORD_SIZE 32

class posRecord
{
public:
   posRecord(unsigned int x = 0, unsigned int y = 0) : 
	pathTermPos(x), bitPos(y) {};
   ~posRecord() {};

   unsigned int operator ==(const posRecord&);

public:
   unsigned int pathTermPos;
   unsigned int bitPos;
};

typedef CC_TValSlist<posRecord> positionArrayT;
typedef CC_TValSlistIterator<posRecord> positionArrayIteratorT;

///////////////////////////////////////////////////////
// A storage/manipulation efficient bit vector class
///////////////////////////////////////////////////////
class BitVector 
{
   unsigned int *f_array;
   unsigned int f_bits;
   unsigned int f_words;
   positionArrayT *f_positionArray;

public:
   BitVector(int bits, unsigned int initValue);
   ~BitVector();

//
// bits range: 0 .. bits-1
// the 0th bit is LSB, the (bits-1)th bit is MSB
//
   void setAllBitsTo(unsigned int);
   void setTo(BitVector&);

   void recordPositions(unsigned int pathTermPos, unsigned int BitPos);
   positionArrayT* positionArray() { return f_positionArray; };

   void setBitTo(int i, unsigned int);
   unsigned int getBit(int i);

   BitVector& operator &=(BitVector&);
   BitVector& operator ^=(BitVector&);
   BitVector& operator |=(BitVector&);

// Note: the MSB is set to 1 after each shiftRightOneBit() call.
   BitVector& shiftRightOneBit();
   BitVector& shiftLeftOneBit();

   friend ostream& operator<<(ostream&, BitVector&);
};

#endif
