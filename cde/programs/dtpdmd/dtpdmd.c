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
/* $XConsortium: dtpdmd.c /main/9 1996/10/30 19:10:08 cde-hp $ */

/******************************************************************************
 ******************************************************************************
 **
 ** File:         dtpdmd.c
 **
 ** Description:  main file for the implementation of the dtpdmd.
 **
 ** (c) Copyright 1995, 1996 Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

/******************************************************************************
 *
 * BASIC dtpdmd FLOW
 *
 * Other than setup, one event loop runs the show.
 *
 * Legend:
 *
 *    ->  means XtAppNextEvent breaks out with an event, and a
 *        case statement figures out where the event should go.
 *
 *    =>  means XtAppNextEvent dispatches the handler for
 *        the event directly.
 *
 * XtAppNextEvent()
 *
 *    -> dispatch_mgr
 *          o find_rec              }
 *          o mgr_initialize        }  mgr_launch_reply and delete_rec
 *          o mgr_fetch_pdm         }  called as soon as any errors occur
 *          o mgr_launch_pdm        }
 *          o mgr_shutdown_scan     }
 *
 *    -> dispatch_mbox
 *          o find_rec              }
 *          o mbox_initialize       }  request for
 *          o mbox_build            }  mailbox
 *          o mbox_reply            }
 *          o mgr_shutdown_scan     }
 *       or
 *          o find_rec_by_mbox_win  }  receive incoming
 *          o mbox_receive          }  mail
 *          o mgr_shutdown_scan     }
 *
 *    => xtkick_proc (SIGCLD write to pipe handler causes this)
 *          o mgr_shutdown_scan
 *
 *    => message_pipe_handler
 *          o child's stderr to pipe to malloc'ed buffer
 *          o trap EOF on pipe
 *          o mgr_shutdown_scan
 *
 *    ~> SIGCLD (not really dispatched)
 *          o the handler notes which child, and exit status,
 *            then writes a byte to tickle the xtkick_proc
 *
 * Note the final usage of:
 *
 *    mgr_shutdown_scan
 *       o possibly pdm_launch_reply
 *       o possibly pdm_shutdown_reply
 *       o as appropriate, delete_rec
 */

/******************************************************************************
 *
 * XIO Error Handling Strategy:
 *
 * XIO errors can occur from up to 3 display connections:
 *
 *    o sel_dpy (aka prop_dpy)
 *         - always active.  If this display connection goes down,
 *           then the dtpdmd will not be able to service any more
 *           pdm requests.
 *
 *    o print_dpy (usually equal to sel_dpy)
 *         - within the pdm, on a per client basis, only active
 *           long enough to 1) fetch the dt-pdm-command attribute,
 *           and to 2) send the final ClientMessage with OK/Cancel.
 *
 *           XIO strategy: wrap setjmp/longjmp around the usages
 *           of XOpenDisplay on print_dpy.
 *
 *           XIO result: For case #1, set dt-pdm-command using
 *           built in defaults, thus ignoring XpGetOneAttribute.
 *
 *           For case #2, the best that can be done is to log
 *           a message to the errorlog.
 *
 *    o video_dpy
 *         - within the pdm, on a per client basis, only active
 *           long enough to 1) run a test connection to verify
 *           authorization.
 *
 *           XIO strategy: wrap setjmp/longjmp around the one
 *           XOpenDisplay of video_dpy.
 *
 *           XIO result: act as if authorization failed.
 */
#define DTPDMD_DOT_C

#include "dtpdmdP.h"
#include "nlmsg.h"

/********************************************************************
 *
 * Globals.
 */
XpPdmGlobals g;			/* global to all modules */

static int xtkick_pipeG[2];	/* global to this module */

/********************************************************************
 *
 * pusage
 */
static void pusage( char *prog_name )
{
    fprintf(stderr, "\n");
    fprintf( stderr, PDMD_MSG_1, prog_name, "PDM_MANAGER", DEFAULT_PDM_EXECUTABLE );
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
}


/******************************************************************************
 *
 * generic_error_handler
 *
 *****************************************************************************/

static int generic_error_handler( edpy, eevent )
    Display     *edpy;
    XErrorEvent *eevent;
{
    g.xerrno  = eevent->error_code;
    g.xerrreq = eevent->request_code;
    g.xerrmin = eevent->minor_code;
}

/******************************************************************************
 *
 * handle_SIGCLD
 *
 *****************************************************************************/
