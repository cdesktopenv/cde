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
/* $XConsortium: UnivCharsetDesc.C /main/1 1996/07/29 17:07:43 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "UnivCharsetDesc.h"
#include "macros.h"
#include "constant.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

UnivCharsetDesc::UnivCharsetDesc()
{
}

UnivCharsetDesc::UnivCharsetDesc(const Range *p, size_t n)
{
  set(p, n);
}

void UnivCharsetDesc::set(const Range *p, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    const Range &r = p[i];
    WideChar max;
    if (r.count > wideCharMax || r.descMin > wideCharMax - r.count)
      max = wideCharMax;
    else
      max = r.descMin + (r.count - 1);
    if (max - r.descMin > univCharMax
	|| r.univMin > univCharMax - (max - r.descMin))
      max = r.descMin + (univCharMax - r.univMin);
    descToUniv_.addRange(r.descMin, max, r.univMin);
  }
}

void UnivCharsetDesc::addBaseRange(const UnivCharsetDesc &baseSet,
				   WideChar descMin,
				   WideChar descMax,
				   WideChar baseMin,
				   ISet<WideChar> &baseMissing)
{
  UnivCharsetDescIter iter(baseSet);
  WideChar baseMax = baseMin + (descMax - descMin);
  WideChar iDescMin, iDescMax;
  UnivChar iBaseMin;
  WideChar missingBaseMin = baseMin;
  Boolean usedAll = 0;
  while (iter.next(iDescMin, iDescMax, iBaseMin) && iDescMin <= baseMax) {
    //  baseMin   baseMax
    //          iDescMin iDescMax
    if (iDescMax >= baseMin) {
      WideChar min = baseMin > iDescMin ? baseMin : iDescMin;
      if (min > missingBaseMin)
	baseMissing.addRange(missingBaseMin, min - 1);
      WideChar max = baseMax < iDescMax ? baseMax : iDescMax;
      missingBaseMin = max + 1;
      if (missingBaseMin == 0)
	usedAll = 1;
      ASSERT(min <= max);
      descToUniv_.addRange(descMin + (min - baseMin),
			   descMin + (max - baseMin),
			   iBaseMin + (min - iDescMin));
    }
  }
  if (!usedAll && baseMax >= missingBaseMin)
    baseMissing.addRange(missingBaseMin, baseMax);
}

#ifdef SP_NAMESPACE
}
#endif
