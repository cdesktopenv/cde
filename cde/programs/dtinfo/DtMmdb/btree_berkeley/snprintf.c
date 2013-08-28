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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: snprintf.c /main/3 1996/06/11 17:14:27 cde-hal $ */
#include <sys/types.h>
#include "cdefs.h"

#include <compat.h>
#include <string.h>
#include <stdio.h>

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

int
#ifdef __STDC__
snprintf(char *str, size_t n, const char *fmt, ...)
#else
snprintf(str, n, fmt, va_alist)
	char *str;
	size_t n;
	const char *fmt;
	va_dcl
#endif
{
	va_list ap;
#ifdef VSPRINTF_CHARSTAR
	char *rp;
#else
	int rval;
#endif

#ifdef __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif

#ifdef VSPRINTF_CHARSTAR
	rp = (char*)(size_t)vsnprintf(str, n, fmt, ap);
	va_end(ap);
	return (strlen(rp));
#else
	rval = vsnprintf(str, n, fmt, ap);
	va_end(ap);
	return (rval);
#endif
}

#if 0
int
vsnprintf(str, n, fmt, ap)
	char *str;
	size_t n;
	const char *fmt;
	va_list ap;
{
#ifdef VSPRINTF_CHARSTAR
	return (strlen((char*)(size_t)vsnprintf(str, fmt, ap)));
#else
	return (vsnprintf(str, fmt, ap));
#endif
}
#endif
