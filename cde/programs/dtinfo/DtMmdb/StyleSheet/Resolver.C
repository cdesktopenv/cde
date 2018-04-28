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
// $XConsortium: Resolver.C /main/5 1996/10/08 19:26:17 cde-hal $
#include "Debug.h"
#include "Resolver.h"
#include "ResolverStack.h"
#include "PathTable.h"
#include "Renderer.h"
#include "Element.h"
#include "Feature.h"
#include "HardCopy/autoNumberFP.h"

extern const Element	       *gCurrentElement	;
extern const FeatureSet	       *gCurrentLocalSet;
extern const FeatureSet	       *gParentCompleteSet;

Resolver::Resolver(PathTable& pTable, Renderer& r)
: f_pathTable(pTable),
  f_Renderer(r),
  f_resolverStack()
{
  // have the Renderer install its default values as the bottom item on the
  // Stack  
  FeatureSet *default_features = f_Renderer.initialize();

  ResolverStackElement *default_element =
    new ResolverStackElement(new Element(gSymTab->intern("!"),0 , 0, 0),
			     0,
			     default_features);
			     
  f_resolverStack.push(default_element);
}

Resolver::~Resolver()
{
}
   
void
Resolver::Begin()
{
  // can do any node pre-processing here 
  f_Renderer.Begin();
}

void
Resolver::End()
{
  // can do any node post-processing here 
  f_Renderer.End();
  gAutoNumberFP.resetAllAutoNumbers();
}

unsigned int
Resolver::beginElement(Element *element)
{
//ON_DEBUG(element -> print(cerr));

  // get raw feature set 
  f_path.append(new PathTerm(*element));
  FeatureSet *rawLocalFeatureSet = f_pathTable.getFeatureSet(f_path);

  // get parent details 
  ResolverStackElement* parent = f_resolverStack.top();

// qifc:
// when rawLocalFeatureSet != 0 to call f_Renderer.preEvaluate is
// too restrictive. Comment out
//
  //if (rawLocalFeatureSet)
  gAutoNumberFP.beginElement(*element);

  FeatureSet *localFeatureSet = new FeatureSet;

  gCurrentElement	= element ;
  gCurrentLocalSet	= localFeatureSet ;
  gParentCompleteSet	= &parent->completeFeatureSet() ;

  //ON_DEBUG(cerr << "Path: " << f_path << endl);
  if (rawLocalFeatureSet)
    {
//     ON_DEBUG(cerr << "rawLocalFeatureSet: " << *rawLocalFeatureSet << endl);

      localFeatureSet = rawLocalFeatureSet->evaluate(localFeatureSet);

//      ON_DEBUG(cerr << "localFeatureSet: " << *localFeatureSet << endl);
    }

// qifc:
// when rawLocalFeatureSet != 0 to call f_Renderer.postEvaluate is
// too restrictive. Comment out
//
  //if (rawLocalFeatureSet)

  // need to create a complete feature set for current element
  // set is Sl U (Spc - Sl)
  FeatureSet *completeFeatureSet = new
    FeatureSet(parent->completeFeatureSet(), 
	       *localFeatureSet);

  // add current element to top of stack 
  f_resolverStack.push(
		       new ResolverStackElement(element,
						localFeatureSet, 
						completeFeatureSet)
		       ); 

  // tell renderer about new element 
  unsigned int ignore = f_Renderer.BeginElement(*element, *localFeatureSet,
						*completeFeatureSet, 
						parent->completeFeatureSet());

  if (ignore)
    {
      gAutoNumberFP.endElement(element->gi());

      // clean up stack and path 
      delete f_resolverStack.pop();
      delete f_path.removeLast();
    }

  return ignore;
}
   
void Resolver::data(const char* data, unsigned int data_length)
{
  // pass through 
  f_Renderer.data(data, data_length);
}
   
void Resolver::endElement(const Symbol& s)
{
  // NOTE: may want to pass top of stack to renderer for post element
  // processing?  

  gAutoNumberFP.endElement(s);

  f_Renderer.EndElement(s);	// pass through 
  delete f_resolverStack.pop();	// pop stack 
  delete f_path.removeLast();	// remove item from path 
}
