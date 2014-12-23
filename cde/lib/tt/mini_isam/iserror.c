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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: iserror.c /main/3 1995/10/23 11:38:16 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)iserror.c 1.10 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iserror.c
 *
 * Description:
 *   	NetISAM error handling functions.
 *
 */

#include "isam_impl.h"
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>
#ifdef __ultrix__
#define LOG_USER LOG_INFO
#endif
/*
 * _isfatal_error(msg)
 *
 * Fatal error. Display message and terminate program.
 */

static int (*fatal_error_user_handler)();    /* set by iscntl(..,ISCNTL_FATAL,..) */

void 
_isfatal_error(char *msg)
{
  int		logerr;

  if (fatal_error_user_handler) {
    logerr = fatal_error_user_handler(msg); /* User returns 1 in order
					     * to use syslog() 
					     */
  }
  else
    logerr = 1;

  if (logerr) {
    openlog("NetISAM", LOG_PID, LOG_USER);

    /* Free one UNIX for syslog */
    (void)close(0);			    
					    
    syslog(LOG_ERR, "Fatal error: %s - UNIX errno %d", msg, errno);

    closelog();
  }
  exit (1);
}

void 
_isfatal_error1(char *msg)
{
  extern int	_is_rpcnetisamd; /* is 1 if this is rpc.netisamd */
  extern int	_is_netisamlockd; /* is 1 if this is netisamlockd */
  int		logerr;

  if (fatal_error_user_handler) {
    logerr = fatal_error_user_handler(msg); /* User returns 1 in order
					     * to use syslog() 
					     */
  }
  else
    logerr = 1;

  if (logerr) {
      openlog("NetISAM", LOG_PID, LOG_USER);

    /* Free one UNIX for syslog */
    (void)close(0);			    
					    
    syslog(LOG_ERR, "Fatal error: %s - UNIX errno %d", msg, errno);

    closelog();
  }
}

void
_isam_warning(char *msg)
{
    openlog("NetISAM", LOG_PID, LOG_USER);
    syslog(LOG_ERR, "%s", msg);
}

/* Set user specified fatal_error handler */
int  (*_isfatal_error_set_func(func))()
    int		(*func)();
{
    int		(*oldfunc)();

    oldfunc = fatal_error_user_handler;
    fatal_error_user_handler = func;

    return (oldfunc);
}

/*
 * _setiserrno2(error, isstat1, isstat2)
 *
 * Set iserrno variable.
 */

void
_setiserrno2(int error, int is1, int is2)
{
    iserrno = error;
    isstat1 = is1;
    isstat2 = is2;
}

/*
 * _seterr_errcode(errcode)
 *
 * Set all error and status variable from errcode structure.
 */

void
_seterr_errcode(struct errcode *errcode)
{
    iserrno = errcode->iserrno;
    isstat1 = errcode->isstat[0];
    isstat2 = errcode->isstat[1];
    isstat3 = errcode->isstat[2];
    isstat4 = errcode->isstat[3];
}
