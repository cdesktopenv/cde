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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tt_string_match.C /main/4 1999/10/15 12:27:43 mgreess $ 			 				
/*
 *
 * tt_string_match.cc
 *
 * Copyright (c) 1991 by Sun Microsystems, Inc.
 * 
 * Implementation of _Tt_string class shell-style
 * wild card matching operations.
 * Split out from the other ops in tt_string.cc since tt_string.cc
 * is already quite large, and the matching code itself is
 * lengthy.
 */

#include "util/tt_string.h"
#include "util/tt_assert.h"
#include <string.h>

#if defined (__hpux)
/*
 * On HP MAXINT is defined in both <values.h> and <sys/param.h>
 */
#undef MAXINT
#endif
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>     
#endif

static	int	_tt_csh_match(const char *, const char *);
static	int	_tt_csh_match_star(const char *, const char *);

int _Tt_string::
sh_match(const _Tt_string &pattern) const
{
	const char *p = (*this)->content;
	const char *q = (const char *)pattern;
	int result;

	if (0==p) {
		result = 0;
	} else {
		result = _tt_csh_match(p, q);
	}
	return result;
}


// The following code came from source browser.  It only
// works for normal non-zero-char-containing strings.

/*
 *	_tt_csh_match(string, pattern)
 *		This will return 1 if "string" matches "pattern".
 */
int
_tt_csh_match(const char *string, const char *pattern)
{
	int		string_ch;
	int		k;
	int		pattern_ch;
	int		lower_bound;

    top:
	for (; 1; pattern++, string++) {
		lower_bound = MAXINT;
		string_ch = *string;
		switch (pattern_ch = *pattern) {
		case '[':
			k= 0;
			while (pattern_ch = *++pattern) {
				switch (pattern_ch) {
				case ']':
					if (!k) {
						return 0;
					}
					string++;
					pattern++;
					goto top;
				case '-':
					k |= (lower_bound <= string_ch) &&
					     (string_ch <=
						(pattern_ch =
						 pattern[1]));
					/* Fall through... */
				default:
					if (string_ch ==
					   (lower_bound = pattern_ch)) {
						k++;
					}
				}
			}
			return 0;
		case '*':
			return _tt_csh_match_star(string, ++pattern);
		case 0:
			return (string_ch ? 0 : 1);
		case '?':
			if (string_ch == 0) {
				return 0;
			}
			break;
		default:
			if (pattern_ch != string_ch) {
				return 0;
			}
			break;
		}
	}
	/* NOTREACHED */
}

/*
 *	_tt_csh_match_star(string, pattern)
 *	This matches the '*' portion of a pattern.
 */
static int
_tt_csh_match_star(const char *string, const char *pattern)
{
	int		pattern_ch;

	switch (*pattern) {
	case 0:
		return 1;
	case '[':
	case '?':
	case '*':
		while (*string) {
			if (_tt_csh_match(string++, pattern)) {
				return 1;
			}
		}
		break;
	default:
		pattern_ch = *pattern++;
		while (*string) {
			if (*string++ == pattern_ch &&
			     _tt_csh_match(string, pattern)) {
				return 1;
			}
		}
		break;
	}
	return 0;
}
