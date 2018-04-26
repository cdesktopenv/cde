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
/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: Square.h /main/3 1995/10/27 10:42:37 rswiston $ */

/*****************************************************************************
*
*  Square.h - widget public header file
*
******************************************************************************/

#ifndef _Square_h
#define _Square_h

#include <Xm/BulletinB.h>

externalref WidgetClass squareWidgetClass;

typedef struct _SquareClassRec *SquareWidgetClass;
typedef struct _SquareRec *SquareWidget;

extern Widget SquareCreate();
extern int SquareMrmInitialize();

#define IsSquare(w) XtIsSubclass((w), squareWidgetClass)

#define SquareWIDTH 0
#define SquareHEIGHT 1

#define SquareNmajorDimension "majorDimension"
#define SquareCMajorDimension "MajorDimension"

#define SquareNmakeSquare "makeSquare"
#define SquareCMakeSquare "MakeSquare"

#endif /* _Square_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
