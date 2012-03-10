/* $XConsortium: utility.h /main/4 1995/11/09 12:54:43 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _UTILITY_H
#define _UTILITY_H

#include "ansi_c.h"

extern char *_DtCmsTarget2Name P((char *target));

extern char *_DtCmsTarget2Location P((char *target));

extern char *_DtCmsTarget2Host P((char *target));

extern char *_DtCmsTarget2Domain P((char *target));

#endif
