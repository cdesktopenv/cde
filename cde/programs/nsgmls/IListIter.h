/* $XConsortium: IListIter.h /main/1 1996/07/29 16:53:32 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef IListIter_INCLUDED
#define IListIter_INCLUDED 1

#include "IListIterBase.h"
#include "IList.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class IListIter : private IListIterBase {
public:
  IListIter(const IList<T> &list) : IListIterBase(list) { }
  T *cur() { return (T *)IListIterBase::cur(); }
  
  IListIterBase::next;
  IListIterBase::done;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not IListIter_INCLUDED */
