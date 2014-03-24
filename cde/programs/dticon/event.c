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
/* $XConsortium: event.c /main/6 1996/10/21 15:28:00 mgreess $ */
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
 **  File:              event.c, which contains the following subroutines or
 **                     functions:
 **                       Clear_ClickCount()
 **                       ProcessTabletEvent()
 **                       Do_ButtonOp()
 **                       iLine()
 **                       iRectangle()
 **                       iArc()
 **                       iPolygon()
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
#include <stdio.h>

#include <Xm/Xm.h>
#include "externals.h"
#include "main.h"

#ifdef DEBUG
extern Widget iconForm;
#endif

static Boolean DoingHotBox=False;
static XtIntervalId polyTimerID;
Widget coordinateText;
static int Prev_x, Prev_y;

static void
Clear_ClickCount( void )
{
#ifdef DEBUG
  if (debug)
    stat_out(" - CLICK-COUNT ZEROED.\n");
#endif
  ClickCount = 0;
}

/* Forward declarations */

void iArc(
        int x,
        int y,
        int width,
        int height,
        Boolean backupFlag);
void iLine(
        int x1,
        int y1,
        int x2,
        int y2,
        Boolean backupFlag);
void iRectangle(
        int x,
        int y,
        int width,
        int height,
        Boolean backupFlag);
void iPolygon(void);
void Do_ButtonOp(XEvent *);

/***************************************************************************
 *                                                                         *
 * Routine:   ProcessTabletEvent                                           *
 *                                                                         *
 * Purpose:   Global event-handling routine for user input on the tablet.  *
 *                                                                         *
 ***************************************************************************/

