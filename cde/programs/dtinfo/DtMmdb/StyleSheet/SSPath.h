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
/* $XConsortium: SSPath.h /main/4 1996/08/21 15:50:49 drk $ */
#ifndef _Path_h
#define _Path_h

#include "SymTab.h"
#include "Element.h"
#include "PathQualifier.h"

#ifndef CDE_NEXT


#else
#include "dti_cc/CC_Dlist.h"
#include "dti_cc/cc_vvect.h"
#endif

// path term object class
// each term is made up of a SGMLGI and an optional
// expression.
class PathTerm
{
public:
   PathTerm(const Element&);
   PathTerm(const Symbol&, PQExpr* = 0);
   PathTerm(const char* symbol, PQExpr* = 0);
   ~PathTerm();

   unsigned int operator ==(const PathTerm&);

   const Element& element() { return f_element; };
   const Symbol& symbol() { return f_element.gi(); };
   PQExpr* pqexpr() { return f_PQExpr; };

   friend ostream& operator <<(ostream&, PathTerm&);

private:
   const Element f_element;
   PQExpr* f_PQExpr;
};

typedef PathTerm* PathTermPtr;

////////////////////////////////////////////
// path of path terms
////////////////////////////////////////////
class SSPath : public CC_TPtrDlist<PathTerm>
{

private:
   unsigned int f_containPathQualifier;
   value_vector<PathTermPtr>* f_fastGetIndex; 

public:
   SSPath(char*, unsigned int assignId); // for test purpose
   SSPath();
   virtual ~SSPath();

// this call update f_containPathQualifier field
   void appendPathTerm(PathTerm*);

   unsigned int containSelector() { return f_containPathQualifier; };

// prepend p to this. Elements in p are added to this.
   void prependPath(SSPath& p); 

// Set up an index so that the random access to elements in the list can 
// be O(1). No range checking is performed.
// Set up the index by calling fastGetIndex() before any fastGetAt() call.
   void fastGetIndex();
   PathTerm* fastGetAt(unsigned int i) { return (*f_fastGetIndex)[i]; };

   friend ostream& operator<< (ostream&, SSPath&);
};

typedef CC_TPtrDlist<char> charPtrDlist;

extern unsigned int gGI_CASE_SENSITIVE;

#endif /* _Path_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
