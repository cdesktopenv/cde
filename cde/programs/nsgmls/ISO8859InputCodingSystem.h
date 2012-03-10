/* $XConsortium: ISO8859InputCodingSystem.h /main/1 1996/07/29 16:54:04 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef ISO8859InputCodingSystem_INCLUDED
#define ISO8859InputCodingSystem_INCLUDED 1

#include "TranslateInputCodingSystem.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ISO8859InputCodingSystem : public TranslateInputCodingSystem {
public:
  // part must be between 2 and 9
  ISO8859InputCodingSystem(int part);
private:
  const Char *partMap(int);
  static const Char maps[8][256];
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ISO8859InputCodingSystem_INCLUDED */
