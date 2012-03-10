/* $XConsortium: assert.C /main/1 1996/07/29 17:08:24 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"
#include <stdlib.h>
#include "macros.h"

#ifdef __GNUG__
void exit(int) __attribute__((noreturn));
#endif

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

void assertionFailed(const char *, const char *, int)
{
  abort();
  exit(1);
}

#ifdef SP_NAMESPACE
}
#endif
