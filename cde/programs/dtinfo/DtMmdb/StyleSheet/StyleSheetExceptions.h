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
/* $TOG: StyleSheetExceptions.h /main/4 1998/04/17 11:49:51 mgreess $ */
#ifndef _StyleSheetExceptions_h
#define _StyleSheetExceptions_h

#include "Exceptions.hh"
#include "SymTab.h"

#define CASTEXCEPT
#define CASTBCEXCEPT
#define CASTBEEXCEPT
#define CASTDPEXCEPT
#define CASTDPUCEXCEPT
#define CASTDPUTEXCEPT
#define CASTDPUDEXCEPT
#define CASTDPUEEXCEPT
#define CASTSSEXCEPT
#define CASTSSSEEXCEPT
#define CASTUAEXCEPT
#define CASTUTEXCEPT
#define CASTUVEXCEPT

class StyleSheetException : public Exception
{
public:
  DECLARE_EXCEPTION(StyleSheetException, Exception);
  virtual ~StyleSheetException();
};

class StyleSheetSyntaxError : public StyleSheetException
{
public:
  DECLARE_EXCEPTION(StyleSheetSyntaxError, StyleSheetException);
  
};

class badEvaluationException : public StyleSheetException
{
public:
  DECLARE_EXCEPTION(badEvaluationException, StyleSheetException);

  virtual ~badEvaluationException();
};

class badCastException : public StyleSheetException
{
public:
  DECLARE_EXCEPTION(badCastException, StyleSheetException);
};

class undefinedVariableException : public badEvaluationException
{
public:
  DECLARE_EXCEPTION(undefinedVariableException, badEvaluationException);

  undefinedVariableException(const Symbol &v)
    : badEvaluationException(),
    f_variable_name(v)
    {}

private:
  Symbol	f_variable_name;

};

class undefinedAttributeException : public badEvaluationException
{
public:
  DECLARE_EXCEPTION(undefinedAttributeException, badEvaluationException);

  undefinedAttributeException(const Symbol &a)
    : badEvaluationException(),
    f_attribute_name(a)
    {}

private:
  Symbol	f_attribute_name;
};

class docParserException : public Exception
{
public:
  DECLARE_EXCEPTION(docParserException, Exception);
};

class unknownTagException : public docParserException
{
public:
  DECLARE_EXCEPTION(unknownTagException, docParserException);
  
};

class docParserUnexpectedCharacter : public docParserException
{
public:
  DECLARE_EXCEPTION(docParserUnexpectedCharacter, docParserException);
  
};
class docParserUnexpectedTag : public docParserException
{
public:
  DECLARE_EXCEPTION(docParserUnexpectedTag, docParserException);
  
};
class docParserUnexpectedData : public docParserException
{
public:
  DECLARE_EXCEPTION(docParserUnexpectedData, docParserException);
  
};
class docParserUnexpectedEof : public docParserException
{
public:
  DECLARE_EXCEPTION(docParserUnexpectedEof, docParserException);
  
};



#endif /* _StyleSheetExceptions_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
