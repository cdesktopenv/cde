/* $XConsortium: sfungetc.c /main/3 1995/11/01 18:38:15 rswiston $ */
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

/*	Push back one byte to a given SF_READ stream
**
**	Written by Kiem-Phong Vo (03/02/91)
*/
#if __STD_C
static _uexcept(reg Sfio_t* f, reg int type, reg Sfdisc_t* disc)
#else
static _uexcept(f,type,disc)
reg Sfio_t	*f;
reg int		type;
reg Sfdisc_t	*disc;
#endif
{	
	/* hmm! This should never happen */
	if(disc != _Sfudisc)
		return -1;

	/* close the unget stream */
	if(type != SF_CLOSE)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	return 1;
}

#if __STD_C
sfungetc(reg Sfio_t* f, reg int c)
#else
sfungetc(f,c)
reg Sfio_t	*f;	/* push back one byte to this stream */
reg int		c;	/* the value to be pushed back */
#endif
{
	reg Sfio_t	*uf;

	if(c < 0 || (f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0))
		return -1;
	SFLOCK(f,0);

	/* fast handling of the typical unget */
	if(f->next > f->data && f->next[-1] == (uchar)c)
	{	f->next -= 1;
		goto done;
	}

	/* make a string stream for unget characters */
	if(f->disc != _Sfudisc)
	{	if(!(uf = sfnew(NIL(Sfio_t*),NIL(char*),-1,-1,SF_STRING|SF_READ)))
		{	c = -1;
			goto done;
		}
		_Sfudisc->exceptf = _uexcept;
		sfdisc(uf,_Sfudisc);
		SFOPEN(f,0); (void)sfstack(f,uf); SFLOCK(f,0);
	}

	/* space for data */
	if(f->next == f->data)
	{	reg uchar*	data;
		if(f->size < 0)
			f->size = 0;
		if(!(data = (uchar*)malloc(f->size+16)))
		{	c = -1;
			goto done;
		}
		f->flags |= SF_MALLOC;
		if(f->data)
			memcpy((char*)(data+16),(char*)f->data,f->size);
		f->size += 16;
		f->data  = data;
		f->next  = data+16;
		f->endb  = data+f->size;
	}

	*--f->next = (uchar)c;
done:
	SFOPEN(f,0);
	return c;
}
