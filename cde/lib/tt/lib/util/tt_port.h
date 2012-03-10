/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_port.h /main/3 1995/10/23 10:43:42 rswiston $ 			 				 */
/* @(#)tt_port.h	1.12 93/09/07
 *
 * tt_port.h
 *
 * Routines for centralizing portability problems.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#if !defined(_TT_PORT_H)
#define _TT_PORT_H

#include <sys/wait.h>
#include "tt_options.h"

// WCOREDUMP is a semi-useful BSDism which we use to print (core dumped)
// after subprocess termination messages.  If it's not available,
// like on AIX, just suppress the message.
#if !defined(WCOREDUMP)
#define WCOREDUMP(x) (0)
#endif


#if defined(OPT_BSD_WAIT)
	typedef union wait	_Tt_wait_status;
#else
	typedef int		_Tt_wait_status;
#endif

#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <syslog.h>
#include "util/tt_string.h"
#if defined(OPT_DEFINE_SIG_PF)
	typedef void (*SIG_PF)(int);
#endif

#if defined(OPT_BUG_USL) ||  defined(OPT_BUG_UXP)
#if !defined(MNTTYPE_NFS)
#define MNTTYPE_NFS "nfs"
#endif
#endif

_Tt_string	_tt_gethostname(void);
int	_tt_getdtablesize(void);
int	_tt_zoomdtablesize(void);
int	_tt_restoredtablesize(void);
long	_tt_gethostid(void);
int	_tt_sigset(int sig, SIG_PF handler);
char   *_tt_putenv(const char *variable, const char *value);
char   *_tt_get_first_set_env_var(int i_num_names, ...);
int	_tt_put_all_env_var (int i_num_names, const char* pc_val, ...);
void	_tt_openlog(const char *prefix, int logopt, int facility);

//
// Log to sink, or use syslog() if sink is 0
//
void	_tt_syslog(FILE *sink, int priority, const char *format, ...);
void	_tt_vsyslog(FILE *sink, int priority, const char *format, va_list args);

#endif
