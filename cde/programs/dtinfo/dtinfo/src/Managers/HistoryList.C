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
 * $XConsortium: HistoryList.C /main/4 1996/12/11 18:19:25 cde-hal $
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

#define C_NodeObject
#define L_Odb

#define C_PrefMgr
#define C_HistoryList
#define C_NodeHistoryMgr
#define C_NodeMgr
#define L_Managers

#include "Prelude.h"

// /////////////////////////////////////////////////////////////////
// constructor
// /////////////////////////////////////////////////////////////////

// give List::List() an increment size so that it doesn't complain 
// when the default_size is zero 

HistoryList::HistoryList (int default_size)
 : List (default_size, default_size ? default_size : 1)
{
  f_first_element = 0;
}

// /////////////////////////////////////////////////////////////////
// remove_first - remove the first item in the history list
// /////////////////////////////////////////////////////////////////

void
HistoryList::remove_first()
{
  if (f_length == 0)  
    return;

  FolioObject* &object = f_list_element[f_first_element];

  if (object) {
    delete object; 
    object = NULL;
  }

  f_first_element = (f_first_element + 1) % f_internal_length;

  f_length--;

  notify (APPENDED);
}

// /////////////////////////////////////////////////////////////////
// append - add an item to the history list
// /////////////////////////////////////////////////////////////////

void
HistoryList::append (FolioObject &object)
{
  // if no history, do not append 
  if (f_internal_length == 0)
    return ;

  ON_DEBUG (printf ("Appending to history elt = %d, max = %d\n",
		    f_length, f_internal_length));

  // See if the history list is full.
  if (f_length == f_internal_length)
    {
      // When full, the first element in the history gets bumped 
      // and we add an new element at the end of the list.
      delete f_list_element[f_first_element];
      f_list_element[f_first_element] = &object;
      f_first_element = (f_first_element + 1) % f_internal_length;
    }
  else // Not full yet
    {
      f_list_element[(f_first_element + f_length) % f_internal_length] = &object;
      f_length++;
    }
  notify (APPENDED);
}

// /////////////////////////////////////////////////////////////////
// set_length - resize the length of the history list
// /////////////////////////////////////////////////////////////////

void
HistoryList::set_length (int new_size)
{
  // NOTE: perhaps check for bogus size (like 0!!)
  // Can't just realloc because of the ring structure.

  /* -------- Allocate new list. -------- */
  FolioObject **new_list =
   (FolioObject **) malloc (sizeof (FolioObject *) * new_size);

  /* -------- Copy the old list to the new list. -------- */
  int i;
  // Copy as many elements as possible without overrunning new space.
  int limit = (f_length < new_size) ? f_length : new_size;

  unsigned int first_element = f_first_element;
  // Skip over excess entries at the beginning of the list if needed. 
  if (new_size < f_length)
    {
      int extras = f_length - new_size;
      f_first_element = (f_first_element + extras) % f_internal_length;
    }
  
  for (i = 0; i < limit; i++)
    new_list[i] = f_list_element[(f_first_element + i) % f_internal_length];

  /* -------- Nuke any elements that didn't fit. -------- */
  if (new_size < f_length)
    for (i = 0 ; i < (f_length - new_size) ; i++)
      delete f_list_element[(first_element + i) % f_internal_length];

  /* -------- Set up and swap in the new list. -------- */
  f_first_element = 0;
  if (new_size < f_length)
    f_length = new_size;
  f_internal_length = new_size;
  ON_DEBUG (printf ("History resized to max %d\n", f_internal_length));

  free ((char *)f_list_element);

  f_list_element = new_list;

  notify(APPENDED);
}

