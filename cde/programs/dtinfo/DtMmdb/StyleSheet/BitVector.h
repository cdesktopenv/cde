/* $XConsortium: BitVector.h /main/4 1996/08/21 15:49:59 drk $ */

#ifndef _BitVector_h
#define _BitVector_h 1

#include <stream.h>

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
   positionArrayT *f_positionArray;
   unsigned int *f_array;
   unsigned int f_bits;
   unsigned int f_words;

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
