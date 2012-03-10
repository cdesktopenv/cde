/* $XConsortium: lutil.h /main/1 1996/04/21 19:23:45 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _LUTIL_H
#define _LUTIL_H

#include "ansi_c.h"

extern char *_DtCmGetPrefix P((char *str, char sep));

extern char *_DtCmGetLocalHost P(());

extern char *_DtCmGetLocalDomain P((char *hostname));

extern char *_DtCmGetHostAtDomain P(());

extern char *_DtCmGetUserName P(());

extern boolean_t _DtCmIsUserName P((char *user));

#endif
