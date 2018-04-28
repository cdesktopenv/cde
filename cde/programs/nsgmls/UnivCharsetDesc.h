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
/* $XConsortium: UnivCharsetDesc.h /main/1 1996/07/29 17:07:49 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef UnivCharsetDesc_INCLUDED
#define UnivCharsetDesc_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include <stddef.h>
#include "types.h"
#include "RangeMap.h"
#include "Boolean.h"
#include "ISet.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifndef SP_API
#define SP_API
#endif

class SP_API UnivCharsetDesc {
public:
  struct SP_API Range {
    WideChar descMin;
    // Note that this is a count, as in the SGML declaration,
    // rather than a maximum.
    unsigned long count;
    UnivChar univMin;
  };
  enum {
    zero = 48,
    A = 65,
    a = 97,
    tab = 9,
    rs = 10,
    re = 13,
    space = 32,
    exclamation = 33,
    lessThan = 60,
    greaterThan = 62
    };
  UnivCharsetDesc();
  UnivCharsetDesc(const Range *, size_t);
  void set(const Range *, size_t);
  Boolean descToUniv(WideChar from, UnivChar &to) const;
  Boolean descToUniv(WideChar from, UnivChar &to, WideChar &alsoMax) const;
  // Return 0 for no matches, 1 for 1, 2 for more than 1
  unsigned univToDesc(UnivChar from, WideChar &to, ISet<WideChar> &toSet)
       const;
  unsigned univToDesc(UnivChar from, WideChar &to, ISet<WideChar> &toSet,
		      WideChar &count)
       const;
  void addRange(WideChar descMin, WideChar descMax, UnivChar univMin);
  void addBaseRange(const UnivCharsetDesc &baseSet,
		    WideChar descMin,
		    WideChar descMax,
		    WideChar baseMin,
		    ISet<WideChar> &baseMissing);
  WideChar maxDesc() const;
private:
  RangeMap<WideChar,UnivChar> descToUniv_;
  friend class UnivCharsetDescIter;
};

class SP_API UnivCharsetDescIter {
public:
  UnivCharsetDescIter(const UnivCharsetDesc &);
  Boolean next(WideChar &descMin, WideChar &descMax, UnivChar &univMin);
private:
  RangeMapIter<WideChar,UnivChar> iter_;
};

inline
Boolean UnivCharsetDesc::descToUniv(WideChar from, UnivChar &to) const
{
  WideChar tem;
  return descToUniv_.map(from, to, tem);
}

inline
Boolean UnivCharsetDesc::descToUniv(WideChar from, UnivChar &to,
				    WideChar &alsoMax) const
{
  return descToUniv_.map(from, to, alsoMax);
}

inline
unsigned UnivCharsetDesc::univToDesc(UnivChar from, WideChar &to,
				     ISet<WideChar> &toSet) const
{
  WideChar tem;
  return descToUniv_.inverseMap(from, to, toSet, tem);
}

inline
unsigned UnivCharsetDesc::univToDesc(UnivChar from, WideChar &to,
				     ISet<WideChar> &toSet,
				     WideChar &count) const
{
  return descToUniv_.inverseMap(from, to, toSet, count);
}

inline
void UnivCharsetDesc::addRange(WideChar descMin,
			       WideChar descMax,
			       UnivChar univMin)
{
  descToUniv_.addRange(descMin, descMax, univMin);
}

inline
UnivCharsetDescIter::UnivCharsetDescIter(const UnivCharsetDesc &desc)
: iter_(desc.descToUniv_)
{
}

inline
Boolean UnivCharsetDescIter::next(WideChar &descMin,
				  WideChar &descMax,
				  UnivChar &univMin)
{
  return iter_.next(descMin, descMax, univMin);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not UnivCharsetDesc_INCLUDED */
