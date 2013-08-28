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
/*	Copyright (c) 1994,1995 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

/*
 * $TOG: QueryGroup.C /main/8 1998/04/17 11:39:15 mgreess $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#define C_QueryGroup
#define C_QueryTerm
#define L_Query

#include "Prelude.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <assert.h>

#include "dti_cc/CC_Stack.h"

// Order of these correspond to operator enum in QueryTerm.hh:
// none, or, xor, and, near, before, proximity, weight, scope, not, completion 
// Bigger numbers mean higher precedence.  (-1 == no precedence)
static int operator_precedence[] =
{
  -1, 0, 0, 1, 2, 2
};

static const char *fulcrum_formats[] =
{ "",                                                // NONE 
  "\\Cu %s %s \\C}",		                     // OR
  "\\Ct\\Cu %s %s \\C} \\Cx\\Ct %s %s \\C}\\C}",     // XOR 
  "\\Ct %s %s \\C}",                                 // AND 
  "\\C%%sy %s %s \\C}",                              // NEAR 
  "\\C%%sp %s %s \\C}",   // BEFORE (%%s is location to insert proximity)
  "%s",                   // PROXIMITY string to insert 
  "\\C%%s$s %s",                                     // WEIGHT
  "%s SCOPE %s",                                     // SCOPE - TBD! 
  "\\Cx %s",                                         // NOT 
  "%s\\C-w",                                         // COMPLETION
  "\\C0p %s \\C}" };                                 // PHRASE 

static const char *infix_formats[] =
#ifdef DtinfoClient
{ "", "(%s | %s)", "(%s ^ %s)", "(%s & %s)",
  "(%s % %s%%s)", "(%s < %s%%s)",  // %%s is loc to insert proximity 
  " @ %s",                        // proximity string to insert
  "%%s # %s", "%s scope %s", "! %s", "%s*", "\"%s\"" };
#else
{ "", "%s or %s", "%s xor %s", "%s and %s",
  "%s near %s%%s", "%s before %s%%s",  // %%s is location to insert proximity 
  " within %s",                        // proximity string to insert
  "%s weight %%s", "%s scope %s", "not %s", "%s*", "\"%s\"" };
#endif

#if 0
static const char *infix_pformats[] =
{ "", "(%s or %s%)", "(%s xor %s%)", "(%s and %s)",
  "(%s near %s%%s)", "(%s before %s%%s)",  // %%s is loc to insert proximity 
  " within %s",                        // proximity string to insert
  "%%s weight %s", "%s scope %s", "not %s", "%s*", "\"%s\"" };
#endif

// Order of these correspond to query_type_t enum in QueryGroup.hh 
static const char **formats[] = { fulcrum_formats, infix_formats };

// The buffer is used to format the query.  As each term is processed
// a NULL-terminated string is tacked on to the end of the buffer.
// When a reduction of a term is necessary (because of term precedence)
// The necessary strings are pulled off the buffer, combined, then the
// new term is added back to the end of the buffer.  In this way the
// buffer acts as a variable length stack.  We use a separate array
// (the real stack) to keep track of the pointers into the buffer.
// The array actually has to keep track of offsets from the start of the
// buffer because the buffer may be dynamically reallocated. 

Buffer QueryGroup::g_buffer;


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

QueryGroup::~QueryGroup()
{
  // The term destructor updates the list ptr which is what lets
  // this loop eventually terminate.
  while (f_term_list)
    delete f_term_list;
}

#ifdef UseQSearch
Set_Not* make_Set_Not(QueryTerm* qterm)
{
    Terminal* term = new Terminal(qterm->term_string());
    /*
	NOTE: If weight is specified, set it here.
	      But if not specified, you can rely on default
	      setting in Terminal constructor. - 6/28/94 kamiya
    */
    if (*qterm->weight()) { // if weight specified, set it
	int weight = atoi(qterm->weight());
	if (weight > 100)
	    weight = 100;
	else if (weight < 1)
	    weight = 1;
	term->weight(weight);
    }
    Set_Not* setnot;
    if (qterm->prefix() && QueryTerm::PFX_NOT_BITS) // not prefix selected
	setnot = new Set_Not(term, 1);
    else
	setnot = new Set_Not(term);

    return setnot;
}


