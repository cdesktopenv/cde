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
