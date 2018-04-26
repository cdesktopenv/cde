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
/* $XConsortium: Undo.h /main/1 1996/07/29 17:07:28 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Undo_INCLUDED
#define Undo_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Link.h"
#include "ContentToken.h"
#include "OpenElement.h"
#include "Allocator.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class ParserState;
class Event;

class Undo : public Link {
public:
  void *operator new(size_t sz, Allocator &alloc) { return alloc.alloc(sz); }
  void *operator new(size_t sz) { return Allocator::allocSimple(sz); }
  void operator delete(void *p) { Allocator::free(p); }
  Undo();
  virtual ~Undo();
  virtual void undo(ParserState *) = 0;
private:
  Undo(const Undo &);		// undefined
  void operator=(const Undo &);	// undefined
};

class UndoTransition : public Undo {
public:
  UndoTransition(const MatchState &);
  void undo(ParserState *);
private:
  UndoTransition(const UndoTransition &); // undefined
  void operator=(const UndoTransition &); // undefined
  MatchState state_;
};

class UndoStartTag : public Undo {
public:
  UndoStartTag();
  void undo(ParserState *);
private:
  UndoStartTag(const UndoStartTag &); // undefined
  void operator=(const UndoStartTag &);	// undefined
};

class UndoEndTag : public Undo {
public:
  UndoEndTag(OpenElement *);
  void undo(ParserState *);
private:
  UndoEndTag(const UndoEndTag &); // undefined
  void operator=(const UndoEndTag &); // undefined
  Owner<OpenElement> element_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Undo_INCLUDED */
