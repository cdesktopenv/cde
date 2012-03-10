#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermPrimWMProtocols.c /main/1 1996/04/21 19:20:04 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */
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
