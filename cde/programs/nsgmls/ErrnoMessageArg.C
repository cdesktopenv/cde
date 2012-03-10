/* $XConsortium: ErrnoMessageArg.C /main/1 1996/07/29 16:50:58 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include "ErrnoMessageArg.h"
#include "StringOf.h"
#include "MessageBuilder.h"

#include <string.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

RTTI_DEF1(ErrnoMessageArg, OtherMessageArg)

MessageArg *ErrnoMessageArg::copy() const
{
  return new ErrnoMessageArg(*this);
}

#ifdef SP_NAMESPACE
}
#endif