void
ProcessTabletEvent(
        Widget w,
        XEvent *xptr,
        String *params,
        Cardinal num_params )
{
#ifdef DEBUG
  Arg args[10];
  Dimension db_width, db_height;
#endif
  int x, y, width, height, i, n, xGrid, yGrid;
  char tmpstr[20];
  XmString str;

  if (num_params == 3)
    ClickCount = atoi(params[2]);
#ifdef DEBUG
  if (debug)
    printf("Event - ");
#endif
  switch(xptr->type) {
    case ButtonPress    : x = xptr->xbutton.x;
                          y = xptr->xbutton.y;
#ifdef DEBUG
  if (debug)
    printf("Btn Press at [%d,%d]\n", x, y);
#endif
                          if (xptr->xbutton.window == tablet_win)
                            Do_ButtonOp(xptr);
                          break;
    case MotionNotify   : x = xptr->xmotion.x;
                          y = xptr->xmotion.y;

                          /* Adjust for the Magnification Factor */
                          xGrid = x/MagFactor + 1;
                          yGrid = y/MagFactor + 1;

                          /*
                           if coordinates are different then
                              update Previous x & y
                              Display new coordinates, but first...
                              If the ERASER is ON then
                                 redraw the contents of the previous Rect.
                                 Draw the bounding Rect.
                          */
                          if (xGrid!=Prev_x || yGrid!=Prev_y){
                             if (GraphicsOp == ERASER)
                             {
                                 Transfer_Back_Image(Prev_x-2, Prev_y-2,
                                                    (Prev_x+1), (Prev_y+1), HOLLOW);
                                 XDrawRectangle(dpy, tablet_win, Erase_gc,
                                                xGrid*MagFactor-(2*MagFactor),
                                                yGrid*MagFactor-(2*MagFactor),
                                                (3*MagFactor), (3*MagFactor) );
                             }
                             Prev_x = xGrid;
                             Prev_y = yGrid;
                             tmpstr[0] = '\0';
                             sprintf(tmpstr, "%dx%d", xGrid, yGrid);
                             str= XmStringCreateLocalized (tmpstr);
                             XtVaSetValues (coordinateText, XmNlabelString, str, NULL);
                             XmStringFree(str);
                          }
#ifdef DEBUG
  if (debug)
    printf("Btn Motion at [%d,%d]\n", x, y);
#endif
                          if (xptr->xmotion.window == tablet_win)
                            Do_ButtonOp(xptr);
                          break;
    case ButtonRelease  : x = xptr->xbutton.x;
                          y = xptr->xbutton.y;
#ifdef DEBUG
  if (debug) {
    printf("Btn Release at [%d,%d]", x, y);
    if (ClickCount)
      printf(", CLICK COUNT = %d", ClickCount);
    printf("\n");
   }
#endif
                          if (xptr->xbutton.window == tablet_win)
                            Do_ButtonOp(xptr);
                          break;
    case Expose         : if (!tablet_win) {
                            tablet_win = XtWindow(tablet_wid);
                          }
                          if (xptr->xexpose.window == tablet_win) {
                            x = xptr->xexpose.x;
                            y = xptr->xexpose.y;
                            width = xptr->xexpose.width;
                            height = xptr->xexpose.height;
#ifdef DEBUG
  if (debug)
    printf("Exposure at [%d,%d] of %dx%d\n", x, y, width, height);
#endif
                            Repaint_Tablet(xptr->xexpose.window, x, y,
                                        width, height);
                           }
                          else if (xptr->xexpose.window ==
                                        XtWindow(iconImage)) {
                            XCopyArea(dpy, color_icon,
                                XtWindow(iconImage),
                                Color_gc, 0, 0, icon_width, icon_height, 0, 0);
#ifdef DEBUG
  if (debug)
    printf("Exposure on iconImage: REPAINTING\n");
#endif
                           }
                          else if (xptr->xexpose.window ==
                                        XtWindow(monoImage)) {
                            XCopyArea(dpy, mono_icon,
                                XtWindow(monoImage),
                                Color_gc, 0, 0, icon_width, icon_height, 0, 0);
#ifdef DEBUG
  if (debug)
    printf("Exposure on monoImage: REPAINTING\n");
#endif
                           }
                          if (argsNeedProcessed)
                            ProcessAppArgs();
                          break;
    case EnterNotify    : x = xptr->xcrossing.x;
                          y = xptr->xcrossing.y;
#ifdef DEBUG
  if (debug)
    printf("Enter window at [%d,%d]\n", x, y);
#endif
                          break;
    case LeaveNotify    : x = xptr->xcrossing.x;
                          y = xptr->xcrossing.y;

                          /* if in ERASER mode
                             restore contents of bounding Rect when
                             leaving the tablet    */
                          if (GraphicsOp == ERASER )
                          Transfer_Back_Image(Prev_x-2, Prev_y-2,
                                             (Prev_x+1), (Prev_y+1), HOLLOW);
                          if (xptr->xcrossing.window == tablet_win)
                            Do_ButtonOp(xptr);
#ifdef DEBUG
  if (debug) {
    printf("Leave window at [%d,%d]\n", x, y);
    i = 0;
    XtSetArg(args[i], XmNwidth, &db_width); i++;
    XtSetArg(args[i], XmNheight, &db_height); i++;
    XtGetValues(iconForm, args, i);
    stat_out(" - iconForm dimensions: [%dx%d]\n", db_width, db_height);
    i = 0;
    XtSetArg(args[i], XmNwidth, &db_width); i++;
    XtGetValues(iconImage, args, i);
    stat_out(" - icon widths: %d\n", db_width);
   }
#endif
                          break;
    case FocusIn        :
    case FocusOut       :
#ifdef DEBUG
  if (debug)
    printf("Focus change\n");
#endif
                          break;
    default             :
#ifdef DEBUG
  if (debug)
    printf("UNKNOWN type: %d\n", xptr->type);
#endif
                          break;
    break;
   } /* switch */
}


/***************************************************************************
 *                                                                         *
 * Routine:   Do_ButtonOp                                                  *
 *                                                                         *
 * Purpose:   Process ButtonPress/ButtonMotion/ButtonRelease events        *
 *            generated by the user on the tablet.  Switch between correct *
 *            behaviors depending on the current GraphicsOp tool selected. *
 *                                                                         *
 ***************************************************************************/

