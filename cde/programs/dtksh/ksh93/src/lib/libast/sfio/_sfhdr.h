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
/* $XConsortium: _sfhdr.h /main/3 1995/11/01 18:25:30 rswiston $ */
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

/*	This header file defines analogues for all public macro functions.
	The analogues are used for building function analogues.
*/

#define _SFPUTC(f,c)	sfputc(f,c)
#define _SFGETC(f)	sfgetc(f)
#define _SFSLEN()	sfslen()
#define _SFFILENO(f)	sffileno(f)
#define _SFEOF(f)	sfeof(f)
#define _SFERROR(f)	sferror(f)
#define _SFCLRERR(f)	sfclrerr(f)
#define _SFSTACKED(f)	sfstacked(f)
#define _SFULEN(v)	sfulen(v)
#define _SFGETU(f)	sfgetu(f)
#define _SFGETL(f)	sfgetl(f)
#define _SFPUTU(f,v)	sfputu(f,v)
#define _SFPUTL(f,v)	sfputl(f,v)
#define _SFPUTD(f,v)	sfputd(f,v)
#define _SFECVT(v,n,d,s) sfecvt(v,n,d,s)
#define _SFFCVT(v,n,d,s) sffcvt(v,n,d,s)

#undef	sfputc
#undef	sfgetc
#undef	sfslen
#undef	sffileno
#undef	sfeof
#undef	sferror
#undef	sfclrerr
#undef	sfstacked
#undef	sfulen
#undef	sfgetu
#undef	sfgetl
#undef	sfputu
#undef	sfputl
#undef	sfputd
#undef	sfecvt
#undef	sffcvt
