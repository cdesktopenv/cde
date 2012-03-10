/* $XConsortium: StringVectorMessageArg.h /main/1 1996/07/29 17:05:44 cde-hp $ */
// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef StringVectorMessageArg_INCLUDED
#define StringVectorMessageArg_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MessageArg.h"
#include "StringC.h"
#include "Vector.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class StringVectorMessageArg : public MessageArg {
public:
  StringVectorMessageArg(const Vector<StringC> &);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  Vector<StringC> v_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not StringVectorMessageArg_INCLUDED */
