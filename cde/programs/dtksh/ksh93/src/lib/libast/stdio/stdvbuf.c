/* $XConsortium: stdvbuf.c /main/3 1995/11/01 18:40:31 rswiston $ */
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
#include	"stdio.h"

/*	Stdio function setvbuf()
**
**	Written by Kiem-Phong Vo (12/10/90)
*/

#if __STD_C
_stdsetvbuf(Sfio_t* f, char *buf, int type, int size)
#else
_stdsetvbuf(f,buf,type,size)
Sfio_t	*f;
char	*buf;
int	type;
int	size;
#endif
{
	if(type == _IOLBF)
		sfset(f,SF_LINE,1);
	else if((f->flags&SF_STRING))
		return -1;
	else if(type == _IONBF)
	{	sfsync(f);
		sfsetbuf(f,NIL(Void_t*),0);
	}
	else if(type == _IOFBF)
	{	if(size == 0)
			size = SF_BUFSIZE;
		sfsync(f);
		sfsetbuf(f,(Void_t*)buf,size);
	}

	return 0;
}
