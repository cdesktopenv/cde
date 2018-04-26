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
/* $XConsortium: ISetIter.h /main/1 1996/07/29 16:54:26 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ISetIter_INCLUDED
#define ISetIter_INCLUDED

#include <stddef.h>
#include "ISet.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class ISetIter {
public:
  ISetIter(const ISet<T> &s) : p_(&s), i_(0) { }
  // min and max are not changed if 0 is returned.
  int next(T &min, T &max)
  {
    if (i_ < p_->r_.size()) {
      min = p_->r_[i_].min;
      max = p_->r_[i_].max;
      i_++;
      return 1;
    }
    else
      return 0;
  }
       
private:
  const ISet<T> *p_;
  size_t i_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* ISetIter_INCLUDED */
