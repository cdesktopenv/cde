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
/* $XConsortium: sffilbuf.c /main/3 1995/11/01 18:29:01 rswiston $ */
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

/*	Fill the buffer of a stream with data.
**	If n < 0, sffilbuf() attempts to fill the buffer if it's empty.
**	If n == 0, if the buffer is not empty, just return the first byte;
**		otherwise fill the buffer and return the first byte.
**	If n > 0, even if the buffer is not empty, try a read to get as
**		close to n as possible. n is reset to -1 if stack pops.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int _sffilbuf(reg Sfio_t* f, reg int n)
#else
_sffilbuf(f,n)
reg Sfio_t	*f;	/* fill the read buffer of this stream */
reg int		n;	/* see above */
#endif
{
	reg int		r, local, rcrv, rc;

	GETLOCAL(f,local);

	/* any peek data must be preserved across stacked streams */
	rcrv = f->mode&(SF_RC|SF_RV|SF_LOCK);
	rc = f->getr;

	for(;; f->mode &= ~SF_LOCK)
	{	/* check mode */
		if(SFMODE(f,local) != SF_READ && _sfmode(f,SF_READ,local) < 0)
			return -1;
		SFLOCK(f,local);

		/* current extent of available data */
		if((r = f->endb-f->next) > 0)
		{	if(n <= 0 || (f->flags&SF_STRING))
				break;

			/* shift left to make room for new data */
			if(!(f->flags&SF_MMAP) && n > (f->size - (f->endb-f->data)) )
			{	memcpy((char*)f->data,(char*)f->next,r);
				f->endb = (f->next = f->data)+r;
			}
		}
		else if(!(f->flags&(SF_STRING|SF_MMAP)) )
			f->next = f->endb = f->endr = f->data;

		if(f->flags&SF_MMAP)
			r = n > 0 ? n : f->size;
		else if(!(f->flags&SF_STRING) )
		{	/* make sure we read no more than required */
			r = f->size - (f->endb - f->data);
			if(n > 0 && r > n && f->extent < 0 && (f->flags&SF_SHARE))
				r = n;
		}

		/* SFRD takes care of discipline read and stack popping */
		f->mode |= rcrv;
		f->getr = rc;
		if((r = SFRD(f,f->endb,r,f->disc)) >= 0)
		{	r = f->endb - f->next;
			break;
		}
	}

	SFOPEN(f,local);
	return (n == 0) ? (r > 0 ? (int)(*f->next++) : EOF) : r;
}
