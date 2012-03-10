/* $XConsortium: sfllen.c /main/3 1995/11/01 18:30:46 rswiston $ */
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

/*	Get size of a long value coded in a portable format
**
**	Written by Kiem-Phong Vo (06/27/90)
*/
#if __STD_C
sfllen(reg long v)
#else
sfllen(v)
reg long	v;
#endif
{
	if(v < 0)
		v = -(v+1);
	v = (ulong)v >> SF_SBITS;
	return 1 + (v > 0 ? sfulen(v) : 0);
}
