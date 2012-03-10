/* $XConsortium: TranslateInputCodingSystem.h /main/1 1996/07/29 17:06:33 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef TranslateInputCodingSystem_INCLUDED
#define TranslateInputCodingSystem_INCLUDED 1

#include "CodingSystem.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API TranslateInputCodingSystem : public InputCodingSystem {
public:
  TranslateInputCodingSystem(const Char *table);
  Decoder *makeDecoder() const;
private:
  const Char *table_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not TranslateInputCodingSystem_INCLUDED */
