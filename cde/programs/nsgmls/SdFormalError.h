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
/* $XConsortium: SdFormalError.h /main/1 1996/07/29 17:03:49 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef SdFormalError_INCLUDED
#define SdFormalError_INCLUDED 1

#include "Link.h"
#include "StringC.h"
#include "Message.h"
#include "Location.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class ParserState;

class SdFormalError : public Link {
public:
  SdFormalError(const Location &, const MessageType1 &, const StringC &);
  void send(ParserState &);
private:
  const MessageType1 *message_;
  Location location_;
  StringC id_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SdFormalError_INCLUDED */
