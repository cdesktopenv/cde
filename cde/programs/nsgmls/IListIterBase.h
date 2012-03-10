/* $XConsortium: IListIterBase.h /main/1 1996/07/29 16:53:38 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef IListIterBase_INCLUDED
#define IListIterBase_INCLUDED 1

#include "Link.h"
#include "IListBase.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API IListIterBase {
public:
  IListIterBase(const IListBase &);
  int done();
  Link *cur();
  void next();
private:
  Link *p_;
};

inline
IListIterBase::IListIterBase(const IListBase &list) : p_(list.head_)
{
}

inline
int IListIterBase::done()
{
  return p_ == 0;
}

inline
Link *IListIterBase::cur()
{
  return p_;
}

inline
void IListIterBase::next()
{
  p_ = p_->next_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not IListIterBase_INCLUDED */