void reduce_stack(Stack<NTerminal*>& operand_stack,
		  Stack<connective_t>& operator_stack)
{
    NTerminal* symbol2 = operand_stack.pop();
    NTerminal* symbol1 = operand_stack.pop();

    connective_t relation = operator_stack.pop();

    NTerminal* new_symbol;

    assert( relation == QueryTerm::C_AND || relation == QueryTerm::C_OR ||
	    relation == QueryTerm::C_XOR );
    Set_And* setand;
    Set_Or*  setor  = 0;
    Set_Xor* setxor = 0;
    switch (relation) {
	case QueryTerm::C_AND :
	    if (symbol1->type() == Set_Not_t)
		setand = new Set_And((Set_Not*)symbol1);
	    else {
		assert( symbol1->type() == Set_And_t );
		setand = (Set_And*)symbol1;
	    }
	    assert( symbol2->type() == Set_Not_t );
	    new_symbol = new Set_And(setand, (Set_Not*)symbol2);
	    break;
	case QueryTerm::C_OR :
	    if (symbol2->type() == Set_Not_t)
		setand = new Set_And((Set_Not*)symbol2);
	    else {
		assert( symbol2->type() == Set_And_t );
		setand = (Set_And*)symbol2;
	    }
	    if (symbol1->type() == Set_Not_t) {
		Set_And* setand = new Set_And((Set_Not*)symbol1);
		setor = new Set_Or(setand);
	    }
	    else if (symbol1->type() == Set_And_t)
		setor = new Set_Or((Set_And*)symbol1);
	    else {
		if (symbol1->type() == Set_Or_t)
		    setor = (Set_Or*)symbol1;
		else {
		    assert( symbol1->type() == Set_Xor_t );
		    setxor = (Set_Xor*)symbol1;
		}
	    }
	    if (setor) {
		assert( setxor == 0 );
		new_symbol = new Set_Or(setor, setand);
	    }
	    else {
		assert( setxor );
		new_symbol = new Set_Or(setxor, setand);
	    }
	    break;
	case QueryTerm::C_XOR :
	    if (symbol2->type() == Set_Not_t)
		setand = new Set_And((Set_Not*)symbol2);
	    else {
		assert( symbol2->type() == Set_And_t );
		setand = (Set_And*)symbol2;
	    }
	    if (symbol1->type() == Set_Not_t) {
		Set_And* setand = new Set_And((Set_Not*)symbol1);
		setor = new Set_Or(setand);
	    }
	    else if (symbol1->type() == Set_And_t)
		setor = new Set_Or((Set_And*)symbol1);
	    else {
		if (symbol1->type() == Set_Or_t)
		    setor = (Set_Or*)symbol1;
		else {
		    assert( symbol1->type() == Set_Xor_t );
		    setxor = (Set_Xor*)symbol1;
		}
	    }
	    if (setor) {
		assert( setxor == 0 );
		new_symbol = new Set_Xor(setor, setand);
	    }
	    else {
		assert( setxor );
		new_symbol = new Set_Xor(setxor, setand);
	    }

	    break;
	default:
	    break;
    }

#ifndef NDEBUG
    printf("(DEBUG) QueryGroup: TYPE %d pushed\n", new_symbol->type());
#endif
    operand_stack.push(new_symbol);
}

// /////////////////////////////////////////////////////////////////
// generate_query
// /////////////////////////////////////////////////////////////////
const NTerminal*
QueryGroup::generate_query()
{
    Stack<NTerminal*> operand_stack;
    Stack<connective_t> operator_stack;

    operator_stack.push(C_NONE);

    for (QueryTerm* term = f_term_list; term; term = term->next()) {
	operand_stack.push(make_Set_Not(term));

	if (term->connective() != C_NONE) {
	    if (operator_precedence[operator_stack.top()] >=
			operator_precedence[term->connective()])
		reduce_stack(operand_stack, operator_stack);
	    operator_stack.push(term->connective());
	}
    }

    while (operator_stack.top() != C_NONE)
	reduce_stack(operand_stack, operator_stack);

    return operand_stack.top();
}

