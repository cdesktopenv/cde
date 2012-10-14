/*
 * $XConsortium: NodeMgr.hh /main/7 1996/09/04 20:11:55 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include "UAS.hh"

class NodeViewInfo;
class NodeHandle;
class NodeWindowAgent;
class HitList ;

struct SelectionChanged : public Destructable
{
};

class NodeMgr : public Long_Lived, public UAS_Sender<SelectionChanged>
{
public:  // functions
  NodeMgr();

  void display (UAS_Pointer<UAS_Common> &);
  // Force the creation of a new window for the next displayed node. 
  void force_new_window()
    { f_force_new_window = TRUE; }
  void set_preferred_window (NodeWindowAgent *nwa)
    { f_preferred_window = nwa; }
  NodeWindowAgent *get_preferred_window ()
    { return f_preferred_window; }
  int font_scale()
    { return (f_font_scale); }
  UAS_Pointer<UAS_Common> &last_displayed()
    { return (f_last_displayed); }
    
  // How the agent notifies the manager of its destruction: 
  void agent_deleted (NodeWindowAgent *);

  // force all nodes to be re-read, re-drawn (new style sheet)
  void re_display_all();

  // for old style.sheet 
  void set_font_scale(int);

  NodeViewInfo *load(UAS_Pointer<UAS_Common> &node_ptr);

  // How the Library mgr notifies us of other toplevel windows being visible
  void windows_notify (bool visible);

  // How the Library mgr knows whether we have visible windows or not
  bool visible_windows() { return f_agent_list.length() != 0; }

  // return list of pointers to the visible node windows
  xList<NodeWindowAgent *> &windows() { return f_agent_list; }

private:  // functions 
  NodeWindowAgent *create_agent();

private:  // variables 
  bool             f_force_new_window;
  NodeWindowAgent *f_preferred_window;
  UAS_Pointer<UAS_Common>    f_last_displayed;
  int		   f_font_scale ; // for old style.sheet 
  xList<NodeWindowAgent *> f_agent_list;

  LONG_LIVED_HH(NodeMgr,node_mgr);
};

LONG_LIVED_HH2(NodeMgr,node_mgr);

