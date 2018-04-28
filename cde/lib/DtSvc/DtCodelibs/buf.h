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
 * File:	buf.h $XConsortium: buf.h /main/3 1995/10/26 16:07:20 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
#include <codelibs/boolean.h>
#define __PRIVATE_
#include <codelibs/privbuf.h>

#define NOQUOTE		0
#define SINGLEQUOTE	1
#define DOUBLEQUOTE	2
#define EXPANDQUOTE	4

typedef unsigned char Quote;

class _SHXcomponents;

class _SHXbuf
{
    boolean glob;
    boolean completion;
    boolean is_pattern;
    Quote _quote;
    boolean _new_token;
    privbuf_charbuf buf;
    privbuf_charbuf flags;
    privbuf_strvec vec;
    void start_token();
    void filegen();
    void expand(_SHXcomponents&, char* const, char*, int);
public:
    _SHXbuf()                    {reset(TRUE, FALSE);}
    void reset(boolean glob, boolean completion);
    void append(int const ch, char flag = 0);
    void append(char const *cp, char flag = 0);
    int ntokens()                {return vec.size();}
    boolean new_token()          {return _new_token;}
    Quote quote()		 {return _quote;}
    void quote(Quote);
    char **vector();
};

#ifndef NULL
#define NULL 0
#endif
