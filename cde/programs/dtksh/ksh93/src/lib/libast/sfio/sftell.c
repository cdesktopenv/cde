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
/* $XConsortium: sftell.c /main/3 1995/11/01 18:37:49 rswiston $ */
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

/*	Tell the current location in a given stream
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
long sftell(reg Sfio_t* f)
#else
long sftell(f)
reg Sfio_t	*f;
#endif
{	
	reg int	mode;

	/* set the stream to the right mode */
	if((mode = f->mode&SF_RDWR) != f->mode && _sfmode(f,mode,0) < 0)
		return -1;

	/* throw away ungetc data */
	if(f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	if(f->flags&SF_STRING)
		return (long)(f->next-f->data);

	/* let sfseek() handle the hard case */
	if(f->extent >= 0 && (f->flags&(SF_SHARE|SF_APPEND)) )
		return sfseek(f,0L,1);

	return f->here + ((f->mode&SF_WRITE) ? f->next-f->data : f->next-f->endb);
}
