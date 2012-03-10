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
