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
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * $TOG: SmWatch.c /main/4 1997/03/19 12:21:06 barstow $
 */
/******************************************************************************

Copyright (c) 1993  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
******************************************************************************/

#include <X11/Intrinsic.h>
#include <X11/ICE/ICElib.h>
#include "SmXSMP.h"

/*
 * Forward declarations
 */
void _XtIceWatchProc ();
void _XtProcessIceMsgProc ();

Status
InitWatchProcs (
	XtAppContext 		appContext)
{
#ifdef DEBUG
    printf ("InitWatchProcs\n");
#endif /* DEBUG */

    return (IceAddConnectionWatch (_XtIceWatchProc, (IcePointer) appContext));
}


void
_XtIceWatchProc (
	IceConn			ice_conn,
	IcePointer		client_data,
	Bool			opening,
	IcePointer		*watch_data)
{
#ifdef DEBUG
    printf ("_XtIceWatchProc\n");
#endif /* DEBUG */

    if (opening)
    {
	XtAppContext appContext = (XtAppContext) client_data;

	*watch_data = (IcePointer) XtAppAddInput (
	    appContext,
	    IceConnectionNumber (ice_conn),
            (XtPointer) XtInputReadMask,
	    _XtProcessIceMsgProc,
	    (XtPointer) ice_conn);
    }
    else
    {
	XtRemoveInput ((XtInputId) *watch_data);
    }
}


void
_XtProcessIceMsgProc (
	XtPointer		client_data,
	int			*source,
	XtInputId		*id)
{
    IceConn			ice_conn = (IceConn) client_data;
    IceProcessMessagesStatus	status;

#ifdef DEBUG
    printf ("_XtProcessIceMsgProc\n");
#endif /* DEBUG */

    status = IceProcessMessages (ice_conn, NULL, NULL);

    if (status == IceProcessMessagesIOError)
    {
	ClientRecPtr		pClientRec;
	int			found = 0;

#ifdef DEBUG
	printf ("IO error on connection (fd = %d)\n", 
		IceConnectionNumber (ice_conn));
#endif /* DEBUG */

	for (pClientRec = connectedList; pClientRec != NULL; 
	  	pClientRec = pClientRec->next) 
	{
	    if (pClientRec->iceConn == ice_conn) 
	    {
	        CloseDownClient (pClientRec);
		found = 1;
	        break;
	    }
	}

	if (!found) {
            IceSetShutdownNegotiation (ice_conn, False);
            IceCloseConnection (ice_conn);
	}
    }
}
