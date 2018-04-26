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
/* $XConsortium: List.h /main/2 1996/08/13 10:08:58 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef List_INCLUDED
#define List_INCLUDED 1

#include "IList.h"
#include "Link.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class ListItem : public Link {
public:
  ListItem(const T &v) : value(v) { }
  T value;
};

template<class T> class ListIter;

template<class T>
class List {
public:
  List() { }
  void append(const T &item) { list_.append(new ListItem<T>(item)); }
  void insert(const T &item) { list_.insert(new ListItem<T>(item)); }
  const T &head() const { return list_.head()->value; }
  void remove(const T &);
  T get();
  int empty() { return list_.empty(); }
  friend class ListIter<T>;
private:
  List(const List<T> &) {}
  void operator=(const List<T> &) {}

  IList<ListItem<T> > list_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not List_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "List.C"
#endif
