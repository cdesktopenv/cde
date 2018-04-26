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
/* $XConsortium: IQueue.h /main/1 1996/07/29 16:53:51 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef IQueue_INCLUDED
#define IQueue_INCLUDED 1

#include "Boolean.h"
#include "Link.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class IQueueBase {
public:
  IQueueBase() : last_(0) { }
  ~IQueueBase() { }
  Boolean empty() const { return last_ == 0; }
  Link *get() {
    Link *tem = last_->next_;
    if (tem == last_)
      last_ = 0;
    else
      last_->next_ = tem->next_;
    return tem;
  }
  void append(Link *p) {
    if (last_) {
      p->next_ = last_->next_;
      last_ = last_->next_ = p;
    }
    else
      last_ = p->next_ = p;
  }
  void swap(IQueueBase &with) {
    Link *tem = last_;
    last_ = with.last_;
    with.last_ = tem;
  }
private:
  Link *last_;

};

template<class T>
class IQueue : private IQueueBase {
public:
  IQueue() { }
  ~IQueue() { clear(); }
  void clear();
  T *get() { return (T *)IQueueBase::get(); }
  void append(T *p) { IQueueBase::append(p); }
  Boolean empty() const { return IQueueBase::empty(); }
  void swap(IQueue<T> &to) { IQueueBase::swap(to); }
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not IQueue_INCLUDED */

#ifdef SP_DEFINE_TEMPLATES
#include "IQueue.C"
#endif
