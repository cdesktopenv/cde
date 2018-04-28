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
/* $XConsortium: NCVector.C /main/1 1996/07/29 16:58:08 cde-hp $ */
// Copyright (c) 1994, 1996 James Clark
// See the file COPYING for copying permission.

#ifndef NCVector_DEF_INCLUDED
#define NCVector_DEF_INCLUDED 1

#include <stddef.h>
#include <string.h>

#ifdef SP_QUAL_TEMPLATE_DTOR_BROKEN
#define DTOR(T) ~T
#else
#define DTOR(T) T::~T
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
NCVector<T>::~NCVector()
{
  if (ptr_) {
    erase(ptr_, ptr_ + size_);
    ::operator delete((void *)ptr_);
  }
}

#if 0

template<class T>
NCVector<T>::NCVector(const NCVector<T> &v)
: ptr_(0), size_(0), alloc_(0)
{
  insert(ptr_ + size_, v.ptr_, v.ptr_ + v.size_);
}

template<class T>
NCVector<T>::NCVector(size_t n, const T &t)
: ptr_(0), size_(0), alloc_(0)
{
  insert(ptr_ + size_, n, t);
}

template<class T>
NCVector<T> &NCVector<T>::operator=(const NCVector<T> &v)
{
  if (&v != this) {
    size_t n = v.size_;
    if (n > size_) {
      n = size_;
      insert(ptr_ + size_, v.ptr_ + size_, v.ptr_ + v.size_);
    }
    else if (n < size_)
      erase(ptr_ + n, ptr_ + size_);
    while (n-- > 0)
      ptr_[n] = v.ptr_[n];
  }
  return *this;
}

template<class T>
void NCVector<T>::assign(size_t n, const T &t)
{
  size_t sz = n;
  if (n > size_) {
    sz = size_;
    insert(ptr_ + size_, n - size_, t);
  }
  else if (n < size_)
    erase(ptr_ + n, ptr_ + size_);
  while (sz-- > 0)
    ptr_[sz] = t;
}

template<class T>
void NCVector<T>::insert(const T *p, size_t n, const T &t)
{
  size_t i = p - ptr_;
  reserve(size_ + n);
  if (i != size_)
    memmove(ptr_ + i + n, ptr_ + i, (size_ - i)*sizeof(T));
  size_ += n;
  for (T *pp = ptr_ + i; n-- > 0; pp++)
    (void)new (pp) T(t);
}

template<class T>
void NCVector<T>::insert(const T *p, const T *q1, const T *q2)
{
  size_t i = p - ptr_;
  size_t n = q2 - q1;
  reserve(size_ + n);
  if (i != size_)
    memmove(ptr_ + i + n, ptr_ + i, (size_ - i)*sizeof(T));
  size_ += n;
  for (T *pp = ptr_ + i; q1 != q2; q1++, pp++)
    (void)new (pp) T(*q1);
}

#endif

template<class T>
void NCVector<T>::swap(NCVector<T> &v)
{
  {
    T *tem = ptr_;
    ptr_ = v.ptr_;
    v.ptr_ = tem;
  }
  {
    size_t tem = size_;
    size_ = v.size_;
    v.size_ = tem;
  }
  {
    size_t tem = alloc_;
    alloc_ = v.alloc_;
    v.alloc_ = tem;
  }
}

template<class T>
void NCVector<T>::append(size_t n)
{
  reserve(size_ + n);
  while (n-- > 0)
    (void)new (ptr_ + size_++) T;
}

template<class T>
T *NCVector<T>::erase(const T *p1, const T *p2)
{
  // typedef T X;
  for (const T *p = p1; p != p2; p++)
    /* ((X *)p)->~X(); */
    p->~T();
  if (p2 != ptr_ + size_)
    memmove((T *)p1, p2, ((const T *)(ptr_ + size_) - p2)*sizeof(T));
  size_ -= p2 - p1;
  return (T *)p1;
}

template<class T>
void NCVector<T>::reserve1(size_t size)
{
  alloc_ *= 2;
  if (size > alloc_)
    alloc_ += size;
  void *p = ::operator new(alloc_*sizeof(T));
  if (ptr_) {
    memcpy(p, ptr_, size_*sizeof(T));
    ::operator delete((void *)ptr_);
  }
  ptr_ = (T *)p;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not NCVector_DEF_INCLUDED */
