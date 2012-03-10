/*
 * $XConsortium: void_ptr_array.cc /main/4 1996/07/18 14:31:06 drk $
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


#include "void_ptr_array.h"

void_ptr_array::void_ptr_array(void_ptr_array& original) : 
buffer(original), v_elmts(original.v_elmts)
{
}

void_ptr_array::void_ptr_array(int exp_sz) : 
buffer(exp_sz * sizeof(voidPtr)), v_elmts(0)
{
   memset(get_base(), (char)0, buf_sz());
}

Boolean void_ptr_array::expandWith(int extra_void_ptrs)
{
   return buffer::expand_chunk( (count() + extra_void_ptrs) * sizeof(void*) );
}

/*
void void_ptr_array::reset_v_elmts(int i)
{
   v_elmts = i;
}

int void_ptr_array::no_v_elmts()
{
   return v_elmts;
}
*/

Boolean void_ptr_array::insert(void* elmt, int i)
{
/*
MESSAGE(cerr, "INSERT to void_ptr_array");
debug(cerr, int(i));
debug(cerr, int(elmt));
*/

   if ( !INRANGE(i, 0, count()-1) ) {
      expandWith( 2*i - count() );
   }

   memcpy(
         v_base + i * sizeof(voidPtr),
         (char*)&elmt,
         sizeof(elmt)
        );

   return true;
}

void void_ptr_array::reset_vptr(voidPtr v)
{
   for ( int i=0; i<count(); i++) {
      insert(v, i);
   }
}

ostream& operator <<(ostream& out, void_ptr_array& va)
{
   for (int i=0; i<va.no_elmts(); i++) {
      out << long(va[i]) << " ";
   }
   out << "\n";
   return out;
}
