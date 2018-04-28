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
/* $XConsortium: XcharMap.C /main/1 1996/07/29 17:08:05 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef XcharMap_DEF_INCLUDED
#define XcharMap_DEF_INCLUDED 1

#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
SharedXcharMap<T>::SharedXcharMap()
{
}

template<class T>
SharedXcharMap<T>::SharedXcharMap(T defaultValue)
{
  for (size_t i = 0; i < sizeof(v)/sizeof(v[0]); i++)
    v[i] = defaultValue;
}

template<class T>
XcharMap<T>::XcharMap()
: ptr_(0)
{
}

template<class T>
XcharMap<T>::XcharMap(T defaultValue)
: sharedMap_(new SharedXcharMap<T>(defaultValue))
{
  ptr_ = sharedMap_->ptr();
}

template<class T>
void XcharMap<T>::setRange(Char min, Char max, T val)
{
  if (min <= max) {
    do {
      ptr_[min] = val;
    } while (min++ != max);
  }
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not XcharMap_DEF_INCLUDED */
