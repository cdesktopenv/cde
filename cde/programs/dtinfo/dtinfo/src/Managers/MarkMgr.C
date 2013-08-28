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
/*	All Rights Reserved				*/

/*
 * $XConsortium: MarkMgr.C /main/12 1996/09/18 10:46:14 mgreess $
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

#include <sys/param.h>
#include <sstream>
using namespace std;

#include "UAS.hh"

#define C_xList
#define L_Support

#define C_Mark
#define C_MarkBase
#define C_Anchor
#define L_Marks

#define C_EnvMgr
#define C_PrefMgr
#define C_MarkMgr
#define C_MessageMgr
#define L_Managers

#define C_MarkListView
#define L_Agents

#define C_WindowSystem
#define L_Other

#include <Prelude.h>

#include "Managers/CatMgr.hh"
#include "Registration.hh"

#if defined(sun)
#if defined(SVR4)
#define SunOS5
#else
#define SunOS4
#endif
#endif

#if defined(Internationalize) && defined(SunOS5)
#include <libintl.h>
#endif

#if defined(UseWideChars)
# if defined(SunOS4)
#   define mbstowcs(a,b,c) Xmbstowcs(a,b,c)
#   define wcstombs(a,b,c) Xwcstombs(a,b,c)
# elif defined(_IBMR2)
#   include <wcstr.h>
# endif
#endif

#include <wchar.h>
#ifdef USL
#include <wctype.h>
#endif

LONG_LIVED_CC (MarkMgr,mark_mgr)

#define CLASS MarkMgr

STATIC_SENDER_CC (MarkCreated)
STATIC_SENDER_CC (MarkMoved)

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MarkMgr::MarkMgr()
: f_num_bases (0),
  f_can_create (FALSE),
  f_mark_list_view (NULL)
{
  char *buffer = NULL;

  // Open the default user mark base. 
  const char *filename = pref_mgr().get_string (PrefMgr::DefaultMarkBase);

  if (!filename || (filename && (*filename == '\0')))
    {
      // the marks directory needs to be in sync with mmdb (user_base.cc)
      buffer = new char[MAXPATHLEN];
      snprintf (buffer, MAXPATHLEN, "%s/marks/default", env().user_path());

#if EAM
      snprintf (buffer, MAXPATHLEN, "%s/.dt/dtinfo/%s/marks/default",
                env().home(), env().lang());
#endif
      filename = buffer;
    }

  ON_DEBUG (printf ("MarkBase(): opening %s\n", filename));
  open_base (filename);
  delete [] buffer;
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

MarkMgr::~MarkMgr() {
    finalize ();
}

void
MarkMgr::finalize () {
  // Delete all the open MarkBases (which closes them). 
  List_Iterator<MarkBase *> b (f_base_list);

  while (b)
    {
      // NOTE: delete removed temporarily until I can deal with
      // dangling references to the mark base in mark objects.
      // See the note in ~Mark_mmdb().  16:19 30-Nov-93 DJB 

      //
      // NOTE NOTE: put this back in. SWM.
      //
      delete b.item();
      f_base_list.remove (b);
      f_num_bases--;
    }

  delete f_mark_list_view;
  f_mark_list_view = 0;
}


// /////////////////////////////////////////////////////////////////
// open a mark base, creating if necessary
// /////////////////////////////////////////////////////////////////

void
MarkMgr::open_base (const char *filename)
{
  // First try to open the base. 
  MarkBase *base = MarkBase::open (filename, FALSE);

  // On failure, try to create it. 
  if (base == NULL)
    base = MarkBase::create (filename, FALSE, "mmdb");

  // Make sure it worked.
  if (base == NULL)
    {
      message_mgr().error_dialog ((char*)UAS_String(
	CATGETS(Set_Messages, 32, "Your mark base cannot be opened.")));
      return;
    }

  // Check the r/w state of the base.
  if (!base->read_only())
    f_can_create = TRUE;

  f_base_list.append (base);
  f_num_bases++;
}


// /////////////////////////////////////////////////////////////////
// create_mark - create a new Mark object
// /////////////////////////////////////////////////////////////////

UAS_Pointer<Mark>
MarkMgr::create_mark (UAS_Pointer<UAS_Common> &doc_ptr, const Anchor &anchor,
		      const char *name, const char *notes)
{
  // First a little error checking. 
  if (f_num_bases == 0)
    {
      message_mgr().error_dialog ((char*)UAS_String(
	CATGETS(Set_Messages, 33, "Dtinfo mark base was not found.\n"
				  "Dtinfo cannot create a bookmark.")));
      return (NULL);
    }
  if (!f_can_create)
    {
      message_mgr().error_dialog ((char*)UAS_String(
	CATGETS(Set_Messages, 34, "This mark base provides read-only permissions.")));
      return (NULL);
    }

  // For now, we create the Mark in the first opened MarkBase.
  // In the future, we may give the user an interface to select
  // the current or active MarkBase in which new Marks are placed.
  // 20:12 29-Sep-93 DJB

  List_Iterator<MarkBase *> b (f_base_list);

  assert (b != 0);

  // Trim leading and trailing space and check for an empty name.
#ifdef UseWideChars
  wchar_t *new_name = (wchar_t*)malloc((strlen(name) + 1) * sizeof(wchar_t));
  wchar_t *start = new_name, *end;
  char* trimmed_name;
  int size = strlen(name) + 1;
  int n = mbstowcs(new_name, name, size);
  if( n >= 0 ) {
    assert( n >= 0 );
  }
  while (*start != (wchar_t)'\0' && iswspace(*start))
    start++;
  if (*start != (wchar_t)'\0') {
      for (end = start; *end != (wchar_t)'\0'; end++)
	;
      for (end --; end != start && iswspace(*end); end --)
	;
      end++;
      *end = '\0';
      size = wcslen(start) * 2 + 1;
      trimmed_name = (char*)malloc(size * sizeof(char));
      n = wcstombs(trimmed_name, start, size);
      assert( n >= 0 );
  }
  else {
      trimmed_name = strdup(CATGETS(Set_AgentLabel, 213, "Unnamed"));
  }
#else
  char *new_name = strdup (name);
  char *p = new_name, *trimmed_name;
  while (*p != '\0' && isspace (*p))
    p++;
  if (*p != '\0')
    {
      trimmed_name = p;
      while (*p != '\0')
	p++;
      while (p != trimmed_name && isspace(*p))
	p--;
      *p = '\0';
    }
  else
    {
      trimmed_name = window_system().get_message("Unnamed");
    }
#endif

  MarkCreated create_message;
  create_message.f_mark_ptr = b.item()->
    create_mark (doc_ptr, anchor, trimmed_name, notes);
  send_message (create_message);
  free (new_name);
#ifdef UseWideChars
  free (trimmed_name);
#endif
  return (create_message.f_mark_ptr);
}


// /////////////////////////////////////////////////////////////////
// move_mark - move an existing Mark object
// /////////////////////////////////////////////////////////////////

UAS_Pointer<Mark>
MarkMgr::move_mark (UAS_Pointer<UAS_Common> &doc_ptr, const Anchor &anchor,
		    UAS_Pointer<Mark> &old_mark)
{
  // First a little error checking. 
  if (f_num_bases == 0)
    {
      message_mgr().error_dialog ((char*)UAS_String(
	CATGETS(Set_Messages, 33, "Dtinfo mark base was not found.\n"
				  "Dtinfo cannot create a bookmark.")));
      return (NULL);
    }
  if (!f_can_create)
    {
      message_mgr().error_dialog ((char*)UAS_String(
	CATGETS(Set_Messages, 34, "This mark base provides read-only permissions.")));
      return (NULL);
    }

  // For now, we create the Mark in the first opened MarkBase.
  // In the future, we may give the user an interface to select
  // the current or active MarkBase in which new Marks are placed.
  // 20:12 29-Sep-93 DJB

  List_Iterator<MarkBase *> b (f_base_list);

  assert (b != 0);

  MarkMoved move_message;
  move_message.f_new_mark_ptr = b.item()->
    create_mark (doc_ptr, anchor, old_mark->name(), old_mark->notes());
  move_message.f_old_mark_ptr = old_mark;
  send_message (move_message);

  // Get rid of the old mark. 
  old_mark->remove();

  // Need to let mark list and viewports, etc know that a new mark
  // was created.  Ideally they should deal with MarkMoved.  DJB
  MarkCreated create_message;
  create_message.f_mark_ptr = move_message.f_new_mark_ptr;
  send_message (create_message);

  return (move_message.f_new_mark_ptr);
}


void
MarkMgr::get_marks (UAS_Pointer<UAS_Common> &doc_ptr, xList<UAS_Pointer<Mark> > &list)
{
  List_Iterator<MarkBase *> b (f_base_list);

  while (b)
    {
      b.item()->get_marks (doc_ptr, list);
      b++;
    }
  ON_DEBUG(cerr << "MarkMgr::get_marks(): " << list.length() << endl);
}

void
MarkMgr::get_all_marks (xList<UAS_Pointer<Mark> > &list)
{
  List_Iterator<MarkBase *> b (f_base_list);

  while (b)
    {
      b.item()->get_all_marks (list);
      b++;
    }
}

void
MarkMgr::display_mark_list()
{
  if (f_mark_list_view == NULL)
    f_mark_list_view = new MarkListView();
  f_mark_list_view->display();
}
