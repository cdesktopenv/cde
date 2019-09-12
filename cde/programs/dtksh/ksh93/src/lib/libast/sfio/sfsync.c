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
/* $XConsortium: sfsync.c /main/3 1995/11/01 18:37:23 rswiston $ */
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

/*	Synchronize data in buffers with the file system.
**	If f is nil, all streams are sync-ed
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

static int _sfall()
{
	reg Sfpool_t	*p, *next;
	reg Sfio_t*	f;
	reg int		n, rv;
	reg int		nsync, count, loop;
#define MAXLOOP 3

	for(loop = 0; loop < MAXLOOP; ++loop)
	{	rv = nsync = count = 0;
		for(p = &_Sfpool; p; p = next)
		{	/* find the next legitimate pool */
			for(next = p->next; next; next = next->next)
				if(next->n_sf > 0)
					break;

			/* walk the streams for _Sfpool only */
			for(n = 0; n < ((p == &_Sfpool) ? p->n_sf : 1); ++n)
			{	count += 1;
				f = p->sf[n];

				if(f->flags&SF_STRING )
					goto did_sync;
				if(SFFROZEN(f))
					continue;
				if((f->mode&SF_READ) && (f->mode&SF_SYNCED) )
					goto did_sync;
				if((f->mode&SF_READ) && !(f->flags&SF_MMAP) &&
				   f->next == f->endb)
					goto did_sync;
				if((f->mode&SF_WRITE) && !(f->flags&SF_HOLE) &&
				   f->next == f->data)
					goto did_sync;

				SFLOCK(f,0);
				if(SFSYNC(f) < 0)
					rv = -1;
				SFOPEN(f,0);

			did_sync:
				nsync += 1;
			}
		}

		if(nsync == count)
			break;
	}
	return rv;
}

#if __STD_C
int sfsync(reg Sfio_t* f)
#else
sfsync(f)
reg Sfio_t*	f;	/* stream to be synchronized */
#endif
{
	reg int	local, rv, mode;

	if(!f)
		return _sfall();

	GETLOCAL(f,local);

	if(f->disc == _Sfudisc)	/* throw away ungetc */
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	rv = 0;

	if((f->mode&SF_RDWR) != SFMODE(f,local) && _sfmode(f,0,local) < 0)
	{	rv = -1;
		goto done;
	}

	for(; f; f = f->push)
	{	SFLOCK(f,local);

		/* pretend that this stream is not on a stack */
		mode = f->mode&SF_PUSH;
		f->mode &= ~SF_PUSH;

		/* these streams do not need synchronization */
		if((f->flags&SF_STRING) || (f->mode&SF_SYNCED))
			goto next;

		if((f->mode&SF_WRITE) && (f->next > f->data || (f->flags&SF_HOLE)) )
		{	/* sync the buffer, make sure pool don't move */
			reg int pool = f->mode&SF_POOL;
			f->mode &= ~SF_POOL;
			if(f->next > f->data && SFFLSBUF(f,-1) < 0)
				rv = -1;
			if(!SFISNULL(f) && (f->flags&SF_HOLE) )
			{	/* realize a previously created hole of 0's */
				if(lseek(f->file,-1L,1) >= 0)
					(void)write(f->file,"",1);
				f->flags &= ~SF_HOLE;
			}
			f->mode |= pool;
		}

		if((f->mode&SF_READ) && f->extent >= 0 &&
		   ((f->flags&SF_MMAP) || f->next < f->endb) )
		{	/* make sure the file pointer is at the right place */
			f->here -= (f->endb-f->next);
			f->endr = f->endw = f->data;
			f->mode = SF_READ|SF_SYNCED|SF_LOCK;
			(void)SFSK(f,f->here,0,f->disc);

			if((f->flags&SF_SHARE) && !(f->flags&SF_PUBLIC) &&
			   !(f->flags&SF_MMAP) )
			{	f->endb = f->next = f->data;
				f->mode &= ~SF_SYNCED;
			}
		}

	next:
		f->mode |= mode;
		SFOPEN(f,local);

		if(!local && !(f->flags&SF_ERROR) && (f->mode&~SF_RDWR) == 0 &&
		   (f->flags&SF_IOCHECK) && f->disc && f->disc->exceptf)
			(void)(*f->disc->exceptf)(f,SF_SYNC,f->disc);
	}

done:
	if(!local && f && (f->mode&SF_POOL) && f->pool && f != f->pool->sf[0])
		SFSYNC(f->pool->sf[0]);

	return rv;
}
