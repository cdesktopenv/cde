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
 * $XConsortium: strhash.C /main/5 1996/06/21 17:36:23 ageorge $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <codelibs/nl_hack.h>
#include <codelibs/stringx.h>


/**************************************************
// This is quick but does a pretty crummy job
unsigned strhash(const unsigned char *key)
{
	unsigned hash = 0;
	while (*key != '\0')
		hash = (hash << 3) + *key++;
	return hash;
}
**************************************************/


#include <limits.h>

#define BITS(type) 	(CHAR_BIT * (int)sizeof(type))

// This is from the "dragon" Compilers book.
// It is much better than the above but somewhat slower.
//
unsigned strhash(const char *p)
{
	unsigned h = 0;
	unsigned g;
	wchar_t __nlh_char[1];

	if (p != NULL)
		while (*p != '\0')
		{
			h = (h << 4) + (unsigned)CHARADV(p);
			if (g = h & ((unsigned)0xF << BITS(unsigned) - 4))
			{
				h ^= g >> BITS(unsigned) - 4;
				h ^= g;
			}
		}
	return h;
}

// Same as above but case insensitive.  Returns the same value as the
// above function if there are no upper case letters in the string.
//
unsigned strhashi(const char *p)
{
	unsigned h = 0;
	unsigned g;
	wchar_t __nlh_char[1];

	if (p != NULL)
		while (*p != '\0')
		{
			if (mblen(p, MB_CUR_MAX) > 1)
				h = (h << 4) + (unsigned)CHARADV(p);
			else
			{
				h = (h << 4) + (unsigned)tolower(*p++);
			}

			if (g = h & ((unsigned)0xF << BITS(unsigned) - 4))
			{
				h ^= g >> BITS(unsigned) - 4;
				h ^= g;
			}
		}
	return h;
}


/**************************************************
// This is about twice as slow as the above but 
// does a slightly better hash.
// by  TJ Merritt

unsigned int
hashfunc(buf, len)
unsigned char *buf;
int len;
{
	unsigned int in;
	unsigned int xor;
	unsigned int t;

	xor = len << 8;
	in = 0;

	while (len-- > 0)
	{
		in <<= 8;
		in += *buf++;
		xor ^= in;
		t = ((xor & 0x3) << 29) | (xor >> 3);
		xor ^= t;
	}

	return xor ^ (xor >> 16);
}
**************************************************/
