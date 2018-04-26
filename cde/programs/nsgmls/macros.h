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
/* $XConsortium: macros.h /main/1 1996/07/29 17:08:55 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef macros_INCLUDED
#define macros_INCLUDED 1

#ifndef __GNUG__
#define __attribute__(args) /* as nothing */
#endif

#ifdef NDEBUG

#include <stdlib.h>
#define ASSERT(expr) ((void)0)
#define CANNOT_HAPPEN() ((void)abort())

#else /* not NDEBUG */

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif
extern SP_API void assertionFailed(const char *, const char *, int)
     __attribute__((noreturn));
#ifdef SP_NAMESPACE
}
#endif

#define ASSERT(expr) \
  ((void)((expr) || \
  (::SP_NAMESPACE_SCOPE assertionFailed(# expr, __FILE__, __LINE__), 0)))
#define CANNOT_HAPPEN() ASSERT(0)

#endif /* not NDEBUG */

#define SIZEOF(v) (sizeof(v)/sizeof(v[0]))

#endif /* not macros_INCLUDED */
