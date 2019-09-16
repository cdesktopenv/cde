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
/* $XConsortium: sfpurge.c /main/3 1995/11/01 18:33:41 rswiston $ */
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

/*	Delete all pending data in the buffer
**
**	Written by Kiem-Phong Vo (07/08/91)
*/

#if __STD_C
int sfpurge(reg Sfio_t* f)
#else
sfpurge(f)
reg Sfio_t	*f;
#endif
{
	reg int	mode;

	if((mode = f->mode&SF_RDWR) != f->mode && _sfmode(f,mode,0) < 0)
		return -1;

	if(f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfile_t*)));

	/* cannot purge read string streams */
	if((f->flags&SF_STRING) && (f->mode&SF_READ) )
		goto done;

	SFLOCK(f,0);

	/* if memory map must be a read stream, pretend data is gone */
#ifdef MAP_TYPE
	if(f->flags&SF_MMAP)
	{	f->here -= f->endb - f->next;
		if(f->data)
		{	(void)munmap((caddr_t)f->data,f->endb-f->data);
			SFSK(f,f->here,0,f->disc);
			f->endb = f->endr = f->endw =
			f->next = f->data = NIL(uchar*);
		}

		SFOPEN(f,0);
		return 0;
	}
#endif

	switch(f->mode&~SF_LOCK)
	{
	default :
		return -1;
	case SF_WRITE :
		f->next = f->data;
		if((f->flags&(SF_PROCESS|SF_RDWR)) != (SF_PROCESS|SF_RDWR))
			break;

		/* 2-way pipe, must clear read buffer */
		(void)_sfmode(f,SF_READ,1);
		/* fall through */
	case SF_READ:
		if(f->extent >= 0 && f->endb > f->next)
		{	f->here -= f->endb-f->next;
			SFSK(f,f->here,0,f->disc);
		}
		f->endb = f->next = f->data;
		break;
	}

	SFOPEN(f,0);

done:
	if((f->flags&SF_IOCHECK) && f->disc && f->disc->exceptf)
		(void)(*f->disc->exceptf)(f,SF_PURGE,f->disc);

	return 0;
}
