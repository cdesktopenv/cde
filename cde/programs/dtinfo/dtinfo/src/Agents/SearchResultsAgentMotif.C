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
 * $TOG: SearchResultsAgentMotif.C /main/17 1998/04/17 11:35:18 mgreess $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * wihtout the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 *"
 */

#include "UAS.hh"


#define C_TOC_Element
#define C_Atomizer
#define L_Basic

#define C_ResultID
#define L_OliasSearch

#define C_PrefMgr
#define C_SearchMgr
#define C_SearchResultsMgr
#define C_MessageMgr
#define C_NodeMgr
#define C_LibraryMgr
#define L_Managers

#define C_SearchResultsAgent
#define C_NodeListAgent
#define C_HelpAgent
#define L_Agents

#define USES_OLIAS_FONT

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"
#include "Managers/WString.hh"

#include <Prelude.h>
#include <iostream>
using namespace std;

#include "Registration.hh"

#include <WWL/WXmList.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmText.h>
#include <WWL/WXmToggleButton.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmPanedWindow.h>

#ifdef SVR4
#ifndef USL
#include <libintl.h>
#endif
#endif

static Boolean g_allow_query_text_change;

#define INITIAL_FILL_SIZE 1000
#define INCREMENTAL_FILL_SIZE 100


// /////////////////////////////////////////////////////////////////
// init
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::init()
{
  NodeListAgent::init();
}


// /////////////////////////////////////////////////////////////////
// form_result_string
// /////////////////////////////////////////////////////////////////

// Hard Coded pixel offset (from left margin) where Node Title Starts in
// result list - 14:36 07/26/93 - jbm
#define SECTION_OFFSET 150

