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
// $XConsortium: iostream.C /main/4 1996/08/21 15:54:53 drk $

#include "utility/c_iostream.h"
#include <stdio.h>
#include <ctype.h>

istream&        
istream::seekg(streampos delta, ios::seek_dir d) 
{
   if ( fail() ) return *this;

   if ( d != ios::beg || sbuf -> seekg(delta) == EOF ) {
     set_bad();
     set_fail();
   }
   return *this;
}

int istream::get()
{
   return sbuf -> get();
}

istream& istream::get(char& c)
{
   if ( fail() ) return *this;

   int i = sbuf -> get();

   if ( i == EOF ) {
     set_fail();
     return *this;
   }

   c = i;
   return *this;
}

istream&        
istream::putback(char c)
{
   sbuf -> putback(c);
   return *this;
}

istream& istream::getline(char* b, int lim, char delim)
{
   return _getline(b, lim, delim, 1);
}

istream& istream::_getline(char* b, int lim, int delim, int fill_zero)
{
   if ( fail() ) return *this;

   if ( sbuf -> examine() == EOF ) {
     set_fail();
     return *this;
   }

   sbuf -> clear_gcount();

   int i;

   int count;
   for ( count = 0 ; count < lim-1; count++ ) {

      i = sbuf -> get();

      if ( i == EOF || i == delim ) {
//fprintf(stderr, "prematual break in _getline(): i=%d, count = %d\n", i, count);
        break;
      }

      b[count] = char(i);
   }

   if ( fill_zero )
      b[count] = 0;

   return *this;
}

istream&        
istream::read(char* s, int n)
{
   return _getline(s, n+1, EOF, 0);
}

int             
istream::gcount() 
{
   return sbuf -> gcount();
}

istream& istream::operator>>(char& c)
{
   int x;

   if ( (x=sbuf->examine()) == EOF ) return *this;

   c = (char)x;

   sbuf -> get();

   return *this;
}

int istream::eatw()
{
   if ( fail() ) return EOF;

   int c = sbuf->examine();

   if (c == EOF) set_fail();
   
   while (isspace(c) && c != EOF) {
      sbuf->get();
      c = sbuf->examine();
   }

   if ( c == EOF ) set_fail();

   return c;
}

istream& istream::operator>>(char* s)
{
   int c;
   if ( (c=eatw()) == EOF ) return *this;

   do {
      *s++ = c;
      sbuf->get();
      c = sbuf -> examine();
   } while (!isspace(c) && c != EOF) ;
   
   *s = '\0';

   if (c == EOF) {
     set_fail();
   }

   return *this;
}

istream&        
istream::operator>>(unsigned short& x)
{
   unsigned int l = 0;

   *this >> l;

   x = (unsigned short)l;

   return *this;
}

istream&        
istream::operator>>(unsigned int& n)
{
   int x;
   if ( (x=eatw()) == EOF ) return *this;

   n = 0;

   if ( isdigit(x) ) {
      do {
          sbuf -> get();
          n = n*10+x-'0';
      } while (isdigit(x=sbuf->examine()));
   } else {
       set_fail();
       return *this;
   }

   return *this;


}

istream&        
istream::operator>>(int& x)
{
   long l = 0;

   *this >> l;

   x = int(l);

   return *this;
}

istream&        
istream::operator>>(long& n)
{
   int x;

   if ( (x=eatw()) == EOF ) return *this;

   int sign = '+';

   switch (x) {
     case '+':
     case '-':
       sign = x;
       sbuf -> get();
       x = sbuf -> examine();
       break;
     case EOF:
       set_fail();
       return *this;
   }

   n = 0;
   if ( isdigit(x) ) {
      do {
          sbuf -> get();
          n = n*10+x-'0';
      } while (isdigit(x=sbuf->examine()));
      if (sign=='-') 
        n = -n;
   } else {
       set_fail();
       return *this;
   }

   return *this;
}

/////////////////////
ostream&        
ostream::operator<<(void* a)
{
   if ( fail() ) return *this;
   long x = (long)a;
   *this << x;
   return *this;
}

ostream&        
ostream::operator<<(const char* str)
{
   if ( str == 0 ) {
     set_bad();
     return *this;
   }

   while (*str) {
     if (sbuf->put(*str++) == EOF) {
        set_fail();
        break;
     }
   } 

   return *this;
}

ostream&        
ostream::operator<<(char c)
{
   if ( fail() ) return *this;
   sbuf -> put(c);
   return *this;
}

ostream&        
ostream::operator<<(int i)
{
   long x = i;
   *this << x;
   return *this;
}

ostream&        
ostream::operator<<(unsigned int l)
{
   long x = l;
   *this << x;
   return *this;
}

ostream&        
ostream::operator<<(long x)
{
   if ( fail() ) return *this;

   char buf[32];
   char *p = buf;

   if (x < 0) {
     sbuf->put('-');
     x = -x;
   } 

   do {
      *p++ = '0' + char(x%10);
      x /= 10;
   } while (x > 0);

   do {
     if (sbuf->put(*--p) == EOF) {
        set_fail();
        break;
     }
   } while (p != buf);


   return *this;
}

ostream&        
ostream::operator<< (ostream& (*f)(ostream&))
{
   return (*f)(*this) ;
}

ostream& ostream::put(char c)
{
   if ( fail() ) return *this;
   sbuf -> put(c);
   return *this;
}

ostream& ostream::flush() 
{
   if ( fail() ) return *this;
   sbuf -> flush();
   return *this;
}

ostream&        
ostream::write(const char* s, int n)
{
   for ( int i=0; i<n; i++ ) {
     if ( sbuf->put(s[i]) == EOF )
        break;
   }
   return *this;
}

ostream& endl(ostream& out) 
{
   return out << '\n';
}

istream::istream(streambuf* sb) : ios(sb)
{
   sbuf = sb;
}

ostream::ostream(streambuf* sb) : ios(sb)
{
   sbuf = sb;
}

iostream::iostream(streambuf* sb) : istream(sb), ostream(sb)
{
   sbuf = sb;
}

