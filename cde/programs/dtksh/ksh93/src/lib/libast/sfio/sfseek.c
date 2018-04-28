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
/* $XConsortium: sfseek.c /main/3 1995/11/01 18:35:35 rswiston $ */
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

/*	Set the IO pointer to a specific location in the stream
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
long sfseek(reg Sfio_t* f, reg long p, reg int type)
#else
long sfseek(f,p,type)
reg Sfio_t	*f;	/* seek to a new location in this stream */
reg long	p;	/* place to seek to */
int		type;	/* 0: from org, 1: from here, 2: from end */
#endif
{
	reg long	r, s;
	reg int		mode, local;

	GETLOCAL(f,local);

	/* set and initialize the stream to a definite mode */
	if(SFMODE(f,local) != (mode = f->mode&SF_RDWR) && _sfmode(f,mode,local) < 0 )
		return -1L;

	if(type < 0 || type > 2 || f->extent < 0L)
		return -1L;

	/* throw away ungetc data */
	if(f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	/* lock the stream for internal manipulations */
	SFLOCK(f,local);

	/* clear error and eof bits */
	f->flags &= ~(SF_EOF|SF_ERROR);

	while(f->flags&SF_STRING)
	{	SFSTRSIZE(f);

		if(type == 1)
			r = p + (f->next - f->data);
		else if(type == 2)
			r = p + f->extent;
		else	r = p;

		if(r >= 0 && r <= f->size)
		{	p = r;
			f->next = f->data+p;
			f->here = p;
			if(p > f->extent)
				memclear((char*)(f->data+f->extent),(int)(p-f->extent));
			goto done;
		}

		/* check exception handler, note that this may pop stream */
		if(SFSK(f,r,0,f->disc) != 0)
		{	p = -1L;
			goto done;
		}
	}

	/* flush any pending write data */
	if((f->mode&SF_WRITE) && f->next > f->data && SFSYNC(f) < 0)
	{	p = -1L;
		goto done;
	}

	/* currently known seek location */
	if((f->flags&(SF_SHARE|SF_PUBLIC)) == (SF_SHARE|SF_PUBLIC))
	{	s = SFSK(f,0L,1,f->disc);
		if(s == f->here)
			goto logical_seek;
	}
	else
	{ logical_seek :
		s = f->here + ((f->mode&SF_READ) ? f->next-f->endb : f->next-f->data);
	}

	/* no need to do anything */
	if(!(f->flags&SF_SHARE) && type == 0 && s == p)
		goto done;

#ifdef MAP_TYPE
	if(f->flags&SF_MMAP)
	{	/* move buffer pointer if possible */
		p += type == 0 ? 0L : type == 1 ? s : f->extent;
		if(p >= (f->here - (f->endb-f->data)) && p < f->here)
			f->next = f->endb - (f->here - p);
		else if(p >= 0)
		{	if(!(f->flags&SF_SHARE) || SFSK(f,p,0,f->disc) == p)
				f->here = p;

			/* map is now invalid */
			if(f->data)
			{	(void)munmap((caddr_t)f->data,f->endb-f->data);
				f->endb = f->endr = f->endw =
				f->next = f->data = NIL(uchar*);
			}
		}

		goto done;
	}
#endif

	if(type == 1 && p == 0 && !(f->flags&(SF_APPEND|SF_SHARE)) )
	{	/* certify current location only */
		if((p = SFSK(f,f->here,0,f->disc)) != f->here)
			p = -1L;
		else	p = s;
		goto done;
	}

	if(f->mode&SF_WRITE)
	{	if(!(f->flags&SF_SHARE) && type < 2 && ((r = type == 0 ? p : p+s)) == s)
		{	/* at where we are supposed to be */
			p = r;
			goto done;
		}
	}
	else if(type < 2 && !(f->flags&(SF_SHARE|SF_APPEND)) &&
		(r = p + (type == 0 ? 0L : s)) > f->here && r < f->extent)
	{	/* move forward by reading */
		if(f->next == f->endb)
			f->endb = f->endr = f->next = f->data;
		if(r < (s + (f->size - (f->endb-f->data))))
			(void)SFFILBUF(f,-1);
	}

	if(type < 2)
	{	/* place to seek to */	
		p += type == 0 ? 0L : s;

		if((f->mode&SF_READ) && !(f->flags&(SF_SHARE|SF_APPEND)) &&
		   p >= (f->here - (f->endb-f->data)) && p <= f->here)
		{	/* still in bound, safe to just move the pointer */
			f->next = f->endb - (f->here-p);
			goto done;
		}
	}
	else
	{	if((p = SFSK(f,p,2,f->disc)) < 0)
			goto done;
		f->here = p;
	}

	if(f->mode&SF_READ)
	{	/* any buffered data is invalid */
		f->next = f->endr = f->endb = f->data;

		/* seek to a rounded boundary to improve performance */
		if(f->size > 1 && p < f->extent && (r = (p/f->size)*f->size) < p &&
		   SFSK(f,r,0,f->disc) == r)
		{	f->here = r;
			SFFILBUF(f,-1);
			if(f->here < p)
				p = -1L;
			else	f->next = f->endb - (f->here-p);
			goto done;
		}
	}

	/* if get here must do a seek */
	if(type < 2 && (p = SFSK(f,p,0,f->disc)) >= 0)
		f->here = p;

done :
	SFOPEN(f,local);
	return p;
}