static int Pressed = False, First = False;

void
Do_ButtonOp(
        XEvent *xptr )
{
  Window lwin;
  int e_type, x, y, i, j;
  XRectangle box;
  char tmpstr[20];
  XmString str;

  e_type = xptr->type;
  switch (e_type) {
    case ButtonPress   :
    case ButtonRelease :  x = xptr->xbutton.x;
                          y = xptr->xbutton.y;
                          lwin = xptr->xbutton.window;
                          break;
    case MotionNotify  :  x = xptr->xmotion.x;
                          y = xptr->xmotion.y;
                          lwin = xptr->xmotion.window;
                          break;
   } /* switch */

  switch (e_type) {
    case ButtonPress :
      switch (GraphicsOp) {
         case S_HOTSPOT : if (xptr->xbutton.window == tablet_win) {
                            tx = x;
                            ty = y;
                            Icon_Coords(tx, ty, &ix, &iy);
                            X_Hot = ix;
                            Y_Hot = iy;
                            hotSpot = True;
#ifdef DEBUG
  if (debug)
    stat_out(" - HOTSPOT selected at [%d,%d]\n", ix, iy);
#endif
                            Repaint_Exposed_Tablet();
                           }
                          break;
         case S_PASTE   :
         case S_ROTATE  :
         case S_SCALE_2 : tx = x;
                          ty = y;
                          if (GraphicsOp == S_PASTE)
                            XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, last_tx, last_ty,
                                        CutCopy->width*MagFactor - MagFactor,
                                        CutCopy->height*MagFactor - MagFactor);
                          else if (GraphicsOp == S_ROTATE)
                            XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, last_tx, last_ty,
                                        Rotate->width*MagFactor - MagFactor,
                                        Rotate->height*MagFactor - MagFactor);
                          else
                            XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, last_tx, last_ty,
                                        Scale->width*MagFactor - MagFactor,
                                        Scale->height*MagFactor - MagFactor);
                          Icon_Coords(tx, ty, &ix, &iy);
                          Do_Paste(ix, iy);
                          break;
         case ERASER    :
         case POINT     : Pressed = True;
                          last_tx = tx;
                          last_ty = ty;
                          tx = x;
                          ty = y;
                          Icon_Coords(tx, ty, &ix, &iy);
                          Backup_Icons();
                          if (GraphicsOp == ERASER) {
                            for (i=ix-1; i<=ix+1; i++)
                              for (j=iy-1; j<=iy+1; j++) {
                                XDrawPoint(dpy, color_icon, Erase_gc, i, j);
                                XDrawPoint(dpy,
                                        XtWindow(iconImage),
                                        Erase_gc, i, j);
                                XDrawPoint(dpy, mono_icon, Erase_gc, i, j);
                                XDrawPoint(dpy,
                                        XtWindow(monoImage),
                                        Erase_gc, i, j);
                                Paint_Tile(i, j, Erase_gc);
                               }
                           }
                          else {
                            XDrawPoint(dpy, color_icon, Color_gc, ix, iy);
                            XDrawPoint(dpy, XtWindow(iconImage),
                                Color_gc, ix, iy);
                            XDrawPoint(dpy, mono_icon, Mono_gc, ix, iy);
                            XDrawPoint(dpy, XtWindow(monoImage),
                                Mono_gc, ix, iy);
                            Paint_Tile(ix, iy, Color_gc);
                           }
         case FLOOD     : break;
         case SELECT    : if (DoingHotBox) {
                            DoingHotBox = False;
                            Selected = False;
                            Stop_HotBox();
                          }
         case LINE      :
         case CIRCLE    :
         case ELLIPSE   :
         case S_SCALE_1 :
         case RECTANGLE : Quantize(&x, &y, True);
                          Pressed = True;
                          tx = x;
                          ty = y;
                          last_tx = tx;
                          last_ty = ty;
                          First = True;
