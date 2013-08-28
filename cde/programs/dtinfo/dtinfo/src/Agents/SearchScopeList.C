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
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved                        	*/

/*
 * $XConsortium: SearchScopeList.C /main/15 1996/09/27 10:16:12 cde-hal $
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

#include "UAS.hh"
#include "Support/UtilityFunctions.hh"

#define C_Buffer
#define L_Basic

#define C_xList
#define L_Support

#define C_SearchScopeList
#define C_SearchScopeAgent
#define L_Agents

#define C_UserPreference
#define C_StringPref
#define L_Preferences

#define C_MessageMgr
#define C_SearchScopeMgr
#define C_EnvMgr
#define L_Managers

#include <Prelude.h>
#include <string.h>

#ifdef SVR4
#ifndef USL
#include <libintl.h>
#endif
#endif

#include <string.h>
#include <iostream>
using namespace std;

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

// rtp - 4/24/95 : Added a very simple buffer class for use in
//                 this module. (Original code written by swm.)
SimpleBuffer::SimpleBuffer ()
: f_data(NULL),
  f_data_len(0)
{
}

SimpleBuffer::~SimpleBuffer ()
{
  delete [] f_data;
  f_data = NULL;
  f_data_len = 0;
}

void
SimpleBuffer::write (char *new_data)
{
  int slen, len = 0;
  int new_len = 0;

  if (f_data)
    len = new_len = strlen (f_data);
  new_len += strlen (new_data);
  // manual realloc
  char *new_buf = new char [new_len + 1];
  if (f_data)
    *((char *) memcpy(new_buf, f_data, len) + len) = '\0';
  else
    *new_buf = 0;
  delete [] f_data;
  f_data = new_buf;
  slen = len;
  len = strlen(new_data);
  *((char *) memcpy(f_data + slen, new_data, len) + len) = '\0';
}

char *
SimpleBuffer::data ()
{
  return f_data;
}

SearchScopeList::SearchScopeList (SearchScopeAgent *ssa)
: f_search_scope_agent (ssa)
{
  f_buffer = new SimpleBuffer;
}

SearchScopeList::~SearchScopeList ()
{
  delete f_buffer;
}

void
SearchScopeList::save()
{
  List_Iterator<UAS_SearchScope *> i (this);
  UAS_SearchScope *scope;
  char scratch[256];
  Buffer buffer, temp_buffer;

  // for each search scope do
  for (; i != 0; i++)
  {
    scope = i.item();
    if (scope->deleted() || scope->read_only())
      continue;

    buffer.reset();

    // Store the scope component mask.
    snprintf (scratch, sizeof(scratch), "%d;", scope->search_zones().zones());
    buffer.write (scratch, sizeof (char), strlen (scratch));

    // get the list of bookcases for the current scope
    UAS_PtrList<UAS_BookcaseEntry> bcases = scope->bookcases();

    // If there are no bookcases in the scope, there is a problem
    // somewhere; skip to the next scope.
    if (bcases.numItems() <= 0)
      continue;

    UAS_BookcaseEntry *bce;

    if(env().debug())
    {
      cerr << endl;
      cerr << "Save Scope: " << scope->name();
    }

    // loop for each bookcase in the search scope
    for (int bc = 0; bc < bcases.numItems(); bc++)
    {
      bce = bcases[bc];

      // write bookcase id and name to buffer
      const char *name = bce->name();
      const char *bid = bce->bid();
      if (bc > 0)
        snprintf (scratch, sizeof(scratch), "&%s(%s)", bid, name);
      else
        snprintf (scratch, sizeof(scratch), "%s(%s)", bid, name);
      buffer.write(scratch, sizeof (char), strlen(scratch));

      if(env().debug())
        cerr << scratch;

      // See if individual books are selected.
      // Store the books by book number.
      UAS_ObjList<int> booklist = bce->book_list();

      // if there are no books--entire bookcase is selected
      if (booklist.numItems() > 0)
      {
        for (int bk = 0; bk < booklist.numItems(); bk++)
        {
          snprintf (scratch, sizeof(scratch), ",%d", booklist[bk]);
          buffer.write(scratch, sizeof (char), strlen(scratch));
        }
        if(env().debug())
          cerr << scratch << endl;
      }
    }
    buffer.write ("\0", sizeof (char), 1);

    // Get the right preference object.
    snprintf (scratch, sizeof(scratch), "Scope.%s", scope->name());
    StringPref store (scratch);

    // Update its value.
    ON_DEBUG (printf ("WRITING: %s: %s\n", scratch, buffer.data()));
    store.value (buffer.data());
  }

  // Now dump the name of each of the scopes.
  buffer.reset();
  const char *name;
  for (i.reset(); i != 0; i++)
  {
    if (i.item()->read_only())
      continue;
    name = i.item()->name();
    buffer.write (name, sizeof (char), strlen (name));
    buffer.write (",", sizeof (char), 1);
  }
  // Backup and overwrite the trailing comma if there is one.
  if (buffer.point() != buffer.data())
    buffer.point (buffer.point() - sizeof (char));
  buffer.write ("\0", sizeof (char), 1);

  StringPref scopes ("Scopes");

  //  buffer used to write out all scope names
  SimpleBuffer output_buf;

  //  if there's invalid scope names in f_buffer
  //               to write out...
  if (f_buffer->data())
  {
    //  copy the invalid names to output buffer
    output_buf.write(f_buffer->data());
    //  if buffer with valid names is "empty",
    //  i.e., only contains '\0' (see code above)
    if (buffer.length() == 1)
    {
      // overwrite the trailing comma at end of
      // the invalid scope names
      char *data = output_buf.data();
      int   len = strlen(data);
      data[len-1] = '\0';
    }
    // otherwise concatanate the valid names
    else
      output_buf.write((char *)buffer.data());
  }
  // otherwise just write out the valid names
  else
    output_buf.write((char *)buffer.data());

  //  write out all scope names to the prefs file
  scopes.value (output_buf.data());
  ON_DEBUG (printf ("SCOPES: %s\n", output_buf.data()));

  // Flush the changes to disk.
  UserPreference::flush_preferences();
}

void
SearchScopeList::restore()
{
  unsigned int  i;
  UAS_SearchScope *s;

  // get the list of bookcase names
  UAS_List<UAS_Common> bcaseList = bookcaseList ();
  UAS_List<UAS_Common> libs = f_search_scope_agent->list();

  // create the set of standard scopes
  UAS_PtrList<UAS_BookcaseEntry>  bcases;

  //
  // Create the scope for the Current Section
  //
  s = f_search_scope_agent->create_scope(
  	(char*)UAS_String(CATGETS(Set_Messages, 30, "Current Section")),
  	bcases, f_section, FALSE);
  s->set_read_only();

  //
  // Create a scope for All Libraries
  //
  bcases = f_search_scope_agent->bookcase_list();
  s = f_search_scope_agent->create_scope(
	(char*)UAS_String(CATGETS(Set_SearchScopeAgent, 4, "All Libraries")),
	bcases, f_all, FALSE);
  s->set_read_only();
  bcases.clear();

  //
  // Create a scope for each infolib
  //
  for (i = 0; i < libs.length(); i++) {
      UAS_String libs_lid(libs[i]->lid());
      s = f_search_scope_agent->create_infolib_scope(libs_lid);
  }
  
  // create all named scopes in preferences file
  create_named_scopes();
  
  // rtp - 4/24/95 : if invalid scopes found then warn the user
  if (f_buffer->data())
    search_scope_mgr().show_warning(True);
  // do not show dialog at this point--toplevel shell has not
  // yet been realized. Just set the flag so dialog can be
  // shown later.
  //   message_mgr().warning_dialog (
  //	(char*)UAS_String(CATGETS(Set_Messages, 31,
  //	"Ignoring invalid scopes in your profile.")));
}

// create all valid named scopes in preferences file
void
SearchScopeList::create_named_scopes()
{
  char scratch[1024];
  bool is_scope_valid;
  UAS_PtrList<UAS_BookcaseEntry>  bookcase_list;
  UAS_BookcaseEntry              *bce;

  // get list of scope names from preference file
  StringPref scopes ("Scopes");
  UAS_String scope_list (scopes.value());
  UAS_List<UAS_String> scope_names = scope_list.splitFields (',');

  // reset invalid scope name buffer
  if (f_buffer != NULL)
  {
    delete f_buffer;
    f_buffer = new SimpleBuffer;
  }

  // retrieve scopes from preference file and validate each one
  for (int sname = 0; sname < (int) scope_names.length(); sname++)
  {
    is_scope_valid = True;

    // Get the specified scope from preferences.
    UAS_String ss = *(UAS_String*) scope_names[sname];
    snprintf (scratch, sizeof(scratch), "Scope.%s", (char*)ss);
    StringPref scope (scratch);

    // Grab the component mask.
    unsigned int mask = 0;
    int i;
    sscanf (scope.value(), "%d%n", &mask, &i);
    if (mask == 0)
    {
      // rtp - 4/24/95 : bad mask means invalid scope
      is_scope_valid = FALSE;
    }

    // skip past component mask
    const char *scope_ptr = scope.value() + i;
    scope_ptr += 1;

    // get list of bookcases
    UAS_String scope_string(scope_ptr);
    UAS_List<UAS_String>bookcases = scope_string.splitFields ('&');

    if(env().debug())
    {
      cerr << endl;
      cerr << "Restore Scope." << (char*)ss;
      cerr << ": " << mask << ";";
    }

    // loop once for each bookcase in search scope. create a
    // bookcase entry for each valid bookcase. if bookcase
    // is invalid, invalidate the scope.
    for (int bname = 0; bname < (int) bookcases.length(); bname++)
    {
      UAS_String str = *(UAS_String*)bookcases[bname];
      UAS_List<UAS_String>bc_list = str.splitFields (',');

      bce = create_bcase_entry(bc_list);
      if (bce == NULL)
      {
        is_scope_valid = False;
        if(env().debug())
          cerr << "  >>>>Scope is invalid" << endl;
        break;
      }

      bookcase_list.append(bce);
    }

    // get scope name
    UAS_String sn = *(UAS_String*)scope_names[sname];
    if(is_scope_valid)
    {
      f_search_scope_agent->create_scope (sn, bookcase_list, mask, False);
    }
    else
    {
      // rtp - 4/24/95 : otherwise store its name for use later;
      //                 see SearchScopeList::save routine above
      snprintf(scratch, sizeof(scratch), "%s%s", (char*)sn, ",");
      f_buffer->write (scratch);
    }
    // reset list for next turn
    bookcase_list.clear();
  } // for (int sname = 0)
}

// Check to see if bookcase is valid on system. If bid is valid,
// return true, otherwise, return false.
UAS_Pointer<UAS_Common>
SearchScopeList::validate_bookcase(UAS_String &bid)
{
  // validate bookcase id
  UAS_List<UAS_Common> libs = f_search_scope_agent->list();

  for (unsigned int i = 0; i < libs.length(); i++)
  {
    UAS_List<UAS_Common> kids = libs[i]->children();
    for (unsigned int j = 0; j < kids.length(); j++)
    {
      if(kids[j]->bid() == bid)
      {
        return  kids[j];
      }
    }
  }
  return (UAS_Pointer<UAS_Common>)NULL;
}

// Create a bookcase_entry and return it.
// The input to this function is a bookcase list.
// The first element is a bookcase id. All subsequent elements
// are books:
// 	BCID [book1 book2 book3 ...]
//
UAS_BookcaseEntry *
SearchScopeList::create_bcase_entry(UAS_List<UAS_String> &bc_list)
{
  UAS_Pointer<UAS_Common> uas_bookcase;

  // get BCID--the first element in list
  UAS_String bs = *(UAS_String*)bc_list[0];
  char buff[40];
  sscanf((char*)bs,"%[^)(]", buff);
  UAS_String bid = UAS_String(buff, strlen(buff));

  // validate BCID
  uas_bookcase = validate_bookcase(bid);
  if (uas_bookcase == (UAS_Pointer<UAS_Common>)NULL)
    return (UAS_BookcaseEntry*)NULL;

  // create new bookcase entry object
  UAS_BookcaseEntry *bce = new UAS_BookcaseEntry(uas_bookcase);

  if(env().debug())
    cerr << "," << (char*)uas_bookcase->bid();

  // Check to see if entire bookcase is selected or are there
  // individual books selected.
  if (bc_list.length() > 1)
  {
    // individual books have been selected.
    int book_num;
    UAS_ObjList<int> booklist;

    for (int book = 1; book < (int) bc_list.length(); book++)
    {
      UAS_String abook = *(UAS_String*)bc_list[book];
      if(sscanf ((char*)abook, "%d", &book_num) == 1)
      {
         booklist.append (book_num);
      }
    }
    if(env().debug())
    {
      for (int bk=0; bk < booklist.numItems(); bk++)
        cerr << "," << booklist[bk];
      cerr << endl;
    }
    bce->set_book_list(booklist);
  }

  return bce;
}

// These routines are called from the SearchScopeAgent; They
// maintain a list of BitHandles that directly coorespond to
// the scopes maintained in the SearchScopeList (i.e., this).
// It's crucial that if a scope is modified in SearchScopeList
// (i.e., added, removed, etc.), it's corresponding BitHandle
// in f_handle_list is modified in lock-step. -rtp

// Note: the index is zero (0) based. -rtp
void
SearchScopeList::add_handle(BitHandle handle, int index)
{
    // index of -1 means to append
    if (index == -1) {
        f_handle_list.append(handle);
        return;
    }

    List_Iterator<BitHandle> h (f_handle_list);
    for (int i = 0; i < index; i++, h++);
    f_handle_list.insert_before (h, handle);
}

// Note: this routine must be called *before* the remove method
// is called for the scope (...otherwise it won't find scope in
// this, duh). -rtp
void
SearchScopeList::remove_handle(UAS_SearchScope *scope)
{
    // first, find index into search scope list
    List_Iterator<UAS_SearchScope *> s (this);
    int i;
    for (i = 0; s != 0 && s.item() != scope; s++, i++);
    // if can't find scope in list then something is hosed
    if (s == NULL)
        return;
    // next, find the bit handle located at this index in handle list
    List_Iterator<BitHandle> h (f_handle_list);
    for (int j = 0; j < i && h != 0; j++, h++);
    // if this happens then something is hosed
    if (h == NULL)
        return;
    // and finally, remove it
    BitHandle bh = h.item();
    f_handle_list.remove(bh);
}

// Routine to lookup BitHandle based on UAS_SearchScope *
BitHandle
SearchScopeList::lookup_handle(UAS_SearchScope *scope)
{
  // first, find index into search scope list
  List_Iterator<UAS_SearchScope *> s (this);

  int i;
  for (i = 0; s != 0 && s.item() != scope; s++, i++);
  // if can't find scope in list then something is hosed
  if (s == NULL)
    return 0;

  // next, find the bit handle located at this index in handle list
  List_Iterator<BitHandle> h (f_handle_list);

  for (int j = 0; j < i && h != 0; j++, h++);
  // if this happens then something is hosed
  if (h == NULL)
    return 0;

  // and finally, return handle
  BitHandle bh = h.item();
  return bh;
}
