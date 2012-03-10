/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_assert.h /main/3 1995/10/23 10:36:45 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_assert.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Definition of ASSERT macro for debugging & testing.
 *
 */

#ifndef  _TT_ASSERT_H
#define  _TT_ASSERT_H

#ifdef TT_DEBUG

#include <stdlib.h>

#define ASSERT(exp, warning) { if (!(exp)) { printf("ERROR: !(exp), "); printf(warning); printf("\n"); \
abort(); } }

#else /* !TT_DEBUG */

#define ASSERT(exp, warning)

#endif /* TT_DEBUG */

#endif  /* _TT_ASSERT_H */
