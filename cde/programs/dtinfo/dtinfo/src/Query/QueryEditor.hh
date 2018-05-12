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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: QueryEditor.hh /main/8 1996/07/10 09:40:32 rcs $
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

class QueryGroup;
class QueryGroupView;

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmText.h>
#include <WWL/WXmArrowButton.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmScrolledWindow.h>

class QueryEditor : public WWL,
		    public UAS_Receiver<ScopeCreated>,
		    public UAS_Receiver<ScopeDeleted>,
		    public UAS_Receiver<ScopeRenamed>
{
public:
  QueryEditor(UAS_SearchEngine& se);

  void display();
  void edit_query (QueryGroup *query);

  Dimension min_term_width()
    { return (f_min_term_width); }
  void min_term_width (Dimension width)
    { f_min_term_width = width; }

  void increment_null_terms();
  void decrement_null_terms();
  void query_changed();

  void update_option_menu();

private:
  void create_ui();
  void search_activate();
  void cancel();
  void clear();
  void scope();
  void modify_verify (WCallback *wcb);

  void fill_menu();
  void receive (ScopeCreated &, void *client_data);
  void receive (ScopeDeleted &, void *client_data);
  void receive (ScopeRenamed &, void *client_data);

private:
  QueryGroup        *f_query;
  QueryGroupView    *f_query_view;
  WTopLevelShell     f_shell;
  WXmOptionMenu      f_scope_option;
  WXmScrolledText    f_query_text;
  WXmPushButton      f_cut_btn;
  WXmPushButton      f_copy_btn;
  WXmPushButton      f_paste_btn;
  WXmPushButton      f_group_btn;
  WXmPushButton      f_ungroup_btn;
  WXmPushButton      f_undo_btn;
  WXmPushButton      f_redo_btn;
  WXmArrowButton     f_hist_prev;
  WXmArrowButton     f_hist_next;
  WXmPushButton      f_search_btn;
  WXmScrolledWindow  f_query_area;
  Dimension          f_min_term_width;
  // Need to track empty terms to control "Search" sensitivity: 
  u_short            f_null_terms;
  inline friend QueryEditor &query_editor();
  static QueryEditor *f_query_editor;
};

inline 
QueryEditor &query_editor()
{
  return (*QueryEditor::f_query_editor);
}
