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
/* $XConsortium: ContentState.h /main/1 1996/07/29 16:48:27 cde-hp $ */
// Copyright (c) 1994, 1996 James Clark
// See the file COPYING for copying permission.

#ifndef ContentState_INCLUDED
#define ContentState_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stddef.h>
#include "OpenElement.h"
#include "IList.h"
#include "Vector.h"
#include "Message.h"
#include "Dtd.h"
#include "Mode.h"
#include "Boolean.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SP_API ContentState {
public:
  ContentState();
  void startContent(const Dtd &);
  void pushElement(OpenElement *);
  OpenElement *popSaveElement();
  void popElement();
  OpenElement &currentElement();
  const OpenElement &currentElement() const;
  void getOpenElementInfo(Vector<OpenElementInfo> &,
			  const StringC &rniPcdata) const;
  unsigned tagLevel() const;
  Boolean elementIsIncluded(const ElementType *) const;
  Boolean elementIsExcluded(const ElementType *) const;
  Boolean elementIsOpen(const ElementType *) const;
  Boolean afterDocumentElement() const;
  const ElementType *lastEndedElementType() const;
  Mode contentMode() const;
  const ElementType *lookupCreateUndefinedElement(const StringC &,
						  const Location &);
  Boolean checkImplyLoop(unsigned);
  static const ShortReferenceMap theEmptyMap;
private:
  IList<OpenElement> openElements_;
  Vector<unsigned> openElementCount_;
  Vector<unsigned> includeCount_;
  Vector<unsigned> excludeCount_;
  unsigned totalExcludeCount_;
  unsigned tagLevel_;
  unsigned netEnablingCount_;
  const ElementType *lastEndedElementType_;
  NamedTable<ElementType> undefinedElementTypeTable_;
  ElementType documentElementContainer_;
};

inline
OpenElement &ContentState::currentElement()
{
  return *openElements_.head();
}

inline
const OpenElement &ContentState::currentElement() const
{
  return *openElements_.head();
}

inline
Boolean ContentState::elementIsOpen(const ElementType *e) const
{
  return openElementCount_[e->index()] != 0;
}

inline
Boolean ContentState::elementIsIncluded(const ElementType *e) const
{
  return includeCount_[e->index()] != 0 && excludeCount_[e->index()] == 0;
}

inline
Boolean ContentState::elementIsExcluded(const ElementType *e) const
{
  return excludeCount_[e->index()] != 0;
}

inline
const ElementType *ContentState::lastEndedElementType() const
{
  return lastEndedElementType_;
}

inline
unsigned ContentState::tagLevel() const
{
  return tagLevel_;
}

inline
Boolean ContentState::afterDocumentElement() const
{
  return tagLevel() == 0 && currentElement().isFinished();
}

inline
Mode ContentState::contentMode() const
{
  return openElements_.head()->mode(netEnablingCount_ > 0);
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ContentState_INCLUDED */
