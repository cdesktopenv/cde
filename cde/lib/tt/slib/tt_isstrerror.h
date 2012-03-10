/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_isstrerror.h /main/3 1995/10/23 12:03:35 rswiston $ 			 				 */
/* 
 * tt_isstrerror.h
 * 
 * Copyright (c) 1994 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_s_mp object which represents the global
 * information for the server MP component. There should only be one
 * instance of a _Tt_s_mp object in the server.
 */
#ifndef _TT_ISSTRERROR_H
#define _TT_ISSTRERROR_H

extern const char * _tt_isstrerror(int an_iserrno);

#endif				/* _TT_ISSSTRERROR_H */
