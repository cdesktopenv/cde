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
// $TOG: UAS_OQLParser.C /main/8 1998/04/17 11:51:31 mgreess $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#if defined(__uxp__) || defined(USL) || defined(linux)
#include <string.h>
#else
#include <strings.h>
#endif
#include <iostream>
using namespace std;
#include <assert.h>

#include "Exceptions.hh"
#include "UAS_OQLParser.hh"

#include "Managers/CatMgr.hh"
#include "Registration.hh"


#include <stdio.h>

/*
  Language:

  E -> T
    -> T 'or' E
    -> T 'xor' E

  T -> F
    -> F 'and' T
    
  F -> 'not' F
    -> P
    -> '(' E ')' [weight]

  P -> W 'near' W [within]
    -> W 'before' W [within]
    -> W

  W -> Word [completion] [weight] [W]
    -> Q [weight] [W]

  Q -> " text "
 */

UAS_OQLParser::UAS_OQLParser()
: fSymbol(NIL), f_input_ptr(NULL), fIndx(0)
{
}

UAS_String
UAS_OQLParser::validate(UAS_String &seql)
{
    return seql;
}

#ifdef DEBUG
static void
dump_stack(Stack<UAS_String> &stack)
{
    fprintf(stderr, "(DEBUG) %d strings found in OQLParser stack\n",
							stack.entries());
    while (stack.entries() > 0) {
	int level = stack.entries();
	UAS_String str = stack.pop();
	fprintf(stderr, "\t%d\t%s\n", level, (char*)str);
    }
}
#endif

UAS_String
UAS_OQLParser::parse(const char *oql)
{
    if (oql == NULL || *oql == '\0') // garbage input
	throw(CASTEXCEPT Exception());

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) OQL=%s\n", oql);
#endif

    UAS_String seql; // query string specific to each search engine

#ifdef DEBUG
    // allow entering engine-specific queries directly for testing
    // purposes. if it starts with a '#' assume a raw query and pass
    // it back
    if (*oql == '#'){
	if (*(++oql) == '\0') // strip of leading '#'
	    throw(CASTEXCEPT Exception());
	seql = oql;

	cerr << "(DEBUG) SEQL=" << (char*)seql << '\n' << flush;
	return seql;
  }
#endif    

    // initialize input parameters 
    f_input_ptr = oql;

    mtry {
	GetSymbol();
	ERecognize();
    }
    mcatch_any()
    {
#ifdef DEBUG
	fprintf(stderr, "(WARNING) OQLParser could not parse expression\n");
#endif
	rethrow;
    } 
    end_try ;

    // NOTE: also check to see if entire string has been parsed 

    if (fSymbol != NIL){
	// NOTE: bad query - error message to user required 
#ifdef DEBUG
	cerr << "(ERROR) bad final symbol" << '\n' << flush;
//	don't abort, just throw the exception
//	abort();
#endif
        // clean up the stack
        seql = f_stack.pop();
	throw(CASTUASEXCEPT UAS_Exception(
             (char*)UAS_String(CATGETS(Set_Messages, 41,
             "Invalid characters in search string."))));
    } else if (f_stack.entries() != 1){
	// NOTE: bad query - error message to user required 
#ifdef DEBUG
	cerr << "(ERROR) stack not 1" << '\n' << flush;
	dump_stack(f_stack);
//	don't abort, just throw the exception
//	abort();
#endif
        seql = f_stack.pop();
	throw(CASTEXCEPT Exception());
    }

    seql = f_stack.pop();
    if (*((char*)seql) == '\0') // empty string
	throw(CASTEXCEPT Exception());

    assert(f_stack.entries() == 0);

    seql = validate(seql);

#ifdef DEBUG
    cerr << "(DEBUG) SEQL=" << (char*)seql << '\n' << flush;
#endif

    return seql ;
}

void
UAS_OQLParser::ERecognize()
{
    // E 

    TRecognize();		// E -> T 
    // E -> T or E 
    if (fSymbol == OR){
	GetSymbol();	 
	ERecognize();

	f_arg2 = f_stack.pop();
	f_arg1 = f_stack.pop();
	f_comp = se_construct(OR, f_arg1, f_arg2, "");
	f_stack.push(f_comp);
      } else if (fSymbol == XOR){
	GetSymbol();
	ERecognize();

	f_arg2 = f_stack.pop();
	f_arg1 = f_stack.pop();
	f_comp = se_construct(XOR, f_arg1, f_arg2, "");
	f_stack.push(f_comp);
      }
}

