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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: strwcmp.C /main/7 1998/04/17 11:25:04 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <codelibs/nl_hack.h>
#include <string.h>
#include <codelibs/stringx.h>

#define QUOTE 0x40000000

#ifdef DEBUG
    static char tabs[] = "          ";
#   define TABS (&tabs[10 - depth])

    static int
    RETURN(int ret, int depth)
    {
	printf("%sreturning %s\n", TABS,
		(ret ? "SUCCEEDED" : "FAILED"));
	return ret;
    }
#else
#define RETURN(x,y) (x)
#endif

/* FORWARD */
static int match(const char *, const char *, int);
static const char *next_patt(const char *, int advance = 1);
static int match_class(const char *, char);

/* INLINE */
static int
next_char(const char *pattern, const char **cpp = NULL)
{
    int ret;
    wchar_t __nlh_char[1];

    ret = pattern ? (int)CHARAT(pattern) : '\0';
    if (ret != '\0')
    {
	ADVANCE(pattern);
        /* AIX needs line broken to get around macro bug (Temporary Fix) */
        if (ret == '\\' &&
            CHARAT(pattern) != '\0')
	    ret = QUOTE | (int)CHARADV(pattern);
    }

    if (cpp != NULL)
	*cpp = pattern;
    return ret;
}

int
strwcmp(const char *pattern, const char *string)
/*
 * String  'pattern'  is matched  against  string  'string'.  Zero is
 * returned  if the match is  successful.  'pattern'  may  contain the
 * shell metas * and ?  and the  semantics are the same.  ?  and * may
 * be escaped with \
 */
{
    return !match(pattern, string, 0);
}

// stwpat returns a pointer to the first meta-character if the string
// is a pattern, else NULL
char *
strwpat(const char *pattern)
{
    int ch;
    char *prev_pattern = (char *)pattern;
    wchar_t __nlh_char[1];

    while ((ch = next_char(pattern, &pattern)) != '\0')
    {
	switch (ch)
	{
	case '*': 
	    return prev_pattern;
	case '?': 
	    return prev_pattern;
	case '[': {
	    const char *eop = next_patt(prev_pattern, 0);
	    if (CHARAT(eop) == ']')
		return prev_pattern;
	    break;
	}
	}

	prev_pattern = (char *)pattern;
    }

    return NULL;
}

/*
 * match will check to see if pattern can successfully be applied to
 * the beginning of string.
 */
static int 
match(const char *pattern, const char *string, int depth)
{
#ifdef DEBUG
    printf("%smatch(\"%s\", \"%s\")\n", TABS, pattern, string);
#endif
    int ch;
    const char *cp;
    wchar_t __nlh_char[1];

    while ((ch = next_char(pattern, &cp)) != '\0')
    {
	const char *laststr = string;
	int testchar = (int)CHARADV(string);

	switch (ch)
	{
	case '*': {
	    pattern = cp;	/* skip over '*' */
	    string = laststr;	/* reverse - testchar not used */

	    const char *s = string;
	    do
		if (match(pattern, s, depth + 1))
		    return RETURN(1, depth);
	    while (CHARADV(s) != '\0');
	    return RETURN(0, depth);
	}
	case '?': 
	    break;
	case '[': {
	    int mt = match_class(pattern, testchar);
	    if (mt == 0)
		return RETURN(0, depth);
	    else if (mt == 2 && ch != testchar)
		return RETURN(0, depth);
	    break;
	}
	default: 
	    if ((ch & ~QUOTE) != testchar)
		return RETURN(0, depth);
	    break;
	}

	if (testchar == '\0')
	    string = laststr;	// reverse string

	pattern = next_patt(pattern);
    }

    return RETURN(CHARAT(string) == '\0', depth);
}

static int
match_class(const char *clss, char testchar)
/*
 *	pattern is a pointer to the leading [ of
 *	a shell-type class.  testchar is the character to match against
 *	the class.
 */
{
    int match = 1;		/* false if first char is '!' */
    wchar_t __nlh_char[1];

    /* find end of class, ie an un-escaped ']' */
    const char *eop = next_patt(clss, 0);
    ADVANCE(clss);

    if (CHARAT(eop) != ']')
	return 2;

    if (CHARAT(clss) == '!')
    {
	match = 0;
	ADVANCE(clss);
    }

    while (clss < eop)
    {
	int ch = next_char(clss, &clss);
	char const *clss_end = clss;
	int sep = next_char(clss_end, &clss_end);
	int ch2 = next_char(clss_end, &clss_end);

	/* check if next three chars are a range */
	if (sep == '-' && ch2 != ']')
	{
	    /* check range - we have to use strcoll to do it right */
	    char c1[MB_LEN_MAX+1], c2[MB_LEN_MAX+1], tc[MB_LEN_MAX+1];
	    memset(c1, 0, sizeof(c1));
	    memset(c2, 0, sizeof(c2));
	    memset(tc, 0, sizeof(tc));
	    ch &= ~QUOTE;
	    WCHAR(ch, c1);
	    ch2 &= ~QUOTE;
	    WCHAR(ch2, c2);
	    WCHAR(testchar, tc);

	    /* if (ch <= testchar && testchar <= ch2) // Original code */

	    /* Second implementation:
	     * if (nl_strncmp(c1, tc, 1) <= 0 && nl_strncmp(tc, c2, 1) <= 0)
	     *     return match;
	     */

	    /* Third, portable implementation: */
	    if (strcoll(c1, tc) <= 0 && strcoll(tc, c2) <= 0)
		return match;
	    clss = clss_end;
	}
	else			/* they are not a range, check simple
				   match */
	{
	    if ((ch & ~QUOTE) == testchar)
		return match;
	}
    }

    return !match;
}

static const char *
next_patt(const char *pattern, int advance)
{
    wchar_t __nlh_char[1];

    if (CHARAT(pattern) == '[')
    {
	int ch;
	const char *pp = pattern;
	ADVANCE(pp);

	if (CHARAT(pp) == '^')
	    ADVANCE(pp);

	if (CHARAT(pp) == ']')
	    ADVANCE(pp);

	char const *np;
	for (; (ch = next_char(pp, &np)) != '\0'; pp = np)
	    if (ch == ']')
		return (advance ? np : pp);
    }

    next_char(pattern, &pattern);
    return pattern;
}

#ifdef DEBUG
#define MAIN main
MAIN()
{
    char pattern[50], string[50];

    while (1)
    {
	putchar('\n');
	printf("pattern:  ");
	if (fgets(pattern, sizeof(pattern)-1, stdin) == NULL)
	    break;
	printf("string:   ");
	if (fgets(string, sizeof(pattern)-1, stdin) == NULL)
	    break;
	printf("MATCH is %s\n",
		((strwcmp(pattern, string) == 0) ? "SUCCEEDED" : "FAILED"));
	putchar('\n');
	printf("MATCHI is %s\n",
		((strwcmpi(pattern, string) == 0) ? "SUCCEEDED" : "FAILED"));
    }

    return 0;
}
#endif
