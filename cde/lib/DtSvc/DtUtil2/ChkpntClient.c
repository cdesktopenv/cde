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
* File:         ChkpntClient.c
* Description:  CDE client-side checkpoint protocol functions. Private API
*		functions for use by the CDE client program.
* Created:      Mon Sep  6 09:00:00 1993
* Language:     C
*
* $TOG: ChkpntClient.c /main/7 1998/04/09 17:49:06 mgreess $
*
* (C) Copyright 1993, Hewlett-Packard, all rights reserved.
*
*******************************************************************************
*/

#define NUMPROPERTIES 8
#define INVALID_TIME	((Time) -1)

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "Dt/ChkpntP.h"
#include "DtSvcLock.h"

static struct {
    Display	*display;		/* Display pointer 		*/
    Window      window;			/* Window id for the program   	*/
    char        *pname;			/* Actual name of the program  	*/
    Atom        aSelection;		/* Atom  for root selection 	*/
    Atom        *aProperty;		/* Atom array for window props 	*/
    int         maxprops;		/* Size of above array		*/
    Atom	aChkpntMsg;		/* Atom  for Checkpoint message	*/
    Boolean     bChkpnt;		/* Should I do checkpointing ? 	*/
}                     dtcp_info;	/* Data structure for this client*/
static DtChkpntMsg    dtcp_msg;		/* Message structure union	*/

/*
 * myCheckClientEvent --- Helper Boolean function to pass to XCheckIfEvent()
 * Checks for PropertyNotify & SelectionNotify events in the event queue.
 */
static Bool myCheckClientEvent(Display *display, XEvent *event, char *args)
{
    Boolean onMyWindow;

    /* Only check the client window events */
    _DtSvcProcessLock();
    onMyWindow = (event->xany.window == dtcp_info.window);
    _DtSvcProcessUnlock();

    if (!onMyWindow)
	return(False);

    switch(event->type)
    {
	case PropertyNotify:
	case SelectionNotify:
	    return(True);
	    break;

	default:
	    break;
    }

    return (False);
}

/*
 * myDtChkpntMsgSend --- Helper function: Send a checkpoint message to the listener
 */
static int
myDtChkpntMsgSend(char *message, char *type)
{
    static long     msgcount = 0;  /* Running count of messages */
    static int      propnum  = 0;  /* Which property are we using ? */
    static Time     oldtime  = INVALID_TIME; /* Recycled from old PropertyNotify events */
    Time            timestamp= INVALID_TIME;
    char            buf_msgcount[32];
    char            buf_seconds[128];
    struct timeval  time_val;
    struct timezone time_zone;
    XTextProperty   tp;
    Status          status;
    Bool            bool;
    XEvent          event;

    /* Check to see if checkpoint is actually on */
    _DtSvcProcessLock();
    if (dtcp_info.bChkpnt == False) {
	_DtSvcProcessUnlock();
	return(0);
    }

    /* Fill the message list. ("pname" and "window" were filled at init) */
    dtcp_msg.record.type = type;
    sprintf(buf_msgcount, "%ld", msgcount);
    dtcp_msg.record.count   = buf_msgcount;	/* Running message count */
    gettimeofday(&time_val, &time_zone);
    sprintf(buf_seconds,"%lf",time_val.tv_sec + (time_val.tv_usec/1000000.0 ));
    dtcp_msg.record.seconds = buf_seconds;	/* Info from gettimeofday()*/
    dtcp_msg.record.message = message;		/* Actual message string */

    /*
     * We maintain a list of properties and use them round robin -- hoping to
     * never run out.
     * The listener should then track the message count to see if messages are
     * getting dropped.
     */

    /* Fill the window property with necessary information */
    status = XStringListToTextProperty(dtcp_msg.array,DT_PERF_CHKPNT_MSG_SIZE, &tp);

    /* Hang the property on the window */
    if ( !( (status == Success) || (status > 0) )) {
	_DtSvcProcessUnlock();
	return(0);
    }

    XSetTextProperty(dtcp_info.display, dtcp_info.window,
			&tp,             dtcp_info.aProperty[propnum]);
    XFree(tp.value);

    if (oldtime != INVALID_TIME) {  /* Valid timestamp to be recycled */
	timestamp = oldtime;
    }
    else {		     /* Check event queue */
	bool = XCheckIfEvent(dtcp_info.display, &event,
			 myCheckClientEvent, NULL);
	if (bool == True) {
	    if (event.type == PropertyNotify)
		 timestamp = event.xproperty.time; 
	    else timestamp = event.xselection.time;
	    }
	else { 	   /* Synthesize time by generating a PropertyNotify */
	    XChangeProperty(dtcp_info.display,     dtcp_info.window,
		    dtcp_info.aProperty[propnum],   XA_STRING,
		    8,                      PropModeAppend,
		    (unsigned char *) NULL, 0);
	    XFlush(dtcp_info.display);
	    loop:
	    XWindowEvent(dtcp_info.display, dtcp_info.window,
			PropertyChangeMask,  &event);
	    if (event.type == PropertyNotify) {
		timestamp = event.xproperty.time;
	    }
	    else goto loop;
	}
    }
    /*
     * Send the checkpoint message: do a ConvertSelection()
     */

    /* Note: Currently listener makes no use of the "aChkpntMsg" info */
    XConvertSelection(dtcp_info.display,dtcp_info.aSelection,
		  dtcp_info.aChkpntMsg, dtcp_info.aProperty[propnum],
		  dtcp_info.window,     timestamp);
    XFlush(dtcp_info.display);

    /*
     * Toss SelectionNotify and PropertyNotify events from the event queue
     */
    oldtime = INVALID_TIME;
    do {
	bool = XCheckIfEvent(dtcp_info.display, &event,
			     myCheckClientEvent, NULL);
	if (event.type == PropertyNotify) /* Save timestamp for recycling */
	     oldtime = event.xproperty.time;
	else oldtime = event.xselection.time;
    } while(bool == True);

    /*
     * Increment the property and message counters
     */
    if (++propnum >= dtcp_info.maxprops)
	propnum = 0;
    msgcount++;

    _DtSvcProcessUnlock();
    return(1);
}

