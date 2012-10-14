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
// $XConsortium: TextRun.cc /main/3 1996/06/11 17:41:52 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
using namespace std;

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

    ostringstream text;

    char ch;
    while (cin.get(ch)) text << ch;

    char* buf = (char *)text.str().c_str();
    *(buf + text.str().size()) = '\0';
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
