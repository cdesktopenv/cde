/* $XConsortium: data.h /main/4 1995/11/09 12:43:12 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _DATA_H
#define _DATA_H

/*
 * Common definition for internal data storage
 */

#include "ansi_c.h"

typedef enum {
	rb_ok		= 0,
	rb_duplicate	= 1,
	rb_badtable	= 2,
	rb_notable	= 3,
	rb_failed	= 4,
	rb_other	= 5
} Rb_Status;

typedef enum {
	_DtCmsIsLess,
	_DtCmsIsEqual,
	_DtCmsIsGreater
} _DtCmsComparisonResult;

typedef caddr_t (*_DtCmsGetKeyProc) (/* caddr_t data */);

typedef _DtCmsComparisonResult (*_DtCmsCompareProc)(/* caddr_t key; caddr_t data */);

typedef boolean_t (*_DtCmsEnumerateProc) (/* caddr_t data */);

#endif
