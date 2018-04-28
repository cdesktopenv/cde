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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: graphics.c /main/4 1995/11/02 14:05:07 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/******************************************************************************
 **  Program:           dticon
 **
 **  Description:       X11-based multi-color icon editor
 **
 **  File:              graphics.c, which contains the following subroutines or
 **                     functions:
 **                       Flicker_Arc()
 **                       Circle_Box()
 **                       Set_HotBox_Coords()
 **                       Start_HotBox()
 **                       Do_HotBox()
 **                       Stop_HotBox()
 **
 ******************************************************************************
 **
 **  Copyright Hewlett-Packard Company, 1990, 1991, 1992.
 **  All rights are reserved.  Copying or reproduction of this program,
 **  except for archival purposes, is prohibited without prior written
 **  consent of Hewlett-Packard Company.
 **
 **  Hewlett-Packard makes no representations about the suitibility of this
 **  software for any purpose.  It is provided "as is" without express or
 **  implied warranty.
 **
 ******************************************************************************/
#include <Xm/Xm.h>
#include <math.h>
#include "externals.h"

extern GC scratch_gc;

Widget editMenu_cut_pb;
Widget editMenu_copy_pb;
Widget editMenu_rotate_pb;
Widget editMenu_flip_pb;
Widget editMenu_scale_pb;

extern void Stop_HotBox(void);

/***************************************************************************
 *                                                                         *
 * Routine:   Flicker_Arc                                                  *
 *                                                                         *
 * Purpose:   Given 2 points (top-left and bottom-right), draw an          *
 *            invertable ellipse around the box they form.                 *
 *                                                                         *
 *X11***********************************************************************/

