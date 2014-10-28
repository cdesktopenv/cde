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
 * $XConsortium: HashTbl.cc /main/3 1996/06/11 16:18:57 cde-hal $
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

#define C_HashTbl
#define L_Basic

#include "Prelude.h"

#include <memory.h>


class HashBucket : public List
{
public: // functions
  HashBucket()
    // NOTE: remove 3rd param after testing
    : List (10, 10, List::GROW_ADD) { };
  int find (FolioObject &);
  using List::remove;
  void remove (FolioObject &);
};


// /////////////////////////////////////////////////////////////////
// bucket find
// /////////////////////////////////////////////////////////////////

int
HashBucket::find (FolioObject &element)
{
  register int i;

  for (i = 0; i < f_length; i++)
    {
      if (((Hashable *)f_list_element[i])->equals ((Hashable &) element))
	return (i);
    }

  return (-1);
}


// /////////////////////////////////////////////////////////////////
// bucket remove
// /////////////////////////////////////////////////////////////////

void
HashBucket::remove (FolioObject &element)
{
  register int i;

  for (i = 0; i < f_length; i++)
    {
      if (((Hashable *)f_list_element[i])->equals ((Hashable &) element))
	{
	  // Shift back the array and overwrite deleted element. 
	  memcpy (&f_list_element[i],
		  &f_list_element[i+1],
		 sizeof (Hashable *) * (f_length - i - 1));
	  f_length--;
	  break;
	}
    }
}


// /////////////////////////////////////////////////////////////////
// constructor
// /////////////////////////////////////////////////////////////////

HashTbl::HashTbl (u_int num_buckets)
{
  f_hash_bucket = (HashBucket **) calloc (num_buckets, sizeof (HashBucket *));
  f_num_buckets = num_buckets;
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

HashTbl::~HashTbl()
{
  remove_all(1);
  free ((char *) f_hash_bucket);
}


// /////////////////////////////////////////////////////////////////
// add - add an entry to the hash table
// /////////////////////////////////////////////////////////////////

void
HashTbl::add (Hashable &element)
{
  /* -------- Find the hash value for this element. -------- */
  u_int where = element.hash_code(0, f_num_buckets-1);

  /* -------- Create a bucket if it doesn't exist yet. -------- */
  if (f_hash_bucket[where] == NULL)
    f_hash_bucket[where] = new HashBucket();

  /* -------- See if it exists. -------- */
  if (f_hash_bucket[where]->find (element) == -1)
    /* -------- Finally, add it. -------- */
    f_hash_bucket[where]->append (element);
}


// /////////////////////////////////////////////////////////////////
// find - find an entry in the hash table
// /////////////////////////////////////////////////////////////////

int
HashTbl::find (Hashable &element) const
{
  u_int where = element.hash_code (0, f_num_buckets - 1);

  if (f_hash_bucket[where] != NULL)
    return (f_hash_bucket[where]->find (element));
  else
    return (-1);
}


// /////////////////////////////////////////////////////////////////
// remove - remove an entry from the hash table
// /////////////////////////////////////////////////////////////////

void
HashTbl::remove (Hashable &element)
{
  u_int where = element.hash_code (0, f_num_buckets - 1);
  if (f_hash_bucket[where] != NULL)
    f_hash_bucket[where]->remove (element);
}


// /////////////////////////////////////////////////////////////////
// remove_all - delete all elements in the hash table
// /////////////////////////////////////////////////////////////////

void
HashTbl::remove_all (bool delete_elements)
{
  for (unsigned int i = 0; i < f_num_buckets; i++)
    if (f_hash_bucket[i] != NULL)
      {
	if (delete_elements)
	  f_hash_bucket[i]->remove_all (TRUE);
	delete f_hash_bucket[i];
	f_hash_bucket[i] = NULL;
      }
}
