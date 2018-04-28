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
/* $XConsortium: Owner.h /main/3 1996/08/13 14:30:51 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Owner_INCLUDED
#define Owner_INCLUDED 1

// A pointer that owns the object pointed to.
// T must be of class type.
// This is coded so that T need not yet have been defined.

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class Owner {
public:
  Owner() : p_(0) { }
  Owner(T *p) : p_(p) { }
  ~Owner();
  void operator=(T *p) {
    if (p_) del();
    p_ = p;
  }
  operator int() const { return p_ != 0; }
  T *pointer() const { return p_; }
  T *operator->() const { return p_; }
  T &operator*() const { return *p_; }
  void swap(Owner<T> &x) {
    T *tem = p_;
    p_ = x.p_;
    x.p_ = tem;
  }
  T *extract() {
    T *tem = p_;
    p_ = 0;
    return tem;
  }
  void clear() {
    if (p_) {
      del();
      p_ = 0;
    }
  }
private:
  Owner(const Owner<T> &) {}
  void operator=(const Owner<T> &) {}
  void del();
  T *p_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Owner_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "Owner.C"
#endif
