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
/*%%  $XConsortium: tt_base64.h /main/4 1996/07/30 17:41:55 barstow $ 			 				 */
/*
 *
 * tt_base64.h
 *
 * Simple routines for encoding and decoding unsigned longs as base-64 numbers
 * Note that encode returns a pointer to internal static storage.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _TT_BASE64_H
#define _TT_BASE64_H

#include "util/tt_string.h"

unsigned long _tt_base64_decode(const char *s);
_Tt_string _tt_base64_encode(unsigned long n);

#endif /* _TT_BASE64_H */

