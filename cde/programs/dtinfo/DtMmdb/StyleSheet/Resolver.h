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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: Resolver.h /main/4 1996/06/11 17:08:28 cde-hal $ */
#ifndef _Resolver_h
#define _Resolver_h

class Element;
class Symbol;
class FeatureSet;
class PathTable;
class Renderer;
class ResolverStack;

#include "SSPath.h"
#include "ResolverStack.h"
#include "Exceptions.hh"


/* **************************************************************

   the Resolver is responsible for taking input from the NodeParser,
   getting a feature set from the Style Sheet and passing it on to a
   Renderer 

   - get Element input from NodeParser
   - consult Style Sheet PathTable to get raw FeatureSet
   - evaluate feature set to resolve variables and expressions
   - merge with parent complete feature set to get complete feature
     set 
   - pass data to Renderer (Element, localFeatures, completeFeatures)

 * ************************************************************** */



class Resolver : public Destructable
{
public:
  Resolver(PathTable& pTable, Renderer& r);
  virtual ~Resolver();

  // beginElement returns a non-zero value if element is to be ignored
  unsigned int beginElement(Element*);
  void data(const char* data, unsigned int data_length);
  void endElement(const Symbol&);

  // called before any data 
  virtual void Begin();
  // called after all data 
  virtual void End();

private:
  SSPath		f_path ;
  PathTable	       &f_pathTable;

   // NOTE: this one could be a pointer so we can change them on the fly
  Renderer	       &f_Renderer;

  ResolverStack	        f_resolverStack;
};

#endif /* _Resolver_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
