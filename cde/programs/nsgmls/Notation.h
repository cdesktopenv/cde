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
/* $XConsortium: Notation.h /main/1 1996/07/29 16:58:47 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Notation_INCLUDED
#define Notation_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Owner.h"
#include "StringC.h"
#include "NamedResource.h"
#include "types.h"
#include "Ptr.h"
#include "ExternalId.h"
#include "Boolean.h"
#include "Attributed.h"
#include "StringResource.h"
#include "EntityDecl.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class ParserState;

class SP_API Notation : public EntityDecl, public Attributed {
public:
  Notation(const StringC &,
	   const ConstPtr<StringResource<Char> > &dtdName,
	   Boolean dtdIsBase);
  void setExternalId(const ExternalId &, const Location &);
  const ExternalId &externalId() const;
  Boolean defined() const;
  void generateSystemId(ParserState &);
  const StringC *systemIdPointer() const;
  const StringC *publicIdPointer() const;
private:
  Notation(const Notation &);	// undefined
  void operator=(const Notation &); // undefined
  PackedBoolean defined_;
  ExternalId externalId_;
};

inline
const ExternalId &Notation::externalId() const
{
  return externalId_;
}

inline
Boolean Notation::defined() const
{
  return defined_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Notation_INCLUDED */
