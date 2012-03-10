/* $XConsortium: _sfulen.c /main/3 1995/11/01 18:26:59 rswiston $ */
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
static __sfulen(reg ulong v)
#else
static __sfulen(v)
reg ulong	v;
#endif
{
	return sfulen(v);
}

#undef sfulen

#if __STD_C
sfulen(reg ulong v)
#else
sfulen(v)
reg ulong	v;
#endif
{
	return __sfulen(v);
}