// /////////////////////////////////////////////////////////////////
// generate_readable  generate human-readable query representation
// /////////////////////////////////////////////////////////////////

const char *
QueryGroup::generate_readable()
{
    g_buffer.reset();
    gen_query_internal();
    return (g_buffer.data());
}
#else    
// /////////////////////////////////////////////////////////////////
// generate_query
// /////////////////////////////////////////////////////////////////
const char *
QueryGroup::generate_query (query_type_t query_type)
{
  g_buffer.reset();

  gen_query_internal (query_type);
  return (g_buffer.data());
}
#endif


// /////////////////////////////////////////////////////////////////
// gen_query_internal
// /////////////////////////////////////////////////////////////////

void
QueryGroup::gen_query_internal (query_type_t query_type)
{
  // Only three levels of precedence, so stack needs to be three deep max.
  /*
    Need access to proximity when reducing...might need a proximity stack!?
    proximity is applied to operator
    scope is applied to term

    maybe proximity field should only appear for near/before queries?
  */
  // These variables are the actual stack data storage.  These are
  // never referenced directly.   
  Stack<int> operand_stack;	 // holds offsets into buffer
  Stack<char *> proximity_stack; // offsets not needed, since not in buffer
  Stack<connective_t> operator_stack;

  // These are the stack pointers into the stack storage areas (above). 
  // Stack pointers always point to current top of stack (current element).
  // The top of the stack is always the last element in the array.
  // If the stack is empty, the stack pointers point before the start
  // of the area.  (Hopefully this won't turn out to be dangerous in
  // practice.  At least Purify can catch access errors.)
  // It would be possible to modify the POP and PUSH macros to do a bounds
  // check first before performing the operation. 

  QueryTerm *term = f_term_list;
  assert (term != NULL);

  // Put low precedence item on stack bottom to prevent reduction past it.
  operator_stack.push (C_NONE);

  for (; term != NULL; term = term->next()) {

      // Put the formatted term on the operand stack.
      operand_stack.push (format (query_type, term));
      proximity_stack.push (term->proximity ());

      // If the operator on the stack has greater precedence, then reduce.
      if (term->connective() != C_NONE)
	{
          if (operator_precedence[operator_stack.top ()] >=
	      operator_precedence[term->connective()])
	    reduce (query_type, operand_stack, operator_stack,
		    proximity_stack);
	  // Put the operator on the operator stack.
	  operator_stack.push (term->connective ());
	}
    }

  // Reduce anything left.  Precedence of operators left must be low to high.
  while (operator_stack.top () != C_NONE) 
    reduce (query_type, operand_stack, operator_stack, proximity_stack);

  // Better be only one item remaining on the operand stack.
  assert ( operand_stack.entries() == 1 );
}


// /////////////////////////////////////////////////////////////////
// format - format a query term, return offset of term in buffer
// /////////////////////////////////////////////////////////////////

