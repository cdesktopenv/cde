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
/* $XConsortium: StringOf.C /main/1 1996/07/29 17:05:16 cde-hp $ */
// Copyright (c) 1994, 1996 James Clark
// See the file COPYING for copying permission.

#ifndef StringOf_DEF_INCLUDED
#define StringOf_DEF_INCLUDED 1

#include <string.h>
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
String<T>::String(const T *ptr, size_t length)
: length_(length), alloc_(length)
{
  if (length) {
    ptr_ = new T[length];
    memcpy(ptr_, ptr, length*sizeof(T));
  }
  else
    ptr_ = 0;
}

template<class T>
String<T>::String()
: ptr_(0), length_(0), alloc_(0)
{
}

template<class T>
String<T>::String(const String<T> &s)
: length_(s.length_), alloc_(s.length_)
{
  if (length_) {
    ptr_ = new T[length_];
    memcpy(ptr_, s.ptr_, length_*sizeof(T));
  }
  else
    ptr_ = 0;
}

template<class T>
String<T> &String<T>::operator=(const String<T> &s)
{
  if (&s != this) {
    if (s.length_ > alloc_) {
      if (ptr_)
	delete [] ptr_;
      ptr_ = new T[alloc_ = s.length_];
    }
    memcpy(ptr_, s.ptr_, s.length_*sizeof(T));
    length_ = s.length_;
  }
  return *this;
}

template<class T>
String<T> &String<T>::insert(size_t i, const String<T> &s)
{
  if (length_ + s.length_ > alloc_)
    grow(s.length_);
  for (size_t n = length_ - i; n > 0; n--)
    ptr_[i + n - 1 + s.length_] = ptr_[i + n - 1];
  length_ += s.length_;
  memcpy(ptr_ + i, s.ptr_, s.length_*sizeof(T));
  return *this;
}

template<class T>
String<T> &String<T>::append(const T *p, size_t length)
{
  if (length_ + length > alloc_)
    grow(length);
  memcpy(ptr_ + length_, p, length*sizeof(T));
  length_ += length;
  return *this;
}

template<class T>
void String<T>::grow(size_t n)
{
  if (alloc_ < n)
    alloc_ += n + 16;
  else
    alloc_ += alloc_;
  T *s = new T[alloc_];
  memcpy(s, ptr_, length_*sizeof(T));
  delete [] ptr_;
  ptr_ = s;
}

template<class T>
void String<T>::swap(String<T> &to)
{
  {
    T *tem = to.ptr_;
    to.ptr_ = ptr_;
    ptr_ = tem;
  }
  {
    size_t tem = to.length_;
    to.length_ = length_;
    length_ = tem;
  }
  {
    size_t tem = to.alloc_;
    to.alloc_ = alloc_;
    alloc_ = tem;
  }
}

template<class T>
String<T> &String<T>::assign(const T *p, size_t n)
{
  if (alloc_ < n) {
    if (ptr_)
      delete [] ptr_;
    ptr_ = new T[alloc_ = n];
  }
  length_ = n;
  for(T *to = ptr_; n > 0; n--, to++, p++)
    *to = *p;
  return *this;
}

template<class T>
void String<T>::resize(size_t n)
{
  if (alloc_ < n) {
    T *oldPtr_ = ptr_;
    ptr_ = new T[alloc_ = n];
    if (length_ > 0) {
      memcpy(ptr_, oldPtr_, length_*sizeof(T));
      delete [] oldPtr_;
    }
  }
  length_ = n;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not StringOf_DEF_INCLUDED */
