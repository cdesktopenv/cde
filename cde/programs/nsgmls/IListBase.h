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
/* $XConsortium: IListBase.h /main/1 1996/07/29 16:53:26 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef IListBase_INCLUDED
#define IListBase_INCLUDED 1

#include "Link.h"
#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API IListBase {
public:
  IListBase();
  IListBase(Link *);
  void  append(Link *);
  void insert(Link *);
  Link *head() const;
  Boolean empty() const;
  Link *get();
  void remove(Link *);
  void swap(IListBase &);
  void clear();
private:
  Link *head_;
friend class IListIterBase;
};

inline
IListBase::IListBase() : head_(0)
{
}

inline
IListBase::IListBase(Link *head) : head_(head)
{
}

inline
void IListBase::insert(Link *p)
{
  p->next_ = head_;
  head_ = p;
}

inline
Link *IListBase::head() const
{
  return head_;
}

inline
Boolean IListBase::empty() const
{
  return head_ == 0;
}

inline
Link *IListBase::get()
{
  Link *tem = head_;
  head_ = head_->next_;
  return tem;
}

inline
void IListBase::swap(IListBase &list)
{
  Link *tem = head_;
  head_ = list.head_;
  list.head_ = tem;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not IListBase_INCLUDED */
