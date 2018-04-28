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
/* $XConsortium: XcharMap.h /main/1 1996/07/29 17:08:09 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef XcharMap_INCLUDED
#define XcharMap_INCLUDED 1

#include "types.h"
#include "Resource.h"
#include "Ptr.h"
#include "constant.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class SharedXcharMap : public Resource {
public:
  SharedXcharMap();
  SharedXcharMap(T defaultValue);
  T *ptr() { return v + 1; }
private:
  T v[2 + charMax];
};

template<class T>
class XcharMap {
public:
  XcharMap();
  XcharMap(T defaultValue);
  T operator[](Xchar c) const { return ptr_[c]; }
  void setRange(Char min, Char max, T val);
  void setChar(Char c, T val) { ptr_[c] = val; }
  void setEe(T val) { ptr_[-1] = val; }
  void clear() { ptr_ = 0; sharedMap_.clear(); }
private:
  T *ptr_;
  Ptr<SharedXcharMap<T> > sharedMap_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not XcharMap_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "XcharMap.C"
#endif
