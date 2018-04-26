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
/* $XConsortium: sfdisc.c /main/3 1995/11/01 18:27:50 rswiston $ */
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

/*	Set a new discipline for a stream.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
Sfdisc_t* sfdisc(reg Sfio_t* f, reg Sfdisc_t* disc)
#else
Sfdisc_t* sfdisc(f,disc)
reg Sfio_t	*f;
reg Sfdisc_t	*disc;
#endif
{
	reg Sfdisc_t	*d;
	reg Sfdisc_t	*rdisc;
	reg Sfread_f	oreadf;
	reg Sfwrite_f	owritef;
	reg Sfseek_f	oseekf;

	if((f->flags&(SF_PROCESS|SF_READ)) == (SF_PROCESS|SF_READ) &&
	   (f->mode&SF_WRITE) )
	{	/* make sure in read mode to check for read-ahead data */
		if(_sfmode(f,SF_READ,0) < 0)
			return NIL(Sfdisc_t*);
	}
	else if((f->mode&SF_RDWR) != f->mode && _sfmode(f,0,0) < 0)
		return NIL(Sfdisc_t*);

	SFLOCK(f,0);
	rdisc = NIL(Sfdisc_t*);

	/* synchronize before switching to a new discipline */
	if(!(f->flags&SF_STRING))
	{	if(((f->mode&SF_WRITE) && f->next > f->data) ||
		   (f->mode&SF_READ) || f->disc == _Sfudisc )
			(void)SFSYNC(f);

		if(((f->mode&SF_WRITE) && f->next > f->data) ||
		   ((f->mode&SF_READ) && f->extent < 0 && f->next < f->endb) )
		{
			reg Sfexcept_f	exceptf;
			reg int		rv = 0;

			exceptf = disc ? disc->exceptf : f->disc ? f->disc->exceptf :
				  NIL(Sfexcept_f);

			/* check with application for course of action */
			if(exceptf)
			{	SFOPEN(f,0);
				rv = (*exceptf)(f,SF_DBUFFER,
						disc ? NIL(Sfdisc_t*) : f->disc);
				SFLOCK(f,0);
			}

			/* can't switch discipline at this time */
			if(rv <= 0)
				goto done;
		}
	}

	/* save old readf, writef, and seekf to see if stream need reinit */
#define GETDISCF(func,iof,type) \
	{ for(d = f->disc; d && !d->iof; d = d->disc) ; \
	  func = d ? d->iof : NIL(type); \
	}
	GETDISCF(oreadf,readf,Sfread_f);
	GETDISCF(owritef,writef,Sfwrite_f);
	GETDISCF(oseekf,seekf,Sfseek_f);

	if(disc == SF_POPDISC)
	{	/* popping, warn the being popped discipline */
		if(!(d = f->disc) )
			goto done;
		disc = d->disc;
		if(d->exceptf)
		{	SFOPEN(f,0);
			if((*(d->exceptf))(f,SF_DPOP,d) < 0 )
				goto done;
			SFLOCK(f,0);
		}
		f->disc = disc;
		rdisc = d;
	}
	else
	{	/* pushing, warn being pushed discipline */
		do
		{	/* loop to handle the case where d may pop itself */
			d = f->disc;
			if(d && d->exceptf)
			{	SFOPEN(f,0);
				if( (*(d->exceptf))(f,SF_DPUSH,d) < 0 )
					goto done;
				SFLOCK(f,0);
			}
		} while(d != f->disc);

		/* make sure we are not creating an infinite loop */
		for(; d; d = d->disc)
			if(d == disc)
				goto done;

		/* set new disc */
		disc->disc = f->disc;
		f->disc = disc;
		rdisc = disc;
	}

	if(!(f->flags&SF_STRING) )
	{	/* this stream may have to be reinitialized */
		reg int	reinit = 0;
#define DISCF(dst,iof,type)	(dst ? dst->iof : NIL(type)) 
#define REINIT(oiof,iof,type) \
		if(!reinit) \
		{	for(d = f->disc; d && !d->iof; d = d->disc) ; \
			if(DISCF(d,iof,type) != oiof) \
				reinit = 1; \
		}

		REINIT(oreadf,readf,Sfread_f);
		REINIT(owritef,writef,Sfwrite_f);
		REINIT(oseekf,seekf,Sfseek_f);

		if(reinit)
		{	SETLOCAL(f);
			f->flags &= ~SF_HOLE;	/* turn off /dev/null handling */
			if((f->flags&SF_MMAP) || (f->mode&SF_INIT))
				sfsetbuf(f,NIL(Void_t*),-1);
			else if(f->data == f->tiny)
				sfsetbuf(f,NIL(Void_t*),0);
			else
			{	int	flags = f->flags;
				sfsetbuf(f,(Void_t*)f->data,f->size);
				f->flags |= (flags&SF_MALLOC);
			}
		}
	}

done :
	SFOPEN(f,0);
	return rdisc;
}
