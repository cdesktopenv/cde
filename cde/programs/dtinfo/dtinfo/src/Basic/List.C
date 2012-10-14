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
 * $XConsortium: List.cc /main/4 1996/08/06 09:18:55 rcs $
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

#define C_List
#define L_Basic

#include "Prelude.h"


#ifdef USL

strcasecmp(register const char *s1,
	   register const char *s2)
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

#endif


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

List::List (int initial_size, int increment, grow_method_t method,
	    bool delete_elements)
: f_dtor_delete_elements (delete_elements)
{
  assert (initial_size >= 0);
  f_length = 0;
  f_internal_length = initial_size;
  f_grow_method = method;
  if (increment == 0)
    f_increment = initial_size;
  else
    f_increment = increment;
  assert (f_increment > 0);
  f_list_element = (FolioObject **)
    malloc (sizeof (FolioObject *) * initial_size);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

List::~List()
{
  if (f_dtor_delete_elements)
    remove_all (TRUE);
  free ((char *) f_list_element);
}

// /////////////////////////////////////////////////////////////////
// check_space - create more space for elements if necessary
// /////////////////////////////////////////////////////////////////

void
List::check_space (int num_additions)
{
  if (f_length + num_additions > f_internal_length)
    {
      // Loop until internal_length has grown large enough. 
      while (f_length + num_additions > f_internal_length)
	{
	  switch (f_grow_method)
	    {
	      case GROW_ADD:
	      f_internal_length = f_internal_length + f_increment;
	      break;
	      case GROW_MULTIPLY:
	      f_internal_length = f_internal_length * f_increment;
	      break;
	      default:
	      abort ();		// harsh!
	    }
	}

      f_list_element =
	(FolioObject **) realloc ((char *) f_list_element,
				  sizeof (FolioObject *) * f_internal_length);
    }
}    


// /////////////////////////////////////////////////////////////////
// append - append an element to the list
// /////////////////////////////////////////////////////////////////

void
List::append (FolioObject &element)
{
  check_space();

  /* -------- Add the element. -------- */
  if (&element == NULL)
    abort();
  f_list_element[f_length] = &element;
  f_length++;
  notify (APPENDED, (void *)(size_t) (f_length - 1));
}


// /////////////////////////////////////////////////////////////////
// remove - remove an element from the list 
// /////////////////////////////////////////////////////////////////

void
List::remove (FolioObject &element)
{
  /* -------- Look for the element in the list -------- */
  int location = find (element);

  if (location != -1)
    remove (location);
}


// /////////////////////////////////////////////////////////////////
// remove - remove an element from the list 
// /////////////////////////////////////////////////////////////////

void
List::remove (unsigned int location)
{
  // NOTE: check bounds 
  // Shift the array back and overwrite deleted element.
  f_length--;
  for (int i = location; i < f_length; i++)
    f_list_element[i] = f_list_element[i+1];
}


// /////////////////////////////////////////////////////////////////
// insert - insert an element in the list
// /////////////////////////////////////////////////////////////////

void
List::insert (unsigned int location, FolioObject *element)
{
  check_space();

  if (location > f_length)
    abort();
 
  // Shift the array forward to make room for new insertion.
  for (int i = f_length; i > location; i--)
    f_list_element[i] = f_list_element[i-1];

  // Insert the new element in the list.
  f_list_element[location] = element;
  f_length++;
  notify (INSERTED, (void *)(size_t) location);
}


// /////////////////////////////////////////////////////////////////
// find - find an element in a list
// /////////////////////////////////////////////////////////////////

int
List::find (FolioObject &element)
{
  /* -------- Search through the list, looking for element. -------- */
  register int i;

  for (i = 0; i < f_length; i++)
    if (f_list_element[i] == &element)
      return (i);

  return (-1);
}


// /////////////////////////////////////////////////////////////////
// copy
// /////////////////////////////////////////////////////////////////

List *
List::copy() const
{
    // copy the contents of the list into a new one 

    // NOTE: use length() instead of f_length because it may be more general
    List *retlist = new List(length());
    for (int i = 0 ; i < length(); i++)
      retlist->append((*this)[i]);

    return retlist ;
}


// /////////////////////////////////////////////////////////////////
// remove_all - remove all the elements from the list
// /////////////////////////////////////////////////////////////////

void
List::remove_all (bool)
{
  unsigned short length = f_length;
  // Set length to 0, in case object dtor tries to mess with the list. 
  f_length = 0;
  // Reverse order delete is important for Long_Lived objects!! 
  while (length)
    delete f_list_element[--length];
}


/*

  NOTE: Here's how to make the list use less space per list:

  Make length an internal_length shorts.

  change grow_method to a bit field of length 1.
  change increment to a bit field of length 7.

  if grow_method is multiple, use the increment as a literal.
  if grow_method is add, use a static lookup table based on
  the increment value.

*/

