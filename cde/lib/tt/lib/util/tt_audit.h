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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_audit.h /main/3 1995/10/23 10:37:05 rswiston $ 			 				 */
/* %W 93/07/20
 *
 * tt_audit.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef TT_AUDIT_H
#define TT_AUDIT_H
#include <stdio.h>
#include <stdarg.h>
#include <util/tt_trace.h>
#include <util/tt_object.h>

class _Tt_audit: public _Tt_trace {
	public:
		Tt_status entry(const char *argskey, _Tt_entry_pt func, ...);
};
#endif	/* TT_AUDIT_H */
