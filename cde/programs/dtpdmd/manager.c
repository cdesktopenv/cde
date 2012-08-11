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
 ** File:         manager.c
 ** RCS:          $XConsortium: manager.c /main/8 1996/10/30 19:10:15 cde-hp $
 **
 ** Description:
 **
 ** (c) Copyright 1995, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#define MANAGER_DOT_C

#define OPTIONAL_PXAUTH_PRETEST 1

#include "dtpdmdP.h"
#include "nlmsg.h"
#include <locale.h>
#include <unistd.h>



/******************************************************************************
 ******************************************************************************
 *
 * The following PDM_MANAGER : PDM_START routines are registered
 * per child, and dispatched out of XtAppNextEvent
 */

/********************************************************************
 *
 * message_pipe_handler()
 *
 * Takes stderr from the child, via a pipe, and stores the output
 * in the client tracking record.
 * 
 */
static void message_pipe_handler( XtPointer w, int *source, XtInputId *id)
{
    int    i, inc;
    struct stat statBuffer;
    int    delt_with;
    int    n, keepon;

    /*
     * Find out who is generating output.
     */
    delt_with = False;

    for ( i = 0; i < g.serviceRecNum; i++ ) {
	if ( g.serviceRecs[i]->message_pipe[0] == *source ) {
	    delt_with = True;
	    /*
	     * Fetch size and grow message_string to hold more.
	     */
	    if ( fstat(*source, &statBuffer) ) {
		/* unable to get size */
		statBuffer.st_size = 0;		/* bail out below */
	    }

	    if ( statBuffer.st_size > 0 ) {
		if ( g.serviceRecs[i]->message_string ) {
		    inc = strlen( g.serviceRecs[i]->message_string );

		    g.serviceRecs[i]->message_string = Xrealloc(
				(char *) g.serviceRecs[i]->message_string,
				statBuffer.st_size + inc + 1 );
		}
		else {
		    inc = 0;

		    g.serviceRecs[i]->message_string = Xmalloc(
				statBuffer.st_size + 1 );
		}

		/*
		 * Read off what we know is there.
		 */
		keepon = True;
		while (keepon) {
		    n = read( *source, &(g.serviceRecs[i]->message_string[inc]),
				statBuffer.st_size );

		    if ( n == statBuffer.st_size ) {
			/* read all there is (per the previous stat) */
			keepon = False;
		    }
		    else if (( n == -1 ) && ( errno == EINTR )) {
			/* an interrupt came in before the read could start */
			keepon = True;
		    }
		    else if (( n == -1 ) || ( n == 0 )) {
			/* problems - bail out */
			g.serviceRecs[i]->message_pipe[0] = -1;
			close(*source);
			XtRemoveInput(*id);
			keepon = False;
		    }
		    else {
			/* only a partial read, probably a sig, try for more */
			inc += n;
			statBuffer.st_size -= n;
			keepon = True;
		    }
		}

		/*
		 * NULL terminate what we have so far to make it look
		 * like a string.
		 */
		g.serviceRecs[i]->message_string[statBuffer.st_size+inc] = '\0';
	    }
	    else {
		/*
		 * No more to read - this really means the pipe is
		 * going down.
		 */
		g.serviceRecs[i]->message_pipe[0] = -1;
		close (*source);
		XtRemoveInput(*id);
	    }
	}
    }

    if (!delt_with) {
	/*
	 * Some stray pipe that we no longer have information on.
	 */
	close(*source);
	XtRemoveInput(*id);
    }

    /*
     * Scan client records and see who is ready to be
     * shut down.
     */
    mgr_shutdown_scan();
}


/******************************************************************************
 ******************************************************************************
 *
 * The following PDM_MANAGER : PDM_START routines are called by
 * the dispatch routines to service selection requests and
 * associated events.
 */

/********************************************************************
 *
 * Setup a child service record per the selection request.
 */
