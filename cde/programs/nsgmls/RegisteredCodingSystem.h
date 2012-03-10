/* $XConsortium: RegisteredCodingSystem.h /main/1 1996/07/29 17:02:50 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef RegisteredCodingSystem_INCLUDED
#define RegisteredCodingSystem_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class InputCodingSystem;

struct RegisteredCodingSystem {
  RegisteredCodingSystem() { }
  ~RegisteredCodingSystem() { }
  const char *name;
  const InputCodingSystem *ics;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not RegisteredCodingSystem_INCLUDED */
