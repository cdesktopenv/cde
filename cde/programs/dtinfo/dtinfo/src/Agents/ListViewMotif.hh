/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: ListViewMotif.hh /main/4 1996/06/11 18:28:31 cde-hal $
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

class List;
class WindowSystem;
class WTopLevelShell;
class WXmList;
class WXmForm ;
// NOTE: how about an include to just declare all classes?? +/-'s? 

// NOTE: In the future using notifies, agents should only need to know about
// the window system. 6/19/92 djb 

#include <WWL/WXmPushButton.h>

class ListView : public Agent
{
public: // notification types
  enum { ENTRY_ACTIVATE = FolioObjectLast, _LAST };
  
public:  // functions 
ListView (List *the_list = NULL, char *name = "ListView")
    : f_the_list(NULL), f_shell (NULL), f_name (name)
    {
      list (the_list);
    }
  ~ListView();

  // displaying
  virtual void display();

  // accessing
  void list (List *list);
  const List *list();

protected: // virtual functions 
  virtual XmString display_value(FolioObject *);

protected: // functions
  void create_ui_objects();
  void close_window (WCallback *wcb);
  void display_list();
  void select (WCallback *);
  void activate (WCallback *);
  void list_changed (FolioObject *object, u_int notify_type,
		     void *notify_data, void *dependent_data);
  
  /* -------- for children -------- */
  virtual void child_create_ui_pre(WXmForm &form);
  virtual void child_create_ui_post(WXmForm &form);

protected: // view variables
  List           *f_the_list;

protected: // ui variables
  WTopLevelShell *f_shell;
  char           *f_name;
  WXmList        *f_list;
  WCallback      *f_wm_delete_callback;
  int             f_selected_item;
  WXmPushButton   f_activate;
};


// NOTE: For now a list view includes a dialog box.  In a real OO UI toolkit
// it wouldn't, but we don't have time for that now :-( 5/19/92 djb
