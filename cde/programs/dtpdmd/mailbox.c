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
/******************************************************************************
 ******************************************************************************
 **
 ** File:         mailbox.c
 ** RCS:          $XConsortium: mailbox.c /main/2 1996/07/18 15:26:34 drk $
 **
 ** Description:
 **
 ** (c) Copyright 1995, 1996, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#define MAILBOX_DOT_C

#include "dtpdmdP.h"
#include <setjmp.h>
#include <locale.h>
#include <unistd.h>


/******************************************************************************
 *
 * Setup a child service record per the selection request.
 */
void mbox_initialize( XEvent *report, XpPdmServiceRec *rec )
{
    Display *testdpy;
    char    buf[1024];

    Display *selection_display;
    Window   requestor;
    Atom     prop_atom;
    unsigned long tafter;

    XTextProperty  text_prop;
    char           **list;
    int            list_cnt;

    rec->selection_display  = report->xselectionrequest.display;
    rec->requestor          = report->xselectionrequest.requestor;
    rec->prop_atom          = report->xselectionrequest.property;
    rec->selection          = report->xselectionrequest.selection;
    rec->time               = report->xselectionrequest.time;

    rec->mbox_flag          = False;	/* still need to complete mbox */
}

/******************************************************************************
 *
 * mbox_build()
 *
 * Build a window per requestor to serve as a mailbox for
 * that requestor.
 *
 * note: a possible optimization is to have a window or two cached
 * that could quickly be assigned as a mailbox.  When all the mail
 * is sent, the window could be released back to the cache.
 */
void mbox_build( XpPdmServiceRec *rec )
{
    int tscreen;

    tscreen = DefaultScreen( rec->selection_display );

    rec->mbox_window = XCreateSimpleWindow( rec->selection_display,
                                  DefaultRootWindow( rec->selection_display ),
                                  0, 0, 1, 1, 1,
                                  BlackPixel(rec->selection_display, tscreen),
                                  WhitePixel(rec->selection_display, tscreen) );
}


/******************************************************************************
 *
 * mbox_reply()
 *
 * Reply to the SelectionRequest.
 */
void mbox_reply( XpPdmServiceRec *rec )
{
    XEvent reply;
    Status status;
    FILE   *errlog;
    long   now;

    Atom    tmpa;


    XChangeProperty( rec->selection_display, rec->requestor,
		     rec->prop_atom, XA_WINDOW,
		     32, PropModeReplace,
		     (unsigned char *) &(rec->mbox_window),
		     1 );

    /*
     * Send a SelectionNotify event, which will conclude the
     * selection handshake.
     */
    reply.xselection.type      = SelectionNotify;
    reply.xselection.requestor = rec->requestor;
    reply.xselection.selection = rec->selection;
    reply.xselection.target    = g.pdm_mbox;
    reply.xselection.property  = rec->prop_atom;
    reply.xselection.time      = rec->time;

    status = XSendEvent( rec->selection_display, rec->requestor, True, 0, &reply );
}


/******************************************************************************
 *
 * mbox_receive()
 *
 * Pickup the mail pieces that come in, buffer until a complete
 * cookie comes in, convert to Xauth format, and eventually mark
 * the mailbox done when the last cookie is fully received.
 */
void mbox_receive( XpPdmServiceRec *rec, XEvent *report )
{
    XClientMessageEvent *cme;
    Xauth *c;			/* shorthand pointer */
    int maxgrab;
    char *tptr;

    cme = (XClientMessageEvent *) report;

    if ( cme -> format == 16 ) {
	/*
	 * Only the cookie header packet would be format 16.
	 */
        rec->cookie_state = cme->data.s[0];

	if (rec->cookie_state == 0) {
	    /*
	     * Terminating cookie.
	     */
	    rec->mbox_flag = True;		/* all done */
	    XDestroyWindow( rec->selection_display, rec->mbox_window );
	    rec->mbox_window = (Window) NULL;
	    return;
	}

	if ( rec->cookie_cnt ) {
	    rec->cookie_cnt++;
	    rec->cookies = (Xauth **) Xrealloc( (char *) rec->cookies,
				           sizeof(Xauth *) * rec->cookie_cnt );
	}
	else {
	    rec->cookie_cnt++;
	    rec->cookies = (Xauth **) Xmalloc( sizeof(Xauth *) );
	}

	rec->cookies[rec->cookie_cnt-1] = (Xauth *) Xmalloc( sizeof(Xauth) );

	c = rec->cookies[rec->cookie_cnt-1];

	c->address_length = (unsigned short) cme->data.s[1];
	c->number_length  = (unsigned short) cme->data.s[2];
	c->name_length    = (unsigned short) cme->data.s[3];
	c->data_length    = (unsigned short) cme->data.s[4];
	c->family         = (unsigned short) cme->data.s[5];

        c->address = Xmalloc( c->address_length );
        c->number  = Xmalloc( c->number_length );
        c->name    = Xmalloc( c->name_length );
        c->data    = Xmalloc( c->data_length );

	rec->in_sofar = 0;
	rec->in_expected = c->address_length + c->number_length +
			   c->name_length + c->data_length;
	rec->in_buf = (char *) Xmalloc( rec->in_expected );

    }
    else if ( cme -> format == 8 ) {
	/*
	 * A cookie crumb has come in.
	 */
	c = rec->cookies[rec->cookie_cnt-1];

	maxgrab = rec->in_expected - rec->in_sofar;
	if ( maxgrab > 20 ) maxgrab = 20;

	memcpy( (char *) &(rec->in_buf[rec->in_sofar]), cme->data.b, maxgrab );

	rec->in_sofar += maxgrab;

	if ( rec->in_sofar == rec->in_expected ) {
	    tptr = rec->in_buf;

	    memcpy( c->address, tptr, c->address_length );
	    tptr += c->address_length;

	    memcpy( c->number, tptr, c->number_length);
	    tptr += c->number_length;

	    memcpy( c->name, tptr, c->name_length );
	    tptr += c->name_length;

	    memcpy( c->data, tptr, c->data_length );

	    Xfree( rec->in_buf ); rec->in_buf = (char *) NULL;

	    if (rec->cookie_state == 1) {
		rec->mbox_flag = True;		/* all done */
		XDestroyWindow( rec->selection_display, rec->mbox_window );
		rec->mbox_window = (Window) NULL;
		return;
	    }
	}
    }
}
