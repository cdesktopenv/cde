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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
