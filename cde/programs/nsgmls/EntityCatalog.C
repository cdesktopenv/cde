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
/* $XConsortium: EntityCatalog.C /main/1 1996/07/29 16:50:12 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "EntityCatalog.h"
#include "EntityDecl.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

EntityCatalog::~EntityCatalog()
{
}


Boolean EntityCatalog::sgmlDecl(const CharsetInfo &,
				Messenger &,
				StringC &) const
{
  return 0;
}

Boolean EntityCatalog::lookup(const EntityDecl &decl,
			      const Syntax &,
			      const CharsetInfo &,
			      Messenger &,
			      StringC &str) const
{
  const StringC *p = decl.systemIdPointer();
  if (!p)
    return 0;
  str = *p;
  return 1;
}

Boolean EntityCatalog::lookupPublic(const StringC &,
				    const CharsetInfo &,
				    Messenger &,
				    StringC &) const
{
  return 0;
}

Boolean EntityCatalog::defaultDoctype(const CharsetInfo &,
				      Messenger &,
				      StringC &,
				      StringC &) const
{
  return 0;
}


#ifdef SP_NAMESPACE
}
#endif
