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
/* $XConsortium: vmdcsbrk.c /main/2 1996/05/08 20:02:18 drk $ */
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

/*	A discipline to get memory using sbrk().
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 01/16/94.
*/
#if __STD_C
static Void_t* sbrkmem(Vmalloc_t* vm, Void_t* caddr, size_t csize, size_t nsize,
			Vmdisc_t* disc)
#else
static Void_t* sbrkmem(vm, caddr, csize, nsize, disc)
Vmalloc_t*	vm;	/* region doing allocation from		*/
Void_t*		caddr;	/* current address			*/
size_t		csize;	/* current size				*/
size_t		nsize;	/* new size				*/
Vmdisc_t*	disc;	/* discipline structure			*/
#endif
{
	reg uchar*	addr;
	reg int		size;
	NOTUSED(vm);
	NOTUSED(disc);

	/* manipulating an existing segment, see if still own current address */
	if(csize > 0 && sbrk(0) != (uchar*)caddr+csize)
		return NIL(Void_t*);

	/* do this because sbrk() uses 'int' argument */
	if(nsize > csize)
		size =  (int)(nsize-csize);
	else	size = -(int)(csize-nsize);

	if((addr = sbrk(size)) == (uchar*)(-1))
		return NIL(Void_t*);
	else	return csize == 0 ? (Void_t*)addr : caddr;
}


Vmdisc_t _Vmdcsbrk = { sbrkmem, NIL(Vmexcept_f), 0 };
