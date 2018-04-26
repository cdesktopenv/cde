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
/* $XConsortium: Undo.C /main/1 1996/07/29 17:07:24 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "Undo.h"
#include "ParserState.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Undo::Undo()
{
}

Undo::~Undo()
{
}

UndoTransition::UndoTransition(const MatchState &state)
: state_(state)
{
}

void UndoTransition::undo(ParserState *parser)
{
  parser->currentElement().setMatchState(state_);
}

UndoStartTag::UndoStartTag()
{
}

void UndoStartTag::undo(ParserState *parser)
{
  parser->popElement();
}

UndoEndTag::UndoEndTag(OpenElement *e)
: element_(e)
{
}

void UndoEndTag::undo(ParserState *parser)
{
  parser->pushElement(element_.extract());
}

#ifdef SP_NAMESPACE
}
#endif
