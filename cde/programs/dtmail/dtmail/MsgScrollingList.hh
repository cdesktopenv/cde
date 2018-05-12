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
 *	$TOG: MsgScrollingList.hh /main/12 1998/10/22 19:56:25 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef MSGSCROLLINGLIST_H
#define MSGSCROLLINGLIST_H

#ifdef DEAD_WOOD
#include "Image.h"
#endif /* DEAD_WOOD */
#include "MsgHndArray.hh"
#include "RoamCmds.h"
#include "ScrollingList.h"
#include "UIComponent.h"
#include "XmStrCollector.h"
#include "XtArgCollector.h"


//#include "UndeleteMessageFromListDialog.h"

extern "C" {
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
}

#include <Xm/Xm.h>

/* MSGLIST_*       msgid 200 -299
 */
#define MSGLIST_SELMV    200
#define MSGLIST_SELCP    201
#define MSGLIST_NORC     202
#define MSGLIST_NOBOX    203
#define MSGLIST_NOOPEN   204
#define MSGLIST_MSGMV    205
#define MSGLIST_MSGSMV   206
#define MSGLIST_MSGCP    207
#define MSGLIST_MSGSCP   208
#define MSGLIST_NOSEL    209
#define MSGLIST_RAISE    210
#define MSGLIST_EXIT	 211
#define MSGLIST_OKBUT	 212
#define MSGLIST_DLGTTL	 213

class Sort;
class MsgScrollingList : public ScrollingList {
    
  public:
    
    MsgScrollingList ( RoamMenuWindow *, Widget, char * );    
    ~MsgScrollingList ();
    
    // Accesors
    int  selected_item_position() { return _selected_item_position; }
    RoamMenuWindow *parent() { return _parent; }

#ifdef DEAD_WOOD
    void addChooseCommand( ChooseCmd * );
    void addDeleteCommand( DeleteCmd * );
#endif /* DEAD_WOOD */
    void select_next_item();
    void select_prev_item();

    DtMailMessageHandle msgno(int index);
    MsgStruct* get_message_struct(int index);

    DtMailMessageHandle current_msg_handle();
    int       position(DtMailMessageHandle msgno);
#ifdef DEAD_WOOD
    int       position(MsgStruct* a_msg_struct);
#endif /* DEAD_WOOD */
    
    virtual const char *const className() {return ("MsgScrollingList");}
    virtual void insertMsg(DtMailMessageHandle);
    virtual void insertDeletedMsg(DtMailMessageHandle);
    virtual void deleteSelected(Boolean silent = TRUE);

    int copySelected(DtMailEnv &, char *, int delete_after_copy, int silent);
    void items( XmString [], int );

#ifdef DEAD_WOOD
    void appendMsg(DtMailMessageHandle msg_hndl, int sess_num);

    void appendMsg(DtMailMessageHandle msgno);

    DtMailMessageHandle lastMsg();
#endif /* DEAD_WOOD */

    void clearMsgs();

    void updateListItems(int current,
			 Boolean renumber_only = TRUE,
			 MsgHndArray *selected = NULL);
    DtMailBoolean senderIsToHeaderWhenMailFromMe(void);
    void checkDisplayProp(void);

    XmString formatHeader(DtMailHeaderLine & retrieved, 
			  int sess_num,
			  DtMailBoolean has_attachments,
			  DtMailBoolean new_msg);

    void visibleItems(int n_vis)
    {
	XtVaSetValues(_w, XmNvisibleItemCount, n_vis, NULL);
    }

    int visibleItems(void)
    {
	int n_vis;
	XtVaGetValues(_w, XmNvisibleItemCount, &n_vis, NULL);
	return(n_vis);
    }

    Widget get_scrolling_list();
    MsgHndArray   *selected();
    DtMailBoolean show_with_attachments(DtMailMessageHandle);
    DtMailBoolean show_with_attachments(DtMail::Message * msg);

    void	display_message( DtMailEnv &, DtMailMessageHandle );
    void	display_and_select_message(DtMailEnv &, DtMailMessageHandle );

    // Select all of the messages in the list and display the last one.
    void	select_all_and_display_last(DtMailEnv &);

    //
    // Select all of the messages in the array[] of DtMailMessageHandle.
    // 'array' is NULL terminated list.
    //
    void	select_all_and_display_last(DtMailEnv &,
					    DtMailMessageHandle *array,
					    unsigned int	elements);

    void	display_no_message();
    void 	scroll_to_bottom();
    void	scroll_to_position( int );
    virtual void	extended_selection( DtMailEnv &, int );
    int		get_selected_item();
    int		get_displayed_item();
    int		get_num_new_messages() { return num_new_messages; }
    int		get_num_deleted_messages() { return num_deleted_messages; }
    void	expunge(void);
    int	 	resetIndexNums(void);
    int	 	resetSessionNums(void);
    void 	layoutLabels();
    void 	layoutLabels(Widget, Widget, Widget, Widget);
    int		get_num_messages() { return _msgs->length(); }
    MsgHndArray	*get_deleted_messages() { return _deleted_messages; }
    MsgHndArray *get_messages() { return _msgs; }
    void	sort_messages();
    
    void	viewInSeparateWindow(DtMailEnv &);
    void	shutdown();

    void	undelete_messages(MsgHndArray *);
    void	undelete_last_deleted();
    void	display_message_summary();
#ifdef DEAD_WOOD
    void	display_message_selected();
#endif /* DEAD_WOOD */
    void	display_message(DtMailEnv &, int a_position);

    int		load_headers(DtMailEnv &);
    void	load_headers(DtMailEnv &, DtMailMessageHandle last);
    void	do_list_vis_adjustment();
    
    // Save these to adjust as msg numbers changed ON/OFF.
    Widget  _sender_lbl;
    Widget  _subject_lbl;
    Widget  _date_lbl;
    Widget  _size_lbl;

  protected:
    virtual void defaultAction( Widget, XtPointer, XmListCallbackStruct * );
    static void extendedSelectionCallback( Widget, XtPointer, XmListCallbackStruct * );
    MsgHndArray *_msgs;
    DtMailHeaderRequest	_header_info;
   
    // Msgs Popup  stuff.
    Widget   _msgsPopupMenu;
    MenuBar  *_menuPopupMsgs;

  private:
#ifdef DEAD_WOOD
    Image *_attach_image;
    Image *_letter_image;
#endif /* DEAD_WOOD */
    RoamMenuWindow *_parent;
    ChooseCmd *_choose;  
    DeleteCmd *_delete;
    int	_selected_item_position;
    int _displayed_item_position;
    int session_message_number;

    int  num_new_messages;
    int	 num_deleted_messages;
    MsgHndArray *_deleted_messages;

    DtMailBoolean	 _numbered;
    Boolean		 _selection_on;
    Sort		*_sorter;

    // Collect the arguments to SetValues in load_headers
    // This prevents multiple redisplays of the XmList widget
    XtArgCollector	*_xtarg_collector;

    // Collect the XmString items in load_headers
    XmStrCollector	*_xmstr_collector;

    // Save the XmString that was allocated so that we can
    // free it later.  It would probably be better to add this
    // to the XtArgCollector class, but since it's only one
    // item it's easier to do it this way.
    XmString		_selected_items;
};

#endif
