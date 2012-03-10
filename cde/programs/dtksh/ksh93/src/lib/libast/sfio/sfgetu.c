/* $XConsortium: sfgetu.c /main/3 1995/11/01 18:30:06 rswiston $ */
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

/*	Read an unsigned long value coded in a portable format.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
ulong _sfgetu(reg Sfio_t* f)
#else
ulong _sfgetu(f)
reg Sfio_t	*f;
#endif
{
	reg uchar	*s, *ends, c;
	reg int		p;
	reg ulong	v;

	if(f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0)
		return (ulong)(-1L);

	SFLOCK(f,0);

	v = SFUVALUE(_Sfi);
	for(;;)
	{
		if(SFRPEEK(f,s,p) <= 0)
		{	f->flags |= SF_ERROR;
			v = (ulong)(-1L);
			goto done;
		}
		for(ends = s+p; s < ends;)
		{
			c = *s++;
			v = (v << SF_UBITS) | SFUVALUE(c);
			if(!(c&SF_MORE))
			{
				f->next = s;
				goto done;
			}
		}
		f->next = s;
	}
done:
	SFOPEN(f,0);
	return v;
}
