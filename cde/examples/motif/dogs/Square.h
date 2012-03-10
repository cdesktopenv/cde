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
