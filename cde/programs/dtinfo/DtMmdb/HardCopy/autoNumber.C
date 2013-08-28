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
// $TOG: autoNumber.C /main/6 1998/04/17 11:47:13 mgreess $

#include <ctype.h>
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>
#endif

#include "HardCopy/autoNumber.h"
#include "HardCopy/FPExceptions.h"

buffer autoNumber::f_buf(128);

autoNumber::autoNumber(const char* nm, enum autoNumberType t, int delta, const char* prefix, const char* postfix) :
   f_name(strdup(nm)), f_type(t), f_delta(delta),
   f_prefix(strdup(prefix)), f_postfix(strdup(postfix)),
   f_initialValue(0)
{
   int x = strlen(prefix) + strlen(postfix) + 12;
   if ( x > f_buf.buf_sz() ) 
      f_buf.expand_chunk(x);
}

autoNumber::~autoNumber()
{
   delete f_name;
   delete f_prefix;
   delete f_postfix;

   while (f_values.entries())
     f_values.pop();
   while (f_serial_nums.entries())
     f_serial_nums.pop();
}

void autoNumber::setNumTagsSeen()
{ 
  if (f_serial_nums.entries() > 0 && f_serial_nums.top() < MAXINT)
     f_serial_nums.top()++;
}

void
autoNumber::reset() 
{ 
   // reset stack of values
   while (f_values.entries() > 1) // leave one entry for re-use
     f_values.pop();
   f_values.top() = f_initialValue;

   // reset stack of serial numbers
   while (f_serial_nums.entries() > 1) // leave one entry for re-use
     f_serial_nums.pop();
   f_serial_nums.top() = 0;
}

void
autoNumber::push()
{
    f_values.push(f_initialValue);
    f_serial_nums.push(0);
}

void
autoNumber::pop()
{
    if (f_values.entries() > 1)
      f_values.pop();
    if (f_serial_nums.entries() > 1)
      f_serial_nums.pop();
}

unsigned int autoNumber::operator==(const autoNumber&)
{
   return false;
}

ostream& operator<<(ostream& out, const autoNumber& an)
{
   debug(cerr, an.f_name);
   debug(cerr, an.f_delta);
   debug(cerr, an.f_prefix);
   debug(cerr, an.f_postfix);
   debug(cerr, an.f_type);
   debug(cerr, an.f_serial_nums.top());
   return out;
}

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////

autoNumberNumeric::autoNumberNumeric(const char* nm, int delta, int inv,
				const char* prefix, const char* postfix) :
   autoNumber(nm, NUMERIC, delta, prefix, postfix)
{
   f_initialValue = inv;

   f_values.push(f_initialValue);
   f_serial_nums.push(0);
}

autoNumberNumeric::~autoNumberNumeric()
{
}

void autoNumberNumeric::setNextValue()
{
  if (f_serial_nums.entries() && f_values.entries())
  {
    if (f_serial_nums.top() >= 2)
      f_values.top() += f_delta;
  }
}

const char*
autoNumberNumeric::getValue()
{
   char* ptr = f_buf.get_base();
   int ptrlen = f_buf.buf_sz();

   if (f_values.entries())
     snprintf(ptr, ptrlen, "%s",
		form("%s%d%s", f_prefix, f_values.top(), f_postfix));
   else
     *ptr = 0;

   return ptr;
}

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////

autoNumberCased::autoNumberCased(const char* nm, autoNumberType an_t, 
				 int delta, enum CaseType ct, const char* prefix, const char* postfix) :
   autoNumber(nm, an_t, delta, prefix, postfix), f_case(ct)
{
}

autoNumberCased::~autoNumberCased()
{
}

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////

char autoNumberAlphabetic::f_lowerCaseLetters[26] =
{
   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
   'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
   'w', 'x', 'y', 'z'
};

char autoNumberAlphabetic::f_upperCaseLetters[26] =
{
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
   'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'T', 'T', 'U', 'V',
   'W', 'X', 'Y', 'Z'
};

autoNumberAlphabetic::autoNumberAlphabetic(
		const char* nm, 
		int delta,
		CaseType ct, 
		const char* InitialValue, const char* prefix, const char* postfix
					  ) :
   autoNumberCased(nm, ALPHABETIC, delta, ct, prefix, postfix)
{
   f_initialValue = alphaToInt(InitialValue, f_case);

   f_values.push(f_initialValue);
   f_serial_nums.push(0);
}

