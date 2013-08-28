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
 * $XConsortium: misc.cc /main/4 1996/07/18 14:39:26 drk $
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


#include "mgrs/misc.h"

Boolean name_oid_ls(const void* o1, const void* o2)
{
   name_oid_t *x = (name_oid_t*)o1;
   name_oid_t *y = (name_oid_t*)o2;

/*
MESSAGE(cerr, "in name_oid_ls");
debug(cerr, x -> v_name);
debug(cerr, y -> v_name);
*/


   return ( x -> v_name.string_LS(y -> v_name) );
}

Boolean name_oid_eq(const void* o1, const void* o2)
{
   name_oid_t *x = (name_oid_t*)o1;
   name_oid_t *y = (name_oid_t*)o2;

/*
MESSAGE(cerr, "in name_oid_eq");
debug(cerr, x -> v_name);
debug(cerr, y -> v_name);
*/
   return ( x -> v_name.string_EQ(y -> v_name) );
}

Boolean oid_ls(const void* o1, const void* o2)
{
   const oid_t& x = ((handler*)o1) -> its_oid();
   const oid_t& y = ((handler*)o2) -> its_oid();

/*
MESSAGE(cerr, "<<<<<<<<<<<<");
x.asciiOut(cerr); cerr << "\n";
y.asciiOut(cerr); cerr << "\n";
MESSAGE(cerr, "<<<<<<<<<<<<");
*/

   return x.ls(y);
}

Boolean oid_eq(const void* o1, const void* o2)
{
   const oid_t& x = ((handler*)o1) -> its_oid();
   const oid_t& y = ((handler*)o2) -> its_oid();

/*
MESSAGE(cerr, "==========");
x.asciiOut(cerr); cerr << "\n";
y.asciiOut(cerr); cerr << "\n";
MESSAGE(cerr, "==========");
*/

   return x.eq(y);
}

Boolean oid_storage_ls(const void* o1, const void* o2)
{
   name_oid_t* x = (name_oid_t*)o1;
   name_oid_t* y = (name_oid_t*)o2;

   if ( x->v_oid.ls(y->v_oid) == true )
      return true;

 
   if ( x->v_oid.eq(y->v_oid) == true )
       if ( long(x->v_store) < long(y->v_store) )
          return true;

   return false;
}

Boolean oid_storage_eq(const void* o1, const void* o2)
{
   name_oid_t* x = (name_oid_t*)o1;
   name_oid_t* y = (name_oid_t*)o2;

   return (x->v_oid.eq(y->v_oid) == true && x->v_store && y->v_store ) ? 
          true : false;
}

void delete_name_oid_rec_f(const void* name_oid_ptr)
{
   name_oid_t* x = (name_oid_t*)name_oid_ptr;
//debug(cerr, x -> v_name.get());
   delete x;
}

// ***************************************************
//
//
// ***************************************************

mark_t::mark_t(char* marks) : ostring(strlen(marks))
{
   set(marks);
}

istream& operator >>(istream& in, mark_t& m)
{
   char c ;
   char* ptr = m.get();
   Boolean read_marks = false;

   while ( in && in.get(c) ) {

      if ( strchr(ptr, c) == NULL ) {
         in.putback(c);
         if ( read_marks == false ) {
            debug(cerr, c); 
            throw(formatException("bad mark to read"));
         } 
         return in;
      } else {
         read_marks = true;
      }
   }

   return in;
}

ostream& operator <<(ostream& out, mark_t& m)
{
   out << m.get();
   return out ;
}

