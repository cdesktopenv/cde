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
/* $XConsortium: Notation.C /main/1 1996/07/29 16:58:43 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "Notation.h"
#include "ParserState.h"
#include "Sd.h"
#include "Syntax.h"
#include "MessageArg.h"
#include "ParserMessages.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Notation::Notation(const StringC &name,
		   const ConstPtr<StringResource<Char> > &dtdName,
		   Boolean dtdIsBase)
: EntityDecl(name, notation, ndata, Location()), defined_(0)
{
  setDeclIn(dtdName, dtdIsBase);
}

void Notation::setExternalId(const ExternalId &id, const Location &defLocation)
{
  externalId_ = id;
  defined_ = 1;
  setDefLocation(defLocation);
}

void Notation::generateSystemId(ParserState &parser)
{
  StringC str;
  if (parser.entityCatalog().lookup(*this,
				    parser.syntax(),
				    parser.sd().docCharset(),
				    parser.messenger(),
				    str))
    externalId_.setEffectiveSystem(str);
  else if (parser.options().warnNotationSystemId)
    parser.message(ParserMessages::cannotGenerateSystemIdNotation,
		   StringMessageArg(name()));
}

const StringC *Notation::systemIdPointer() const
{
  return externalId_.systemIdString();
}

const StringC *Notation::publicIdPointer() const
{
  return externalId_.publicIdString();
}


#ifdef SP_NAMESPACE
}
#endif
