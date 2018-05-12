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
// $XConsortium: WString.hh /main/4 1996/08/23 20:43:34 cde-hal $
#ifndef __WCHAR_H_
#define __WCHAR_H_

#include <wchar.h>
#include <string.h>

class WStringRep
{
    friend class WString;
private:
    WStringRep(const wchar_t* data, int length);
    ~WStringRep();

    void incref();
    void decref();

private:
    int f_refcount;
    wchar_t* f_data;
    int f_length;
};

inline
WStringRep::WStringRep(const wchar_t* data, int length) :
    f_refcount(0), f_length(length)
{
    f_data = new wchar_t[f_length + 1];
    memcpy((void*)f_data, (void*)data, f_length*sizeof(wchar_t));
    f_data[f_length] = 0;
}

inline
WStringRep::~WStringRep()
{
    delete[] f_data;
}

inline void
WStringRep::decref()
{
    if (--f_refcount == 0)
	delete this;
}

inline void
WStringRep::incref()
{
    ++f_refcount;
}

class WString
{
public:
    WString();

    WString(const WString &);
    WString(const char *,   int length = -1);
    WString(const wchar_t*, int length = -1);

    ~WString();

    WString& operator=(const WString&);

    operator wchar_t*() const;

    char*    get_mbstr(); // user is responsible for freeing memory
    wchar_t* get_wstr();  // user is responsible for freeing memory

    int length() const;

protected:
    void set_wstring(WStringRep*);
    void unset_wstring();

private:

    WStringRep* f_wstringrep;
};
#endif

