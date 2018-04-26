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
/* $XConsortium: tokline.c /main/3 1995/11/01 18:52:58 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*         THIS IS PROPRIETARY SOURCE CODE LICENSED BY          *
*                          AT&T CORP.                          *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*                     All Rights Reserved                      *
*                                                              *
*           This software is licensed by AT&T Corp.            *
*       under the terms and conditions of the license in       *
*       http://www.research.att.com/orgs/ssr/book/reuse        *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                     gsf@research.att.com                     *
*                                                              *
***************************************************************/

/* : : generated by proto : : */

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
#include <ast.h>

typedef struct
{
	Sfdisc_t	disc;
	Sfio_t*		sp;
	int		quote;
	int*		line;
} SPLICE;

/*
 * the splicer
 */

static int
splice __PARAM__((Sfio_t* s, int op, Sfdisc_t* ad), (s, op, ad)) __OTORP__(Sfio_t* s; int op; Sfdisc_t* ad;){
	SPLICE*		d = (SPLICE*)ad;
	register char*	b;
	register int	c;
	register int	n;
	register int	q;
	register char*	e;
	char*		buf;

	switch (op)
	{
	case SF_CLOSE:
		sfclose(d->sp);
		return(0);
	case SF_DPOP:
		free(d);
		return(0);
	case SF_READ:
		do
		{
			if (!(buf = sfgetr(d->sp, '\n', 0)) && !(buf = sfgetr(d->sp, '\n', -1)))
				return(0);
			n = sfslen();
			q = d->quote;
			(*d->line)++;
			if (n > 1 && buf[n - 2] == '\\')
			{
				n -= 2;
				if (q == '#')
				{
					n = 0;
					break;
				}
			}
			else if (q == '#')
			{
				q = 0;
				n = 0;
				break;
			}
			if (n > 0)
			{
				e = (b = buf) + n;
				while (b < e)
				{
					if ((c = *b++) == '\\') b++;
					else if (c == q) q = 0;
					else if (!q)
					{
						if (c == '\'' || c == '"') q = c;
						else if (c == '#' && (b == (buf + 1) || (c = *(b - 2)) == ' ' || c == '\t'))
						{
							if (buf[n - 1] != '\n')
							{
								q = '#';
								n = b - buf - 2;
							}
							else if (n = b - buf - 1) buf[n - 1] = '\n';
							break;
						}
					}
				}
				if (n <= 0) break;
				if (buf[n - 1] != '\n' && (s->flags & SF_STRING))
					buf[n++] = '\n';
				if (q && buf[n - 1] == '\n') buf[n - 1] = '\r';
			}
		} while (n <= 0);
		sfsetbuf(s, buf, n);
		d->quote = q;
		return(1);
	default:
		return(0);
	}
}

/*
 * open a stream to parse lines
 *
 *	flags: 0		arg: open Sfio_t* 
 *	flags: SF_READ		arg: file name
 *	flags: SF_STRING	arg: null terminated char*
 *
 * if line!=0 then it points to a line count that starts at 0
 * and is incremented for each input line
 */

Sfio_t*
tokline __PARAM__((const char* arg, int flags, int* line), (arg, flags, line)) __OTORP__(const char* arg; int flags; int* line;){
	Sfio_t*		f;
	Sfio_t*		s;
	SPLICE*		d;

	static int	hidden;

	if (!(d = newof(0, SPLICE, 1, 0))) return(0);
	if (!(s = sfopen(NiL, NiL, "s")))
	{
		free(d);
		return(0);
	}
	if (!(flags & (SF_STRING|SF_READ))) f = (Sfio_t*)arg;
	else if (!(f = sfopen(NiL, arg, (flags & SF_STRING) ? "s" : "r")))
	{
		free(d);
		sfclose(s);
		return(0);
	}
	d->disc.exceptf = splice;
	d->sp = f;
	*(d->line = line ? line : &hidden) = 0;
	sfdisc(s, (Sfdisc_t*)d);
	return(s);
}
