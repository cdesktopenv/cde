/*
 * $XConsortium: strend.C /main/4 1996/04/21 19:09:40 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <codelibs/stringx.h>

char *
strend(register const char *str)
{
	if (str == NULL)
		return NULL;

	while (*str != '\0')
		str++;

	return (char *)str;
}
