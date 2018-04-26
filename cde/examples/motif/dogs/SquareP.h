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
/*   $XConsortium: SquareP.h /main/3 1995/10/27 10:42:55 rswiston $ */


#ifndef _SquareP_h
#define _SquareP_h

#include "Square.h"
#include <Xm/BulletinBP.h>

#define SquareIndex (XmBulletinBIndex + 1)

typedef struct _SquareClassPart
{
   int reserved;
} SquareClassPart;


typedef struct _SquareClassRec
{
   CoreClassPart       core_class;
   CompositeClassPart  composite_class;
   ConstraintClassPart constraint_class;
   XmManagerClassPart  manager_class;
   XmBulletinBoardClassPart  bulletin_board_class;
   SquareClassPart     square_class;
} SquareClassRec;

externalref SquareClassRec squareClassRec;

typedef struct _SquarePart
{
    int major_dimension;
} SquarePart;


/*  Full instance record declaration  */

typedef struct _SquareRec
{
   CorePart	  core;
   CompositePart  composite;
   ConstraintPart constraint;
   XmManagerPart  manager;
   XmBulletinBoardPart  bulletin_board;
   SquarePart     square;
} SquareRec;

typedef struct _SquareConstraintPart
{
   Boolean make_square;
} SquareConstraintPart;

typedef struct _SquareConstraintRec
{
   XmManagerConstraintPart manager;
   SquareConstraintPart    square;
} SquareConstraintRec, *SquareConstraint;


#endif /* _SquareP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
