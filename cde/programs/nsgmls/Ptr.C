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
/* $XConsortium: Ptr.C /main/1 1996/07/29 17:02:08 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Ptr_DEF_INCLUDED
#define Ptr_DEF_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
Ptr<T>::Ptr(T *ptr) : ptr_(ptr)
{
  if (ptr_)
    ptr_->ref();
}

template<class T>
Ptr<T>::~Ptr()
{
  if (ptr_) {
    if (ptr_->unref())
      delete ptr_;
    ptr_ = 0;
  }
}

template<class T>
Ptr<T>::Ptr(const Ptr<T> &p)
: ptr_(p.ptr_)
{
  if (p.ptr_)
    p.ptr_->ref();
}

template<class T>
Ptr<T> &Ptr<T>::operator=(const Ptr<T> &p)
{
  if (p.ptr_)
    p.ptr_->ref();
  if (ptr_ && ptr_->unref())
    delete ptr_;
  ptr_ = p.ptr_;
  return *this;
}

template<class T>
Ptr<T> &Ptr<T>::operator=(T *p)
{
  if (p)
    p->ref();
  if (ptr_ && ptr_->unref())
    delete ptr_;
  ptr_ = p;
  return *this;
}

template<class T>
void Ptr<T>::clear()
{
  if (ptr_) {
    if (ptr_->unref())
      delete ptr_;
    ptr_ = 0;
  }
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Ptr_DEF_INCLUDED */
