/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*******************************************************************************
**
**  deskset.c
**
**  $TOG: deskset.c /main/4 1999/02/03 15:35:56 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static  char sccsid[] = "@(#)deskset.c 1.11 94/12/22 Copyr 1993 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <string.h>
#ifndef SVR4
#include <strings.h>
#endif /* SVR4 */
#include <ctype.h>
#include <pwd.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Dt/Dt.h>
#include "deskset.h"
#include "revision.h"

#define DS_TITLE_LINE_HEIGHT	25

/******************************************************************************
**
**  Function:		ds_relname
**
**  Description:	Return revision name.
**		
**  Parameters:		None
**
**  Returns:		Revision name (char *)
**
******************************************************************************/
extern char *
ds_relname()
{
	static char	buf[MAXNAMELEN];

	sprintf(buf, "Version %d.%d.%d Revision %d",
		DtVERSION, DtREVISION, DtUPDATE_LEVEL,
		DTCM_INTERNAL_REV);
	return buf;
}

/*
 * Function:       ds_position_popup
 *
 * Description:    Position a popup relative to the parent frame
 *                 making sure it doesn't go off of the screen.
 *
 * Parameters:     base            Popup's parent widget
 *                 popup           Popup widget
 *                 location_op     Where you would like the popup to
 *                                 appear.  Location_op may be any
 *                                 the following:
 *
 * DS_POPUP_LEFT   Place the popup to the left of base with the tops flush
 * DS_POPUP_RIGHT  Place the popup to the right of base with the tops flush
 * DS_POPUP_ABOVE  Place the popup above base with the left edges flush
 * DS_POPUP_BELOW  Place the popup below base with the left edges flush
 * DS_POPUP_LOR    Place the popup either to the left or right of base
 *                 depending on which side has the most space.
 * DS_POPUP_AOF    Place the popup either above or below base
 *                 depending on which side has the most space.
 * DS_POPUP_CENTERED       Center popup within baseframe
 *
 * Returns:        0       Could not get screen size
 *                 1       All is well
 */
extern int
ds_position_popup(Widget base, Widget popup, ds_location_op location_op) {
  int bh, bw, bx, by, ph, pw, px, py ;
  int screen_width, screen_height ;
  Position base_x, base_y, popup_x, popup_y ;
  Dimension base_width, base_height, popup_width, popup_height ;

  XtVaGetValues(base,
                XmNx,      &base_x,
                XmNy,      &base_y,
                XmNwidth,  &base_width,
                XmNheight, &base_height,
                NULL) ;
  bx = (int) base_x ;
  by = (int) base_y ;
  bw = (int) base_width ;
  bh = (int) base_height ;

  XtVaGetValues(popup,
                XmNx,      &popup_x,
                XmNy,      &popup_y,
                XmNwidth,  &popup_width,
                XmNheight, &popup_height,
                NULL) ;

  px = (int) popup_x ;
  py = (int) popup_y ;
  pw = (int) popup_width ;
  ph = (int) popup_height ;

  ds_get_screen_size(popup, &screen_width, &screen_height) ;
 
  if (location_op == DS_POPUP_LOR)
    {
      if (bx >= screen_width - bw - bx) location_op = DS_POPUP_LEFT ;
      else                              location_op = DS_POPUP_RIGHT ;
    }
  else if (location_op == DS_POPUP_AOB)
    {
      if (by > screen_height - bh - by) location_op = DS_POPUP_ABOVE ;
      else                              location_op = DS_POPUP_BELOW ;
    }

  switch (location_op)
    {
      case DS_POPUP_RIGHT    : px = bx + bw + 5 ;
                               py = by - DS_TITLE_LINE_HEIGHT ;
                               break ;
      case DS_POPUP_LEFT     : px = bx - pw - 5 ;
                               py = by - DS_TITLE_LINE_HEIGHT ;
                               break ;
      case DS_POPUP_ABOVE    : px = bx - 5 ;
                               py = by - ph - 10 ;
                               break ;
      case DS_POPUP_BELOW    : px = bx - 5 ;
                               py = by + bh + 5 ;
                               break ;
      case DS_POPUP_CENTERED :
      default                : px = bx + (bw - pw) / 2 ;
                               py = by + (bh - ph) / 2 ;
    }
  ds_force_popup_on_screen(popup, &px, &py) ;
  return 1;
}


/*  Function:       ds_force_popup_on_screen
 *
 *  Description:    Make sure that the specified widget appears entirely
 *                  on the screen.
 *
 *                  You specify the x and y where you would like the
 *                  popup to appear.  If this location would cause any
 *                  portion of the popup to appear off of the screen
 *                  then the routine makes the minimum adjustments
 *                  necessary to move it onto the screen.
 *
 *                  NOTE:   The following coordinates must be specified
 *                          relative to the screen origin *not* the
 *                          parent widget!
 *
 *  Parameters:     popup_x_p       Pointer to x location where you would
 *                                  like the popup to appear.  If the popup
 *                                  is moved this is updated to reflect
 *                                  the new position.
 *                  popup_y_p       Pointer to y location where you would
 *                                  like the popup to appear.  If the popup
 *                                  is moved this is updated to reflect
 *                                  the new position.
 *
 *                  popup           Popup`s widget.
 *
 *  Returns:        TRUE    The popup was moved
 *                  FALSE   The popup was not moved
 */
extern int
ds_force_popup_on_screen(Widget popup, int *popup_x_p, int *popup_y_p) {
  Dimension popup_width, popup_height ;
  Position left, top ;
  int popup_x, popup_y ;
  int n, rcode, screen_width, screen_height ;

  popup_x = *popup_x_p ;
  popup_y = *popup_y_p ;

/* Get the screen size */

  ds_get_screen_size(popup, &screen_width, &screen_height) ;

  XtVaGetValues(popup,
                XmNwidth,  &popup_width,
                XmNheight, &popup_height,
                NULL) ;

/* Make sure frame does not go off side of screen */

  n = popup_x + (int) popup_width ;
  if (n > screen_width) popup_x -= (n - screen_width) ;
  else if (popup_x < 0) popup_x = 0 ;

/* Make sure frame doen't go off top or bottom */

  n = popup_y + (int) popup_height ;
  if (n > screen_height) popup_y -= n - screen_height ;
  else if (popup_y < 0) popup_y = 0 ;

/* Set location and return */

  left = (Position) popup_x ;
  top  = (Position) popup_y ;
  XtVaSetValues(popup,
                XmNx, left,
                XmNy, top,
                0) ;

  if (popup_x != *popup_x_p || popup_y != *popup_y_p) rcode = TRUE ;
  else                                                rcode = FALSE ;
  *popup_x_p = popup_x ;
  *popup_y_p = popup_y ;
  return(rcode) ;
}


/*  Function:       ds_get_screen_size
 *
 *  Description:    Get the width and height of the screen in pixels
 *
 *  Parameters:     width_p         Pointer to an integer to place width
 *                  height_p        Pointer to an integer to place height
 *        
 *  Returns:        None.
 */
extern void
ds_get_screen_size(Widget widget, int *width_p, int *height_p) {
  Display *dpy  = XtDisplay(widget) ;
  int screen    = DefaultScreen(dpy) ;
 
  *width_p  = DisplayWidth(dpy, screen) ;
  *height_p = DisplayHeight(dpy, screen) ;
}