void
Flicker_Arc(
        Window win,
        int x1,
        int y1,
        int x2,
        int y2 )
{
  int x, y, width, height;

  x = min(x1, x2);
  y = min(y1, y2);
  width  = abs(x1 - x2);
  height = abs(y1 - y2);

#ifdef DEBUG
  if (debug)
    stat_out("Doing Flicker_Arc: x=%d, y=%d, width=%d, height=%d\n",
                x, y, width, height);
#endif

  if ((width > 0) && (height > 0))
    XDrawArc(dpy, win, Flicker_gc, x, y, width, height, 0, 360*64);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Circle_Box                                                   *
 *                                                                         *
 * Purpose:   Given 2 points (the center and radius of a circle)           *
 *            generate a box which would exactly enclose this circle,      *
 *            and then draw a flickering circle that matches the box.      *
 *            WARNING: [x1,y1] are always assumed to be the centerpoint    *
 *            and this routine will generate bogus results if this is      *
 *            not TRUE.                                                    *
 *                                                                         *
 *X11***********************************************************************/

void
Circle_Box(
        Window win,
        int x1,
        int y1,
        int x2,
        int y2,
        XRectangle *box )
{
  int radius, top_x, top_y, bottom_x, bottom_y, width, height;
  double size;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Circle_Box\n");
#endif

  width  = mag(x1, x2);
  height = mag(y1, y2);
  size   = (double) ((width * width) + (height * height));
  radius = (int) sqrt(size);

  top_x = x1 - radius;
  top_y = y1 - radius;
  bottom_x  = x1 + radius;
  bottom_y  = y1 + radius;

#ifdef DEBUG
  if (debug)
    stat_out("  Circle_Box values: tx=%d, ty=%d, bx=%d, by=%d\n",
                        top_x, top_y, bottom_x, bottom_x);
#endif

  Flicker_Arc(win, top_x, top_y, bottom_x, bottom_y);

  box->x = top_x;
  box->y = top_y;
  box->width  = bottom_x - top_x + 1;
  box->height = bottom_y - top_y + 1;
#ifdef DEBUG
  if (debug)
    stat_out("Leaving Circle_Box\n");
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Set_HotBox_Coords                                            *
 *                                                                         *
 * Purpose:   A SELECT operation has just occurred.  Initiate a timer      *
 *            calculate the area to enclose.                               *
 *            not TRUE.                                                    *
 *                                                                         *
 *X11***********************************************************************/

#define FLASH_INTERVAL    300
static Boolean FlashState=False;
static int flash_x, flash_y, flash_width, flash_height;
static int box_x1, box_y1, box_x2, box_y2;
static XtIntervalId selectTimerID;
static void Do_HotBox();

void
Set_HotBox_Coords( void )
{
  int min_x, min_y, max_x, max_y, tmp_x, tmp_y;

  min_x = min(ix, last_ix);
  min_y = min(iy, last_iy);
  max_x = max(ix, last_ix);
  max_y = max(iy, last_iy);

/*** make sure all four points are on the tablet ***/

  if (min_x < 0)
    min_x = 0;
  if (min_y < 0)
    min_y = 0;
  if ((max_x) >= icon_width)
    max_x = icon_width-1;
  if ((max_y) >= icon_height)
    max_y = icon_height-1;

  select_box.x = min_x;
  select_box.y = min_y;
  select_box.width  = max_x - min_x + 1;
  select_box.height = max_y - min_y + 1;

  box_x1 = min_x;
  box_y1 = min_y;
  box_x2 = max_x+1;
  box_y2 = max_y+1;

  Tablet_Coords(min_x, min_y, &flash_x, &flash_y);
  Tablet_Coords(max_x+1, max_y+1, &tmp_x, &tmp_y);
  flash_width  = tmp_x - flash_x;
  flash_height = tmp_y - flash_y;

#ifdef DEBUG
  if (debug) {
    stat_out("  select_box: x=%d, y=%d, width=%d, height=%d\n",
                select_box.x, select_box.y,
                select_box.width, select_box.height);
    stat_out("  flash box: x=%d, y=%d, width=%d, height=%d\n",
                flash_x, flash_y, flash_width, flash_height);
   }
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Start_HotBox                                                 *
 *                                                                         *
 * Purpose:   A SELECT operation has just occurred.  Initiate a timer      *
 *            which flashes a 1-pixel wide box around the perimeter of     *
 *            the selected rectangle every FLASH_INTERVAL milliseconds.    *
 *            Use the global variables ix, iy, last_ix, last_iy to         *
 *            calculate the area to enclose.                               *
 *                                                                         *
 *X11***********************************************************************/

void
Start_HotBox(
        int flag )
{

#ifdef DEBUG
  if (debug)
    stat_out("Entering Start_HotBox\n");
#endif

  Selected = True;
  /* turn on stuff that uses the selected area */
  XtSetSensitive( editMenu_cut_pb, True);
  XtSetSensitive( editMenu_copy_pb, True);
  XtSetSensitive(editMenu_rotate_pb, True);
  XtSetSensitive(editMenu_flip_pb,  True);
  XtSetSensitive(editMenu_scale_pb,  True);
  XSync(dpy, 0);
  if (flag == INITIAL)
    Set_HotBox_Coords();

  selectTimerID = XtAppAddTimeOut(AppContext,
                                  FLASH_INTERVAL,
                                  (XtTimerCallbackProc) Do_HotBox,
                                  NULL);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Start_HotBox - TimerID=%d\n", selectTimerID);
#endif

}


/***************************************************************************
 *                                                                         *
 * Routine:   Do_HotBox                                                    *
 *                                                                         *
 * Purpose:   Flash one alternating pulse around the selected area, and    *
 *            then re-set itself to activate again in FLASH_INTERVAL       *
 *            milliseconds.                                                *
 *                                                                         *
 * note: Check selectTimerID so that timeouts added for previous selects   *
 *       are ignored.  ex: if new select is started before previous select *
 *       timeout is serviced, "Selected" will already be set to true again *
 *       (for new select) when timeout from old selection is called... so  *
 *       now Stop_HotBox is called immediately (before HotBox Coords are   *
 *       set for new select), and last timeout is ignored.                 *
 *                                                                         *
 *X11***********************************************************************/

static void
Do_HotBox(
        XtPointer *client_data,
        XtIntervalId *local_id )
{

#ifdef DEBUG
  if (debug)
    stat_out(". ");
#endif

  if (GraphicsOp != SELECT)
    Selected = False;
  if (*local_id == selectTimerID)
  {
    if (Selected)
    {
      if (FlashState) {
        FlashState = False;
        XSetForeground(dpy, scratch_gc, black_pixel);
      }
      else
      {
        FlashState = True;
        XSetForeground(dpy, scratch_gc, white_pixel);
      }
    XSetLineAttributes(dpy, scratch_gc, 1, LineSolid, CapButt, JoinMiter);
    XDrawRectangle(dpy, tablet_win, scratch_gc,
        flash_x, flash_y, flash_width, flash_height);
    selectTimerID=XtAppAddTimeOut(AppContext,
                                  FLASH_INTERVAL,
                                  (XtTimerCallbackProc) Do_HotBox,
                                  NULL);
    }
    else
      Stop_HotBox();
  }
}


/***************************************************************************
 *                                                                         *
 * Routine:   Stop_HotBox                                                  *
 *                                                                         *
 * Purpose:   Undo the last Selected border operation.                     *
 *                                                                         *
 *X11***********************************************************************/

void
Stop_HotBox( void )
{
  int min_x, min_y, max_x, max_y, tmp_x, tmp_y;
  static int tmp_ix, tmp_iy;
  static Boolean Rotate_Move=False;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Stop_HotBox\n");
#endif

  if (GridEnabled) {
    XDrawLine(dpy, tablet_win, Grid_gc,
                flash_x, flash_y, (flash_x+flash_width), flash_y);
    XDrawLine(dpy, tablet_win, Grid_gc,
                flash_x, (flash_y+flash_height), (flash_x+flash_width),
                (flash_y+flash_height));
    XDrawLine(dpy, tablet_win, Grid_gc,
                flash_x, flash_y, flash_x, (flash_y+flash_height));
    XDrawLine(dpy, tablet_win, Grid_gc,
                (flash_x+flash_width), flash_y, (flash_x+flash_width),
                (flash_y+flash_height));
   }
  else {
    /* since Rotate left and right moves ix and iy revert to tmp_ix, tmp_iy */
    if (Rotate_Move) { Rotate_Move = False;
                       ix = tmp_ix;
                       iy = tmp_iy; }
    min_x = min(ix, last_ix);
    min_y = min(iy, last_iy);
    max_x = max(ix, last_ix);
    max_y = max(iy, last_iy);
    if (++max_x >= icon_width) max_x--;
    if (++max_y >= icon_height) max_y--;
    Transfer_Back_Image(min_x, min_y, max_x, max_y, HOLLOW);
    /* if it is a Rotate Op. then keep ix, iy */
    if(GraphicsOp == S_ROTATE) { Rotate_Move = True;
                                 tmp_ix = ix;
                                 tmp_iy = iy; }
   }

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Stop_HotBox\n");
#endif

}
