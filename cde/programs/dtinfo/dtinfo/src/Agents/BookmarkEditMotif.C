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
 * $XConsortium: BookmarkEditMotif.C /main/13 1996/10/04 10:40:31 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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

#define C_List
#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_HelpAgent
#ifdef UseTmlRenderer
#define C_MarkView
#endif
#define C_BookmarkEdit
#define L_Agents

#define C_MessageMgr
#define L_Managers

#define C_Mark
#define L_Marks

#include "Managers/CatMgr.hh"
#include "Other/XmStringLocalized.hh"

#include "Prelude.h"

#include "Registration.hh"

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmLabelGadget.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmText.h>
#include <WWL/WXmPushButtonGadget.h>
#include <WWL/WXmSeparatorGadget.h>

#include <iostream>
using namespace std;

int BookmarkEdit::g_modified_count = 0;

#ifdef SVR4
#ifndef USL
#include <libintl.h>
#endif
#endif

#define THIS_CLASS BookmarkEdit
#define ON_ACTIVATE(WOBJ,FUNC) \
  (WOBJ).SetActivateCallback (this, (WWL_FUN) &THIS_CLASS::FUNC)
#if defined(__STDC__) || defined(hpux) || defined(__osf__)
#define BUTTON(PARENT,NAME) \
  WXmPushButtonGadget NAME (PARENT, #NAME, WAutoManage); \
  ON_ACTIVATE (NAME, NAME)
#else
#define BUTTON(PARENT,NAME) \
  WXmPushButtonGadget NAME (PARENT, "NAME", WAutoManage); \
  ON_ACTIVATE (NAME, NAME)
#endif

#define CALL_DATA(TYPE,NAME) \
  TYPE *NAME = (TYPE *) wcb->CallData()

class BookmarkMgr : public UAS_Receiver<EditMark>
{
public:
  BookmarkMgr()
    { ON_DEBUG(printf ("requesting MarkEdit from %p\n", this));
      Mark::request (this); }
  void destroy (BookmarkEdit *);
private:
  void receive (EditMark &message, void *client_data);

  xList<BookmarkEdit *> f_editor_list;
};

void
BookmarkMgr::receive (EditMark &message, void *client_data)
{
  // First see if there's already a window editing the Mark.
  List_Iterator<BookmarkEdit *> l (f_editor_list);

  while (l)
    {
      if (l.item()->mark_ptr() == message.f_mark_ptr)
	{
	  l.item()->display();
	  return;
	}
      l++;
    }

  // Time to create a new editor:
  BookmarkEdit *editor = new BookmarkEdit (message.f_mark_ptr);
  f_editor_list.append (editor);
  editor->display();
}

void
BookmarkMgr::destroy (BookmarkEdit *editor)
{
  // First remove it from the editor list.
  f_editor_list.remove (editor);
  delete editor;
}


static BookmarkMgr g_bookmark_mgr;

  
// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

BookmarkEdit::~BookmarkEdit()
{
  if (f_shell != NULL)
    {
      delete f_name_text;
      delete f_notes_text;
      delete f_wm_delete_callback;
      f_shell->Destroy();
      delete f_shell;
    }
  // Reduce the modified count when closed, if we've been modified.
  if (f_modified)
    g_modified_count--;
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::display()
{
  if (f_shell == NULL)
    create_ui();

  f_name_text->SetString ((char *) f_mark_ptr->name());
  f_notes_text->SetString ((char *) f_mark_ptr->notes());
  f_ok.SetSensitive (False); // 94/10/28 haya no modified in first display

  f_name_text->InsertionPosition (f_name_text->LastPosition());
  f_modified = FALSE; // prevent making ok sensitive
  g_modified_count--;

  Widget scrolled = XtParent (*f_notes_text);
  ((WXmForm *) XtParent (scrolled))->InitialFocus (scrolled);
  f_shell->Popup();
  f_shell->DeIconify();
}


// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::create_ui()
{
  XmStringLocalized mtfstring;
  String	    string;

  f_shell =
    new WTopLevelShell (window_system().toplevel(), WPopup, "bookmark_edit");
  window_system().register_shell (f_shell);

  string = CATGETS(Set_BookmarkEdit, 1, "Dtinfo: Bookmark Editor");
  XtVaSetValues(*f_shell, XmNtitle, string, NULL);

  WXmForm              form        (*f_shell, "form");
  WXmLabelGadget       name        (form,     "name",        WAutoManage);
  f_name_text = new WXmTextField   (form,     "name_text",   WAutoManage);
  WXmLabelGadget       notes       (form,     "notes",       WAutoManage);

  mtfstring = CATGETS(Set_AgentLabel, 163, "Name");
  XtVaSetValues(name, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 164, "Notes");
  XtVaSetValues(notes, XmNlabelString, (XmString)mtfstring, NULL);

  // NOTE: Don't add spaces after "," because of stupid CPP.
  f_ok = WXmPushButtonGadget (form, "ok", WAutoManage);
  ON_ACTIVATE (f_ok, ok);

  mtfstring = CATGETS(Set_AgentLabel, 161, "OK");
  XtVaSetValues(f_ok, XmNlabelString, (XmString)mtfstring, NULL);

  BUTTON (form,view);
  BUTTON (form,cancel);

  mtfstring = CATGETS(Set_AgentLabel, 165, "Display");
  XtVaSetValues(view, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 162, "Cancel");
  XtVaSetValues(cancel, XmNlabelString, (XmString)mtfstring, NULL);

  WXmPushButtonGadget help (form, "help", WAutoManage);
  mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);
  help_agent().add_activate_help (help, (char*)"bookmark_edit_help");
  f_wm_delete_callback =
    new WCallback (*f_shell, window_system().WM_DELETE_WINDOW(),
                   this, (WWL_FUN) &BookmarkEdit::cancel);

  WXmSeparatorGadget   separator   (form,     "separator",   WAutoManage);

  Widget notes_text = XmCreateScrolledText (form, (char*)"notes_text", NULL, 0);
  f_notes_text = new WXmText (notes_text);
  f_notes_text->Manage();

  f_name_text->SetModifyVerifyCallback (this, (WWL_FUN)&THIS_CLASS::modified);
  f_notes_text->SetModifyVerifyCallback (this, (WWL_FUN)&THIS_CLASS::modified);
  f_name_text->SetValueChangedCallback (this, (WWL_FUN)&THIS_CLASS::changed);
  f_notes_text->SetValueChangedCallback (this, (WWL_FUN)&THIS_CLASS::changed);

  form.DefaultButton (f_ok);
  form.ShadowThickness (0);
  form.Manage();

  f_shell->Realize();
  // Set the min size based on the current size.
  f_shell->MinWidth (f_shell->Width());
  f_shell->MinHeight (f_shell->Height());
}


// /////////////////////////////////////////////////////////////////
// ok
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::ok()
{
  // Only save if the mark is modified.  This shouldn't be necessary,
  // since the OK button shouldn't be sensitive, but let's be safe... 
  if (f_modified)
    {
      // NOTE: Need to strip leading/trailing spaces (or should that be
      // done in the mark object??

      // Update mark fields. 
      f_mark_ptr->set_name (f_name_text->Value());
      f_mark_ptr->set_notes (f_notes_text->Value());
      // Save the mark to disk.
      mtry
	{
	  f_mark_ptr->save();
	}
      mcatch_any()
	{
	  message_mgr().error_dialog(
		CATGETS(Set_BookmarkEdit, 1, "Dtinfo: Bookmark Editor"));
	}
      end_try;
    }

  // Get rid of the dialog now that the user is done with it. 
  g_bookmark_mgr.destroy (this);
}


// /////////////////////////////////////////////////////////////////
// view
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::view()
{
  f_mark_ptr->view();
}


// /////////////////////////////////////////////////////////////////
// cancel
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::cancel()
{
  bool confirmed = TRUE;
  // Make the user verify a cancel if the data has been changed. 

  if (f_modified)
    confirmed = message_mgr().question_dialog(
                  (char*)UAS_String(CATGETS(Set_Messages, 2,
						"Abandon changes?")));

  if (confirmed)
    g_bookmark_mgr.destroy (this);
}


// /////////////////////////////////////////////////////////////////
// modified - bookmark has been edited
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::modified (WCallback *wcb)
{
  CALL_DATA (XmTextVerifyCallbackStruct,tvp);
  bool notes = wcb->GetWidget() == *f_notes_text;
  // Change non-printable characters to spaces.
  if (tvp->text != NULL)
    {
      int length = tvp->text->length;
      char *p = tvp->text->ptr;
#ifdef Internationalize
      int n_bytes;
      for (; length > 0; p += n_bytes, length -= n_bytes) {
	wchar_t wc;
	n_bytes = mbtowc(&wc, p, MB_CUR_MAX);
	assert( n_bytes > 0 );
	if (n_bytes == 1 && !isprint(*p) && (!notes || !isspace(*p)))
	    *p = ' ';
      }
      assert( length == 0 );
#else
      for (; length > 0; p++, length--)
	{
	  if (!isprint(*p) && (!notes || (notes && !isspace(*p))))
	    *p = ' ';
	}
#endif
    }
}


// /////////////////////////////////////////////////////////////////
// changed - value in one of the fields has changed
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::changed (WCallback *wcb)
{
  // Update the ok button. 
  if (wcb->GetWidget() == *f_name_text)
    {
      char *name = f_name_text->Value();
      if (*name != '\0')
	f_ok.SetSensitive (True);
      else
	f_ok.SetSensitive (False);
      XtFree (name);
    }
  else if (!f_modified)
    {
      f_ok.SetSensitive (True);
    }
  if (!f_modified)
    {
      f_modified = TRUE;
      g_modified_count++;
    }
}


// /////////////////////////////////////////////////////////////////
// receive - MarkMoved
// /////////////////////////////////////////////////////////////////

void
BookmarkEdit::receive (MarkMoved &message, void *client_data)
{
  // First, see if it is ours. 
  if (message.f_old_mark_ptr != f_mark_ptr)
    return;

  ON_DEBUG (printf ("BookmarkEdit -- updating to moved mark\n"));
  // Ok, replace our pointer when moved version.
  f_mark_ptr = message.f_new_mark_ptr;
}
