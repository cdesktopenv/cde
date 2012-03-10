/* $XConsortium: stdscanf.c /main/3 1995/11/01 18:40:07 rswiston $ */
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

/*	Read formatted data from a stream
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
_stdscanf(const char *form, ...)
#else
_stdscanf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;

#if __STD_C
	va_start(args,form);
#else
	reg char	*form;	/* scanning format */
	va_start(args);
	form = va_arg(args,char*);
#endif

	rv = sfvscanf(sfstdin,form,args);
	va_end(args);
	return rv;
}
