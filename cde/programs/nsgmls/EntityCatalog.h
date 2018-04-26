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
/* $XConsortium: EntityCatalog.h /main/1 1996/07/29 16:50:18 cde-hp $ */
#ifndef EntityCatalog_INCLUDED
#define EntityCatalog_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "StringC.h"
#include "Resource.h"
#include "SubstTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;
class CharsetInfo;
class EntityDecl;

class SP_API EntityCatalog : public Resource {
public:
  class SP_API Syntax {
  public:
    virtual Boolean namecaseGeneral() const = 0;
    virtual Boolean namecaseEntity() const = 0;
    virtual const SubstTable<Char> &upperSubstTable() const = 0;
    virtual const StringC &peroDelim() const = 0;
  };
  virtual ~EntityCatalog();
  virtual Boolean sgmlDecl(const CharsetInfo &,
			   Messenger &,
			   StringC &) const;
  virtual Boolean lookup(const EntityDecl &,
			 const Syntax &,
			 const CharsetInfo &,
			 Messenger &,
			 StringC &) const;
  virtual Boolean lookupPublic(const StringC &,
			       const CharsetInfo &,
			       Messenger &,
			       StringC &) const;
  virtual Boolean defaultDoctype(const CharsetInfo &,
				 Messenger &,
				 StringC &,
				 StringC &) const;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EntityCatalog_INCLUDED */
