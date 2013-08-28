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
 * $TOG: MarkBase_mmdb.C /main/8 1998/04/17 11:37:49 mgreess $
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

#include <sstream>
using namespace std;


#define C_xList
#define L_Support


#define C_MarkBase
#define C_Mark
#define C_Anchor
#define L_Marks

#include <Prelude.h>

#include "oliasdb/mark_base.h"

#include "MarkBase_mmdb.hh"
#include "Mark_mmdb.hh"

#include <sys/stat.h>

static int kludge1 = MarkBase::register_open_func (MarkBase_mmdb::open);
static int kludge2 = MarkBase::register_create_func (MarkBase_mmdb::create,
						     "mmdb");


// /////////////////////////////////////////////////////////////////
// Open the base if it exists, return NULL otherwise.
// /////////////////////////////////////////////////////////////////

MarkBase *
MarkBase_mmdb::open (const char *filename, bool read_only)
{
#if 0
  // force creation of olias_server 
  the_mmdb();
#endif
  // See if there's read a names.mmdb file in the specified directory.

  // NOTE: Should really use pathconf() to get buffer size. 
  char buffer[256];
  struct stat file_info;

  snprintf (buffer, sizeof(buffer), "%s/names.mmdb", filename);
  int status = stat (buffer, &file_info);

  if (status == -1)
    return (NULL);

  mark_base *base;
  user_base::rw_flag_t mode = read_only ? user_base::READ : user_base::WRITE;
  mtry
    {
      base = new mark_base (filename, "mark_base", "", mode);
    }
  mcatch_any()
    {
      return (NULL);
    }
  end_try;

  // Opened the base successfully, so create the MarkBase_mmdb object.
  return (new MarkBase_mmdb (base, read_only));
}


// /////////////////////////////////////////////////////////////////
// Create a new mmdb mark base
// /////////////////////////////////////////////////////////////////

MarkBase *
MarkBase_mmdb::create (const char *filename, bool read_only)
{
#if 0
  // force creation of olias_server 
  the_mmdb();
#endif
  // Just let the mmdb code go for it...
  mark_base *base;
  user_base::rw_flag_t mode = read_only ? user_base::READ : user_base::WRITE;

  mtry
    {
      if (!read_only)
	{
	  base = new mark_base (filename, "mark_base", "", mode);
	  if (base->open_status() != user_base::SUCC)
	    {
	      delete base;
	      read_only = TRUE;
	      mode = user_base::READ;
	    }
	}

      // We try again in read only mode if write mode failed. 
      if (read_only)
	{
	  base = new mark_base (filename, "mark_base", "", mode);
	  if (base->open_status() != user_base::SUCC)
	    {
	      delete base;
	      throw (CASTEXCEPT Exception());
	    }
	}
    }
  mcatch_any()
    {
      return (NULL);
    }
  end_try;

  // Creation worked, so return the base object. 
  return (new MarkBase_mmdb (base, read_only));
}


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MarkBase_mmdb::MarkBase_mmdb (mark_base *base, bool read_only)
: f_mark_base (base),
  f_read_only (read_only)
{
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

MarkBase_mmdb::~MarkBase_mmdb()
{
  // Close down the base. 
  delete f_mark_base;
}


// /////////////////////////////////////////////////////////////////
// find_mark - find an existing mark, look in the cache first
// /////////////////////////////////////////////////////////////////

// The cache must be maintained by the base, because it is keyed
// from the oids of the Marks, and two marks in two separate
// mmdb mark bases can have the same oid.  I suppose it is would
// be possible to use a global cache implemented in the Mark_mmdb
// object itself keyed off the base and the mark oid, but it would
// have to be more complex to avoid being slower than this method.
//   20:24 28-Sep-93 DJB 

Mark_mmdb *
MarkBase_mmdb::find_mark (oid_t &id, UAS_Pointer<UAS_Common> &doc_ptr)
{
  List_Iterator<Mark_mmdb *> m (f_mark_cache);

  while (m)
    {
      if (m.item()->id().eq (id))
	return (m.item());
      m++;
    }

  // Didn't find it, so create a new Mark and cache it.
  // Mark creation may throw an Exception, which our caller catches. 
  Mark_mmdb *mark = new Mark_mmdb (*this, id, doc_ptr);
  f_mark_cache.insert (mark);
  return (mark);
}


// /////////////////////////////////////////////////////////////////
// create_mark - create a new mark
// /////////////////////////////////////////////////////////////////

UAS_Pointer<Mark>
MarkBase_mmdb::create_mark (UAS_Pointer<UAS_Common> &doc_ptr, const Anchor &anchor,
			    const char *name, const char *notes)
{
  // Creation of mark may throw an exception out of here. 
  Mark_mmdb *mark = new Mark_mmdb (*this, doc_ptr, anchor, name, notes);
  f_mark_cache.insert (mark);
  return (mark);
}


// /////////////////////////////////////////////////////////////////
// get_marks - get marks corresponding to a specific document
// /////////////////////////////////////////////////////////////////

void
MarkBase_mmdb::get_marks (UAS_Pointer<UAS_Common> &doc_ptr,
			  xList<UAS_Pointer<Mark> > &list)
{
#ifdef FUJITSU_OLIAS
  UAS_String theId = doc_ptr->bid();
  theId = theId + "/" + doc_ptr->id();
#else
  UAS_String theId = doc_ptr->id();
#endif

  oid_list_handler *l =
    f_mark_base->get_mark_list ((char *) theId);

  if (l != NULL)
    {
      for (int i = 1; i <= (*l)->count(); i++)
	{
	  // Pull the entry from the list and replace it with NULL so
	  // that it doesn't get deleted when the list is deleted.
	  oid_t id = (*l)-> operator()(i);

	  // Assure that the oid_t has not been deleted. qfc 7-6-93
          if (id.eq (oid_t (c_code_t(0), 0)) == false)
	    {
	      mtry
		{
		  Mark_mmdb *m = find_mark (id, doc_ptr);
		  list.append (m);
		}
	      mcatch_any()
		{
		  // ignore failure for now! DJB 
		}
	      end_try;
	    }
	}
      delete l;
    }
}


// /////////////////////////////////////////////////////////////////
// get_all_marks - get all marks in the current mark base
// /////////////////////////////////////////////////////////////////

void
MarkBase_mmdb::get_all_marks (xList<UAS_Pointer<Mark> > &list)
{
  static UAS_Pointer<UAS_Common> null_doc;
  mmdb_pos_t i;
  Mark_mmdb *m;
  for (i = f_mark_base->first(); i != 0; f_mark_base->next(i))
    {
      oid_t id = f_mark_base->get_mark_oid (i);
      mtry
	{
	  m = find_mark (id, null_doc);
	  list.append (m);
	}
      mcatch_any()
	{
	  // ignore any errors for now -- DJB
	}
      end_try;
    }
}


bool
MarkBase_mmdb::read_only()
{
  return (f_read_only);
}


// /////////////////////////////////////////////////////////////////
// uncache_mark - remove a mark from the cache
// /////////////////////////////////////////////////////////////////

// This function is called by an Mark_mmdb when it is deleted.

void
MarkBase_mmdb::uncache_mark (Mark_mmdb *mark)
{
  // Search for the Mark in the cache and delete it.
  f_mark_cache.remove (mark);
}
