/* $XConsortium: vmregion.c /main/2 1996/05/08 20:05:00 drk $ */
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
#include	"vmhdr.h"

/*	Return the containing region of an allocated piece of memory.
**	Beware: this only works with Vmbest and Vmtrace.
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/
#if __STD_C
Vmalloc_t* vmregion(reg Void_t* addr)
#else
Vmalloc_t* vmregion(addr)
reg Void_t*	addr;
#endif
{	return addr ? VM(BLOCK(addr)) : NIL(Vmalloc_t*);
}
