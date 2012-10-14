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
// $TOG: Expression.C /main/9 1998/04/17 11:48:40 mgreess $
#include "Attribute.h"
#include "AttributeList.h"
#include "Expression.h"
#include "FeatureValue.h"
#include "ResolverStack.h"
#include "StyleSheetExceptions.h"
#include "VariableTable.h"
#include "Renderer.h"
#include "Debug.h"
#include <sstream>

#include "HardCopy/autoNumberFP.h"


extern const Element	       	*gCurrentElement;
extern const FeatureSet	       	*gCurrentLocalSet;
extern const FeatureSet	       	*gParentCompleteSet;

Expression::Expression(TermNode *root)
: f_root(root)
{
}

Expression::Expression(const Expression &e)
: f_root(e.f_root->clone())
{
}

ConstantNode::ConstantNode(FeatureValue *v)
: f_value(v)
{
}

VariableNode::VariableNode(const Symbol &name)
: f_name(name)
{
}

CompositeVariableNode::CompositeVariableNode()
: f_items(4)
{
}

CompositeVariableNode::CompositeVariableNode(size_t capac)
: f_items(capac)
{
}

CompositeVariableNode::~CompositeVariableNode()
{
  f_items.clearAndDestroy();
}

void
CompositeVariableNode::prependItem(const Symbol& item)
{
  f_items.prepend(new Symbol(item));
}

void
CompositeVariableNode::appendItem(const Symbol& item)
{
  f_items.append(new Symbol(item));
}

const Symbol* 
CompositeVariableNode::convertableToVariable()
{
   const Symbol* x = 0;
   if ( f_items.entries() == 1 ) {
      x = f_items.first();
      if ( gVariableTable -> exists(*x) ) 
         return x; 
   }
 
   return 0;
}

BinaryOperatorNode::BinaryOperatorNode(operatorType t,
				       TermNode *left, TermNode *right)
: f_operator(t), f_left(left), f_right(right)
{
}

BinaryOperatorNode::~BinaryOperatorNode()
{
  delete f_left ;
  delete f_right;
}

SgmlAttributeNode::SgmlAttributeNode(const Symbol &name)
: f_name(name)
{
}

FeatureValue *
Expression::evaluate() const
{
  return f_root->evaluate();
}

Expression::~Expression()
{
  delete  f_root ;
}

TermNode::~TermNode()
{
}

ConstantNode::~ConstantNode()
{
  delete f_value ;
}

FeatureValue *
BinaryOperatorNode::evaluate() const
{
  // calculate children trees and then have feature value do the operation 

#if !defined(SC3) && !defined(_IBMR2) && !defined(__uxp__) && !defined(__osf__) && !defined(USL) && !defined(linux) && !defined(CSRG_BASED)
  volatile
#endif
  FeatureValue *left =  0;
#if !defined(SC3) && !defined(_IBMR2) && !defined(__uxp__) && !defined(__osf__) && !defined(USL) && !defined(linux) && !defined(CSRG_BASED)
  volatile
#endif
  FeatureValue *right = 0;
#if !defined(SC3) && !defined(_IBMR2) && !defined(__uxp__) && !defined(__osf__) && !defined(USL) && !defined(linux) && !defined(CSRG_BASED)
  volatile
#endif
  FeatureValue *result = 0;

  mtry
    {
      left  = f_left->evaluate();
      right = f_right->evaluate();

      switch (f_operator)
	{
	case PLUS:
	  result = *left + *right ;
	  break;
	case MINUS:
	  result = *left - *right ;
	  break;
	case TIMES:
	  result = *left * *right ;
	  break;
	case DIVIDE:
	  result = *left / *right ;
	  break;
	}

    }
  mcatch_any()
    {
      delete left ;
      delete right ;
      delete result ;
      rethrow;
    }
  end_try ;
  delete left ;
  delete right ;
  return result ;

}


FeatureValue *
VariableNode::evaluate() const
{
  // this could be a feature or a variable 
  // first look in the parent feature set 

  // NOTE: actual operation should be to look in the local feature set first
  // before going to the parent unless the inherit operator was used. Not sure
  // how to do this, because at this point, we are trying to evaluate the
  // current feature set

  // see if item exists in parent feature hierarchy, and if not, then we go to
  // the variable table

//MESSAGE(cerr, "VariableNode::evaluate()");
//f_name.print(cerr);
  
  FeatureValue *variable_value = gVariableTable->lookup(f_name).evaluate();

//debug(cerr, int(variable_value));

  if (!variable_value)
    throw(CASTUVEXCEPT undefinedVariableException(f_name));

  // have to evaluate it in case it contains expressions or other variables
  // etc.  
  FeatureValue *return_variable = 0;

  mtry
    {
      return_variable = variable_value->evaluate() ;
    }
  mcatch_any()
    {
      delete return_variable; 
      delete variable_value ;
      rethrow;
    }
  end_try;

//MESSAGE(cerr, "VariableNode::evaluate() completes");

  delete variable_value ;
  return return_variable;
}

