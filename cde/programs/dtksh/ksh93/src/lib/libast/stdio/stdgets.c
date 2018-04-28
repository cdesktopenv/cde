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
/* $XConsortium: stdgets.c /main/3 1995/11/01 18:39:28 rswiston $ */
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
#include	"stdio.h"

/*	Read a line into a buffer.
**
**	Written by Kiem-Phong Vo (10/15/91)
*/

#if __STD_C
char *_stdgets(reg Sfio_t *f, char* us, reg int n, int isgets)
#else
char *_stdgets(f,us,n,isgets)
reg Sfio_t	*f;	/* stream to read from */
char		*us;	/* space to read into */
reg int		n;	/* max number of bytes to read */
int		isgets;	/* gets(), not fgets() */
#endif
{
	reg int		p;
	reg uchar	*is, *ps;

	if(n <= 0 || !us || (f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0))
		return NIL(char*);

	SFLOCK(f,0);

	n -= 1;
	is = (uchar*)us;
	
	while(n)
	{	/* peek the read buffer for data */
		if((p = f->endb - (ps = f->next)) <= 0 )
		{	f->getr = '\n';
			f->mode |= SF_RC;
			if(SFRPEEK(f,ps,p) <= 0)
				break;
		}

		if(p > n)
			p = n;

#if _vax_asm	/* p is r9, is is r8, ps is r7 */
		{ reg int	q;
		q = p;					/* save data length */
		asm( "locc	$0xa,r9,(r7)" );	/* look for \n */
		asm( "subl2	r0,r9" );		/* copy length */
		if(++p > q)				/* if \n found, copy it too */
			--p;
		0;					/* avoid if branching bug */
		asm( "movc3	r9,(r7),(r8)" );	/* copy data */
		ps += p;
		is += p;
		}
#else
#if _lib_memccpy
		if((ps = (uchar*)memccpy((char*)is,(char*)ps,'\n',p)) != NIL(uchar*))
			p = ps-is;
		is += p;
		ps  = f->next+p;
#else
		if(!(f->flags&(SF_BOTH|SF_MALLOC)))
		{	while(p-- && (*is++ = *ps++) != '\n')
				;
			p = ps-f->next;
		}
		else
		{	reg int	c = ps[p-1];
			if(c != '\n')
				ps[p-1] = '\n';
			while((*is++ = *ps++) != '\n')
				;
			if(c != '\n')
			{	f->next[p-1] = c;
				if((ps-f->next) >= p)
					is[-1] = c;
			}
		}
#endif
#endif

		/* gobble up read data and continue */
		f->next = ps;
		if(is[-1] == '\n')
			break;
		else if(n > 0)
			n -= p;
	}

	if((_Sfi = is - ((uchar*)us)) <= 0)
		us = NIL(char*);
	else if(isgets && is[-1] == '\n')
	{	is[-1] = '\0';
		_Sfi -= 1;
	}
	else	*is = '\0';

	SFOPEN(f,0);
	return us;
}
