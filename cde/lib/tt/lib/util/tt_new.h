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
/*%%  $XConsortium: tt_new.h /main/3 1995/10/23 10:41:57 rswiston $ 			 				 */
/*-*-C++-*-
 * @(#)tt_new.h	1.13 @(#)
 * tt_new.h - defines the new and delete operators for all TT classes
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#if !defined(_TT_NEW_H)
#define _TT_NEW_H

#if !defined(__STDC__)
#include <malloc.h>
#include <sys/types.h>
#endif /* __STDC__ */
#include <stdlib.h>
#if defined(sun)
#  define MALLOCTYPE	char
#else
#  define MALLOCTYPE	void
#endif


//
// All classes must at least inherit from this class in order to
// remove dependencies on libC-defined "new" and "delete".
//
class _Tt_allocated {
      public:
	void		*operator new(size_t s);
	void		operator  delete(void *p);
};

#endif
