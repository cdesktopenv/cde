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
// $XConsortium: UAS_String.hh /main/3 1996/06/11 16:40:38 cde-hal $
# ifndef _UAS_String_hh_
# define _UAS_String_hh_

# include "UAS_Base.hh"

typedef enum {
    UAS_NOT_OWNER = 0,
    UAS_OWNER = 1
} UAS_Owner;

class UAS_String;
template <class T> class UAS_List;

class UAS_StringRep {
    friend class UAS_String;
    private:
	UAS_StringRep (const char *data, int data_size, UAS_Owner owner);
	~UAS_StringRep ();

	void		incRef ();
	void		decRef ();
    private:
	int		fRefCnt;
	char *		fData;
	int		fDataSize;
	UAS_Owner	fOwner;

};

class UAS_String: public UAS_Base {

    friend int operator == (const UAS_String &, const UAS_String &);
    friend int operator == (const UAS_String &, const char *);
    friend int operator != (const UAS_String &, const UAS_String &);
    friend int operator != (const UAS_String &, const char *);

    friend int operator <  (const UAS_String &, const UAS_String &);

    public:
	UAS_String ();
	UAS_String (const char *,int length = -1,UAS_Owner owner = UAS_OWNER);
	UAS_String (const UAS_String &);
	~UAS_String ();

    public:
	operator char * () const;
	UAS_String &operator = (const UAS_String &);
	UAS_String operator + (const UAS_String &);
	UAS_String &append(const UAS_String &);

    public:
	int length () const;
	void split(const char, UAS_String &left, UAS_String &right) const;
	UAS_List<UAS_String> splitFields (const char separator) const;

    private:
	void setString (UAS_StringRep *);
	void unsetString ();

    private:
	UAS_StringRep	*fStringRep;
};

# endif
