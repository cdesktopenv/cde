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
/* $XConsortium: EntityDecl.h /main/1 1996/07/29 16:50:29 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef EntityDecl_INCLUDED
#define EntityDecl_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedResource.h"
#include "Ptr.h"
#include "StringResource.h"
#include "Location.h"
#include "types.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API EntityDecl : public NamedResource {
public:
  enum DeclType { generalEntity, parameterEntity, doctype, linktype,
		  notation };
  enum DataType { sgmlText, pi, cdata, sdata, ndata, subdoc };
  EntityDecl(const StringC &, DeclType declType, DataType dataType,
	     const Location &defLocation);
  DataType dataType() const;
  DeclType declType() const;
  const Location &defLocation() const;
  Boolean declInDtdIsBase() const;
  Boolean declInActiveLpd() const;
  const StringC *declInDtdNamePointer() const;
  const StringC *declInLpdNamePointer() const;
  void setDeclIn(const ConstPtr<StringResource<Char> > &dtdName,
		 Boolean dtdIsBase,
		 const ConstPtr<StringResource<Char> > &lpdName,
		 Boolean lpdIsActive);
  void setDeclIn(const ConstPtr<StringResource<Char> > &dtdName,
		 Boolean dtdIsBase);
  void setDefLocation(const Location &);
  virtual const StringC *systemIdPointer() const;
  virtual const StringC *publicIdPointer() const;
  virtual const StringC *effectiveSystemIdPointer() const;
private:
  DeclType declType_;
  DataType dataType_;
  PackedBoolean dtdIsBase_;
  PackedBoolean lpdIsActive_;
  Location defLocation_;
  ConstPtr<StringResource<Char> > dtdName_;
  ConstPtr<StringResource<Char> > lpdName_;
};

inline
const Location &EntityDecl::defLocation() const
{
  return defLocation_;
}

inline
EntityDecl::DeclType EntityDecl::declType() const
{
  return declType_;
}

inline
EntityDecl::DataType EntityDecl::dataType() const
{
  return dataType_;
}

inline
const StringC *EntityDecl::declInDtdNamePointer() const
{
  return dtdName_.pointer();
}

inline
const StringC *EntityDecl::declInLpdNamePointer() const
{
  return lpdName_.pointer();
}

inline
Boolean EntityDecl::declInDtdIsBase() const
{
  return dtdIsBase_;
}

inline
Boolean EntityDecl::declInActiveLpd() const
{
  return lpdIsActive_;
}

inline
void EntityDecl::setDefLocation(const Location &loc)
{
  defLocation_ = loc;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EntityDecl_INCLUDED */
