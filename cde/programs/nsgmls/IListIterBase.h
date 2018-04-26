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
/* $XConsortium: IListIterBase.h /main/1 1996/07/29 16:53:38 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef IListIterBase_INCLUDED
#define IListIterBase_INCLUDED 1

#include "Link.h"
#include "IListBase.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API IListIterBase {
public:
  IListIterBase(const IListBase &);
  int done();
  Link *cur();
  void next();
private:
  Link *p_;
};

inline
IListIterBase::IListIterBase(const IListBase &list) : p_(list.head_)
{
}

inline
int IListIterBase::done()
{
  return p_ == 0;
}

inline
Link *IListIterBase::cur()
{
  return p_;
}

inline
void IListIterBase::next()
{
  p_ = p_->next_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not IListIterBase_INCLUDED */
