/* $XConsortium: SharedProcs.h /main/4 1995/10/26 15:28:07 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SharedProcs.h
 **
 **   Project:     SUI
 **
 **   Description: Public include file for some shared functions.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _SharedProcs_h
#define _SharedProcs_h


/********    Public Function Declarations    ********/

extern String _DtStripSpaces( 
                        String string) ;
extern void _DtMessage(
                        Widget w,
                        char *title,
                        char *message_text,
                        XtPointer helpIdStr,
                        void (*helpCallback)()) ;
extern Widget _DtMessageDialog(
                        Widget w,
                        char *title,
                        char *message_text,
                        XtPointer helpIdStr,
                        Boolean cancel_btn,
                        void (*cancel_callback)(),
                        void (*ok_callback)(),
                        void (*close_callback)(),
                        void (*help_callback)(),
                        Boolean deleteOnClose,
                        int dialogType) ;
extern void _DtMessageOK(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern void _DtMessageClose(
                        Widget w,
                        XtPointer client_data,
                        XEvent *event) ;

/********    End Public Function Declarations    ********/

/* _DtMessage Dialog build defines */
#define ERROR_DIALOG  1
#define WARNING_DIALOG  2
#define QUESTION_DIALOG 3

/* Flag which can be used to prevent error dialogs from being posted */
extern Boolean messageDisplayEnabled;

/* Flag controlling whether dialogs are auto-positioned */
extern Boolean disableDialogAutoPlacement;

/* Generic overlay for all dialog 'Rec' structures */
typedef struct
{
   Widget shell;
} GenericRecord;

#endif /* _SharedProcs_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
