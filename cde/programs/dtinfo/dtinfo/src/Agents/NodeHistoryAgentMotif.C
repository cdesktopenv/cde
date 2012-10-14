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
// $TOG: NodeHistoryAgentMotif.C /main/13 1998/04/17 11:34:27 mgreess $
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/
/*
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

#include "UAS.hh"

#define C_TOC_Element
#define L_Basic

#define C_xList
#define L_Support

#define C_GlobalHistoryMgr
#define C_MessageMgr
#define C_NodeMgr
#define L_Managers

#define C_HelpAgent
#define C_NodeHistoryAgent
#define L_Agents

#define C_WindowSystem
#define L_Other

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"
#include "Managers/WString.hh"

#include <Prelude.h>

#include "Registration.hh"

#include <WWL/WXmForm.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmLabel.h>

#define CLASS NodeHistoryAgent
#include "create_macros.hh"


NodeHistoryAgent::~NodeHistoryAgent()
{
  if (f_shell != 0)
    f_shell.Destroy();
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
NodeHistoryAgent::display()
{
  if (f_shell == 0)
    create_ui();

  refresh_list();
  ON_DEBUG (puts ("Popping up the node hist list"));
  f_shell.Popup();
  f_shell.DeIconify();
  f_popped_up = TRUE;
}


// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

#define SECTION_OFFSET 150

void
NodeHistoryAgent::create_ui()
{
  XmStringLocalized mtfstring;
  String	    string;

  f_shell = WTopLevelShell (window_system().toplevel(), WPopup, "node_hist");
  window_system().register_shell (&f_shell);

  string = CATGETS(Set_NodeHistoryAgent, 1, "Dtinfo: Section History");
  XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

  DECL  (WXmForm,         form,       f_shell,   "form");
  ASSNM (WXmPushButton,   f_display,  form,      "display");
  DECLM (WXmPushButton,   close,      form,      "close");
  DECLM (WXmPushButton,   help,       form,      "help");
  DECLM (WXmSeparator,    sep,        form,      "separator");

  mtfstring = CATGETS(Set_AgentLabel, 165, "Display");
  XtVaSetValues(f_display, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 12, "Close");
  XtVaSetValues(close, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);

  // set up column labels (Book, Section) 
  WXmLabel booklabel	(form, "book", WAutoManage);
  WXmLabel sectionlabel	(form, "section", WAutoManage);

  mtfstring = CATGETS(Set_AgentLabel, 184, "Book");
  XtVaSetValues(booklabel, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 185, "Section");
  XtVaSetValues(sectionlabel, XmNlabelString, (XmString)mtfstring, NULL);

  Widget scrolled_list =  XmCreateScrolledList (form, (char*)"list", NULL, 0);
  f_list = WXmList (scrolled_list);
  f_list.Manage();

  XmFontList defaultList;
  {
    XmFontList font;
    XtVaGetValues(scrolled_list, XmNfontList, &font, NULL);
    defaultList = XmFontListCopy(font);
  }

  if (window_system().dtinfo_space_font())
    defaultList = XmFontListAppendEntry(defaultList,
					window_system().dtinfo_space_font());
  XtVaSetValues(scrolled_list, XmNfontList, defaultList, NULL);

  SET_CALLBACK (f_shell,Popdown,popdown);
  SET_CALLBACK (f_list,SingleSelection,select);
  SET_CALLBACK (f_list,BrowseSelection,select);
  SET_CALLBACK (f_list,DefaultAction,view);
  ON_ACTIVATE (f_display,view);
  ON_ACTIVATE (close,close);
  help_agent().add_activate_help (help, (char*)"node_hist_help");

  global_history_mgr().UAS_Sender<HistoryAdd>::request (this);
  global_history_mgr().UAS_Sender<HistoryDelete>::request (this);

  form.DefaultButton (f_display);
  form.ShadowThickness(0);
  form.Manage();

  // Set the min size based on the current size.
  f_shell.Realize();
  f_shell.MinWidth (f_shell.Width());
  f_shell.MinHeight (f_shell.Height());

// account for list offset in its frame 
#define MARGIN_FACTOR 3
  int offset = 0 ;
  booklabel.LeftOffset(MARGIN_FACTOR + offset); 

  sectionlabel.LeftPosition(0);
  sectionlabel.LeftAttachment(XmATTACH_POSITION);
  sectionlabel.LeftOffset(SECTION_OFFSET + MARGIN_FACTOR + offset);
#undef MARGIN_FACTOR

  WXmPrimitive prim(XtParent(f_list));
  prim.TopWidget(booklabel);
  prim.TopAttachment(XmATTACH_WIDGET);
}


// /////////////////////////////////////////////////////////////////
// refresh_list - get all history list items and display them
// /////////////////////////////////////////////////////////////////

void
NodeHistoryAgent::refresh_list()
{
  Xassert (f_list != 0);

  xList<UAS_Pointer<UAS_Common> > &hist_list = global_history_mgr().history_list();
  XmStringTable table = new XmString [hist_list.length()];
  List_Iterator<UAS_Pointer<UAS_Common> > hl (hist_list);

  // Create the XmString values for the list.
  // Stick the items in the list in reverse order. 
  int i = hist_list.length() - 1;
  while (hl)
    {
      UAS_String bn = hl.item()->book_name(UAS_SHORT_TITLE);
      UAS_String tt = hl.item()->title();

      table[i--] = compose_entry(bn, tt);
      hl++;
    }

  // Set the items in the list. 
  f_list.DeselectAllItems();
  f_display.SetSensitive (False);
  f_list.Set (WArgList (XmNitems, (XtArgVal) table,
			XmNitemCount, hist_list.length(),
			NULL));

  // Free up memory used to make the list.
  for (i = hist_list.length() - 1; i >= 0; i--)
    XmStringFree (table[i]);
  delete table;
}


// /////////////////////////////////////////////////////////////////
// select - handle item selection
// /////////////////////////////////////////////////////////////////

void
NodeHistoryAgent::select (WCallback *wcb)
{
  CALL_DATA (XmListCallbackStruct,lcs);

  f_selected_item = lcs->item_position;

  ON_DEBUG (printf ("Selected item #%d\n", f_selected_item);)

  f_display.SetSensitive (True);
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
NodeHistoryAgent::view()
{
  Xassert (f_list.SelectedItemCount() == 1);
  Xassert (f_selected_item != 0);

  xList<UAS_Pointer<UAS_Common> > &hist_list = global_history_mgr().history_list();
  List_Iterator<UAS_Pointer<UAS_Common> > n (hist_list);
  int i = hist_list.length();

  // Skip ahead to the selected item in the list. 
  while (i > f_selected_item)
    i--, n++;

  // Now `n' points to the selected Node.
  n.item()->retrieve();
}

// /////////////////////////////////////////////////////////////////
// close
// /////////////////////////////////////////////////////////////////

void
NodeHistoryAgent::close()
{
  f_shell.Popdown();
}


void
NodeHistoryAgent::popdown()
{
  f_popped_up = FALSE;
  f_list.DeleteAllItems();
  f_selected_item = 0;
  f_display.SetSensitive (False);
}


// /////////////////////////////////////////////////////////////////
// receive
// /////////////////////////////////////////////////////////////////

void
NodeHistoryAgent::receive (HistoryAdd &message, void* /*client_data*/)
{
  if (!f_popped_up)
    return;
  // Add new item to the beginning (index 1) of the list.
  UAS_String bn = message.f_new_entry->book_name(UAS_SHORT_TITLE);
  UAS_String tt = message.f_new_entry->title();

  f_list.AddItemUnselected (compose_entry(bn, tt), 1);
  f_list.SetPos (1);
  if (message.f_moving)
    f_list.SelectPos (1, True);
}

