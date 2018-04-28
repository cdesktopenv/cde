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
/* $XConsortium: Param.h /main/1 1996/07/29 17:00:19 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Param_INCLUDED
#define Param_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Boolean.h"
#include "ContentToken.h"
#include "StringC.h"
#include "Location.h"
#include "MessageArg.h"
#include "Mode.h"
#include "NameToken.h"
#include "Owner.h"
#include "Ptr.h"
#include "Syntax.h"
#include "Text.h"
#include "Vector.h"

// This describes a markup declaration parameter.

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class ElementType;

class Param {
public:
  Param() : type(invalid), lita(false) { }
  typedef unsigned char Type;
  enum {
    invalid,
    dso,
    mdc,
    minus,
    pero,
    inclusions,
    exclusions,
    nameGroup,
    nameTokenGroup,
    modelGroup,
    number,
    minimumLiteral,
    attributeValueLiteral,
    tokenizedAttributeValueLiteral,
    systemIdentifier,
    paramLiteral,
    name,
    entityName,
    paramEntityName,
    attributeValue,
    reservedName,		// Syntax::ReservedName is added to this
    // this is a reserved name preceded by the RNI delimiter
    indicatedReservedName = reservedName + Syntax::nNames
    };
  enum { nTypes = indicatedReservedName + Syntax::nNames };
  Type type;
  Location startLocation;
  Text literalText;
  Boolean lita;
  Owner<ModelGroup> modelGroupPtr;
  Vector<NameToken> nameTokenVector;
  StringC token;		// name nameToken; with substitution
  Vector<const ElementType *> elementVector;
private:
  Param(const Param &);		// undefined
  void operator=(const Param &); // undefined
};

class AllowedParams {
public:
  AllowedParams(Param::Type,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid,
		Param::Type = Param::invalid);
  AllowedParams(const Param::Type *types, int nTypes);
  Mode mainMode() const;
  Boolean mdc() const;
  Boolean rni() const;
  Boolean dso() const;
  Boolean inclusions() const;
  Boolean exclusions() const;
  Boolean reservedName(Syntax::ReservedName) const;
  Param::Type group() const;
  Param::Type nameStart() const;
  Param::Type digit() const;
  Param::Type nmchar() const;
  Param::Type literal() const;
private:
  void init();
  void allow(Param::Type);
  PackedBoolean mdc_;
  PackedBoolean rni_;
  PackedBoolean dso_;
  PackedBoolean inclusions_;
  PackedBoolean exclusions_;
  // invalid, minus, pero
  Param::Type extraDelimiter_;
  // invalid, nameGroup, nameTokenGroup, modelGroup
  Param::Type group_;
  // invalid, reservedName, name, entityName, paramEntityName, attributeValue
  Param::Type nameStart_;
  // invalid, number, attributeValue
  Param::Type digit_;
  // invalid, attributeValue
  Param::Type nmchar_; // LCNMCHAR or UCNMCHAR
  // invalid, minimumLiteral, systemIdentifier, paramLiteral,
  // (tokenized)attributeValueLiteral
  Param::Type literal_;
  PackedBoolean reservedNames_[Syntax::nNames];
  Mode mainMode_;		// mdMode mdMinusMode mdPeroMode
};

class MessageBuilder;

class AllowedParamsMessageArg : public MessageArg {
public:
  AllowedParamsMessageArg(const AllowedParams &allow,
			  const ConstPtr<Syntax> &syntax);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  AllowedParams allow_;
  ConstPtr<Syntax> syntax_;
};

inline
Mode AllowedParams::mainMode() const
{
  return mainMode_;
}

inline
Boolean AllowedParams::mdc() const
{
  return mdc_;
}

inline
Boolean AllowedParams::rni() const
{
  return rni_;
}

inline
Boolean AllowedParams::dso() const
{
  return dso_;
}

inline
Boolean AllowedParams::inclusions() const
{
  return inclusions_;
}

inline
Boolean AllowedParams::exclusions() const
{
  return exclusions_;
}

inline
Boolean AllowedParams::reservedName(Syntax::ReservedName i) const
{
  return reservedNames_[i];
}

inline
Param::Type AllowedParams::group() const
{
  return group_;
}

inline
Param::Type AllowedParams::nameStart() const
{
  return nameStart_;
}

inline
Param::Type AllowedParams::digit() const
{
  return digit_;
}

inline
Param::Type AllowedParams::nmchar() const
{
  return nmchar_;
}

inline
Param::Type AllowedParams::literal() const
{
  return literal_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Param_INCLUDED */
