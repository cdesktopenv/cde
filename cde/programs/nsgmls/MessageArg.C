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
/* $XConsortium: MessageArg.C /main/1 1996/07/29 16:57:04 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "MessageArg.h"
#include "MessageBuilder.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

MessageArg::MessageArg()
{
}

MessageArg::~MessageArg()
{
}

StringMessageArg::StringMessageArg(const StringC &s)
: s_(s)
{
}

MessageArg *StringMessageArg::copy() const
{
  return new StringMessageArg(*this);
}

void StringMessageArg::append(MessageBuilder &builder) const
{
  builder.appendChars(s_.data(), s_.size());
}

NumberMessageArg::NumberMessageArg(unsigned long n)
: n_(n)
{
}

MessageArg *NumberMessageArg::copy() const
{
  return new NumberMessageArg(*this);
}

void NumberMessageArg::append(MessageBuilder &builder) const
{
  builder.appendNumber(n_);
}


OrdinalMessageArg::OrdinalMessageArg(unsigned long n)
: n_(n)
{
}

MessageArg *OrdinalMessageArg::copy() const
{
  return new OrdinalMessageArg(*this);
}

void OrdinalMessageArg::append(MessageBuilder &builder) const
{
  builder.appendOrdinal(n_);
}

RTTI_DEF0(OtherMessageArg)

OtherMessageArg::OtherMessageArg()
{
}

void OtherMessageArg::append(MessageBuilder &builder) const
{
  builder.appendOther(this);
}

#ifdef SP_NAMESPACE
}
#endif
