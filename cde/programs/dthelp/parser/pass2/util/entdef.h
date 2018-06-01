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
/* $XConsortium: entdef.h /main/3 1995/11/08 11:04:14 rswiston $ */
/* Copyright (c) 1988, 1989 Hewlett-Packard Co. */
/* Entdef.h contains definitions relevant to entities */

#define M_GENERAL 0
#define M_SYSTEM 1
#define M_STARTTAG 2
#define M_ENDTAG 3
#define M_MD 4
#define M_MS 5
#ifdef  M_PI
#undef  M_PI
#endif
#define M_PI 6
#define M_CDATAENT 7
#define M_SDATA 8
#define M_CODEPI 9
#define M_CODESDATA 10

#define M_DBUILD 1
#define M_DELTDEF 2
#define M_DPARSER 3


