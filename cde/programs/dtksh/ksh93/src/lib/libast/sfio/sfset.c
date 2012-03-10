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
sfset(reg Sfio_t* f, reg int flags, reg int set)
#else
sfset(f,flags,set)
reg Sfio_t	*f;
reg int		flags;
reg int		set;
#endif
{
	reg int	oflags;

	if(flags == 0)
		return (f->flags&SF_FLAGS);

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
	return (oflags&SF_FLAGS);
}
