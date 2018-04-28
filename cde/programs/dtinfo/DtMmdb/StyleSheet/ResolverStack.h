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
/* $XConsortium: ResolverStack.h /main/4 1996/08/21 15:50:41 drk $ */
#ifndef _ResolverStack_h
#define _ResolverStack_h

#ifndef CDE_NEXT

#else
//#include <StyleSheet/cde_next.h>
#include "dti_cc/CC_Dlist.h"
#endif

class Element ;
class FeatureSet ;

class ResolverStackElement 
{
public:
  // note, this object will delete these items in its destructor
  ResolverStackElement(Element*,
		       FeatureSet *local,
		       FeatureSet *complete);

  ~ResolverStackElement();

  int
  operator==(const ResolverStackElement &);

  // only pass out references because we own these guys 
  FeatureSet &completeFeatureSet()	{ return *f_completeFeatureSet ; }
  FeatureSet &localFeatureSet()		{ return *f_localFeatureSet ; }
  Element    &element()			{ return *f_element ; }
  
private:
  Element      *f_element;
  FeatureSet   *f_localFeatureSet;
  FeatureSet   *f_completeFeatureSet;
};

class ResolverStack : private CC_TPtrDlist<ResolverStackElement>
{

public:
  ResolverStack();
  ~ResolverStack();

  void push(ResolverStackElement*);
  ResolverStackElement* pop();
  ResolverStackElement* top();
};


#endif /* _ResolverStack_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
