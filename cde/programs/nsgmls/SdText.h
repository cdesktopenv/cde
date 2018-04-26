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
/* $XConsortium: SdText.h /main/1 1996/07/29 17:03:58 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifndef SdText_INCLUDED
#define SdText_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "types.h"
#include "StringOf.h"
#include "Vector.h"
#include "Location.h"
#include <stddef.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct SP_API SdTextItem {
  SdTextItem();
  Location loc;
  size_t index;
};

class SP_API SdText {
public:
  SdText();
  SdText(const Location &loc, Boolean lita);
  void swap(SdText &);
  void addChar(SyntaxChar, const Location &);
  const String<SyntaxChar> &string() const;
  Boolean lita() const;
  Location endDelimLocation() const;
private:
  Boolean lita_;
  String<SyntaxChar> chars_;
  Vector<SdTextItem> items_;
  friend class SdTextIter;
};

class SP_API SdTextIter {
public:
  SdTextIter(const SdText &);
  Boolean next(const SyntaxChar *&, size_t &, Location &);
private:
  const SdText *ptr_;
  size_t itemIndex_;
};

inline
Boolean SdText::lita() const
{
  return lita_;
}

inline
const String<SyntaxChar> &SdText::string() const
{
  return chars_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SdText_INCLUDED */
