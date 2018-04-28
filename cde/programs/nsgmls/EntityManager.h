/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
