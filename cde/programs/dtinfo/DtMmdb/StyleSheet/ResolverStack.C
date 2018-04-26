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
// $XConsortium: ResolverStack.cc /main/3 1996/06/11 17:08:33 cde-hal $
#include "ResolverStack.h"

#include "Element.h"
#include "Feature.h"


// we have the responsibility of deleting these guys

ResolverStackElement::ResolverStackElement(Element	*element,
					   FeatureSet	*local,
					   FeatureSet	*complete)
: f_element(element),
  f_localFeatureSet(local),
  f_completeFeatureSet(complete)
{
}
  

ResolverStackElement::~ResolverStackElement()
{
  delete f_element;
  delete f_localFeatureSet;
  delete f_completeFeatureSet ;
}

// required by CC_TPtrDlist 
int
ResolverStackElement::operator==(const ResolverStackElement &stack_element)
{
  return
    stack_element.f_element		== f_element &&
    stack_element.f_localFeatureSet	== f_localFeatureSet &&
    stack_element.f_completeFeatureSet	== f_completeFeatureSet ;
}


ResolverStack::ResolverStack()
: CC_TPtrDlist<ResolverStackElement>()
{
}

ResolverStack::~ResolverStack()
{
  // delete all remaining items in stack 
  clearAndDestroy();
}

void
ResolverStack::push(ResolverStackElement *item)
{
  prepend(item);
}

ResolverStackElement *
ResolverStack::top()
{
  return first();
}

ResolverStackElement *
ResolverStack::pop()
{
  return removeFirst();
}
