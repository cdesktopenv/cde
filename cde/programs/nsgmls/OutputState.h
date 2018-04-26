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
/* $XConsortium: OutputState.h /main/1 1996/07/29 16:59:49 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef OutputState_INCLUDED
#define OutputState_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Location.h"
#include "IList.h"
#include "Link.h"
#include "Boolean.h"
#include "types.h"
#include "EventsWanted.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct OutputStateLevel : public Link {
  OutputStateLevel();
  Boolean hasPendingRe() const;
  char state;			// should be OutputState::State
  unsigned long reSerial;
  Location reLocation;
};

class EventHandler;
class Allocator;

class OutputState {
public:
  OutputState();
  void init();
  void handleRe(EventHandler &, Allocator &, const EventsWanted &,
		Char, const Location &);
  void noteRs(EventHandler &, Allocator &, const EventsWanted &);
  void noteMarkup(EventHandler &, Allocator &, const EventsWanted &);
  void noteData(EventHandler &, Allocator &, const EventsWanted &);
  void noteStartElement(Boolean included,
			EventHandler &, Allocator &, const EventsWanted &);
  void noteEndElement(Boolean included,
		      EventHandler &, Allocator &, const EventsWanted &);
private:
  OutputState(const OutputState &); // undefined
  void operator=(const OutputState &); // undefined
  enum State {
    afterStartTag,
    afterRsOrRe,
    afterData,
    pendingAfterRsOrRe,
    pendingAfterMarkup
  };
  IList<OutputStateLevel> stack_;
  OutputStateLevel &top();
  Char re_;
  unsigned long nextSerial_;
  friend struct OutputStateLevel;
};

inline
Boolean OutputStateLevel::hasPendingRe() const
{
  return int(state) >= int(OutputState::pendingAfterRsOrRe);
}

inline
OutputStateLevel &OutputState::top()
{
  return *stack_.head();
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not OutputState_INCLUDED */
