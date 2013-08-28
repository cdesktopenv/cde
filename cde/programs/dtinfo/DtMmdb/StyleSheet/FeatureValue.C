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
// $TOG: FeatureValue.C /main/6 1998/04/17 11:49:16 mgreess $
#include "StyleSheetExceptions.h"
#include "SymTab.h"
#include "Feature.h"
#include "FeatureValue.h"
#include "Expression.h"
#include "Debug.h"
#include <utility/funcs.h>
#include <ctype.h>

#include <string.h>

/* cloning */
FeatureValue *
FeatureValueInt::clone() const
{
  return new FeatureValueInt(*this);
}
FeatureValue *
FeatureValueReal::clone() const
{
  return new FeatureValueReal(*this);
}
FeatureValue *
FeatureValueString::clone() const
{
  return new FeatureValueString(*this);
}
FeatureValue *
FeatureValueSymbol::clone() const
{
  return new FeatureValueSymbol(*this);
}
FeatureValue *
FeatureValueExpression::clone() const
{
  return new FeatureValueExpression(*this);
}
FeatureValue *
FeatureValueFeatureSet::clone() const
{
  return new FeatureValueFeatureSet(*this);
}
FeatureValue *
FeatureValueDimension::clone() const
{
  return new FeatureValueDimension(*this);
}

// copy constructors

/* -------- Int -------- */

FeatureValueInt::FeatureValueInt(const FeatureValueInt &object)
: FeatureValue(integer), f_value(object.f_value)
{
}

/* -------- real -------- */
FeatureValueReal::FeatureValueReal(const FeatureValueReal &object)
: FeatureValue(real), f_value(object.f_value)
{
}

/* -------- String -------- */
FeatureValueString::FeatureValueString(const FeatureValueString &object)
: FeatureValue(string), f_value(object.f_value)
{
}


/* -------- Symbol -------- */
FeatureValueSymbol::FeatureValueSymbol(const FeatureValueSymbol &object)
: FeatureValue(symbol), f_value(object.f_value)
{
}

/* -------- Expression -------- */
FeatureValueExpression::FeatureValueExpression(const FeatureValueExpression &object)
: FeatureValue(expression), f_value(new Expression(*object.f_value))
{
}
FeatureValueExpression::FeatureValueExpression(Expression *e)
: FeatureValue(expression), f_value(e)
{
}

/* -------- Feature Set -------- */
FeatureValueFeatureSet::FeatureValueFeatureSet(FeatureSet *fs)
: FeatureValue(featureset), f_value(fs)
{
}

FeatureValueFeatureSet::FeatureValueFeatureSet(const FeatureValueFeatureSet &object)
: FeatureValue(featureset), f_value(new FeatureSet(*object.f_value))
{
}


FeatureValueDimension::FeatureValueDimension(const FeatureValueDimension& object)
: FeatureValue(dimension),
  f_cachedValue(object.f_cachedValue),
  f_unit(object.f_unit)
{
   f_value = (object.f_value == 0) ? 0 : object.f_value -> clone();
}


// /////////////////////////////////////////////////////////////////////////
// Destructors
// /////////////////////////////////////////////////////////////////////////

FeatureValue::~FeatureValue()
{
}


FeatureValueString::~FeatureValueString()
{
}

FeatureValueExpression::~FeatureValueExpression()
{
  delete f_value ;
}

FeatureValueFeatureSet::~FeatureValueFeatureSet()
{
  delete f_value;
}

FeatureValueDimension::~FeatureValueDimension()
{
  delete f_value;
}

/*
  // operators 
  virtual FeatureValue *operator+(FeatureValue&);
  virtual FeatureValue *operator-(FeatureValue&);
  virtual FeatureValue *operator*(FeatureValue&);
  virtual FeatureValue *operator/(FeatureValue&);
  */

FeatureValue *
FeatureValue::operator+(const FeatureValue &) const
{
  // if we are here, then we did not override this function, and therefore
  // cannot properly respond, so we use zero as our value
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
};

FeatureValue *
FeatureValue::operator-(const FeatureValue &) const
{
  // if we are here, then we did not override this function, and therefore
  // cannot properly respond, so we use zero as our value
  throw (CASTBEEXCEPT badEvaluationException());
  return 0 ;
};

FeatureValue *
FeatureValue::operator*(const FeatureValue &) const
{
  // if we are here, then we did not override this function, and therefore
  // cannot properly respond, so we use zero as our value
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
};

FeatureValue *
FeatureValue::operator/(const FeatureValue &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
};

FeatureValue *
FeatureValue::operator+(const int)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}
FeatureValue *
FeatureValue::operator-(const int)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue *
FeatureValue::operator*(const int)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue *
FeatureValue::operator/(const int)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue *
FeatureValue::operator+(const float)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}


FeatureValue *
FeatureValue::operator-(const float)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}
FeatureValue *
FeatureValue::operator*(const float)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue *
FeatureValue::operator/(const float)  const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}


