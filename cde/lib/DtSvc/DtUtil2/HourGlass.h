/* $XConsortium: HourGlass.h /main/4 1995/10/26 15:23:08 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       HourGlass.h
 **
 **   Project:     DT
 **
 **   Description: Public include file for HourGlass Library.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _hourglass_h
#define _hourglass_h

/* _DtGetHourGlassCursor -
 *
 *   Builds and returns the appropriate HourGlass cursor.
 */

extern Cursor _DtGetHourGlassCursor( 
                        Display *dpy) ;

/* _DtTurnOnHourGlass -
 *
 *   Gets and displays an hourglass cursor in the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtTurnOnHourGlass( 
                        Widget w) ;
 
 /*  Widget widget;
  *
  * widget is the toplevel shell of the window you want
  * the hourglass cursor to appear in.
  */


/* _DtTurnOffHourGlass -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtTurnOffHourGlass( 
                        Widget w) ;
  
 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */


#endif /* _hourglass_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */



