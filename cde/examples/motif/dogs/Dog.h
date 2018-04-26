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
/*   $XConsortium: Dog.h /main/3 1995/10/27 10:42:01 rswiston $ */

/*****************************************************************************
*
*  Dog.h - widget public header file
*  
******************************************************************************/

#ifndef _Dog_h
#define _Dog_h

externalref WidgetClass dogWidgetClass;

typedef struct _DogClassRec *DogWidgetClass;
typedef struct _DogRec *DogWidget;

#define DogNbarkCallback "barkCallback"
#define DogNwagTime "wagTime"
#define DogNbarkTime "barkTime"

#define DogCWagTime "WagTime"
#define DogCBarkTime "BarkTime"

#define IsDog(w) XtIsSubclass((w), dogWidgetClass)

extern Widget DogCreate();
extern int DogMrmInitialize();

#endif /* _Dog_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