autoNumberAlphabetic::~autoNumberAlphabetic()
{
}

static const int base = 26;
   
int autoNumberAlphabetic::alphaToInt(const char* alpha, enum CaseType a_case) 
{
   int digits = strlen(alpha);
   int i;
   int offset = 0;

   switch ( a_case ) {
     case UPPER:
       for (i=0; i<digits; i++)
          if ( isupper(alpha[i]) == 0 ) {
             MESSAGE(cerr, 
               "Initial alphabetic autonumber value is not capitalized");
             throw(CASTHCREXCEPT hardCopyRendererException());
          }
       offset = 'A';
       break;
     case LOWER:
       for (i=0; i<digits; i++)
          if ( islower(alpha[i]) == 0 ) {
             MESSAGE(cerr, 
               "Initial alphabetic autonumber value is not in small case");
             throw(CASTHCREXCEPT hardCopyRendererException());
          }
       offset = 'a';
       break;
   }

   int x = 0;
   int expansionFactor = 1;
   for ( i=digits-1; i>=0; i-- ) {
      x += (alpha[i] - offset)*expansionFactor; 
      expansionFactor *= base;
   }

   x += int((pow(base, digits)-1) / (base-1)) - 1;
  
   return x;
}

// Algorithm: converting integer values to/from alphabetic autonumbers

// The alphabetic autonumbers are grouped into blocks where each
// block represents autonumbers with same number of digits. The size
// of a block of d digts = 26^d. Now assigning a sequece number (an integer)
// to each autonumber in blocks. This number in fact is the integer
// value (internal) of the autonumber.
//
// block 1: [a, b, c, ..., z]
// seq num: [0, 1, 2,      25]
//
// block 2: [aa, ab, ac, ..., zz]
// seq num: [26, 27, 28,      701]
//
// In general, the 1st sequence number in a block for d digits:
//   x = int((pow(26, d)-1) / (26-1)) - 1;
//
// given an integer x, its number of ditigs when converted to an autonumber: 
//   digits = int(log((26-1)*x + 26) / log(26));


const char* autoNumberAlphabetic::intToAlpha(int x, enum CaseType a_case) 
{
   if ( x < 0 ) {
     MESSAGE(cerr, "Negaitve alphabetic autonumber value");
     throw(CASTHCREXCEPT hardCopyRendererException());
   }

   int digits = int(log((base-1)*x + base) / log(base));

   if ( digits > 50 ) {
     MESSAGE(cerr, "alphabetic autonumber value too large");
     throw(CASTHCREXCEPT hardCopyRendererException());
   }

   //debug(cerr, digits);   
   //debug(cerr, (pow(base, digits)-1) / (25) -1);

   x -= int((pow(base, digits)-1) / (base-1)) - 1;

   char* letters = 
	(a_case == UPPER ) ? f_upperCaseLetters : f_lowerCaseLetters; 

   int y, z;
   static char buf[51], buf1[51];
   int i =0;

   while (1) {
      y = x % base;
      z = x / base;
      buf1[i++] = letters[y];
      if ( z == 0 )
         break;
      x = z;
   }

   int k;
   for (k=0; k<digits-i; k++ )
      buf[k] = letters[0];

   int n;
   for (n=0; n<i; n++ )
      buf[k+n] = buf1[n];

   buf[k+n] = 0;

   //debug(cerr, buf);
   return buf;
}

void autoNumberAlphabetic::setNextValue()
{
  if (f_serial_nums.entries() && f_values.entries())
  {
    if (f_serial_nums.top() >= 2)
      f_values.top() += f_delta;
  }
}

const char* autoNumberAlphabetic::getValue()
{
   char* ptr = f_buf.get_base();
   int ptrlen = f_buf.buf_sz();

   if (f_values.entries())
     snprintf(ptr, ptrlen, "%s", form("%s%s%s", f_prefix,
		     intToAlpha(f_values.top(), f_case), f_postfix));
   else
     *ptr = 0;

   return ptr;
}


//////////////////////////////////////////////////
//
//////////////////////////////////////////////////

char autoNumberRoman::RomanNumberBuf[256]; 

