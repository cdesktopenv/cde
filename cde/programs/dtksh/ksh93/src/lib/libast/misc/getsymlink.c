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
/* $XConsortium: getsymlink.c /main/3 1995/11/01 17:56:59 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*        THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF        *
*                    AT&T BELL LABORATORIES                    *
*         AND IS NOT TO BE DISCLOSED OR USED EXCEPT IN         *
*            ACCORDANCE WITH APPLICABLE AGREEMENTS             *
*                                                              *
*          Copyright (c) 1994 AT&T Bell Laboratories           *
*              Unpublished & Not for Publication               *
*                     All Rights Reserved                      *
*                                                              *
*       The copyright notice above does not evidence any       *
*      actual or intended publication of such source code      *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                   advsoft@research.att.com                   *
*                 Randy Hackbarth 908-582-5245                 *
*                  Dave Belanger 908-582-7427                  *
*                                                              *
***************************************************************/

/* : : generated by proto : : */

#line 1

#if !defined(__PROTO__)
#if defined(__STDC__) || defined(__cplusplus) || defined(_proto) || defined(c_plusplus)
#if defined(__cplusplus)
#define __MANGLE__	"C"
#else
#define __MANGLE__
#endif
#define __STDARG__
#define __PROTO__(x)	x
#define __OTORP__(x)
#define __PARAM__(n,o)	n
#if !defined(__STDC__) && !defined(__cplusplus)
#if !defined(c_plusplus)
#define const
#endif
#define signed
#define void		int
#define volatile
#define __V_		char
#else
#define __V_		void
#endif
#else
#define __PROTO__(x)	()
#define __OTORP__(x)	x
#define __PARAM__(n,o)	o
#define __MANGLE__
#define __V_		char
#define const
#define signed
#define void		int
#define volatile
#endif
#if defined(__cplusplus) || defined(c_plusplus)
#define __VARARG__	...
#else
#define __VARARG__
#endif
#if defined(__STDARG__)
#define __VA_START__(p,a)	va_start(p,a)
#else
#define __VA_START__(p,a)	va_start(p)
#endif
#endif

#line 7
#include "univlib.h"

#ifdef UNIV_MAX

#include <ctype.h>

#endif

/*
 * return external representation for symbolic link text of name in buf
 * the link text string length is returned
 */

int
getsymlink __PARAM__((const char* name, char* buf, int siz), (name, buf, siz)) __OTORP__(const char* name; char* buf; int siz;)
#line 22
{
	int	n;

	if ((n = readlink(name, buf, siz)) < 0) return(-1);
	if (n >= siz)
	{
		errno = EINVAL;
		return(-1);
	}
	buf[n] = 0;
#ifdef UNIV_MAX
	if (isspace(*buf))
	{
		register char*	s;
		register char*	t;
		register char*	u;
		register char*	v;
		int		match = 0;
		char		tmp[PATH_MAX];

		s = buf;
		t = tmp;
		while (isalnum(*++s) || *s == '_' || *s == '.');
		if (*s++)
		{
			for (;;)
			{
				if (!*s || isspace(*s))
				{
					if (match)
					{
						*t = 0;
						n = t - tmp;
						strcpy(buf, tmp);
					}
					break;
				}
				if (t >= &tmp[sizeof(tmp)]) break;
				*t++ = *s++;
				if (!match && t < &tmp[sizeof(tmp) - univ_size + 1]) for (n = 0; n < UNIV_MAX; n++)
				{
					if (*(v = s - 1) == *(u = univ_name[n]))
					{
						while (*u && *v++ == *u) u++;
						if (!*u)
						{
							match = 1;
							strcpy(t - 1, univ_cond);
							t += univ_size - 1;
							s = v;
							break;
						}
					}
				}
			}
		}
	}
#endif
	return(n);
}
