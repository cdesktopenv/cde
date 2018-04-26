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
/* $XConsortium: EventsWanted.h /main/1 1996/07/29 16:51:38 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef EventsWanted_INCLUDED
#define EventsWanted_INCLUDED 1

#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API EventsWanted {
public:
  EventsWanted();
  Boolean wantInstanceMarkup() const;
  Boolean wantCommentDecls() const; // in instance
  Boolean wantMarkedSections() const; // in instance
  Boolean wantPrologMarkup() const;
  void addInstanceMarkup();
  void addCommentDecls();
  void addMarkedSections();
  void addPrologMarkup();
private:
  PackedBoolean instanceMarkup_;
  PackedBoolean commentDecls_;
  PackedBoolean markedSections_;
  PackedBoolean prologMarkup_;
};

inline
EventsWanted::EventsWanted()
: instanceMarkup_(0), commentDecls_(0), markedSections_(0), prologMarkup_(0)
{
}

inline
Boolean EventsWanted::wantInstanceMarkup() const
{
  return instanceMarkup_;
}

inline
void EventsWanted::addInstanceMarkup()
{
  instanceMarkup_ = 1;
  commentDecls_ = 1;
  markedSections_ = 1;
}

inline
Boolean EventsWanted::wantCommentDecls() const
{
  return commentDecls_;
}

inline
void EventsWanted::addCommentDecls()
{
  commentDecls_ = 1;
}

inline
Boolean EventsWanted::wantMarkedSections() const
{
  return markedSections_;
}

inline
void EventsWanted::addMarkedSections()
{
  markedSections_ = 1;
}

inline
Boolean EventsWanted::wantPrologMarkup() const
{
  return prologMarkup_;
}

inline
void EventsWanted::addPrologMarkup()
{
  prologMarkup_ = 1;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EventsWanted_INCLUDED */
