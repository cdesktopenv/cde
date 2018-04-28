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
/* $XConsortium: Link.h /main/1 1996/07/29 16:55:39 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Link_INCLUDED
#define Link_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifndef SP_API
#define SP_API
#endif

class SP_API Link {
public:
  Link();
  Link(Link *);
  virtual ~Link();
private:
  Link *next_;

friend class IListBase;
friend class IListIterBase;
friend class IQueueBase;
};

inline
Link::Link() : next_(0)
{
}

inline
Link::Link(Link *next) : next_(next)
{
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Link_INCLUDED */
