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

/******************************************************************************
 ******************************************************************************
 **
 ** File:         records.c
 ** RCS:          $XConsortium: records.c /main/4 1996/05/28 13:34:16 cde-hp $
 **
 ** Description:
 **
 ** (c) Copyright 1995, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#define RECORDS_DOT_C

#include "dtpdmdP.h"


/********************************************************************
 *
 * Routines to BLOCK and UNBLOCK the signal SIGCLD.
 *
 * Use whenever modifying the array of client tracking
 * records (critical section).  If a SIGCLD happens,
 * dtpdmd.c:handle_SIGCLD() will be called, and it must
 * be able to work on a stable set of client tracking
 * records.
 */
static void block_SIGCLD( void )
{
    sigset_t newset;
    int      rtn;

    sigemptyset( &newset );
    sigaddset( &newset, SIGCHLD );
    rtn = sigprocmask( SIG_BLOCK, &newset, (sigset_t *) NULL );
}

static void unblock_SIGCLD( void )
{
    sigset_t newset;
    int      rtn;

    sigemptyset( &newset );
    sigaddset( &newset, SIGCHLD );
    rtn = sigprocmask( SIG_UNBLOCK, &newset, (sigset_t *) NULL );
}

/********************************************************************
 *
 * Try to find a service record based on ID.  Optionally create a
 * new service record if one is not found.
 */
XpPdmServiceRec *find_rec( Window requestor )
{
    int i;
    XpPdmServiceRec *r;


    if (!requestor)
	return( (XpPdmServiceRec *) NULL );

    /*
     * See if the record already exists.
     */
    for ( i=0; i < g.serviceRecNum; i++ ) {
	if ( g.serviceRecs[i]->requestor == requestor )
	    return( g.serviceRecs[i] );
    }

    /*
     * Will need to add - see if we need more room in the child
     * tracking record array.
     */
    block_SIGCLD();

    if ( g.serviceRecNum + 1 > g.maxServiceRecNum ) {
	g.maxServiceRecNum += 5;
	if ( g.maxServiceRecNum == 5 ) {
	    g.serviceRecs =
		 (XpPdmServiceRec **) Xmalloc( sizeof(XpPdmServiceRec *) *
						g.maxServiceRecNum );
	}
	else {
	    g.serviceRecs =
		 (XpPdmServiceRec **) Xrealloc( (char *) g.serviceRecs,
						sizeof(XpPdmServiceRec *) *
						g.maxServiceRecNum );
	}
    }

    /*
     * Create a new child tracking record and add to array.
     */
    r = (XpPdmServiceRec *) Xmalloc( sizeof(XpPdmServiceRec) );

    g.serviceRecs[g.serviceRecNum] = r;

    memset( (void *) r, 0, sizeof(XpPdmServiceRec) );    /* cheat NULLing */
    r->mgr_flag  = False;
    r->mbox_flag = False;

    r->message_pipe[0] = -1;
    r->message_pipe[1] = -1;

    g.serviceRecNum++;

    unblock_SIGCLD();

    return( r );
}

/********************************************************************
 *
 * Try to find a service record based on previously assigned
 * mailbox window ID.
 */
XpPdmServiceRec *find_rec_by_mbox_win( Window window )
{
    int i;
    XpPdmServiceRec *r;


    if (!window)
	return( (XpPdmServiceRec *) NULL );

    /*
     * See if the record already exists.
     */
    for ( i=0; i < g.serviceRecNum; i++ ) {
	if ( g.serviceRecs[i]->mbox_window == window )
	    return( g.serviceRecs[i] );
    }

    return( (XpPdmServiceRec *) NULL );
}


/********************************************************************
 *
 * Delete the specified service record from global memory.
 */
void delete_rec( XpPdmServiceRec *rec )
{
    int i,j;

    block_SIGCLD();

    for ( i=0; i< g.serviceRecNum; i++ ) {
	if ( g.serviceRecs[i] == rec ) {

	    /*
             * Delete memory for current rec
	     */
	    if (rec->mbox_window)
		XDestroyWindow( rec->selection_display, rec->mbox_window );

	    unlink( rec->auth_filename );

	    Xfree( g.serviceRecs[i]->video_display_str );
	    Xfree( g.serviceRecs[i]->print_display_str );
	    Xfree( g.serviceRecs[i]->locale_hint );

	    for ( j = 0; g.serviceRecs[i]->pdm_exec_argvs[j]; j++ )
		Xfree( (char *) g.serviceRecs[i]->pdm_exec_argvs[j] );
	    Xfree( (char *) g.serviceRecs[i]->pdm_exec_argvs );

	    Xfree( g.serviceRecs[i]->pdm_exec_errormessage );

	    Xfree( g.serviceRecs[i]->message_string );
	    Xfree( g.serviceRecs[i]->message_string2 );

	    Xfree( g.serviceRecs[i]->in_buf );
	    for ( j=i; j < g.serviceRecs[i]->cookie_cnt; j++ )
		Xfree( (char *) g.serviceRecs[i]->cookies[j] );
	    Xfree( (char *) g.serviceRecs[i]->cookies );

            Xfree( (char *) g.serviceRecs[i] );

	    /*
	     * Compress list around defunct entry
	     */
	    for ( j=i; j < g.serviceRecNum-1; j++ ) {
		g.serviceRecs[j] = g.serviceRecs[j+1];
	    }
	}
    }

    g.serviceRecNum--;

    unblock_SIGCLD();
}

