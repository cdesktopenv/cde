/*
 * $XConsortium: NodeListAgentMotif.hh /main/3 1996/06/11 16:15:07 cde-hal $
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


class Manager;
class WTopLevelShell;
class WXmList;
class WXmForm;
class WXmMenuBar;
class WXmTextField;
class WXmLabel;
class WXmPanedWindow;

#include <WWL/WXmPushButton.h>

class NodeListAgent : public Agent
{
public: // functions
  NodeListAgent() {}
  ~NodeListAgent();

  void init ();
  void display ();

protected: // functions
  virtual void create_base_window (char *name);
  virtual void close_window (WCallback *wcb);
  static Cardinal pane_insert_position (Widget);

protected: // variables
  WTopLevelShell	*f_shell;
  WXmForm  		*f_form;
  WXmMenuBar		*f_menu_bar;
  WXmForm		*f_list_form ;
  WXmList               *f_list;
  WXmForm  		*f_panel;
  WXmPanedWindow        *f_pane;
  static Cardinal        f_pane_insert_position;
  WCallback *f_wm_delete_callback;
  WXmPushButton          f_display;
  WXmPushButton          f_help;
  /* WXmTextField          *f_selection; */
};
