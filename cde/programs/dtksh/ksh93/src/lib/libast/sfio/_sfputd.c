/* $XConsortium: _sfputd.c /main/3 1995/11/01 18:25:57 rswiston $ */
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
static __sfputd(reg Sfio_t* f, reg double v)
#else
static __sfputd(f,v)
reg Sfio_t	*f;
reg double	v;
#endif
{
	return sfputd(f,v);
}

#undef sfputd

#if __STD_C
sfputd(reg Sfio_t* f, reg double v)
#else
sfputd(f,v)
reg Sfio_t	*f;
reg double	v;
#endif
{
	return __sfputd(f,v);
}
