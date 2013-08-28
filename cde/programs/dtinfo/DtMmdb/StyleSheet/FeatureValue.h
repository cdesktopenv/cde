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
/* $XConsortium: FeatureValue.h /main/5 1996/08/21 15:50:29 drk $ */
#ifndef _FeatureValue_h
#define _FeatureValue_h

//#include "Feature.h"

#ifndef CDE_NEXT

#else
#include "dti_cc/cc_pvect.h"
#endif

#include <string.h>

#if 0
//  SWM -- COMMENT THIS OUT -- MMDB utility/funcs.h defines this
#ifdef _IBMR2
extern "C" int strcasecmp(const char*, const char*);
#endif
#endif

class FeatureValue;
class FeatureSet;
class Expression;

ostream &operator << (ostream &, const FeatureValue &);

/* **************************************************************
 * class FeatureValue
 * ************************************************************** */

class FeatureValueDimension;
class FeatureValueExpression;
class FeatureValueFeatureSet;
class FeatureValueInt;
class FeatureValueReal;
class FeatureValueString;
class FeatureValueSymbol;
class FeatureValueArray;

class FeatureValue
{
public:
  enum FeatureType { real, integer, string, symbol, expression, featureset, dimension, array } ;
  enum Unit { INCH=0, PICA=1, POINT=2, CM=3, PIXEL=4, NONE=5 };

  FeatureValue	(FeatureType type) : f_type(type) {}
  virtual ~FeatureValue();

  const FeatureType	type()	const { return f_type ; }
  
  virtual FeatureValue *clone() const = 0; /* deep copy */

  virtual FeatureValue *evaluate() const;

  virtual unsigned int operator==(const FeatureValue &) const;
  virtual unsigned int operator==(const FeatureValueInt &) const;
  virtual unsigned int operator==(const FeatureValueString &) const;
  virtual unsigned int operator==(const FeatureValueReal &) const;
  virtual unsigned int operator==(const FeatureValueSymbol &) const;
  
  // produce a new object which is a merge with this object and the parameter
  virtual FeatureValue *merge(const FeatureValue &);

  // these should return a FeatureValue of the appropriate type
  // the operations are resolved vi double dispatching
  // all non-numeric types will use zero for their value unless they 
  // are the denominator in a division operation, then they will use a 
  // value of 1
  virtual FeatureValue *operator+(const FeatureValue&) const ;
  virtual FeatureValue *operator-(const FeatureValue&) const ;
  virtual FeatureValue *operator*(const FeatureValue&) const ;
  virtual FeatureValue *operator/(const FeatureValue&) const ;

  virtual FeatureValue *operator+(const FeatureValueInt&) const ;
  virtual FeatureValue *operator-(const FeatureValueInt&) const ;
  virtual FeatureValue *operator*(const FeatureValueInt&) const ;
  virtual FeatureValue *operator/(const FeatureValueInt&) const ;

  virtual FeatureValue *operator+(const FeatureValueReal&) const ;
  virtual FeatureValue *operator-(const FeatureValueReal&) const ;
  virtual FeatureValue *operator*(const FeatureValueReal&) const ;
  virtual FeatureValue *operator/(const FeatureValueReal&) const ;

