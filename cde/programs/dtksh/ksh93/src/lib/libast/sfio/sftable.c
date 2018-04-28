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
/* $XConsortium: sftable.c /main/3 1995/11/01 18:37:37 rswiston $ */
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

Sftab_t	_Sftable =
{
	{ 1e1, 1e2, 1e4, 1e8, 1e16, 1e32 },
	{ 1e-1, 1e-2, 1e-4, 1e-8, 1e-16, 1e-32 },
	{ '0','0', '0','1', '0','2', '0','3', '0','4',
	  '0','5', '0','6', '0','7', '0','8', '0','9',
	  '1','0', '1','1', '1','2', '1','3', '1','4',
	  '1','5', '1','6', '1','7', '1','8', '1','9',
	  '2','0', '2','1', '2','2', '2','3', '2','4',
	  '2','5', '2','6', '2','7', '2','8', '2','9',
	  '3','0', '3','1', '3','2', '3','3', '3','4',
	  '3','5', '3','6', '3','7', '3','8', '3','9',
	  '4','0', '4','1', '4','2', '4','3', '4','4',
	  '4','5', '4','6', '4','7', '4','8', '4','9',
	  '5','0', '5','1', '5','2', '5','3', '5','4',
	  '5','5', '5','6', '5','7', '5','8', '5','9',
	  '6','0', '6','1', '6','2', '6','3', '6','4',
	  '6','5', '6','6', '6','7', '6','8', '6','9',
	  '7','0', '7','1', '7','2', '7','3', '7','4',
	  '7','5', '7','6', '7','7', '7','8', '7','9',
	  '8','0', '8','1', '8','2', '8','3', '8','4',
	  '8','5', '8','6', '8','7', '8','8', '8','9',
	  '9','0', '9','1', '9','2', '9','3', '9','4',
	  '9','5', '9','6', '9','7', '9','8', '9','9',
	},
	{	64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 63, 64, 64,
		 0,  1,  2,  3,  4,  5,  6,  7,
		 8,  9, 64, 64, 64, 64, 64, 64,
		62, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24,
		25, 26, 27, 28, 29, 30, 31, 32,
		33, 34, 35, 64, 64, 64, 64, 64,
		64, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24,
		25, 26, 27, 28, 29, 30, 31, 32,
		33, 34, 35, 64, 64, 64, 64, 64
	},
	{	64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 63, 64, 64,
		 0,  1,  2,  3,  4,  5,  6,  7,
		 8,  9, 64, 64, 64, 64, 64, 64,
		62, 36, 37, 38, 39, 40, 41, 42,
		43, 44, 45, 46, 47, 48, 49, 50,
		51, 52, 53, 54, 55, 56, 57, 58,
		59, 60, 61, 64, 64, 64, 64, 64,
		64, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24,
		25, 26, 27, 28, 29, 30, 31, 32,
		33, 34, 35, 64, 64, 64, 64, 64
	},
	"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLNMOPQRSTUVWXYZ@_",
};
