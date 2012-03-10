/* $XConsortium: EntityManager.h /main/1 1996/07/29 16:50:39 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef EntityManager_INCLUDED
#define EntityManager_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "StringC.h"
#include "Resource.h"
#include "EntityCatalog.h"
#include "Ptr.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;
class InputSourceOrigin;
class CharsetInfo;
class InputSource;

class SP_API EntityManager : public Resource {
public:
  virtual ~EntityManager();
  virtual InputSource *open(const StringC &sysid,
			    const CharsetInfo &,
			    InputSourceOrigin *,
			    Boolean mayRewind,
			    Messenger &) = 0;
  // Make a catalog for a document or subdocument with specified
  // system identifier.
  // The catalog can cause the system identifier to be replaced.
  virtual ConstPtr<EntityCatalog>
    makeCatalog(StringC &systemId, const CharsetInfo &, Messenger &) = 0;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EntityManager_INCLUDED */
