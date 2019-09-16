/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: sfputr.c /main/3 1995/11/01 18:34:20 rswiston $ */
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

/*	Put out a nul-terminated string
**	Note that the reg declarations below must be kept in
**	their relative order so that the code will configured
**	correctly on Vaxes to use "asm()".
**
**	Written by Kiem-Phong Vo
*/
#if __STD_C
int sfputr(reg Sfio_t* f, const char* s, reg int rc)
#else
sfputr(f,s,rc)
reg Sfio_t*	f;	/* write to this stream. r11 on Vax	*/
char*		s;	/* string to write			*/
reg int		rc;	/* record separator. r10 on Vax		*/
#endif
{
	reg int		p;		/* r9 on Vax		*/
	reg uchar	*os, *ps;	/* r8, r7 on Vax	*/
	reg int		n;		/* r6 on Vax		*/

	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;

	SFLOCK(f,0);
	os = (uchar*)s;
	if(f->size <= 0)
	{	/* unbuffered stream */
		n = strlen((char*)os);
		if((p = SFWRITE(f,(Void_t*)os,n)) > 0)
			os += p;
		goto done;
	}

	while(*os)
	{	/* peek buffer for space */
		if(SFWPEEK(f,ps,p) <= 0)
			break;

#if _vax_asm	/* p is r9, os is r8, and ps is r7 */
		0;					/* avoid if() branching bug */
		asm( "locc	$0,r9,(r8)" );		/* look for the \0 */
		asm( "subl2	r0,r9" );		/* length of data to copy */
		asm( "movc3	r9,(r8),(r7)" );	/* copy data */
		ps += p;
		os += p;
#else
#if _lib_memccpy
		if((ps = (uchar*)memccpy(ps,os,'\0',p)) != NIL(uchar*))
			ps -= 1;
		else	ps  = f->next+p;
		os += ps - f->next;
#else
		/* fast copy loop */
		while((*ps++ = *os++) != '\0' && --p > 0)
			;
		if(*--ps != 0)
			ps += 1;
		else	os -= 1;
#endif
#endif
		f->next = ps;
	}

done:
	p = (char*)os - (char*)s;
	if(rc >= 0)
	{	if(f->next >= f->endb)
			(void)SFFLSBUF(f,(int)((uchar)rc));
		else	*f->next++ = (uchar)rc;
		p += 1;
	}

	/* sync unseekable shared streams */
	if(f->extent < 0 && (f->flags&SF_SHARE) )
		(void)SFFLSBUF(f,-1);

	/* check for line buffering */
	else if((f->flags&SF_LINE) && !(f->flags&SF_STRING) && (n = f->next-f->data) > 0)
	{	if(n > p)
			n = p;
		f->next -= n;
		(void)SFWRITE(f,(Void_t*)f->next,n);
	}

	SFOPEN(f,0);

	return p;
}
