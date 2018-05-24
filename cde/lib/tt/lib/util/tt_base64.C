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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_base64.C /main/4 1996/07/30 17:41:40 barstow $ 			 				
/*
 *
 * tt_base64.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "util/tt_base64.h"

static	const	char	base64_chars[] =
	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";

// The following table was mechanically generated to be the inverse
// permutation of the above table.  Thus for 0<=i<63,
// base_64_values[base64_chars[i]]==i.
	
static  const	char	base64_values[256] = {
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  63,  64,  64,
	   0,   1,   2,   3,   4,   5,   6,   7,
	   8,   9,  64,  64,  64,  64,  64,  64,
	  64,  10,  11,  12,  13,  14,  15,  16,
	  17,  18,  19,  20,  21,  22,  23,  24,
	  25,  26,  27,  28,  29,  30,  31,  32,
	  33,  34,  35,  64,  64,  64,  64,  62,
	  64,  36,  37,  38,  39,  40,  41,  42,
	  43,  44,  45,  46,  47,  48,  49,  50,
	  51,  52,  53,  54,  55,  56,  57,  58,
	  59,  60,  61,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64,
	  64,  64,  64,  64,  64,  64,  64,  64};

/*
 * _tt_base64_encode(i) -- convert number to base-64 character string
 *
 * Purpose: Convert an long value (typically a time) to a "base-64"
 *   representation.  Return a _Tt_string containing
 *   the representation, not including any leading zeroes.
 */
_Tt_string
_tt_base64_encode(unsigned long i)
{
	int maxlen = (sizeof(long)*8+5)/6;

	_Tt_string retval(maxlen);
	int c = maxlen-1;

	if (i==0) {
		retval = "0";
	} else {
		while (i>0) {
			retval[c--] = base64_chars[i & 077];
			i >>= 6;
		}
		retval = retval.mid(c+1,retval.len());
	}
	return retval;
}

/*
 * _tt_base64_decode(i) -- convert base-64 character string to number
 *
 * Purpose: Convert a string in a "base-64"
 *   representation to an unsigned long.
 */
unsigned long
_tt_base64_decode(const char *s)
{
	int c,v;
	unsigned long r;

	r = 0;
	while (c = *s++) {
		v = base64_values[c];
		if (v==64) {	// illegal char, quit early
			return r;
		} else {
			r = (r<<6)+v;
		}
	}
	return r;
}