void mgr_initialize( XEvent *report, XpPdmServiceRec *rec )
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

    /*
     * Grab the PDM_CLIENT_PROP from which all the juicy
     * information is retrieved.
     */
    selection_display = report->xselectionrequest.display;
    requestor = report->xselectionrequest.requestor;
    prop_atom = report->xselectionrequest.property;

    if ( XGetWindowProperty( selection_display, requestor, prop_atom,
			0, 100000, True, AnyPropertyType,
                        &text_prop.encoding,
			&text_prop.format,
			&text_prop.nitems,
			&tafter,
                        &text_prop.value ) != Success ) {
	/*
	 * Error
	 */
	rec->pdm_exec_errorcode = g.pdm_start_error;

	sprintf( buf, PDMD_MSG_5, g.prog_name );
	rec->pdm_exec_errormessage = xpstrdup( buf );

	return;
    }

    if ( text_prop.format != 8 ) {
	/*
	 * Error
	 */
	rec->pdm_exec_errorcode = g.pdm_start_error;

	sprintf( buf, PDMD_MSG_6, g.prog_name );
	rec->pdm_exec_errormessage = xpstrdup( buf );

	return;
    }

    if ( XmbTextPropertyToTextList( selection_display, &text_prop,
				     &list, &list_cnt ) < 0 ) {
	/*
	 * Error
	 */
	rec->pdm_exec_errorcode = g.pdm_start_error;

	sprintf( buf, PDMD_MSG_7, g.prog_name );
	rec->pdm_exec_errormessage = xpstrdup( buf );

	return;
    }

    /*
     * Fill in the PDM_MANAGER portion of the client record.
     */
    rec->video_display_str  = xpstrdup( list[0] );
    rec->video_window       = strtol(list[1], (char **)NULL, 16);
    rec->print_display_str  = xpstrdup( list[2] );
    rec->print_window       = strtol(list[3], (char **)NULL, 16);
    rec->print_context      = strtol(list[4], (char **)NULL, 16);
    rec->locale_hint        = xpstrdup( list[5] );
    XFreeStringList( list );

    rec->selection_display  = selection_display;
    rec->requestor          = requestor;
    rec->prop_atom          = prop_atom;
    rec->selection          = report->xselectionrequest.selection;
    rec->time               = report->xselectionrequest.time;

    rec->mgr_flag           = True;	/* mgr portion of rec now valid */

    /*
     * Optimization.  The only live display connection, for which we
     * need to trap XIO errors, is "selection display".  For the
     * "video" and "print" displays, we have the display strings and
     * can establish connections as we need them.  Since they are rarely
     * used, and opening them up here would create XIO liability problems
     * and a startup performance hit, we won't establish connections now.
     *
     * One optimization however is to see if the "print" display would
     * just happen to be the same at the "selection display" currently
     * open.
     */
    if ( !strcmp( XDisplayString(rec->selection_display),
		  rec->print_display_str ) ) {
	rec->seldpy_as_printdpy = True;
    }
    else {
	rec->seldpy_as_printdpy = False;

#ifdef OPTIONAL_PXAUTH_PRETEST
	/*
	 * Verify connectability to the Print Server.
	 *
	 * Note: once beyond the selection phase, all communication
	 *       will be by way of the Print Server.  If we cannot
	 *       connect later, then we will have no way to deliver
	 *       EXIT_PXAUTH, EXIT_VXAUTH, EXIT_ERROR, EXIT_OK or
         *       EXIT_CANCEL.  Real bad news!
	 *
	 * It is better to discover now that we don't have
	 * connection authorization for the print display since
	 * we can still let the user know of PXAUTH problems
	 * via the selection display currently open.
	 *
	 * Unfortunately, this pre-test is a performance hit in the
	 * startup phase.
	 */
	if ( ! (testdpy = XOpenDisplay(rec->print_display_str)) ) {
	    rec->pdm_exec_errorcode = g.pdm_start_pxauth;
	    return;
	}
	XCloseDisplay( testdpy );
#endif /* OPTIONAL_PXAUTH_PRETEST */
    }

