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
// $XConsortium: PathQualifier.cc /main/4 1996/06/11 17:07:46 cde-hal $
#include "PathQualifier.h"
#include "Element.h"
#include "Attribute.h"
#include "Debug.h"


PQExpr::~PQExpr()
{
  // empty 
}

PQAttributeSelector::~PQAttributeSelector()
{
  // empty 
}

PQLogExpr::~PQLogExpr()
{
  delete f_left ;
  delete f_right ;
}

PQNot::PQNot(PQExpr *expr)
: f_expr(expr)
{
}

PQNot::~PQNot()
{
  delete f_expr;
}

PQBoolean
PQNot::evaluate(const Element &element)
{
  return (f_expr->evaluate(element) == PQTrue) ? PQFalse : PQTrue ;
}

PQPosition::PQPosition(PQEqOp optype, int position)
: f_optype(optype),
  f_position(position)
{
}

PQBoolean
PQPosition::evaluate(const Element &element)
{
   switch ( f_optype ) {
     case PQEqual: 
       if ( f_position == (int) element.sibling_number() ||
            ( f_position==-1 && element.last_child() )
          )
          return PQTrue;
       else 
          return PQFalse;
       break;

     default: 
       if ( f_position==-1 ) {
          if ( element.last_child() == 0 )
            return PQTrue;
          else
            return PQFalse;
       } else
          if ( f_position != (int) element.sibling_number() )
             return PQTrue;
          else
             return PQFalse;
       break;
   }
}

PQSibling::PQSibling(PQEqOp optype, int sib)
: f_optype(optype),
  f_sibling(sib)
{
}

PQBoolean
PQSibling::evaluate(const Element &element)
{
   switch ( f_optype ) {
     case PQEqual: 
       if ( f_sibling == (int) element.relative_sibling_number() ||
            ( f_sibling ==-1 && element.relatively_last_child() )
          )
          return PQTrue;
       else 
          return PQFalse;
       break;

     default: 
       if ( f_sibling ==-1 ) {
          if ( element.relatively_last_child() == 0 )
            return PQTrue;
          else
            return PQFalse;
       } else
          if ( f_sibling != (int) element.relative_sibling_number() )
             return PQTrue;
          else
             return PQFalse;
       break;
   }
}

PQAttributeSelector::PQAttributeSelector(const Symbol &attrname,
					 PQEqOp op,
					 const CC_String &string) 
: f_optype(op),
  f_attribute(attrname),
  f_string(string)
{
debug(cerr, op);
}

PQBoolean
PQAttributeSelector::evaluate(const Element &element)
{
  PQBoolean return_value = PQFalse ;
  
  // see if attribute exists first 
  const Attribute *attr = element.get_attribute(f_attribute);

  if (attr)
    {
      // comparison value of 0 means strings are equal
      int comparison = f_string.compareTo(attr->value());

      if (((f_optype == PQEqual) && (comparison == 0)) ||
	  ((f_optype == PQNotEqual) && (comparison != 0)))
	{
	  return_value = PQTrue ;
	}
    }
  else
    {
      // not attribute, but if operator is not equal, we should return true
      if (f_optype == PQNotEqual)
	return_value = PQTrue ;
    }
  return return_value ;
}

PQLogExpr::PQLogExpr(PQExpr *left, PQLogOp op, PQExpr *right)
: f_optype(op),
  f_left(left),
  f_right(right)
{
}


PQBoolean
PQLogExpr::evaluate(const Element &element)
{
  PQBoolean left_value = f_left->evaluate(element);
  
  if ((left_value == PQTrue) && (f_optype == PQor))
    return PQTrue ;

  if ((left_value == PQFalse) && (f_optype == PQand))
    return PQFalse ;

  return f_right->evaluate(element);
}

