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
/* $XConsortium: ErrnoMessageArg.h /main/1 1996/07/29 16:51:02 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ErrnoMessageArg_INCLUDED
#define ErrnoMessageArg_INCLUDED 1

#include "MessageArg.h"
#include "rtti.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ErrnoMessageArg : public OtherMessageArg {
  RTTI_CLASS
public:
  ErrnoMessageArg(int errnum) : errno_(errnum) { }
  MessageArg *copy() const;
  // errno might be a macro so we must use a different name
  int errnum() const;
private:
  int errno_;
};

inline
int ErrnoMessageArg::errnum() const
{
  return errno_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ErrnoMessageArg_INCLUDED */