#ifdef OPTIONAL_VXAUTH_PRETEST
    /*
     * Verify connectability to the Video Server.
     *
     * It is better to discover now that we don't have
     * connection authorization for the video display since
     * we can still let the user know of VXAUTH problems
     * via the selection display currently open.
     *
     * Unfortunately, this pre-test is a performance hit in the
     * startup phase.
     */
    if ( ! (testdpy = XOpenDisplay(rec->video_display_str)) ) {
	rec->pdm_exec_errorcode = g.pdm_start_vxauth;
	return;
    }
    XCloseDisplay( testdpy );
#endif /* OPTIONAL_VXAUTH_PRETEST */

}
/********************************************************************
 *
 * fork/exec a child pdm after setting up a message pipe. 
 */
void mgr_launch_pdm( XpPdmServiceRec *rec )
{
    int       i;
    struct sigaction svec;
    char      buf[1024];
    int       original_umask;
    char      *existing_name;
    FILE      *existing_file;
    Xauth     *entry;
    char      *envstr;


    /*
     * Setup message pipe.
     */
    if ( pipe(rec->message_pipe) == -1 ) {
	rec->pdm_exec_errorcode = g.pdm_start_error;
	sprintf( buf, PDMD_MSG_8, g.prog_name );
	rec->pdm_exec_errormessage = xpstrdup( buf );
	return;
    }

    rec->message_xtid = XtAppAddInput( g.context, rec->message_pipe[0],
			  (XtPointer) XtInputReadMask,
			  message_pipe_handler, (XtPointer) NULL );

    /*
     * See if a cookie file is needed.
     */
    if (rec->cookie_cnt) {
	/*
	 * Create new .Xauthority file.
	 */
	original_umask = umask (0077);      /* disallow non-owner access */
	tmpnam( rec->auth_filename );
	rec->auth_file = fopen( rec->auth_filename, "w" );

	if (rec->auth_file) {
	    /*
	     * Copy existing .Xauthority entries.
	     */
	    existing_name = XauFileName ();

	    if (existing_name) {
		if (access (existing_name, R_OK) == 0) {     /* checks REAL id */
		    existing_file = fopen (existing_name, "r");
		    if (existing_file) {
			for (;;) {
			    entry = XauReadAuth (existing_file);
			    if (!entry)
				break;

			    XauWriteAuth( rec->auth_file, entry );
			    XauDisposeAuth (entry);
			}
			fclose (existing_file);
		    }
		}
	    }

	    /*
	     * Merge in cookies recently sent.
	     */
	    for ( i = 0; i < rec->cookie_cnt; i++ ) {
		XauWriteAuth( rec->auth_file, rec->cookies[i] );
	    }

	    fclose( rec->auth_file );
	}
	original_umask = umask (original_umask);
    }


    rec->pid = fork();

    if ( rec->pid < 0 ) {
	rec->pdm_exec_errorcode = g.pdm_start_error;
	sprintf( buf, PDMD_MSG_9, g.prog_name );
	rec->pdm_exec_errormessage = xpstrdup( buf );
	return;
    }
    else if ( rec->pid == 0) {
	/*
	 * Child process.
	 */

	/*
	 * Hook stderr back to parent via message pipe.
	 */
	dup2(rec->message_pipe[1], 2);
	close(rec->message_pipe[0]);

	/*
	 * The child should have default behavior for all signals.
	 */
	sigemptyset(&svec.sa_mask);
	svec.sa_flags   = 0;
	svec.sa_handler = SIG_DFL;
	(void) sigaction(SIGCHLD, &svec, (struct sigaction *) NULL);

	for (i=3; i < FOPEN_MAX; i++) {
	    if ((i != rec->message_pipe[1]) && 
		(rec->auth_file && (i != fileno(rec->auth_file))))
	    {
		(void) fcntl (i, F_SETFD, 1);
	    }
	}

	/*
	 * Set the new locale for the child.
	 *
	 * note: the locale hint will be of the form:
	 *
	 *    name_spec[;registry_spec[;ver_spec[;encoding_spec]]]
	 *
	 * for now, just pull out the name_spec (e.g. 'C')
	 * and use it.   With a little work, a more complex
	 * syntax could be understood and the appropriate
	 * actions taken here rather than just wedging
	 * name_spec into setlocale() and hoping.
	 */
	if ( !(rec->locale_hint) ) {
	    /*
	     * Leave current locale alone.
	     */
	}
	else if ( strcmp( rec->locale_hint, "" ) ) {
	    /*
	     * Leave current locale alone.  Note that "" into
	     * setlocale says to go with default vs leave it alone.
	     */
	}
	else {
	    char *tptr1, *tptr2;

	    tptr1 = xpstrdup( rec->locale_hint );
            tptr2 = strchr( tptr1, ';' );
	    if (tptr2) *tptr2 = '\0';
	
	    setlocale( LC_ALL, tptr1 );
	    XFree( tptr1 );
	}

	/*
	 * Set XAUTHORITY env var if needed.
	 */
	if ((rec->cookie_cnt) && (rec->auth_filename) && (rec->auth_file)) {
	    envstr = Xmalloc( strlen(rec->auth_filename) + 12 );
	    sprintf( envstr, "XAUTHORITY=%s", rec->auth_filename );
	    putenv( envstr );
	}

	/*
	 * Start the child for real.
	 */
	(void) execvp(rec->pdm_exec_argvs[0], rec->pdm_exec_argvs);

	(void) fprintf (stderr, PDMD_MSG_10, g.prog_name, rec->pdm_exec_argvs[0]);

	/*
	 * tomg - need to deal with failed child start.
	 */
	exit(PDM_EXIT_ERROR);
    }
    else {
	/*
	 * Parent process.
	 */

	/*
	 * Close the write end of the pipe - only the child needs it.
	 */
	close(rec->message_pipe[1]);
	rec->message_pipe[1] = -1;
    }
}


