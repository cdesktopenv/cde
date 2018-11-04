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
/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include <Xm/Xm.h>
#include <Xm/Protocols.h>

static Atom xa_WM_DELETE_WINDOW;
static Atom xa_WM_SAVE_YOURSELF;
static Boolean initialized = False;

static void
protocolsInitialize(Widget topLevel)
{
    if (!initialized) {
	xa_WM_DELETE_WINDOW = XInternAtom(XtDisplay(topLevel),
		"WM_DELETE_WINDOW", False);
	xa_WM_SAVE_YOURSELF = XInternAtom(XtDisplay(topLevel),
		"WM_SAVE_YOURSELF", False);
	initialized = True;
    }
}

void
_DtTermPrimAddDeleteWindowCallback(Widget topLevel, XtCallbackProc callback,
	XtPointer client_data)
{
    /* initialize things... */
    _DtTermProcessLock();
    if (!initialized)
	(void) protocolsInitialize(topLevel);
    _DtTermProcessUnlock();
    (void) XmAddWMProtocols(topLevel, &xa_WM_DELETE_WINDOW, 1);
    (void) XmAddWMProtocolCallback(topLevel, xa_WM_DELETE_WINDOW, callback,
	    client_data);
}

void
_DtTermPrimAddSaveYourselfCallback(Widget topLevel, XtCallbackProc callback,
	XtPointer client_data)
{
    /* initialize things... */
    _DtTermProcessLock();
    if (!initialized)
	(void) protocolsInitialize(topLevel);
    _DtTermProcessUnlock();
    (void) XmAddWMProtocols(topLevel, &xa_WM_SAVE_YOURSELF, 1);
    (void) XmAddWMProtocolCallback(topLevel, xa_WM_SAVE_YOURSELF, callback,
	    client_data);
}
