/* $XConsortium: StringResource.h /main/1 1996/07/29 17:05:26 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef StringResource_INCLUDED
#define StringResource_INCLUDED 1

#include "StringOf.h"
#include "Resource.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

template<class T>
class StringResource : public String<T>, public Resource {
public:
  StringResource(const String<T> &s) : String<T>(s) { }
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not StringResource_INCLUDED */
