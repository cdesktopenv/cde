/* $XConsortium: _sfputl.c /main/3 1995/11/01 18:26:09 rswiston $ */
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
static __sfputl(reg Sfio_t* f, reg long v)
#else
static __sfputl(f,v)
reg Sfio_t	*f;
reg long	v;
#endif
{
	return sfputl(f,v);
}

#undef sfputl

#if __STD_C
sfputl(reg Sfio_t* f, reg long v)
#else
sfputl(f,v)
reg Sfio_t	*f;
reg long	v;
#endif
{
	return __sfputl(f,v);
}
