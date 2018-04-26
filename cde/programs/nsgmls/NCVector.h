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
/* $XConsortium: NCVector.h /main/1 1996/07/29 16:58:13 cde-hp $ */
// Copyright (c) 1994, 1996 James Clark
// See the file COPYING for copying permission.

#ifndef NCVector_INCLUDED
#define NCVector_INCLUDED 1

#include <stddef.h>
#include "xnew.h"

// This offers a subset of the interface offered by the standard C++
// vector class as defined in the Jan 96 WP.
// Code in SP currently assumes that size_type is size_t.

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class NCVector {
public:
  typedef size_t size_type;
  typedef T *iterator;
  typedef const T *const_iterator;
  NCVector() : ptr_(0), size_(0), alloc_(0) { }
  NCVector(size_t n) : ptr_(0), size_(0), alloc_(0) { append(n); }
  ~NCVector();
  void resize(size_t n) {
    if (n < size_)
      erase(ptr_ + n, ptr_ + size_);
    else if (n > size_)
      append(n - size_);
  }
  void swap(NCVector<T> &);
  void clear() { erase(ptr_, ptr_ + size_); }
  size_t size() const { return size_; }
  T &operator[](size_t i) { return ptr_[i]; }
  const T &operator[](size_t i) const { return ptr_[i]; }
  iterator begin() { return ptr_; }
  const_iterator begin() const { return ptr_; }
  T &back() { return ptr_[size_ - 1]; }
  const T &back() const { return ptr_[size_ - 1]; }
  void reserve(size_t n) {  if (n > alloc_) reserve1(n); }
  iterator erase(const_iterator, const_iterator);
private:
  void append(size_t);
  void reserve1(size_t);
  
  size_t size_;
  T *ptr_;
  size_t alloc_;		// allocated size
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not NCVector_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "NCVector.C"
#endif
