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
/* $XConsortium: MessageEventHandler.C /main/1 1996/07/29 16:57:23 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "MessageEventHandler.h"
#include "SgmlParser.h"
#include "ParserOptions.h"
#ifdef __GNUG__
#include "Entity.h"
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

MessageEventHandler::MessageEventHandler(Messenger *messenger,
					 const SgmlParser *parser)
: messenger_(messenger), parser_(parser)
{
}

void MessageEventHandler::subdocEntity(SubdocEntityEvent *event)
{
  const SubdocEntity *entity = event->entity();
  if (entity && parser_) {
    SgmlParser::Params params;
    params.subdocReferenced = 1;
    params.subdocInheritActiveLinkTypes = 1;
    params.origin = event->entityOrigin()->copy();
    params.parent = parser_;
    params.sysid = entity->externalId().effectiveSystemId();
    params.entityType = SgmlParser::Params::subdoc;
    SgmlParser parser(params);
    const SgmlParser *oldParser = parser_;
    parser_ = &parser;
    parser.parseAll(*this);
    parser_ = oldParser;
  }
  delete event;
}

void MessageEventHandler::message(MessageEvent *event)
{
  messenger_->dispatchMessage(event->message());
  ErrorCountEventHandler::message(event);
}

#ifdef SP_NAMESPACE
}
#endif
