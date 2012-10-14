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
 * $TOG: LocalHistoryMgr.C /main/5 1998/04/17 11:36:56 mgreess $
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

// NOTE: this will have to be an anchor subclass
#define C_Anchor
#define L_Doc
#define L_Agents

#define C_LocalHistoryMgr
#define L_Managers

#include <Prelude.h>
#include <Marks/Anchor.hh>


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

LocalHistoryMgr::LocalHistoryMgr()
{
  f_current = NULL;
}


// /////////////////////////////////////////////////////////////////
// class destructors
// /////////////////////////////////////////////////////////////////


LocalHistoryEntry::~LocalHistoryEntry()
{
  delete f_anchor;
}

LocalHistoryMgr::~LocalHistoryMgr()
{
  // Delete all links in the list. 
  LocalHistoryEntry *e = f_first, *tmp;
  while (e != NULL)
    {
      tmp = e->f_next;
      delete e;
      e = tmp;
    }
}


// /////////////////////////////////////////////////////////////////
// truncate - add an entry to the history
// /////////////////////////////////////////////////////////////////

void
LocalHistoryMgr::truncate()
{
  if (f_current == NULL)
    return;
  // Remove all entrys after the current one.
  LocalHistoryEntry *e = f_current->f_next, *tmp;
  while (e != NULL)
    {
      tmp = e->f_next;
      delete e;
      e = tmp;
    }
  f_current->f_next = NULL;
}


// /////////////////////////////////////////////////////////////////
// append - append a new entry to the history
// /////////////////////////////////////////////////////////////////

void
LocalHistoryMgr::append (UAS_Pointer<UAS_Common> &doc_ptr)
{
  ON_DEBUG (printf ("&&&&& APPENDING <%s> to history\n",
		(char*)doc_ptr->title()));
  LocalHistoryEntry *entry = new LocalHistoryEntry (doc_ptr);
  truncate();
  entry->f_previous = f_current;
  if (f_current != NULL)
    f_current = f_current->f_next = entry;
  else
    {
      // Once the first element is set it can never be truncated. 
      f_first = f_current = entry;
    }
  entry->f_next = NULL;
}


// /////////////////////////////////////////////////////////////////
// set_anchor
// /////////////////////////////////////////////////////////////////

void
LocalHistoryMgr::set_anchor (Anchor *anchor)
{
  // Replace the current anchor with the new one. 
  delete f_current->f_anchor;
  f_current->f_anchor = anchor;
}

void
LocalHistoryMgr::library_removed (UAS_Pointer<UAS_Common> &lib) {
  //
  // The assumption in this routine is that, when it is called,
  // f_current is NOT from the dead library and thus, it will not
  // be removed in this routine.
  //

  LocalHistoryEntry *e = f_first, *tmp;
  while (e != NULL)
    {
      tmp = e->f_next;
      if (e->f_doc_ptr->get_library() == lib) {
	  if (e == f_first) {
	    f_first = tmp;
	  }
	  tmp->f_previous = e->f_previous;
	  if (e->f_previous) {
	    e->f_previous->f_next = tmp;
	  }
	  delete e;
      }
      e = tmp;
    }
}