#ifdef DEBUG
  if (debug)
    stat_out(" - Quantized to [%d,%d]\n", x, y);
#endif
                          break;
         case S_GRAB    : XGrabServer(dpy);
                          XSync(dpy, False);
                          Pressed = True;
                          tx = x;
                          ty = y;
                          last_tx = tx;
                          last_ty = ty;
                          First = True;
                          break;
         case POLYLINE  :
         case POLYGON   :
                          Quantize(&x, &y, True);
                          tx = x;
                          ty = y;
                          First = True;
                          Icon_Coords(tx, ty, &ix, &iy);
                          pointList[pointCount].x = ix;
                          pointList[pointCount].y = iy;
                          if (pointCount < MAX_PTS) {
                            pointCount++;
#ifdef DEBUG
  if (debug)
    stat_out(" - pointCount++ [%d]\n", pointCount);
#endif
                           }
                          if (!Anchored) {
                            Anchored = True;
                            pointCount = 1;
                            Backup_Icons();
                           }
                          else {
                            if (ClickCount) {
                              Anchored = False;
                              ClickCount = 0;
                              if (GraphicsOp == POLYGON)
                                iPolygon();
                              pointCount = 0;
                             }
                            Icon_Coords(last_tx, last_ty, &last_ix, &last_iy);
                            iLine(ix, iy, last_ix, last_iy, False);
                           }
                          last_tx = tx;
                          last_ty = ty;
                          ClickCount++;
                          polyTimerID = XtAppAddTimeOut( AppContext,
                                                         multiClickTime,
                                                         (XtTimerCallbackProc)
                                                             Clear_ClickCount,
                                                         NULL);
                          break;
       } /* switch(GraphicsOp) */
    break;
    case ButtonRelease :
      switch (GraphicsOp) {
         case ERASER    :
         case POINT     : if (Pressed) {
                            last_tx = tx;
                            last_ty = ty;
                            tx = x;
                            ty = y;
                            Icon_Coords(tx, ty, &ix, &iy);
                            if (GraphicsOp == ERASER) {
                              for (i=ix-1; i<=ix+1; i++)
                                for (j=iy-1; j<=iy+1; j++) {
                                  XDrawPoint(dpy, color_icon, Erase_gc, i, j);
                                  XDrawPoint(dpy,
                                        XtWindow(iconImage),
                                        Erase_gc, i, j);
                                  XDrawPoint(dpy, mono_icon, Erase_gc, i, j);
                                  XDrawPoint(dpy,
                                        XtWindow(monoImage),
                                        Erase_gc, i, j);
                                  Paint_Tile(i, j, Erase_gc);
                                 } /* for */
                             } /* if(GraphicsOp...) */
                            else {
                              XDrawPoint(dpy, color_icon, Color_gc, ix, iy);
                              XDrawPoint(dpy, XtWindow(iconImage),
                                Color_gc, ix, iy);
                              XDrawPoint(dpy, mono_icon, Mono_gc, ix, iy);
                              XDrawPoint(dpy, XtWindow(monoImage),
                                Mono_gc, ix, iy);
                              Paint_Tile(ix, iy, Color_gc);
                             } /* else */
                            Pressed = False;
                           } /* if(Pressed) */
                          break;
         case FLOOD     : tx = x;
                          ty = y;
                          Icon_Coords(tx, ty, &ix, &iy);
                          Flood_Region(ix, iy);
                          break;
         case LINE      : if (Pressed) {
                            Quantize(&x, &y, True);
#ifdef DEBUG
  if (debug)
    stat_out(" - Quantized to [%d,%d]\n", x, y);
#endif
                            XDrawLine(dpy, tablet_win,
                                Flicker_gc, tx, ty, last_tx, last_ty);
                            tx = x;
                            ty = y;
                            Pressed = False;
                            Icon_Coords(tx, ty, &ix, &iy);
                            Icon_Coords(last_tx, last_ty, &last_ix, &last_iy);
                            iLine(ix, iy, last_ix, last_iy, True);
                           }
                          break;
         case S_SCALE_1 :
         case SELECT    : if (Pressed) {
                            Quantize(&x, &y, True);
                            XDrawRectangle(dpy, tablet_win,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty));
                            tx = x;
                            ty = y;
                            Pressed = False;
                            Icon_Coords(tx, ty, &ix, &iy);
                            Icon_Coords(last_tx, last_ty, &last_ix, &last_iy);
                            if (GraphicsOp == SELECT) {
                              DoingHotBox = True;
                              Start_HotBox(INITIAL);
                             }
                            else {
                              Scale_Image(); /***TAG***/
                              GraphicsOp = S_SCALE_2;
                             }
                           }
                          break;
         case S_GRAB    : if (Pressed) {
                            XDrawRectangle(dpy, lwin,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty));
                            if (mag(last_tx, x) <= xrdb.maxIconWidth)
                              tx = x;
                            if (mag(last_ty, y) <= xrdb.maxIconHeight)
                              ty = y;
                            LoadGrabbedImage(
                                min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty));
                            GraphicsOp = Backup_G_Op;
                            Backup_G_Op = 0;
                            Pressed = False;
                           }
                          break;
         case RECTANGLE : if (Pressed) {
                            Quantize(&x, &y, True);
                            XDrawRectangle(dpy, tablet_win,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty));
                            tx = x;
                            ty = y;
                            Pressed = False;
                            Icon_Coords(tx, ty, &ix, &iy);
                            Icon_Coords(last_tx, last_ty, &last_ix, &last_iy);
                            iRectangle(min(ix, last_ix), min(iy, last_iy),
                                abs(ix-last_ix), abs(iy-last_iy), True);
                           }
                          break;
         case S_HOTSPOT :
         case S_WAIT_RELEASE :
                          GraphicsOp = Backup_G_Op;
                          Backup_G_Op = 0;
                          break;
         case S_ROTATE  :
         case S_SCALE_2 :
         case POLYLINE  :
         case POLYGON   :
                          break;
         case CIRCLE    : if (Pressed) {
                            Quantize(&x, &y, True);
                            Circle_Box(tablet_win,
                                last_tx, last_ty, tx, ty, &box);
                            tx = x;
                            ty = y;
                            Pressed = False;
                            box.x /= MagFactor;
                            box.y /= MagFactor;
                            box.width =
				((short)(box.width - 1) / MagFactor) + 1;
                            box.height =
				((short)(box.height - 1) / MagFactor) + 1;
                            iArc(box.x, box.y, box.width, box.height, True);
                           }
                          break;
         case ELLIPSE   : if (Pressed) {
                            Quantize(&x, &y, True);
                            XDrawArc(dpy, tablet_win,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty), 0, 360*64);
                            tx = x;
                            ty = y;
                            Pressed = False;
                            Icon_Coords(tx, ty, &ix, &iy);
                            Icon_Coords(last_tx, last_ty, &last_ix, &last_iy);
                            iArc( min(ix, last_ix), min(iy, last_iy),
                                abs(ix-last_ix)+1, abs(iy-last_iy)+1, True);
                           }
                          break;
       } /* switch(GraphicsOp) */
    break;
    case MotionNotify :
      switch (GraphicsOp) {
         case S_PASTE   :
         case S_ROTATE  :
         case S_SCALE_2 : last_tx = tx;
                          last_ty = ty;
                          tx = x;
                          ty = y;
                          if (!FirstRigid) {
                            if (GraphicsOp == S_PASTE)
                              XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, last_tx, last_ty,
                                        CutCopy->width*MagFactor - MagFactor,
                                        CutCopy->height*MagFactor - MagFactor);
                            else if (GraphicsOp == S_ROTATE)
                              XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, last_tx, last_ty,
                                        Rotate->width*MagFactor - MagFactor,
                                        Rotate->height*MagFactor - MagFactor);
                            else
                              XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, last_tx, last_ty,
                                        Scale->width*MagFactor - MagFactor,
                                        Scale->height*MagFactor - MagFactor);
                           }
                          if (GraphicsOp == S_PASTE)
                            XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, tx, ty,
                                        CutCopy->width*MagFactor - MagFactor,
                                        CutCopy->height*MagFactor - MagFactor);
                          else if (GraphicsOp == S_ROTATE)
                            XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, tx, ty,
                                        Rotate->width*MagFactor - MagFactor,
                                        Rotate->height*MagFactor - MagFactor);
                          else
                            XDrawRectangle(dpy, tablet_win,
                                        Flicker_gc, tx, ty,
                                        Scale->width*MagFactor - MagFactor,
                                        Scale->height*MagFactor - MagFactor);
                          FirstRigid = False;
         case S_HOTSPOT : break;
         case ERASER    :
         case POINT     : if (Pressed) {
                            last_tx = tx;
                            last_ty = ty;
                            tx = x;
                            ty = y;
                            Icon_Coords(tx, ty, &ix, &iy);
                            if (GraphicsOp == ERASER) {
                              for (i=ix-1; i<=ix+1; i++)
                                for (j=iy-1; j<=iy+1; j++) {
                                  XDrawPoint(dpy, color_icon, Erase_gc, i, j);
                                  XDrawPoint(dpy,
                                        XtWindow(iconImage),
                                        Erase_gc, i, j);
                                  XDrawPoint(dpy, mono_icon, Erase_gc, i, j);
                                  XDrawPoint(dpy,
                                        XtWindow(monoImage),
                                        Erase_gc, i, j);
                                  Paint_Tile(i, j, Erase_gc);
                                 } /* for */
                             } /* if(GraphicsOp...) */
                            else {
                              XDrawPoint(dpy, color_icon, Color_gc, ix, iy);
                              XDrawPoint(dpy, XtWindow(iconImage),
                                Color_gc, ix, iy);
                              XDrawPoint(dpy, mono_icon, Mono_gc, ix, iy);
                              XDrawPoint(dpy, XtWindow(monoImage),
                                Mono_gc, ix, iy);
                              Paint_Tile(ix, iy, Color_gc);
                             } /* else */
                           }
         case FLOOD     : break;
         case POLYLINE  :
         case POLYGON   : if (Anchored) {
                            Quantize(&x, &y, True);
                            if (!First)
                              XDrawLine(dpy, tablet_win,
                                Flicker_gc, tx, ty, last_tx, last_ty);
                            tx = x;
                            ty = y;
                            XDrawLine(dpy, tablet_win,
                                Flicker_gc, tx, ty, last_tx, last_ty);
                            First = False;
                           }
                          break;
         case LINE      : if (Pressed) {
                            Quantize(&x, &y, True);
                            if (!First)
                              XDrawLine(dpy, tablet_win,
                                Flicker_gc, tx, ty, last_tx, last_ty);
                            tx = x;
                            ty = y;
                            XDrawLine(dpy, tablet_win,
                                Flicker_gc, tx, ty, last_tx, last_ty);
                            First = False;
                           }
                          break;
         case ELLIPSE   :
         case SELECT    :
         case S_SCALE_1 :
         case RECTANGLE : if (Pressed) {
                            Quantize(&x, &y, True);
                            if (!First) {
                              if (GraphicsOp != ELLIPSE)
                                XDrawRectangle(dpy, tablet_win, Flicker_gc,
                                        min(tx, last_tx), min(ty, last_ty),
                                        abs(tx-last_tx), abs(ty-last_ty));
                              else
                                XDrawArc(dpy, tablet_win, Flicker_gc,
                                        min(tx, last_tx), min(ty, last_ty),
                                        abs(tx-last_tx), abs(ty-last_ty),
                                        0, 360*64);
                             }
                            tx = x;
                            ty = y;
                            if (GraphicsOp != ELLIPSE)
                              XDrawRectangle(dpy, tablet_win,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty));
                            else
                              XDrawArc(dpy, tablet_win,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty), 0, 360*64);
                            First = False;
                           }
                          break;
         case S_GRAB    : if (Pressed) {
                            if (!First)
                                XDrawRectangle(dpy, lwin, Flicker_gc,
                                        min(tx, last_tx), min(ty, last_ty),
                                        abs(tx-last_tx), abs(ty-last_ty));
                            if (mag(last_tx, x) <= xrdb.maxIconWidth)
                              tx = x;
                            if (mag(last_ty, y) <= xrdb.maxIconHeight)
                              ty = y;
                            XDrawRectangle(dpy, lwin,
                                Flicker_gc, min(tx, last_tx), min(ty, last_ty),
                                abs(tx-last_tx), abs(ty-last_ty));
                            First = False;
                            /* Tell the user the size of the grabbed image
                            */
                             sprintf(tmpstr, 
			       GETSTR(18,2,"Grabbed Image Size = %dx%d"), 
			       abs(tx-last_tx), abs(ty-last_ty));
                             str= XmStringCreateLocalized (tmpstr);
                             XtVaSetValues (coordinateText, XmNlabelString, str, NULL);
                             XmStringFree(str);
                           }
                          break;
         case CIRCLE    : if (Pressed) {
                            Quantize(&x, &y, True);
                            if (!First)
                              Circle_Box(tablet_win,
                                last_tx, last_ty, tx, ty, &box);
                            tx = x;
                            ty = y;
                            Circle_Box(tablet_win,
                                last_tx, last_ty, tx, ty, &box);
                            First = False;
                           }
                          break;
       } /* switch(GraphicsOp) */
    break;
    case LeaveNotify :
      switch (GraphicsOp) {
         case POLYLINE  :
         case POLYGON   : if (Anchored) {
                            if (!First)
                              XDrawLine(dpy, tablet_win, Flicker_gc, tx, ty,
                                        last_tx, last_ty);
                            First = True;
                           }
                          break;
       } /* switch(GraphicsOp) */
    break;
   } /* switch(e_type) */
}