void
UAS_OQLParser::TRecognize()
{
    // T -> F 
    FRecognize();

    // T -> F 'and' T 
    if (fSymbol == AND){
	GetSymbol();
	TRecognize();

	f_arg2 = f_stack.pop();
	f_arg1 = f_stack.pop();
	f_comp = se_construct(AND, f_arg1, f_arg2, "");
	f_stack.push(f_comp);
    }
}

void
UAS_OQLParser::FRecognize()
{
    // F -> 'not' F
    if (fSymbol == NOT){
	GetSymbol();
	FRecognize();

	f_arg1 = f_stack.pop();
	f_comp = se_construct(NOT, f_arg1, "", "");
	f_stack.push(f_comp);
    } else if ((fSymbol == QUOTE) || (fSymbol == WORD)){
    // F -> P
      PRecognize();
    } else if (fSymbol == LPAREN) {
	// F -> ( E ) [weight]
	GetSymbol();
	ERecognize();
	if (fSymbol == RPAREN)
	  GetSymbol();
	else {
	  throw(CASTEXCEPT Exception());
	}
	WeightRecognize();
    } 
}

void
UAS_OQLParser::WeightRecognize()
{
    if (fSymbol == WEIGHT){
	GetNumber();
	// if number was invalid, a zero string is pushed which
	// actually works...in the future we will throw an exception
	// from GetNumber to signal an error 
	f_stack.push(fBuffer);

	// stack should be number, expression
	// expression is on top of stack
	f_arg2 = f_stack.pop();
	f_arg1 = f_stack.pop();
	f_comp = se_construct(WEIGHT, f_arg1, f_arg2, ""); // expression, number
	f_stack.push(f_comp);

	GetSymbol();
    }
}

void
UAS_OQLParser::QRecognize()
{
  // Q -> " text "
  char c ;
  fIndx = 0 ;
#if 0
  while ((c = *f_input_ptr) &&
	 (c != '"')){
    f_input_ptr++ ;
    fBuffer[fIndx++] = c ;
    if (!(fIndx < sizeof(fBuffer)))
      throw(CASTEXCEPT Exception());
  }
#else
    while ((c = *f_input_ptr++) && (c != '"')) {
	fBuffer[fIndx++] = c ;
	if (fIndx == sizeof(fBuffer))
	    throw(CASTEXCEPT Exception());
    }
#endif
    fBuffer[fIndx] = 0 ;

    assert( c == 0 || c == '"' );

#ifdef FULCRUM_HACK // what is this hack for?
  int len = fIndx ;
  // bad hack - for query editor, if last character is a '*'
  // then insert fulcrum completion operator
  int insert_completion = 0;
  if (fBuffer[fIndx - 1] == '*')
    {
      insert_completion  = 1 ;
      ON_DEBUG(cerr << "got one" << endl);
      len += 4 ; // for \C-w 
    }

  char *tmpstr = new char [len + 1] ;
  strcpy(tmpstr, fBuffer);
  
  if (insert_completion)
    {
      tmpstr[fIndx - 1] = 0 ;	// get rid of the * 
      strcat(tmpstr, "\\C-w");
    }

  f_stack.push(tmpstr);
#else
  f_stack.push(fBuffer);
#endif

  f_arg1 = f_stack.pop();
  f_comp = se_construct(QUOTED_PHRASE, f_arg1, "", "");
  f_stack.push(f_comp);
  
  GetSymbol();
}

void
UAS_OQLParser::CompletionRecognize()
{
    if (fSymbol == COMPLETION){
	f_arg1 = f_stack.pop();
	f_comp = se_construct(COMPLETION, f_arg1, "", "");
	f_stack.push(f_comp);

	GetSymbol();
    }
}

