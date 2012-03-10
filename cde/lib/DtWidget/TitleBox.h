/* $XConsortium: TitleBox.h /main/4 1995/10/26 09:34:50 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		TitleBox.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtTitleBox class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtTitleBox_h
#define _DtTitleBox_h

#include <Xm/Xm.h>
#include <Dt/DtStrDefs.h>

#ifndef DtIsTitleBox
#define DtIsTitleBox(w) XtIsSubclass(w, DtTitleBoxClass)
#endif /* DtIsTitleBox */


/********    Public Function Declarations    ********/

extern Widget _DtCreateTitleBox( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget _DtTitleBoxGetTitleArea( 
                        Widget w) ;
extern Widget _DtTitleBoxGetWorkArea( 
                        Widget w) ;


extern WidgetClass	dtTitleBoxWidgetClass;

typedef struct _DtTitleBoxClassRec * DtTitleBoxWidgetClass;
typedef struct _DtTitleBoxRec      * DtTitleBoxWidget;


#define XmTITLE_TOP	0
#define XmTITLE_BOTTOM	1

#define XmTITLE_AREA	1


#endif /* _DtTitleBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
