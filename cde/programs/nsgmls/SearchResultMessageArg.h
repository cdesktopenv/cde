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
/* $XConsortium: SearchResultMessageArg.h /main/1 1996/07/29 17:04:07 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef SearchResultMessageArg_INCLUDED
#define SearchResultMessageArg_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MessageArg.h"
#include "StringC.h"
#include "Vector.h"
#include "Vector.h"
#include "rtti.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API SearchResultMessageArg : public OtherMessageArg {
  RTTI_CLASS
public:
  SearchResultMessageArg();
  void add(StringC &, int);
  MessageArg *copy() const;
  size_t nTried() const;
  const StringC &filename(size_t) const;
  int errnum(size_t) const;
private:
  Vector<StringC> filename_;
  Vector<unsigned int> errno_;
};

inline
size_t SearchResultMessageArg::nTried() const
{
  return filename_.size();
}

inline
const StringC &SearchResultMessageArg::filename(size_t i) const
{
  return filename_[i];
}

inline
int SearchResultMessageArg::errnum(size_t i) const
{
  return errno_[i];
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SearchResultMessageArg_INCLUDED */