void
UAS_OQLParser::PRecognize()
{
    //  P -> W P
    if ((fSymbol == WORD) || (fSymbol == QUOTE)) {
	WRecognize();

	while ((fSymbol == NEAR) || (fSymbol == BEFORE)){
	    OQL_Token type = fSymbol;
	    GetSymbol();
	    WRecognize();
	    if (fSymbol == WITHIN) {
		GetNumber();
		f_stack.push(fBuffer);

		f_arg3 = f_stack.pop();
		f_arg2 = f_stack.pop();
		f_arg1 = f_stack.pop();
		if (type == NEAR) {
		    f_comp = se_construct(NEAR_WITHIN, f_arg1, f_arg2, f_arg3);
		}
		else {
		    assert( type ==  BEFORE );
		    f_comp = se_construct(BEFORE_WITHIN, f_arg1, f_arg2, f_arg3);
		}
		f_stack.push(f_comp);
		GetSymbol();
	    }
	    else {
		f_arg2 = f_stack.pop();
		f_arg1 = f_stack.pop();
		if (type == NEAR) {
		    f_comp = se_construct(NEAR, f_arg1, f_arg2, "");
		}
		else {
		    assert( type == BEFORE );
		    f_comp = se_construct(BEFORE, f_arg1, f_arg2, "");
		}
		f_stack.push(f_comp);
	    }
	}
    }
}

void
UAS_OQLParser::WRecognize()
{
    // W [completion] [weight] 
    if (fSymbol == WORD) {
	f_stack.push(fBuffer);
	GetSymbol();
	CompletionRecognize();
	WeightRecognize();
    } else if (fSymbol == QUOTE) {
	QRecognize();
	WeightRecognize();
    } else {
	throw(CASTEXCEPT Exception());
    }

    if ((fSymbol == WORD) || (fSymbol == QUOTE)){
	WRecognize();

	f_arg2 = f_stack.pop();
	f_arg1 = f_stack.pop();
	f_comp = se_construct(PHRASE, f_arg1, f_arg2, "");
	f_stack.push(f_comp);
    }
}

void
UAS_OQLParser::GetNumber()
{
    char c ;
    fIndx = 0 ;
    while (*f_input_ptr == ' ')	// skip whitespace
      f_input_ptr++ ;
    
    while (c = *f_input_ptr){
	f_input_ptr++ ;
	
	if ((c < '0') || (c > '9')){
	    --f_input_ptr ;	// put back character 
	    break ;		// exit loop 
	}

	fBuffer[fIndx++] = c ;
	assert(fIndx < sizeof(fBuffer));
    }
    fBuffer[fIndx] = 0 ;
//    assert(fIndx > 0);		// NOTE: throw exception if no value found for number 
  
    if (fIndx == 0)
      fSymbol = ERROR ;		// temporary error handling 
    else
      fSymbol = NUMBER ;

}

