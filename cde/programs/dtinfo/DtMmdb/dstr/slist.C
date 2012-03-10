/*
 * $XConsortium: slist.cc /main/4 1996/07/18 14:30:12 drk $
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


#include "dstr/slist.h"
#include "utility/funcs.h"

slist::slist(slist_cell* x) : v_ct(0), v_head(0), v_tail(0)
{
   if ( x )
      insert_as_tail(x);
}

slist::~slist() 
{
   slist_cell* y = 0;
   slist_cell* x = v_head;

   while (x) {
      y = x -> v_succ;
      delete x;
      x = y;
   }
}

void slist::append(slist* tail_list)
{
   if ( tail_list == 0 ) return;

   if ( v_tail != 0 )
      v_tail -> v_succ = tail_list -> v_head;

   if ( v_head == 0 )
      v_head = tail_list -> v_head;

   v_tail = tail_list -> v_tail;
   v_ct += tail_list -> v_ct;

   tail_list -> v_head = tail_list -> v_tail = 0;
   tail_list -> v_ct = 0;
}

void slist::insert_as_tail(slist_cell* x) 
{
   if ( v_head == 0 ) {
      v_head = v_tail = x;
   } else {
      v_tail -> v_succ = x;
      v_tail = x;
   }
   x -> v_succ = 0;
   v_ct++;
}

void slist::delete_head() 
{
   if ( v_ct == 1 )
       v_head = v_tail = 0;
   else {
       v_head = v_head -> v_succ;
   } 

   v_ct--;
}

long slist::first()
{
   return long(v_head);
}

long slist::last()
{
   return long(v_tail);
}

void slist::next(long& index)
{
   if ( index == long(v_tail) )
      index = 0;
   else
      index = long( ((slist_cell*)(index)) -> v_succ );
}

slist* slist::concate_with(slist* first_list ...)
{
   va_list ap;
   va_start (ap, first_list);

   this -> append(first_list);

   for (;;) {
      slist* p = va_arg(ap, slist*);
      if ( p == 0 )  
         break;
      else
         this -> append(p);
   }

   va_end(ap);

   return this;
}
