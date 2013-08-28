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
/* $XConsortium: PathQualifier.h /main/5 1996/08/21 15:50:33 drk $ */
#ifndef _PathQualifier_h
#define _PathQualifier_h

#ifndef CDE_NEXT

#else
#include "dti_cc/CC_String.h"
#endif

#include "SymTab.h"

// PathEXPR = PathTERM | PathTERM log PathTERM

// PathTERM = position | attribute-selector

// position = posn eqop number
// attribute-selector = attribute eqop string

// log = and | or 


class Element;
class Symbol ;

enum PQEqOp { PQEqual, PQNotEqual };
enum PQBoolean { PQFalse, PQTrue };
enum PQLogOp { PQand, PQor };

class PQExpr
{
public:
  virtual ~PQExpr();
  virtual PQBoolean evaluate(const Element &) = 0;
};

class PQNot : public PQExpr
{
public:
  PQNot(PQExpr *);
  ~PQNot();
  virtual PQBoolean evaluate(const Element &);
private:
  PQExpr	*f_expr;
};


class PQPosition : public PQExpr
{
public:
  PQPosition(PQEqOp optype, int position);
  virtual PQBoolean evaluate(const Element &);

private:
  PQEqOp	f_optype;
  int	f_position ;
};

class PQSibling: public PQExpr
{
public:
  PQSibling(PQEqOp optype, int sibling);
  virtual PQBoolean evaluate(const Element &);

private:
  PQEqOp	f_optype;
  int	f_sibling;
};

class PQAttributeSelector : public PQExpr
{
public:
  PQAttributeSelector(const Symbol &attrname, PQEqOp, const CC_String &string);
  ~PQAttributeSelector();
  virtual PQBoolean evaluate(const Element &);
private:
  PQEqOp	f_optype ;
  Symbol	f_attribute;
  CC_String	f_string ;
};

class PQLogExpr : public PQExpr
{
public:

  PQLogExpr(PQExpr *left, PQLogOp op, PQExpr *right);
  ~PQLogExpr();

  virtual PQBoolean evaluate(const Element &);

private:

  PQLogOp	 f_optype ;
  PQExpr	*f_left ;
  PQExpr	*f_right ;
};




#endif /* _PathQualifier_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */


