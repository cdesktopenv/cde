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
/* $XConsortium: Ptr.h /main/2 1996/08/12 16:00:24 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Ptr_INCLUDED
#define Ptr_INCLUDED 1

#include "Boolean.h"

// T must have Resource as a public base class
// T may be an incomplete type

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class Ptr {
public:
  Ptr() : ptr_(0) { }
  Ptr(T *ptr);
  ~Ptr();
  Ptr(const Ptr<T> &);
  Ptr<T> &operator=(const Ptr<T> &);
  Ptr<T> &operator=(T *);
  T *pointer() const { return ptr_; }
  T *operator->() const { return ptr_; }
  T &operator*() const { return *ptr_; }
  void swap(Ptr<T> &p) {
    T *tem = p.ptr_;
    p.ptr_ = ptr_;
    ptr_ = tem;
  }
  Boolean isNull() const { return ptr_ == 0; }
  // operator const void *() const { return ptr_; }
  void clear();
  Boolean operator==(const Ptr<T> &p) const {
    return ptr_ == p.ptr_;
  }
  Boolean operator!=(const Ptr<T> &p) const {
    return ptr_ != p.ptr_;
  }
  Boolean operator==(const T *p) const {
    return ptr_ == p;
  }
  Boolean operator!=(const T *p) const {
    return ptr_ != p;
  }
private:
  T *ptr_;
};

template<class T>
#if defined (USL)
class ConstPtr : public Ptr<T> {
#else
class ConstPtr : private Ptr<T> {
#endif
public:
  ConstPtr() { }
  ConstPtr(T *ptr) : Ptr<T>(ptr) { }
  ConstPtr(const Ptr<T> &p) : Ptr<T>(p) { }
  ConstPtr(const ConstPtr<T> &p) : Ptr<T>(p) { }
#if defined (USL)
#else
  ConstPtr<T> &operator=(const Ptr<T> &p) {
    Ptr<T>::operator=(p); return *this;
  }
#endif
  ConstPtr<T> &operator=(const ConstPtr<T> &p) {
    Ptr<T>::operator=(p); return *this;
  }
  ConstPtr<T> &operator=(T *p) {
    Ptr<T>::operator=(p); return *this;
  }
  const T *pointer() const { return Ptr<T>::pointer(); }
  const T *operator->() const { return Ptr<T>::pointer(); }
  const T &operator*() const { return *Ptr<T>::pointer(); }
  void swap(ConstPtr<T> &p) { Ptr<T>::swap(p); }
  using Ptr<T>::isNull;
  using Ptr<T>::clear;
  Boolean operator==(const Ptr<T> &p) const { return Ptr<T>::operator==(p); }
  Boolean operator!=(const Ptr<T> &p) const { return Ptr<T>::operator!=(p); }
  Boolean operator==(const ConstPtr<T> &p) const {
    return Ptr<T>::operator==(p);
  }
  Boolean operator!=(const ConstPtr<T> &p) const {
    return Ptr<T>::operator!=(p);
  }
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Ptr_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "Ptr.C"
#endif
