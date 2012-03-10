/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_log.h /main/3 1995/10/23 10:41:16 rswiston $ 			 				 */
/*
 *
 * tt_log.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#if !defined(TT_LOG_H)
#define TT_LOG_H

void _tt_log_error(int sys_errno, int line, char *file, char *msg);

#define TT_LOG_ERR(err, msg) \
_tt_log_error((int)err, __LINE__, __FILE__, (char *)msg)

#endif /* TT_LOG_H */
