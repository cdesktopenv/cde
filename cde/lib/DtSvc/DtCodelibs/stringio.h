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
 * File:	stringio.h $XConsortium: stringio.h /main/3 1995/10/26 16:10:00 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
#include <codelibs/boolean.h>
#define __PRIVATE_
#include <codelibs/privbuf.h>

class _StringIO
{
    privbuf_strvec ptr;
    int curr;
    int old_curr;
    char const *old_ccp;
    int doit(int commit);
public:
    void push(char const *ccp);
    _StringIO();
    _StringIO(char const *ccp);
    void unget();
    int get();
    int next();
    boolean in_expansion();
};

inline
_StringIO::_StringIO()
{
    curr = old_curr = -1;
    old_ccp = 0;
}

inline void
_StringIO::push(char const *ccp)
{
    ptr[++curr] = (char*)ccp;
}

inline
_StringIO::_StringIO(char const *ccp)
{
    curr = old_curr = -1;
    old_ccp = 0;
    push(ccp);
}

inline void
_StringIO::unget()
{
    if (curr >= 0)
	ptr[curr] = (char*)old_ccp;
    curr = old_curr;
}

inline int
_StringIO::get()
{
    return doit(1);
}

inline int
_StringIO::next()
{
    return doit(0);
}

inline boolean
_StringIO::in_expansion()
{
    return ((boolean)(curr > 0));
}
