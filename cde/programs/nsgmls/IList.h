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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: IList.h /main/2 1996/08/13 10:08:48 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef IList_INCLUDED
#define IList_INCLUDED 1

#include "IListBase.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T> class IListIter;

// This owns the objects that are put in it.

template<class T>
class IList : private IListBase {
public:
  IList() { }
  IList(T *p) : IListBase(p) { }
  ~IList() { clear(); }
  void append(T *p) { IListBase::append(p); }
  void insert(T *p) { IListBase::insert(p); }
  void remove(T *p) { IListBase::remove(p); }
  void swap(IList<T> &list) { IListBase::swap(list); }
  T *head() const { return (T *)IListBase::head(); }
  T *get() { return (T *)IListBase::get(); }
  using IListBase::clear;
  using IListBase::empty;
friend class IListIter<T>;
private:
  IList(const IList<T> &) {}
  IList<T> &operator=(const IList<T> &) { return *this; }
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not IList_INCLUDED */
