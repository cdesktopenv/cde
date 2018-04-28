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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ttdbck.h /main/3 1995/10/20 16:34:12 rswiston $ 			 				 */
/*
 *
 * ttdbck.h
 *
 * ToolTalk 1.0 spec data base inspect and repair tool global declarations
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _TTDBCK_H
#define _TTDBCK_H

#include "spec.h"
#include "options_tt.h"
#include "mp_typedb.h"

extern char *progname;			// from argv[0]
extern Spec_list_ptr specs_to_repair;
extern _Tt_typedb_ptr *tdb_ptr;
extern Dbck_specoptions *opts;

#endif /* _TTDBCK_H */
