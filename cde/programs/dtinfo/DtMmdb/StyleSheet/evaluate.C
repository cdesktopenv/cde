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
// $XConsortium: evaluate.cc /main/3 1996/06/11 17:10:18 cde-hal $
#include "Types.h"
#include "Expression.h"
#include "Feature.h"
#include "FeatureValue.h"
#include "ResolverStack.h"
#include "StyleSheet.h"
#include "StyleSheetExceptions.h"
#include "SymTab.h"
#include "VariableTable.h"

extern const Element	       *gCurrentElement	;
extern const FeatureSet	       *gCurrentLocalSet;
extern const FeatureSet	       *gParentCompleteSet;

// unused except by HardCopy 
Renderer *gRenderer = 0 ; 

void
styleerror(char *errorstr)
{
  cerr << "Parse Error: " << errorstr << endl;
}
main(int argc, char **argv)
{
  INIT_EXCEPTIONS();
  
  StyleSheet ss;

  FeatureSet fs;

  
  FeatureValue *exp ;

  /* -------- String  -------- */
  fs.add(new Feature(gSymTab->intern("string"),
		     new FeatureValueString("this is a string")));

  /* -------- Symbol  -------- */
  fs.add(new Feature(gSymTab->intern("symbol"),
		     new FeatureValueSymbol(gSymTab->intern("Symbol"))));

  /* -------- Integer  -------- */

  fs.add(new Feature(gSymTab->intern("int"),
		     new FeatureValueInt(17)));

  /* -------- Real  -------- */

  fs.add(new Feature(gSymTab->intern("real"),
		     new FeatureValueReal(42.2)));


  /* -------- Integer addition -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::PLUS,
					   new ConstantNode(new FeatureValueInt(10)),
					   new ConstantNode(new FeatureValueInt(10)))));


  // check cloning
  FeatureValueExpression ep(*(FeatureValueExpression*)exp);


  fs.add(new Feature(gSymTab->intern("int + int"), exp));

  /* -------- Integer subtraction -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::MINUS,
					   new ConstantNode(new FeatureValueInt(3)),
					   new ConstantNode(new FeatureValueInt(127)))));

  fs.add(new Feature(gSymTab->intern("int - int"), exp));

  /* -------- Integer division -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::DIVIDE,
					   new ConstantNode(new FeatureValueInt(10)),
					   new ConstantNode(new FeatureValueInt(2)))));

  fs.add(new Feature(gSymTab->intern("int / int"), exp));

  /* -------- Real division -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::DIVIDE,
					   new ConstantNode(new FeatureValueReal(5)),
					   new ConstantNode(new FeatureValueReal(2)))));

  fs.add(new Feature(gSymTab->intern("real / real"), exp));

  /* -------- Real multiplication -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::TIMES,
					   new ConstantNode(new FeatureValueReal(5.1)),
					   new ConstantNode(new FeatureValueReal(8.7)))));

  fs.add(new Feature(gSymTab->intern("real * real"), exp));

  /* -------- int + real -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::PLUS,
					   new ConstantNode(new FeatureValueInt(5)),
					   new ConstantNode(new FeatureValueReal(8.7)))));

  fs.add(new Feature(gSymTab->intern("int + real"), exp));


  /* -------- real + int -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::PLUS,
					   new ConstantNode(new FeatureValueReal(8.7)),
					   new ConstantNode(new FeatureValueInt(5)))));

  fs.add(new Feature(gSymTab->intern("real + int"), exp));


  /* -------- now do some variable stuff -------- */

  // empty top of stack item 
  // gTopOfStack = new ResolverStackElement(0,new FeatureSet(),new FeatureSet());

  FeatureSet *localSet = new FeatureSet ;
  gCurrentElement = 0 ;
  gCurrentLocalSet = localSet ;
  gParentCompleteSet = new FeatureSet ;

  // add an element for variable lookup
  localSet->add(new Feature(gSymTab->intern("size"),
			    new FeatureValueInt(10)));

  // make some variable entries
  // insert a duplicate to check memory leaks 

  gVariableTable->enter(gSymTab->intern("DEFAULT_FONT_FAMILY"),
			new Expression(new ConstantNode(new FeatureValueString("courier"))));
  gVariableTable->enter(gSymTab->intern("DEFAULT_FONT_FAMILY"),
			new Expression(new ConstantNode(new FeatureValueString("helvetica"))));


  fs.add(new Feature(gSymTab->intern("exp"),
		     new FeatureValueExpression
		     (new Expression (new VariableNode(gSymTab->intern("DEFAULT_FONT_FAMILY"))))));

  CompositeVariableNode *cvn = new CompositeVariableNode;
  cvn->appendItem(gSymTab->intern("size"));
  fs.add(new Feature(gSymTab->intern("Xsize"),
		     new FeatureValueExpression(new Expression (cvn))));

  /* -------- dimensions -------- */
  fs.add(new Feature(gSymTab->intern("dim"),
		     new FeatureValueDimension(new FeatureValueInt(10),
					       FeatureValueDimension::INCH)));

  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::PLUS,
			     new ConstantNode(new FeatureValueInt(5)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(25),
			       FeatureValueDimension::INCH)))));

  fs.add(new Feature(gSymTab->intern("int + dim"), exp));

  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::PLUS,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(25),
			       FeatureValueDimension::INCH)),
			     new ConstantNode(new FeatureValueInt(5)))));

  fs.add(new Feature(gSymTab->intern("dim + int"), exp));


  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::PLUS,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(25),
			       FeatureValueDimension::POINT)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(5),
			       FeatureValueDimension::INCH)))));


  fs.add(new Feature(gSymTab->intern("dim + dim"), exp));

  /* -------- real * dim -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::TIMES,
			     new ConstantNode
			     (new FeatureValueReal(2.2)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)))));


  fs.add(new Feature(gSymTab->intern("real * dim"), exp));


  /* -------- dim * real -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::TIMES,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)),
			     new ConstantNode
			     (new FeatureValueReal(2.1)))));


  fs.add(new Feature(gSymTab->intern("dim * real"), exp));


  /* -------- dim / real -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::DIVIDE,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)),
			     new ConstantNode
			     (new FeatureValueReal(2.0)))));


  fs.add(new Feature(gSymTab->intern("dim / real"), exp));


  /* -------- real / dim -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::DIVIDE,
			     new ConstantNode
			     (new FeatureValueReal(11)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(2),
			       FeatureValueDimension::POINT)))));


  fs.add(new Feature(gSymTab->intern("real / dim"), exp));


  /* -------- dim * int -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::TIMES,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)),
			     new ConstantNode
			     (new FeatureValueInt(2)))));


  fs.add(new Feature(gSymTab->intern("dim * int"), exp));


  /* -------- int * dim -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::TIMES,
			     new ConstantNode
			     (new FeatureValueInt(2)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)))));


  fs.add(new Feature(gSymTab->intern("int * dim"), exp));


  /* -------- dim / int -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::DIVIDE,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)),
			     new ConstantNode
			     (new FeatureValueInt(2)))));


  fs.add(new Feature(gSymTab->intern("dim / int"), exp));


  /* -------- int / dim -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::DIVIDE,
			     new ConstantNode
			     (new FeatureValueInt(22)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)))));


  fs.add(new Feature(gSymTab->intern("int / dim"), exp));


  
  /* -------- int - dim -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::MINUS,
			     new ConstantNode
			     (new FeatureValueInt(2)),
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)))));


  fs.add(new Feature(gSymTab->intern("int - dim"), exp));


  /* -------- dim - int -------- */
  exp = new FeatureValueExpression
    (new Expression
     (new BinaryOperatorNode(BinaryOperatorNode::MINUS,
			     new ConstantNode
			     (new FeatureValueDimension
			      (new FeatureValueInt(11),
			       FeatureValueDimension::POINT)),
			     new ConstantNode
			     (new FeatureValueInt(2)))));


  fs.add(new Feature(gSymTab->intern("dim - int"), exp));


  /* -------- String + String -------- */
  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::PLUS,
					   new ConstantNode(new FeatureValueString("foo")),
					   new ConstantNode(new FeatureValueString("bar")))));

  fs.add(new Feature(gSymTab->intern("String + String"), exp));



  /* -------- String + String + String-------- */
  FeatureValueExpression *exp1 =
    new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::PLUS,
					   new ConstantNode(new FeatureValueString("foo")),
					   new ConstantNode(new FeatureValueString("bar")))));


  exp = new FeatureValueExpression
    (new Expression(new BinaryOperatorNode(BinaryOperatorNode::PLUS,
					   new ConstantNode(exp1),
					   new ConstantNode(new FeatureValueString("baz")))));

  fs.add(new Feature(gSymTab->intern("String + String + String"), exp));



/* -------- print out our set -------- */

  cout << fs << endl;

  FeatureSet *evaluated = fs.evaluate();

  cout << *evaluated << endl;

  delete evaluated ;


  cout << *gVariableTable << endl;

  // write this guy out that we created at the top 
  cout << ep << endl;

  delete gCurrentLocalSet ;
  delete gParentCompleteSet ;

  FeatureValueDimension *dtest =
    new FeatureValueDimension
    (new FeatureValueDimension(new FeatureValueInt(72),
			       FeatureValue::POINT),
     FeatureValue::INCH);

  cout << *dtest << endl;
  cout << (float)*dtest << endl;
  delete dtest ;

}