  virtual FeatureValue *operator+(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator-(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator*(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator/(const FeatureValueDimension&) const ;

  virtual FeatureValue *operator+(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator-(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator*(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator/(const FeatureValueExpression&) const ;

  virtual FeatureValue *operator+(const int i) const ; /* returns this + i */
  virtual FeatureValue *operator-(const int i) const ; /* returns this - i */
  virtual FeatureValue *operator*(const int i) const ; /* returns this * i */
  virtual FeatureValue *operator/(const int i) const ; /* returns this / i */

  virtual FeatureValue *operator+(const float f) const ; /* returns this + f */
  virtual FeatureValue *operator-(const float f) const ; /* returns this - f */
  virtual FeatureValue *operator*(const float f) const ; /* returns this * f */
  virtual FeatureValue *operator/(const float f) const ; /* returns this / f */

  virtual FeatureValue *rdiv(const FeatureValue &) const ;
  virtual FeatureValue *rsub(const FeatureValue &) const ;

  virtual FeatureValue *rdiv(const FeatureValueInt &) const ;
  virtual FeatureValue *rsub(const FeatureValueInt &) const ;

  virtual FeatureValue *rdiv(const FeatureValueReal &) const ;
  virtual FeatureValue *rsub(const FeatureValueReal &) const ;

  virtual FeatureValue *rdiv(const FeatureValueExpression &) const ;
  virtual FeatureValue *rsub(const FeatureValueExpression &) const ;

  virtual FeatureValue *rdiv(const FeatureValueDimension &) const ;
  virtual FeatureValue *rsub(const FeatureValueDimension &) const ;

  virtual FeatureValue *rdiv(const int) const ;
  virtual FeatureValue *rsub(const int) const ;

  virtual FeatureValue *rdiv(const float) const ;
  virtual FeatureValue *rsub(const float) const ;


  virtual operator float() const;
  virtual operator int() const;
  virtual operator const char *() const;

  virtual operator const FeatureSet * () const ;

  virtual ostream &print(ostream&) const = 0;


  virtual FeatureValue *doConvert(Unit) const ;
  virtual FeatureValue *convertTo(Unit) const ;
  virtual FeatureValue *convertTo(Unit from,
				  Unit to) const ;


private:
  FeatureType	f_type; 
};



class FeatureValueReal : public FeatureValue
{
public:
  FeatureValueReal(float value)
    : FeatureValue(real), f_value(value)
    {}

  FeatureValueReal(const FeatureValueReal &);

  virtual FeatureValue *clone() const; /* deep copy */

  // operators 
  virtual unsigned int operator==(const FeatureValue &) const;
  virtual unsigned int operator==(const FeatureValueInt &) const;
  virtual unsigned int operator==(const FeatureValueString &) const;
  virtual unsigned int operator==(const FeatureValueReal &) const;
  virtual unsigned int operator==(const FeatureValueSymbol &) const;

  virtual FeatureValue *operator+(const FeatureValueInt&) const ;
  virtual FeatureValue *operator-(const FeatureValueInt&) const ;
  virtual FeatureValue *operator*(const FeatureValueInt&) const ;
  virtual FeatureValue *operator/(const FeatureValueInt&) const ;

  virtual FeatureValue *operator+(const FeatureValueReal&) const ;
  virtual FeatureValue *operator-(const FeatureValueReal&) const ;
  virtual FeatureValue *operator*(const FeatureValueReal&) const ;
  virtual FeatureValue *operator/(const FeatureValueReal&) const ;

  virtual FeatureValue *operator+(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator-(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator*(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator/(const FeatureValueDimension&) const ;

  virtual FeatureValue *operator+(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator-(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator*(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator/(const FeatureValueExpression&) const ;

  virtual FeatureValue *operator+(const FeatureValue&) const ;
  virtual FeatureValue *operator-(const FeatureValue&) const ;
  virtual FeatureValue *operator*(const FeatureValue&) const ;
  virtual FeatureValue *operator/(const FeatureValue&) const ;

  virtual FeatureValue *operator+(const int i) const ; /* returns this + i */
  virtual FeatureValue *operator-(const int i) const ; /* returns this - i */
  virtual FeatureValue *operator*(const int i) const ; /* returns this * i */
  virtual FeatureValue *operator/(const int i) const ; /* returns this / i */

  virtual FeatureValue *operator+(const float f) const ; /* returns this + f */
  virtual FeatureValue *operator-(const float f) const ; /* returns this - f */
  virtual FeatureValue *operator*(const float f) const ; /* returns this * f */
  virtual FeatureValue *operator/(const float f) const ; /* returns this / f */

  virtual FeatureValue *rdiv(const FeatureValue &) const ;
  virtual FeatureValue *rsub(const FeatureValue &) const ;

  virtual FeatureValue *rdiv(const FeatureValueInt &) const ;
  virtual FeatureValue *rsub(const FeatureValueInt &) const ;

  virtual FeatureValue *rdiv(const FeatureValueReal &) const ;
  virtual FeatureValue *rsub(const FeatureValueReal &) const ;

  virtual FeatureValue *rdiv(const FeatureValueExpression &) const ;
  virtual FeatureValue *rsub(const FeatureValueExpression &) const ;

  virtual FeatureValue *rdiv(const FeatureValueDimension &) const ;
  virtual FeatureValue *rsub(const FeatureValueDimension &) const ;

  virtual FeatureValue *rdiv(const int) const ;
  virtual FeatureValue *rsub(const int) const ;

  virtual FeatureValue *rdiv(const float) const ;
  virtual FeatureValue *rsub(const float) const ;
  virtual FeatureValue *convertTo(Unit) const ;
  virtual FeatureValue *convertTo(Unit from,
				  Unit to) const ;
  virtual FeatureValue *doConvert(Unit) const ;

  virtual operator float() const;
  virtual operator int() const;
  virtual operator const char *() const;

  virtual ostream& print(ostream&) const;

private:
  float		f_value ;
    
};

class FeatureValueInt : public FeatureValue
{
public:
  FeatureValueInt(int value)
    : FeatureValue(integer), f_value(value)
    {}

  FeatureValueInt(const FeatureValueInt &);

  virtual FeatureValue *clone() const; /* deep copy */

  // operators 

  virtual unsigned int operator==(const FeatureValue &) const;
  virtual unsigned int operator==(const FeatureValueInt &) const;
  virtual unsigned int operator==(const FeatureValueString &) const;
  virtual unsigned int operator==(const FeatureValueReal &) const;
  virtual unsigned int operator==(const FeatureValueSymbol &) const;

  virtual FeatureValue *operator+(const FeatureValue&) const ;
  virtual FeatureValue *operator-(const FeatureValue&) const ;
  virtual FeatureValue *operator*(const FeatureValue&) const ;
  virtual FeatureValue *operator/(const FeatureValue&) const ;

  virtual FeatureValue *operator+(const FeatureValueInt&) const ;
  virtual FeatureValue *operator-(const FeatureValueInt&) const ;
  virtual FeatureValue *operator*(const FeatureValueInt&) const ;
  virtual FeatureValue *operator/(const FeatureValueInt&) const ;

  virtual FeatureValue *operator+(const FeatureValueReal&) const ;
  virtual FeatureValue *operator-(const FeatureValueReal&) const ;
  virtual FeatureValue *operator*(const FeatureValueReal&) const ;
  virtual FeatureValue *operator/(const FeatureValueReal&) const ;

  virtual FeatureValue *operator+(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator-(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator*(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator/(const FeatureValueDimension&) const ;

  virtual FeatureValue *operator+(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator-(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator*(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator/(const FeatureValueExpression&) const ;

  virtual FeatureValue *operator+(const int i) const ; /* returns this + i */
  virtual FeatureValue *operator-(const int i) const ; /* returns this - i */
  virtual FeatureValue *operator*(const int i) const ; /* returns this * i */
  virtual FeatureValue *operator/(const int i) const ; /* returns this / i */

  virtual FeatureValue *operator+(const float f) const ; /* returns this + f */
  virtual FeatureValue *operator-(const float f) const ; /* returns this - f */
  virtual FeatureValue *operator*(const float f) const ; /* returns this * f */
  virtual FeatureValue *operator/(const float f) const ; /* returns this / f */

  virtual FeatureValue *rdiv(const FeatureValue &) const ;
  virtual FeatureValue *rsub(const FeatureValue &) const ;

  virtual FeatureValue *rdiv(const FeatureValueInt &) const ;
  virtual FeatureValue *rsub(const FeatureValueInt &) const ;

  virtual FeatureValue *rdiv(const FeatureValueReal &) const ;
  virtual FeatureValue *rsub(const FeatureValueReal &) const ;

  virtual FeatureValue *rdiv(const FeatureValueExpression &) const ;
  virtual FeatureValue *rsub(const FeatureValueExpression &) const ;

  virtual FeatureValue *rdiv(const FeatureValueDimension &) const ;
  virtual FeatureValue *rsub(const FeatureValueDimension &) const ;

  virtual FeatureValue *rdiv(const int) const ;
  virtual FeatureValue *rsub(const int) const ;

  virtual FeatureValue *rdiv(const float) const ;
  virtual FeatureValue *rsub(const float) const ;

  virtual FeatureValue *convertTo(Unit) const ;
  virtual FeatureValue *convertTo(Unit from, Unit to) const ;
  virtual FeatureValue *doConvert(Unit) const ;

  virtual operator float() const;
  virtual operator int() const;
  virtual operator const char *() const;

  virtual ostream& print(ostream&) const;

private:
  int		f_value ;
    
};

class FeatureValueString : public FeatureValue
{
public:
  FeatureValueString(const char *value)
    : FeatureValue(string), f_value(value)
    {}

  FeatureValueString(const FeatureValueString &);
  ~FeatureValueString();

  virtual FeatureValue *clone() const ; /* deep copy */

  virtual unsigned int operator==(const FeatureValue &) const;
  virtual unsigned int operator==(const FeatureValueInt &) const;
  virtual unsigned int operator==(const FeatureValueString &) const;
  virtual unsigned int operator==(const FeatureValueReal &) const;
  virtual unsigned int operator==(const FeatureValueSymbol &) const;

// ops for string concatenation
// this -> operator+(const FeatureValueExpression&) evaluates 
// the argument and then call this -> operator+(const FeatureValue&).
  virtual FeatureValue *operator+(const FeatureValue&) const ;
  virtual FeatureValue *operator+(const FeatureValueExpression&) const ;

// call FeatureValue::operator+ directly.
  virtual FeatureValue *operator+(const FeatureValueInt&) const ;
  virtual FeatureValue *operator+(const FeatureValueReal&) const ;
  virtual FeatureValue *operator+(const FeatureValueDimension&) const ;

  virtual FeatureValue *operator+(const int i) const ;
  virtual FeatureValue *operator+(const float f) const ;

  virtual operator float() const;
  virtual operator int() const;
  virtual operator const char *() const;

  virtual ostream& print(ostream&) const;

private:

#ifndef CDE_NEXT
  CC_String   f_value ;
#else
  CC_String	f_value ;
#endif
};

class FeatureValueSymbol : public FeatureValue
{
public:
  FeatureValueSymbol(const Symbol &value)
    : FeatureValue(symbol), f_value(value)
    {}
  FeatureValueSymbol(const FeatureValueSymbol&);

  virtual FeatureValue *clone() const ; /* deep copy */

  virtual unsigned int operator==(const FeatureValue &) const;
  virtual unsigned int operator==(const FeatureValueInt &) const;
  virtual unsigned int operator==(const FeatureValueString &) const;
  virtual unsigned int operator==(const FeatureValueReal &) const;
  virtual unsigned int operator==(const FeatureValueSymbol &) const;

  virtual operator float() const;
  virtual operator int() const;
  virtual operator const char *() const;

  virtual ostream& print(ostream&) const;

private:
  Symbol	f_value;
    
};

class FeatureValueExpression : public FeatureValue
{
public:
  FeatureValueExpression(Expression *);
  FeatureValueExpression(const FeatureValueExpression&);
  ~FeatureValueExpression();

  virtual FeatureValue *clone() const ; /* deep copy */

  virtual FeatureValue *evaluate() const;

  // operators 

  virtual FeatureValue *operator+(const FeatureValue&) const ;
  virtual FeatureValue *operator-(const FeatureValue&) const ;
  virtual FeatureValue *operator*(const FeatureValue&) const ;
  virtual FeatureValue *operator/(const FeatureValue&) const ;

  virtual FeatureValue *operator+(const FeatureValueInt&) const ;
  virtual FeatureValue *operator-(const FeatureValueInt&) const ;
  virtual FeatureValue *operator*(const FeatureValueInt&) const ;
  virtual FeatureValue *operator/(const FeatureValueInt&) const ;

  virtual FeatureValue *operator+(const FeatureValueReal&) const ;
  virtual FeatureValue *operator-(const FeatureValueReal&) const ;
  virtual FeatureValue *operator*(const FeatureValueReal&) const ;
  virtual FeatureValue *operator/(const FeatureValueReal&) const ;

  virtual FeatureValue *operator+(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator-(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator*(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator/(const FeatureValueDimension&) const ;

  virtual FeatureValue *operator+(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator-(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator*(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator/(const FeatureValueExpression&) const ;

  virtual FeatureValue *operator+(const int i) const ; /* returns this + i */
  virtual FeatureValue *operator-(const int i) const ; /* returns this - i */
  virtual FeatureValue *operator*(const int i) const ; /* returns this * i */
  virtual FeatureValue *operator/(const int i) const ; /* returns this / i */

  virtual FeatureValue *operator+(const float f) const ; /* returns this + f */
  virtual FeatureValue *operator-(const float f) const ; /* returns this - f */
  virtual FeatureValue *operator*(const float f) const ; /* returns this * f */
  virtual FeatureValue *operator/(const float f) const ; /* returns this / f */

  virtual FeatureValue *rdiv(const FeatureValue &) const ;
  virtual FeatureValue *rsub(const FeatureValue &) const ;

  virtual FeatureValue *rdiv(const FeatureValueInt &) const ;
  virtual FeatureValue *rsub(const FeatureValueInt &) const ;

  virtual FeatureValue *rdiv(const FeatureValueReal &) const ;
  virtual FeatureValue *rsub(const FeatureValueReal &) const ;

  virtual FeatureValue *rdiv(const FeatureValueExpression &) const ;
  virtual FeatureValue *rsub(const FeatureValueExpression &) const ;

  virtual FeatureValue *rdiv(const FeatureValueDimension &) const ;
  virtual FeatureValue *rsub(const FeatureValueDimension &) const ;

  virtual FeatureValue *rdiv(const int) const ;
  virtual FeatureValue *rsub(const int) const ;

  virtual FeatureValue *rdiv(const float) const ;
  virtual FeatureValue *rsub(const float) const ;

  virtual operator float() const;
  virtual operator int() const;
  virtual operator const char *() const;

  virtual ostream& print(ostream&) const;

private:
  Expression	*f_value;
};

class FeatureValueFeatureSet : public FeatureValue
{
public:
  FeatureValueFeatureSet(FeatureSet *);
  FeatureValueFeatureSet(const FeatureValueFeatureSet&);
  ~FeatureValueFeatureSet();

  const FeatureSet     *value()	const { return f_value ; }

  virtual FeatureValue *evaluate() const; 

  virtual FeatureValue *clone() const; /* deep copy */

  virtual FeatureValue *merge(const FeatureValue &f);

  virtual ostream& print(ostream&) const;

  virtual operator const FeatureSet *() const ;

private:
  FeatureSet   *f_value;
};

#ifndef CDE_NEXT
typedef pointer_vector<FeatureValue> ArrayType;
#else
typedef pointer_vector<FeatureValue> ArrayType;
#endif

class FeatureValueArray: public FeatureValue,  public ArrayType
{
public:
  FeatureValueArray(const char* array_name, int size);
  FeatureValueArray(const FeatureValueArray&);
  ~FeatureValueArray();

  virtual FeatureValue *evaluate() const;

  virtual FeatureValue *clone() const 
     { return new FeatureValueArray(*this); }; /* deep copy */

  virtual ostream& print(ostream&) const;

  const char* name() { return f_name; };

private:
  char* f_name;
};


class FeatureValueDimension : public FeatureValue
{
public:
  
  FeatureValueDimension(FeatureValue *value, const char* unit_string);
  FeatureValueDimension(FeatureValue *value, Unit unit);

  FeatureValueDimension(float value, const char* unit_string) ;
  FeatureValueDimension(float value, Unit unit) :
    FeatureValue(dimension), f_value(0),
    f_cachedValue(value), f_unit(unit) {};



  FeatureValueDimension(const FeatureValueDimension&);
  ~FeatureValueDimension();

  virtual FeatureValue *clone() const; /* deep copy */

  virtual FeatureValue *evaluate() const ;

  // operators 
  virtual FeatureValue *operator+(const FeatureValue&) const ;
  virtual FeatureValue *operator-(const FeatureValue&) const ;
  virtual FeatureValue *operator*(const FeatureValue&) const ;
  virtual FeatureValue *operator/(const FeatureValue&) const ;

  virtual FeatureValue *operator+(const FeatureValueInt&) const ;
  virtual FeatureValue *operator-(const FeatureValueInt&) const ;
  virtual FeatureValue *operator*(const FeatureValueInt&) const ;
  virtual FeatureValue *operator/(const FeatureValueInt&) const ;

  virtual FeatureValue *operator+(const FeatureValueReal&) const ;
  virtual FeatureValue *operator-(const FeatureValueReal&) const ;
  virtual FeatureValue *operator*(const FeatureValueReal&) const ;
  virtual FeatureValue *operator/(const FeatureValueReal&) const ;

  virtual FeatureValue *operator+(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator-(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator*(const FeatureValueDimension&) const ;
  virtual FeatureValue *operator/(const FeatureValueDimension&) const ;

  virtual FeatureValue *operator+(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator-(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator*(const FeatureValueExpression&) const ;
  virtual FeatureValue *operator/(const FeatureValueExpression&) const ;

  virtual FeatureValue *operator+(const int i) const ; /* returns this + i */
  virtual FeatureValue *operator-(const int i) const ; /* returns this - i */
  virtual FeatureValue *operator*(const int i) const ; /* returns this * i */
  virtual FeatureValue *operator/(const int i) const ; /* returns this / i */

  virtual FeatureValue *operator+(const float f) const ; /* returns this + f */
  virtual FeatureValue *operator-(const float f) const ; /* returns this - f */
  virtual FeatureValue *operator*(const float f) const ; /* returns this * f */
  virtual FeatureValue *operator/(const float f) const ; /* returns this / f */

  virtual FeatureValue *rdiv(const FeatureValue &) const ;
  virtual FeatureValue *rsub(const FeatureValue &) const ;

  virtual FeatureValue *rdiv(const FeatureValueInt &) const ;
  virtual FeatureValue *rsub(const FeatureValueInt &) const ;

  virtual FeatureValue *rdiv(const FeatureValueReal &) const ;
  virtual FeatureValue *rsub(const FeatureValueReal &) const ;

  virtual FeatureValue *rdiv(const FeatureValueExpression &) const ;
  virtual FeatureValue *rsub(const FeatureValueExpression &) const ;

  virtual FeatureValue *rdiv(const FeatureValueDimension &) const ;
  virtual FeatureValue *rsub(const FeatureValueDimension &) const ;

  virtual FeatureValue *rdiv(const int) const ;
  virtual FeatureValue *rsub(const int) const ;

  virtual FeatureValue *rdiv(const float) const ;
  virtual FeatureValue *rsub(const float) const ;

  virtual FeatureValue *convertTo(Unit) const ;
  virtual FeatureValue *convertTo(Unit from, Unit to) const ;
  virtual FeatureValue *doConvert(Unit) const ;

  virtual operator float()  const;
  virtual operator int()  const;
  virtual operator const char *()  const;

  float getValue(Unit);

  FeatureValue *value() { return f_value ; }
  Unit		unit()	{ return f_unit ; }

  virtual ostream& print(ostream&) const;

private:
  FeatureValue *f_value ;
  float         f_cachedValue;
  Unit		f_unit ;

private:
  float convert(float y, Unit dimensionOfy, Unit dimensionOfReturn);
  Unit convertToUnit(const char*);

};
#endif /* _FeatureValue_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
