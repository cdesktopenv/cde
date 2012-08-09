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
/* $XConsortium: cstring.C /main/2 1995/07/17 14:10:22 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "cstring.h"
#include <ctype.h>

CString::CString()
{
    contents = 0;
    skipWhiteSpace = 1;
}

CString::CString
	(
	const char *  s,
	unsigned char ws
	) : skipWhiteSpace(ws)
{
    if (s != 0) {
	contents = new char [strlen(s) + 1];
	strcpy(contents,s);
    }
    else {
	contents = new char [1];
	strcpy(contents,"");
    }

}

CString::CString
	(
	const char c,
	unsigned char ws
	) : skipWhiteSpace(ws)
{
    contents = new char [2];
    contents[0] = c;
    contents[1] = 0;
}

CString::~CString()
{
    delete [] contents;
}

CString::CString
	(
	const CString & s
	)
{
    contents = new char [s.length() + 1];
    strcpy(contents,s.data());
    skipWhiteSpace = s.skipWhiteSpace;
}

char & CString::operator[]
	(
	int index
	) const
{
    if (index < 0)
	return contents[0];

    if (index > strlen(contents))
	return contents[strlen(contents)-1];

    return contents[index];
}

int CString::operator!=
	(
	const CString & s
	) const
{
    return !(*this == s);
}

int CString::operator==
	(
	const CString & s
	) const
{
    if (isNull() && s.isNull())
	return 1;

    if (isNull() || s.isNull())
	return 0;

    if (strcmp(contents,s.data()) == 0)
	return 1;

    return 0;
}

CString & CString::operator=
	(
	const CString & s
	)
{
    if (s != *this) {
	delete [] contents;
	contents = new char [s.length() + 1];
	strcpy(contents,s.data());
    }
    return *this;
}

CString & CString::operator=
	(
	const char * s
	)
{
    delete [] contents;
    contents = new char [strlen(s) + 1];
    strcpy(contents,s);
    return *this;
}

CString & CString::operator+=
	(
	const char * cs
	)
{
char * temp;

    if (contents) {
	if (cs)
	    temp = new char [strlen(contents) + strlen(cs) + 1];
	else
	    return *this;
    }
    else {
	if (cs)
	    temp = new char [strlen(cs) + 1];
	else
	    return *this;
    }

    *temp = 0;
    if (contents)
	strcat(temp,contents);
    if (cs)
	strcat(temp,cs);

    delete [] contents;
    contents = temp;

    return *this;
}


CString & CString::operator+= 
	(
	const CString & s
	)
{
char * temp;

    if (contents) {
	if (s.contents)
	    temp = new char [strlen(contents) + s.length() + 1];
	else
	    return *this;
    }
    else {
	if (s.contents)
	    temp = new char [s.length() + 1];
	else
	    return *this;
    }

    *temp = 0;
    if (contents)
	strcat(temp,contents);
    if (s.contents)
	strcat(temp,s.data());

    delete [] contents;
    contents = temp;

    return *this;
}


CString operator+ 
	(
	const CString & s1, 
	const CString & s2
	)
{
  CString result(s1);

  result += s2;
  return result;
}


CString operator+ 
	(
	const CString & s, 
	const char * cs
	)
{
  CString result(s);

  result += cs;
  return result;
}


CString operator+ 
	(
	const char * cs, 
	const CString & s
	)
{
  CString result(cs);

  result += s;
  return result;
}


CString CString::copy 
	(
	unsigned int start, 
	const char * delim
	)
{
    if (isNull())
	return *this;

    for (int i = 0; i < strlen(delim); i++) {
	char * q;
	if (q = strchr(contents,delim[i])) {
	    char remember = *q;
	    *q = 0;
	    CString result(&contents[start]);
	    *q = remember;
	    return result;
	}
    }
    return *this;
}

CString CString::copy 
	(
	const char * delim1, 
	const char * delim2
	)
{
    if (isNull())
	return *this;

    for (int i = 0; i < strlen(delim1); i++) {
	char * q;
	if (q = strchr(contents,delim1[i])) {
	    for (int j = 0; j < strlen(delim2); j++) {
		char * p;
		if (p = strchr(q+1,delim2[j])) {
		    char remember = *p;
		    *p = 0;
		    CString result(q+1);
		    *p = remember;
		    return result;
		}
	    }
	    return CString(q);
	}
    }
    return *this;

}

CString CString::find 
	(
	const char * cs
	)
{
char * q;

    if (isNull())
	return *this;

    if (q = strstr(contents,cs))
	return CString(q);

    return *this;
}

int CString::contains 
	(
	const CString & s,
	const char *    leader,
	const char *    trailer
	) const
{
    return contains(s.data(), leader, trailer);
}

int CString::contains 
	(
	const char * cs,
	const char * leader,
	const char * trailer
	) const
{
    // first see if there is anything in contents

    if (isNull())
	return 0;

    // or if the strings are exactly the same

    if (strcmp(cs,data()) == 0)
	return 1;

    // now check to see if it is embedded in the string

    CString search(leader);
    search += cs;
    search += trailer;

    if (strstr (contents, search.data()))
	return 1;

    // if skipWhiteSpace is on then check for spaces on one or 
    // both sides of the target in the source.

    if (skipWhiteSpace) {

	// first, a space in front, delimiter on the end

	search = " ";
	search += cs;
	search += trailer;
	if (strstr (contents, search.data()))
	    return 1;

	// second, a delimiter on the front, space on the end

	search = leader;
	search += cs;
	search += " ";
	if (strstr (contents, search.data()))
	    return 1;

	// third, spaces before and after

	search = " ";
	search += cs;
	search += " ";
	if (strstr (contents, search.data()))
	    return 1;
    }

    // now check to see if it is the start of the string

    search = cs;
    search += trailer;
    if (strncmp (contents, search.data(), search.length()) == 0)
	return 1;

    // now check to see if it is the start of the string followed
    // by a space

    search = cs;
    search += " ";
    if (strncmp (contents, search.data(), search.length()) == 0)
	return 1;

    // last, check to see if it is on the end of the string using
    // the delimiter and a space

    search = leader;
    while (1) {
	search += cs;
	char *q, *r = 0;
	char *p = contents;
	while (q = strstr(p, search.data())) {
	    r = q + strlen(leader);
	    p = q + 1;
	}
	if (r && strcmp(r,cs) == 0)
	    return 1;

	if (search[0] == ' ')
	    break;

	search = " ";
    }

    return 0;
}

void CString::replace
	(
	const CString & to_be_replaced,
	const CString & replacee
	)
{
char * q;

    if (isNull())
	return;

    if (q = strstr(contents,to_be_replaced.data())) {
	*q = 0;
	char * prefix = new char [strlen(contents) + 1];
	strcpy(prefix,contents);
	q += to_be_replaced.length();
	char * suffix = new char [strlen(q) + 1];
	strcpy(suffix,q);
	delete [] contents;
	contents = new char [strlen(prefix) + 
			     replacee.length() + 
			     strlen(suffix) + 1];
	strcpy(contents,prefix);
	strcat(contents,replacee.data());
	strcat(contents,suffix);
	delete [] prefix;
	delete [] suffix;
    }
}

int CString::isNull() const
{
    if (contents == 0)
	return 1;

    if (strlen(contents) == 0)
	return 1;

    return 0;
}


CTokenizedString::CTokenizedString()
	: CString(""), finished(0), cursor(0), delimiter(0)
{}


CTokenizedString::CTokenizedString
	(
	const CTokenizedString & s
	)
{
  if (s != *this) {
    cursor = new char [strlen(s.cursor) + 1];
    strcpy(cursor,s.cursor);
    delimiter = new char [strlen(s.delimiter) + 1];
    strcpy(delimiter,s.delimiter);
    finished = s.finished;
    skipWhiteSpace = s.skipWhiteSpace;
    contents = new char [strlen(s.contents) + 1];
    strcpy(contents,s.contents);
  }
}

CTokenizedString::CTokenizedString
	(
	const CString & s, 
	char * d,
	unsigned char ws
	) : CString(s), finished(0)
{
    cursor = new char [s.length() + 1];
    strcpy(cursor,s.data());
    delimiter = new char [strlen(d) + 1];
    strcpy(delimiter,d);
    skipWhiteSpace = ws;
}


CTokenizedString::~CTokenizedString()
{
    delete [] cursor;
    delete [] delimiter;
}


CString CTokenizedString::next()
{
char * q  = 0;
char * p;
int i;

    if (cursor) {
	if (strlen(delimiter) == 1)
	    q = strchr(cursor,delimiter[0]);
	else {
	    for (i = 0; i < strlen(cursor); i++)
		if (strchr(delimiter,cursor[i])) {
		    q = &cursor[i];
		    break;
		}
	}
    }

    if (q) {

	// a delimiter has been found. Create the return token,
	// adjust the cursor

	*q = 0;

	// eliminate trailing white space

	if (skipWhiteSpace) {
	    for (p = q; isspace(*(p-1)); p--);
	    *p = 0;
	}
	CString result(cursor);

	// eliminate leading white space

	if (skipWhiteSpace)
	    for (; isspace(*(q+1)); q++);

	char * temp = new char [strlen(q+1) + 1];
	strcpy(temp,q+1);
	delete [] cursor;
	cursor = temp;
	return result;
    }
    else if (cursor) {

	// no delimiter found. Return what is left of the string

	CString result(cursor);
	delete [] cursor;
	cursor = 0;
	return result;
    }
    else {
	finished = 1;
        return CString("");
    }
}

#if defined(linux)
std::ostream & operator<<
	(
	std::ostream &       os,
	const CString & s
	)
#else
ostream & operator<<
	(
	ostream &       os,
	const CString & s
	)
#endif
{
    if (s.isNull())
	return os << "(null)";

    return os << s.data();
}
