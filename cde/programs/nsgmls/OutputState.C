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
/* $XConsortium: OutputState.C /main/1 1996/07/29 16:59:44 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "OutputState.h"
#include "Event.h"
#include "Allocator.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

OutputState::OutputState()
: re_(0)
{
  init();
}

void OutputState::init()
{
  nextSerial_ = 0;
  stack_.clear();
  stack_.insert(new OutputStateLevel);
}

OutputStateLevel::OutputStateLevel()
: state(OutputState::afterStartTag), reSerial(0)
{
}

void OutputState::handleRe(EventHandler &handler, Allocator &alloc,
			   const EventsWanted &eventsWanted, Char re,
			   const Location &location)
{
  re_ = re;
  if (eventsWanted.wantInstanceMarkup())
    handler.reOrigin(new (alloc) ReOriginEvent(re_, location, nextSerial_));
  switch (top().state) {
  case afterStartTag:
    // it's the first RE in the element
    if (eventsWanted.wantInstanceMarkup())
      handler.ignoredRe(new (alloc) IgnoredReEvent(re_, location, nextSerial_++));
    top().state = afterRsOrRe;
    break;
  case afterRsOrRe:
  case afterData:
    top().state = pendingAfterRsOrRe;
    top().reLocation = location;
    top().reSerial = nextSerial_++;
    break;
  case pendingAfterRsOrRe:
    // We now know that the pending RE won't be ignored as the last RE.
    handler.data(new (alloc) ReEvent(&re_, top().reLocation, top().reSerial));
    top().state = pendingAfterRsOrRe;
    top().reLocation = location;
    top().reSerial = nextSerial_++;
    break;
  case pendingAfterMarkup:
    // We've had only markup since the last RS or RE, so this
    // RE is ignored.  Note that it's this RE that's ignored, not
    // the pending one.
    if (eventsWanted.wantInstanceMarkup())
      handler.ignoredRe(new (alloc) IgnoredReEvent(re_, location, nextSerial_++));
    top().state = pendingAfterRsOrRe;
    break;
  }
}

void OutputState::noteRs(EventHandler &, Allocator &, const EventsWanted &)
{
  if (top().hasPendingRe())
    top().state = pendingAfterRsOrRe;
  else
    top().state = afterRsOrRe;
}

void OutputState::noteMarkup(EventHandler &, Allocator &, const EventsWanted &)
{
  switch (top().state) {
  case afterRsOrRe:
    top().state = afterStartTag;
    break;
  case pendingAfterRsOrRe:
    top().state = pendingAfterMarkup;
    break;
  default:
    break;			// avoid warning
  }
}

void OutputState::noteData(EventHandler &handler, Allocator &alloc,
			   const EventsWanted &)
{
  if (top().hasPendingRe())
    handler.data(new (alloc) ReEvent(&re_, top().reLocation, top().reSerial));
  top().state = afterData;
}

void OutputState::noteStartElement(Boolean included,
				   EventHandler &handler, Allocator &alloc,
				   const EventsWanted &)
{
  if (included)
    stack_.insert(new OutputStateLevel);
  else {
    if (top().hasPendingRe())
      handler.data(new (alloc) ReEvent(&re_, top().reLocation, top().reSerial));
    top().state = afterStartTag;
  }
}

void OutputState::noteEndElement(Boolean included, EventHandler &handler,
				 Allocator &alloc,
				 const EventsWanted &eventsWanted)
{
  if (eventsWanted.wantInstanceMarkup() && top().hasPendingRe())
    handler.ignoredRe(new (alloc) IgnoredReEvent(re_, top().reLocation,
						 top().reSerial));
  if (included) {
    delete stack_.get();
    noteMarkup(handler, alloc, eventsWanted);
  }
  else
    top().state = afterData;
}

#ifdef SP_NAMESPACE
}
#endif
