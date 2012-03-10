/* $XConsortium: DialogBox.h /main/4 1995/10/26 09:30:36 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		DialogBox.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtDialogBox class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtDialogBox_h
#define _DtDialogBox_h

#include <Xm/Xm.h>
#include <Dt/DtStrDefs.h>

#ifndef DtIsDialogBox
#define DtIsDialogBox(w) XtIsSubclass(w, dtDialogBoxWidgetClass)
#endif /* XmIsDialogBox */


extern Widget _DtCreateDialogBox( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget __DtCreateDialogBoxDialog( 
                        Widget ds_p,
                        String name,
                        ArgList db_args,
                        Cardinal db_n) ;
extern Widget _DtDialogBoxGetButton( 
                        Widget w,
                        Cardinal pos) ;
extern Widget _DtDialogBoxGetWorkArea( 
                        Widget w) ;


extern WidgetClass dtDialogBoxWidgetClass;

typedef struct _DtDialogBoxClassRec * DtDialogBoxWidgetClass;
typedef struct _DtDialogBoxRec      * DtDialogBoxWidget;


#define XmBUTTON	11


#define XmCR_DIALOG_BUTTON	100

typedef struct
{
	int		reason;
	XEvent *	event;
	Cardinal	button_position;
	Widget		button;
} DtDialogBoxCallbackStruct;


#endif /* _DtDialogBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
