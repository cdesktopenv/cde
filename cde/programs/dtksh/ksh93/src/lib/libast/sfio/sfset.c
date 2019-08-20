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
/* $XConsortium: sfset.c /main/3 1995/11/01 18:35:47 rswiston $ */
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

/*	Set some control flags or file descript for the stream
**
**	Written by Kiem-Phong Vo (07/16/90)
*/

#if __STD_C
int sfset(reg Sfio_t* f, reg int flags, reg int set)
#else
sfset(f,flags,set)
reg Sfio_t	*f;
reg int		flags;
reg int		set;
#endif
{
	reg int	oflags;

	if(flags == 0)
		return (f->flags&SFIO_FLAGS);

	if((oflags = (f->mode&SF_RDWR)) != f->mode && _sfmode(f,oflags,0) < 0)
		return 0;

	SFLOCK(f,0);

	/* at least preserve one rd/wr flag of the right type */
	oflags = f->flags;
	if(!(oflags&SF_BOTH))
		flags &= ~SF_RDWR;

	/* set the flag */
	if(set)
		f->flags |=  (flags&SF_SETS);
	else	f->flags &= ~(flags&SF_SETS);

	/* must have at least one of read/write */
	if(!(f->flags&SF_RDWR))
		f->flags |= (oflags&SF_RDWR);

	if(f->extent < 0)
		f->flags &= ~SF_APPEND;

	/* turn to appropriate mode as necessary */
	if(!(flags &= SF_RDWR) )
		flags = f->flags&SF_RDWR;
	if((flags == SF_WRITE && !(f->mode&SF_WRITE)) ||
	   (flags == SF_READ && !(f->mode&(SF_READ|SF_SYNCED))) )
		(void)_sfmode(f,flags,1);

	/* if not shared or unseekable, public means nothing */
	if(!(f->flags&SF_SHARE) || f->extent < 0)
		f->flags &= ~SF_PUBLIC;

	SFOPEN(f,0);
	return (oflags&SFIO_FLAGS);
}
