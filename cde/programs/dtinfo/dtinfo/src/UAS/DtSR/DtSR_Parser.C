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
// $XConsortium: DtSR_Parser.C /main/6 1996/10/16 14:24:17 cde-hal $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <assert.h>
#include <string.h>

#include <iostream>
using namespace std;

#include "UAS_String.hh"

#include "DtSR_Parser.hh"

#include "Managers/CatMgr.hh"

DtSR_Parser::DtSR_Parser() : f_completion_specified(0),
			     f_completion_specified_transient(0)
{
}

UAS_String
DtSR_Parser::validate(UAS_String &seql)
{
    char* op = (char*)seql;

    if (*op == '\0') {
#ifdef DEBUG
	cerr << "(DEBUG) " << __FILE__ << ' ' << __LINE__ << '\n' << flush;
#endif
	throw(UAS_Exception(""));
    }

    // find the 1st meaningful character
    while (*op == ' ' || *op == '\t' || *op == '\n')
	op++;

    if (*op == '(') {

	char* cp;
	{ // find the correspondence of op
	    int n = 1;
	    for (cp = op + 1; n > 0 && *cp; cp++) {
		if (*cp == ')')
		    n--;
		else if (*cp == '(')
		    n++;
	    }
	    if (n > 0) { // parentheses not balanced
#ifdef DEBUG
		cerr << "(DEBUG) " << __FILE__ << ' ' << __LINE__ << '\n' << flush;
#endif
		throw(UAS_Exception(""));
	    }
	    cp--;
	}
	assert( *cp == ')' );

	char* tail = op + strlen(op) - 1;
	while (*tail == ' ' || *tail == '\t' ||
	       *tail == '\n') // find the last meaningful character
	    tail--;

	if (cp == tail) { // redundant parentheses (i.e. ^( ... )$)
	    // find the next meaningful character
	    op++;
	    while (*op == ' ' || *op == '\t' || *op == '\n')
	        op++;

	    for (tail = cp - 1; *tail == ' ' || *tail == '\t' || *tail == '\n';)
		cp = tail--;
	    if (! (op < cp)) {
#ifdef DEBUG
		cerr << "(DEBUG) " << __FILE__ << __LINE__ << '\n' << flush;
#endif
		throw(UAS_Exception(""));
	    }
	    *cp = '\0';
	    UAS_String newstr = op;
	    return validate(newstr);
	}
    }

    f_completion_specified = f_completion_specified_transient;
    f_completion_specified_transient = 0;

    return op;
}

UAS_String
DtSR_Parser::se_construct(OQL_Token token,
			UAS_String arg1, UAS_String arg2, UAS_String)
{
#ifndef IGNORE_UNSUPPORTED
    char msg[512];
#endif

    if (token == COMPLETION) { // completion not supported
	f_completion_specified_transient++;
	return arg1;
    }
    else if (token == WEIGHT) { // weight not supported
#ifdef IGNORE_UNSUPPORTED
	return arg1;
#else
	UAS_String str   = CATGETS(Set_DtSR_Parser, 1,
				"The ``%s'' Search feature is not supported.");
	UAS_String opstr = CATGETS(Set_DtSR_Parser, 3, "weight");
	snprintf(msg, sizeof(msg), (char*)str, (char*)opstr);
	UAS_String uas_msg = msg;
	throw(UAS_Search_Feature_Unavailable(uas_msg));
#endif
    }
    else if (token == NOT) {
	UAS_String newstr = "~";
	newstr = newstr + arg1;
	return newstr;
    }
    else if (token == PHRASE) {
	if (*(char*)arg2 == '\0')
	    return arg1;
	if (*(char*)arg1 == '\0')
	    return arg2;
	UAS_String newstr = "( ";
	newstr = newstr + arg1 + " & " + arg2 + " )";
	return newstr;
    }
    else if (token == QUOTED_PHRASE) {
#ifdef SUPPORT_QUOTED_PHRASE
	UAS_String newstr;
	char* phrase = (char*)arg1;
	for (char* p = phrase; *phrase; phrase = p) {
	    while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0')
		p++;
	    if (*p != '\0')
		*p++ = '\0'; // tampering with arg1
	    if (*(char*)newstr == '\0') {
		newstr = phrase;
	    }
	    else {
		UAS_String tmpstr = "( ";
		newstr = tmpstr + newstr + " & " + phrase + " )";
	    }
	}
	return newstr;
#else
	UAS_String str   = CATGETS(Set_DtSR_Parser, 1,
				"The ``%s'' Search feature is not supported.");
	UAS_String opstr = CATGETS(Set_DtSR_Parser, 2, "quoted phrase");
	snprintf(msg, sizeof(msg), (char*)str, (char*)opstr);

	UAS_String uas_msg = msg;
	throw(UAS_Search_Feature_Unavailable(uas_msg));
#endif
    }
    else if ((token == NEAR) || (token == NEAR_WITHIN) ||
	     (token == BEFORE) || (token == BEFORE_WITHIN)){
#ifdef IGNORE_UNSUPPORTED
	if (*(char*)arg2 == '\0')
	    return arg1;
	if (*(char*)arg1 == '\0')
	    return arg2;

	UAS_String newstr = "( ";
	newstr = newstr + arg1 + " & " + arg2 + " )";
	return newstr;
#else
	UAS_String str   = CATGETS(Set_DtSR_Parser, 1,
				"The ``%s'' Search feature is not supported.");
	UAS_String opstr = CATGETS(Set_DtSR_Parser, 4, "proximity");
	snprintf(msg, sizeof(msg), (char*)str, (char*)opstr);
	UAS_String uas_msg = msg;
	throw(UAS_Search_Feature_Unavailable(uas_msg));
#endif
    }
    else if (token == OR) {
	if (*(char*)arg2 == '\0')
	    return arg1;
	if (*(char*)arg1 == '\0')
	    return arg2;

	UAS_String newstr = "( ";
	newstr = newstr + arg1 + " | " + arg2 + " )";
	return newstr;
    }
    else if (token == AND) {
	if (*(char*)arg2 == '\0')
	    return arg1;
	if (*(char*)arg1 == '\0')
	    return arg2;

	UAS_String newstr = "( ";
	newstr = newstr + arg1 + " & " + arg2 + " )";
	return newstr;
    }
    else {
	assert( token == XOR );

#ifdef IGNORE_UNSUPPORTED
	if (*(char*)arg2 == '\0')
	    return arg1;
	if (*(char*)arg1 == '\0')
	    return arg2;

	UAS_String newstr = "( ";
	newstr = newstr + arg1 + " & " + arg2 + " )";
	return newstr;
#else
	UAS_String str   = CATGETS(Set_DtSR_Parser, 1,
				"The ``%s'' Search feature is not supported.");
	UAS_String opstr = CATGETS(Set_DtSR_Parser, 5, "xor");
	snprintf(msg, sizeof(msg), (char*)str, (char*)opstr);
	UAS_String uas_msg = msg;
	throw(UAS_Search_Feature_Unavailable(uas_msg));
#endif
    }
}
