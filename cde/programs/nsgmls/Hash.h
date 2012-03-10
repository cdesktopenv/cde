/* $XConsortium: Hash.h /main/1 1996/07/29 16:52:46 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Hash_INCLUDED
#define Hash_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "StringC.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifndef SP_API
#define SP_API /* as nothing */
#endif

class SP_API Hash {
public:
  static unsigned long hash(const StringC &);
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Hash_INCLUDED */