void
UAS_OQLParser::GetSymbol()
{
    // lots of room to optimize if we care [ jbm ] 
    char c ;
    fIndx = 0 ;

    c = *f_input_ptr ;
    while (c == ' ' || c == '\t' || c == '\n' || c == '\\') {
	f_input_ptr++ ;
    
	// handle escape chars
	if(c == '\\')
	  f_input_ptr++ ;

	c = *f_input_ptr ;
      }

    while (c = *f_input_ptr) {
	f_input_ptr++ ;

	if (c == '\\')
	  {
	    --f_input_ptr ;
	    break;
	  }

	if (c == '('){
	    if (fIndx == 0) {
		fSymbol = LPAREN ;
		return ;	//  function exit
	    }
	    --f_input_ptr;	// putback character 
	    break ;		// loop exit 
	}
	if (c == ')') {
	    if (fIndx == 0){
		fSymbol = RPAREN ;
		return ;	//  function exit
	    }
	    --f_input_ptr ;	// putback character 
	    break ;		// loop exit 
	}
	
	if (c == '*') {
	    if (fIndx == 0){
		fSymbol = COMPLETION ;
		return ;
	    }
	    --f_input_ptr ;
	    break;
	}
	
	if (c == '"'){
	  if (fIndx == 0){
	    fSymbol = QUOTE ;
	    return ;
	  }
	  --f_input_ptr ;
	  break;
	}

	// single char version of operators

	if (c == '#'){
	    if (fIndx == 0){
		fSymbol = WEIGHT ;
		return ;	// function exit 
	    }
	    --f_input_ptr ;	// put back character
	    break ;		// loop exit 
	}

	if (c == '&'){
	    if (fIndx == 0){
		fSymbol = AND ;
		return ;	// function exit 
	    }
	    --f_input_ptr ;	// put back character
	    break ;		// loop exit 
	}

	if (c == '|'){
	    if (fIndx == 0){
		fSymbol = OR ;
		return ;	// function exit 
	    }
	    --f_input_ptr ;	// put back character
	    break ;		// loop exit 
	}

	if (c == '!'){
	    if (fIndx == 0){
		fSymbol = NOT ;
		return ;	// function exit 
	    }
	    --f_input_ptr ;	// put back character
	    break ;		// loop exit 
	}

	if (c == '%'){
	  if (fIndx == 0){
	    fSymbol = NEAR ;
	    return ;
	  }
	  --f_input_ptr ;
	  break;
	}

	if (c == '<'){
	  if (fIndx == 0){
	    fSymbol = BEFORE ;
	    return ;
	  }
	  --f_input_ptr ;
	  break;
	}

	if (c == '@'){
	  if (fIndx == 0){
	    fSymbol = WITHIN ;
	    return ;
	  }
	  --f_input_ptr ;
	  break;
	}
	  

	if (c == ' ' || c == '\t' || c == '\n')
	  break ;		// loop exit 
	
	fBuffer[fIndx++] = isascii(c) ? tolower(c) : c;

	// assert(fIndx < sizeof(fBuffer)); // NOTE: exception condition or
	// error handling reqd 
	if (!(fIndx < sizeof(fBuffer)))
	  throw(CASTEXCEPT Exception()); // protect overflow
    }
    fBuffer[fIndx] = 0 ;
 

   // messy 
 
    if (fIndx == 0)
      fSymbol = NIL ;
    else if (!strcmp(fBuffer, "and"))
      fSymbol = AND ;
    else if (!strcmp(fBuffer, "or"))
      fSymbol = OR ;
    else if (!strcmp(fBuffer, "not"))
      fSymbol = NOT ;
    else if (!strcmp(fBuffer, "weight"))
      fSymbol = WEIGHT ;
    else if (!strcmp(fBuffer, "within"))
      fSymbol = WITHIN ;
    else if (!strcmp(fBuffer, "near"))
      fSymbol = NEAR ;
    else if (!strcmp(fBuffer, "before"))
      fSymbol = BEFORE ;
    else if (!strcmp(fBuffer, "xor"))
      fSymbol = XOR ;
    else
      fSymbol = WORD ;
}

unsigned int
UAS_OQLParser::determine_caps()
{
    UAS_String word = "olias";
    UAS_String quoted_phrase = "olias browser";
    UAS_String digits = "20";

    unsigned int caps = 0;

    caps |= 0x01 << OQL_AND;
    mtry {
	se_construct(AND, word, word, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_AND);
    }
    end_try;

    caps |= 0x01 << OQL_OR;
    mtry {
	se_construct(OR, word, word, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_OR);
    }
    end_try;

    caps |= 0x01 << OQL_XOR;
    mtry {
	se_construct(XOR, word, word, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_XOR);
    }
    end_try;

    caps |= 0x01 << OQL_NOT;
    mtry {
	se_construct(NOT, word, "", "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_NOT);
    }
    end_try;

    caps |= 0x01 << OQL_WEIGHT;
    mtry {
	se_construct(WEIGHT, word, digits, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_WEIGHT);
    }
    end_try;

    caps |= 0x01 << OQL_COMPLETION;
    mtry {
	se_construct(COMPLETION, word, "", "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_COMPLETION);
    }
    end_try;

    caps |= 0x01 << OQL_PHRASE;
    mtry {
	se_construct(PHRASE, word, word, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_PHRASE);
    }
    end_try;

    caps |= 0x01 << OQL_QUOTED_PHRASE;
    mtry {
	se_construct(QUOTED_PHRASE, quoted_phrase, "", "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_QUOTED_PHRASE);
    }
    end_try;

    caps |= 0x01 << OQL_NEAR;
    mtry {
	se_construct(NEAR, word, word, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_NEAR);
    }
    end_try;

    caps |= 0x01 << OQL_BEFORE;
    mtry {
	se_construct(BEFORE, word, word, "");
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_BEFORE);
    }
    end_try;

    caps |= 0x01 << OQL_NEAR_WITHIN;
    mtry {
	se_construct(NEAR_WITHIN, word, word, digits);
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_NEAR_WITHIN);
    }
    end_try;

    caps |= 0x01 << OQL_BEFORE_WITHIN;
    mtry {
	se_construct(BEFORE_WITHIN, word, word, digits);
    }
    mcatch_any() { // OQL parse failed
	caps &= ~(0x01 << OQL_BEFORE_WITHIN);
    }
    end_try;

    return caps;
}
