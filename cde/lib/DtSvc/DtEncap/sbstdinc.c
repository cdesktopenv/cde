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
 * File:         sbstdinc.c $TOG: sbstdinc.c /main/5 1999/10/14 15:06:26 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define  SBSTDINC_H_NO_REDEFINE

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */

#include <stdarg.h>

static XeChar XESTRING_SAFE_NULL[1] = {(XeChar)0};

/************************************************************************/
/* Routines from <string.h> 						*/
/************************************************************************/

XeString Xestrcat(XeString s1, ConstXeString s2) 
{ 
    if (!s1) return XeString_NULL; 
    if (!s2) return s1;
  
   return (XeString) strcat(s1, s2); 
}

XeString Xestrncat(XeString s1, ConstXeString s2, size_t n) 
{
    if (!s1) return XeString_NULL; 
    if (!s2) return s1; 

    return (XeString) strncat(s1, s2, n); 
}

int Xestrcmp(ConstXeString s1, ConstXeString s2) 
{ 
    if (s1 == s2) return 0; 
    {
        const XeChar * p1 = (s1) ? s1 : XESTRING_SAFE_NULL;
	const XeChar * p2 = (s2) ? s2 : XESTRING_SAFE_NULL;

	return strcmp(p1, p2); 
    }
}

int Xestrncmp(ConstXeString s1, ConstXeString s2, size_t n) 
{
    if (s1 == s2) return 0; 
    {
	ConstXeString p1 = (s1) ? s1 : XESTRING_SAFE_NULL; 
	ConstXeString p2 = (s2) ? s2 : XESTRING_SAFE_NULL; 
	return strncmp( (char *) p1, (char *) p2, n); 
    }
}

XeString Xestrcpy(XeString s1, ConstXeString s2) 
{ 
    if (!s1) return s1; 
    if (!s2) { 
	*s1 = (XeChar)0; 
	return s1; 
    } 

   return (XeString) strcpy(s1, s2); 
}

XeString Xestrncpy(XeString s1, ConstXeString s2, size_t n) 
{
    if (!s1) return s1; 
    if (!s2 && n) { 
	*s1 = (XeChar)0; 
	return s1; 
    } 

   return (XeString) strncpy(s1, s2, n); 
}

int Xestrcoll(ConstXeString s1, ConstXeString s2) 
{ 
    if (s1 == s2) return 0; 
    {
	XeString p1 = (s1) ? (XeString) s1 : XESTRING_SAFE_NULL; 
	XeString p2 = (s2) ? (XeString) s2 : XESTRING_SAFE_NULL; 
	return strcoll(p1, p2);
    }
}

size_t Xestrxfrm(XeString s1, ConstXeString s2, size_t n) 
{ 
    return strxfrm(s1, s2, n); 
}

XeString Xestrchr(ConstXeString s, int c) 
{ 
    if (!s) return XeString_NULL; 

    return (XeString) strchr(s, c); 
}

XeString Xestrrchr(ConstXeString s, int c) 
{ 
    if (!s) return XeString_NULL; 

    return (XeString) strrchr(s, c); 
}

XeString Xestrpbrk(ConstXeString s1, ConstXeString s2) 
{ 
    if (!s1) return XeString_NULL; 
    if (!s2) return XeString_NULL; 

    return (XeString) strpbrk(s1, s2); 
}

XeString Xestrstr(ConstXeString s1, ConstXeString s2) 
{ 
    if (!s1) return XeString_NULL; 
    if (!s2) return XeString_NULL;

    return (XeString) strstr(s1, s2); 
}

XeString Xestrtok(XeString s1, ConstXeString s2) 
{ 
    /* s1 is null except after the first call */
    if (!s2) return XeString_NULL;

    return (XeString) strtok(s1, s2); 
}

size_t Xestrlen(ConstXeString const s) 
{ 
    if (!s) return (size_t)0; 

    return strlen((char *) s); 
}

XeString Xestrdup(ConstXeString s) 
{ 
    if (!s) return XeString_NULL; 

    return (XeString) strdup(s); 
}
