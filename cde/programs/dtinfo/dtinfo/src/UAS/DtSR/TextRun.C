// $XConsortium: TextRun.cc /main/3 1996/06/11 17:41:52 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <strstream.h>

main(int argc, char** argv)
{
    if (argc < 3) {
	fprintf(stderr, "Usage: %s offset length\n", argv[0]);
	return 1;
    }

    int offset = atoi(argv[1]);
    int length = atoi(argv[2]);

    if (offset < 0) {
	fprintf(stderr, "(ERROR) offset must be non-negative\n");
	return 1;
    }
    if (length <= 0) {
	fprintf(stderr, "(ERROR) length must be positive\n");
	return 1;
    }

    ostrstream text;

    char ch;
    while (cin.get(ch)) text << ch;

    char* buf = text.str();
    *(buf + text.pcount()) = '\0';
    char* p = buf;

    if (buf == NULL || *buf == '\0') {
	fprintf(stderr, "(ERROR) empty input stream\n");
	return 1;
    }

    int n;
    for (; offset > 0; p += n, offset -= n) {
	n = mblen(p, MB_CUR_MAX);
	assert( n > 0 );
    }
    assert( offset == 0 );

    fprintf(stdout, "\"%.*s\"\n", length, p);
}
