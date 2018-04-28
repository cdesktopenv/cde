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
 *+SNOTICE
 *
 *
 *	$XConsortium: ObjectKey.C /main/4 1996/04/21 19:48:43 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <assert.h>
#include <string.h>
#include <DtMail/ObjectKey.hh>

ObjectKey::ObjectKey(ObjectKeyType type)
{
    _type = strdup(type);
    return;
}

ObjectKey::ObjectKey(ObjectKey & key)
{
    _type = strdup(key._type);
}

ObjectKey::~ObjectKey(void)
{
    delete _type;
}

#ifdef DEAD_WOOD
int
ObjectKey::operator==(ObjectKey &)
{
    assert(!"Pure virtual ObjectKey::operator== called");
    return(0);
}

int
ObjectKey::operator!=(ObjectKey &)
{
    assert(!"Pure virtual ObjectKey::operator!= called\n");
    return(0);
}

int
ObjectKey::operator<(ObjectKey &)
{
    assert(!"Pure virtual ObjectKey::operator< called\n");
    return(0);
}

int
ObjectKey::operator<=(ObjectKey &)
{
    assert(!"Pure virtual ObjectKey::operator<= called\n");
    return(0);
}

int
ObjectKey::operator>(ObjectKey &)
{
    assert(!"Pure virtual ObjectKey::operator> called\n");
    return(0);
}

int
ObjectKey::operator>=(ObjectKey &)
{
    assert(!"Pure virtual ObjectKey::operator>= called\n");
    return(0);
}

HashVal
ObjectKey::hashValue(void)
{
    assert(!"Pure virtual ObjectKey::hashValue called\n");
    return(0);
}
#endif /* DEAD_WOOD */

HashVal
ObjectKey::genericHashValue(void * buf, int len)
{
    short * sbuf = (short *)buf;
    int slen = len / 2;
    HashVal hash = 0;

    // Sum the bytes, 2 at a time. We will deal with fractional
    // stuff after we do the main buffer.
    //
    for (int cur = 0; cur < slen; cur++) {
	hash ^= sbuf[cur];
    }

    // If the length is not even, then we need to add the last
    // byte to the high order bits of the hash value.
    //
    if ((len % 2)) {
	hash ^= ((char *)buf)[len - 1] << 8;
    }

    if (hash < 0) hash = -hash;
    return(hash);
}