FeatureValue*
FeatureValue::operator+(const FeatureValueInt&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator-(const FeatureValueInt&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator*(const FeatureValueInt&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator/(const FeatureValueInt&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}


FeatureValue*
FeatureValue::operator+(const FeatureValueReal&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator-(const FeatureValueReal&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator*(const FeatureValueReal&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator/(const FeatureValueReal&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}


FeatureValue*
FeatureValue::operator+(const FeatureValueDimension&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator-(const FeatureValueDimension&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator*(const FeatureValueDimension&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator/(const FeatureValueDimension&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}


FeatureValue*
FeatureValue::operator+(const FeatureValueExpression&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator-(const FeatureValueExpression&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator*(const FeatureValueExpression&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValue::operator/(const FeatureValueExpression&) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}


// /////////////////////////////////////////////////////////////////////////
// operators for FeatureValueInt
// /////////////////////////////////////////////////////////////////////////

FeatureValue *
FeatureValueInt::operator+(const FeatureValue &value) const
{
  return value + f_value ;
}

FeatureValue *
FeatureValueInt::operator-(const FeatureValue &value) const
{
  return value.rsub(f_value) ;
}

FeatureValue *
FeatureValueInt::operator*(const FeatureValue &value) const
{
  return value * f_value ;
}

FeatureValue *
FeatureValueInt::operator/(const FeatureValue &value) const
{
  return value.rdiv(f_value);
}

// now comes the ability to do actual operations because all types are know

FeatureValue*
FeatureValueInt::operator+(const int i) const
{
  return new FeatureValueInt(f_value + i);
}

FeatureValue*
FeatureValueInt::operator-(const int i) const 
{
  return new FeatureValueInt(f_value - i);
}

FeatureValue*
FeatureValueInt::operator*(const int i) const
{
  return new FeatureValueInt(f_value * i);
}

FeatureValue*
FeatureValueInt::operator/(const int i) const
{
  return new FeatureValueReal((float)f_value / (float)i);
}


// /////////////////////////////////////////////////////////////////////////
// Printing
// /////////////////////////////////////////////////////////////////////////

// output operator, uses virtual function print
ostream & operator << (ostream &o, const FeatureValue &f)
{
  return f.print(o);
}

ostream  &
FeatureValueReal::print(ostream &o) const
{
  return o << f_value ; 
}
ostream &
FeatureValueInt::print(ostream &o) const
{
  return o << f_value ; 
}
ostream &
FeatureValueString::print(ostream &o) const
{
  return o << '"' << f_value << '"'; 
}
ostream &
FeatureValueSymbol::print(ostream &o) const
{
  return o << f_value ; 
}
ostream &
FeatureValueExpression::print(ostream &o) const
{
  return o << *f_value ; 
}
ostream &
FeatureValueFeatureSet::print(ostream &o) const
{
  return o << *f_value ; 
}


// /////////////////////////////////////////////////////////////////////////
// Casting
// /////////////////////////////////////////////////////////////////////////

FeatureValue::operator const FeatureSet *() const
{
  throw(CASTBCEXCEPT badCastException());
  return 0 ;
}

FeatureValueFeatureSet::operator const FeatureSet *() const
{
  return f_value ;
}

FeatureValue::operator float() const
{
  throw(CASTBCEXCEPT badCastException());
  return 0;
}
FeatureValue::operator int() const
{
  throw(CASTBCEXCEPT badCastException());
  return 0;
}
FeatureValue::operator const char *() const
{
  throw(CASTBCEXCEPT badCastException());
  return 0;
}

FeatureValueReal::operator int() const
{
  return f_value ;
}

FeatureValueReal::operator float() const
{
  return f_value ;
}

FeatureValue *
FeatureValueString::operator+(const FeatureValue& x) const 
{
   if ( x.type() != string )
     return FeatureValue::operator+(x);
   else {
     const char* str1 = *this;
     const char* str2 = x;

     int len1 = f_value.length();
     int len2 = ((FeatureValueString*)&x) -> f_value.length();

     char* newString = new char[len1 + len2 + 1];

     int i;
     for ( i=0; i<len1; i++ )
        newString[i] = str1[i];
       
     for ( i=0; i<len2; i++ )
        newString[i+len1] = str2[i];
       
     newString[len1+len2] = 0;

     FeatureValueString *z = new FeatureValueString(newString);
     delete newString;

     return z;
   }
}

FeatureValue *
FeatureValueString::operator+(const FeatureValueInt& x) const 
{
   return FeatureValue::operator+(x);
}

FeatureValue *
FeatureValueString::operator+(const FeatureValueReal& x) const 
{
   return FeatureValue::operator+(x);
}

FeatureValue *
FeatureValueString::operator+(const FeatureValueDimension& x) const 
{
   return FeatureValue::operator+(x);
}

FeatureValue *
FeatureValueString::operator+(const FeatureValueExpression& expr) const 
{
   FeatureValue *x = 0;
   FeatureValue *y = 0;
   mtry
    {
      x = expr.evaluate();
      y = operator+(*x);
      delete x;
      return y;
    }
   mcatch_any()
    {
      delete x;
      rethrow;
    }
   end_try ;
}
FeatureValue *
FeatureValueString::operator+(const int i) const 
{
   return FeatureValue::operator+(i);
}

FeatureValue *
FeatureValueString::operator+(const float f) const 
{
   return FeatureValue::operator+(f);
}

FeatureValueString::operator const char *() const
{
  return f_value ;
}

FeatureValueSymbol::operator const char *() const
{
  return strdup(f_value.name()) ;
}

FeatureValueInt::operator const char *() const
{
  throw(CASTBCEXCEPT badCastException()) ;
  return 0;
}

FeatureValueExpression::operator const char *() const
{
  throw(CASTBCEXCEPT badCastException()) ;
  return 0;
}

FeatureValueDimension::operator int ()  const
{
  if ( f_cachedValue != -1 )
    return f_cachedValue;

  if ( f_value == 0 )
     throw(CASTBEEXCEPT badEvaluationException()) ;

  FeatureValue *intermediate = f_value->doConvert(f_unit);
  int i = *intermediate ;
  delete intermediate ;
  return i ;
}

FeatureValueDimension::operator float()  const
{
  if ( f_cachedValue != -1 )
    return f_cachedValue;

  if ( f_value == 0 )
     throw(CASTBEEXCEPT badEvaluationException()) ;

  FeatureValue *intermediate = f_value->doConvert(f_unit);
  float f = *intermediate ;
  delete intermediate ;
  return f ;
}

FeatureValueDimension::operator const char *() const 
{
  throw(CASTBCEXCEPT badCastException()) ;
  return 0;
}

// /////////////////////////////////////////////////////////////////////////
// Evaluate
// /////////////////////////////////////////////////////////////////////////

FeatureValue *
FeatureValue::evaluate() const
{
   return clone();
}

FeatureValue *
FeatureValueFeatureSet::evaluate() const
{
#ifdef TK
  FeatureSet* fs = new FeatureSet;
  return new FeatureValueFeatureSet(f_value->evaluate(fs)); 
#else
  return new FeatureValueFeatureSet(f_value->evaluate());
#endif
}

FeatureValue *
FeatureValueExpression::evaluate() const
{
  return f_value->evaluate();
}

// /////////////////////////////////////////////////////////////////////////
// more math for Expression
// /////////////////////////////////////////////////////////////////////////

FeatureValue *
FeatureValueExpression::operator/(const FeatureValue &f) const
{
  // Need parameters for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) / f ;
  delete eval ;
  return rval ;
}

FeatureValue *
FeatureValueExpression::operator*(const FeatureValue &f) const
{
  // Need parameters for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) * f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator-(const FeatureValue &f) const
{
  // Need parameters for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) - f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator+(const FeatureValue &f) const
{
  // Need parameters for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) + f ;
  delete eval ;
  return rval ;
}

FeatureValue *
FeatureValueExpression::operator/(const float f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) / f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator*(const float f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) * f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator-(const float f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) - f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator+(const float f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) + f ;
  delete eval ;
  return rval ;
}


FeatureValue *
FeatureValueExpression::operator/(const int f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) / f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator*(const int f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) * f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator-(const int f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) - f ;
  delete eval ;
  return rval ;
}
FeatureValue *
FeatureValueExpression::operator+(const int f) const
{
  // need values for evaluate 
  FeatureValue *eval = evaluate();
  FeatureValue *rval = (*eval) + f ;
  delete eval ;
  return rval ;
}

FeatureValue *
FeatureValueInt::operator/(const float f) const
{
  return new FeatureValueReal(f_value / f);
}
FeatureValue *
FeatureValueInt::operator*(const float f) const
{
  return new FeatureValueReal(f_value * f);
}

FeatureValue *
FeatureValueInt::operator-(const float f) const
{
  return new FeatureValueReal(f_value - f);
}

FeatureValue*
FeatureValueInt::operator+(const float f) const
{
  return new FeatureValueReal(f_value + f);
}

FeatureValueReal::operator const char*(void) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValueString::operator int(void) const
{
#ifdef old_code
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
#else
  return atoi (f_value);
#endif
}

FeatureValueString::operator float(void) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValueSymbol::operator int(void) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValueSymbol::operator float(void) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}

FeatureValue*
FeatureValueReal::operator/(const FeatureValue &f) const
{
  return f.rdiv(f_value);
}

FeatureValue*
FeatureValueReal::operator*(const FeatureValue &f) const
{
  return f * f_value ;
}
FeatureValue*
FeatureValueReal::operator-(const FeatureValue &f) const
{
  return f.rsub(f_value);
}
FeatureValue*
FeatureValueReal::operator+(const FeatureValue &f) const
{
  return f + f_value ;
}


FeatureValueExpression::operator int(void) const
{
  FeatureValue *eval = evaluate();
  int i = (int)(*eval);
  delete eval;
  return i;
}

FeatureValueExpression::operator float(void) const
{
  FeatureValue *eval = evaluate();
  float f = (float)(*eval);
  delete eval;
  return f;
}

FeatureValue *
FeatureValueReal::operator/(const float f) const
{
  return  new FeatureValueReal(f_value / f);
}
FeatureValue *
FeatureValueReal::operator*(const float f) const
{
  return  new FeatureValueReal(f_value * f);
}
FeatureValue *
FeatureValueReal::operator-(const float f) const
{
  return  new FeatureValueReal(f_value - f);
}
FeatureValue *
FeatureValueReal::operator+(const float f) const
{
  return  new FeatureValueReal(f_value + f);
}

FeatureValue*
FeatureValueReal::operator/(const int i) const
{
  return new FeatureValueReal(f_value / (float) i);
}
FeatureValue*
FeatureValueReal::operator*(const int i) const
{
  return new FeatureValueReal(f_value * (float) i);
}
FeatureValue*
FeatureValueReal::operator-(const int i) const
{
  return new FeatureValueReal(f_value - (float) i);
}
FeatureValue*
FeatureValueReal::operator+(const int i) const
{
  return new FeatureValueReal(f_value + (float) i);
}

FeatureValueInt::operator int(void) const
{
  return f_value  ;
}

FeatureValueInt::operator float(void) const
{
  return f_value ;
}


// /////////////////////////////////////////////////////////////////////////
// operator ==
// /////////////////////////////////////////////////////////////////////////

unsigned int
FeatureValue::operator==(const FeatureValue &) const 
{
  // false unless overridden by derived objects
  return 0 ;
}
unsigned int
FeatureValue::operator==(const FeatureValueInt &) const 
{
  return 0 ;
}
unsigned int
FeatureValue::operator==(const FeatureValueReal &) const
{
  return 0 ;
}
unsigned int
FeatureValue::operator==(const FeatureValueString &) const
{
  return 0 ;
}
unsigned int
FeatureValue::operator==(const FeatureValueSymbol &) const
{
  return 0 ;
}


unsigned int
FeatureValueInt::operator==(const FeatureValue &f) const
{
  return f == *this ;
}

unsigned int
FeatureValueInt::operator==(const FeatureValueInt &f) const
{
  return f.f_value == f_value ;
}
unsigned int
FeatureValueInt::operator==(const FeatureValueReal &) const
{
  return 0 ;
}
unsigned int
FeatureValueInt::operator==(const FeatureValueString &) const
{
  return 0 ;
}
unsigned int
FeatureValueInt::operator==(const FeatureValueSymbol &) const
{
  return 0 ;
}


unsigned int
FeatureValueReal::operator==(const FeatureValue &f) const
{
  return f == *this ;
}

unsigned int
FeatureValueReal::operator==(const FeatureValueReal &f) const
{
  return f.f_value == f_value ;
}
unsigned int
FeatureValueReal::operator==(const FeatureValueInt &) const
{
  return 0 ;
}
unsigned int
FeatureValueReal::operator==(const FeatureValueString &) const
{
  return 0 ;
}
unsigned int
FeatureValueReal::operator==(const FeatureValueSymbol &) const
{
  return 0 ;
}

unsigned int
FeatureValueSymbol::operator==(const FeatureValue &f) const
{
  return f == *this ;
}
unsigned int
FeatureValueSymbol::operator==(const FeatureValueSymbol &f) const
{
  return f.f_value == f_value ;
}
unsigned int
FeatureValueSymbol::operator==(const FeatureValueInt &) const
{
  return 0 ;
}
unsigned int
FeatureValueSymbol::operator==(const FeatureValueReal &) const
{
  return 0 ;
}
unsigned int
FeatureValueSymbol::operator==(const FeatureValueString &) const
{
  return 0 ;
}

unsigned int
FeatureValueString::operator==(const FeatureValue &f) const
{
  return f == *this ;
}
unsigned int
FeatureValueString::operator==(const FeatureValueString &f) const
{
  return !f_value.compareTo(f.f_value, CC_String::exact);
}
unsigned int
FeatureValueString::operator==(const FeatureValueInt &) const
{
  return 0 ;
}
unsigned int
FeatureValueString::operator==(const FeatureValueReal &) const
{
  return 0 ;
}
unsigned int
FeatureValueString::operator==(const FeatureValueSymbol &) const
{
  return 0 ;
}

// /////////////////////////////////////////////////////////////////////////
// merge
// /////////////////////////////////////////////////////////////////////////


FeatureValue *
FeatureValue::merge(const FeatureValue &f)
{
  return f.clone();
}

FeatureValue *
FeatureValueFeatureSet::merge(const FeatureValue &f)
{
  if (f.type() == featureset)
    return new FeatureValueFeatureSet(new FeatureSet(*f_value, 
 						     *((FeatureValueFeatureSet*)&f)->f_value));
  else
    return f.clone() ;
}

static float dimensionConversionTable[] = {72, 12, 1, 72/2.54};

static float
convert(float y,
        FeatureValue::Unit dimensionOfy,
        FeatureValue::Unit dimensionOfReturn
        )
{
   if ( dimensionOfy == FeatureValue::NONE ||
        dimensionOfReturn == FeatureValue::NONE ||
        dimensionOfy == dimensionOfReturn
      )
     return y;

// handle PIXEL case here.


// handle INCH, PICA, POINT and CM cases here

//debug(cerr, y);
//debug(cerr, dimensionConversionTable[dimensionOfy]);
//debug(cerr, dimensionConversionTable[dimensionOfReturn]);

   return y *
          dimensionConversionTable[dimensionOfy] /
          dimensionConversionTable[dimensionOfReturn];
}


float
FeatureValueDimension::convert(float y,
                               Unit dimensionOfy,
                               Unit dimensionOfReturn
                              )
{
   return ::convert(y, dimensionOfy, dimensionOfReturn);
}

FeatureValue *FeatureValueDimension::operator/(const FeatureValue& fv) const
{
  return fv.rdiv(*this);
}

FeatureValue *FeatureValueDimension::operator*(const FeatureValue& fv) const
{
  return fv * *this ;
}

FeatureValue *FeatureValueDimension::operator-(const FeatureValue& fv) const
{
  return fv.rsub(*this);
}

FeatureValue *FeatureValueDimension::operator+(const FeatureValue& fv) const
{
  return fv + *this ;
}

FeatureValue *FeatureValueDimension::operator/(const float x) const
{
  FeatureValue *intermediate = *f_value / x;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete  intermediate;
  return result ;
}

FeatureValue *FeatureValueDimension::operator*(const float x) const
{
  FeatureValue *intermediate = *f_value * x ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate ;
  return result ;
}

FeatureValue *FeatureValueDimension::operator-(const float x) const
{
  FeatureValue *intermediate = *f_value - x;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue *FeatureValueDimension::operator+(const float x) const
{
  FeatureValue *intermediate = *f_value + x;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue *FeatureValueDimension::operator/(const int x) const
{
  FeatureValue *intermediate = *f_value / x ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue *FeatureValueDimension::operator*(const int x) const
{
  FeatureValue *intermediate = *f_value * x ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue *FeatureValueDimension::operator-(const int x) const
{
  FeatureValue *intermediate = *f_value - x ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate ;
  return result ;
}

FeatureValue *FeatureValueDimension::operator+(const int x) const
{
  FeatureValue *intermediate = *f_value + x ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate ;
  return result ;
}

ostream& FeatureValueDimension::print(ostream& out) const
{
  out << '<' ;
  if ( f_value == 0 )
    if ( f_cachedValue != -1 )
      out << f_cachedValue << " ";
    else
      throw(CASTBEEXCEPT badEvaluationException());
  else
     out << *f_value << " ";

  switch ( f_unit )
    {
    case INCH:
      out << "inch";
      break;
    case PICA:
      out << "pica";
      break;
    case POINT:
      out << "point";
      break;
    case CM:
      out << "cm";
      break;
    case PIXEL:
      out << "pixel";
      break;
    default:
      throw(CASTBEEXCEPT badEvaluationException());
    }
  out << '>' ;
  return out ;
}

FeatureValueDimension::FeatureValueDimension(float f, const char* u)
: FeatureValue(dimension), f_value(0), 
  f_cachedValue(f)
{
  f_unit = convertToUnit(u);
}

FeatureValueDimension::FeatureValueDimension(FeatureValue *value, Unit unit)
: FeatureValue(dimension),
  f_value(value), 
  f_cachedValue(-1),
  f_unit(unit)
{
}

FeatureValueDimension::FeatureValueDimension(FeatureValue* f, const char* u)
: FeatureValue(dimension), f_value(f), f_cachedValue(-1)
{
  f_unit = convertToUnit(u);
}

FeatureValue::Unit 
FeatureValueDimension::convertToUnit(const char* u)
{
   Unit x;
   if ( strcasecmp(u, "inch") == 0 || strcasecmp(u, "in") == 0 )
       x=INCH;
   else
   if ( strcasecmp(u, "cm") == 0 )
      x=CM;
   else
   if ( strcasecmp(u, "pica") == 0 || strcasecmp(u, "pc") == 0 )
      x=PICA;
   else
   if ( strcasecmp(u, "point") == 0 || strcasecmp(u, "pt") == 0 )
      x=POINT;
   else
   if ( strcasecmp(u, "pixel") == 0 )
      x=PIXEL;
   else {
     throw(CASTBEEXCEPT badEvaluationException());
   }
   return x;
}

float FeatureValueDimension::getValue(Unit u)
{
   return convert(float(*this), f_unit, u);
}

// real

FeatureValue*
FeatureValueReal::operator+(const FeatureValueInt &value) const
{
  return value + f_value ;
}

FeatureValue*
FeatureValueReal::operator-(const FeatureValueInt &value) const
{
  return new FeatureValueReal(f_value - (int)value);
}

FeatureValue*
FeatureValueReal::operator*(const FeatureValueInt &value) const
{
  return value * f_value ;
}

FeatureValue*
FeatureValueReal::operator/(const FeatureValueInt &value) const
{
  return value.rdiv(f_value);
}


FeatureValue*
FeatureValueReal::operator+(const FeatureValueReal &value) const
{
  return value + f_value ;
}

FeatureValue*
FeatureValueReal::operator-(const FeatureValueReal &value) const
{
  return new FeatureValueReal(f_value - value.f_value) ;
}

FeatureValue*
FeatureValueReal::operator*(const FeatureValueReal &value) const
{
  return value * f_value ;
}

FeatureValue*
FeatureValueReal::operator/(const FeatureValueReal &value) const
{
  return value.rdiv(f_value);
}


FeatureValue*
FeatureValueReal::operator+(const FeatureValueDimension &value) const
{
  return value + f_value ;
}

FeatureValue*
FeatureValueReal::operator-(const FeatureValueDimension &value) const
{
  return value.rsub(f_value);
}

FeatureValue*
FeatureValueReal::operator*(const FeatureValueDimension &value) const
{
  return value * f_value ;
}

FeatureValue*
FeatureValueReal::operator/(const FeatureValueDimension &value) const
{
  return value.rdiv(f_value);
}


FeatureValue*
FeatureValueReal::operator+(const FeatureValueExpression &value) const
{
  FeatureValue *eval = value.evaluate();
  FeatureValue *return_val = *eval + f_value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueReal::operator-(const FeatureValueExpression &value) const
{
  FeatureValue *eval   = value.evaluate();
  FeatureValue *result = eval->rsub( f_value );
  delete eval;
  return result ;
}

FeatureValue*
FeatureValueReal::operator*(const FeatureValueExpression &value) const
{
  FeatureValue *eval = value.evaluate();
  FeatureValue *return_val = *eval * f_value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueReal::operator/(const FeatureValueExpression &value) const
{
  FeatureValue *eval   = value.evaluate();
  FeatureValue *result = eval->rdiv(f_value) ;
  delete eval;
  return result ;
}


// int
FeatureValue*
FeatureValueInt::operator+(const FeatureValueInt &value) const
{
  return value + f_value ;
}

FeatureValue*
FeatureValueInt::operator-(const FeatureValueInt &value) const
{
  return value.rsub(f_value);
}

FeatureValue*
FeatureValueInt::operator*(const FeatureValueInt &value) const
{
  return value * f_value ;
}

FeatureValue*
FeatureValueInt::operator/(const FeatureValueInt &value) const
{
  return value.rdiv(f_value);
}


FeatureValue*
FeatureValueInt::operator+(const FeatureValueReal &value) const
{
  return value + f_value ;
}

FeatureValue*
FeatureValueInt::operator-(const FeatureValueReal &value) const
{
  return value.rsub(f_value);
}

FeatureValue*
FeatureValueInt::operator*(const FeatureValueReal &value) const
{
  return value * f_value ;
}

FeatureValue*
FeatureValueInt::operator/(const FeatureValueReal &value) const
{
  return value.rdiv(f_value);
}


FeatureValue*
FeatureValueInt::operator+(const FeatureValueDimension &value) const
{
  return value + f_value ;
}

FeatureValue*
FeatureValueInt::operator-(const FeatureValueDimension &value) const
{
  return value.rsub(f_value);
}

FeatureValue*
FeatureValueInt::operator*(const FeatureValueDimension &value) const
{
  return value * f_value ;
}

FeatureValue*
FeatureValueInt::operator/(const FeatureValueDimension &value) const
{
  return value.rdiv(f_value);
}


FeatureValue*
FeatureValueInt::operator+(const FeatureValueExpression &value) const
{
  FeatureValue *eval = value.evaluate();
  FeatureValue *return_val = *eval + f_value ;
  delete eval;
  return return_val ;
}

FeatureValue*
FeatureValueInt::operator-(const FeatureValueExpression &value) const
{
  FeatureValue *eval   = value.evaluate();
  FeatureValue *result = eval->rsub(f_value) ;
  delete eval;
  return result;
}

FeatureValue*
FeatureValueInt::operator*(const FeatureValueExpression &value) const
{
  FeatureValue *eval = value.evaluate();
  FeatureValue *return_val = *eval * f_value ;
  delete eval;
  return return_val ;
}

FeatureValue*
FeatureValueInt::operator/(const FeatureValueExpression &value) const
{
  FeatureValue *eval = value.evaluate();
  FeatureValue *result = eval->rdiv(f_value) ;
  delete eval;
  return result ;
}

// expression
FeatureValue*
FeatureValueExpression::operator+(const FeatureValueInt &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval + value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator-(const FeatureValueInt &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval - value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator*(const FeatureValueInt &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval * value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator/(const FeatureValueInt &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval / value ;
  delete eval ;
  return return_val ;
}


FeatureValue*
FeatureValueExpression::operator+(const FeatureValueReal &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval + value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator-(const FeatureValueReal &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval - value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator*(const FeatureValueReal &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval * value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator/(const FeatureValueReal &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval / value ;
  delete eval ;
  return return_val ;
}


FeatureValue*
FeatureValueExpression::operator+(const FeatureValueDimension &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval + value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator-(const FeatureValueDimension &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval - value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator*(const FeatureValueDimension &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval * value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator/(const FeatureValueDimension &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval / value ;
  delete eval ;
  return return_val ;
}


FeatureValue*
FeatureValueExpression::operator+(const FeatureValueExpression &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval + value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator-(const FeatureValueExpression &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval - value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator*(const FeatureValueExpression &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval * value ;
  delete eval ;
  return return_val ;
}

FeatureValue*
FeatureValueExpression::operator/(const FeatureValueExpression &value) const
{
  FeatureValue *eval = evaluate();
  FeatureValue *return_val = *eval / value ;
  delete eval ;
  return return_val ;
}


// dimension
FeatureValue*
FeatureValueDimension::operator+(const FeatureValueInt &value) const
{
  FeatureValue *intermediate =  *f_value + value ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator-(const FeatureValueInt &value) const
{
  FeatureValue *intermediate =  *f_value - value ;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator*(const FeatureValueInt &value) const
{
  FeatureValue *intermediate =  *f_value * value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator/(const FeatureValueInt &value) const
{
  FeatureValue *intermediate =  *f_value / value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}


FeatureValue*
FeatureValueDimension::operator+(const FeatureValueReal &value) const
{
  FeatureValue *intermediate =  *f_value + value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator-(const FeatureValueReal &value) const
{
  FeatureValue *intermediate =  *f_value - value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator*(const FeatureValueReal &value) const
{
  FeatureValue *intermediate =  *f_value * value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator/(const FeatureValueReal &value) const
{
  FeatureValue *intermediate =  *f_value / value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}


FeatureValue*
FeatureValueDimension::operator+(const FeatureValueDimension &value) const
{
  FeatureValue *i1 = value.doConvert(f_unit);
  FeatureValue *i2 = f_value->doConvert(f_unit);
  FeatureValue *i3 = *i1 + *i2 ;
  delete i2 ;
  delete i1 ;
  return new FeatureValueDimension(i3, f_unit) ;
}

FeatureValue*
FeatureValueDimension::operator-(const FeatureValueDimension &value) const
{
  FeatureValue *intermediate =  *f_value - value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator*(const FeatureValueDimension &value) const
{
  FeatureValue *intermediate =  *f_value * value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator/(const FeatureValueDimension &value) const
{
  FeatureValue *intermediate =  *f_value / value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}


FeatureValue*
FeatureValueDimension::operator+(const FeatureValueExpression &value) const
{
  FeatureValue *intermediate =  *f_value + value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator-(const FeatureValueExpression &value) const
{
  FeatureValue *intermediate =  *f_value - value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator*(const FeatureValueExpression &value) const
{
  FeatureValue *intermediate =  *f_value * value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}

FeatureValue*
FeatureValueDimension::operator/(const FeatureValueExpression &value) const
{
  FeatureValue *intermediate =  *f_value / value;
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result ;
}


/* -------- rdiv, rsub -------- */

FeatureValue *
FeatureValue::rdiv(const FeatureValue &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rsub(const FeatureValue &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rdiv(const FeatureValueInt &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rsub(const FeatureValueInt &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rdiv(const FeatureValueReal &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rsub(const FeatureValueReal &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rdiv(const FeatureValueExpression &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rsub(const FeatureValueExpression &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rdiv(const FeatureValueDimension &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rsub(const FeatureValueDimension &) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rdiv(const int) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}
FeatureValue *
FeatureValue::rsub(const int) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}
FeatureValue *
FeatureValue::rdiv(const float) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0;
}
FeatureValue *
FeatureValue::rsub(const float) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}

/* --------  FeatureValueInt rdiv, rsub -------- */

FeatureValue *
FeatureValueInt::rdiv(const FeatureValue &f) const
{
  return f / f_value;
}
FeatureValue *
FeatureValueInt::rsub(const FeatureValue &f) const
{
  return f - f_value ;
}
FeatureValue *
FeatureValueInt::rdiv(const FeatureValueInt &i) const
{
  return i / f_value ;
}
FeatureValue *
FeatureValueInt::rsub(const FeatureValueInt &i) const
{
  return i - f_value ;
}
FeatureValue *
FeatureValueInt::rdiv(const FeatureValueReal &r) const
{
  return r / f_value ;
}
FeatureValue *
FeatureValueInt::rsub(const FeatureValueReal &r) const
{
  return r - f_value ;
}
FeatureValue *
FeatureValueInt::rdiv(const FeatureValueExpression &e) const
{
  return e / f_value ;
}
FeatureValue *
FeatureValueInt::rsub(const FeatureValueExpression &e) const
{
  return e - f_value ;
}
FeatureValue *
FeatureValueInt::rdiv(const FeatureValueDimension &d) const
{
  return d / f_value ;
}
FeatureValue *
FeatureValueInt::rsub(const FeatureValueDimension &d) const
{
  return d - f_value ;
}
FeatureValue *
FeatureValueInt::rdiv(const int i) const
{
  return new FeatureValueInt(i / f_value);
}
FeatureValue *
FeatureValueInt::rsub(const int i) const
{
  return new FeatureValueInt(i - f_value);
}
FeatureValue *
FeatureValueInt::rdiv(const float f) const
{
  return new FeatureValueReal(f / f_value);
}
FeatureValue *
FeatureValueInt::rsub(const float f) const
{
  return new FeatureValueReal(f - f_value);
}
/* --------  FeatureValueReal rdiv, rsub -------- */
FeatureValue *
FeatureValueReal::rdiv(const FeatureValue &f) const
{
  return f / f_value ;
}
FeatureValue *
FeatureValueReal::rsub(const FeatureValue &f) const
{
  return f - f_value ;
}
FeatureValue *
FeatureValueReal::rdiv(const FeatureValueInt &i) const
{
  return i / f_value ;
}
FeatureValue *
FeatureValueReal::rsub(const FeatureValueInt &i) const
{
  return i - f_value ;
}
FeatureValue *
FeatureValueReal::rdiv(const FeatureValueReal &r) const
{
  return r / f_value ;
}
FeatureValue *
FeatureValueReal::rsub(const FeatureValueReal &r) const
{
  return r - f_value ;
}
FeatureValue *
FeatureValueReal::rdiv(const FeatureValueExpression &e) const
{
  return e / f_value ;
}
FeatureValue *
FeatureValueReal::rsub(const FeatureValueExpression &e) const
{
  return e - f_value ;
}
FeatureValue *
FeatureValueReal::rdiv(const FeatureValueDimension &d) const
{
  return d / f_value ;
}
FeatureValue *
FeatureValueReal::rsub(const FeatureValueDimension &d) const
{
  return d - f_value ;
}
FeatureValue *
FeatureValueReal::rdiv(const int i) const
{
  return new FeatureValueReal(i / f_value);
}
FeatureValue *
FeatureValueReal::rsub(const int i) const
{
  return new FeatureValueReal(i - f_value);
}
FeatureValue *
FeatureValueReal::rdiv(const float f) const
{
  return new FeatureValueReal(f / f_value);
}
FeatureValue *
FeatureValueReal::rsub(const float f) const
{
  return new FeatureValueReal(f - f_value);
}
/* -------- FeatureValueExpression rdiv, rsub -------- */

FeatureValue *
FeatureValueExpression::rdiv(const FeatureValue &f) const
{
  return f / *this ;
}
FeatureValue *
FeatureValueExpression::rsub(const FeatureValue &f) const
{
  return f - *this ;
}
FeatureValue *
FeatureValueExpression::rdiv(const FeatureValueInt &i) const
{
  return i / *this ;
}
FeatureValue *
FeatureValueExpression::rsub(const FeatureValueInt &i) const
{
  return i - *this ;
}
FeatureValue *
FeatureValueExpression::rdiv(const FeatureValueReal &r) const
{
  return r / *this ;
}
FeatureValue *
FeatureValueExpression::rsub(const FeatureValueReal &r) const
{
  return r - *this ;
}
FeatureValue *
FeatureValueExpression::rdiv(const FeatureValueExpression &e) const
{
  return e / *this ;
}
FeatureValue *
FeatureValueExpression::rsub(const FeatureValueExpression &e) const
{
  return e - *this ;
}
FeatureValue *
FeatureValueExpression::rdiv(const FeatureValueDimension &d) const
{
  return d / *this ;
}
FeatureValue *
FeatureValueExpression::rsub(const FeatureValueDimension &d) const
{
  return d - *this ;
}
FeatureValue *
FeatureValueExpression::rdiv(const int i) const
{
  FeatureValue *e = evaluate();
  FeatureValue *intermediate = e->rdiv(i);
  delete e ;
  return intermediate;
}
FeatureValue *
FeatureValueExpression::rsub(const int i) const
{
  FeatureValue *e = evaluate();
  FeatureValue *intermediate = e->rsub(i);
  delete e ;
  return intermediate;
}
FeatureValue *
FeatureValueExpression::rdiv(const float f) const
{
  FeatureValue *e = evaluate();
  FeatureValue *intermediate = e->rdiv(f);
  delete e ;
  return intermediate;
}
FeatureValue *
FeatureValueExpression::rsub(const float f) const
{
  FeatureValue *e = evaluate();
  FeatureValue *intermediate = e->rsub(f);
  delete e ;
  return intermediate;
}
/* -------- FeatureValueDimension rdiv, rsub -------- */
FeatureValue *
FeatureValueDimension::rdiv(const FeatureValue &f) const
{
  return f / *this ;
}
FeatureValue *
FeatureValueDimension::rsub(const FeatureValue &f) const
{
  return f - *this ;
}
FeatureValue *
FeatureValueDimension::rdiv(const FeatureValueInt &i) const
{
  return i / *this ;
}
FeatureValue *
FeatureValueDimension::rsub(const FeatureValueInt &i) const
{
  return i - *this ;
}
FeatureValue *
FeatureValueDimension::rdiv(const FeatureValueReal &r) const
{
  return r / *this ;
}
FeatureValue *
FeatureValueDimension::rsub(const FeatureValueReal &r) const
{
  return r - *this ;
}
FeatureValue *
FeatureValueDimension::rdiv(const FeatureValueExpression &e) const
{
  return e / *this ;
}
FeatureValue *
FeatureValueDimension::rsub(const FeatureValueExpression &e) const
{
  return e - *this ;
}
FeatureValue *
FeatureValueDimension::rdiv(const FeatureValueDimension &d) const
{
  return d / *this ;
}
FeatureValue *
FeatureValueDimension::rsub(const FeatureValueDimension &d) const
{
  return d - *this ;
}
FeatureValue *
FeatureValueDimension::rdiv(const int i) const
{
  // rdiv the value then convert result to our unit type 
  FeatureValue *intermediate = f_value->rdiv(i);
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result;
}

FeatureValue *
FeatureValueDimension::rsub(const int i) const
{
  FeatureValue *intermediate = f_value->rsub(i);
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result;
}
FeatureValue *
FeatureValueDimension::rdiv(const float f) const
{
  FeatureValue *intermediate = f_value->rdiv(f);
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result;
}
FeatureValue *
FeatureValueDimension::rsub(const float f) const
{
  FeatureValue *intermediate = f_value->rsub(f);
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;
  return result;
}

/* -------- convert To -------- */

FeatureValue *
FeatureValue::convertTo(Unit ) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}

FeatureValue *
FeatureValueInt::convertTo(Unit unit) const
{
  return new FeatureValueDimension(new FeatureValueInt(*this), unit);
}

FeatureValue *
FeatureValueReal::convertTo(Unit unit) const
{
  return new FeatureValueDimension(new FeatureValueReal(*this), unit);
}

FeatureValue *
FeatureValueDimension::convertTo(Unit unit) const
{
  if (f_unit == unit)
    return new FeatureValueDimension(*this);
  
  // convert our value to new unit type 
  return new FeatureValueDimension(f_value->convertTo(f_unit, unit),
				   unit);
}

FeatureValue *
FeatureValue::convertTo(Unit, Unit) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
}

FeatureValue *
FeatureValueReal::convertTo(Unit old_unit,
			    Unit new_unit) const
{
  return new FeatureValueReal(convert(f_value,old_unit, new_unit));
}
FeatureValue *
FeatureValueInt::convertTo(Unit old_unit,
			   Unit new_unit) const
{
  return new FeatureValueReal(convert(f_value,old_unit, new_unit));
}

FeatureValue*
FeatureValueDimension::convertTo(Unit old_unit,
				 Unit new_unit) const
{
  FeatureValue *intermediate = f_value->convertTo(old_unit, f_unit);
  FeatureValue *result = intermediate->convertTo(new_unit);
  delete intermediate; 
  return result ;
}

/* -------- FeatureValueDimension::evaluate -------- */

FeatureValue *
FeatureValueDimension::evaluate() const
{
  FeatureValue *intermediate = f_value->evaluate();
  FeatureValue *result = intermediate->convertTo(f_unit);
  delete intermediate;

  return result ;
}

FeatureValue *
FeatureValue::doConvert(Unit) const
{
  throw(CASTBEEXCEPT badEvaluationException());
  return 0 ;
};

FeatureValue *
FeatureValueReal::doConvert(Unit) const
{
  return new FeatureValueReal(*this);
}

FeatureValue *
FeatureValueInt::doConvert(Unit) const
{
  return new FeatureValueInt(*this);
}

FeatureValue*
FeatureValueDimension::doConvert(Unit unit) const
{
  if (unit == f_unit)
    return f_value->doConvert(unit);

  return f_value->convertTo(f_unit, unit);
}

///////////////////////////////////////////////////
// FeatureValueArray
///////////////////////////////////////////////////

FeatureValueArray::FeatureValueArray(const char* nm, int size) :
   FeatureValue(array), pointer_vector<FeatureValue>(size, 0), f_name(strdup(nm))
{
}

FeatureValueArray::FeatureValueArray(const FeatureValueArray& x) :
   FeatureValue(array), pointer_vector<FeatureValue>(x.length(), 0),
   f_name(strdup(x.f_name))
{
   mtry
    {
      for ( unsigned int i=0; i<length(); i++ )
         (*this)[i] = x[i] -> clone();
      return;
    }
   mcatch_any()
    {
      for ( unsigned int i=0; i<length(); i++ )
         delete (*this)[i];
      rethrow;
    }
   end_try ;
}

FeatureValueArray::~FeatureValueArray()
{
   for ( unsigned int i=0; i<length(); i++ )
     delete (*this)[i];

   delete f_name;
}

FeatureValue *
FeatureValueArray::evaluate() const
{
  FeatureValueArray *result = new FeatureValueArray(f_name, length());

  mtry
    {
      for ( unsigned int i=0; i<length(); i++ ) {
        (*result)[i] = (*this)[i] -> evaluate();
      }
      return result;
    }
   mcatch_any()
    {
      delete result;
      rethrow;
    }
   end_try ;
}

ostream& 
FeatureValueArray::print(ostream& out) const
{
   out << f_name << "[\n";

   for ( unsigned int i=0; i<length(); i++ ) {

     if ( (*this)[i] == 0 ) {
        MESSAGE(cerr, form("%d is a null slot", i));
        continue;
     }

     out << "\t" << *((*this)[i]) << "\n";
   }

   out << "]" << endl;
   return out;
}


