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
// $XConsortium: UAS_String.cc /main/3 1996/06/11 16:40:32 cde-hal $
# include <string.h>
# include "UAS_String.hh"
# include "UAS_List.hh"

UAS_StringRep::UAS_StringRep (const char *data,int data_size,UAS_Owner owner) {
    fRefCnt = 0;
    fOwner = owner;
    fDataSize = data_size;
    if (fOwner == UAS_OWNER) {
	fData = new char[fDataSize + 1];
	(void) memcpy (fData, data, fDataSize);;
	fData[fDataSize] = 0;
    } else {
	fData = (char *) data;
    }
}

UAS_StringRep::~UAS_StringRep () {
    if (fOwner == UAS_OWNER)
	delete fData;
}

void
UAS_StringRep::incRef () {
    fRefCnt ++;
}

void
UAS_StringRep::decRef () {
    if (--fRefCnt == 0)
	delete this;
}

static UAS_StringRep *gNullString = 0;

UAS_String::UAS_String () {
    fStringRep = 0;
    if (!gNullString) {
	gNullString = new UAS_StringRep ("", 0, UAS_NOT_OWNER);
	gNullString->incRef (); // So it stays around forever
    }
    setString (gNullString);
}

UAS_String::UAS_String (const UAS_String &str) {
    fStringRep = 0;
    setString (str.fStringRep);
}

UAS_String::UAS_String (const char *data, int data_size, UAS_Owner owner) {
    fStringRep = 0;
    if (!data) {
	if (!gNullString) {
	    gNullString = new UAS_StringRep ("", 0, UAS_NOT_OWNER);
	    gNullString->incRef (); // So it stays around forever
	}
	setString (gNullString);
    } else {
	if (data_size < 0) {
	    data_size = strlen (data);
	}
	setString (new UAS_StringRep (data, data_size, owner));
    }
}

UAS_String&
UAS_String::append(const UAS_String& post_fix)
{
    int length = fStringRep->fDataSize + post_fix.fStringRep->fDataSize;
    char* new_string = new char[length + 1];
    memcpy(new_string, fStringRep->fData, fStringRep->fDataSize);
    memcpy(new_string + fStringRep->fDataSize, post_fix.fStringRep->fData,
					post_fix.fStringRep->fDataSize);
    new_string[length] = '\0';
    
    UAS_StringRep* strrep =
		new UAS_StringRep(new_string, length, UAS_NOT_OWNER);
    strrep->fOwner = UAS_OWNER;
    setString(strrep);

    return *this;
}

UAS_String::operator char * () const {
    return fStringRep->fData;
}

int
UAS_String::length () const {
    return fStringRep->fDataSize;
}

UAS_String &
UAS_String::operator = (const UAS_String &s) {
    setString (s.fStringRep);
    return *this;
}

UAS_String::~UAS_String () {
    unsetString ();
}

void
UAS_String::setString (UAS_StringRep *sr) {
    //
    //  Important to incRef first in the case that
    //  sr == fStringRep
    //
    sr->incRef ();
    unsetString ();
    fStringRep = sr;
}

void
UAS_String::unsetString () {
    if (fStringRep) {
	fStringRep->decRef ();
	fStringRep = 0;
    }
}

void
UAS_String::split (const char field, UAS_String &left, UAS_String &right) const{
    int i;
    for (i = 0; i < fStringRep->fDataSize; i ++)
	if (fStringRep->fData[i] == field)
	    break;
    if (i >= fStringRep->fDataSize) {
	left = UAS_String ();
	right = *this;
    } else {
	left = UAS_String (fStringRep->fData, i);
	i ++;
	right = UAS_String (fStringRep->fData + i, fStringRep->fDataSize - i);
    }
}

int
operator == (const UAS_String &s1, const UAS_String &s2) {
    if (s1.length() != s2.length())
        return 0;
    return !memcmp ((char *) s1, (char *) s2, s1.length());
}

int
operator != (const UAS_String &s1, const UAS_String &s2) {
    return !(s1 == s2);
}

int
operator == (const UAS_String &s1, const char *cp) {
    if (!cp) {
        return s1.length() == 0;
    }
    if (strlen (cp) != (size_t)s1.length())
        return 0;
    return !memcmp (cp, (char *) s1, s1.length());
}

int
operator != (const UAS_String &s1, const char *cp) {
    return !(s1 == cp);
}

int
operator < (const UAS_String &s1, const UAS_String &s2)
{
    return (strcmp((const char*)s1, (const char*)s2) < 0);
}

UAS_List<UAS_String>
UAS_String::splitFields (const char separator) const {
    UAS_List<UAS_String> rval;
    UAS_Pointer<UAS_String> cur;
    int start = 0;
    int i;
    for (i = 0; i < fStringRep->fDataSize; i ++) {
	if (fStringRep->fData[i] == separator) {
	    cur = new UAS_String(&(fStringRep->fData[start]),
				i - start, UAS_OWNER);
	    rval.insert_item(cur);
	    start = i + 1;
	}
    }
    if (i != start) {
	cur = new UAS_String(&(fStringRep->fData[start]), i - start, UAS_OWNER);
	rval.insert_item(cur);
    }
    return rval;
}

UAS_String
UAS_String::operator + (const UAS_String &more) {
    int newLength = length() + more.length();
    char *ptr = new char[newLength + 1];
    (void) memcpy (ptr, fStringRep->fData, fStringRep->fDataSize);
    (void) memcpy (ptr + fStringRep->fDataSize,
		   more.fStringRep->fData,
		   more.fStringRep->fDataSize);
    ptr[newLength] = 0;
    //
    // The following two lines are weird but serve to
    // remove and extra malloc/copy.
    //
    UAS_String rval (ptr, newLength, UAS_NOT_OWNER);
    rval.fStringRep->fOwner = UAS_OWNER;
    return rval;
}
