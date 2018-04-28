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
/* $XConsortium: ISet.h /main/1 1996/07/29 16:54:18 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ISet_INCLUDED
#define ISet_INCLUDED


#include <stddef.h>
#include "Vector.h"
#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T> class ISetIter;

template<class T>
struct ISetRange {
  ISetRange() { }
  ~ISetRange() { }
  T min;
  T max;
};
  
template<class T>
class ISet {
public:
  ISet();
  ISet(const T *, size_t);
  ~ISet();
  Boolean contains(T) const;
  void remove(T);
  void add(T x) { addRange(x, x); }
  void addRange(T, T);
#if 0
  void add(const ISet<T> &);
#endif
  void check();
  void operator+=(T x) { addRange(x, x); }
  void clear();
  Boolean isSingleton() const {
    return r_.size() == 1 && r_[0].min == r_[0].max;
  }
  Boolean isEmpty() const { return r_.size() == 0; }
  void swap(ISet<T> &x) { r_.swap(x.r_); }
friend class ISetIter<T>;
private:
  Vector<ISetRange<T> > r_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ISet_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "ISet.C"
#endif
