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
/* $XConsortium: sfpoll.c /main/3 1995/11/01 18:32:40 rswiston $ */
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

/*	Poll a set of streams to see if any is available for I/O.
**
**	Written by Kiem-Phong Vo (06/18/92).
*/

#if __STD_C
int sfpoll(Sfio_t** fa, reg int n, int tm)
#else
sfpoll(fa, n, tm)
Sfio_t**	fa;	/* array of streams to poll */
reg int		n;	/* number of streams in array */
int		tm;	/* the amount of time in ms to wait for selecting */
#endif
{
	reg int		r, c, m;
	reg Sfio_t*	f;
	reg Sfdisc_t*	d;

	if(n <= 0 || !fa)
		return -1;

	/* this loop partitions the streams into 3 sets: Ready, Check, Notready */
	r = c = 0;
	while(c < n)
	{	f = fa[c];

		/* this loop pops a stream stack as necessary */
		for(;;)
		{	/* check accessibility */
			m = f->mode&SF_RDWR;
			if(f->mode != m && _sfmode(f,m,0) < 0)
				goto not_ready;

			/* clearly ready */
			if(f->next < f->endb)
				goto ready;

			/* has discipline, ask its opinion */
			for(d = f->disc; d; d = d->disc)
				if(d->exceptf)
					break;
			if(d)
			{	if((m = (*d->exceptf)(f,SF_DPOLL,d)) < 0)
					goto not_ready;
				else if(m > 0)
					goto ready;
				/*else check file descriptor*/
			}

			/* unseekable stream, must check for blockability */
			if(f->extent < 0)
				goto check;

			/* string/regular streams with no possibility of blocking */
			if(!f->push)
				goto ready;

			/* stacked regular file stream with I/O possibility */
			if(!(f->flags&SF_STRING) &&
			   ((f->mode&SF_WRITE) || f->here < f->extent) )
				goto ready;

			/* at an apparent eof, pop stack if ok, then recheck */
			SETLOCAL(f);
			switch(_sfexcept(f,f->mode&SF_RDWR,0,f->disc))
			{
			case SF_EDONE:
				if(f->flags&SF_STRING)
					goto not_ready;
				else	goto ready;
			case SF_EDISC:
				if(f->flags&SF_STRING)
					goto ready;
			case SF_ESTACK:
			case SF_ECONT:
				continue;
			}
		}

		check:	/* local function to set a stream for further checking */
		{	c += 1;
			continue;
		}

		ready:	/* local function to set the ready streams */
		{	if(r < c)
			{	fa[c] = fa[r];
				fa[r] = f;
			}
			r += 1;
			c += 1;
			continue;
		}

		not_ready: /* local function to set the not-ready streams */
		{	if((n -= 1) > c)
			{	fa[c] = fa[n];
				fa[n] = f;
			}
			continue;
		}
	}

#if _lib_poll
	if(r == 0 && c > 0)
	{	static struct pollfd*	fds;
		static int		n_fds;

		if(c > n_fds)	/* get space for the poll structures */
		{	if(n_fds > 0)
				free((char*)fds);
			if(!(fds = (struct pollfd*)malloc(c*sizeof(struct pollfd))) )
			{	n_fds = 0;
				return -1;
			}
			n_fds = c;
		}

		/* construct the poll array */
		for(m = 0; m < c; ++m)
		{	fds[m].fd = fa[m]->file;
			fds[m].events = (fa[m]->mode&SF_READ) ? POLLIN : POLLOUT;
			fds[m].revents = 0;
		}

		for(;;)	/* this loop takes care of interrupts */
		{	reg int	p;
			if((p = SFPOLL(fds,c,tm)) == 0)
				break;
			else if(p < 0)
			{	if(errno == EINTR || errno == EAGAIN)
				{	errno = 0;
					continue;
				}
				else	break;
			}

			while(r < c)
			{	f = fa[r];
				if(((f->mode&SF_READ) && (fds[r].revents&POLLIN)) ||
			   	((f->mode&SF_WRITE) && (fds[r].revents&POLLOUT)) )
					r += 1;
				else if((c -= 1) > r)
				{	fa[r] = fa[c];
					fa[c] = f;
				}
			}
			break;
		}
	}
#endif /*_lib_poll*/

#if _lib_select
	if(r == 0 && c > 0)
	{	fd_set		rd, wr;
		struct timeval	tmb, *tmp;

		FD_ZERO(&rd);
		FD_ZERO(&wr);
		m = 0;
		for(n = 0; n < c; ++n)
		{	f = fa[n];
			if(f->file > m)
				m = f->file;
			if(f->mode&SF_READ)
				FD_SET(f->file,&rd);
			else	FD_SET(f->file,&wr);
		}
		if(tm < 0)
			tmp = NIL(struct timeval*);
		else
		{	tmp = &tmb;
			tmb.tv_sec = tm/SECOND;
			tmb.tv_usec = (tm%SECOND)*SECOND;
		}
		for(;;)
		{	reg int	s;
			if((s = select(m+1,&rd,&wr,NIL(fd_set*),tmp)) == 0)
				break;
			else if(s < 0)
			{	if(errno == EINTR)
					continue;
				else	break;
			}

			while(r < c)
			{	f = fa[r];
				if(((f->mode&SF_READ) && FD_ISSET(f->file,&rd)) ||
				   ((f->mode&SF_WRITE) && FD_ISSET(f->file,&wr)) )
					r += 1;
				else if((c -= 1) > r)
				{	fa[r] = fa[c];
					fa[c] = f;
				}
			}
			break;
		}
	}
#endif /*_lib_select*/

	return r;
}
