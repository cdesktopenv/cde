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
// $XConsortium: TextParser.cc /main/4 1996/06/11 17:41:43 cde-hal $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>
using namespace std;

#include "TextParser.hh"

#ifndef False
#define False	0
#endif
#ifndef True
#define True	1
#endif

char *
StringParser::brute_force(const char* text_in, int n_of_pats,
				const char* patterns, int sensitive)
{
    if (text_in == NULL || *text_in == '\0')
	return NULL;
    else if (patterns == NULL || *patterns == '\0')
	return NULL;

    if (! n_of_pats > 0)
	return NULL;

    char** pat_tbl = new char*[n_of_pats + 1];
    char** pat_tbl_end = pat_tbl + n_of_pats;

    int npat, len;
    for (npat = 0; *patterns && n_of_pats > 0; npat++, n_of_pats--) {
	char* del = (char *)strchr(patterns, '\n');
	if (del != NULL) { // more pattern specified
	    len = del - patterns;
	    pat_tbl[npat] = new char[len + 1];
	    *((char *) memcpy(pat_tbl[npat], patterns, len) + len) = '\0';
	    patterns = del + 1;
	}
	else {
	    int len = strlen(patterns);
	    pat_tbl[npat] = new char[len + 1];
	    *((char *) memcpy(pat_tbl[npat], patterns, len) + len) = '\0';
	    patterns += strlen(patterns);
	    assert( *patterns == '\0' );
	}
    }
    pat_tbl[npat] = NULL; // pointer table terminated

    assert( npat > 0 ); // at least one pattern available here

#ifdef DEBUG
    if (*patterns)
	fprintf(stderr, "(WARNING) more patterns available than specified\n");
    if (n_of_pats > 0)
	fprintf(stderr, "(WARNING) less patterns available than specified\n");
#endif

    int text_len = strlen(text_in);

    // remove null and too long patterns
    int i;
    for (i = 0 ; pat_tbl[i]; i++) {
	if (*(pat_tbl[i]) == '\0' || text_len < (int) strlen(pat_tbl[i])) {
	    delete[] pat_tbl[i];
	    pat_tbl[i] = NULL;
	    npat--;
	}
    }

    // remove redundance
    char** cursor;
    for (cursor = pat_tbl; cursor < pat_tbl_end; cursor++) {
	if (*cursor == NULL)
	    continue;
	char** p = cursor + 1;
	for (; p < pat_tbl_end; p++) {
	    if (*p == NULL)
		continue;
	    if (strcmp(*cursor, *p) == 0)
		break;
	}
	if (p < pat_tbl_end) { // same pattern found
	    delete[] *cursor;
	    *cursor = NULL;
	    npat--;
	}
    }

    // compact pat_tbl
    char** free_slot;
    for (free_slot = pat_tbl; *free_slot; free_slot++);
    if (free_slot < pat_tbl_end) { // there is a free slot
	cursor = pat_tbl;
	for (i = 0; i < npat; i++, cursor++) {
	    // find next pattern
	    for (; *cursor == NULL && cursor < pat_tbl_end; cursor++);
	    assert( cursor < pat_tbl_end );
	    if (free_slot && free_slot < cursor) {
		*free_slot = *cursor;
		*cursor = NULL;
		// find next available free slot
		free_slot++;
		for (; *free_slot; free_slot++);
		if (free_slot == pat_tbl_end)
		    free_slot = NULL;
	    }
	}
    }
    else {
	free_slot = NULL;
    }

    if (npat == 0) { // there is no effective patterns after all
	delete[] pat_tbl;
	return NULL;
    }

#ifdef DEBUG
    fprintf(stderr, "(DEBUG) %d effective patterns=", npat);
    for (int k = 0; pat_tbl[k]; k++) {
	fprintf(stderr, "\"%s\" ", pat_tbl[k]);
    }
    fprintf(stderr, "\n");
#endif

    char* caped_text = NULL;

    if (sensitive == False) { // case-insensitive search
	unsigned char *p;
	for (int i = 0; i < npat; i++) {
	    for (p = (unsigned char*)pat_tbl[i]; *p; p++) {
		if (*p < 0x7B && *p > 0x60) // a ... z
		    *p = *p - 0x20;
	    }
	}
	ostringstream capitalized;
	for (p = (unsigned char*)text_in; *p; p++) {
	    if (*p < 0x7B && *p > 0x60) // a ... z
		capitalized << (char)(*p - 0x20); // capitalize
	    else
		capitalized << *p;
	}
	text_in = caped_text = (char *)capitalized.str().c_str();
	*(char*)(text_in + capitalized.str().size()) = '\0';
    }

    ostringstream text_run;

    for (int index = 0; index < text_len;) {
	unsigned int candidate = (1 << npat) - 1;
	unsigned int success = 0;
	int i, j;
	for (i = index, j = 0 ; i < text_len + 1 && candidate; i++, j++) {
	    for (int n = 0; n < npat; n++) {
		unsigned int mask = 1;
		mask = mask << (npat - 1 - n);
		if (candidate & mask) { // still candidate
		    if (pat_tbl[n][j] == '\0') {
			success |= mask;
			candidate &= ~mask;
			continue;
		    }
		    else if (pat_tbl[n][j] != text_in[i]) {
			candidate &= ~mask;
			continue;
		    }
		}
		else {
		    continue;
		}
	    }
	}

	if (success) { // matched
	    // select the longest one
#ifdef SETECT_LONGEST
	    int nth = npat;
#endif
	    for (int n = 0; success > 0; success /= 0x02, n++) {
		if (success & 0x01) {
#ifdef SETECT_LONGEST
		    if (nth == npat)
			nth = npat - 1 - n;
		    else {
			if (strlen(pat_tbl[nth])
					< strlen(pat_tbl[npat - 1 - n])) {
			    nth = npat - 1 - n;
			}
		    }
#else
		    text_run << index << '\t' <<
				strlen(pat_tbl[npat - 1 - n]) << '\n';
#endif
		}
	    }
#ifdef SETECT_LONGEST
	    text_run << index << '\t' << strlen(pat_tbl[nth]) << '\n';
#endif
	}

	index += mblen(text_in + index, MB_CUR_MAX);
    }

    for (i = 0; i < npat; i++)
	free (pat_tbl[i]);
    free (pat_tbl);

    if (caped_text)
	delete[] caped_text;

    char* ret_text = (char *)text_run.str().c_str();

    if (ret_text == NULL)
	return NULL;
    else if (*ret_text == '\0') {
	delete[] ret_text;
	return NULL;
    }
    else
	return ret_text;

}

