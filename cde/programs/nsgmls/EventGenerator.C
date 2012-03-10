/* $XConsortium: EventGenerator.C /main/1 1996/07/29 16:51:25 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "splib.h"
#include "Boolean.h"
#include "EventGenerator.h"

EventGenerator::~EventGenerator()
{
}

void EventGenerator::inhibitMessages(bool)
{
}

EventGenerator *
EventGenerator::makeSubdocEventGenerator(const SGMLApplication::Char *,
					 size_t)
{
  return 0;
}
