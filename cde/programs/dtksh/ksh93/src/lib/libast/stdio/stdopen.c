/* $XConsortium: stdopen.c /main/3 1995/11/01 18:39:42 rswiston $ */
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

/*	Open a stream given a file descriptor.
**
**	Written by Kiem-Phong Vo (06/27/90).
*/

#if __STD_C
Sfio_t *_stdopen(reg int fd, reg const char* mode)
#else
Sfio_t *_stdopen(fd,mode)
reg int		fd;
reg char	*mode;
#endif
{
	int		sflags;

	if(fd < 0 || (sflags = _sftype(mode,NIL(int*))) == 0)
		return NIL(Sfio_t*);
	else	return sfnew(NIL(Sfio_t*),NIL(Void_t*),-1,fd,sflags);
}
