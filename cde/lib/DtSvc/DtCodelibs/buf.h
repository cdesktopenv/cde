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
