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
/* $XConsortium: Boolean.h /main/1 1996/07/29 16:46:49 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Boolean_INCLUDED
#define Boolean_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef SP_HAVE_BOOL

typedef bool Boolean;
typedef char PackedBoolean;

#else /* not SP_HAVE_BOOL */

typedef int Boolean;
typedef char PackedBoolean;

#endif /* not SP_HAVE_BOOL */

#ifdef SP_NAMESPACE
}
#endif

#ifndef SP_HAVE_BOOL

typedef int bool;

const int true = 1;
const int false = 0;

#endif /* not SP_HAVE_BOOL */

#endif /* not Boolean_INCLUDED */