FeatureValue *
CompositeVariableNode::evaluate() const
{
/*
MESSAGE(cerr, "CompositeVariableNode::evaluate():");
print(cerr);
cerr << "\n";
f_items[0] -> print(cerr);
MESSAGE(cerr, "");
*/

//debug(cerr, int(gCurrentLocalSet));
//debug(cerr, int(gParentCompleteSet));

  const Feature *f = 0;

  if ( gCurrentLocalSet )
     f = gCurrentLocalSet->deep_lookup(f_items) ;

//debug(cerr, int(f));

  if (!f && gParentCompleteSet )
    f = gParentCompleteSet->deep_lookup(f_items);

//debug(cerr, int(f));

  //if ( f == 0 && gRenderer ) {
  if ( f == 0 ) {
    FeatureValue* fv = gAutoNumberFP.evaluate(f_items[0] -> name());
    if ( fv == 0 ) {
       //print(cerr);
       throw(CASTBEEXCEPT badEvaluationException());
    } else
       return fv;
  }

  if (!f) {
    //print(cerr);
    throw(CASTBEEXCEPT badEvaluationException());
  }

  return f->evaluate();
}




FeatureValue *
ConstantNode::evaluate() const
{
  //return f_value->clone();
  return f_value->evaluate();
}

extern unsigned g_validation_mode;

FeatureValue *
SgmlAttributeNode::evaluate() const
{
  if ( g_validation_mode == true ) {
    throw(CASTUAEXCEPT undefinedAttributeException(f_name));
  }

  const Attribute *attr = gCurrentElement->get_attribute(f_name);

  if (attr)
    return new FeatureValueString(attr->value());
  
  throw(CASTUAEXCEPT undefinedAttributeException(f_name));

  return 0 ;
}


 // ////////////////////////////////////////////////////////////////////////
 // Printing
 // ////////////////////////////////////////////////////////////////////////

ostream &operator<<(ostream &o, const Expression &e)
{
  return e.print(o);
}

ostream &operator<< (ostream &o, const TermNode &t)
{
  return t.print(o);
}

ostream &
Expression::print(ostream &o) const
{
  return o << *f_root ; 
}

ostream &
VariableNode::print(ostream &o) const
{
  return o << f_name ;
}

ostream &
BinaryOperatorNode::print(ostream &o) const
{
  o << "(" << *f_left << ' ';
  switch (f_operator)
    {
    case PLUS:
      o << '+' ;
      break;
    case MINUS:
      o << '-';
      break;
    case TIMES:
      o << '*' ;
      break;
    case DIVIDE:
      o << '/';
      break;
    }

  return o << ' ' << *f_right << ')' ;
}

ostream &
SgmlAttributeNode::print(ostream &o) const
{
  return o << '@' << f_name ;
}

ostream &
ConstantNode::print(ostream &o) const
{
  return o << *f_value ;
}


ostream &
CompositeVariableNode::print(ostream &o) const
{
  int length = f_items.entries();
  for (int i = 0; i < length; i++)
    {
      o << *f_items[i] ;
      if (i < length - 1)
	o << "." ;
    }

  return o ;
}

// /////////////////////////////////////////////////////////////////////////
// cloning
// /////////////////////////////////////////////////////////////////////////

TermNode *
VariableNode::clone() const
{
  return new VariableNode(f_name);
}

TermNode *
CompositeVariableNode::clone() const
{
  int nitems = f_items.entries();
  CompositeVariableNode *node = new CompositeVariableNode(nitems);

  for (int i = 0; i < nitems; i++)
    node->appendItem(*f_items(i));

  return node ;
}

TermNode *
BinaryOperatorNode::clone() const
{
  return new BinaryOperatorNode(f_operator, f_left->clone(), f_right->clone());
}

TermNode *
SgmlAttributeNode::clone() const
{
  return new SgmlAttributeNode(f_name);
}

TermNode *
ConstantNode::clone() const
{
  return new ConstantNode(f_value->clone());
}

