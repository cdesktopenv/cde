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
/* $XConsortium: Resource.h /main/1 1996/07/29 17:02:54 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Resource_INCLUDED
#define Resource_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifndef SP_API
#define SP_API /* as nothing */
#endif

class SP_API Resource {
public:
  Resource();
  Resource(const Resource &);
  int unref();			// return 1 if it should be deleted
  void ref();
  int count() const;
private:
  int count_;
};

inline
Resource::Resource()
: count_(0)
{
}

inline
Resource::Resource(const Resource &)
: count_(0)
{
}

inline
int Resource::count() const
{
  return count_;
}

inline
int Resource::unref()
{
  return --count_ <= 0;
}

inline
void Resource::ref()
{
  ++count_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Resource_INCLUDED */
