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
/* $XConsortium: ElementType.C /main/1 1996/07/29 16:49:35 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif
#include "splib.h"
#include "ElementType.h"
#include "ContentToken.h"
#include "macros.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

ElementType::ElementType(const StringC &name, size_t index)
: Named(name), index_(index), map_(0), defIndex_(0)
{
}

ElementDefinition::ElementDefinition(const Location &location,
				     size_t index,
				     unsigned char omitFlags,
				     DeclaredContent declaredContent)
: location_(location),
  index_(index),
  omitFlags_(omitFlags),
  declaredContent_(declaredContent)
{
  computeMode();
}

ElementDefinition::ElementDefinition(const Location &location,
				     size_t index,
				     unsigned char omitFlags,
				     DeclaredContent declaredContent,
				     Owner<CompiledModelGroup> &modelGroup)
: location_(location),
  index_(index),
  omitFlags_(omitFlags),
  declaredContent_(declaredContent),
  modelGroup_(modelGroup.extract())
{
  computeMode();
}

void ElementDefinition::computeMode()
{
  switch (declaredContent_) {
  case modelGroup:
    if (!modelGroup_->containsPcdata()) {
      netMode_ = econnetMode;
      mode_ = econMode;
      break;
    }
    // fall through
  case any:
    netMode_ = mconnetMode;
    mode_ = mconMode;
    break;
  case cdata:
    netMode_ = cconnetMode;
    mode_ = cconMode;
    break;
  case rcdata:
    netMode_ = rcconnetMode;
    mode_ = rcconMode;
    break;
  case empty:
    break;
  default:
    CANNOT_HAPPEN();
  }
}

void ElementType::swap(ElementType &to)
{
  Named::swap(to);
  {
    size_t tem = to.index_;
    to.index_ = index_;
    index_ = tem;
  }
  {
    size_t tem = to.defIndex_;
    to.defIndex_ = defIndex_;
    defIndex_ = tem;
  }
  def_.swap(to.def_);
  {
    const ShortReferenceMap *tem = to.map_;
    to.map_ = map_;
    map_ = tem;
  }
}

RankStem::RankStem(const StringC &name, size_t index)
: Named(name), index_(index)
{
}

void RankStem::addDefinition(const ConstPtr<ElementDefinition> &p)
{
  def_.push_back(p);
}


#ifdef SP_NAMESPACE
}
#endif
