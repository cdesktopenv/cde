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
/* $XConsortium: sfsk.c /main/3 1995/11/01 18:36:35 rswiston $ */
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

/*	Seek function that knows discipline
**
**	Written by Kiem-Phong Vo (02/12/91)
*/
#if __STD_C
long sfsk(reg Sfio_t* f, reg long addr, reg int type, reg Sfdisc_t* disc)
#else
long sfsk(f,addr,type,disc)
reg Sfio_t	*f;
reg long	addr;
reg int		type;
reg Sfdisc_t	*disc;
#endif
{
	reg long	p;
	reg int		s, local, string;

	GETLOCAL(f,local);
	if(!local && !(f->mode&SF_LOCK))
		return -1;

	if(!(string = (f->flags&SF_STRING)))
		SFDISC(f,disc,seekf,local);

	for(;;)
	{
		if(string)
		{	SFSTRSIZE(f);
			if(type == 0)
				s = (int)addr;
			else if(type == 1)
				s = (int)(addr + f->here);
			else	s = (int)(addr + f->extent);
		}
		else
		{
			if(disc && disc->seekf)
				p = (*(disc->seekf))(f,addr,type,disc);
			else	p = lseek(f->file,addr,type);
			if(p >= 0)
				return p;
			s = -1;
		}

		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_SEEK,s,disc))
		{
		case SF_EDISC:
		case SF_ECONT:
			if(string)
				return 0L;
			continue;
		default:
			return -1L;
		}
	}
}
