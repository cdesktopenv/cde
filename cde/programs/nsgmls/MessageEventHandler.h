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
/* $XConsortium: MessageEventHandler.h /main/1 1996/07/29 16:57:27 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef MessageEventHandler_INCLUDED
#define MessageEventHandler_INCLUDED 1

#include "Event.h"
#include "ErrorCountEventHandler.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class OutputCharStream;
class SgmlParser;

class SP_API MessageEventHandler : public ErrorCountEventHandler {
public:
  // if parser is non-null then subdocs will be parsed automatically
  MessageEventHandler(Messenger *messenger, const SgmlParser *parser = 0);
  void message(MessageEvent *);
  void subdocEntity(SubdocEntityEvent *);
  Messenger *messenger() const;
private:
  Messenger *messenger_;
  const SgmlParser *parser_;
};

inline
Messenger *MessageEventHandler::messenger() const
{
  return messenger_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not MessageEventHandler_INCLUDED */
