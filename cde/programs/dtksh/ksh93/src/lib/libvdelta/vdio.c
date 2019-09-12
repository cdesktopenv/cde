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
/* $XConsortium: vdio.c /main/2 1996/05/08 20:07:43 drk $ */
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
#include	"vdelhdr.h"

/*	IO subsystem for the delta routines
**
**	Written by Kiem-Phong Vo (12/15/94)
*/

#if __STD_C
static int _vdinit(reg Vdio_t* io)
#else
static _vdinit(io)
reg Vdio_t*	io;
#endif
{
	if((io->data = (uchar*)io->delta->data) )
		io->size = io->delta->size;
	else
	{	io->data = io->buf;
		io->size = sizeof(io->buf);
	}
	io->next = io->data;
	io->endb = io->data + io->size;
}


#if __STD_C
static int _vdfilbuf(reg Vdio_t* io)
#else
static _vdfilbuf(io)
reg Vdio_t*	io;
#endif
{	reg int	n;

	if(!io->data)
		_vdinit(io);

	if(io->data != io->buf)	/* all data was given in core */
		return REMAIN(io);

	if((n = (*READF(io))(DATA(io),SIZE(io),HERE(io),DELTA(io))) > 0)
	{	ENDB(io) = (NEXT(io) = DATA(io)) + n;
		HERE(io) += n;
	}
	return n;
}

#if __STD_C
static int _vdflsbuf(reg Vdio_t* io)
#else
static _vdflsbuf(io)
reg Vdio_t*	io;
#endif
{	reg int n;

	if(!io->data )
		_vdinit(io);

	if(io->data != io->buf)	/* all space was given	*/
		return REMAIN(io);

	if((n = NEXT(io) - DATA(io)) > 0 &&
	   (*WRITEF(io))(DATA(io),n,HERE(io),DELTA(io)) != n)
		return -1;

	HERE(io) += n;
	NEXT(io) = DATA(io);
	return SIZE(io);
}

#if __STD_C
static ulong _vdgetu(reg Vdio_t* io, reg ulong v)
#else
static ulong _vdgetu(io,v)
reg Vdio_t*	io;
reg ulong	v;
#endif
{	reg int		c;

	for(v &= I_MORE-1;;)
	{	if((c = VDGETC(io)) < 0)
			return (ulong)(-1L);
		if(!(c&I_MORE) )
			return ((v<<I_SHIFT) | c);
		v = (v<<I_SHIFT) | (c & (I_MORE-1));
	}
}

#if __STD_C
static int _vdputu(reg Vdio_t* io, ulong v)
#else
static _vdputu(io, v)
reg Vdio_t*	io;
reg ulong	v;
#endif
{
	reg uchar	*s, *next;
	reg int		len;
	uchar		c[sizeof(ulong)+1];

	s = next = &c[sizeof(c)-1];
	*s = I_CODE(v);
	while((v >>= I_SHIFT) )
		*--s = I_CODE(v)|I_MORE;
	len = (next-s) + 1;

	if(REMAIN(io) < len && _vdflsbuf(io) < len)
		return -1;

	next = io->next;
	switch(len)
	{
	default: memcpy((Void_t*)next,(Void_t*)s,len); next += len; break;
	case 3:	*next++ = *s++;
	case 2:	*next++ = *s++;
	case 1:	*next++ = *s;
	}
	io->next = next;

	return len;
}

#if __STD_C
static int _vdread(Vdio_t* io, reg uchar* s, reg int n)
#else
static _vdread(io, s, n)
Vdio_t*		io;
reg uchar*	s;
reg int		n;
#endif
{
	reg uchar*	next;
	reg int		r, m;

	for(m = n; m > 0; )
	{	if((r = REMAIN(io)) <= 0 && (r = _vdfilbuf(io)) <= 0)
			break;
		if(r > m)
			r = m;

		next = io->next;
		MEMCPY(s,next,r);
		io->next = next;

		m -= r;
	}
	return n-m;
}

#if __STD_C
static int _vdwrite(Vdio_t* io, reg uchar* s, reg int n)
#else
static _vdwrite(io, s, n)
Vdio_t*		io;
reg uchar*	s;
reg int		n;
#endif
{
	reg uchar*	next;
	reg int		w, m;

	for(m = n; m > 0; )
	{	if((w = REMAIN(io)) <= 0 && (w = _vdflsbuf(io)) <= 0)
			break;
		if(w > m)
			w = m;

		next = io->next;
		MEMCPY(next,s,w);
		io->next = next;

		m -= w;
	}
	return n-m;
}


Vdbufio_t	_Vdbufio =
{	_vdfilbuf,
	_vdflsbuf,
	_vdgetu,
	_vdputu,
	_vdread,
	_vdwrite
};
