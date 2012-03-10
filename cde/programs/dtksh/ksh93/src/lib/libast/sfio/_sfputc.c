/* $XConsortium: _sfputc.c /main/3 1995/11/01 18:25:42 rswiston $ */
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

#if __STD_C
static __sfputc(reg Sfio_t* f, reg int c)
#else
static __sfputc(f,c)
reg Sfio_t	*f;
reg int		c;
#endif
{
	return sfputc(f,c);
}

#undef sfputc

#if __STD_C
sfputc(reg Sfio_t* f, reg int c)
#else
sfputc(f,c)
reg Sfio_t	*f;
reg int		c;
#endif
{
	return __sfputc(f,c);
}
