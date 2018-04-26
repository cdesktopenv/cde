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
/* $XConsortium: ExtendEntityManager.h /main/1 1996/07/29 16:51:47 cde-hp $ */
// Copyright (c) 1994, 1995 James Clark
// See the file COPYING for copying permission.

#ifndef ExtendEntityManager_INCLUDED
#define ExtendEntityManager_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "EntityManager.h"
#include "CharsetInfo.h"
#include "types.h"
#include "Boolean.h"
#include "StringC.h"
#include "types.h"
#include "Vector.h"
#include "Location.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class StorageManager;
class InputCodingSystem;
class Messenger;

struct SP_API StorageObjectSpec {
  StorageObjectSpec();
  StorageManager *storageManager;
  const char *codingSystemName;
  const InputCodingSystem *codingSystem;
  StringC specId;		// specified id
  StringC baseId;		// id that specified id is relative to
  StringC id;			// actual id used (filled in after opening)
  enum Records {
    find,
    cr,
    lf,
    crlf,
    asis
    };
  Records records;
  PackedBoolean notrack;
  PackedBoolean zapEof;		// zap a final Ctrl-Z
  PackedBoolean search;
};

struct SP_API ParsedSystemIdMap {
  enum Type {
    catalogDocument,
    catalogPublic
  };
  Type type;
  StringC publicId;
};

struct SP_API ParsedSystemId : public Vector<StorageObjectSpec> {
  ParsedSystemId();
  void unparse(const CharsetInfo &charset, StringC &result) const;
  Vector<ParsedSystemIdMap> maps;
};

struct SP_API StorageObjectLocation {
  const StorageObjectSpec *storageObjectSpec;
  unsigned long lineNumber;
  unsigned long columnNumber;
  unsigned long byteIndex;
  unsigned long storageObjectOffset;
};

class SP_API ExtendEntityManager : public EntityManager {
public:
  class SP_API CatalogManager {
  public:
    virtual ~CatalogManager();
    virtual ConstPtr<EntityCatalog>
      makeCatalog(StringC &systemId,
		  const CharsetInfo &charset,
		  ExtendEntityManager *,
		  Messenger &) const = 0;
    virtual Boolean mapCatalog(ParsedSystemId &systemId,
			       ExtendEntityManager *em,
			       Messenger &mgr) const = 0;
  };
  virtual void registerStorageManager(StorageManager *) = 0;
  virtual void registerCodingSystem(const char *, const InputCodingSystem *)
    = 0;
  virtual void setCatalogManager(CatalogManager *) = 0;
  virtual InputSource *openIfExists(const StringC &sysid,
				    const CharsetInfo &,
				    InputSourceOrigin *,
				    Boolean mayRewind,
				    Messenger &) = 0;
  virtual Boolean expandSystemId(const StringC &,
				 const Location &,
				 Boolean isNdata,
				 const CharsetInfo &,
				 const StringC *mapCatalogPublic,
				 Messenger &,
				 StringC &) = 0;
  virtual Boolean mergeSystemIds(const Vector<StringC> &sysids,
				 Boolean mapCatalogDocument,
				 const CharsetInfo &,
				 Messenger &mgr,
				 StringC &) const = 0;
  virtual Boolean parseSystemId(const StringC &str,
				const CharsetInfo &idCharset,
				Boolean isNdata,
				const StorageObjectSpec *defSpec,
				Messenger &mgr,
				ParsedSystemId &parsedSysid) const = 0;
  static Boolean externalize(const ExternalInfo *,
			     Offset,
			     StorageObjectLocation &);
  static const ParsedSystemId *
    externalInfoParsedSystemId(const ExternalInfo *);
  static ExtendEntityManager *make(StorageManager *,
				   const InputCodingSystem *);
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ExtendEntityManager_INCLUDED */