/***************************************************************************
 * Routine:   EndPolyOp                                                    *
 * Purpose:   When user does a mouse action outside of the drawing window, *
 *            this implicitly ends the poly* operation.  This means that   *
 *            things like drawing circles and changing pen color while in  *
 *            the middle of a polyline or polygon is no longer allowed.    *
 ***************************************************************************/
void
EndPolyOp()
{
    Anchored = False;
    Clear_ClickCount();
    pointCount = 0;
}

void
iLine(
        int x1,
        int y1,
        int x2,
        int y2,
        Boolean backupFlag )
{
  if (backupFlag)
    Backup_Icons();
  XDrawLine(dpy, color_icon, Color_gc, x1, y1, x2, y2);
  XDrawLine(dpy, mono_icon, Mono_gc, x1, y1, x2, y2);
  XDrawLine(dpy, XtWindow(iconImage), Color_gc, x1, y1, x2, y2);
  XDrawLine(dpy, XtWindow(monoImage), Mono_gc, x1, y1, x2, y2);
#ifdef DEBUG
  if (debug)
    stat_out("Transferring from [%d,%d] to [%d,%d]\n", tx, ty,
                last_tx, last_ty);
#endif
  Transfer_Back_Image(x1, y1, x2, y2, FILL);
}

void
iRectangle(
        int x,
        int y,
        int width,
        int height,
        Boolean backupFlag )
{
  if (backupFlag)
    Backup_Icons();
  if (FillSolids) {
    XFillRectangle(dpy, color_icon, Color_gc, x, y, width+1, height+1);
    XFillRectangle(dpy, mono_icon, Mono_gc, x, y, width+1, height+1);
    XFillRectangle(dpy, XtWindow(iconImage), Color_gc,
                        x, y, width+1, height+1);
    XFillRectangle(dpy, XtWindow(monoImage), Mono_gc,
                        x, y, width+1, height+1);
   }
  else {
    XDrawRectangle(dpy, color_icon, Color_gc, x, y, width, height);
    XDrawRectangle(dpy, mono_icon, Mono_gc, x, y, width, height);
    XDrawRectangle(dpy, XtWindow(iconImage), Color_gc,
                        x, y, width, height);
    XDrawRectangle(dpy, XtWindow(monoImage), Mono_gc,
                        x, y, width, height);
   }
  Transfer_Back_Image(x, y, x+width, y+height, FILL);
}

