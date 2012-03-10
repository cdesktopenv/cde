/* $TOG: StyleSheetExceptions.h /main/4 1998/04/17 11:49:51 mgreess $ */
#ifndef _StyleSheetExceptions_h
#define _StyleSheetExceptions_h

#include "Exceptions.hh"
#include "SymTab.h"

#if defined(linux)
#define CASTEXCEPT (Exception*)
#define CASTBCEXCEPT (badCastException*)
#define CASTBEEXCEPT (badEvaluationException*)
#define CASTDPEXCEPT (docParserException*)
#define CASTDPUCEXCEPT (docParserUnexpectedCharacter*)
#define CASTDPUTEXCEPT (docParserUnexpectedTag*)
#define CASTDPUDEXCEPT (docParserUnexpectedData*)
#define CASTDPUEEXCEPT (docParserUnexpectedEof*)
#define CASTSSEXCEPT (StyleSheetException*)
#define CASTSSSEEXCEPT (StyleSheetSyntaxError*)
#define CASTUAEXCEPT (undefinedAttributeException*)
#define CASTUTEXCEPT (unknownTagException*)
#define CASTUVEXCEPT (undefinedVariableException*)
#else
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
#endif

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
