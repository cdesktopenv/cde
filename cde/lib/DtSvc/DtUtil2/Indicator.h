/* $XConsortium: Indicator.h /main/4 1995/10/26 15:23:41 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
********************************************************************************
*
* File:         Indicator.h
* Description:  Public header for Activity Indicator
*
* (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
*
********************************************************************************
*/

#ifndef _Indicator_h
#define _Indicator_h

extern void _DtSendActivityNotification( int ) ;
   /* int duration;        Maximum activation time for the indicator */

/*
 * _DtSendActivityNotification() provides the application with the means for
 * notifying the world that an activity has been started, and may take upto
 * 'duration' seconds.  For now, the workspace manager will enable the
 * activity indicator for upto the indicated duration of time; the time is 
 * in units of seconds.
 */

extern void _DtSendActivityDoneNotification( void ) ;

/*
 * _DtSendActivityDoneNotification() provides the application with the means for
 * notifying the world that an activity which had earlier been started, is
 * now complete.
 */

#endif /* _Indicator_h */
/* DON'T ADD STUFF AFTER THIS #endif */
