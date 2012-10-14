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
// $TOG: SSPath.C /main/5 1998/04/17 11:49:33 mgreess $

#ifndef CDE_NEXT

#else
#include "dti_cc/CC_Tokenizer.h"
#endif

#include "SSPath.h"
#include "Debug.h"
#include "SymTab.h"
#include "StyleSheetExceptions.h"

unsigned int GI_CASE_SENSITIVE = false;

PathTerm::PathTerm(const Element& element) :
	f_element(element), f_PQExpr(0)
{
}

PathTerm::PathTerm(const Symbol& symbol, PQExpr* expr) :
	f_element(symbol), f_PQExpr(expr)
{
}

PathTerm::PathTerm(const char* symbol, PQExpr* expr) :
	f_element(gElemSymTab -> intern(symbol, true)), f_PQExpr(expr)
{
}

PathTerm::~PathTerm()
{
   delete f_PQExpr;
}

unsigned int PathTerm::operator ==(const PathTerm&)
{
   MESSAGE(cerr, "PathTerm::operator ==() should not be called");
   throw(CASTBEEXCEPT badEvaluationException());
   return 0;
}

ostream& operator <<(ostream& out, PathTerm& pt)
{
   out << pt.symbol() << " (" << size_t(pt.f_PQExpr) << ") ";
   return out;
}

////////////////////////////////////////////
//
////////////////////////////////////////////

void SSPath::appendPathTerm(PathTerm* pt)
{
   if ( pt -> pqexpr() )
      f_containPathQualifier = true;

   append(pt);
}

void SSPath::prependPath(SSPath& p)
{
   if ( p.entries() == 0 )
      return;

   CC_TPtrDlistIterator<PathTerm> l_Iter(p);
   l_Iter += p.entries();

   PathTerm* l_pathTerm = 0;

   do {
      //prepend(new PathTerm(*l_Iter.key()));

      l_pathTerm = l_Iter.key();
 
      prepend(l_pathTerm);

      if ( l_pathTerm -> pqexpr() )
         f_containPathQualifier = true;

      
   } while ( --l_Iter );
}

SSPath::SSPath()
: f_containPathQualifier(false), f_fastGetIndex(0)
{
}

SSPath::~SSPath()
{
  // clean up memory 
  clearAndDestroy();
  delete f_fastGetIndex;
}

SSPath::SSPath(char* str, unsigned int AssignId) : 
   f_containPathQualifier(false), f_fastGetIndex(0)
{
   CC_String a(str);
   CC_Tokenizer next(a);
   CC_String token;
 
#ifndef CDE_NEXT
   while ( !(token=next()).isNull() ) {
     append(new PathTerm(token.data(), 0));
   }
#else
   while ( next() ) {
     append(new PathTerm(token.data(), 0));
   }
#endif
}

ostream& operator<<(ostream& out, SSPath& p)
{
   CC_TPtrDlistIterator<PathTerm> l_Iter(p);
   while ( ++l_Iter ) {
     out << *l_Iter.key() << ' ';
   }
   return out;
}

void SSPath::fastGetIndex()
{
   f_fastGetIndex = new value_vector<PathTermPtr>(entries());

   CC_TPtrDlistIterator<PathTerm> l_Iter(*this);
   int i=0;
   while ( ++l_Iter ) {
     (*f_fastGetIndex)[i++] = l_Iter.key();
   }
}

