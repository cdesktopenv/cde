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
/* $XConsortium: Priority.h /main/1 1996/07/29 17:02:03 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Priority_INCLUDED
#define Priority_INCLUDED 1

#include <limits.h>
#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Priority {
public:
  typedef unsigned char Type;
  enum {
    data = 0,
    function = 1,
    delim = UCHAR_MAX
    };
  static inline Type blank(int n) {
    // `Priority::' works round gcc 2.5.5 bug
    return Priority::Type(n + 1);
  }
  static inline Boolean isBlank(Type t) {
    return function < t && t < delim;
  }
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Priority_INCLUDED */
