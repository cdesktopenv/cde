/* $XConsortium: sfstr.h /main/3 1995/11/01 17:39:35 rswiston $ */
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
/*
 * macro interface for sfio write strings
 *
 * NOTE: see <stak.h> for an alternative interface
 *	 read operations require sfseek()
 */

#ifndef _SFSTR_H
#define _SFSTR_H

#include <sfio.h>

#define sfstropen()	sfnew((Sfile_t*)0,(char*)0,-1,-1,SF_WRITE|SF_STRING)
#define sfstrnew(m)	sfnew((Sfile_t*)0,(char*)0,-1,-1,(m)|SF_STRING)
#define sfstrclose(f)	sfclose(f)

#define sfstrtell(f)	((f)->next - (f)->data)
#define sfstrrel(f,p)	((p) == (0) ? (char*)(f)->next : \
			 ((f)->next += (p), \
			  ((f)->next >= (f)->data && (f)->next  <= (f)->endb) ? \
				(char*)(f)->next : ((f)->next -= (p), (char*)0) ) )

#define sfstrset(f,p)	(((p) >= 0 && (p) <= (f)->size) ? \
				(char*)((f)->next = (f)->data+(p)) : (char*)0 )

#define sfstrbase(f)	((char*)(f)->data)
#define sfstrsize(f)	((f)->size)

#define sfstrrsrv(f,n)	(sfreserve(f,(long)(n),1)?(sfwrite(f,(char*)(f)->next,0),(char*)(f)->next):(char*)0)

#define sfstruse(f)	(sfputc(f,0), (char*)((f)->next = (f)->data) )

#endif
