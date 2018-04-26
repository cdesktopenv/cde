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
//%%  $XConsortium: binkey.C /main/3 1995/10/20 16:24:54 rswiston $ 			 				
/*
 *
 * binkey.cc
 *
 * Copyright (c) 1991 by Sun Microsystems, Inc.
 */

#include "util/tt_string.h"
#include "db/tt_db_key_utils.h"
#include "binkey.h"
#include <memory.h>

static unsigned char sixteen_zeroes[OID_KEY_LENGTH] = {
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0};
static unsigned char sixteen_foxes[OID_KEY_LENGTH] = {
	255,255,255,255,
	255,255,255,255,
	255,255,255,255,
	255,255,255,255};

Binkey Binkey::smallest(sixteen_zeroes);
Binkey Binkey::largest(sixteen_foxes);

static int binkey_compare(const unsigned char *a, const unsigned char *b);

Binkey::
Binkey()
{
	memset((char *)_binkey, 0, sizeof(_binkey));
	_key = (_Tt_db_key *)0;
}

Binkey::
Binkey(const unsigned char *k)
{
	_Tt_string bks(k, sizeof(_binkey));
	memcpy((char *)_binkey, k, sizeof(_binkey));
	_key = new _Tt_db_key(bks);
}

Binkey & Binkey::
operator=(const Binkey &k)
{
	memcpy((char *)_binkey, (char *)k._binkey, sizeof(_binkey));
	_key = new _Tt_db_key(k);
	return *this;
}

Binkey & Binkey::
operator=(const unsigned char *k)
{
	_Tt_string bks(k, sizeof(_binkey));
	memcpy((char *)_binkey, k, sizeof(_binkey));
	_key = new _Tt_db_key(bks);
	return *this;
}

int
operator==(const Binkey &a, const _Tt_db_key &b)
{
	return b==*a._key;
}	

int
operator==(const Binkey &a, const Binkey &b)
{
	return binkey_compare(a._binkey,b._binkey)==0;
}
int
operator<(const Binkey &a, const Binkey &b)
{
	return binkey_compare(a._binkey,b._binkey)<0;
}
int
operator>(const Binkey &a, const Binkey &b)
{
	return binkey_compare(a._binkey,b._binkey)>0;
}

static int
binkey_compare(const unsigned char *pa, const unsigned char *pb)
{
	// No libc routines seem to guarantee to handle unsigned chars!
	int i = OID_KEY_LENGTH;
	while (i--) {
		if (*pa>*pb) return 1;
		if (*pa++<*pb++) return -1;
	}
	return 0;
}

void Binkey::
print(FILE* f) const
{
	if (_key.is_null()) {
		fprintf(f,"(null key)");
	} else {
		_key->print(f);
	}
}


