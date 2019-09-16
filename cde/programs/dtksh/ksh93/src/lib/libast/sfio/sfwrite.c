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
/* $XConsortium: sfwrite.c /main/3 1995/11/01 18:39:07 rswiston $ */
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

/*	Write data out to the file system
**	Note that the reg declarations below must be kept in
**	their relative order so that the code will configured
**	correctly on Vaxes to use "asm()".
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int sfwrite(reg Sfio_t* f, const Void_t* buf, reg int n)
#else
sfwrite(f,buf,n)
reg Sfio_t*	f;	/* write to this stream. r11 on Vax	*/
Void_t*		buf;	/* buffer to be written.		*/
reg int		n;	/* number of bytes. r10 on Vax		*/
#endif
{
	reg char*	s;	/* r9 on Vax	*/
	reg uchar*	next;	/* r8 on Vax	*/
	reg int		w;	/* r7 on Vax	*/
	reg char*	begs;
	reg int		local;

	GETLOCAL(f,local);

	/* release peek lock */
	if(f->mode&SF_PEEK)
	{	if(!(f->mode&SF_WRITE) && (f->flags&SF_RDWR) != SF_RDWR)
			return -1;

		if((uchar*)buf != f->next)
		{	reg Sfrsrv_t*	frs = _sfrsrv(f,0);
			if((uchar*)buf != frs->data)
				return -1;
		}

		f->mode &= ~SF_PEEK;

		if(f->mode&SF_PKRD)
		{	/* read past peeked data */
			char	buf[16];
			reg int	r;

			for(w = n; w > 0; )
			{	if((r = w) > sizeof(buf))
					r = sizeof(buf);
				if((r = read(f->file,buf,r)) <= 0)
				{	n -= w;
					break;
				}
				else	w -= r;
			}

			f->mode &= ~SF_PKRD;
			f->endb = f->data + n;
			f->here += n;
		}

		if((f->mode&SF_READ) && (f->flags&SF_PROCESS))
			f->next += n;
	}

	s = begs = (char*)buf;
	for(;; f->mode &= ~SF_LOCK)
	{	/* check stream mode */
		if(SFMODE(f,local) != SF_WRITE && _sfmode(f,SF_WRITE,local) < 0 )
			return s > begs ? s-begs : -1;

		if(n <= 0)
			break;

		SFLOCK(f,local);

		/* current available buffer space */
		if((w = f->endb - f->next) > n)
			w = n;

		if((uchar*)s == f->next)
		{	/* fast write after sfreserve() */
			f->next += w;
			s += w;
			n = 0;
			break;
		}

		if(w > 0 && ((f->flags&SF_STRING) || w != (f->endb-f->data)) )
		{	/* copy into buffer */
			next = f->next;

#if _vax_asm		/* s is r9, next is r8, w is r7 */
			asm( "movc3	r7,(r9),(r8)" );
			s += w;
			next += w;
#else
			MEMCPY(next,s,w);
#endif
			f->next = next;
			if((n -= w) <= 0)
				break;
		}
		else if(!(f->flags&SF_STRING) && f->next > f->data)
		{	if(SFFLSBUF(f,-1) < 0)
				break;
		}
		else if((w = SFWR(f,s,n,f->disc)) == 0)
			break;
		else if(w > 0)
		{	s += w;
			if((n -= w) <= 0)
				break;
		}
	}

	/* always flush buffer for share streams */
	if(f->extent < 0 && (f->flags&SF_SHARE) && !(f->flags&SF_PUBLIC) )
		(void)SFFLSBUF(f,-1);

	/* check to see if buffer should be flushed */
	else if(n == 0 && (f->flags&SF_LINE) && !(f->flags&SF_STRING))
	{	if((n = f->next-f->data) > (w = s-begs))
			n = w;
		if(n > 0 && n < HIFORLINE)
		{	w = *(next = f->next-n); *next = '\n'; next += n;
			while(*--next != '\n')
				;
			f->next[-n] = w;
			if(*next == '\n')
				n = HIFORLINE;
		}
		if(n >= HIFORLINE)
			(void)SFFLSBUF(f,-1);
	}

	SFOPEN(f,local);

	return s-begs;
}
