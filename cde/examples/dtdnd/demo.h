/* $TOG: demo.h /main/4 1999/07/20 14:49:21 mgreess $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         demo.h
 **
 **   Description:  Header for foundation of CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * Public General-Purpose Demo Function Declarations
 */

extern void	demoDragFinishCallback(Widget, XtPointer, XtPointer);
extern void	demoDragMotionHandler(Widget, XtPointer, XtPointer, XEvent*);
extern void	demoProcessPress(Widget, XEvent*, String*, Cardinal*);
extern void	demoDrawExposeCallback(Widget, XtPointer, XtPointer);

extern XtAppContext	demoAppContext;
extern Widget		demoTopLevel;