/********************************************************************
 *
 * Figure out which pdm executable to later fork/exec.
 */
void mgr_fetch_pdm( XpPdmServiceRec *rec )
{
    char tstr[1024], *tptr1, *tptr2, *tptr3;
    int  firstTime;
    long now;
    Display *tdpy;
    int lxerrno;

    if ( g.override_pdm ) {
	/*
	 * Override all defaults and other possible settings.
	 */
	tptr1 = xpstrdup(g.override_pdm);
    }
    else {
	/*
	 * See if the print context says which pdm to run.
	 */
	g.xerrno = 0;		/* Error Handler */
	lxerrno = 0;		/* XIO Error Handler */

	if ( setjmp( xio_quickie_jmp_buf ) == 0 ) {
	    XSetIOErrorHandler( xio_quickie_handler );

	    if ( rec->seldpy_as_printdpy ) {
		tptr1 = XpGetOneAttribute( rec->selection_display,
				   rec->print_context,
				   XPPrinterAttr, "dt-pdm-command" );
	    }
	    else {
		tdpy = XOpenDisplay( rec->print_display_str );
		if (tdpy) {
		    tptr1 = XpGetOneAttribute( tdpy,
                                   rec->print_context,
                                   XPPrinterAttr, "dt-pdm-command" );
		    XCloseDisplay( tdpy );
		}
	    }

	    XSetIOErrorHandler( NULL );
	}
	else {
	    lxerrno = 1;

	    XSetIOErrorHandler( NULL );
	}

	/*
	 * See if we got a useful pdm exec string.  Use
	 * default if not.
	 */
	if ( g.xerrno || lxerrno ) {
	    rec->pdm_exec_errorcode = g.pdm_start_error;
	    return;
	}
	else if (!tptr1) {
	    tptr1 = xpstrdup(g.default_pdm);
	}
	else if (!tptr1[0]) {
	    tptr1 = xpstrdup(g.default_pdm);
	}
    }
    
    /*
     * Convert pdm-command into argv[] style array.
     *
     * Note: this parsing code does NOT respect shell
     * quotes and other items.   --tomg
     */
    rec->pdm_exec_argvs = (char **) NULL;

    tptr2 = tptr1;	/* retain orig pointer for freeing */
    firstTime = 1;

    while (1) {
	if (firstTime) {
	    tptr3 = xpstrtok( tptr2, " \n\t" );
	    firstTime = 0;

	    if (!tptr3) {
		/*
		 * There were NO useful tokens to begin with, so
		 * we'll have to fall back on the default.
		 */
		xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup( g.default_pdm ));
		break;
	    }
	}
	else {
	    tptr3 = xpstrtok( (char *) NULL, " \n\t" );
	}

	if (tptr3) {
	    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup( tptr3 ) );
	}
	else {
	    break;
	}
    }
    Xfree(tptr1);

    /*
     * Add standard command line parameters.
     */
    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup("-display") );
    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup(rec->video_display_str) );

    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup("-window") );
    sprintf( tstr, "0x%lx", rec->video_window );
    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup(tstr) );

    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup("-pdisplay") );
    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup(rec->print_display_str) );

    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup("-pwindow") );
    sprintf( tstr, "0x%lx", rec->print_window );
    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup(tstr) );

    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup("-pcontext") );
    sprintf( tstr, "0x%lx", rec->print_context );
    xp_add_argv( &(rec->pdm_exec_argvs), xpstrdup(tstr) );
}

