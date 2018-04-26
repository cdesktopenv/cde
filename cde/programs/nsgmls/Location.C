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
/* $XConsortium: Location.C /main/1 1996/07/29 16:56:17 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "Location.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Location::Location()
: index_(0)
{
}

Location::Location(Origin *origin, Index i)
: origin_(origin), index_(i)
{
}

Location::Location(ConstPtr<Origin> origin, Index i)
: origin_(origin), index_(i)
{
}

Origin::~Origin()
{
}

const EntityOrigin *Origin::asEntityOrigin() const
{
  return 0;
}

const InputSourceOrigin *Origin::asInputSourceOrigin() const
{
  return 0;
}

Index Origin::refLength() const
{
  return 0;
}

Boolean Origin::origChars(const Char *&) const
{
  return 0;
}

Boolean Origin::inBracketedTextOpenDelim() const
{
  return 0;
}

Boolean Origin::inBracketedTextCloseDelim() const
{
  return 0;
}

Boolean Origin::isNumericCharRef(const Markup *&) const
{
  return 0;
}

Boolean Origin::isNamedCharRef(Index, NamedCharRef &) const
{
  return 0;
}

const EntityDecl *Origin::entityDecl() const
{
  return 0;
}

BracketOrigin::BracketOrigin(const Location &loc, Position pos)
: loc_(loc), pos_(pos)
{
}

const Location &BracketOrigin::parent() const
{
  return loc_;
}

Boolean BracketOrigin::inBracketedTextOpenDelim() const
{
  return pos_ == open;
}

Boolean BracketOrigin::inBracketedTextCloseDelim() const
{
  return pos_ == close;
}

InputSourceOrigin::InputSourceOrigin()
{
}

InputSourceOrigin::InputSourceOrigin(const Location &refLocation)
: refLocation_(refLocation)
{
}

const InputSourceOrigin *InputSourceOrigin::asInputSourceOrigin() const
{
  return this;
}

Boolean InputSourceOrigin::defLocation(Offset, Location &) const
{
  return 0;
}

const StringC *InputSourceOrigin::entityName() const
{
  return 0;
}

InputSourceOrigin *InputSourceOrigin::copy() const
{
  return new InputSourceOrigin(refLocation_);
}

const Location &InputSourceOrigin::parent() const
{
  return refLocation_;
}

void InputSourceOrigin::setExternalInfo(ExternalInfo *info)
{
  externalInfo_ = info;
}

void InputSourceOrigin::noteCharRef(Index replacementIndex,
			       const NamedCharRef &ref)
{
  charRefs_.resize(charRefs_.size() + 1);
  charRefs_.back().replacementIndex = replacementIndex;
  charRefs_.back().refStartIndex = ref.refStartIndex();
  charRefs_.back().refEndType = ref.refEndType();
  charRefs_.back().origNameOffset = charRefOrigNames_.size();
  charRefOrigNames_ += ref.origName();
}

// Number of character references whose replacement index < ind.

size_t InputSourceOrigin::nPrecedingCharRefs(Index ind) const
{
  size_t i;
  // Find i such that
  // charRefs_[I].replacementIndex >= ind
  // charRefs_[i - 1].replacementIndex < ind
  if (charRefs_.size() == 0
      || ind > charRefs_.back().replacementIndex)
    // This will be a common case, so optimize it.
    i = charRefs_.size();
  else {
    // Binary search
    // Invariant:
    // charRefs_ < i have replacementIndex < ind
    // charRefs_ >= lim have replacementIndex >= ind
    i = 0;
    size_t lim = charRefs_.size();
    while (i < lim) {
      size_t mid = i + (lim - i)/2;
      if (charRefs_[mid].replacementIndex >= ind)
	lim = mid;
      else
	i = mid + 1;
    }
  }
  return i;
}

Offset InputSourceOrigin::startOffset(Index ind) const
{
  size_t n = nPrecedingCharRefs(ind);
  if (n < charRefs_.size()
      && ind == charRefs_[n].replacementIndex) {
    for (;;) {
      ind = charRefs_[n].refStartIndex;
      if (n == 0 || charRefs_[n - 1].replacementIndex != ind)
	break;
      --n;
    }
  }
  // charRefs[n - 1].replacementIndex < ind
  return Offset(ind - n);
}

Boolean InputSourceOrigin::isNamedCharRef(Index ind, NamedCharRef &ref) const
{
  size_t n = nPrecedingCharRefs(ind);
  if (n < charRefs_.size() && ind == charRefs_[n].replacementIndex) {
    ref.set(charRefs_[n].refStartIndex,
	    charRefs_[n].refEndType,
	    charRefOrigNames_.data() + charRefs_[n].origNameOffset,
	    (n + 1 < charRefs_.size()
	     ? charRefs_[n + 1].origNameOffset
	     : charRefOrigNames_.size())
	    - charRefs_[n].origNameOffset);
    return 1;
  }
  return 0;
}

ReplacementOrigin::ReplacementOrigin(const Location &loc, Char origChar)
: loc_(loc), origChar_(origChar)
{
}

const Location &ReplacementOrigin::parent() const
{
  return loc_;
}

Boolean ReplacementOrigin::origChars(const Char *&s) const
{
  if (loc_.origin().isNull() || !loc_.origin()->origChars(s))
    s = &origChar_;
  return 1;
}

MultiReplacementOrigin::MultiReplacementOrigin(const Location &loc,
					       StringC &origChars)
: loc_(loc)
{
  origChars.swap(origChars_);
}

const Location &MultiReplacementOrigin::parent() const
{
  return loc_;
}

Boolean MultiReplacementOrigin::origChars(const Char *&s) const
{
  if (loc_.origin().isNull() || !loc_.origin()->origChars(s))
    s = origChars_.data();
  return 1;
}

ExternalInfo::~ExternalInfo()
{
}

RTTI_DEF0(ExternalInfo)

NamedCharRef::NamedCharRef()
: refStartIndex_(0), refEndType_(endOmitted)
{
}

NamedCharRef::NamedCharRef(Index refStartIndex, RefEndType refEndType,
			   const StringC &origName)
: refStartIndex_(refStartIndex),
  refEndType_(refEndType),
  origName_(origName)
{
}

void NamedCharRef::set(Index refStartIndex, RefEndType refEndType,
		       const Char *s, size_t n)
{
  refStartIndex_ = refStartIndex;
  refEndType_ = refEndType;
  origName_.assign(s, n);
}

#ifdef SP_NAMESPACE
}
#endif
