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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/* -*-C-*-
*******************************************************************************
*
* File:         ChkpntListen.c
* Description:  CDE listener-side checkpoint protocol functions. Internal API 
*               functions for use by the CDE checkpoint listener.
* Created:      Mon Sep  6 09:00:00 1993
* Language:     C
*
* $XConsortium: ChkpntListen.c /main/6 1996/08/28 15:14:40 drk $
*
* (C) Copyright 1993, Hewlett-Packard, all rights reserved.
*
*******************************************************************************
*/

#define INVALID_TIME	((Time) -1)

#include <stdio.h>
#include <string.h>
#include "Dt/ChkpntP.h"
#include "DtSvcLock.h"

static struct {
    Display	*display;		/* Display pointer 		*/
    Window      window;			/* Window id for the program    */
    Atom        aSelection;		/* Atom  for root selection 	*/
    Boolean     bListen;		/* Should I do listening ?      */
} dtsvc_info;

/* _DtPerfChkpntListenInit(): Start the Checkpoint listener */
int
#ifdef _NOPROTO
_DtPerfChkpntListenInit(display, parentwin)
Display *display;	/* Current display */
Window  parentwin;	/* Parent of window associated with listener */
#else
_DtPerfChkpntListenInit(Display *display, Window parentwin)
#endif
{
    Time   timestamp = INVALID_TIME;
    Window tmpwin;
    Bool   bsuccess = False;

    _DtSvcProcessLock();
    dtsvc_info.display    = display;
    dtsvc_info.window     = XCreateSimpleWindow(display, parentwin,
			      1, 1, 100, 100, 1,
			      BlackPixel(display,DefaultScreen(display)),
			      WhitePixel(display,DefaultScreen(display)));
    dtsvc_info.bListen    = True;
    /*
     * Assert ownership over the appropriate root window selection
     */
    dtsvc_info.aSelection = XInternAtom(display, DT_PERF_CHKPNT_SEL, False);
    if (XGetSelectionOwner(dtsvc_info.display, dtsvc_info.aSelection) == None) {
	if (timestamp == INVALID_TIME) { /* Generate a valid timestamp */
	    XEvent event;
	    Atom aProperty;
	    char   propname[80];	/* Temp buffer for property name */

	    sprintf(propname, "%s_%x", DT_PERF_CLIENT_CHKPNT_PROP, 0);
	    aProperty  = XInternAtom(dtsvc_info.display, propname,False);
	    XSelectInput(dtsvc_info.display,
				    dtsvc_info.window, PropertyChangeMask);

	    XChangeProperty(dtsvc_info.display,  dtsvc_info.window,
				    aProperty, XA_STRING, 8,
				    PropModeAppend, (unsigned char *) NULL, 0);
	    XFlush(dtsvc_info.display);
	    loop:
	    XWindowEvent(dtsvc_info.display, dtsvc_info.window,
				    PropertyChangeMask,  &event);
	    if (event.type == PropertyNotify) {
		timestamp = event.xproperty.time;
	    }
	    else goto loop;
	}
	XSetSelectionOwner(dtsvc_info.display, dtsvc_info.aSelection,
			   dtsvc_info.window,  timestamp);
	tmpwin = XGetSelectionOwner(dtsvc_info.display, dtsvc_info.aSelection);
	if ( tmpwin == dtsvc_info.window)
		bsuccess = True;/* We are now the listener! */
    }

    if (bsuccess == False) {
	fprintf(stderr,
	  "\t** Chkpnt listener: Cannot gain ownership of root selection **\n");
	fprintf(stderr,
	  "\t** Chkpnt listener: Selection is owned by window: %lx **\n",
	  (long) tmpwin);
	dtsvc_info.bListen = False;
	_DtSvcProcessUnlock();
	return(0);
    }

    /* Express interests in Events on this window */
    XSelectInput(dtsvc_info.display, dtsvc_info.window,
		 SelectionRequest | SelectionClear);

    _DtSvcProcessUnlock();
    return(1);
}

/* Helper Boolean function to pass to XCheckIfEvent() */
static Bool myCheckSelectionEvent(Display *display, XEvent *event, char *args)
{
    _DtSvcProcessLock();

    if (event->xany.window != dtsvc_info.window) { /* Only listener window events*/
	_DtSvcProcessUnlock();
	return(False);
    }
    else switch(event->type) {		        /* Selection stuff ? */
	case SelectionClear:
	case SelectionRequest:
	    _DtSvcProcessUnlock();
	    return (True);
	    break;
	default:
	    _DtSvcProcessUnlock();
	    return (False);
	    break;
    }
}

/*
 * _DtPerfChkpntMsgReceive() Non blocking fetch from message queue
 */
Bool _DtPerfChkpntMsgReceive(DtChkpntMsg *dtcp_msg, Bool bBlock)
{
    XEvent        event;
    Bool          bool=True;
    XTextProperty tp;
    int 	  i;
    static char   **Stringlist;
    static int    numfields = 0;

    _DtSvcProcessLock();
    if (dtsvc_info.bListen == False) {
	_DtSvcProcessUnlock();
	return(False);
    }

    if (numfields) {	/* Free the storage from last time around */
	XFreeStringList(Stringlist);
	numfields = 0;
    }

    if (bBlock == True)
	XIfEvent(dtsvc_info.display, &event, myCheckSelectionEvent, NULL);
    else
	bool = XCheckIfEvent(dtsvc_info.display, &event,
				myCheckSelectionEvent, NULL);
    
    if (bool == True) {
	switch (event.type) {
	    case SelectionRequest:	/* Message received from a client */
		/* Is this a Checkpoint request ?*/
		if (event.xselectionrequest.selection == dtsvc_info.aSelection){
		    /* Correct selection, now fetch the property */
		    /*
		     * Note: Need to handle errors if the client is dead
		     * and the property no longer exists.
		     */
		    XGetTextProperty(dtsvc_info.display,
				     event.xselectionrequest.requestor,
				     &tp,
				     event.xselectionrequest.property);
		    XDeleteProperty (dtsvc_info.display, 
				     event.xselectionrequest.requestor,
				     event.xselectionrequest.property);
		    XTextPropertyToStringList(&tp, &Stringlist, &numfields);
		    XFree(tp.value);
		    for (i = 0; i < DT_PERF_CHKPNT_MSG_SIZE; i++)
			dtcp_msg->array[i] = (Stringlist)[i];

		    /* End? Destroy the, RetainPermanent client window */
		    if (!strcmp(dtcp_msg->record.type, DT_PERF_CHKPNT_MSG_END))
			XDestroyWindow(dtsvc_info.display, 
				       event.xselectionrequest.requestor);
		}
		break;
	    /* */
	    case SelectionClear:	/* We no longer own the selection */
	    default:
		fprintf(stderr,"\t** Chkpnt listener: Warning - loss of Selection ownership **\n");
		bool = False;
		break;
	}
    }
    _DtSvcProcessUnlock();
    return(bool);
}
