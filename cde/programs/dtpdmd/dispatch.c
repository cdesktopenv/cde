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
 ** File:         dispatch.c
 ** RCS:          $XConsortium: dispatch.c /main/4 1996/05/28 13:33:53 cde-hp $
 **
 ** Description:
 **
 ** (c) Copyright 1995, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#define DISPATCH_DOT_C

#include "dtpdmdP.h"


/******************************************************************************
 *
 * Dispatch PDM_MANAGER requests
 *
 * note: 'rec->pdm_exec_errorcode' is set to 0 (zero) and
 *       stays that way as long as no errors occur.
 *       Ultimately it gets set to either and error or
 *       success code.
 */
void dispatch_mgr( XEvent *report )
{
    XpPdmServiceRec *rec;


    /*
     * Locate or create a client record for this request.
     */
    rec = find_rec( report->xselectionrequest.requestor );

    /*
     * Setup the client record per the request.
     */
    mgr_initialize( report, rec );

    if ( rec->pdm_exec_errorcode ) {
	/*
	 * Reply to requestor, but with bad news that
	 * basic initialization in prep to launch a
	 * pdm could not be completed.
	 */
	mgr_launch_reply( rec );

	delete_rec( rec );
	return;
    }

    /*
     * Locate the pdm to be launched.
     */
    mgr_fetch_pdm( rec );

    if ( rec->pdm_exec_errorcode ) {
	/*
	 * Reply to requestor, but with bad news that
	 * selection of a pdm to launch could not be
	 * completed.
	 */
	mgr_launch_reply( rec );

	delete_rec( rec );
	return;
    }

    /*
     * Launch the pdm.
     */
    mgr_launch_pdm( rec );

    if ( rec->pdm_exec_errorcode ) {
	/*
	 * Reply to requestor, but with bad news that
	 * the pdm could not be launched.
	 */
	mgr_launch_reply( rec );

	delete_rec( rec );
	return;
    }

    /*
     * Set state flags indicating a pdm has been started, but
     * defer the calling of 'mgr_launch_reply' until after the
     * pdm itself tells us it is up and running.
     *
     * The pdm can generate stderr which we will trap, and can
     * generate a SIGCLD which we will also trap and take
     * significant on.
     */
    rec->pdm_exec_errorcode = g.pdm_start_ok;
    rec->do_launch_reply = True;
    /* sometime later someone else calls mgr_launch_reply( rec ) */

    return;
}

/******************************************************************************
 *
 * Dispatch MBOX and MAIL requests
 */
void dispatch_mbox( XEvent *report )
{
    XpPdmServiceRec *rec;


    if (report->type == SelectionRequest) {
	/*
	 * Locate or create a client record for this request.
	 */
	rec = find_rec( report->xselectionrequest.requestor );

        mbox_initialize( report, rec );

        mbox_build( rec );

        mbox_reply( rec );
    }
    else /* if (report->type == ClientMessage) */ {
	/*
	 * Locate (do not create) a client record for this request.
	 */
	rec = find_rec_by_mbox_win( report->xclient.window );

	mbox_receive( rec, report );
    }

    return;
}

/******************************************************************************
 *
 * Dispatch TARGETS requests
 */
void dispatch_targets( XEvent *report )
{
    Atom   *target_list;
    int    target_count;
    XEvent reply;
    Status status;


    target_count = 4;

    target_list = (Atom *) Xmalloc( sizeof(Atom) * target_count );

    target_list[0] = g.pdm_targets;
    target_list[1] = g.pdm_start;
    target_list[2] = g.pdm_timestamp;
    target_list[3] = g.pdm_mbox;

    /* target_list[4] = g.pdm_multiple; */

    XChangeProperty( report->xselectionrequest.display,
		     report->xselectionrequest.requestor,
		     report->xselectionrequest.property,
		     XA_ATOM,
		     32,
		     PropModeReplace,
		     (unsigned char *) target_list,
		     target_count );

    reply.xselection.type      = SelectionNotify;
    reply.xselection.requestor = report->xselectionrequest.requestor;
    reply.xselection.selection = report->xselectionrequest.selection;
    reply.xselection.target    = report->xselectionrequest.target;
    reply.xselection.property  = report->xselectionrequest.property;
    reply.xselection.time      = report->xselectionrequest.time;

    status = XSendEvent( report->xselectionrequest.display,
			 report->xselectionrequest.requestor,
			 True, 0, &reply );
}


/******************************************************************************
 *
 * Dispatch MULTIPLE requests
 */
void dispatch_multiple( XEvent *report )
{
}


/******************************************************************************
 *
 * Dispatch TIMESTAMP requests
 */
void dispatch_timestamp( XEvent *report )
{
    XEvent reply;
    Status status;


    XChangeProperty( report->xselectionrequest.display,
		     report->xselectionrequest.requestor,
		     report->xselectionrequest.property,
		     XA_INTEGER,
		     32,
		     PropModeReplace,
		     (unsigned char *) &g.time,
		     1 );

    reply.xselection.type      = SelectionNotify;
    reply.xselection.requestor = report->xselectionrequest.requestor;
    reply.xselection.selection = report->xselectionrequest.selection;
    reply.xselection.target    = report->xselectionrequest.target;
    reply.xselection.property  = report->xselectionrequest.property;
    reply.xselection.time      = report->xselectionrequest.time;

    status = XSendEvent( report->xselectionrequest.display,
			 report->xselectionrequest.requestor,
			 True, 0, &reply );
}

/******************************************************************************
 *
 * Dispatch unsupported selection requests
 */
void dispatch_not_supported( XEvent *report )
{
    XEvent reply;
    Status status;

    reply.xselection.type      = SelectionNotify;
    reply.xselection.requestor = report->xselectionrequest.requestor;
    reply.xselection.selection = report->xselectionrequest.selection;
    reply.xselection.target    = report->xselectionrequest.target;
    reply.xselection.property  = None;
    reply.xselection.time      = report->xselectionrequest.time;

    status = XSendEvent( report->xselectionrequest.display,
			 report->xselectionrequest.requestor,
			 True, 0, &reply );
}