/********************************************************************
 *
 * Once a pdm has been lauched, reply to the SelectionRequest so that
 * the requestors knows.
 */
void mgr_launch_reply( XpPdmServiceRec *rec )
{
    XEvent reply;
    Status status;
    FILE   *errlog;
    long   now;
    char   *eec;

    Atom    tmpa;


    XChangeProperty( rec->selection_display, rec->requestor,
		     rec->prop_atom, XA_ATOM,
		     32, PropModeReplace,
		     (unsigned char *) &(rec->pdm_exec_errorcode),
		     1 );

    /*
     * Write optional error message to log file.
     *
     * Expected errors like PXAUTH and VXAUTH should not have
     * textual descriptions too - they're obvious as is.  Only
     * real nasty errors should have a message for the log file.
     */
    if ((rec->pdm_exec_errormessage) && (g.log_file)) {
	if ((errlog = fopen(g.log_file, "a+")) != NULL) {
	    now = time((time_t)0);

	    if ( rec->pdm_exec_errorcode == g.pdm_start_ok )
		eec = "PDM_START_OK";
	    else if ( rec->pdm_exec_errorcode == g.pdm_start_vxauth )
		eec = "PDM_START_VXAUTH";
	    else if ( rec->pdm_exec_errorcode == g.pdm_start_pxauth )
		eec = "PDM_START_PXAUTH";
	    else if ( rec->pdm_exec_errorcode == g.pdm_start_error )
		eec = "PDM_START_ERROR";
	    else
		eec = "unknown error";
		
	    fprintf( errlog, PDMD_MSG_11, g.prog_name, ctime(&now),
			rec->pdm_exec_errormessage,
			eec,
			rec->pdm_exec_argvs[0],
			rec->print_display_str,
			rec->video_display_str );

	    fclose(errlog);
	}
    }


    /*
     * Send a SelectionNotify event, which will conclude the
     * selection handshake.
     */
    reply.xselection.type      = SelectionNotify;
    reply.xselection.requestor = rec->requestor;
    reply.xselection.selection = rec->selection;
    reply.xselection.target    = g.pdm_start;
    reply.xselection.property  = rec->prop_atom;
    reply.xselection.time      = rec->time;

    status = XSendEvent( rec->selection_display, rec->requestor, True, 0, &reply );
}

/********************************************************************
 *
 * Send the final OK/CANCEL ClientMessage.
 */
