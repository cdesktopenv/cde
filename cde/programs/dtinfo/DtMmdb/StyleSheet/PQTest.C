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
// $XConsortium: PQTest.cc /main/3 1996/06/11 17:07:26 cde-hal $
#include <iostream>
using namespace std;
#include "SymTab.h"
#include "PathQualifier.h"
#include "Element.h"
#include "Attribute.h"
#include "AttributeList.h"

void
styleerror(char *)
{
  // bogus 
  abort();
}

class Renderer;
Renderer *gRenderer = 0;

main()
{
  SymbolTable symtab ;

  Attribute *attr1a = new Attribute(symtab.intern("attr1a"),"one-a");
  Attribute *attr1b = new Attribute(symtab.intern("attr1b"),"one-b");

  AttributeList *alist1 = new AttributeList ;
  alist1->add(attr1a);
  alist1->add(attr1b);

  Element one(symtab.intern("One"), 1, alist1, 0);
  Element two(symtab.intern("Two"), 2, 0, 0);

  PQPosition posn_eq(PQEqual, 1);
  PQPosition posn_neq(PQNotEqual, 1);

// /////////////////////////////////////////////////////////////////////////
// Test Position
// /////////////////////////////////////////////////////////////////////////


  /* -------- Test Position Equal  -------- */
  cout << posn_eq.evaluate (one) << endl; // 1 
  cout << posn_eq.evaluate (two) << endl; // 0 
  cout << "---" << endl;
  /* -------- Test Position Not Equal -------- */
  cout << posn_neq.evaluate (one) << endl; // 0 
  cout << posn_neq.evaluate (two) << endl; // 1 
  cout << "---" << endl;

  // ///////////////////////////////////////////////////////////////////////
  // Test Attribute Comparison
  // ///////////////////////////////////////////////////////////////////////

  PQAttributeSelector pqas_eqa(symtab.intern("attr1a"), PQEqual, "one-a");
  PQAttributeSelector pqas_eqb(symtab.intern("attr1a"), PQEqual, "one-b");
  PQAttributeSelector pqas_neqa(symtab.intern("attr1a"), PQNotEqual, "one-a");
  PQAttributeSelector pqas_neqb(symtab.intern("attr1a"), PQNotEqual, "one-b");

  PQAttributeSelector pqas_eqa2(symtab.intern("attr2a"), PQEqual, "one-a");
  PQAttributeSelector pqas_eqb2(symtab.intern("attr2a"), PQEqual, "one-b");
  PQAttributeSelector pqas_neqa2(symtab.intern("attr2a"), PQNotEqual, "one-a");
  PQAttributeSelector pqas_neqb2(symtab.intern("attr2a"), PQNotEqual, "one-b");

  cout << pqas_eqa.evaluate(one) << endl ; // 1 
  cout << pqas_eqa.evaluate(two) << endl ; // 0 

  cout << pqas_eqb.evaluate(one) << endl ; // 0 
  cout << pqas_eqb.evaluate(two) << endl ; // 0 

  cout << pqas_neqa.evaluate(one) << endl ; // 0 
  cout << pqas_neqa.evaluate(two) << endl ; // 1

  cout << pqas_neqb.evaluate(one) << endl ; // 1 
  cout << pqas_neqb.evaluate(two) << endl ; // 1 

  cout << pqas_eqa2.evaluate(one) << endl ; // 0
  cout << pqas_eqa2.evaluate(two) << endl ; // 0 

  cout << pqas_eqb2.evaluate(one) << endl ; // 0 
  cout << pqas_eqb2.evaluate(two) << endl ; // 0 

  cout << pqas_neqa2.evaluate(one) << endl ; // 1 
  cout << pqas_neqa2.evaluate(two) << endl ; // 1 

  cout << pqas_neqb2.evaluate(one) << endl ; // 1 
  cout << pqas_neqb2.evaluate(two) << endl ; // 1 

  cout << "****" << endl;

  // ///////////////////////////////////////////////////////////////////////
  // composite
  // ///////////////////////////////////////////////////////////////////////

  
    // position = 1 and attr(attr1a) == "one-a" 

  PQPosition *p1 = new PQPosition(PQEqual, 1);
  PQAttributeSelector *a1 = new PQAttributeSelector(symtab.intern("attr1a"),
						    PQEqual, "one-a"); 

  PQAttributeSelector *a2 = new PQAttributeSelector(symtab.intern("attr1a"),
						    PQEqual, "value"); 

  PQLogExpr l1 (p1, PQand, a1);
  PQLogExpr l2 (p1, PQor,  a1);
  PQLogExpr l3 (p1, PQand, a2);
  PQLogExpr l4 (p1, PQor,  a2);

  cout << l1.evaluate(one) << endl ; // 1 
  cout << l1.evaluate(two) << endl ; // 0 
  
  cout << l2.evaluate(one) << endl ; // 1 
  cout << l2.evaluate(two) << endl ; // 0 

  cout << l3.evaluate(one) << endl ; // 0 
  cout << l3.evaluate(two) << endl ; // 0 

  cout << l4.evaluate(one) << endl ; // 1 
  cout << l4.evaluate(two) << endl ; // 0 

  cout << "..." << endl;
  cout << PQNot(&l4).evaluate(one) << endl ; // 0 
  cout << PQNot(&l4).evaluate(two) << endl ; // 1 

  
  
}
