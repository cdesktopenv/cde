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
 * File:         Message.h $XConsortium: Message.h /main/5 1996/03/01 16:36:42 drk $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_message_h
#define _Dt_message_h

#include <Dt/DataTypes.h>
#include <Dt/DtP.h>

#include <Dt/Service.h>

/* This header file is a modifed version of <Xv/Message.h> that 
 * provides compatibility between the BMS messaging and the new
 * ICCCM messaging used in DT.
 *
 * The following symbols that were defined in <Xv/Message.h> are
 * no longer available.  Any code that depended on them will have
 * to be rewritten:
 *
 *	DtServerDeathFn
 *	DtCloseMsgServerConnect
 *	DtAddFailNotificationCallback
 *	DtStatusResponse
 *	DtUniqueRequestId
 *	DtSendFailNotification
 *	DtSendMsg
 */


/**********************************
 *
 * Message Format
 *
 **********************************/

/*
 * DT messages have the following format:
 *
 *	Request message: 	<request> [args ...]
 *	Reply message:		<SUCCESS | FAILURE> [args ...]
 *	Notify message:		<notification> [args ...]
 *
 * The following fields in BMS messages are no longer supported:
 *
 *	DT_MSG_SENDER
 *	DT_MSG_REQUEST_ID
 *	DT_MSG_TOOL
 *	DT_MSG_HOST
 *	DT_MSG_DIR
 *	DT_MSG_FILE
 */

#define DT_MSG_TYPE		0
#define DT_MSG_COMMAND		0
#define DT_MSG_DATA_1		1
#define DT_MSG_DATA_2		2
#define DT_MSG_DATA_3		3
#define DT_MSG_DATA_4		4
#define DT_MSG_DATA_5		5
#define DT_MSG_DATA_6		6
#define DT_MSG_DATA_7		7
#define DT_MSG_DATA_8		8
#define DT_MSG_DATA_9		9
#define DT_MSG_DATA_10		10
#define DT_MSG_DATA_11		11
#define DT_MSG_DATA_12		12
#define DT_MSG_DATA_13		13
#define DT_MSG_DATA_14		14
#define DT_MSG_DATA_15		15
#define DT_MSG_DATA_16		16
#define DT_MSG_DATA_17		17
#define DT_MSG_DATA_18		18
#define DT_MSG_DATA_19		19
#define DT_MSG_DATA_20		20

#define DtDONT_CARE_FIELD		"*"
/*
	If a particular message does not require a value in one of the
	fields, use DtDONT_CARE_FIELD for the value of the field.
*/

#endif /*_Dt_message_h*/
/* Do not add anything after this endif. */
