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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: xtclient.c /main/1 1996/04/21 19:24:58 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <rpc/rpc.h>
#include "debug.h"

typedef struct _appctlist {
	XtAppContext	appct;
	struct _appctlist *next;
} AppCtList;

static AppCtList *registered_appct = NULL;

/*****************************************************************************
 * forward declaration of static functions
 *****************************************************************************/
static void xtcallback(XtPointer data, int *fid, XtInputId *id);
static boolean_t new_appct(XtAppContext apptct);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * register callback for all file descriptors that's set
 * (since we don't know which one is ours).
 */
extern void
_DtCm_register_xtcallback(XtAppContext appct)
{
	XtInputId	id;
	int	i;
	fd_set	fdset = svc_fdset;

	DP(("xtclient.c: _DtCm_register_xtcallback()\n"));

	if (new_appct(appct) == B_FALSE)
		return;

	/* assuming only 1 bit is set */

	for (i = 0; i < FD_SETSIZE; i++) {
          if (FD_ISSET(i, &svc_fdset))
            {
              /* register callback with XtAppAddInput
               * for rpc input
               */
              id = XtAppAddInput(appct, i,
                                 (XtPointer)XtInputReadMask,
                                 xtcallback, NULL);

              DP(("xtclient.c: id %d for input at fd %d\n",
                  id, i));
            }
        }
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

/*
 * callback for rpc events
 */
static void
xtcallback(XtPointer data, int *fid, XtInputId *id)
{
	fd_set rpc_bits;

	DP(("xtcallback called\n"));

	FD_ZERO(&rpc_bits);
	FD_SET(*fid, &rpc_bits);
	svc_getreqset(&rpc_bits);
}

/*
 * need to lock registered_appct
 */
static boolean_t
new_appct(XtAppContext appct)
{
	AppCtList	*lptr;
	boolean_t	newappct = B_TRUE;

	for (lptr = registered_appct; lptr != NULL; lptr = lptr->next) {
		if (lptr->appct == appct) {
			newappct = B_FALSE;
			break;
		}
	}

	if (newappct == B_TRUE) {
		lptr = (AppCtList *)calloc(1, sizeof(AppCtList));
		lptr->appct = appct;
		lptr->next = registered_appct;
		registered_appct = lptr;
	}

	return (newappct);
}

