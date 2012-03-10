/* $XConsortium: dtkcvt.h /main/5 1996/03/13 13:19:27 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        dtkcvt.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for dtkcvt.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_dtkcvt_h
#define _Dtksh_dtkcvt_h


extern void DtkshCvtWindowToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtScreenToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtStringToScreen( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtStringToTopItemPosition( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtHexIntToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtIntToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtBooleanToString( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtStringToPointer( 
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval) ;
extern void DtkshCvtStringToWidget( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToCallback( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtCallbackToString( 
                        Display *display,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer converterData) ;
extern void DtkshCvtStringToPrintSetupProc( 
                        Display *display,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data);
extern void DtkshCvtWidgetToString( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToEventMask( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToListItems( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToWidgetClass( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtWidgetClassToString( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToMWMDecoration( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtMWMDecorationToString( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToMWMFunctions( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtMWMFunctionsToString( 
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtStringToPanedWinPosIndex(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;
extern void DtkshCvtPanedWinPosIndexToString(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *nargs,
                        XrmValuePtr fval,
                        XrmValuePtr toval,
                        XtPointer data) ;



#endif /* _Dtksh_dtkcvt_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
