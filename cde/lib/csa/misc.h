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
/* $XConsortium: misc.h /main/1 1996/04/21 19:23:54 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _MISC_H
#define _MISC_H

/*
 * This file contains some miscellaneous defines.
 */

#define	_DtCM_FIRST_EXTENSIBLE_SERVER_VERSION	5
#define	_DtCM_FIRST_EXTENSIBLE_DATA_VERSION	4

#define _DtCM_SPEC_VERSION_SUPPORTED \
	"-//XAPIA/CSA/VERSION1/NONSGML CSA Version 1//EN"

#define _DtCM_PRODUCT_IDENTIFIER \
	"-//DT//NONSGML Calendar Product Version 1//EN"

/*
 * Time related definitions
 */

#define	_DtCM_BOT	0
#define	_DtCM_EOT	2147483647	/* 2^31-1 */

#endif
