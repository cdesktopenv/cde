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
 * $XConsortium: SearchHistoryListViewMotif.C /main/10 1996/11/14 16:08:06 cde-hal $
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

#include "iostream"
using namespace std;

#include "UAS.hh"

#define C_List
#define C_TOC_Element
#define L_Basic


#define C_ResultID
#define L_OliasSearch

#define C_SearchHistoryListView
#define L_Agents

#include <Prelude.h>

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"
#include "Managers/WString.hh"

#include "Registration.hh"

#include <WWL/WXmString.h>
#include <WWL/WXmList.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmLabel.h>

#include <stdio.h>
// /////////////////////////////////////////////////////////////////////////
// display_list()
// put the list in the Motif List Widget -> f_list 
// /////////////////////////////////////////////////////////////////////////

XmString
SearchHistoryListView::display_value(FolioObject *object)
{
  ResultID &rid = *(ResultID*)object;

  char buf[8];
  sprintf(buf, "%d", rid.ndocs());
  WXmString matchString = buf;

  UAS_Pointer<UAS_SearchResults> results = rid.results();  
  WXmString scopeString = (char*)*results->scope_name();

  WXmString queryString = rid.display_as();

  XmFontList fontlist = f_list->FontList();

  int number_width = WXmString("0").Width(fontlist) ;

  WXmString matchPostfix(
	window_system().make_space(
		8 * number_width - matchString.Width(fontlist),
		*f_list
	),
	False);

  int scope_field_width = 22 * number_width;

  int allowance;

  if ((allowance = scope_field_width - scopeString.Width(fontlist)) < 0)
  {
    char* dots = (char*)"...";

    // need to free ungenerated at the end
    char* ungenerated = (char*)(scopeString + WXmString(dots));

    wchar_t* buf = WString(ungenerated);
    wchar_t* ptr = buf + wcslen(buf) - (strlen(dots)+1);

    XtFree(ungenerated);

    WXmString stake;
    WString wdots(dots);

    do
    {
      char* str;
      memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

      stake = str = WString(buf).get_mbstr();
      allowance = scope_field_width - stake.Width(fontlist);

      delete[] str;
    }
    while (allowance <= 0 && ptr >= buf);

    while (*ptr <= (wchar_t)' ' && ptr >= buf)
    {
      char* str;
      memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

      stake = str = WString(buf).get_mbstr();
      allowance = scope_field_width - stake.Width(fontlist);

      delete[] str;
    }

    scopeString = stake;
  }

  WXmString scopePostfix(
	window_system().make_space(allowance, *f_list), False);

  WXmString entry_string = matchString + matchPostfix +
			   scopeString + scopePostfix +
			   queryString;

  return entry_string.disown();
}

void
SearchHistoryListView::child_create_ui_pre(WXmForm &form)
{
  Widget header = WXmLabel(form, "header", WAutoManage);

  XmFontList header_font;
  {
    XmFontList font;
    XtVaGetValues(header, XmNfontList, &font, NULL);
    header_font = XmFontListCopy(font);
  }

  if (window_system().dtinfo_space_font())
    header_font = XmFontListAppendEntry(header_font,
                                        window_system().dtinfo_space_font());

  XtVaSetValues(header, XmNfontList, header_font, NULL);

#ifdef FONT_DEBUG
  XmFontContext font_context;

  if (XmFontListInitFontContext(&font_context, header_font))
  {

    XmFontListEntry fle = NULL;
    do
    {
      if (fle = XmFontListNextEntry(font_context)) {
	char* tag = XmFontListEntryGetTag(fle);
	if (tag)
	  fprintf(stderr, "(DEBUG) tag=%s\n", tag);
      }
    }
    while (fle);
  
    XmFontListFreeFontContext(font_context);
  }
#endif

  int number_width = WXmString("0").Width(header_font);

  WXmString matchString = CATGETS(Set_AgentLabel, 246, "Matches");
  WXmString scopeString = CATGETS(Set_AgentLabel, 243, "Scope");
  WXmString queryString = CATGETS(Set_AgentLabel, 230, "Query");

  int scope_offset = 8 * number_width;

  if (matchString.Width(header_font) >= scope_offset)
  {
    char* dots = (char*)"...";

    char* ungenerated = (char*)(matchString + WXmString(dots));

    WString anonym_wstring = ungenerated; // important for digital compiler!
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
      
      allowance = scope_offset - stake.Width(header_font);

      delete[] str;
    }
    while (allowance <= 0 && ptr >= buf);

    while (*ptr <= (wchar_t)' ' && ptr >= buf)
    {
      char* str;
      memcpy(ptr--, (wchar_t*)wdots, (strlen(dots)+1) * sizeof(wchar_t));

      stake = str = WString(buf).get_mbstr();
      allowance = scope_offset - stake.Width(header_font);

      delete[] str;
    }
    
    matchString = stake;
  }

  WXmString matchPostfix(
	window_system().make_space(
		scope_offset - matchString.Width(header_font),
		header
	),
	False);
  WXmString scopePostfix(
	window_system().make_space(
		22 * number_width - scopeString.Width(header_font),
		header
	),
	False);
	
  WXmString header_string = matchString + matchPostfix +
			    scopeString + scopePostfix +
			    queryString;
			
  XtVaSetValues(header, XmNlabelString, (XmString)header_string, NULL);

}

void
SearchHistoryListView::display()
{
#ifdef VF_DEBUG
  cerr << "ListView::display()...";  
#endif
  ListView::display();
#ifdef VF_DEBUG
  cerr << "done." << endl; 
#endif
  XtVaSetValues(*f_shell, XmNtitle,
	CATGETS(Set_SearchHistoryList, 1, "Dtinfo: Search History"), NULL);
}
