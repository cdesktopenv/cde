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
 * $XConsortium: ostring.cc /main/3 1996/06/11 17:38:07 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#include "utility/ostring.h"

#ifdef C_API
char* ostring::input_buf = 0;
#else
char ostring::input_buf[LBUFSIZ];
#endif

ostring::ostring() : v_sz(0), v_allo_sz(1)
{
   v_p = new char[1];
}

ostring::ostring(const int i) : v_sz(0), v_allo_sz(i+1)
{
   v_p = new char[i+1];
}

ostring::ostring(char* x, const int i) 
{
   int w = i;

   if ( w == -1 ) {
      w = strlen(x);
   }

   v_sz = w;
   v_allo_sz = w+1;

   v_p = new char[w+1];
   memcpy(v_p, x, w);
   v_p[w] = 0;
}

ostring::ostring(const ostring& s) : 
v_sz(s.v_sz), v_allo_sz(s.v_allo_sz)
{
   v_p = new char[v_allo_sz];
   memcpy(v_p, s.v_p, v_sz);
   v_p[v_sz] = 0;
}

/*
ostring::~ostring()
{
   delete v_p;
}
*/

Boolean ostring::set(const char* x, int l)
{
   expand(l+1);

   memcpy(v_p, x, l);
   v_p[l] = 0;
   v_sz = l;

   return true;
}

/*
Boolean ostring::set(const char* x)
{
   return set(x, strlen(x));
}

void ostring::reset()
{
   v_sz = 0;
}

void ostring::set_size(const int s)
{
   v_sz = s;
   v_p[v_sz] = 0;
}
*/

/********************************************/
// set alloc_sz to at least new_alloc_sz bytes
/********************************************/
Boolean ostring::expand(const int new_alloc_sz, Boolean pre_zero)
{
   if ( new_alloc_sz > v_allo_sz ) {
      v_allo_sz = new_alloc_sz+1;
      char* new_p = new char[v_allo_sz];

      if ( pre_zero == true )
         memset(new_p, char(0), v_allo_sz);
  
      if ( v_p ) {
         memcpy(new_p, v_p, v_sz);
         delete v_p;
      }

      v_p = new_p;
   }
   return true;
}

char* ostring::acquire() 
{          
   v_allo_sz = v_sz = 0;
   char *x = v_p;
   v_p = 0;
   return x; 
}

Boolean ostring::append(const char* x, int l)
{
   expand(v_sz+l+1);

   memcpy(v_p+v_sz, x, l);
   v_sz += l;
   v_p[v_sz] = 0;
   return true;
}

Boolean ostring::update(const char* x, int l, int offset)
{
   if ( offset + l > v_sz ) {
      MESSAGE(cerr, "update(): char chunk too small");
      throw(boundaryException(0, v_sz, offset+l));
   }

   memcpy(v_p+offset, x, l);
   return true;
}

int ostring::substr(ostring& s)
{
   if ( v_p == 0 || s.v_p == 0 )
      return -1;

   char* sub_p = strstr(v_p, s.v_p);

   if ( sub_p == 0 )
      return -1;
   else
      return (int)(sub_p - v_p);
}

/*
int ostring::size() const
{
   return v_sz;
}

int ostring::alloc_size() const
{
   return v_allo_sz;
}
*/

ostring& ostring::operator +(ostring& s)
{
   int l1 = v_sz;
   int l2 = s.v_sz;

   ostring *new_ostring = new ostring(l1+l2);

   int i;
   for ( i = 0; i<l1; (*new_ostring).v_p[i] = s.v_p[i] ) i++;
   for ( i = 0; i<l2; (*new_ostring).v_p[l1 + i] = s.v_p[i] ) i++;

   return *new_ostring;
}

Boolean ostring::string_LS(ostring& y) const
{
   char* x_str = this -> get() ;
   char* y_str = y.get() ;

   int x_sz = this -> size() ;
   int y_sz = y.size() ;

   if ( x_sz == y_sz ) {
      if ( memcmp(x_str, y_str, x_sz ) < 0 )
         return true;
      else
         return false;
   } else {

      int min = MIN(x_sz, y_sz);

      for ( int i=0; i<min; i++ ) {
         if ( x_str[i] < y_str[i] ) 
            return true;
         else
         if ( x_str[i] > y_str[i] ) 
            return false;
      }
      if ( x_sz < y_sz )
         return true;
      else
         return false;
   }
}

Boolean ostring::string_EQ(ostring& y) const
{
  if ( this -> size() == y.size() &&
       memcmp(this -> get(), y.get(), this -> size() ) == 0 
     )
     return true;
  else
     return false;
}

ostream& operator <<(ostream& s, const ostring& o)
{
   if ( o.v_p ) {
      //s << o.v_sz << ":";
      for ( int i=0; i<o.v_sz; i++ )
     //    if ( isprint(o.v_p[i]) )
            s << o.v_p[i];
     //    else
     //       s << int(o.v_p[i]);
   }
   return s;
}

istream& operator >>(istream& s, ostring& o)
{
   s.getline( o.input_buf, LBUFSIZ );
   o.set(o.input_buf);
   return s;
}

   
ostring* ostring::operator+= (ostring* )
{
  return 0;
}
   
ostring* ostring::concate_with(...)
{
  return 0 ;
}

