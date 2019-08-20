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
/* $XConsortium: sfwr.c /main/3 1995/11/01 18:38:54 rswiston $ */
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

/*	Write with discipline.
**
**	Written by Kiem-Phong Vo (02/11/91)
*/

/* hole preserving writes */
#if __STD_C
static int sfoutput(Sfio_t* f, reg char* buf, reg int n)
#else
static sfoutput(f,buf,n)
Sfio_t*		f;
reg char*	buf;
reg int		n;
#endif
{	reg char	*sp, *wbuf, *endbuf;
	reg int		s, w, wr;

	s = w = 0;
	wbuf = buf;
	endbuf = buf+n;
	while(n > 0)
	{
		if(n < _Sfpage)
		{	/* no hole possible */
			buf += n;
			s = n = 0;
		}
		else while(n >= _Sfpage)
		{	/* see if a hole of 0's starts here */
			sp = buf+1;
			if(buf[0] == 0 && buf[_Sfpage-1] == 0)
			{	/* check byte at a time until int-aligned */
				while(((int)sp)%sizeof(int))
				{	if(*sp != 0)
						goto check_hole;
					sp += 1;
				}

				/* check using int to speed up */
				while(sp < endbuf)
				{	if(*((int*)sp) != 0)
						goto check_hole;
					sp += sizeof(int);
				}

				/* check the remaining bytes */
				if(sp > endbuf)
				{	sp -= sizeof(int);
					while(sp < endbuf)
					{	if(*sp != 0)
							goto check_hole;
						sp += 1;
					}
				}
			}

		check_hole: /* found a hole */
			if((s = sp-buf) >= _Sfpage)
				break;

			/* skip a dirty page */
			n -= _Sfpage;
			buf += _Sfpage;
		}

		/* write out current dirty pages */
		if(buf > wbuf)
		{	if((wr = write(f->file,wbuf,buf-wbuf)) > 0)
			{	w += wr;
				f->flags &= ~SF_HOLE;
			}
			if(wr != (buf-wbuf))
				break;
			wbuf = buf;
		}

		/* seek to a rounded boundary within the hole */
		if(s >= _Sfpage)
		{	s = (s/_Sfpage)*_Sfpage;
			if(SFSK(f,(long)s,1,NIL(Sfdisc_t*)) < 0)
				break;
			w += s;
			n -= s;
			wbuf = (buf += s);
			f->flags |= SF_HOLE;

			if(n > 0)
			{	/* next page must be dirty */
				s = n <= _Sfpage ? 1 : _Sfpage;
				buf += s;
				n -= s;
			}
		}
	}

	return w > 0 ? w : -1;
}

#if __STD_C
int sfwr(reg Sfio_t* f, reg const Void_t* buf, reg int n, reg Sfdisc_t* disc)
#else
sfwr(f,buf,n,disc)
reg Sfio_t*	f;
reg Void_t*	buf;
reg int		n;
reg Sfdisc_t	*disc;
#endif
{
	reg int		w, local, string, oerrno;

	GETLOCAL(f,local);
	if(!local && !(f->mode&SF_LOCK))
		return -1;

	if(!(string = (f->flags&SF_STRING)) )
		SFDISC(f,disc,writef,local);

	for(;;)
	{
		/* stream locked by sfsetfd() */
		if(!string && f->file < 0)
			return 0;

		/* clear current error states */
		f->flags &= ~(SF_EOF|SF_ERROR);

		if(string)	/* total required buffer */
			w = n + (f->next - f->data);
		else
		{
			/* warn that a write is about to happen */
			if(disc && disc->exceptf && (f->flags&SF_IOCHECK) )
			{	reg int	rv;
				if(local)
					SETLOCAL(f);
				if((rv = _sfexcept(f,SF_WRITE,n,disc)) > 0)
					n = rv;
				else if(rv < 0)
				{	f->flags |= SF_ERROR;
					return rv;
				}
			}

			if(f->extent >= 0)
			{	/* make sure we are at the right place to write */
				if(f->flags&SF_APPEND)
				{	/* must be at the end of stream */
					if(f->here != f->extent || (f->flags&SF_SHARE))
						f->here = SFSK(f,0L,2,disc);
				}
				else if(f->flags&SF_SHARE)
				{	if(!(f->flags&SF_PUBLIC))
						f->here = SFSK(f,f->here,0,disc);
					else	f->here = SFSK(f,0L,1,disc);
				}
			}

			oerrno = errno;
			errno = 0;

			if(disc && disc->writef)
				w = (*(disc->writef))(f,buf,n,disc);
			else if(SFISNULL(f))
				w = n;
			else if(n >= _Sfpage && !(f->flags&(SF_SHARE|SF_APPEND)) &&
				f->here == f->extent && (f->here%_Sfpage) == 0)
			{	if((w = sfoutput(f,(char*)buf,n)) <= 0)
					goto do_write;
			}
			else
			{
			do_write:
				if((w = write(f->file,(char*)buf,n)) > 0)
					f->flags &= ~SF_HOLE;
			}

			if(errno == 0)
				errno = oerrno;

			if(w > 0)
			{	if(local)
				{	f->here += w;
					if(f->extent >= 0 && f->here > f->extent)
						f->extent = f->here;
				}
				return w;
			}
		}

		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_WRITE,w,disc))
		{
		case SF_ECONT :
			continue;
		case SF_EDONE :
			return local ? 0 : w;
		case SF_EDISC :
			if(!local && !string)
				continue;
			/* else fall thru */
		case SF_ESTACK :
			return -1;
		}
	}
}