static void
#if defined(__aix) || defined(linux)
handle_SIGCLD(int sigNum)
#else
handle_SIGCLD(void)
#endif /* __aix */
{
    int exitStatus, i;
    pid_t pid;

    /*
     * Query why the SIGCLD happened - a true termination or just
     * a stopage.  We only care about terminations.
     */
    pid = wait(&exitStatus);

    if (pid == -1) {
	/*
	 * No specific child found with wait() - punt.
	 */
    }
    else if (!WIFSTOPPED(exitStatus)) {
        /*
         * The SIGCLD was *not* the result of being stopped, so the child
         * has indeed terminated.  Look for its tracking record and mark
	 * accordingly for later shutdown.
         */
	for ( i = 0; i < g.serviceRecNum; i++ ) {
	    if ( g.serviceRecs[i]->pid == pid ) {
		/*
		 * Update tracking record.
		 */
		g.serviceRecs[i]->exit_received = True;
		g.serviceRecs[i]->exit_code = (int) WEXITSTATUS(exitStatus);

		/*
		 * Figure out who will deliver the "tickle" to XtAppNextEvent
		 * that will allow this child's status to be rediscovered
		 * so that a subsequent shutdown can be done.
		 *
		 *    - if the child's message_pipe is still up,
		 *      let its upcoming disconnection be the tickle.
		 *
		 *    - if the child's message pipe is aleady down,
		 *      then this is our last event related to the
		 *      child, so tickle the "Xt Signal Pipe" we
		 *      installed.
		 *
		 *    - if the Xt Signal Pipe is down, then we'll have
		 *      to rely upon a 3rd party to deliver an event
		 *      that will reactivate the Xt Main Loop.
		 */
		if ( g.serviceRecs[i]->message_pipe[0] == -1 ) {
		    /*
		     * Tickle our "Xt Signal Pipe".
		     */
		    write( xtkick_pipeG[1], (char *) "", 1 );
		}

		break;
	    }
	}
    }
}

static void xtkick_proc( XtPointer w, int *source, XtInputId *id)
{
    char buffer[128];

    /*
     * Drain off the kicker bytes.  Don't care how many, since
     * we'll look at all client records.
     */
    if ( read(*source, buffer, sizeof (buffer) - 1) <= 0 ) {
	close (*source);   /* aka xtkick_pipeG[0] */
	XtRemoveInput (*id);

	xtkick_pipeG[0] = -1;	/* mark the pipe as invalid */
	xtkick_pipeG[1] = -1;
    }

    /*
     * Scan client records and see who is ready to be
     * shut down.
     */
    mgr_shutdown_scan();
}

/******************************************************************************
 *
 * main
 */
