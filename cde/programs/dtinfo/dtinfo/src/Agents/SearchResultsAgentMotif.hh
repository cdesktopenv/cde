/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: SearchResultsAgentMotif.hh /main/4 1996/11/18 16:31:10 cde-hal $
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

class NodeListAgent;
class Manager;
class ResultID;
class WXmText;
class AgentListEntry;
class WXmToggleButton;

class SearchResultsAgent : public NodeListAgent
{
public: // functions
  SearchResultsAgent();

  void init();
  XmString form_result_string (UAS_Pointer<UAS_SearchResultsEntry>);
  void display (ResultID *results);
  void select_item (WCallback *wcb);
  void view_activate (WCallback *);
  void retain_changed (WCallback *wcb);
  void set_agent_list_entry (AgentListEntry *my_ale)
    { f_my_ale = my_ale; }
  bool retain ()
    { return f_retain; }

  ResultID *resultID()
    { return f_results; }

  static Boolean fill_list_wp (XtPointer client_data);
  Boolean fill_list(UAS_List<UAS_SearchResultsEntry> &);

protected: // functions
  virtual void create_window();
  virtual void close_window (WCallback *wcb);
  void  set_retain_default ();
  void modify_verify (WCallback *wcb);

  void resize();

private:
  static void resizeCB(Widget, XtPointer, XEvent*, Boolean*);
  void compose_header();

protected: // variables
  AgentListEntry *f_my_ale;
  bool   f_retain;
  ResultID *f_results;
  int       f_selected_item;
  WXmToggleButton *f_retain_toggle;
  WXmText  *f_query_text;
  WXmLabel *f_hits_label;
  WXmLabel *f_scope_label;

  unsigned int f_docs_to_display ;
  unsigned int f_count ;
  long f_scale;
  XtWorkProcId f_work_proc_id;
  bool f_popped_up;
  Dimension f_header_indent;
};

// inlines:

inline
SearchResultsAgent::SearchResultsAgent()
: f_work_proc_id (0), f_popped_up(FALSE)
{
}
