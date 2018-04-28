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
/* $XConsortium: SgmlParser.h /main/1 1996/07/29 17:04:16 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef SgmlParser_INCLUDED
#define SgmlParser_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringC.h"
#include "Ptr.h"
#include "Location.h"
#include "EntityManager.h"

#include <signal.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Event;
class Parser;
class UnivCharsetDesc;
class EventHandler;
struct ParserOptions;
template<class T> class Ptr;
template<class T> class ConstPtr;
class InputSourceOrigin;
class Sd;
class Syntax;
class Dtd;

class SP_API SgmlParser {
public:
  struct SP_API Params {
    enum EntityType {
      document,
      subdoc,
      dtd
    };
    Params();
    EntityType entityType;	// defaults to document
    StringC sysid;		// must be specified
    Ptr<InputSourceOrigin> origin;
    Ptr<EntityManager> entityManager;
    const UnivCharsetDesc *initialCharset;
    const SgmlParser *parent;
    ConstPtr<Sd> sd;
    ConstPtr<Syntax> prologSyntax;
    ConstPtr<Syntax> instanceSyntax;
    unsigned subdocLevel;
    const ParserOptions *options;
    PackedBoolean subdocInheritActiveLinkTypes;
    // referenced subdocs count against SUBDOC limit in SGML declaration
    PackedBoolean subdocReferenced;
    StringC doctypeName;
  };
  SgmlParser();			// must call init
  SgmlParser(const Params &params);
  void init(const Params &params);
  ~SgmlParser();
  Event *nextEvent();
  void parseAll(EventHandler &,
		SP_CONST SP_VOLATILE sig_atomic_t *cancelPtr = 0);
  ConstPtr<Sd> sd() const;
  ConstPtr<Syntax> instanceSyntax() const;
  ConstPtr<Syntax> prologSyntax() const;
  const EntityManager &entityManager() const;
  const EntityCatalog &entityCatalog() const;
  const ParserOptions &options() const;
  // Only to be called after the parse has ended.
  Ptr<Dtd> baseDtd();
  void activateLinkType(const StringC &);
  void allLinkTypesActivated();
  void swap(SgmlParser &);
  friend class Parser;
private:
  SgmlParser(const SgmlParser &);
  void operator=(const SgmlParser &);
  Parser *parser_;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SgmlParser_INCLUDED */
