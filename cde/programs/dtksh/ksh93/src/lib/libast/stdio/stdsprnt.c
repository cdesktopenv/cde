/* $XConsortium: stdsprnt.c /main/3 1995/11/01 18:40:19 rswiston $ */
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


/*	sprintf function
**
**	Written by Kiem-Phong Vo (12/10/90)
*/


#if __STD_C
_stdsprintf(char *s, const char *form, ...)
#else
_stdsprintf(va_alist)
va_dcl
#endif
{
	va_list	args;
	Sfio_t	f;
	reg int	rv;

#if __STD_C
	va_start(args,form);
#else
	reg char	*s;
	reg char	*form;
	va_start(args);
	s = va_arg(args,char*);
	form = va_arg(args,char*);
#endif

	if(!s)
		return -1;

	/* make a fake stream */
	SFCLEAR(&f);
	f.flags = SF_STRING|SF_WRITE;
	f.mode = SF_WRITE;
	f.size = 4*SF_BUFSIZE;
	f.data = f.next = f.endr = (uchar*)s;
	f.endb = f.endw = f.data+f.size;

	rv = sfvprintf(&f,form,args);
	*f.next = '\0';
	_Sfi = f.next - f.data;

	va_end(args);

	return rv;
}
