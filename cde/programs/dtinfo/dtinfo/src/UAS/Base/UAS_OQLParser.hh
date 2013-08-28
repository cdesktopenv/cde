// $XConsortium: UAS_OQLParser.hh /main/4 1996/08/21 15:56:33 drk $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __UAS_OQLParser_HH__
#define __UAS_OQLParser_HH__

#include "dti_cc/CC_Stack.h"

#include "UAS_String.hh"
#include "UAS_Exceptions.hh"

class UAS_OQLParser_Exception : public UAS_Exception
{
  public:
    DECLARE_EXCEPTION(UAS_OQLParser_Exception, Exception);

    UAS_OQLParser_Exception(const UAS_String &m) : UAS_Exception(m) {};
    virtual ~UAS_OQLParser_Exception() {};
};

class UAS_Search_Feature_Unavailable : public UAS_OQLParser_Exception
{
  public:
    DECLARE_EXCEPTION(UAS_Search_Feature_Unavailable, Exception);

    UAS_Search_Feature_Unavailable(const UAS_String &m) :
					UAS_OQLParser_Exception(m) {};
};

class UAS_OQLParser
{
public:

  UAS_OQLParser();

  virtual ~UAS_OQLParser() { };

  UAS_String parse(const char *oql);

  unsigned int determine_caps();

  enum OQL_CAPS {
		OQL_AND, OQL_OR, OQL_XOR, OQL_NOT, OQL_WEIGHT,
		OQL_COMPLETION, OQL_PHRASE, OQL_QUOTED_PHRASE,
		OQL_NEAR, OQL_NEAR_WITHIN, OQL_BEFORE, OQL_BEFORE_WITHIN
	};

protected:
  enum OQL_Token {
		ERROR, WORD, LPAREN, RPAREN, AND,
		OR, NIL, NOT, WEIGHT, NUMBER, COMPLETION, PHRASE, QUOTE,
		QUOTED_PHRASE, NEAR, NEAR_WITHIN, BEFORE, BEFORE_WITHIN,
		WITHIN, XOR
	};

  virtual UAS_String se_construct (OQL_Token,
				UAS_String, UAS_String, UAS_String) = 0;

  virtual UAS_String validate(UAS_String & seql);

private:

  void ERecognize();
  void TRecognize();
  void FRecognize();
  void PRecognize();
  void WRecognize();
  void QRecognize();
  void WeightRecognize();
  void CompletionRecognize();

  void GetSymbol();
  void GetNumber();

private:	// variables

  OQL_Token		fSymbol;

  const char*	f_input_ptr; // olias query language string

  char		fBuffer[1024];
  unsigned int	fIndx;

  UAS_String f_arg1, f_arg2, f_arg3, f_comp;

  Stack<UAS_String> f_stack;

};

#endif
