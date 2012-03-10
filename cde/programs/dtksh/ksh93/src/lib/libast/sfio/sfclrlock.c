/* $XConsortium: sfclrlock.c /main/3 1995/11/01 18:27:23 rswiston $ */
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

/*	Function to clear a locked stream.
**	This is useful for programs that longjmp from the mid of an sfio function.
**	There is no guarantee on data integrity in such a case.
**
**	Written by Kiem-Phong Vo (07/20/90).
*/
#if __STD_C
sfclrlock(reg Sfio_t* f)
#else
sfclrlock(f)
reg Sfio_t	*f;
#endif
{
	/* already closed */
	if(f->mode&SF_AVAIL)
		return 0;

	/* clear these bits */
	f->flags &= ~(SF_ERROR|SF_EOF);

	if(!(f->mode&(SF_LOCK|SF_PEEK)) )
		return (f->flags&SF_FLAGS);

	/* clear peek locks */
	f->mode &= ~SF_PEEK;
	if(f->mode&SF_PKRD)
	{	f->here -= f->endb-f->next;
		f->endb = f->next;
		f->mode &= ~SF_PKRD;
	}

	f->mode &= (SF_RDWR|SF_INIT|SF_POOL|SF_PUSH|SF_SYNCED|SF_STDIO);

	return _sfmode(f,0,0) < 0 ? 0 : (f->flags&SF_FLAGS);
}
