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
/*%%  $XConsortium: tt_assert.h /main/3 1995/10/23 10:36:45 rswiston $ 			 				 */
/*
 * Tool Talk Utility - tt_assert.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Definition of ASSERT macro for debugging & testing.
 *
 */

#ifndef  _TT_ASSERT_H
#define  _TT_ASSERT_H

#ifdef TT_DEBUG

#include <stdlib.h>

#define ASSERT(exp, warning) { if (!(exp)) { printf("ERROR: !(exp), "); printf(warning); printf("\n"); \
abort(); } }

#else /* !TT_DEBUG */

#define ASSERT(exp, warning)

#endif /* TT_DEBUG */

#endif  /* _TT_ASSERT_H */
