/* $XConsortium: _sfgetu.c /main/3 1995/11/01 18:25:17 rswiston $ */
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
static ulong __sfgetu(reg Sfio_t* f)
#else
static ulong __sfgetu(f)
reg Sfio_t	*f;
#endif
{
	return sfgetu(f);
}

#undef sfgetu

#if __STD_C
ulong sfgetu(reg Sfio_t* f)
#else
ulong sfgetu(f)
reg Sfio_t	*f;
#endif
{
	return __sfgetu(f);
}
