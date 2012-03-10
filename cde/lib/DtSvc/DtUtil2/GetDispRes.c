/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/****************************<+>*************************************
 **
 **   File:     GetDispRes.c
 **
 **   RCS:	$XConsortium: GetDispRes.c /main/4 1995/10/26 15:22:04 rswiston $
 **
 **   Project:  HP DT Runtime Library
 **
 **   Description: Get the display resolution of a particular display
 **                and screen.
 **
 **   (c) Copyright 1992 by Hewlett-Packard Company
 **
 ********************************************************************/
#include <stdio.h>
#include <X11/Xlib.h>
#include "GetDispRes.h"

/******************************<->*************************************
 *
 *  _DtGetDisplayResolution(DISPLAY, screen)
 *
 *  Description:
 *  -----------
 *  This function determines whether a display is HIGH, MEDIUM, or
 *  LOW display resolution.
 *
 *  Inputs:
 *  -------
 *  Pointer to the DISPLAY and a screen number
 *
 *  Outputs:
 *  -------
 *  One of four types of resolution:  HIGH_RES_DISPLAY, 
 *				      MED_RES_DISPLAY,
 *				      LOW_RES_DISPLAY,
 *				      NO_RES_DISPLAY (if below min for LOW) 
 * 
 *  Comments:
 *  --------
 *  Algorithm only uses XWidthOfScreen to make the determination.
 * 
 ******************************<->***********************************/
int
_DtGetDisplayResolution(Display *disp, int  screen)
{
	int screenWidth = XDisplayWidth(disp, screen);

	if (screenWidth >= _DT_HIGH_RES_MIN) {
	   return(HIGH_RES_DISPLAY);
	}

	if (screenWidth >= _DT_MED_RES_MIN) {
	   return(MED_RES_DISPLAY);
	}

	if (screenWidth >= _DT_LOW_RES_MIN) {
	   return(LOW_RES_DISPLAY);
  	}

	return(NO_RES_DISPLAY);
}
/**************         eof          ******************/
