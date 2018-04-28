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
/* $XConsortium: sfswap.c /main/3 1995/11/01 18:37:12 rswiston $ */
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

/*	Swap two streams. If the second argument is NULL,
**	a new stream will be created. Always return the second argument
**	or the new stream. Note that this function will always work
**	unless streams are locked by SF_PUSH.
**
**	Written by Kiem-Phong Vo (02/07/94)
*/

#if __STD_C
Sfio_t* sfswap(reg Sfio_t* f1, reg Sfio_t* f2)
#else
Sfio_t* sfswap(f1,f2)
reg Sfio_t*	f1;
reg Sfio_t*	f2;
#endif
{
	Sfio_t	tmp;
	int	f1pool, f2pool, f1mode, f2mode, f1flags, f2flags;

	if(!f1 || (f1->mode&SF_AVAIL) || (SFFROZEN(f1) && (f1->mode&SF_PUSH)) )
		return NIL(Sfio_t*);
	if(f2 && SFFROZEN(f2) && (f2->mode&SF_PUSH) )
		return NIL(Sfio_t*);
	if(f1 == f2)
		return f2;

	f1mode = f1->mode;
	SFLOCK(f1,0);
	f1->mode |= SF_PUSH;		/* make sure there is no recursion on f1 */
	
	if(f2)
	{	f2mode = f2->mode;
		SFLOCK(f2,0);
		f2->mode |= SF_PUSH;	/* make sure there is no recursion on f2 */
	}
	else
	{	f2 = f1->file == 0 ? sfstdin :
		     f1->file == 1 ? sfstdout :
		     f1->file == 2 ? sfstderr : NIL(Sfio_t*);
		if((!f2 || !(f2->mode&SF_AVAIL)) )
		{	if(!SFALLOC(f2) )
			{	f1->mode = f1mode;
				SFOPEN(f1,0);
				return NIL(Sfio_t*);
			}
			else	SFCLEAR(f2);
		}
		f2->mode = SF_AVAIL|SF_LOCK;
		f2mode = SF_AVAIL;
	}

	if(!f1->pool)
		f1pool = -1;
	else for(f1pool = f1->pool->n_sf-1; f1pool >= 0; --f1pool)
		if(f1->pool->sf[f1pool] == f1)
			break;
	if(!f2->pool)
		f2pool = -1;
	else for(f2pool = f2->pool->n_sf-1; f2pool >= 0; --f2pool)
		if(f2->pool->sf[f2pool] == f2)
			break;

	f1flags = f1->flags;
	f2flags = f2->flags;

	/* swap image and pool entries */
	memcpy((Void_t*)(&tmp),(Void_t*)f1,sizeof(Sfio_t));
	memcpy((Void_t*)f1,(Void_t*)f2,sizeof(Sfio_t));
	memcpy((Void_t*)f2,(Void_t*)(&tmp),sizeof(Sfio_t));
	if(f2pool >= 0)
		f1->pool->sf[f2pool] = f1;
	if(f1pool >= 0)
		f2->pool->sf[f1pool] = f2;

	_sfswap(f1,f2,0);

	if(f2flags&SF_STATIC)
		f2->flags |= SF_STATIC;
	else	f2->flags &= ~SF_STATIC;

	if(f1flags&SF_STATIC)
		f1->flags |= SF_STATIC;
	else	f1->flags &= ~SF_STATIC;

	if(f2mode&SF_AVAIL)	/* swapping to a closed stream */
	{	if(!(f1->flags&SF_STATIC) )
			SFFREE(f1);
	}
	else
	{	f1->mode = f2mode;
		SFOPEN(f1,0);
	}

	f2->mode = f1mode;
	SFOPEN(f2,0);
	return f2;
}