autoNumberRoman::autoNumberRoman(
		const char* nm, 
		int delta,
		CaseType ct, 
		const char* InitialValue, const char* prefix, const char* postfix
				) :
   autoNumberCased(nm, ROMAN, delta, ct, prefix, postfix)
{
   f_initialValue = RomanToArabic(InitialValue);

   f_values.push(f_initialValue);
   f_serial_nums.push(0);
}

autoNumberRoman::~autoNumberRoman()
{
}

void autoNumberRoman::setNextValue() 
{
  if (f_serial_nums.entries() && f_values.entries())
  {
    if (f_serial_nums.top() >= 2) {
      f_values.top() += f_delta;

      if (f_values.top() < 1) {
	MESSAGE(cerr, "Value too small.");
	throw(CASTHCREXCEPT hardCopyRendererException());
      }
    }
  }
}

const char* autoNumberRoman::getValue() 
{
   char* ptr;

   if (f_values.entries())
     return ArabicToRoman(f_values.top());
   else {
     ptr = f_buf.get_base();
    *ptr = 0;
   }

   return ptr;
}

int autoNumberRoman::getDigit(const char*& p)
{
   int x = 0;
   switch ( p[0] ) {
        case 'I':
        case 'i':
           if ( p[1] != 0 && p[1] == 'V' ) {
              p++;
	      x = 4;
           } else
              x = 1;
           break;
        case 'V':
        case 'v':
	   x = 5;
           break;
        case 'X':
        case 'x':
           if ( p[1] != 0 && p[1] == 'I' ) {
	      x = 9;
              p++;
           } else
              x = 10;
           break;
        case 'L': //50
        case 'l':
           if ( p[1] != 0 && p[1] == 'X' ) {
	      x = 40;
              p++;
           } else
              x = 50;
           break;
        case 'C': // 100
        case 'c':
           if ( p[1] != 0 && p[1] == 'X' ) {
	      x = 90;
              p++;
           } else
              x = 100;
           break;
        case 'D': // 500
        case 'd':
           if ( p[1] != 0 && p[1] == 'C' ) {
	      x = 400;
              p++;
           } else
              x = 500;
           break;
        case 'M': //1000
        case 'm':
           if ( p[1] != 0 && p[1] == 'C' ) {
	      x = 900;
              p++;
           } else
              x = 1000;
           break;
        default:
           MESSAGE(cerr, "unknown roman numeral letter");
           throw(CASTHCREXCEPT hardCopyRendererException());
   }
   p++;
   return x;
}

int autoNumberRoman::RomanToArabic(const char* romanString)
{
   int x = 0;
   const char* bound = romanString + strlen(romanString);
   const char* p = (char*)romanString;

   while ( p != bound ) {
      x += getDigit(p);
   }
   return x;
}

const char* romanCardinals[4][9] = 
{
   { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" },
   { "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC" },
   { "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM" },
   { "M", "MM", "MMM", "MV", "V", "VM", "VMM", "VMMM", "MX" }
};

const char* 
autoNumberRoman::ArabicToRoman(int x)
{
   unsigned int len, slen;

   RomanNumberBuf[0] = 0;
   if ( x > 3999 ) {
      MESSAGE(cerr, "Value too large.");
      throw(CASTHCREXCEPT hardCopyRendererException());
   }

   char* buf = form("%d", x);

   int j=strlen(buf)-1;
   for ( unsigned int i=0; i<strlen(buf); i++ ) {
      if ( buf[i] != '0' )
      {
	 const char* romanCardinal = romanCardinals[j][buf[i]-'1'];
	 char  precise_romanCardinal[8];

	 int k;
	 if (f_case == UPPER) {
	    for (k=0; romanCardinal[k]; k++)
		precise_romanCardinal[k] = romanCardinal[k];
	    precise_romanCardinal[k] = 0;
	 }
	 else {
	    for (k=0; romanCardinal[k]; k++)
		precise_romanCardinal[k] = tolower(romanCardinal[k]);
	    precise_romanCardinal[k] = 0;
	 }

         slen = strlen(RomanNumberBuf);
         len = MIN(strlen(precise_romanCardinal), 256 - 1 - slen);
         *((char *) memcpy(RomanNumberBuf + slen,
			   precise_romanCardinal, len) + len) = '\0';
      }
      j--;
   }

   return RomanNumberBuf;
}


////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
unsigned int
autoNumberListT::operator==(const autoNumberListT&)
{
   return false;
}