void
iArc(
        int x,
        int y,
        int width,
        int height,
        Boolean backupFlag )
{
  if (backupFlag)
    Backup_Icons();
  if (FillSolids) {
    XFillArc(dpy, color_icon, Color_gc, x, y, width, height, 0, 360*64);
    XFillArc(dpy, mono_icon, Mono_gc, x, y, width, height, 0, 360*64);
    XFillArc(dpy, XtWindow(iconImage),
        Color_gc, x, y, width, height, 0, 360*64);
    XFillArc(dpy, XtWindow(monoImage),
        Mono_gc, x, y, width, height, 0, 360*64);
   }
  else {
    XDrawArc(dpy, color_icon, Color_gc, x, y, width, height, 0, 360*64);
    XDrawArc(dpy, mono_icon, Mono_gc, x, y, width, height, 0, 360*64);
    XDrawArc(dpy, XtWindow(iconImage),
        Color_gc, x, y, width, height, 0, 360*64);
    XDrawArc(dpy, XtWindow(monoImage),
        Mono_gc, x, y, width, height, 0, 360*64);
   }
  Transfer_Back_Image(x, y, x+width, y+height, FILL);
}

void
iPolygon( void )
{
  int min_x, min_y, max_x, max_y, i;

#ifdef DEBUG
  if (debug)
    stat_out("iPolygon: pointCount = %d\n", pointCount);
#endif

  if (FillSolids) {
    XFillPolygon(dpy, color_icon, Color_gc, pointList, pointCount,
        Complex, CoordModeOrigin);
    XFillPolygon(dpy, mono_icon, Mono_gc, pointList, pointCount,
        Complex, CoordModeOrigin);
    XFillPolygon(dpy, XtWindow(iconImage),
        Color_gc, pointList, pointCount, Complex, CoordModeOrigin);
    XFillPolygon(dpy, XtWindow(monoImage),
        Mono_gc, pointList, pointCount, Complex, CoordModeOrigin);
   }
  else {
    if (pointCount < MAX_PTS) {
      pointList[pointCount].x = pointList[0].x;
      pointList[pointCount].y = pointList[0].y;
      pointCount++;
     }
    XDrawLines(dpy, color_icon, Color_gc, pointList, pointCount, Complex);
    XDrawLines(dpy, mono_icon, Mono_gc, pointList, pointCount, Complex);
    XDrawLines(dpy, XtWindow(iconImage), Color_gc, pointList,
        pointCount, Complex);
    XDrawLines(dpy, XtWindow(monoImage), Mono_gc, pointList,
        pointCount, Complex);
   }
  min_x = pointList[0].x;
  min_y = pointList[0].y;
  max_x = min_x;
  max_y = min_y;
  for (i=1; i<pointCount; i++) {
    if (pointList[i].x < min_x) min_x = pointList[i].x;
    if (pointList[i].y < min_y) min_y = pointList[i].y;
    if (pointList[i].x > max_x) max_x = pointList[i].x;
    if (pointList[i].y > max_y) max_y = pointList[i].y;
   }
  Transfer_Back_Image(min_x, min_y, max_x, max_y, FILL);
}