/*
 * _DtPerfChkpntInit --- Initialize the checkpointing mechanism
 */
int
_DtPerfChkpntInit(Display      *display,
	     Window       parentwin,
	     char         *prog_name,
	     Boolean      bChkpnt)
{
    static        char winstring[32];	/* Storage for window id */
    Window        tmpwin;
    char          propname[80];	/* Temp buffer for property name */
    Display       *tmpdisplay;
    int    i;

    /*
     * Fill the dtcp_info structure 
     */
    _DtSvcProcessLock();
    dtcp_info.display    = display;
    dtcp_info.pname      = prog_name;
    dtcp_info.bChkpnt    = bChkpnt;
    dtcp_info.aChkpntMsg = XA_STRING;

    /* Pre-compute Atom names and save them away in the dtcp_info structure */
    dtcp_info.aSelection = XInternAtom(dtcp_info.display, 
					DT_PERF_CHKPNT_SEL,     False);
    dtcp_info.maxprops   = NUMPROPERTIES;
    dtcp_info.aProperty  = (Atom *) malloc(dtcp_info.maxprops * sizeof(Atom));
    for (i= 0; i < dtcp_info.maxprops; i++) {
	sprintf(propname, "%s_%x", DT_PERF_CLIENT_CHKPNT_PROP, i);
	dtcp_info.aProperty[i]  = XInternAtom(dtcp_info.display,
					    propname, False);
    }

    /*
     * Check to see if listener is available
     */
    tmpwin  = XGetSelectionOwner(dtcp_info.display, dtcp_info.aSelection);
    if (tmpwin == None) {	/* No listener */
	dtcp_info.bChkpnt   = False;
	_DtSvcProcessUnlock();
	return(0);
    }

    /*
     * Create a permanent window for hanging messages on
     */
    tmpdisplay = display;
    tmpdisplay = XOpenDisplay("");	/* Temporary display connection */
    XSetCloseDownMode(tmpdisplay, RetainPermanent);
    dtcp_info.window     = XCreateSimpleWindow(tmpdisplay, parentwin, 
			    1, 1, 100, 100, 1,
			    BlackPixel(display,DefaultScreen(display)),
			    WhitePixel(display,DefaultScreen(display)));
    {	/* Hang a name on the permanent window => helps debugging */
	char *buffer;
	char *array[2];
	XTextProperty text_prop;

	buffer = malloc(strlen(prog_name) + 8);
	sprintf(buffer, "DtPerf %s", prog_name);
	array[0] = buffer;
	array[1] = "";
	XStringListToTextProperty(array, 1, &text_prop);
	XSetWMName(tmpdisplay, dtcp_info.window, &text_prop);
	XFree(text_prop.value);
	if (buffer) free(buffer);
    }

    XCloseDisplay(tmpdisplay);

    /*
     * Pre-fill the message structure entries for "pname" and "window"
     */
    dtcp_msg.record.pname  = prog_name;
    sprintf(winstring, "%lx", (long) dtcp_info.window);
    dtcp_msg.record.window = winstring;

    /*
     * Express interest in Property change events
     */
    XSelectInput(dtcp_info.display, dtcp_info.window, PropertyChangeMask);

    /* Inform listener that you are ready to send messages */
    myDtChkpntMsgSend("Begin checkpoint delivery", DT_PERF_CHKPNT_MSG_INIT);
    _DtSvcProcessUnlock();
    return(1);
} 	/* DtChkpntInit() */


/*
 * _DtPerfChkpntMsgSend --- Send a checkpoint message to the listener
 */
void
_DtPerfChkpntMsgSend(char *message) 
{
    myDtChkpntMsgSend(message, DT_PERF_CHKPNT_MSG_CHKPNT);
}

/*
 * myDtPerfChkpntEnd --- End the checkpointing message delivery
 */
int
_DtPerfChkpntEnd() 
{
    myDtChkpntMsgSend("End checkpoint delivery", DT_PERF_CHKPNT_MSG_END);
    return(1);
}

