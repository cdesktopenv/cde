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
/*%%  $XConsortium: iscntl.c /main/3 1995/10/23 11:36:59 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)iscntl.c	1.8 94/11/17";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iscntl.c
 *
 * Description:
 *	Generic control function
 */

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include "isam_impl.h"


/*
 * err =  iscntl(isfd, args)
 *
 * Functions:
 *
 * iscntl(isfd, ISCNTL_RPCS_TO_SET, tout) - Set timeout for short operations
 * iscntl(isfd, ISCNTL_RPCL_TO_SET, tout) - Set timeout for long operations
 * iscntl(isfd, ISCNTL_RPCS_TO_GET) - Get timeout for short operations
 * iscntl(isfd, ISCNTL_RPCL_TO_GET) - Get timeout for long operations
 * iscntl(isfd, ISCNTL_TCP_TO_SET) - Set TCP reconnect timeout
 * iscntl(isfd, ISCNTL_TCP_TO_GET) - Get TCP reconnect timeout
 *
 * iscntl(isfd, ISCNTL_APPLMAGIC_WRITE, string) - Write application magic
 * iscntl(isfd, ISCNTL_APPLMAGIC_READ, buf) - Read application magic
 *
 * iscntl(ALLISFD, ISCNTL_FDLIMIT_SET, n) - Set limit on UNIX fd use
 * iscntl(ALLISFD, ISCNTL_FDLIMIT_GET) - Set limit on UNIX fd use
 *
 * oldfunc = iscntl(ALLISFD, ISCNTL_FATAL, func) - Set fatal error handler
 *     int func(msg) - Apllication handler
 *     if 0 is returned, NetISAM will use openlog("NetISAM") and
 *     syslog(ERR_LOG, msg) to log the error.
 *
 * iscntl(ALLISFD, ISCNTL_MASKSIGNALS, bool) 1 mask, 0 don't mask
 *
 * iscntl(isfd, ISCNTL_FSYNC) -  synchronize a file's in-core state
 *				 with that on disk
 *
 */

typedef int (* intfunc)();

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
int 
iscntl(int isfd, int func, ...)
#else
int 
iscntl(isfd, func, va_alist)
    int			isfd;
    int			func;
    va_dcl
#endif
{
    extern int		(*_isfatal_error_set_func())();
    va_list		pvar;
    int			ret;

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
    va_start(pvar, func);
#else
    va_start(pvar);
#endif
    switch (func) {

	  case ISCNTL_MASKSIGNALS:
	    ret =  _issignals_cntl(va_arg(pvar, int));
	    break;

	  case ISCNTL_FATAL:
	    ret =  (int)_isfatal_error_set_func(va_arg(pvar,  intfunc));
	    break;

	  case ISCNTL_FDLIMIT_SET:
	    ret =  _watchfd_max_set(va_arg(pvar, int));
	    break;

	  case ISCNTL_FDLIMIT_GET:
	    ret =  _watchfd_max_get();
	    break;

	  case ISCNTL_APPLMAGIC_WRITE:
	    ret =  _isapplmw(isfd, (va_arg(pvar, char *)));
	    break;

	  case ISCNTL_APPLMAGIC_READ:
	    ret =  _isapplmr(isfd, (va_arg(pvar, char *)));
	    break;

	  case ISCNTL_FSYNC:
	    ret = _isfsync(isfd);
	    break;

	  default:
	    _setiserrno2(EBADARG, '9', '0');
	    ret =  ISERROR;
    }

    va_end(pvar);
    return (ret);
}
