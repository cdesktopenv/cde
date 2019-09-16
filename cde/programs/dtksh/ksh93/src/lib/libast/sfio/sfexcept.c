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
/* $XConsortium: sfexcept.c /main/3 1995/11/01 18:28:25 rswiston $ */
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

/*	Function to handle io exceptions.
**	Written by Kiem-Phong Vo (8/18/90)
*/
#if __STD_C
int _sfexcept(reg Sfio_t* f, reg int type, reg int io, reg Sfdisc_t* disc)
#else
_sfexcept(f,type,io,disc)
reg Sfio_t	*f;	/* stream where the exception happened */
reg int		type;	/* io type that was performed */
reg int		io;	/* the io return value that indicated exception */
reg Sfdisc_t	*disc;	/* discipline in use */
#endif
{
	reg int		d, local, lock;
	reg uchar	*data;

	GETLOCAL(f,local);
	lock = f->mode&SF_LOCK;

	if(local && io <= 0)
		f->flags |= io < 0 ? SF_ERROR : SF_EOF;

	if(disc && disc->exceptf)
	{	/* let the stream be generally accessible for this duration */
		if(local && lock)
			SFOPEN(f,0);

		/* so that exception handler knows what we are asking for */
		_Sfi = io;
		d = (*(disc->exceptf))(f,type,disc);

		/* relock if necessary */
		if(local && lock)
			SFLOCK(f,0);

		if(io > 0 && !(f->flags&SF_STRING) )
			return d;
		else if(d < 0)
			return SF_EDONE;
		else if(d > 0)
			return SF_EDISC;
	}

	if(f->flags&SF_STRING)
	{	if(type == SF_READ)
			goto chk_stack;
		else if(type != SF_WRITE && type != SF_SEEK)
			return SF_EDONE;
		if(local && io >= 0)
		{	if(f->size >= 0 && !(f->flags&SF_MALLOC))
				goto chk_stack;
			/* extend buffer */
			if((d = f->size) < 0)
				d = 0;
			if((io -= d) <= 0)
				io = SF_GRAIN;
			d = ((d+io+SF_GRAIN-1)/SF_GRAIN)*SF_GRAIN;
			if(f->size > 0)
				data = (uchar*)realloc((char*)f->data,d);
			else	data = (uchar*)malloc(d);
			if(!data)
				goto chk_stack;
			f->endb = data + d;
			f->next = data + (f->next - f->data);
			f->endr = f->endw = f->data = data;
			f->size = d;
		}
		return SF_EDISC;
	}

	if(errno == EINTR)
	{	/* if just an interrupt, we can continue */
		errno = 0;
		f->flags &= ~(SF_EOF|SF_ERROR);
		return SF_ECONT;
	}

chk_stack:
	if(local && f->push &&
	   ((type == SF_READ  && f->next >= f->endb) ||
	    (type == SF_WRITE && f->next <= f->data)))
	{	/* pop the stack */
		reg Sfio_t	*pf;

		if(lock)
			SFOPEN(f,0);

		/* pop and close */
		pf = (*_Sfstack)(f,NIL(Sfio_t*));
		if((d = sfclose(pf)) < 0) /* can't close, restack */
			(*_Sfstack)(f,pf);

		if(lock)
			SFLOCK(f,0);

		return d < 0 ? SF_EDONE : SF_ESTACK;
	}

	return SF_EDONE;
}
