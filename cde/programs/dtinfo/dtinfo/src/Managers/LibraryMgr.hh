/*
 * $XConsortium: LibraryMgr.hh /main/5 1996/09/17 09:18:18 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
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

# include "UAS.hh"

class OutlineList;
class LibraryAgent;
class OutlineElement;

class LibraryMgr : public Long_Lived
{
public: // functions
  LibraryMgr();
  ~LibraryMgr();

  // Display a new top-level entry (not copied from existing window) 
  void display (UAS_Pointer<UAS_Common> &);
  // Display a "detached" part of an existing window
  void display (OutlineElement *subtree);
  void undisplay (OutlineElement *root);
  void remove (UAS_Pointer<UAS_Common> lib);

  OutlineList *library();

  // How the Node mgr notifies us of other toplevel windows being visible
  void windows_notify (bool visible);

  // How the Node mgr knows whether we have visible windows or not
  bool visible_windows() { return f_visible_num != 0; }

  // How the agent notifies the manager of its unmapping: 
  void agent_unmapped (LibraryAgent *);

  void init(UAS_List<UAS_String>&);

  bool lib_exist(UAS_String&);

protected: // functions
  void create_library();
  void agent_destroyed (LibraryAgent *, u_int, void *, LibraryAgent *);

private: // variables
  LibraryAgent          *f_first_agent;
  UAS_List<UAS_Common>   fObjList;
  xList<LibraryAgent *>  f_agent_list;
  unsigned int           f_visible_num;

private:
  LONG_LIVED_HH(LibraryMgr,library_mgr);
};

LONG_LIVED_HH2(LibraryMgr,library_mgr);
