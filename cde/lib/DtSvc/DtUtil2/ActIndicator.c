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
/* $TOG: ActIndicator.c /main/5 1998/07/30 12:11:42 mgreess $ */

/******************************************************************************
 *
 * File Name: Indicator.c
 *
 *  This file defines the API for interacting with the activity indicator.
 *
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 *
 *****************************************************************************/

#include <X11/Intrinsic.h>

#include <Dt/DtP.h>
#include <Dt/IndicatorM.h>
#include <Tt/tttk.h>


/********    Public Function Declarations    ********/

extern void _DtSendActivityNotification( int ) ;
extern void _DtSendActivityDoneNotification( void ) ;

/********    End Public Function Declarations    ********/



/*
 * _DtSendActivityNotification()
 *
 *   This function provides the client with a means for broadcasting
 *   notification that an activity has been started.  For the present
 *   time, this will enable the activity indicator for upto a specified 
 *   number of seconds.
 */

void 
_DtSendActivityNotification(
        int duration )
{
    Tt_message	msg;
    Tt_status	status;

    msg = tt_pnotice_create(TT_SESSION, "DtActivity_Beginning");
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
	return;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
	return;
    }
    tt_message_destroy(msg);
}


/*
 * _DtSendActivityDoneNotification()
 *
 *   This function provides the client with a means for broadcasting
 *   notification that an activity which had earlier been started, is
 *   now down.
 */

void 
_DtSendActivityDoneNotification( void )
{
    Tt_message	msg;
    Tt_status	status;

    msg = tt_pnotice_create(TT_SESSION, "DtActivity_Began");
    status = tt_ptr_error(msg);
    if (status != TT_OK) {
	return;
    }
    status = tt_message_send(msg);
    if (status != TT_OK) {
	return;
    }
    tt_message_destroy(msg);
}
