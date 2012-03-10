/* $XConsortium: SaverP.h /main/4 1995/10/26 15:10:42 rswiston $ */
/*                                                                            *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                           *
 * (c) Copyright 1993, 1994 International Business Machines Corp.             *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                            *
 * (c) Copyright 1993, 1994 Novell, Inc.                                      *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Saver.h
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   managing external screen savers
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _dtsaverp_h
#define _dtsaverp_h

/*************************************<->*************************************
 *
 *  _DtSaverStart()
 *
 *
 *  Description:
 *  -----------
 *  Store provided array of windows on root window property
 *  _DT_SAVER_WINDOWS and launch specified screen saver. Screen savers
 *  will attempt to use these windows as drawables.
 *
 *  Inputs:
 *  ------
 *  display - display structure
 *  drawArea - array of widgets to be drawn upon by screen saver
 *  count - number of elements in drawArea array
 *  saverAction - screen saver action to invoke
 *  wAction - action UI widget
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  -------
 *  state - pointer to opaque state structure
 *
 *  Comments:
 *  --------
 *  This function uses _DtActionInvoke() to launch an action. As a result,
 *  the caller is responsible for loading and maintaining the action database
 *  using the DtDbLoad() function and procedures. The caller
 *  must call _DtSaverStop() to terminate screen saver
 *
 *************************************<->***********************************/

extern void * _DtSaverStart(Display *, Widget *, int, char *, Widget);

/*************************************<->*************************************
 *
 *  _DtSaverStop()
 *
 *
 *  Description:
 *  -----------
 *  Stop an external screen saver started with DtStartSaver(). Deletes
 * _DT_SAVER_WINDOWS property from root window.
 *
 *  Inputs:
 *  ------
 *  display - display structure
 *  state - state returned from _DtSaverStart()
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  -------
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

extern void _DtSaverStop(Display *, void *);


#endif /*_dtsaverp_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
