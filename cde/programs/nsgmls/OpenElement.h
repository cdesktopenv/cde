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
/* $XConsortium: OpenElement.h /main/1 1996/07/29 16:59:20 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef OpenElement_INCLUDED
#define OpenElement_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Boolean.h"
#include "ContentToken.h"
#include "ElementType.h"
#include "Link.h"
#include "Mode.h"
#include "Allocator.h"
#include "Location.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API OpenElement : public Link {
public:
  void *operator new(size_t sz, Allocator &alloc) { return alloc.alloc(sz); }
  void *operator new(size_t sz) { return Allocator::allocSimple(sz); }
  void operator delete(void *p) { Allocator::free(p); }
  OpenElement(const ElementType *, Boolean net, Boolean included,
	      const ShortReferenceMap *currentMap,
	      const Location &startLocation);
  Boolean isFinished() const;
  Boolean tryTransition(const ElementType *);
  const LeafContentToken *invalidExclusion(const ElementType *) const;
  // This doesn't handle declared content of EMPTY.
  // If this situation can arise must use declaredEmpty().
  Boolean tryTransitionPcdata();
  const LeafContentToken *impliedStartTag() const;
  void doRequiredTransition();
  const ElementType *type() const;
  Boolean netEnabling() const;
  Boolean included() const;
  const MatchState &matchState() const;
  void setMatchState(const MatchState &);
  Mode mode(Boolean netEnabled) const;
  const ShortReferenceMap *map() const;
  void setMap(const ShortReferenceMap *);
  Boolean requiresSpecialParse() const;
  const Location &startLocation() const;
  const LeafContentToken *currentPosition() const;
  Boolean declaredEmpty() const;
  void setConref();
private:
  OpenElement(const OpenElement &); // undefined
  void operator=(const OpenElement &); // undefined
  const ElementType *elementType_;
  PackedBoolean netEnabling_;		// start-tag was net-enabling
  PackedBoolean included_;
  MatchState matchState_;
  ElementDefinition::DeclaredContent declaredContent_;
  const ShortReferenceMap *map_;
  Location startLocation_;
};

inline
const ElementType *OpenElement::type() const
{
  return elementType_;
}

inline 
Boolean OpenElement::netEnabling() const
{
  return netEnabling_;
}

inline
Boolean OpenElement::included() const
{
  return included_;
}

inline
const MatchState &OpenElement::matchState() const
{
  return matchState_;
}

inline
void OpenElement::setMatchState(const MatchState &state)
{
  matchState_ = state;
}

inline
Boolean OpenElement::isFinished() const
{
  return (declaredContent_ != ElementDefinition::modelGroup
	  || matchState_.isFinished());
}
  
inline
Boolean OpenElement::tryTransition(const ElementType *e)
{
  return (declaredContent_ == ElementDefinition::modelGroup
	  ? matchState_.tryTransition(e)
	  : (declaredContent_ == ElementDefinition::any));
}

inline
Boolean OpenElement::tryTransitionPcdata()
{
  return (declaredContent_ == ElementDefinition::modelGroup
	  ? matchState_.tryTransitionPcdata()
	  : 1);			// CDATA, RCDATA, ANY all ok
}

inline
const LeafContentToken *OpenElement::invalidExclusion(const ElementType *e)
     const
{
  return (declaredContent_ == ElementDefinition::modelGroup
	  ? matchState_.invalidExclusion(e)
	  : 0);
}

inline
void OpenElement::doRequiredTransition()
{
  matchState_.doRequiredTransition();
}

inline
const LeafContentToken *OpenElement::impliedStartTag() const
{
  return (declaredContent_ == ElementDefinition::modelGroup
	  ? matchState_.impliedStartTag()
	  : 0);
}

inline
const ShortReferenceMap *OpenElement::map() const
{
  return map_;
}

inline
void OpenElement::setMap(const ShortReferenceMap *map)
{
  map_ = map;
}

inline
Boolean OpenElement::requiresSpecialParse() const
{
  return (declaredContent_ == ElementDefinition::cdata
	  || declaredContent_ == ElementDefinition::rcdata);
}

inline
Mode OpenElement::mode(Boolean netEnabled) const
{
  return elementType_->definition()->mode(netEnabled);
}

inline
const Location &OpenElement::startLocation() const
{
  return startLocation_;
}

inline
const LeafContentToken *OpenElement::currentPosition() const
{
  return (declaredContent_ == ElementDefinition::modelGroup
	  ? matchState_.currentPosition()
	  : 0);
}

inline
Boolean OpenElement::declaredEmpty() const
{
  return declaredContent_ == ElementDefinition::empty;
}

inline
void OpenElement::setConref()
{
  declaredContent_ = ElementDefinition::empty;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not OpenElement_INCLUDED */