int
main( argc, argv )
    int argc;
    char **argv;
{
    int tscreen;
    XtInputId xtid;
    Window sel_window;
    Display *sel_display;
    char *display_str, *auth_file;
    Bool security_flag;
    XEvent report;
    struct sigaction svec;


    g.serviceRecNum = 0;
    g.maxServiceRecNum = 0;

    g.prog_name = argv[0];
    g.alt_selection = "PDM_MANAGER";
    g.default_pdm = DEFAULT_PDM_EXECUTABLE;
    g.override_pdm = (char *) NULL;
    display_str = getenv("DISPLAY");
    auth_file = (char *) NULL;
    g.log_file  = (char *) NULL;
    security_flag  = False;

    /*
     * Parse command line arguments.
     */
    while (*++argv) {
        if (!strcmp (*argv, "-a")) {
            g.alt_selection = *++argv;
        }
        else if (!strncmp (*argv, "-d", 2)) {
            display_str = *++argv;
        }
        else if (!strcmp (*argv, "-p")) {
            g.default_pdm = *++argv;
        }
        else if (!strcmp (*argv, "-P")) {
            g.override_pdm = *++argv;
        }
        else if (!strcmp (*argv, "-s")) {
            security_flag = True;
        }
        else if (!strcmp (*argv, "-f")) {
            auth_file = *++argv;
        }
        else if (!strcmp (*argv, "-l")) {
            g.log_file = *++argv;
        }
        else if (!strncmp (*argv, "-h", 2)) {
            pusage( g.prog_name );
	    exit(1);
        }
        else {
            /*
             * Ignore unknown options.
             */
        }
    }


    /*
     * Open a connection to the X-Server.
     */
    XtToolkitInitialize ();
    g.context = XtCreateApplicationContext();
    if ( (sel_display = XtOpenDisplay( g.context, display_str,
					"dtpdmd", "Dtpdmd",
					0, 0, &argc, argv )) == NULL ) {
	fprintf( stderr , "\n" );
	fprintf( stderr , PDMD_MSG_2, g.prog_name, display_str );
	fprintf( stderr , "\n" );
	fprintf( stderr , "\n" );
	exit(0);
    }

    /*
     * Create master PDM window upon which selections
     * will be created.
     */
    tscreen = DefaultScreen( sel_display );

    sel_window = XCreateSimpleWindow( sel_display,
				   DefaultRootWindow( sel_display ),
				   0, 0, 1, 1, 1,
				   BlackPixel(sel_display, tscreen),
				   WhitePixel(sel_display, tscreen) );

    /*
     * Setup PDM_MANAGER selection
     */
    if ( ! _PdmMgrSetup( sel_display, sel_window, security_flag ) ) {
	fprintf( stderr , "\n" );
	fprintf( stderr , PDMD_MSG_3, g.prog_name , g.alt_selection );
	fprintf( stderr , "\n" );
	fprintf( stderr , "\n" );
	exit(0);
    }


    /*
     * Install the "Xt Signal Pipe" Kicker handler.
     */
    if ( pipe(xtkick_pipeG) == -1 ) {
	fprintf( stderr , "\n" );
	fprintf( stderr , PDMD_MSG_4, g.prog_name );
	fprintf( stderr , "\n" );
	fprintf( stderr , "\n" );
	exit(0);
    }
    xtid = XtAppAddInput( g.context, xtkick_pipeG[0],
			  (XtPointer) XtInputReadMask,
			  xtkick_proc, (XtPointer) NULL );

    /*
     * Install signal handers.
     */
    sigemptyset(&svec.sa_mask);
    svec.sa_flags   = 0;
    svec.sa_handler = handle_SIGCLD;
    (void) sigaction(SIGCHLD, &svec, (struct sigaction *) NULL);

    /*
     * After this point, we need to trap all X and XIO errors.
     *
     * XIO trap handlers are installed at critical points, and
     * the following generic X trap handler is used to set
     * globals.
     */
    XSetErrorHandler( generic_error_handler );

    /*
     * MASTER EVENT LOOP
     */
    while (1) {
	/*
	 * XtAppNextEvent breaks for several reasons:
	 *    - X event received
	 *
	 * XtAppNextEvent will dispatch for several reasons:
	 *    - Alt input occurs on a pdm message pipe
	 *    - Alt input occurs because a pdm message pipe was disconnected
	 *    - Alt input occurs on our "Xt Signal Pipe" and we need to
	 *      rediscover what happened as a result of that signal.
	 */
	XtAppNextEvent( g.context, &report );
	switch (report.type) {

	    case ClientMessage:
		if (report.xclient.message_type == g.pdm_mail) {
		    dispatch_mbox( &report );
		}
		else {
		    /* ignore/pitch the event */
		}
		break;

	    case SelectionRequest:
		if (report.xselectionrequest.selection == g.pdm_selection) {
		    if (report.xselectionrequest.target == g.pdm_start) {
			dispatch_mgr( &report );
		    }
		    else if (report.xselectionrequest.target==g.pdm_targets) {
			dispatch_targets( &report );
		    }
		    else if (report.xselectionrequest.target==g.pdm_multiple){
			dispatch_multiple( &report );
		    }
		    else if (report.xselectionrequest.target==g.pdm_timestamp){
			dispatch_timestamp( &report );
		    }
		    else if (report.xselectionrequest.target==g.pdm_mbox) {
			dispatch_mbox( &report );
		    }
		    else {
			dispatch_not_supported( &report );
		    }
		}
		else {
		    /* ignore/pitch the event */
		}
		break;

	    case SelectionNotify:
		/* pitch the event */
		break;

	    case SelectionClear:
		/*
		 * Someone is trying to tear away the selection.
		 * REACT by trying to reclaim it and logging an
		 * error.
		 */
		break;

	    default:
		break;
	}

	/*
	 * Use the opportunity to check for freshly finished
	 * PDMs, and close them out.
	 */
	mgr_shutdown_scan();
    }

    XDestroyWindow( sel_display, sel_window );

    XtCloseDisplay( sel_display );

    exit(0);
}

