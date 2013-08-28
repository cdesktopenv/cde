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
// $XConsortium: TextTest.cc /main/3 1996/06/11 17:41:57 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
using namespace std;

#include "TextParser.hh"

main(int argc, char** argv)
{
    char* patterns = NULL;

    setlocale(LC_CTYPE, "");

    if (! (argc > 1)) {
	fprintf(stderr, "Usage: %s pattern1 pattern2 ...\n", argv[0]);
	return 1;
    }

    int size = 256;
    patterns = (char*) malloc(size);
    *patterns = '\0';
    int npat = argc - 1, i;
    for (i = 1; i <= npat; i++) {
	if (strlen(patterns) + strlen(argv[i]) + 2 > size) { // 2 = '\n'+'\0'
	    size += 256;
	    patterns = (char*) realloc(patterns, size);
	}
	snprintf(patterns, size, "%s%s\n", patterns, argv[i]);
    }

    ostringstream text;

    char ch;
    while (cin.get(ch)) text << ch;

    char* buf = (char *)text.str().c_str();
    *(buf + text.str().size()) = '\0';

    char* match = StringParser::brute_force(buf, npat, patterns);

    if (match) {
	cout << match;
	delete[] match;
    }
    else
	fprintf(stderr, "match not found for \"%s\"\n", patterns);
	
}
