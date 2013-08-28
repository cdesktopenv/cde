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
 * $TOG: Mark_mmdb.C /main/9 1998/04/17 11:38:06 mgreess $
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


#define C_Mark
#define C_MarkBase
#define C_Anchor
#define L_Marks

#define C_Buffer
#define L_Basic

#include <Prelude.h>

#include "Mark_mmdb.hh"
#include "MarkBase_mmdb.hh"

#define MARK_VERSION 4

Buffer g_buffer;

Mark_mmdb::Mark_mmdb (MarkBase_mmdb &base, UAS_Pointer<UAS_Common> &doc_ptr,
		      const Anchor &anchor,
		      const char *name, const char *notes)
: f_doc_ptr (doc_ptr),
  f_anchor (anchor),
  f_mark_base (base),
  f_name (NULL),
  f_notes (NULL)
{
  // This creates a new user mark in the mmdb mark database. 

#ifdef FUJITSU_OLIAS
  // Id = bookcase id / section id
  UAS_String theId = doc_ptr->bid();
  theId = theId + "/" + doc_ptr->id();
#else
  UAS_String theId = doc_ptr->id();
#endif
  
  f_user_mark = new mark_smart_ptr (base, theId);
  set_name (name);
  set_notes (notes);

  // Flush the new mark to disk. 
  do_save();
}


Mark_mmdb::Mark_mmdb (MarkBase_mmdb &base, const oid_t &mark_oid,
		      UAS_Pointer<UAS_Common> &n)
: f_doc_ptr (n),
  f_anchor (NULL, NULL, TRUE),
  f_mark_base (base),
  f_name (NULL), f_notes (NULL)
{
  // This restores user mark from the mmdb mark database. 
  f_user_mark = new mark_smart_ptr (base, mark_oid);
  restore();
}


Mark_mmdb::Mark_mmdb (MarkBase_mmdb &base, const oid_t &mark_oid)
: f_anchor (NULL, NULL, TRUE),
  f_mark_base (base),
  f_name (NULL), f_notes (NULL)
{
  // This restores user mark from the mmdb mark database. 
  f_user_mark = new mark_smart_ptr (base, mark_oid);
  restore();
}


Mark_mmdb::~Mark_mmdb()
{
  // BUG:  It's possible for mark references to exist elsewhere in the
  //  browser _after_ the MarkMgr and MarkBases have been destroyed! 
  f_mark_base.uncache_mark (this);
  delete [] f_name;
  delete [] f_notes;
  delete f_user_mark;
}


UAS_Pointer<UAS_Common> &
Mark_mmdb::doc_ptr()
{
  // If the doc_ptr isn't known yet, find it. 
  if (f_doc_ptr == (const int)NULL)
    {
      UAS_String url = "mmdb:LOCATOR=";

      UAS_String real_uid(f_user_mark->node_locator());
      UAS_String infobase_id, section_id;
      real_uid.split('/', infobase_id, section_id);

      // Just use section id for now
      // NOTE: We may need to use infobase id as well to precisely
      //       identify the section
      url = url + section_id;

      f_doc_ptr = UAS_Common::create (url);

#ifdef FUJITSU_OLIAS
      // check to see if the section is in the same corpus
      // as the one in which it has been created.
      if (UAS_String(f_doc_ptr->bid()) != infobase_id)
	f_doc_ptr = NULL;
#endif
    }

  return (f_doc_ptr);
}


const char *
Mark_mmdb::name()
{
  return (f_name);
}

void
Mark_mmdb::set_name (const char *name)
{
  unsigned int l = strlen (name);
  delete [] f_name;
  f_name = new char [l + 1];
  memcpy (f_name, name, l + 1);
}

const char *
Mark_mmdb::notes()
{
  return (f_notes);
}

void
Mark_mmdb::set_notes (const char *notes)
{
  unsigned int l = strlen (notes);
  delete [] f_notes;
  f_notes = new char [l + 1];
  memcpy (f_notes, notes, l + 1);
}


// /////////////////////////////////////////////////////////////////
// save - commit a record to the database
// /////////////////////////////////////////////////////////////////

void
Mark_mmdb::do_save()
{
  // Convert the mark info to a stream. 
  g_buffer.reset();
  g_buffer.write (MARK_VERSION);
  g_buffer.write (f_anchor.location());
  g_buffer.write (f_anchor.format());
  g_buffer.write (f_name);
  g_buffer.write (f_notes);

  // Write the mark to disk.
  f_user_mark->update_usermark (g_buffer.data(), g_buffer.length());
}


// /////////////////////////////////////////////////////////////////
// remove - remove this record from the database
// /////////////////////////////////////////////////////////////////

void
Mark_mmdb::do_remove()
{
  f_user_mark->remove_from_db();
}


void
Mark_mmdb::restore()
{
  int sz;
  char *str;

  mtry
    {
      sz = f_user_mark->mark_value()->size();
      str = f_user_mark->mark_value()->get();
    }
  mcatch_any()
    {
      delete this;
      rethrow;
    }
  end_try;
  
  // NOTE: Need better guard against corrupt mark data!! 
  if (sz == 0)
    {
      ON_DEBUG (puts ("*** ZERO LENGTH MARK ***"));
      throw (CASTEXCEPT Exception());
    }
  // Suck the data out of the mmdb mark object. 
  g_buffer.reset();
  g_buffer.write (str, sizeof (char), sz);

  // Read the fields out of the buffer.
  int mark_version;
  g_buffer.reset();
  g_buffer.read (&mark_version);
  // NOTE: Check the version here!!  Throw exception on incompatiblity.

  char *location, *format;
  g_buffer.read (&location);
  g_buffer.read (&format);
  f_anchor.set (location, format, TRUE);
  g_buffer.read (&f_name);
  g_buffer.read (&f_notes);
}
