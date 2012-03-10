/*
 * $XConsortium: mbschr.C /main/5 1996/06/21 17:36:36 ageorge $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <codelibs/mbstring.h>
#include <codelibs/nl_hack.h>

#ifdef __cplusplus
extern "C"
#endif
#if defined(__cplusplus) || defined(__STDC__)
char *
_mb_schr(const char *str, wchar_t ch)
#else
char *
_mb_schr(str, ch)
register unsigned char *str;
register wchar_t ch;
#endif
{
    wchar_t __nlh_char[1];

    for (; *str != '\0'; ADVANCE(str))
	if (CHARAT(str) == ch)
	    return (char *)str;

    return (char *)0;
}
