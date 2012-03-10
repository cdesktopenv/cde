/* $XConsortium: sfputu.c /main/3 1995/11/01 18:34:32 rswiston $ */
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

/*	Write out an unsigned long value in a portable format.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
_sfputu(reg Sfio_t* f, reg ulong v)
#else
_sfputu(f,v)
reg Sfio_t	*f;	/* write a portable ulong to this stream */
reg ulong	v;	/* the unsigned value to be written */
#endif
{
#define N_ARRAY		(2*sizeof(long))
	reg uchar	*s, *ps;
	reg int		n, p;
	uchar		c[N_ARRAY];

	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;
	SFLOCK(f,0);

	/* code v as integers in base SF_UBASE */
	s = ps = &(c[N_ARRAY-1]);
	*s = (uchar)SFUVALUE(v);
	while((v >>= SF_UBITS) )
		*--s = (uchar)(SFUVALUE(v) | SF_MORE);
	n = (ps-s)+1;

	if(n > 4 || SFWPEEK(f,ps,p) < n)
		n = SFWRITE(f,(Void_t*)s,n); /* write the hard way */
	else
	{	switch(n)
		{
		case 4 : *ps++ = *s++;
		case 3 : *ps++ = *s++;
		case 2 : *ps++ = *s++;
		case 1 : *ps++ = *s++;
		}
		f->next = ps;
	}

	SFOPEN(f,0);
	return n;
}
