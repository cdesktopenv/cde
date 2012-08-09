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
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: flag_shutdown
 *              init_user_interrupt
 *              main
 *              user_interrupt
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************** USERINT.C *************************
 * $XConsortium: userint.c /main/5 1996/05/07 13:49:00 drk $
 * April 1992.
 * 
 * Init_user_interrupt() initializes interrupt handler.
 * 
 * User_interrupt() returns TRUE if any of several signals
 * has been received anytime since initialization or last call.
 * These occur when user pushes CTRL-C, CTRL-BREAK,
 * or when process is killed by external process.
 * It's usually called in a major loop in a program at a place
 * where it is convenient to allow a graceful exit.  
 * SIGHUP is not trapped because this module is only used in
 * offline programs and users often want to nohup their
 * offline scripts.
 * 
 * Replaces quit_escape() after version 4.0 because
 * quit_escape() only worked on dos platform because of
 * the dependency on conio/curses functions.
 * Current user_interrupt() works on all C platforms.
 *
 * $Log$
 * Revision 2.4  1996/03/13  22:59:54  miker
 * Removed trap of SIGABRT to enable external kills with core dump.
 *
 * Revision 2.3  1996/02/01  19:28:30  miker
 * Made shutdown_now flag volatile.
 *
 * Revision 2.2  1995/10/25  15:03:09  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:21:36  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.2  1995/09/05  19:18:14  miker
 * Minor name changes for DtSearch.
 */
#include "SearchP.h"
#include <signal.h>

/***********#define INCLUDE_MAIN******************/


volatile int	shutdown_now = 0;	/* i.e. FALSE */

/************************************************/
/*						*/
/*		   flag_shutdown		*/
/*						*/
/************************************************/
/* interrupt handler for termination signals */
static void     flag_shutdown (int sig)
{
    shutdown_now = sig;
    return;
}

/************************************************/
/*						*/
/*		init_user_interrupt		*/
/*						*/
/************************************************/
void            init_user_interrupt (void)
{
    signal (SIGINT, flag_shutdown);	/* interrupt (ctrl-c) */
    signal (SIGQUIT, flag_shutdown);	/* quit (ctrl-d) */
    signal (SIGTRAP, flag_shutdown);	/* trace trap */
    signal (SIGKILL, flag_shutdown);	/* kill -9, cannot be trapped */
    signal (SIGALRM, flag_shutdown);	/* called alarm() polling timer */
    signal (SIGTERM, flag_shutdown);	/* kill [-15], sfwr terminate */
#ifdef SIGPWR
    signal (SIGPWR, flag_shutdown);	/* power failure imminent */
#endif
    signal (SIGUSR1, flag_shutdown);	/* kill -30, "pings" OE */
#ifdef _AIX
    signal (SIGXCPU, flag_shutdown);	/* cpu time limit exceeded */
    signal (SIGDANGER, flag_shutdown);	/* out of paging space,
					 * crash immin */
#endif
    return;
}  /* init_user_interrupt() */

/************************************************/
/*						*/
/*		  user_interrupt		*/
/*						*/
/************************************************/
int             user_interrupt (void)
{ return shutdown_now; }



#ifdef INCLUDE_MAIN
/******************** main for testing ****************************/
main ()
{
    int             done = FALSE;
    int             i = 0;

    init_user_interrupt ();
    while (!user_interrupt ())
	printf ("\r%5d: Push CTRL-C or CTRL-BREAK to exit... ", ++i);
    printf ("\nWhew!  Thank-you.\n");
    return;
}  /* main() */

#endif

/******************** USERINT.C *************************/
