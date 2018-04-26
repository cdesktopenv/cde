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
/* $XConsortium: dtpdmdP.h /main/6 1996/10/30 19:11:15 cde-hp $ */
/******************************************************************************
 ******************************************************************************
 **
 ** File:         dtpdmdP.h
 **
 ** Description:  misc header stuff for the dtpdmd
 **
 ** (c) Copyright 1995, 1996, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "X11/Xlibint.h"
#include "X11/Intrinsic.h"
#include "X11/Xatom.h"
#if 0 && defined(PRINTING_SUPPORTED)
#include <X11/extensions/Print.h>
#endif /* PRINTING_SUPPORTED */
#include <X11/Xauth.h>


/*****************************************************************************
 *
 * dtpdm/dtpdmd Protocol
 */
#include <Dt/dtpdmd.h>


/******************************************************************************
 *
 * Child Tracking Record - for each fork/exec'ed child, the following
 * structure is added to an array to maintain status on the child.
 */
typedef struct
{
    /*
     * Manager selection portion.
     */
    Bool        mgr_flag;		/* is MGR tracking portion active */

    char        *video_display_str;	/* Video Server Connection Info */
    Window      video_window;

    char        *print_display_str;	/* Print Server Connection Info */
    Window      print_window;
#if 0 && defined(PRINTING_SUPPORTED)
    XPContext   print_context;
#endif /* PRINTING_SUPPORTED */
    char        *locale_hint;

    Display     *selection_display;	/* Selection & Property Connection */
    Window      requestor;
    Atom        prop_atom;
    Atom        selection;

    Time	time;			/* time of selection request */

    Bool	seldpy_as_printdpy;	/* Can the selection display connection
					   be used in lieu of opening a new
					   print display connection? */

    /*
     * fork/exec and SIGCLD tracking portion.
     */

    /* pre-exec stage */

    int         message_pipe[2];	/* pipe fildes, -1 if disconnected */
    XtInputId   message_xtid;		/* XtAddInput id for message handler */
    char        *message_string;	/* child's stderr */
    char        *message_string2;	/* dtpdmd's addition to child's err */

    /* exec stage */

    char        **pdm_exec_argvs;	/* PDM exec string in argv[] format */
    char        *pdm_exec_errormessage;	/* exec error message (opt) for log */
    Atom	pdm_exec_errorcode;	/* exec error code */
    pid_t       pid;			/* pid of child */
    Bool	do_launch_reply;	/* can someone call mgr_launch_reply? */

    /* shutdown stage */
    Bool	exit_received;		/* child has exited (SIGCLD) */
    int         exit_code;		/* exit code */

    /*
     * Mailbox selection portion.
     */
    Bool        mbox_flag;		/* do we have valid cookies */
    Window      mbox_window;		/* assigned window serving as mbox */
    char        *in_buf;		/* partial-cookie crumbs received */
    int         in_sofar;		/*    - total in so far */
    int		in_expected;		/*    - grand total expected */
	
    Xauth	**cookies;		/* completed cookie collection */
    int         cookie_cnt;		/* cookie cnt - partial & completed */
    short       cookie_state;		/* 0 = NULL term, 1 = non-NULL term,
					   2 = more to come */
    char        auth_filename[L_tmpnam];
    FILE        *auth_file;
} XpPdmServiceRec, *XpPdmServiceList;

/******************************************************************************
 *
 * Global dtpdmd information.   Rather than pass it around in
 * parameter lists, clump in all in one global.
 */
typedef struct
{
    /*
     * Type Atom References
     */
    Atom pdm_selection;			/* PDM Selection */
    Atom pdm_targets;			/* - target */
    Atom pdm_timestamp;			/* - target */
    Atom pdm_multiple;			/* - target */
    Atom pdm_start;			/* - target */
    Atom pdm_start_ok;			/*   - status code */
    Atom pdm_start_vxauth;		/*   - status code */
    Atom pdm_start_pxauth;		/*   - status code */
    Atom pdm_start_error;		/*   - status code */

    Atom pdm_reply;			/* OK/Cancel SendMessage type */
    Atom pdm_exit_ok;			/*   - status code */
    Atom pdm_exit_cancel;		/*   - status code */
    Atom pdm_exit_vxauth;		/*   - status code */
    Atom pdm_exit_pxauth;		/*   - status code */
    Atom pdm_exit_error;		/*   - status code */

    Atom pdm_mbox;			/* - target */

    Atom pdm_mail;			/* Cookie-Package SendMessage type */

    /*
     * X-Selection Information
     */
    char *alt_selection;

    Time time;				/* time selection was owned */

    /*
     * PDM Information
     */
    char *default_pdm, *override_pdm;

    /*
     * Child Service Tracking Record Information
     */
    int             serviceRecNum;
    int             maxServiceRecNum;
    XpPdmServiceRec **serviceRecs;

    /*
     * Xt Information
     */
    XtAppContext context;

    /*
     * Other
     */
    char  *log_file;
    char  *prog_name;

    unsigned char xerrno;
    unsigned char xerrreq;
    unsigned char xerrmin;
} XpPdmGlobals, *XpPdmGlobalsP;

/******************************************************************************
 *
 * Global functions.
 *
 * In each group, one module will define, and all other modules
 * will make extern references to them.  Here are the extern
 * reference statements.
 */

/* dispatch.c */
extern void dispatch_mgr();
extern void dispatch_mbox();
extern void dispatch_targets();
extern void dispatch_multiple();
extern void dispatch_timestamp();
extern void dispatch_not_supported();

/* dtpdmd.c */
extern XpPdmGlobals g;

/* mailbox.c */
extern void mbox_initialize();
extern void mbox_build();
extern void mbox_reply();
extern void mbox_receive();

/* manager.c */
extern void mgr_initialize();
extern void mgr_launch_pdm();
extern void mgr_fetch_pdm();
extern void mgr_launch_reply();
extern void mgr_shutdown_reply();
extern void mgr_shutdown_scan();

/* records.c */
extern XpPdmServiceRec *find_rec();
extern XpPdmServiceRec *find_rec_by_mbox_win();
extern void delete_rec( XpPdmServiceRec *rec );

/* setup.c */
extern Bool _PdmMgrSetup();

/* util.c */
extern jmp_buf xio_quickie_jmp_buf;
extern int xio_quickie_handler();
extern char *xpstrdup();
extern int xpstrspn();
extern int xpstrcspn();
extern char *xpstrtok();
extern void xp_add_argv();

