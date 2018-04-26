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
/* $XConsortium: MessageArg.h /main/1 1996/07/29 16:57:11 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef MessageArg_INCLUDED
#define MessageArg_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "StringC.h"
#include "rtti.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class MessageBuilder;

class SP_API MessageArg {
public:
  MessageArg();
  virtual ~MessageArg();
  virtual MessageArg *copy() const = 0;
  virtual void append(MessageBuilder &) const = 0;
};

class SP_API StringMessageArg : public MessageArg {
public:
  StringMessageArg(const StringC &);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  StringC s_;
};

class SP_API NumberMessageArg : public MessageArg {
public:
  NumberMessageArg(unsigned long);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  unsigned long n_;
};

class SP_API OrdinalMessageArg : public MessageArg {
public:
  OrdinalMessageArg(unsigned long);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  unsigned long n_;
};

class SP_API OtherMessageArg : public MessageArg {
  RTTI_CLASS
public:
  OtherMessageArg();
  void append(MessageBuilder &) const;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not MessageArg_INCLUDED */
