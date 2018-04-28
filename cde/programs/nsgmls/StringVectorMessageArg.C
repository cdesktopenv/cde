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
/* $XConsortium: StringVectorMessageArg.C /main/1 1996/07/29 17:05:39 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "StringVectorMessageArg.h"
#include "MessageBuilder.h"
#include "ParserMessages.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

StringVectorMessageArg::StringVectorMessageArg(const Vector<StringC> &v)
: v_(v)
{
}

MessageArg *StringVectorMessageArg::copy() const
{
  return new StringVectorMessageArg(*this);
}

void StringVectorMessageArg::append(MessageBuilder &builder) const
{
  for (size_t i = 0; i < v_.size(); i++) {
    if (i > 0)
      builder.appendFragment(ParserMessages::listSep);
    builder.appendChars(v_[i].data(), v_[i].size());
  }
}

#ifdef SP_NAMESPACE
}
#endif
