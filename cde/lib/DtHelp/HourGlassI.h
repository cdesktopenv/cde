/* $XConsortium: HourGlassI.h /main/5 1995/10/26 12:27:38 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       HourGlassI.h
 **
 **   Project:     Cache Creek
 **
 **   Description: Internal include file for HourGlass Library.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _hourglassI_h
#define _hourglassI_h

/* DtHelpGetHourGlassCursor -
 *
 *   Builds and returns the appropriate HourGlass cursor.
 */

extern Cursor DtHelpGetHourGlassCursor( 
                        Display *dpy) ;

/* DtHelpTurnOnHourGlass -
 *
 *   Gets and displays an hourglass cursor in the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOnHourGlass( 
                        Widget w) ;
 
 /*  Widget widget;
  *
  * widget is the toplevel shell of the window you want
  * the hourglass cursor to appear in.
  */


/* DtHelpTurnOffHourGlass -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOffHourGlass( 
                        Widget w) ;
  
 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */

/* DtHelpTurnOnNoEnter -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOnNoEnter( 
                        Widget w) ;

 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */

/* DtHelpTurnOffNoEnter -
 *
 *   Removes the hourglass cursor from the window of the widget
 *   which is passed in to the funciton.
 */

extern void _DtHelpTurnOffNoEnter( 
                        Widget w) ;

 /* Widget widget;
  *
  *  widget is the toplevel shell of the window you want
  *  to remove hourglass cursor from.
  */


#endif /* _hourglassI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
