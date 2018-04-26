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
/* $XConsortium: NumericCharRefOrigin.C /main/1 1996/07/29 16:58:56 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "NumericCharRefOrigin.h"
#include "Markup.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

NumericCharRefOrigin::NumericCharRefOrigin(const Location &start,
					   Index refLength,
					   Owner<Markup> &markup)
: start_(start), refLength_(refLength)
{
  markup.swap(markup_);
}

const Location &NumericCharRefOrigin::parent() const
{
  return start_;
}

Index NumericCharRefOrigin::refLength() const
{
  return refLength_;
}

Boolean NumericCharRefOrigin::isNumericCharRef(const Markup *&markup) const
{
  markup = markup_.pointer();
  return 1;
}


#ifdef SP_NAMESPACE
}
#endif
