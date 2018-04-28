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
/* $XConsortium: EventQueue.h /main/1 1996/07/29 16:51:33 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef EventQueue_INCLUDED
#define EventQueue_INCLUDED 1

#include "IQueue.h"
#include "Event.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class EventQueue : public EventHandler, public IQueue<Event> {
public:
  EventQueue();
private:
#define EVENT(c, f) void f(c *);
#include "events.h"
#undef EVENT
  void append(Event *);
};

class Pass1EventHandler : public EventQueue {
public:
  Pass1EventHandler();
  void init(EventHandler *origHandler);
  void message(MessageEvent *);
  Boolean hadError() const;
  EventHandler *origHandler() const;
private:
  Boolean hadError_;
  EventHandler *origHandler_;
};

inline
void EventQueue::append(Event *event)
{
  IQueue<Event>::append(event);
}

inline
Boolean Pass1EventHandler::hadError() const
{
  return hadError_;
}

inline
EventHandler *Pass1EventHandler::origHandler() const
{
  return origHandler_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EventQueue_INCLUDED */