XmString
NodeHistoryAgent::compose_entry(UAS_String book, UAS_String section)
{
  WXmString bookString((char*)book);
  WXmString sectString((char*)section);

  if (bookString.Width(f_list.FontList()) >= SECTION_OFFSET)
  {
    char* dots = (char*)"...";

    char* ungenerated = (char*)(bookString + WXmString(dots));

    WString anonym_wstring = ungenerated;
    wchar_t* buf = (wchar_t*)anonym_wstring;
    wchar_t* ptr = buf + wcslen(buf) - (strlen(dots)+1);

    XtFree(ungenerated);

    WXmString stake;
    WString wdots(dots);

    int allowance;
    do
    {
      char* str;
      memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

      stake = str = WString(buf).get_mbstr();

      allowance = SECTION_OFFSET - stake.Width(f_list.FontList());

      delete[] str;
    }
    while (allowance <= 0 && ptr >= buf);

    while (*ptr <= (wchar_t)' ' && ptr >= buf)
    {
      char* str;
      memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

      stake = str = WString(buf).get_mbstr();
      allowance = SECTION_OFFSET - stake.Width(f_list.FontList());

      delete[] str;
    }

    bookString = stake;
  }

  WXmString space(window_system().
		  make_space(SECTION_OFFSET -
			     bookString.Width(f_list.FontList()), f_list),
		  False);

  WXmString entry = bookString + space + sectString;

  return entry.disown();
}

void
NodeHistoryAgent::receive (HistoryDelete &message, void* /*client_data*/)
{
  if (!f_popped_up)
    return;
  // Remove old items from the list.
  int length = f_list.ItemCount();
  ON_DEBUG (printf ("----- history list delete: count %d, index %d, len %d\n",
		    message.f_count,
		    length - message.f_index - message.f_count + 1, length));
  // The list is stored in inverted form, ie: new items on top.
  int item = length - message.f_index - message.f_count + 1;
  if (f_selected_item == item && message.f_moving == TRUE)
    f_display.SetSensitive (False);
  f_list.DeleteItemsPos (message.f_count, item);
}
