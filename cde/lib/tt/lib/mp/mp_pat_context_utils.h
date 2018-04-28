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
/*%%  $XConsortium: mp_pat_context_utils.h /main/3 1995/10/23 10:26:36 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_pat_context_utils.h	1.4 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_pat_context_utils.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_pat_context is a context slot in a pattern.
 */

#ifndef MP_PAT_CONTEXT_UTILS_H
#define MP_PAT_CONTEXT_UTILS_H

#include <util/tt_object.h>
#include <util/tt_list.h>
#include <mp/mp_context_utils.h>

class _Tt_pat_context;
declare_derived_list_of(_Tt_pat_context,_Tt_context)

#endif				/* MP_PAT_CONTEXT_UTILS_H */
