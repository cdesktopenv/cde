// $XConsortium: TextTest.cc /main/3 1996/06/11 17:41:57 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream.h>
#include <strstream.h>

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
	sprintf(patterns, "%s%s\n", patterns, argv[i]);
    }

    ostrstream text;

    char ch;
    while (cin.get(ch)) text << ch;

    char* buf = text.str();
    *(buf + text.pcount()) = '\0';

    char* match = StringParser::brute_force(buf, npat, patterns);

    if (match) {
	cout << match;
	delete[] match;
    }
    else
	fprintf(stderr, "match not found for \"%s\"\n", patterns);
	
}
