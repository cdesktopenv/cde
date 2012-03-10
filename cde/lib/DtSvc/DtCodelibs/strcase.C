/*
 * $XConsortium: strcase.C /main/4 1996/04/21 19:09:37 drk $
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
#include <codelibs/stringx.h>

char *strupper(char *str)
{
    int len;

    if (str != NULL)
      {
	for (register char *s = str; *s != '\0'; s++)
	  if ((len = mblen(s, MB_CUR_MAX)) > 1)
	    s += len;
	  else
	    *s = toupper((unsigned char)*s);
      }

    return str;
}

char *strlower(char *str)
{
    int len;

    if (str != NULL)
      {
	for (register char *s = str; *s != '\0'; s++)
	  if ((len = mblen(s, MB_CUR_MAX)) > 1)
	    s += len;
	  else
	    *s = tolower((unsigned char)*s);
      }

    return str;
}
