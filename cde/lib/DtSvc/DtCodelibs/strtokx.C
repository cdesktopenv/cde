/*
 * $XConsortium: strtokx.C /main/4 1996/04/21 19:09:46 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <codelibs/nl_hack.h>
#include <codelibs/mbstring.h>
#include <codelibs/stringx.h>

char *
strtokx(register char *&ptr, register const char *sep)
{
    if (ptr == NULL)
	return NULL;

    // find the beginning of the token
    register char *ret = ptr;
    while (*ret != '\0' && _mb_schr(sep, *ret) != NULL)
	ADVANCE(ret);

    // find the end of the token
    register char *end = ret;
    while (*end != '\0' && _mb_schr(sep, *end) == NULL)
	ADVANCE(end);

    ptr = end;

    // If this isn't the last token, advance pointer and terminate
    // current token.
    if (*end != '\0')
    {
	ADVANCE(ptr);
	WCHAR('\0', end);
    }

    if (*ret == '\0')
	return NULL;

    return ret;
}
