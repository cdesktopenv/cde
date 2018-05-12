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
/*	Copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __XM_STRING_LOCALIZED_HH__
#define __XM_STRING_LOCALIZED_HH__

#include <Xm/Xm.h>

class XmStringLocalized
{
public:
    XmStringLocalized(char* = NULL);
    ~XmStringLocalized();

    // conv operator
    operator XmString () { return f_string; }

    XmStringLocalized& operator= (char*);

private:
    XmString f_string;
};

inline
XmStringLocalized::XmStringLocalized(char* string) : f_string(NULL)
{
    if (string)
        f_string = XmStringCreateLocalized(string);
}

inline
XmStringLocalized::~XmStringLocalized()
{
    if (f_string) {
        XmStringFree(f_string);
        f_string = NULL;
    }   
}

inline XmStringLocalized&
XmStringLocalized::operator = (char* string)
{
    if (f_string) {
	XmStringFree(f_string);
	f_string = NULL;
    }
    if (string)
	f_string = XmStringCreateLocalized(string);

    return *this;
}

#endif
