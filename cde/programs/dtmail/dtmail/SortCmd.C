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
/* $TOG: SortCmd.C /main/8 1998/10/22 19:57:10 mgreess $ */
#include "SortCmd.hh"
#include <stdlib.h>
#include <unistd.h>
#include "RoamApp.h"
#include "MailMsg.h"
#include "MotifCmds.h"
#include "MsgHndArray.hh"

SortCmd::SortCmd( 
    char *name, 
    char *label,
    int active, 
    RoamMenuWindow *window,
    enum sortBy sortstyle 
    ) : ToggleButtonCmd ( name, label, active, FALSE, XmONE_OF_MANY_ROUND )
{
	// initialize the internal state
	_sortparent = window;
	_sortstyle = sortstyle;
	_sorter = new Sort ();
}

void
SortCmd::doit()
{
	MsgScrollingList	*displayList;
	DtMail::MailBox		*mbox;
	int			current_msg;

    	theRoamApp.busyAllWindows(GETMSG(DT_catd, 1, 219, "Sorting..."));

	// Get Mailbox
	mbox = _sortparent->mailbox();

	// Get array of message handles from scrolling list
	displayList = _sortparent->list();

        MsgHndArray *selected_messages = displayList->selected();

	// Sort array of message handles
	current_msg = _sorter->sortMessages (displayList, mbox, _sortstyle);

	// The array of message handles is sorted. Now we need to update
	// the display preserving the state of the selected messages.
        displayList->updateListItems(current_msg, FALSE, selected_messages);

        delete selected_messages;


	_sortparent->last_sorted_by(_sortstyle);
	_sortparent->message_summary();

        theRoamApp.unbusyAllWindows();
}