int
QueryGroup::format (query_type_t query_type, QueryTerm *term)
{
  char *s = (char*)"\001s", *tmp;
  // Need two buffers for alternating writes. 
  // Allocate a buffer with enough room for term plus operators.
  static char *buf[2];
  // 48 is enough space to handle phrase, not, weight, and completion. 
  int n = 48 + strlen (term->term_string());
  int buflen = n;
  buf[0] = (char *) malloc (n);
  buf[1] = (char *) malloc (n);
  n = 0;

  int len = strlen(s);
  *((char *) memcpy(buf[n++], s, len) + len) = '\0';

  if (term->prefix() & QueryTerm::PFX_COMPLETE_BITS){
    char * p = buf [n++ % 2];
    snprintf (p, buflen, formats[query_type][OPT_COMPLETE], s);
    s = p;
  }
  // If there's any interal spaces, we need to use the phase operator.
#ifdef UseWideChars
  unsigned char* cp = (unsigned char*)term->term_string();
  for (; *cp > 0x20; cp++); // looking for control chars
  if (*cp != '\0') {
    char *p = buf [n++ % 2];
    snprintf (p, buflen, infix_formats[OPT_PHRASE], s);
    s = p; /// alternating
  }
  if (term->prefix() & QueryTerm::PFX_NOT_BITS)
    {
      char *p = buf [n++ % 2];
      snprintf (p, buflen, formats[query_type][OPT_NOT], s);
      s = p;
    }
#else
  char *t = term->term_string();
  while (*t && !iswspace (*t))
    t++;
  // If *t isn't NULL, then it must be a space.
  // NOTE: Always use phrase operator for now to escape out
  // OQL keywords passed off to QueryParser.  15:27 20-May-94 DJB 
  if (TRUE || *t != '\0')
    {
      char *p = buf [n++ % 2];
      snprintf (p, buflen, formats[query_type][OPT_PHRASE], s);
      s = p;
    }
  if (term->prefix() & QueryTerm::PFX_NOT_BITS)
    {
      char *p = buf [n++ % 2];
      snprintf (p, buflen, formats[query_type][OPT_NOT], s);
      s = p;
    }
#endif

#ifdef UseQSearch
  if (!(term->prefix() & QueryTerm::PFX_NOT_BITS) && *term->weight() != NULL)
#else
  if (*term->weight() != '\0')
#endif
    {
      // First plug in the weight format string, then add the weight to it. 
      char *p = buf [n++ % 2];
      snprintf (p, buflen, formats[query_type][OPT_WEIGHT], s);
      s = p;
      p = buf [n++ % 2];
      snprintf (p, buflen, s, term->weight());
      s = p;
    }

  // INSERT THING FOR SCOPE HANDLING RIGHT HERE!  DJB 

  // Finally, add in the term itself.
  tmp = strchr (s, '\001');
  *tmp = '%';

  {
    char *p = buf [n % 2];
    snprintf (p, buflen, s, term->term_string());
    s = p;
  }
  // Rember the point, since it will change after the write.
  int offset = g_buffer.offset (g_buffer.point());
  g_buffer.write (s, sizeof (char), strlen (s) + 1);

  free (buf[0]);
  free (buf[1]);

  // Return the OFFSET into the buffer. 
  return (offset);
}


// /////////////////////////////////////////////////////////////////
// reduce - reduce parse stacks
// /////////////////////////////////////////////////////////////////

void
QueryGroup::reduce (query_type_t query_type,
		    Stack<int> &operand_stack, 
		    Stack<connective_t> &operator_stack,
		    Stack<char *> &proximity_stack)
{
  // Get operands and operator to reduce with.
  // (The buffer position method converts an offset to a char *.) 
  char *op2 = g_buffer.position (operand_stack.pop ());
  char *op1 = g_buffer.position (operand_stack.pop ());
  char *proximity = proximity_stack.pop ();

  if (*proximity == '\0') {
      proximity = (char*)"32"; // NOTE: Hard coded proximity. 12:39 02/09/93 DJB
  }

  connective_t opt = (connective_t) operator_stack.pop();

  // Allocate formatting buffers. 
  int opsize = strlen (op1) + strlen (op2);
  if (opt == C_XOR) opsize *= 2;
  int fmt_length = strlen (formats[query_type][opt]) +
                   strlen (formats[query_type][OPT_PROXIMITY]);
  int buflen = fmt_length + opsize + 1;
  char *buf = (char*) malloc (buflen);

  snprintf (buf, buflen, formats[query_type][opt], op1, op2, op1, op2);

  // Take care of proximity if necessary.
  if (opt == C_NEAR || opt == C_BEFORE)
    {
      char *b2 = (char *) malloc (buflen);
      snprintf (b2, buflen, buf, formats[query_type][OPT_PROXIMITY]);
      snprintf (buf, buflen, b2, proximity);
      free (b2);
    }

  int offset = g_buffer.offset (op1);
  g_buffer.point (op1);
  g_buffer.write (buf, sizeof (char), strlen (buf) + 1);
  free (buf);

  operand_stack.push (offset);

}
