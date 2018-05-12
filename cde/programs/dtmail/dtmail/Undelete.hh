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
/*
 *+SNOTICE
 *
 *      $TOG: Undelete.hh /main/6 1998/09/02 15:58:26 mgreess $
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _UNDELETE_HH
#define _UNDELETE_HH

#include "DialogShell.h"
#include "MsgScrollingList.hh"
#include "CmdList.h"

/* UNDEL_*      msgid 600 - 699
 */
#define UNDEL_SENDER  600
#define UNDEL_SUB     601
#define UNDEL_DATE    602
#define UNDEL_SIZE    603
#define UNDEL_UNDEL   604
#define UNDEL_CLOSE   605
#define UNDEL_EXIT    606
#define UNDEL_OKBUT   607
#define UNDEL_DLGTTL  608

class RoamMenuWindow;

class UndelMsgScrollingList : public MsgScrollingList {
  public:
    UndelMsgScrollingList ( RoamMenuWindow *, Widget, char * );
    ~UndelMsgScrollingList ();
    void extended_selection(DtMailEnv &, int);
    void insertMsg(DtMailMessageHandle);
    void insertMsg(DtMailEnv &, MsgStruct *);
    void loadMsgs(DtMailEnv &, MsgHndArray *, int);

    virtual void deleteSelected(Boolean silent = TRUE);
};


class UndelFromListDialog : public DialogShell {
  public:
    UndelFromListDialog (char *, RoamMenuWindow *);
    ~UndelFromListDialog();
    Widget  createWorkArea(Widget);
    virtual void initialize();
    virtual void quit();


	// Accessors
	// Mutators
    void popped_down();
    void popped_up();
    void insertMsg(DtMailEnv &, MsgStruct *);
    void loadMsgs(DtMailEnv &, MsgHndArray *, int);

    void undelSelected();
    void undelLast();
    UndelMsgScrollingList *undelList() { return _list; }
    void addToRowOfButtons();

    void expunge(void);

    void replaceItems(XmString * items, int num_items) {
	if (_list) {
	    XmListReplaceItemsPos(_list->get_scrolling_list(), items, num_items, 1);
	}
    }

  protected:
    Cmd *_undelete_button;
    Cmd *_close_button;
  private:
    UndelMsgScrollingList *_list;
    Widget *_undel_list_button;
    Widget rowOfLabels, rowOfButtons;
    Widget rowOfMessageStatus;
    RoamMenuWindow *my_owner;
};



#endif
