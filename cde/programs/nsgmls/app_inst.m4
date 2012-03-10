/* $XConsortium: app_inst.m4 /main/2 1996/08/09 15:32:58 mgreess $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#include "splib.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Vector.h"
#include "Owner.h"
#include "Options.h"
#undef SP_DEFINE_TEMPLATES

#include <stddef.h>
#include "CodingSystem.h"
#include "CmdLineApp.h"
#include "Event.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

__instantiate(Vector<const AppChar *>)
__instantiate(Owner<Encoder>)
__instantiate(Options<AppChar>)
__instantiate(Owner<EventHandler>)

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
