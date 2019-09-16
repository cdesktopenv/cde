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
/* $XConsortium: sfclose.c /main/3 1995/11/01 18:27:11 rswiston $ */
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

/*	Close a stream. A file stream is synced before closing.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int sfclose(reg Sfio_t* f)
#else
sfclose(f)
reg Sfio_t*	f;
#endif
{
	reg int		local;
	reg Sfdisc_t*	disc;

	if(!f)
		return -1;

	GETLOCAL(f,local);

	if(!(f->mode&SF_INIT) &&
	   SFMODE(f,local) != (f->mode&SF_RDWR) &&
	   SFMODE(f,local) != (f->mode&(SF_READ|SF_SYNCED)) &&
	   _sfmode(f,0,local) < 0)
		return -1;

	/* closing a stack of streams */
	while(f->push)
	{	reg Sfio_t*	pop;

		if(!(pop = (*_Sfstack)(f,NIL(Sfio_t*))) )
			return -1;
		if(sfclose(pop) < 0)
		{	(*_Sfstack)(f,pop);
			return -1;
		}
	}

	/* this is from popen */
	if(f->flags&SF_PROCESS)
	{	if(local)
			SETLOCAL(f);
		return _sfpclose(f);
	}

	if(f->disc == _Sfudisc)	/* closing the ungetc stream */
		f->disc = NIL(Sfdisc_t*);
	/* sync file pointer and announce SF_SYNC event */
	else if((f->disc && f->disc->exceptf) || (f->flags&(SF_SHARE|SF_WRITE)) )
		(void)sfsync(f);

	SFLOCK(f,0);

	/* zap any associated auxiliary buffer */
	(void)_sfrsrv(f,-1);

	/* terminate disciplines */
	for(disc = f->disc; disc; )
	{	reg Sfdisc_t*	next = disc->disc;
		reg int		ex;

		if(disc->exceptf)
		{	SFOPEN(f,0);
			ex = (*disc->exceptf)(f,local ? SF_NEW : SF_CLOSE,disc );
			if(ex < 0)
				return ex;
			else if(ex > 0)
				return 0;
			SFLOCK(f,0);
		}

		if((disc = next) )
		{	reg Sfdisc_t*	d;

			/* make sure that the next discipline hasn't been popped */
			for(d = f->disc; d; d = d->disc)
				if(d == disc)
					break;
			if(!d)
				disc = f->disc;
		}
	}

	if(!local && f->pool)
	{	/* remove from pool */
		if(f->pool == &_Sfpool)
		{	reg int	n;
			for(n = 0; n < _Sfpool.n_sf; ++n)
			{	if(_Sfpool.sf[n] != f)
					continue;
				/* found it */
				_Sfpool.n_sf -= 1;
				for(; n < _Sfpool.n_sf; ++n)
					_Sfpool.sf[n] = _Sfpool.sf[n+1];
				break;
			}
		}
		else
		{	f->mode &= ~SF_LOCK;	/**/ASSERT(_Sfpmove);
			if((*_Sfpmove)(f,-1) < 0)
			{	SFOPEN(f,0);
				return -1;
			}
			f->mode |= SF_LOCK;
		}

		f->disc = NIL(Sfdisc_t*);
	}

	/* tell the function */
	if(_Sfnotify)
		(*_Sfnotify)(f,SF_CLOSE,f->file);

	if(f->data && (!local || (f->flags&(SF_STRING|SF_MMAP))))
	{	/* free buffer */
#ifdef MAP_TYPE
		if(f->flags&SF_MMAP)
			munmap((caddr_t)f->data,f->endb-f->data);
		else
#endif
		if(f->flags&SF_MALLOC)
			free((char*)f->data);

		f->data = NIL(uchar*);
		f->size = -1;
	}

	/* zap the file descriptor */
	if(f->file >= 0 && !(f->flags&SF_STRING))
		CLOSE(f->file);
	f->file = -1;

	f->mode = SF_AVAIL|SF_LOCK;	/* prevent muttiple closings */

	if(!local && !(f->flags&SF_STATIC) )
		SFFREE(f);
	else
	{	f->flags = (f->flags&SF_STATIC);
		f->here = 0L;
		f->extent = -1L;
		f->endb = f->endr = f->endw = f->next = f->data;
	}

	return 0;
}