void mgr_shutdown_reply( XpPdmServiceRec *rec )
{
    XEvent cme;
    Display *pdpy;
    char *mess;
    int inc;
    char buf[2048];
    int lxerrno;


    /*
     * Setup client message event.
     */
    cme.xclient.type         = ClientMessage;
    cme.xclient.window       = rec->print_window;
    cme.xclient.message_type = g.pdm_reply;
    cme.xclient.format       = 32;

    /*
     * Look at the current exit code.  Let 'mess' be both a message
     * string, and a string we would XInternAtom on if we find out
     * that the "Print" X-Server is differnent than the "Selection"
     * X-Server.  Up till now, we've been using atom constants
     * generated via the Selection X-Server.
     */
    switch (rec->exit_code) {
	case PDM_EXIT_OK:
		cme.xclient.data.l[0] = (long) g.pdm_exit_ok;
		mess = "PDM_EXIT_OK";
		break;
	case PDM_EXIT_CANCEL:
		cme.xclient.data.l[0] = (long) g.pdm_exit_cancel;
		mess = "PDM_EXIT_CANCEL";
		break;
	case PDM_EXIT_VXAUTH:
		cme.xclient.data.l[0] = (long) g.pdm_exit_vxauth;
		mess = "PDM_EXIT_VXAUTH";
		break;
	case PDM_EXIT_PXAUTH:
		cme.xclient.data.l[0] = (long) g.pdm_exit_pxauth;
		mess = "PDM_EXIT_PXAUTH";
		break;
	case PDM_EXIT_ERROR:
	default:
		cme.xclient.data.l[0] = (long) g.pdm_exit_error;
		mess = "PDM_EXIT_ERROR";
		break;
    }

    /*
     * Try to send ClientMessage that will carry the reply.
     */

    g.xerrno = 0;  /* Error Handler */
    lxerrno = 0;   /* XIO Error Handler */

    if ( setjmp( xio_quickie_jmp_buf ) == 0 ) {
	XSetIOErrorHandler( xio_quickie_handler );

	if ( rec->seldpy_as_printdpy ) {
	    /*
	     * Since the "Print" X-Server is the same as the
	     * "Selection" X-Server, against which we have an
	     * active display connection and atom values, go
	     * ahead and use it.
	     */
	    XSendEvent( rec->selection_display, rec->print_window, False, 0L, &cme );
	}
	else {
	    pdpy = XOpenDisplay( rec->print_display_str );
	    if (pdpy) {
		/*
		 * The "Print" X-Server is different than the
		 * "Selection" X-Server.  Map values over.
		 */
		cme.xclient.message_type =
				XInternAtom( pdpy, "PDM_REPLY", False );
		cme.xclient.data.l[0] =
				(long) XInternAtom( pdpy, mess, False );
		XSendEvent( pdpy, rec->print_window, False, 0L, &cme );
		XCloseDisplay( pdpy );
	    }
	}

	XSetIOErrorHandler( NULL );
    }
    else {
	lxerrno = 1;

	XSetIOErrorHandler( NULL );
    }

    if ( g.xerrno || lxerrno ) {
	/*
	 * Problem - we can't get back to the requestor.
	 *
	 * This is really a PANIC situation, since the requesting
	 * client will hang around forever, waiting for this
	 * final reply.  The best we can do it log an error for
	 * the sys admin, and hope they can notice.
	 */
	if (g.xerrno)
	    sprintf( buf, PDMD_MSG_12, g.prog_name, mess );
	else
	    sprintf( buf, PDMD_MSG_13, g.prog_name, mess );

	rec->message_string2 = Xmalloc( strlen( buf ) + 1 );
	strcpy( rec->message_string2 , buf );
    }
}


Bool has_exec_token( XpPdmServiceRec *rec )
{
    char *s1, *s2, *s3;
    int i1;

    s1 = rec->message_string;

    if (s1) {
	/*
	 * Look for "PDM_START_*" tokens burried in the
	 * stderr output of the PDM.  If found, react as
	 * required, and eliminate the token from the
	 * output.
	 */
	if ( s2 = strstr( s1, "PDM_START_OK") ) {
	    rec->pdm_exec_errorcode = g.pdm_start_ok;
	    i1 = 12;
	}
	else if ( s2 = strstr( s1, "PDM_START_VXAUTH") ) {
	    rec->pdm_exec_errorcode = g.pdm_start_vxauth;
	    i1 = 16;
	}
	else if ( s2 = strstr( s1, "PDM_START_PXAUTH") ) {
	    rec->pdm_exec_errorcode = g.pdm_start_pxauth;
	    i1 = 16;
	}
	else if ( s2 = strstr( s1, "PDM_START_ERROR") ) {
	    rec->pdm_exec_errorcode = g.pdm_start_error;
	    i1 = 15;
	}

	if (s2) {
	    /*
	     * Compress out the token.
	     */
	    s3 = s2 + i1;
	    while ( *s2++ = *s3++ );

	    if ( strlen(s1) == 0 ) {
		/*
		 * The token was it - free the buffer now so that
		 * it appears no stderr was ever generated. 
		 */
		Xfree( rec->message_string );
		rec->message_string = (char *) NULL;
	    }
	    else if ((strlen(s1) == 1) && (s1[0] == '\n')) {
		/*
		 * All but a \n remains - free the buffer.
		 */
		Xfree( rec->message_string );
		rec->message_string = (char *) NULL;
	    }

	    return( True );
	}
	else {
	    return( False );
	}
    }
    else {
	return( False );
    }
}

