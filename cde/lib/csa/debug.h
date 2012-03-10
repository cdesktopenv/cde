/* $XConsortium: debug.h /main/1 1996/04/21 19:23:07 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include "ansi_c.h"

#ifdef CM_DEBUG
#define DP(a)	{ printf a ; }
#else
#define DP(a)
#endif /* CM_DEBUG */

extern void		_DtCm_print_tick		P((long));

extern void		_DtCm_print_errmsg		P((const char *));

#endif /* _DEBUG_H */
