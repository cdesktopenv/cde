/* $XConsortium: _sffileno.c /main/3 1995/11/01 18:24:40 rswiston $ */
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
static __sffileno(reg Sfio_t* f)
#else
static __sffileno(f)
reg Sfio_t	*f;
#endif
{
	return sffileno(f);
}

#undef sffileno

#if __STD_C
sffileno(reg Sfio_t* f)
#else
sffileno(f)
reg Sfio_t	*f;
#endif
{
	return __sffileno(f);
}
