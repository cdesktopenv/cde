/* $XConsortium: _sfeof.c /main/3 1995/11/01 18:24:01 rswiston $ */
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
static __sfeof(reg Sfio_t* f)
#else
static __sfeof(f)
reg Sfio_t	*f;
#endif
{
	return sfeof(f);
}

#undef sfeof

#if __STD_C
sfeof(reg Sfio_t* f)
#else
sfeof(f)
reg Sfio_t	*f;
#endif
{
	return __sfeof(f);
}
