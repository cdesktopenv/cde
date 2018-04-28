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
/* $XConsortium: sfgetd.c /main/3 1995/11/01 18:29:27 rswiston $ */
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

/*	Read a portably coded double value
**
**	Written by Kiem-Phong Vo (08/05/90)
*/

#if __STD_C
double sfgetd(Sfio_t* f)
#else
double sfgetd(f)
Sfio_t	*f;
#endif
{
	reg uchar	*s, *ends, c;
	reg double	v;
	reg int		p, sign, exp;

	if((sign = sfgetc(f)) < 0 || (exp = (int)sfgetu(f)) < 0)
		return -1.;

	SFLOCK(f,0);

	v = 0.;
	for(;;)
	{	/* fast read for data */
		if(SFRPEEK(f,s,p) <= 0)
		{	f->flags |= SF_ERROR;
			v = -1.;
			goto done;
		}

		for(ends = s+p; s < ends; )
		{
			c = *s++;
			v += SFUVALUE(c);
			v = ldexp(v,-SF_PRECIS);
			if(!(c&SF_MORE))
			{	f->next = s;
				goto done;
			}
		}
		f->next = s;
	}

done:
	v = ldexp(v,(sign&02) ? -exp : exp);
	if(sign&01)
		v = -v;

	SFOPEN(f,0);
	return v;
}
