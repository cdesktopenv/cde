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
/* $XConsortium: SdText.C /main/1 1996/07/29 17:03:53 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "SdText.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

SdText::SdText()
: lita_(false)
{
}

SdText::SdText(const Location &loc, Boolean lita)
: lita_(lita)
{
  items_.resize(items_.size() + 1);
  items_.back().loc = loc;
  items_.back().index = 0;
}

void SdText::addChar(SyntaxChar c, const Location &loc)
{
  if (items_.size() == 0
      || loc.origin().pointer() != items_.back().loc.origin().pointer()
      || loc.index() != (items_.back().loc.index()
			 + (chars_.size() - items_.back().index))) {
    items_.resize(items_.size() + 1);
    items_.back().loc = loc;
    items_.back().index = chars_.size();
  }
  chars_ += c;
}

void SdText::swap(SdText &to)
{
  items_.swap(to.items_);
  chars_.swap(to.chars_);
  {
    Boolean tem = to.lita_;
    to.lita_ = lita_;
    lita_ = tem;
  }
}

Location SdText::endDelimLocation() const
{
  Location loc(items_.back().loc);
  loc += chars_.size() - items_.back().index;
  return loc;
}

SdTextItem::SdTextItem()
: index(0)
{
}

SdTextIter::SdTextIter(const SdText &text)
: ptr_(&text),
  itemIndex_(0)
{
}

Boolean SdTextIter::next(const SyntaxChar *&ptr, size_t &length, Location &loc)
{
  const Vector<SdTextItem> &items = ptr_->items_;
  if (itemIndex_ >= items.size())
    return 0;
  loc = items[itemIndex_].loc;
  const String<SyntaxChar> &chars = ptr_->chars_;
  size_t charsIndex = items[itemIndex_].index;
  ptr = chars.data() + charsIndex;
  if (itemIndex_ + 1 < items.size())
    length = items[itemIndex_ + 1].index - charsIndex;
  else
    length = chars.size() - charsIndex;
  itemIndex_++;
  return 1;
}

#ifdef SP_NAMESPACE
}
#endif