/********************************************************************
 *
 * Search through all the child tracking records and see if
 * any can be shutdown.
 */
void mgr_shutdown_scan()
{
    int        i;
    long       now;
    FILE       *errlog;
    static int errlog_problem_notice = 0;
    Bool       shutdown_time;

    for ( i = 0; i < g.serviceRecNum; i++ ) {

	shutdown_time = False;

	if ( (g.serviceRecs[i]->do_launch_reply) &&
	     (has_exec_token(g.serviceRecs[i]) )    ) {
	    /*
	     * Need to send our PDM_START fork/exec reply still.
	     *
	     * Note that if we send pdm_start_ok, we'll need to send
	     * a pdm_exit_* code later.  If we send pdm_start_vxauth,
	     * pdm_start_pxauth, or pdm_start_error, it indicates to
	     * the client a fatal condition, and they will NOT receive
	     * any more messages, including any pdm_exit_* codes.  The
	     * dtpdmd will hang around however, if for no other reason
	     * than to capture stderr and log it when the child finally
	     * goes down.
	     */
	    mgr_launch_reply( g.serviceRecs[i] );
	    g.serviceRecs[i]->do_launch_reply = False;	/* it has been done */
	}

	if ( (g.serviceRecs[i]->pdm_exec_errorcode == g.pdm_start_ok) &&
	     (g.serviceRecs[i]->exit_received) &&
	     (g.serviceRecs[i]->message_pipe[0] == -1) ) {
	    /*
	     * Child is down, and since we sent a pdm_start_ok,
	     * we need to send a PDM_REPLY pdm_exit_* code too.
	     */
	    mgr_shutdown_reply( g.serviceRecs[i] );
	}

	if ( (g.serviceRecs[i]->exit_received) &&
	     (g.serviceRecs[i]->message_pipe[0] == -1) ) {
	    /*
	     * Child is down.  Log any final error messages
	     * from the child, and from the dtpdmd on behalf of
	     * the child.
	     */
	    if ( ((g.serviceRecs[i]->message_string) ||
		  (g.serviceRecs[i]->message_string2)) && (g.log_file) ) {
		if ((errlog = fopen(g.log_file, "a+")) != NULL) {
		    now = time((time_t)0);

		    fprintf (errlog, PDMD_MSG_14, g.prog_name, ctime(&now),
					g.serviceRecs[i]->pdm_exec_argvs[0],
					g.serviceRecs[i]->print_display_str,
					g.serviceRecs[i]->video_display_str,
					g.serviceRecs[i]->exit_code,
					g.serviceRecs[i]->message_string);

		    if (g.serviceRecs[i]->message_string2) {
			fprintf (errlog, PDMD_MSG_15, g.serviceRecs[i]->message_string2);
		    }

		    fprintf (errlog, "\n");
		    fclose(errlog);
		}
		else if (!errlog_problem_notice) {
		    fprintf (stderr, PDMD_MSG_16, g.prog_name, g.log_file );
		    fflush (stderr);
		    errlog_problem_notice = 1;
		}
	    }

	    /*
	     * The child is done and can be cleaned up.
	     */
	    delete_rec( g.serviceRecs[i] );
	}
    }
}