XmString
SearchResultsAgent::form_result_string (UAS_Pointer<UAS_SearchResultsEntry> re)
{
  static char relevancy[3];
  if (relevancy[1] == '\0')
    {
      if (window_system().nofonts())
	relevancy[1] = ' ';
      else
	relevancy[1] = OLIAS_SPACE08;
    }

  // Scale all weights from to the range 0 to 8.  While 8 / scale
  // may look like a constant, it cannot be factored because integer
  // arithmetic rounding might adversely affect the results.  DJB
  relevancy[0] = OLIAS_RELEVANCY_ICON;
  if (f_scale != 0)
    relevancy[0] += (char) ((re->relevance() * 8 ) / f_scale);


  WXmString rel  (relevancy, (char*)OLIAS_FONT);

  WXmString book;
  WXmString name;

  if (re->book()!= "")
    {
      book = (char *) re->book();
      name = (char *) re->section();
    }
  else
    {
      // backward compatibility 

      UAS_Pointer<UAS_Common> doc_ptr = UAS_Common::create (re->id());
      UAS_String bn = doc_ptr->book_name (UAS_SHORT_TITLE);
      UAS_String tt = doc_ptr->title();
      book = bn;
      name = tt;
    }  

  int section_offset = f_list->Width() / 3;

  int allowance = section_offset - book.Width(f_list->FontList());
  WXmString space;
  if (allowance > 0) {
    // NOTE: space is hardcoded and will cause problems if font changes
    // -10:24 07/12/93 - jbm 
    XmString xmstr = window_system().make_space(allowance, *f_list);
    space = xmstr;
    XmStringFree(xmstr);
  }
  else {

    char* dots = (char*)"...";

    WXmString temp_xmstring = book + WXmString(dots);
    char *temp_str = (char*)temp_xmstring;

    WString temp_WString(temp_str);   
    wchar_t* buf = (wchar_t*) temp_WString;
    wchar_t* ptr = buf + wcslen(buf) - (strlen(dots)+1);

    WXmString stake;
    WString wdots(dots);
    do
    {
	char* str;	
	memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

	stake = str = WString(buf).get_mbstr();

        allowance = section_offset - stake.Width(f_list->FontList());

	delete[] str;
    }
    while (allowance <= 0 && ptr >= buf);

#if 0
    while (*ptr <= (wchar_t)' ' && ptr >= buf)
    {
	char* str;
	memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

	stake = str = WString(buf).get_mbstr();
	allowance = section_offset - stake.Width(f_list->FontList());

	delete[] str;
    }
#endif

    XmString xmstr = window_system().make_space(allowance, *f_list);
    space = xmstr;
    XmStringFree(xmstr);

    book = stake;
  }

  // Entire string must be disowned to prevent destructor of temporary
  // object from freeing its XmString. 

  XmString string = (rel + book + space + name).disown();

  return (string);
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

inline int
imin(int x, int y)
{
  return x < y ? x : y ;
}

void
SearchResultsAgent::resizeCB(Widget, XtPointer client_data, XEvent*, Boolean*)
{
  SearchResultsAgent* agent = (SearchResultsAgent*)client_data;

  agent->resize();
}

void
SearchResultsAgent::compose_header()
{
  WXmLabel* header =
	(WXmLabel*)XtNameToWidget((Widget)*f_list_form, "header");

  WXmString bookString = CATGETS(Set_AgentLabel, 184, "Book");
  WXmString sectString = CATGETS(Set_AgentLabel, 185, "Section");

  WXmString bookPostfix(
	window_system().make_space(
		f_list->Width() / 3 - bookString.Width(header->FontList()),
		*header
	),
	False);

  WXmString header_string = bookString + bookPostfix + sectString;

  header->LabelString((XmString)header_string);
}

void
SearchResultsAgent::resize()
{
  UAS_List<UAS_SearchResultsEntry>& rlist =
		*(f_results->results()->create_results(0, f_count));

  fill_list(rlist);

  compose_header();
}

void
SearchResultsAgent::display (ResultID *results)
{
  if (f_shell == NULL)
    create_window();

  f_results = results;

  // UAS_SearchEngine &se = search_mgr().search_engine();

  // get window up quick 

  f_docs_to_display = imin(results->ndocs(), pref_mgr().get_int(PrefMgr::MaxSearchHits));

#if 0
  if (INITIAL_FILL_SIZE < f_docs_to_display)
    f_count = INITIAL_FILL_SIZE;
  else
    f_count = f_docs_to_display ;
#else
  // INITIAL_FILL_SIZE(1000) is greater than the maximum of preference
  // so, f_docs_to_display never exceeds INITIAL_FILL_SIZE
  f_count = f_docs_to_display;
#endif

  UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > tmpList =
			  results->results()->create_results(0, f_count);
  UAS_List<UAS_SearchResultsEntry>& rlist =
		*(UAS_List<UAS_SearchResultsEntry> *)tmpList;

  // All entries scaled by the weight of the first because it is
  // be the max weight for the list. 
  assert( rlist[0] != 0);
  f_scale = rlist[0]->relevance ();

  // NOTE: Using bogus internal string below: 
  if (f_scale == 0)
    message_mgr().warning_dialog (
	(char*)UAS_String(CATGETS(Set_Messages, 69,
			"Search results weighting are not available.")));
  
  /* -------- Display the number of hits. -------- */
  char buffer[80];

  snprintf (buffer, sizeof(buffer), "%d of %d sections",
	   f_docs_to_display, results->ndocs());
  f_hits_label->LabelString (buffer);
  snprintf (buffer, sizeof(buffer), "%s",
		(char *)*(UAS_String*)(results->results()->scope_name()));
  f_scope_label->LabelString (buffer);

  /* -------- Display the text of the query. -------- */
  g_allow_query_text_change = True;

  f_query_text->Value ((char*)*(UAS_String*)(results->results()->query()));

  g_allow_query_text_change = False;

  f_popped_up = TRUE;
  NodeListAgent::display();

  fill_list(rlist);
  compose_header();

  if (pref_mgr().get_boolean (PrefMgr::DisplayFirstHit))
    {
      f_list->SelectPos (1, True);
    }
  else
    {
      f_list->DeselectAllItems();
      f_display.SetSensitive (False);
    }

  XtAddEventHandler((Widget)*f_list_form, StructureNotifyMask, False,
				resizeCB, (XtPointer)this);
}


// /////////////////////////////////////////////////////////////////
// fill_list_wp
// /////////////////////////////////////////////////////////////////

Boolean
SearchResultsAgent::fill_list_wp (XtPointer /*client_data*/)
{
#if 0
  return ((SearchResultsAgent *) client_data)->fill_list();
#else
  return True;
#endif
}


// /////////////////////////////////////////////////////////////////
// fill_list
// /////////////////////////////////////////////////////////////////

Boolean
SearchResultsAgent::fill_list(UAS_List<UAS_SearchResultsEntry>& rlist)
{
  static XmString string_table[INITIAL_FILL_SIZE];
  XmString *t = string_table;

  unsigned int string_count = 0 ;
  unsigned int i;
  for (i = 0; i < rlist.length(); i ++) {
      string_count++ ;
      *t++ = form_result_string (rlist[i]);
  }

  f_list->SetPos (1);
  f_list->Set (WArgList (XmNitems, (XtArgVal) string_table,
			 XmNitemCount, f_count,
			 NULL));

  for (i = 0 ; i < string_count; i++) {
     XmStringFree(string_table[i]);
     string_table[i] = NULL;
  }

#if 0
  static XmString string_table [INCREMENTAL_FILL_SIZE];
  XmString *t = string_table;

  int number, num_remaining = f_docs_to_display - f_count;

  if (INCREMENTAL_FILL_SIZE < num_remaining)
    number = INCREMENTAL_FILL_SIZE;
  else
    number = num_remaining;

  UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > tmpList =
			  f_results->results()->create_results(f_count, number);
  UAS_List<UAS_SearchResultsEntry> & rlist =
			*(UAS_List<UAS_SearchResultsEntry>*)tmpList;

  int i;
  for (i = 0; i < rlist.length(); i ++) {
    *t++ = form_result_string (rlist[i]);
  }
  // 0 means add at last position in the list. 
  XmListAddItems (*f_list, (XmString *) string_table, number, 0);

  f_count += number;

  for (i = 0 ; i < number; i++)
    XmStringFree ((XmString) string_table[i]);

  // Return True if the list is complete, False otherwise.
  if (f_count == f_docs_to_display)
    {
      f_work_proc_id = NULL;
      return (True);
    }
  else
    {
      return (False);
    }
#else
  return True;
#endif
}


// /////////////////////////////////////////////////////////////////
// create_window
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::create_window()
{
  create_base_window ((char*)"results");

  XmStringLocalized mtfstring;
  String	    string;

  string = CATGETS(Set_SearchResultsAgent, 1, "Dtinfo: Search Results");
  XtVaSetValues((Widget)*f_shell, XmNtitle, string, NULL);

  help_agent().add_activate_help(f_help, (char*)"results_help");

#define AM WAutoManage
  // NOTE: need form to contain things in the same row 'cause of resizing
  // rtp
#if 1
  f_retain_toggle =
    new WXmToggleButton (*f_form, "retain", AM,
			 WArgList (XmNlabelPixmap,
				   (XtArgVal)window_system().unlocked_pixmap(*f_form),
				   XmNselectPixmap,
				   window_system().semilocked_pixmap(*f_form),
				   NULL));
#else
  f_retain_toggle =  new WXmToggleButton (*f_form,     "retain",         AM);
#endif
  WXmLabel retrieved (*f_form, "retrieved", AM);
  f_hits_label = (WXmLabel*)(Widget) WXmLabel (*f_form,    "num_hits", AM);
  WXmLabel scope (*f_form, "scope", AM);
  f_scope_label = (WXmLabel*)(Widget) WXmLabel (*f_form,   "scope_name", AM);
  WXmLabel query_label (*f_form, "query_label", AM);

  mtfstring = CATGETS(Set_AgentLabel, 219, "Retrieved:");
  XtVaSetValues(retrieved, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 194, "File a Bug");
  XtVaSetValues((Widget)f_hits_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 220, "Scope:");
  XtVaSetValues(scope, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 194, "File a Bug");
  XtVaSetValues((Widget)f_scope_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 221, "Query:");
  XtVaSetValues(query_label, XmNlabelString, (XmString)mtfstring, NULL);
  
  Widget textw = XmCreateScrolledText (*f_pane, (char*)"query_text", NULL, 0);
  f_query_text = new WXmText (textw);
  f_query_text->Manage();
//  WXmPushButton      editq           (*f_panel,       "edit_query",     AM);

  f_retain_toggle->SetValueChangedCallback
    (this, (WWL_FUN) &SearchResultsAgent::retain_changed);
  set_retain_default();
  
  {
    CXmForm pane (*f_pane);
    WArgList args;

    pane.TopWidget (query_label, args);
    pane.TopAttachment (XmATTACH_WIDGET, args);
    pane.Set (args.Args(), args.NumArgs());
  }

  static char relevancy[3];
  if (relevancy[0] == '\0')
    {
      relevancy[0] = OLIAS_RELEVANCY_ICON;
      if (window_system().nofonts())
	relevancy[1] = ' ';
      else
	relevancy[1] = OLIAS_SPACE08;
    }

  // set up column labels (Book, Section) 
  WXmLabel header	(*f_list_form, "header", AM);

  { // header needs dtinfo space font
    XmFontList header_font = XmFontListCopy(header.FontList());

    if (window_system().dtinfo_space_font())
      header_font = XmFontListAppendEntry(header_font,
					  window_system().dtinfo_space_font());

    header.FontList(header_font);
  }

  int offset = WXmString(relevancy,(char*)OLIAS_FONT).Width(f_list->FontList());
  header.LeftOffset(offset); 

  WXmPrimitive prim(XtParent(*f_list));
  prim.TopWidget(header);
  prim.TopAttachment(XmATTACH_WIDGET);

  f_pane->Manage();
  f_form->Manage();
  
  f_list->SetSingleSelectionCallback (this,
			      (WWL_FUN) &SearchResultsAgent::select_item);
  f_list->SetBrowseSelectionCallback (this,
			      (WWL_FUN) &SearchResultsAgent::select_item);
  f_list->SetDefaultActionCallback (this,
			    (WWL_FUN) &SearchResultsAgent::view_activate);
  f_display.SetActivateCallback (this,
			    (WWL_FUN) &SearchResultsAgent::view_activate);
  f_query_text->SetModifyVerifyCallback (this,
			    (WWL_FUN) &SearchResultsAgent::modify_verify);

  XmProcessTraversal (f_display, XmTRAVERSE_CURRENT);
#undef AM
}

// /////////////////////////////////////////////////////////////////
// select_item
// /////////////////////////////////////////////////////////////////

// NOTE: perhaps this should be in the base class?? 6/12/92 djb 

void
SearchResultsAgent::select_item (WCallback *wcb)
{
  XmListCallbackStruct *lcs = (XmListCallbackStruct *) wcb->CallData();

  f_selected_item = lcs->item_position - 1;

  ON_DEBUG (printf ("Selected item #%d\n", f_selected_item);)

  f_display.SetSensitive (True);
}


// /////////////////////////////////////////////////////////////////
// view_activate
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::view_activate (WCallback *)
{
  if (f_list->SelectedItemCount() != 1)
    return;

  Wait_Cursor bob;

  UAS_String target;

  UAS_List<UAS_SearchResultsEntry> & rlist = f_results->results()->results();

  search_mgr().current_hits (rlist[f_selected_item]->create_matches());
  target = rlist[f_selected_item]->id();

  UAS_Pointer<UAS_Common> doc_ptr = UAS_Common::create (target);

  UAS_String temp_str = doc_ptr->lid();
  if (library_mgr().lib_exist(temp_str)) {
      doc_ptr->retrieve();
  }
  else {
      message_mgr().warning_dialog (
	  (char*)UAS_String(CATGETS(Set_Messages, 51,
	  "The document or section requested is not available.")));
  }
}

// /////////////////////////////////////////////////////////////////
// retain_changed
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::retain_changed (WCallback *wcb)
{
  XmToggleButtonCallbackStruct &tbcs =
    *((XmToggleButtonCallbackStruct *) wcb->CallData());

  f_retain = tbcs.set;

  if (f_retain)
    f_retain_toggle->WObject::Set (WArgList (XmNselectPixmap,
				    window_system().locked_pixmap(wcb->GetWidget()),
				    XmNlabelPixmap,
				    window_system().semilocked_pixmap(wcb->GetWidget()),
				    NULL));
  else
    f_retain_toggle->WObject::Set (WArgList (XmNselectPixmap,
				    window_system().semilocked_pixmap(wcb->GetWidget()),
				    XmNlabelPixmap,
				    window_system().unlocked_pixmap(wcb->GetWidget()),
				    NULL));
}

// /////////////////////////////////////////////////////////////////
// close_window
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::close_window (WCallback *wcb)
{
  if (f_popped_up == FALSE)
    return;
  f_popped_up = FALSE;

  NodeListAgent::close_window (wcb);
  search_results_mgr().deactivate (f_my_ale);
  
  set_retain_default();
}

// /////////////////////////////////////////////////////////////////
// set_retain_default - set retain to default settings
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::set_retain_default()
{
  // NOTE: hardcoded for now, user preference later

  f_retain = FALSE;
  f_retain_toggle->Set (False);
  f_retain_toggle->WObject::Set (WArgList (XmNselectPixmap,
				  window_system().semilocked_pixmap(f_retain_toggle->Parent()),
				  XmNlabelPixmap,
				  window_system().unlocked_pixmap(f_retain_toggle->Parent()),
				  NULL));
}


// /////////////////////////////////////////////////////////////////
// modify_verify
// /////////////////////////////////////////////////////////////////

void
SearchResultsAgent::modify_verify (WCallback *wcb)
{
  if (g_allow_query_text_change)
    return;

  XmTextVerifyPtr tvp = (XmTextVerifyPtr) wcb->CallData();

  // Editing never allowed. 
  tvp->doit = False;

  message_mgr().warning_dialog (
	(char*)UAS_String(CATGETS(Set_Messages, 47,
					"This is a display-only field.")));
}
