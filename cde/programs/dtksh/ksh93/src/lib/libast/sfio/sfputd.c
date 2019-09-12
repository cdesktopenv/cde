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
/* $XConsortium: sfputd.c /main/3 1995/11/01 18:33:55 rswiston $ */
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
#include	"sfhdr.h"

/*	Write out a double value in a portable format
**
**	Written by Kiem-Phong Vo (08/05/90)
*/

#if __STD_C
int _sfputd(Sfio_t* f, reg double v)
#else
_sfputd(f,v)
Sfio_t		*f;
reg double	v;
#endif
{
#define N_ARRAY		(16*sizeof(double))
	reg int		n, w;
	reg double	x;
	reg uchar	*s, *ends;
	int		exp;
	uchar		c[N_ARRAY];

	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;
	SFLOCK(f,0);

	/* get the sign of v */
	if(v < 0.)
	{	v = -v;
		n = 1;
	}
	else	n = 0;

	/* make the magnitude of v < 1 */
	if(v != 0.)
		v = frexp(v,&exp);
	else	exp = 0;

	/* code the sign of v and exp */
	if((w = exp) < 0)
	{	n |= 02;
		w = -w;
	}

	/* write out the signs and the exp */
	SFOPEN(f,0);
	if(sfputc(f,n) < 0 || (w = sfputu(f,w)) < 0)
		return -1;
	SFLOCK(f,0);
	w += 1;

	s = (ends = &c[0])+sizeof(c);
	while(s > ends)
	{	/* get 2^SF_PRECIS precision at a time */
		n = (int)(x = ldexp(v,SF_PRECIS));
		*--s = n|SF_MORE;
		if((v = x-n) <= 0.)
			break;
	}

	/* last byte is not SF_MORE */
	ends = &c[0] + sizeof(c) -1;
	*ends &= ~SF_MORE;

	/* write out coded bytes */
	n = ends - s + 1;
	w = SFWRITE(f,(Void_t*)s,n) == n ? w+n : -1;

	SFOPEN(f,0);
	return w;
}
