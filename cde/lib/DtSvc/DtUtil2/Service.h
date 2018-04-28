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
 * File:         Service.h $XConsortium: Service.h /main/3 1995/10/26 15:27:03 rswiston $
 * Language:     C
 *
 * (C) Copyright 1993, Hewlett-Packard, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_Service_h
#define _Dt_Service_h

/************* DATA TYPES *****************************************/

/* Many of the data types are standard DT types. */
#include <Dt/DataTypes.h>

/* Built on ICCC-based messaging library */
#include <Dt/Msg.h>

/* Specific messaging data types */
typedef struct _DtSvcHandle *	DtSvcHandle;
typedef DtMsgContext		DtSvcMsgContext;

/************* MESSAGE DEFINITIONS *********************************/
#define DT_SVC_MSG_SUCCESS	"SUCCESS"
#define DT_SVC_MSG_FAIL	"FAIL"
#define DT_SVC_MSG_REQUEST	"REQUEST"
#define DT_SVC_MSG_NOTIFY	"NOTIFY"

/************* CALLBACK PROTOTYPES ******************************
 */

typedef void (*DtSvcReceiveProc) ();
/*
    DtSvcHandle	service,
    DtSvcMsgContext	reply_context,
    Pointer		client_data,
    String *		message_fields,
    int			num_fields);
*/
/*
 *  service		A handle for the service.
 *
 *  reply_context	Opaque context information for the request that 
 *			was received.  This data is needed when 
 *			generating a reply to a request.
 *
 *  client_data		A pointer to the data that was specified when
 *			the callback was registered.
 *
 *  message_fields	A pointer to an array of strings that is the
 *			contents of the request. (See note 2.)
 *
 *  num_fields		The number of fields in the message_fields 
 *			array.
 */

typedef void (*DtSvcMessageProc) ();
/*
    DtSvcHandle	service,
    Pointer		client_data,
    String *		message_fields,
    int			num_fields);
*/
/*
 *  service		A handle for the service.
 *
 *  client_data		A pointer to the data that was specified when
 *			the callback was registered.
 *
 *  message_fields	A pointer to an array of strings that is the
 *			contents of the message. (See note 2.)
 *
 *  num_fields		The number of fields in the message_fields 
 *			array.
 */

typedef void (*DtSvcStatusProc) ();
/*
    DtSvcHandle	service,
    int			status,
    Pointer		client_data);
*/
/*
 *  service		Handle to the service whose status is being
 *			reported.
 *
 *  status		The status of the service being started.
 *
 *  client_data		Pointer to the data that was registered when
 *			the callback was registered.
 */

/************* CONSTANTS ******************************************
 */

/* The following are types of reply messages.
 */

#define DT_SVC_SUCCESS		 1	/* The request succeeded. */

#define DT_SVC_FAIL		-1	/* The service failed to carry
					 * out the request.
					 */

#define DT_SVC_DELIVERY_FAIL	-2	/* The request could not be 
					 * delivered to the service for
					 * some reason.  For example,
					 * the service may not be running
					 * and cannot be invoked.
					 */

/* The following are types of status.
 */

#define DT_SVC_START		 1	/* The service was started. */

#define DT_SVC_NO_START	-1	/* The service failed to start.
					 */

#define DT_SVC_LOST		-2	/* The service was lost. Another
					   provider took over ownership
					   for this service. */

#endif /*_Dt_Service_h*/
/* Do not add anything after this endif. */
