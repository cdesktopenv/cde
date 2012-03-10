/* $XConsortium: _sfclrerr.c /main/3 1995/11/01 18:23:35 rswiston $ */
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
static __sfclrerr(reg Sfio_t* f)
#else
static __sfclrerr(f)
reg Sfio_t	*f;
#endif
{
	return sfclrerr(f);
}

#undef sfclrerr

#if __STD_C
sfclrerr(reg Sfio_t* f)
#else
sfclrerr(f)
reg Sfio_t	*f;
#endif
{
	return __sfclrerr(f);
}
