/* $XConsortium: Fixed2CodingSystem.h /main/1 1996/07/29 16:52:10 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Fixed2CodingSystem_INCLUDED
#define Fixed2CodingSystem_INCLUDED 1

#include "CodingSystem.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API Fixed2CodingSystem : public CodingSystem {
public:
  Decoder *makeDecoder() const;
  Encoder *makeEncoder() const;
  unsigned fixedBytesPerChar() const;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Fixed2CodingSystem_INCLUDED */