char *
StringParser::project_textrun(const char* org_textrun)
{
    if (org_textrun == NULL || *org_textrun == '\0')
	return NULL;

    istringstream textrun(org_textrun);

    char line[128];
    textrun.get(line, 128, '\n');
    if (textrun.get() != '\n')
	return NULL;

    char *offstr, *lenstr;

    offstr = line;
    if ((lenstr = strchr(line, '\t')) == NULL)
	return NULL;
    *lenstr++ = '\0';

    int off, len;
    off = atoi(offstr);
    len = atoi(lenstr);
    if (off < 0 || len <= 0) {
#ifdef DEBUG
	fprintf(stderr, "(ERROR) either off=%d or len=%d is invalid\n",
								off, len);
#endif
	return NULL;
    }

    ostringstream ret_text;

    while (textrun.get(line, 128, '\n')) {
	if (textrun.get() != '\n') {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) line is not followed by newline\n");
#endif
	    break;
	}

	int next_off, next_len;
	offstr = line;
	if ((lenstr = strchr(line, '\t')) == NULL) {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) tab chatacter not found in \"%s\"\n", line);
#endif
	    break;
	}
	*lenstr++ = '\0';
	next_off = atoi(offstr);
	next_len = atoi(lenstr);
	if (next_off < off || next_len <= 0) {
#ifdef DEBUG
	    fprintf(stderr, "(ERROR) either off=%d or length=%d is invalid\n",
							next_off, next_len);
#endif
	    break;
	}

	if (next_off <= off + len) { // overlap detected
	    if (off + len < next_off + next_len)
		len = next_off + next_len - off; // merge
	}
	else {
	    ret_text << off << '\t' << len << '\n';
	    off = next_off;
	    len = next_len;
	}
    }

    ret_text << off << '\t' << len << '\n' << '\0';

    return (char *)ret_text.str().c_str();
}

char *
StringParser::hilite(const char* text, int n, const char* pats)
{
    char* textrun = brute_force(text, n, pats);

    if (textrun == NULL)
	return NULL;

    char* prjed_textrun = project_textrun(textrun);
    delete[] textrun;

    return prjed_textrun;
}



