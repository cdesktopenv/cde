/* $XConsortium: sfnotify.c /main/3 1995/11/01 18:31:35 rswiston $ */
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


/*	Set the function to be called when a stream is opened or closed
**
**	Written by Kiem-Phong Vo (01/06/91)
*/
#if __STD_C
sfnotify(void (*notify)(Sfio_t*, int, int))
#else
sfnotify(notify)
void	(*notify)();
#endif
{
	_Sfnotify = notify;
	return 0;
}
