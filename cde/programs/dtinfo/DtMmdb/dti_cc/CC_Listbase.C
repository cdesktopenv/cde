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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: CC_Listbase.C /main/4 1998/04/17 11:44:10 mgreess $
 * $TOG: CC_Listbase.C /main/4 1998/04/17 11:44:10 mgreess $
 * $TOG: CC_Listbase.C /main/4 1998/04/17 11:44:10 mgreess $
 * $TOG: CC_Listbase.C /main/4 1998/04/17 11:44:10 mgreess $
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
#include "Exceptions.hh"
#include "cc_exceptions.h"
#include "CC_Listbase.h"

// /////////////////////////////////////////////////////////////////
// CC_Listbase::insert - append a new link to the end of the list
// /////////////////////////////////////////////////////////////////

void
CC_Listbase::insert (CC_Link_base *element)
{
  if (!element) {
    throw(CASTCCEXCEPT ccException());
  }

  if (!f_tail) {
    f_tail = f_head = element;
  }
  else {
    f_tail->f_next = element;
    element->f_prev = f_tail;
    f_tail = element;
  }

  f_length++;
}


// /////////////////////////////////////////////////////////////////
// CC_Listbase::insert - insert a new link into the list
// /////////////////////////////////////////////////////////////////

void
CC_Listbase::prepend (CC_Link_base *element)
{

  if ( !element ) {
    throw(CASTCCEXCEPT ccException());
  }

  if ( !f_head ) {
    f_head = element;
  }
  else {
    element->f_next = f_head;
    f_head->f_prev = element;
    f_head = element;
  }

  if (f_tail == NULL)
    f_tail = element;
  f_length++;
}



// /////////////////////////////////////////////////////////////////
// CC_Listbase::remove - remove element pointed to by iterator
// /////////////////////////////////////////////////////////////////
CC_Link_base *
CC_Listbase::remove (CC_List_Iterator_base &iterator)
{
  // Make sure the iterator points to this this.
  if (iterator.f_list != this)
    throw (CASTCCEXCEPT ccException());

  // Make sure the iterator is pointing to an element. 
  if (iterator.f_current == NULL)
    return(NULL);

  // NOTE: If two iterators are active in the list at the same time
  // it is possible to blow away an element that another iterator
  // is pointing at (either previous or current).  We could make this
  // safer by only marking elements as deleted for now, then actually
  // delete the items when there are no more iterators pointing at it.
  //    19:41 22-Jul-93 DJB 

  // Link around the link we're removing.
  if (iterator.f_previous != NULL) {
    iterator.f_previous->f_next = iterator.f_current->f_next;
    if ( f_tail != iterator.f_current ) { 
      iterator.f_current->f_next->f_prev  = iterator.f_previous;
    }
  }
  else {   // must be at the head 
    f_head = iterator.f_current->f_next;
    if ( f_head ) {
      f_head->f_prev = NULL;
    }
  }
  
  if (iterator.f_current == f_tail) {
    f_tail = iterator.f_previous;
    if ( f_tail ) { 
      f_tail->f_next = NULL;
    }
    
  }

  // Increment the iterator. 
  CC_Link_base *entry = iterator.f_current;
  iterator.f_current = iterator.f_current->f_next;

  f_length--;

  return (entry);
}

CC_Link_base *
CC_Listbase::removeFirst()
{
  if ( f_head ) {

    CC_Link_base *remove_item = f_head;
    f_head = f_head->f_next;
    remove_item->f_next = NULL;

    if ( f_head ) {
      f_head->f_prev = NULL;
    }

    if ( f_tail == remove_item ) { /* only one item on the list */
      f_tail = NULL;
    }

    f_length--;
    return(remove_item);
  }

  return(NULL);
}

//------------------------------------------------------------------
CC_Link_base *
CC_Listbase::removeLast()
{
  if ( f_tail ) {
    CC_Link_base *remove_item = f_tail;
    f_tail = f_tail->f_prev;
    remove_item->f_prev = NULL;

    if ( f_tail ) {
      f_tail->f_next = NULL;
    }
    if ( f_head == remove_item ) { // one item left on the list
      f_head = NULL;
    }
    f_length--;
    return(remove_item);
  }

  return(NULL);
}

// /////////////////////////////////////////////////////////////////
// CC_List_Iterator::CC_List_Iterator - class constructor
// /////////////////////////////////////////////////////////////////

CC_List_Iterator_base::CC_List_Iterator_base (CC_Listbase *list)
: f_list (list)
{
  if ( !list ) {
    throw(CASTCCEXCEPT ccException());
  }

  reset();
}

// /////////////////////////////////////////////////////////////////
// CC_List_Iterator::reset - reset the iterator to the list start
// /////////////////////////////////////////////////////////////////

void
CC_List_Iterator_base::reset()
{
  f_current = f_previous = NULL;
}


// /////////////////////////////////////////////////////////////////
// CC_List_Iterator:: operator ++ - increment the list iterator
// /////////////////////////////////////////////////////////////////

CC_Boolean 
CC_List_Iterator_base::operator++()
{
  if (!f_current) { // havn't touched the first element 
    f_current = f_list->first();
    f_previous = NULL;
    if ( f_current ) return ( TRUE );
    else return(FALSE);
  }

  f_previous = f_current;
  f_current = f_current->f_next;

  if ( f_current ) {
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}      

CC_Boolean
CC_List_Iterator_base::operator--()
{
  if (!f_current) {
    return(FALSE);
  }

  f_current = f_previous;
  if ( f_previous ) {
    f_previous = f_previous->f_prev;
  }

  if ( f_current ) { return(TRUE); }
  else { return(FALSE); }
}
    
