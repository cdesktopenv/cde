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
 * $XConsortium: OutlineList.C /main/4 1996/10/04 11:13:58 drk $
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

#define C_OutlineList
#define C_OutlineElement
#define L_Basic

#include "Prelude.h"

#define OUTLINE_ELEMENT(I) ((OutlineElement *)f_list_element[i])


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

OutlineList::OutlineList (int initial_size, int increment,
			  List::grow_method_t grow_method)
: List (initial_size, increment, grow_method)
{
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

OutlineList::~OutlineList ()
{
  // recursively nuke lists
}


// /////////////////////////////////////////////////////////////////
// count_expanded - return the number expanded below + number here
// /////////////////////////////////////////////////////////////////

u_int
OutlineList::count_expanded (BitHandle data_handle)
{
  u_int i, count = 0;

  for (i = 0; i < f_length; i++)
    if (OUTLINE_ELEMENT(i)->is_expanded (data_handle) &&
	OUTLINE_ELEMENT(i)->children() != NULL)
      count += OUTLINE_ELEMENT(i)->f_children->count_expanded (data_handle);

  count += f_length;

  return (count);
}


// /////////////////////////////////////////////////////////////////////////
// copy_selected - copy selected flags from src pos to dest pos
// /////////////////////////////////////////////////////////////////////////

void
OutlineList::copy_selected(BitHandle src, BitHandle dest)
{
  for (unsigned int i = 0 ; i < length(); i++)
    {
      if (OUTLINE_ELEMENT(i)->is_selected(src))
	OUTLINE_ELEMENT(i)->set_selected(dest) ;
      else
	OUTLINE_ELEMENT(i)->unset_selected(dest);
      // do children
      if (OUTLINE_ELEMENT(i)->children_cached() &&
	  OUTLINE_ELEMENT(i)->has_children())
	OUTLINE_ELEMENT(i)->children()->copy_selected(src, dest);
    }
}


// /////////////////////////////////////////////////////////////////////////
// copy_expanded - copy expanded flags from src pos to dest pos
// /////////////////////////////////////////////////////////////////////////

void
OutlineList::copy_expanded (BitHandle src, BitHandle dest)
{
  for (unsigned int i = 0 ; i < length(); i++)
    {
      if (OUTLINE_ELEMENT(i)->is_expanded (src))
	OUTLINE_ELEMENT(i)->set_expanded (dest);
      else
	OUTLINE_ELEMENT(i)->set_contracted (dest);
      // do children
      if (OUTLINE_ELEMENT(i)->children_cached() &&
	  OUTLINE_ELEMENT(i)->has_children())
	OUTLINE_ELEMENT(i)->children()->copy_expanded (src, dest);
    }
}


// /////////////////////////////////////////////////////////////////////////
// recursive_select - if item is (un)selected, (un)select all children
// /////////////////////////////////////////////////////////////////////////

void
OutlineList::recursive_select(BitHandle data_handle)
{
  for (unsigned int i = 0 ; i < length() ; i++ )
    if (OUTLINE_ELEMENT(i)->children_cached() &&
	OUTLINE_ELEMENT(i)->has_children())
      {
	if (OUTLINE_ELEMENT(i)->is_selected(data_handle))
	  OUTLINE_ELEMENT(i)->children()->select_all(data_handle);
	else
	  OUTLINE_ELEMENT(i)->children()->deselect_all(data_handle);
      }
}


// /////////////////////////////////////////////////////////////////////////
// select_all - select all items and same for children
// /////////////////////////////////////////////////////////////////////////

void
OutlineList::select_all(BitHandle data_handle)
{
  for (unsigned int i = 0 ; i < length(); i++)
    {
      OUTLINE_ELEMENT(i)->set_selected(data_handle);
      if (OUTLINE_ELEMENT(i)->children_cached() &&
	  OUTLINE_ELEMENT(i)->has_children())
	OUTLINE_ELEMENT(i)->children()->select_all(data_handle);
    }
  
}


// /////////////////////////////////////////////////////////////////////////
// deselect_all - unselect all items and same for children
// /////////////////////////////////////////////////////////////////////////

void
OutlineList::deselect_all(BitHandle data_handle)
{
  for (unsigned int i = 0 ; i < length(); i++)
    {
      OUTLINE_ELEMENT(i)->unset_selected(data_handle);
      if (OUTLINE_ELEMENT(i)->children_cached() &&
	  OUTLINE_ELEMENT(i)->has_children())
	OUTLINE_ELEMENT(i)->children()->deselect_all(data_handle);
    }
  
}


// /////////////////////////////////////////////////////////////////
// OutlineElement::print - print representation of self
// /////////////////////////////////////////////////////////////////

#ifdef DEBUG
#undef PRINT_POINTER
#define PRINT_POINTER(PTR) \
  printf ("  PTR = 0x%04lx\n", PTR)
#undef PRINT_BITS
#define PRINT_BITS(BITS) \
  { long bits = BITS; printf ("  BITS = 0x%04x", BITS); \
    for (int i = 0; i < sizeof(BITS) * 8; i++, bits <<= 1) { \
      if (!(i%8)) putchar (' '); \
      (bits & (1L << (sizeof(BITS) * 8)-1)) ? putchar('1') : putchar('0'); } \
    putchar ('\n'); } 

void
OutlineElement::print ()
{
#if 0
  PRINT_OBJECT (OutlineElement);
  PRINT_POINTER (f_object);
  PRINT_POINTER (f_children);
  PRINT_INT (f_level);
  PRINT_BITS (f_selected);
  PRINT_BITS (f_expanded);
  PRINT_POINTER (f_xm_string);
#else
  puts ("WARNING: Can't print OutlineElement due to source code restriction.");
#endif
}
#endif /* DEBUG */

// /////////////////////////////////////////////////////////////////
// selected_items
// /////////////////////////////////////////////////////////////////

List *
OutlineList::selected_items (BitHandle data_handle, List *l)
{
  if (l == NULL)
    l = new List (8, 8, List::GROW_MULTIPLY);

  for (unsigned int i = 0; i < length(); i++)
    {
      if (OUTLINE_ELEMENT(i)->is_selected (data_handle))
	l->append (OUTLINE_ELEMENT(i));
      if (OUTLINE_ELEMENT(i)->children_cached() &&
	  OUTLINE_ELEMENT(i)->has_children())
	OUTLINE_ELEMENT(i)->children()->selected_items (data_handle, l);
    }

  return (l);
}
