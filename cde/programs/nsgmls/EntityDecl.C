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
/* $XConsortium: EntityDecl.C /main/1 1996/07/29 16:50:24 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "EntityDecl.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

EntityDecl::EntityDecl(const StringC &str, DeclType declType, DataType dataType,
		       const Location &defLocation)
: NamedResource(str), declType_(declType), dataType_(dataType),
  defLocation_(defLocation), dtdIsBase_(0), lpdIsActive_(0)
{
}

void EntityDecl::setDeclIn(const ConstPtr<StringResource<Char> > &dtdName,
			   Boolean dtdIsBase,
			   const ConstPtr<StringResource<Char> > &lpdName,
			   Boolean lpdIsActive)
{
  dtdName_ = dtdName;
  lpdName_ = lpdName;
  dtdIsBase_ = dtdIsBase;
  lpdIsActive_ = lpdIsActive;
}

void EntityDecl::setDeclIn(const ConstPtr<StringResource<Char> > &dtdName,
			   Boolean dtdIsBase)
{
  dtdName_ = dtdName;
  lpdName_.clear();
  dtdIsBase_ = dtdIsBase;
}

const StringC *EntityDecl::systemIdPointer() const
{
  return 0;
}

const StringC *EntityDecl::publicIdPointer() const
{
  return 0;
}

const StringC *EntityDecl::effectiveSystemIdPointer() const
{
  return 0;
}


#ifdef SP_NAMESPACE
}
#endif
