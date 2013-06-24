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
/* $TOG: utils.c /main/9 1998/09/24 12:32:05 samborn $ */
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
 **  File:              utils.c, which contains the following subroutines or
 **                     functions:
 **                       Create_Gfx_Labels()
 **                       Init_Editor()
 **                       New_MagFactor()
 **                       New_FileFormat()
 **                       Icon_Coords()
 **                       Tablet_Coords()
 **                       Quantize()
 **                       Repaint_Exposed_Tablet()
 **                       Repaint_Tablet()
 **                       Paint_Tile()
 **                       Transfer_Back_Image()
 **                       Init_Widget_List()
 **                       Init_Pen_Colors()
 **                       Init_Color_Table()
 **                       Size_IconForm()
 **                       Init_Icons()
 **                       Abort()
 **                       stat_out()
 **                       Switch_FillSolids()
 **                       Select_New_Pen()
 **                       Backup_Icons()
 **                       DoErrorDialog()
 **                       DoQueryDialog()
 **                       Do_GrabOp()
 **                       LoadGrabbedImage()
 **                       ParseAppArgs()
 **                       ProcessAppArgs()
 **                       Set_Gfx_Labels()
 **                       PixelTableLookup()
 **                       PixelTableClear()
 **
 ******************************************************************************
 **
 **  Copyright 1991 by Hewlett-Packard Company, 1990, 1991, 1992.
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
#include <sys/stat.h>
#include <X11/cursorfont.h>
#include <Xm/Protocols.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/MessageB.h>
#include <Xm/VendorSEP.h>
#include <Dt/UserMsg.h>
#include <Xm/DragC.h>
#include <Dt/Dnd.h>
#include "externals.h"
#include "main.h"
/* Copied from Xm/BaseClassI.h */
extern XmWidgetExtData _XmGetWidgetExtData( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */

#include "pixmaps/Circle.pm"
#include "pixmaps/CircleSolid.pm"
#include "pixmaps/Connected.pm"
#include "pixmaps/ConnectedClosed.pm"
#include "pixmaps/ConnecClosedSolid.pm"
#include "pixmaps/Ellipse.pm"
#include "pixmaps/EllipseSolid.pm"
#include "pixmaps/Eraser.pm"
#include "pixmaps/Rectangle.pm"
#include "pixmaps/RectangleSolid.pm"
#include "pixmaps/PaintBucket.pm"
#include "pixmaps/Pencil.pm"
#include "pixmaps/Line.pm"
#include "pixmaps/SelectArea.pm"

#include "bitmaps/q_mark.xbm"

extern Widget  color_pb1, color_pb2, color_pb3, color_pb4,
               color_pb5, color_pb6, color_pb7, color_pb8;
extern Widget  grey_pb1, grey_pb2, grey_pb3, grey_pb4,
               grey_pb5, grey_pb6, grey_pb7, grey_pb8;
extern Widget  fgColorToggle, bgColorToggle, tsColorToggle,
               bsColorToggle, selectColorToggle,
               transparentColorToggle;
extern Widget  pointButton, floodButton, lineButton, polylineButton,
               rectangleButton, polygonButton, circleButton,
               ellipseButton, eraseButton, selectButton;
extern Widget  viewport, tablet, tabletBorder, tabletFrame;
extern Widget  iconForm, iconSize, monoLabel;
extern Widget  fillToggle, magMenu_8x_tb;
extern Widget  formatMenu_xbm_tb, formatMenu_xpm_tb;
extern Widget  optionsMenu_grid, menu1, queryDialog, stdErrDialog;
extern Widget  tablet_wid;
extern Window  tablet_win;

static int jskXerrorDebug();
static int jskXerrorIODebug();

Widget editMenu_undo_pb;

void GetSessionInfo( void );
void Set_Gfx_Labels( Boolean );
void Init_Icons(
                Dimension width,
                Dimension height,
                Boolean saveFlag);
int PixelTableLookup(
                Pixel pixelIn,
                Boolean allocNew);
extern void *Process_DropCheckOp(
                Widget,
                XtPointer,
                XtPointer);
extern void *Process_DropOp(
                Widget,
                XtPointer,
                XtPointer);
extern void Repaint_Tablet(Window, int, int, int, int);
extern void Init_Widget_List(void);
extern void Init_Pen_Colors(Widget);
extern void Init_Color_Table(void);
extern void RegisterDropSites(void);
extern void Abort(char *);
extern void DoErrorDialog(char *);

char dash_list[2] = {1,1};
char err_str[80];
char start_file[256];
char start_size[80];
Widget clip_wid;
GC scratch_gc;
Position x_margin, y_margin;
Pixmap pointPix, floodPix, linePix, polylinePix, rectPix;
Pixmap polygonPix, circlePix, ellipsePix, eraserPix, selectPix;
Pixmap rectSolidPix, circleSolidPix, polygonSolidPix, ellipseSolidPix;
extern int successFormat, x_hot, y_hot;
extern unsigned int width_ret, height_ret;

/*  Structure used on a save session to see if a dt is iconic  */
typedef struct
{
   int state;
   Window icon;
} WM_STATE;

/***************************************************************************
 *                                                                         *
 * Routine:   Create_Gfx_Labels                                            *
 *                                                                         *
 * Purpose:   Initialize all the global variables used by the icon editor. *
 *                                                                         *
 ***************************************************************************/

void
Create_Gfx_Labels(
        unsigned long fg,
        unsigned long bg )
{
  Arg args[10];
  int i, depth;
  Pixmap q_markPix, mask;

  depth = XDefaultDepth(dpy, screen);

/**********
  pointPix = XCreatePixmapFromBitmapData(dpy, root,
                        point_bits, point_width, point_height,
                        fg, bg, depth);
  floodPix = XCreatePixmapFromBitmapData(dpy, root,
                        flood_bits, flood_width, flood_height,
                        fg, bg, depth);
  linePix = XCreatePixmapFromBitmapData(dpy, root,
                        line_bits, line_width, line_height,
                        fg, bg, depth);
  polylinePix = XCreatePixmapFromBitmapData(dpy, root,
                        polyline_bits, polyline_width, polyline_height,
                        fg, bg, depth);
  rectPix = XCreatePixmapFromBitmapData(dpy, root,
                        rectangle_bits, rectangle_width, rectangle_height,
                        fg, bg, depth);
  polygonPix = XCreatePixmapFromBitmapData(dpy, root,
                        polygon_bits, polygon_width, polygon_height,
                        fg, bg, depth);
  circlePix = XCreatePixmapFromBitmapData(dpy, root,
                        circle_bits, circle_width, circle_height,
                        fg, bg, depth);
  ellipsePix = XCreatePixmapFromBitmapData(dpy, root,
                        ellipse_bits, ellipse_width, ellipse_height,
                        fg, bg, depth);
  eraserPix = XCreatePixmapFromBitmapData(dpy, root,
                        eraser_bits, eraser_width, eraser_height,
                        fg, bg, depth);
  selectPix = XCreatePixmapFromBitmapData(dpy, root,
                        select_bits, select_width, select_height,
                        fg, bg, depth);
**********/

  xpm_ReadAttribs.valuemask = READ_FLAGS;
  xpm_ReadAttribs.colorsymbols = colorSymbols;
  xpm_ReadAttribs.numsymbols = NUM_PENS;

  status = _DtXpmCreatePixmapFromData(dpy, root, Circle, &circlePix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,2, "Cannot initialize button icon for circle"));
  status = _DtXpmCreatePixmapFromData(dpy, root, CircleSolid, &circleSolidPix,
                                &mask, &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,4, "Cannot initialize button icon for solid circle"));
  status = _DtXpmCreatePixmapFromData(dpy, root, Connected, &polylinePix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,6, "Cannot initialize button icon for polyline"));
  status = _DtXpmCreatePixmapFromData(dpy, root, ConnectedClosed, &polygonPix,
                                &mask, &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,8, "Cannot initialize button icon for polygon"));
  status = _DtXpmCreatePixmapFromData(dpy, root, ConnecClosedSolid,
                        &polygonSolidPix, &mask, &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,10, "Cannot initialize button icon for solid polygon"));
  status = _DtXpmCreatePixmapFromData(dpy, root, Ellipse, &ellipsePix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,12, "Cannot initialize button icon for ellipse"));
  status = _DtXpmCreatePixmapFromData(dpy, root, EllipseSolid, &ellipseSolidPix,
                                &mask, &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,14, "Cannot initialize button icon for solid ellipse"));
  status = _DtXpmCreatePixmapFromData(dpy, root, Eraser, &eraserPix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,16, "Cannot initialize button icon for eraser"));
  status = _DtXpmCreatePixmapFromData(dpy, root, Line, &linePix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,18, "Cannot initialize button icon for line"));
  status = _DtXpmCreatePixmapFromData(dpy, root, PaintBucket, &floodPix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,20, "Cannot initialize button icon for flood"));
  status = _DtXpmCreatePixmapFromData(dpy, root, Pencil, &pointPix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,22, "Cannot initialize button icon for point"));
  status = _DtXpmCreatePixmapFromData(dpy, root, Rectangle, &rectPix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,24, "Cannot initialize button icon for rectangle"));
  status = _DtXpmCreatePixmapFromData(dpy, root, RectangleSolid, &rectSolidPix,
                                &mask, &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,26, "Cannot initialize button icon for solid rectangle"));
  status = _DtXpmCreatePixmapFromData(dpy, root, SelectArea, &selectPix, &mask,
                                &xpm_ReadAttribs);
  if (status != XpmSuccess) Abort(GETSTR(10,28, "Cannot initialize button icon for select"));

  q_markPix = XCreatePixmapFromBitmapData(dpy, root,
                        (char*)q_mark_bits, q_mark_width, q_mark_height,
                        fg, bg, depth);

  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, pointPix); i++;
  XtSetValues(pointButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, linePix); i++;
  XtSetValues(lineButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, eraserPix); i++;
  XtSetValues(eraseButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, floodPix); i++;
  XtSetValues(floodButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, polylinePix); i++;
  XtSetValues(polylineButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, selectPix); i++;
  XtSetValues(selectButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelPixmap, q_markPix); i++;
  XtSetValues(queryDialog, args, i);

  Set_Gfx_Labels(HOLLOW);
}

/***************************************************************************
 *                                                                         *
 * Routine:   Init_Editor                                                  *
 *                                                                         *
 * Purpose:   Initialize all the global variables, states, widgets, etc,   *
 *            for the icon editor.  This is a long, messy, somewhat        *
 *            rambling routine.                                            *
 *                                                                         *
 ***************************************************************************/

void
Init_Editor(
        Widget wid )
{
  Window win;
  Arg args[10];
  int i;
  Position lx, ly;

/*** window ID of tablet ***/
  tablet_win = 0;

/*** Nothing needs to be saved, yet ***/
  Dirty = False;

/*** Initial Graphics states, pen color, etc. ***/
  GraphicsOp = POINT;
  CurrentColor = COLOR1;
  ColorBlock = STATIC_COLOR;
  FillSolids = False;
  MagFactor = 8;
  pointCount = 0;

/*** file I/O related globals ***/
  last_fname[0] = '\0';
  X_Hot = -1;
  Y_Hot = -1;

  XSetErrorHandler(jskXerrorDebug);
  XSetIOErrorHandler(jskXerrorIODebug);

/*** Xlib-related globals ***/
  dpy = XtDisplay(wid);
  win = XtWindow(wid);
  root = DefaultRootWindow(dpy);
  screen = DefaultScreen(dpy);
  screen_ptr = XtScreen(wid);
  black_pixel = BlackPixel(dpy, screen);
  white_pixel = WhitePixel(dpy, screen);
  Cmap = DefaultColormap(dpy, screen);

/*** initialize global pixel table data ***/
  pixelTable.pixelTableSize = 0;
  pixelTable.numItems = 0;
  pixelTable.lastFound = 0;
  pixelTable.item = NULL;

/*** Initialize the GCs used by the app. ***/
  Color_gc = XCreateGC(dpy, root, 0, 0);
  Mono_gc = XCreateGC(dpy, root, 0, 0);
  Grid_gc = XCreateGC(dpy, root, 0, 0);
  Erase_gc = XCreateGC(dpy, root, 0, 0);
  Flicker_gc = XCreateGC(dpy, root, 0, 0);
  scratch_gc = XCreateGC(dpy, root, 0, 0);
  XSetState(dpy, Flicker_gc, black_pixel, white_pixel, GXinvert,
       (DefaultDepthOfScreen(XtScreen(wid)) > 8) ? AllPlanes : 0x01);
  XSetSubwindowMode(dpy, Flicker_gc, IncludeInferiors);
  XSetDashes(dpy, Grid_gc, 0, dash_list, 2);
  XSetLineAttributes(dpy, Grid_gc, 0, LineDoubleDash, CapButt, JoinMiter);

/*** Initialize the widget variables ***/
  Init_Widget_List();

/*** Initialize the pen colors and the internal color table ***/
  Init_Pen_Colors(wid);
  Init_Color_Table();
  XSetForeground(dpy, Erase_gc, Transparent);

/*** configure the color and mono icons to their initial size ***/
  Init_Icons(icon_width, icon_height, DO_NOT_SAVE);

  xrdb.useBMS = FALSE;
  DtInitialize (dpy, wid, progName, progName);

/*** Drop site registration must occur after DtInitialize() ***/

  RegisterDropSites();

  multiClickTime = XtGetMultiClickTime(dpy);

/*** Now, do all the tedious widget initialization ***/
  i = 0;
  XtSetArg(args[i], XmNset, True); i++;
  XtSetValues(StaticWid[CurrentColor], args, i);
  XtSetValues(GraphicOpsWid[GraphicsOp], args, i);
  XtSetValues(magMenu_8x_tb, args, i);
  XtSetValues(optionsMenu_grid, args, i);
  XtSetValues(formatMenu_xpm_tb, args, i);
  i = 0;
  XtSetArg(args[i], XmNbackground, Transparent); i++;
  XtSetValues(tablet_wid, args, i);
  XtSetValues(tabletBorder, args, i);
  XSetForeground(dpy, Color_gc, StaticPen[0]);
  XSetForeground(dpy, Mono_gc, StaticMono[0]);
  i = 0;
  XtSetArg(args[i], XmNset, FillSolids); i++;
  XtSetValues(fillToggle, args, i);
  i = 0;
  XtSetArg(args[i], XmNclipWindow, &clip_wid); i++;
  XtGetValues(viewport, args, i);

  x_margin = 0;
  y_margin = 0;

#ifdef DEBUG
  if (debug) {
    stat_out("DEFAULT TIME-OUT VALUE IS %d MILLISECONDS\n", multiClickTime);
    stat_out("****CLIP WINDOW WIDGET = %d\n", clip_wid);
    stat_out("    TABLET OFFSETS [x,y] = [%d,%d]\n", x_margin, y_margin);
   }
#endif

  i = 0;
  XtSetArg(args[i], XmNbackground, Transparent); i++;
  XtSetValues(clip_wid, args, i);
  i = 0;
  XtSetArg(args[i], XmNset, False); i++;
  XtSetValues(floodButton, args, i);
  XtSetValues(lineButton, args, i);
  XtSetValues(polylineButton, args, i);
  XtSetValues(rectangleButton, args, i);
  XtSetValues(polygonButton, args, i);
  XtSetValues(circleButton, args, i);
  XtSetValues(ellipseButton, args, i);
  XtSetValues(eraseButton, args, i);
  XtSetValues(selectButton, args, i);
  i = 0;
  XtSetArg(args[i], XmNset, True); i++;
  XtSetValues(pointButton, args, i);
}


/***************************************************************************
 *                                                                         *
 * Routine:   GetMarginData                                                *
 *                                                                         *
 * Purpose:   We need to get margin data AFTER these widgets are realized  *
 *            in order to get valid data... so do it here                  *
 *                                                                         *
 ***************************************************************************/

void
GetMarginData( void )
{
  Arg args[10];
  int i;
  Position lx, ly;


#ifdef DEBUG
  if (debug)
    stat_out("****** Getting margin data: x=%d y=%d\n", x_margin, y_margin);
#endif

  i = 0;
  XtSetArg(args[i], XmNx, &lx); i++;
  XtSetArg(args[i], XmNy, &ly); i++;
  XtGetValues(tabletFrame, args, i);
  x_margin = lx;
  y_margin = ly;
  i = 0;
  XtSetArg(args[i], XmNx, &lx); i++;
  XtSetArg(args[i], XmNy, &ly); i++;
  XtGetValues(tablet_wid, args, i);
  x_margin += lx;
  y_margin += ly;

#ifdef DEBUG
  if (debug)
    stat_out("******         margin data: x=%d y=%d\n", x_margin, y_margin);
#endif

}


/***************************************************************************
 *                                                                         *
 * Routine:   New_MagFactor                                                *
 *                                                                         *
 * Purpose:   The user has just selected a new magnification factor for    *
 *            the fat pixels on the tablet.  Update the MagFactor global   *
 *            variable and reapaint the visible portion of the tablet to   *
 *            reflect the change.                                          *
 *                                                                         *
 ***************************************************************************/

void
New_MagFactor(
        int new_value )
{
  Arg args[10];
  int i;

#ifdef DEBUG
  if (debug)
    stat_out("****** Changing MagFactor to %d\n", new_value);
#endif

  MagFactor = new_value;
  i = 0;
  XtSetArg(args[i], XmNwidth, (icon_width*MagFactor)); i++;
  XtSetArg(args[i], XmNheight, (icon_height*MagFactor)); i++;
  XtSetValues(tablet_wid, args, i);
  if (Selected)
    Set_HotBox_Coords();
/* This code does not seem to be needed and since it slows down
   the grid drawing I'll take it out */
  /*Repaint_Exposed_Tablet();*/
}


/***************************************************************************
 *                                                                         *
 * Routine:   New_FileFormat                                               *
 *                                                                         *
 * Purpose:   Update the fileFormat value to reflect the user's current    *
 *            choice of default file output formats.                       *
 *                                                                         *
 ***************************************************************************/

void
New_FileFormat(
        int new_value )
{

#ifdef DEBUG
  if (debug) {
    stat_out("****** Changing Output File Format to ");
    switch (new_value) {
      case FORMAT_XPM : stat_out("XPM\n");
                        break;
      case FORMAT_XBM : stat_out("XBM\n");
                        break;
      default         : stat_out("UNKNOWN\n");
                        break;
     } /* switch */
   } /* if */
#endif

  fileFormat = new_value;

}


/***************************************************************************
 *                                                                         *
 * Routine:   Icon_Coords                                                  *
 *                                                                         *
 * Purpose:   Convert a set of [x,y] values (acquired from the tablet)     *
 *            into the equivalent [x,y] values they represent on the       *
 *            actual icon being created/modified.                          *
 *                                                                         *
 ***************************************************************************/

void
Icon_Coords(
        int normal_x,
        int normal_y,
        int *fat_x,
        int *fat_y )
{
  *fat_x = normal_x / MagFactor;
  *fat_y = normal_y / MagFactor;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Tablet_Coords                                                *
 *                                                                         *
 * Purpose:   Convert a set of fat [x,y] values back into raw [x,y] values *
 *            which map to the top-left corner of the fat pixel value.     *
 *                                                                         *
 ***************************************************************************/

void
Tablet_Coords(
        int fat_x,
        int fat_y,
        int *raw_x,
        int *raw_y )
{
  *raw_x = fat_x * MagFactor;
  *raw_y = fat_y * MagFactor;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Quantize                                                     *
 *                                                                         *
 * Purpose:   Adjust a set of [x,] values so that they mark the top-left   *
 *            corner (or the middle, depending on the 'center' flag) of    *
 *            whatever 'fat' pixel they map onto.  This will insure that   *
 *            any operation involving rubberband lines leaps from fat      *
 *            pixel center to fat pixel center, providing a clear visual   *
 *            indicator of which pixel currently contains the line/point.  *
 *                                                                         *
 ***************************************************************************/

void
Quantize(
        int *x,
        int *y,
        int center )
{
  int lx, ly, half;

  lx = *x;
  ly = *y;
  half = MagFactor/2;
  if ((lx%MagFactor) != 0)
    lx = (lx / MagFactor) * MagFactor;
  if ((ly%MagFactor) != 0)
    ly = (ly / MagFactor) * MagFactor;
  if (center) {
    lx += half;
    ly += half;
   }
  *x = lx;
  *y = ly;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Repaint_Exposed_Tablet                                       *
 *                                                                         *
 * Purpose:   Repaint the rectangular section of the tablet currently      *
 *            visible in the clipping area widget of the scrolled window   *
 *            widget.  To do this, we get the current width and height     *
 *            of the clipping area widget, the current width and height    *
 *            of the drawing area manager widget (the clipping area        *
 *            widget's immediate child), and the x and y position of the   *
 *            drawing area manager widget.  This will allow us to cal-     *
 *            culate what portion of the tablet (drawn button widget) is   *
 *            currently visible in the clipping area widget.  We then      *
 *            request th Repaint_Tablet() routine to repaint that portion  *
 *            of the drawn button.                                         *
 *                                                                         *
 ***************************************************************************/

void
Repaint_Exposed_Tablet( void )
{
  int i;
  Arg args[10];
  Position t_x, t_y;
  Dimension c_width, c_height, t_width, t_height;

  i = 0;
  XtSetArg(args[i], XmNwidth, &c_width); i++;
  XtSetArg(args[i], XmNheight, &c_height); i++;
  XtGetValues(clip_wid, args, i);
  i = 0;
  XtSetArg(args[i], XmNx, &t_x); i++;
  XtSetArg(args[i], XmNy, &t_y); i++;
  XtSetArg(args[i], XmNwidth, &t_width); i++;
  XtSetArg(args[i], XmNheight, &t_height); i++;
  XtGetValues(tabletBorder, args, i);

  t_x += x_margin;
  t_y += y_margin;
  t_x = -t_x;
  t_y = -t_y;
  if (t_x < 0) t_x = 0;
  if (t_y < 0) t_y = 0;

  Repaint_Tablet(tablet_win, t_x, t_y, c_width, c_height);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Repaint_Tablet                                               *
 *                                                                         *
 * Purpose:   Repaint the rectangular section of the tablet described by   *
 *            the [x,y,width,height] values in the XRectangle.             *
 *                                                                         *
 ***************************************************************************/

void
Repaint_Tablet(
        Window win,
        int x,
        int y,
        int width,
        int height )
{
  int i, j, lx, ly;
  XImage *scratch_img;

  scratch_img = XGetImage(dpy, color_icon, 0, 0, icon_width, icon_height,
                AllPlanes, ZPixmap);
#ifdef DEBUG
  if (debug)
    stat_out("****** Repaint_Tablet: x,y=%d,%d w,h=%d,%d\n", x,y,width,height);
#endif

  for (i=x; i<x+width+MagFactor; i+=MagFactor)
    for (j=y; j<y+height+MagFactor; j+=MagFactor) {
      Icon_Coords(i, j, &lx, &ly);
      if ((lx >= 0) && (lx < icon_width) &&
          (ly >= 0) && (ly < icon_height)) {
        XSetForeground(dpy, scratch_gc, XGetPixel(scratch_img, lx, ly));
        XFillRectangle(dpy, tablet_win, scratch_gc,
        lx*MagFactor, ly*MagFactor, MagFactor, MagFactor);
        if (hotSpot)
          if ((lx == X_Hot) && (ly == Y_Hot)) {
            XDrawLine(dpy, tablet_win, Grid_gc,
            lx*MagFactor, ly*MagFactor+MagFactor, lx*MagFactor+MagFactor, ly*MagFactor);
            XDrawLine(dpy, tablet_win, Grid_gc,
            lx*MagFactor, ly*MagFactor, lx*MagFactor+MagFactor, ly*MagFactor+MagFactor);
          }
       } /* if */
     } /* for */

  Quantize(&x, &y, False);
  width += (MagFactor*2);
  height += (MagFactor*2);

/*
   Draw the grid if Enabled....
*/
  if (GridEnabled) {
    for (i=x; i<=x+width+MagFactor; i+=MagFactor)
      XDrawLine(dpy, win, Grid_gc, i, y, i, (y+height));
    for (i=y; i<=y+height+MagFactor; i+=MagFactor)
      XDrawLine(dpy, win, Grid_gc, x, i, (x+width), i);
   }


  XDestroyImage(scratch_img);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Paint_Tile                                                   *
 *                                                                         *
 * Purpose:   Paint a single rectangular tile of size MagFactor x          *
 *            MagFactor with the color supplied in the passed GC.          *
 *            The X and Y parameters are coordinates for the 1:1           *
 *            sized icon, and not direct coordinates for the tablet.       *
 *                                                                         *
 ***************************************************************************/

void
Paint_Tile(
        int x,
        int y,
        GC gc )
{
  XFillRectangle(dpy, tablet_win, gc,
        x*MagFactor, y*MagFactor, MagFactor, MagFactor);

  if (GridEnabled)
    XDrawRectangle(dpy, tablet_win, Grid_gc,
        x*MagFactor, y*MagFactor, MagFactor, MagFactor);

  if (hotSpot)
    if ((x == X_Hot) && (y == Y_Hot)) {
      XDrawLine(dpy, tablet_win, Grid_gc,
        x*MagFactor, y*MagFactor+MagFactor, x*MagFactor+MagFactor, y*MagFactor);
      XDrawLine(dpy, tablet_win, Grid_gc,
        x*MagFactor, y*MagFactor, x*MagFactor+MagFactor, y*MagFactor+MagFactor);
     }
}


/***************************************************************************
 *                                                                         *
 * Routine:   Transfer_Back_Image                                          *
 *                                                                         *
 * Purpose:   Paint a single rectangular tile of size MagFactor x          *
 *            MagFactor with the color supplied in the passed GC.          *
 *            The [x,y] coordinate pairs are both icon coordinates,        *
 *            not tablet coordinates.                                      *
 *                                                                         *
 ***************************************************************************/

void
Transfer_Back_Image(
        int x1,
        int y1,
        int x2,
        int y2,
        Boolean tflag )
{
  int min_x, min_y, max_x, max_y, i, j;
  XImage *scratch_img;

#ifdef DEBUG
  if (debug) {
    stat_out("Entering Transfer_Back_Image\n");
    stat_out("Values are x1:%d, y1:%d, x2:%d, y2:%d, flag = ",
                x1, y1, x2, y2);
    stat_out("    icon size is width:%d, height:%d\n", icon_width, icon_height);
    switch(tflag) {
      case HOLLOW : stat_out("HOLLOW\n");
                    break;
      case FILL   : stat_out("FILL\n");
                    break;
      default     : stat_out("UNKNOWN\n");
                    break;
     } /* switch */
   }
#endif

  min_x = ((x1 < x2) ? x1 : x2);
  min_y = ((y1 < y2) ? y1 : y2);
  max_x = ((x1 > x2) ? x1 : x2);
  max_y = ((y1 > y2) ? y1 : y2);

/*** make sure max_x and max_y are within icon ***/
  if (max_x >= icon_width)
    max_x = icon_width-1;
  if (max_y >= icon_height)
    max_y = icon_height-1;

  scratch_img = XGetImage(dpy, color_icon, 0, 0, icon_width, icon_height,
                AllPlanes, ZPixmap);
/*** do the entire rectangular area... ***/
  if (tflag == FILL) {
    for (i = min_x; i <= max_x; i++)
      for (j = min_y; j <= max_y; j++) {
        XSetForeground(dpy, scratch_gc, XGetPixel(scratch_img, i, j));
        Paint_Tile(i, j, scratch_gc);
       } /* for */
   } /* if */
/*** ...or just do the border of rectangle ***/
  else {
    for (i = min_x; i <= max_x; i++) {
      XSetForeground(dpy, scratch_gc, XGetPixel(scratch_img, i, min_y));
      Paint_Tile(i, min_y, scratch_gc);
     }
    for (i = min_x; i <= max_x; i++) {
      XSetForeground(dpy, scratch_gc, XGetPixel(scratch_img, i, max_y));
      Paint_Tile(i, max_y, scratch_gc);
     }
    for (i = min_y; i <= max_y; i++) {
      XSetForeground(dpy, scratch_gc, XGetPixel(scratch_img, min_x, i));
      Paint_Tile(min_x, i, scratch_gc);
     }
    for (i = min_y; i <= max_y; i++) {
      XSetForeground(dpy, scratch_gc, XGetPixel(scratch_img, max_x, i));
      Paint_Tile(max_x, i, scratch_gc);
     }
   } /* else */
  XDestroyImage(scratch_img);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Transfer_Back_Image\n");
#endif

}


/***************************************************************************
 *                                                                         *
 * Routine:   Init_Widget_list                                             *
 *                                                                         *
 * Purpose:   Initialize all the pen color widgets into 2 widget ID        *
 *            arrays, to simplify referencing them later.                  *
 *                                                                         *
 ***************************************************************************/

void
Init_Widget_List( void )
{


  GraphicOpsWid[POINT]     = pointButton;
  GraphicOpsWid[FLOOD]     = floodButton;
  GraphicOpsWid[LINE]      = lineButton;
  GraphicOpsWid[POLYLINE]  = polylineButton;
  GraphicOpsWid[RECTANGLE] = rectangleButton;
  GraphicOpsWid[POLYGON]   = polygonButton;
  GraphicOpsWid[CIRCLE]    = circleButton;
  GraphicOpsWid[ELLIPSE]   = ellipseButton;
  GraphicOpsWid[ERASER]    = eraseButton;
  GraphicOpsWid[SELECT]    = selectButton;

  DynamicWid[BG_COLOR-BG_COLOR]     = bgColorToggle;
  DynamicWid[FG_COLOR-BG_COLOR]     = fgColorToggle;
  DynamicWid[TS_COLOR-BG_COLOR]     = tsColorToggle;
  DynamicWid[BS_COLOR-BG_COLOR]     = bsColorToggle;
  DynamicWid[SELECT_COLOR-BG_COLOR] = selectColorToggle;
  DynamicWid[TRANS_COLOR-BG_COLOR]  = transparentColorToggle;

  StaticWid[COLOR1] = color_pb1;
  StaticWid[COLOR2] = color_pb2;
  StaticWid[COLOR3] = color_pb3;
  StaticWid[COLOR4] = color_pb4;
  StaticWid[COLOR5] = color_pb5;
  StaticWid[COLOR6] = color_pb6;
  StaticWid[COLOR7] = color_pb7;
  StaticWid[COLOR8] = color_pb8;
  StaticWid[GREY1]  = grey_pb1;
  StaticWid[GREY2]  = grey_pb2;
  StaticWid[GREY3]  = grey_pb3;
  StaticWid[GREY4]  = grey_pb4;
  StaticWid[GREY5]  = grey_pb5;
  StaticWid[GREY6]  = grey_pb6;
  StaticWid[GREY7]  = grey_pb7;
  StaticWid[GREY8]  = grey_pb8;

  tablet_wid = tablet;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Init_Pen_Colors                                              *
 *                                                                         *
 * Purpose:   Initialize all the pen colors (both Static and Dynamic) and  *
 *            set the appropriate fg/bg colors for each pen button widget  *
 *            to reflect this.                                             *
 *                                                                         *
 ***************************************************************************/

void
Init_Pen_Colors(
        Widget wid )
{
  Window win;
  Pixel  transFg;
  XColor exact_def;
  Arg arg[10];
  int i, j, pixelTableIndex;

  i = 0;
  XtSetArg(arg[i], XmNbackground, &Background); i++;
  XtGetValues(wid, arg, i);
  XmGetColors(screen_ptr, Cmap, Background, &Foreground, &TopShadow,
                &BottomShadow, &Select);

#ifdef DEBUG
  if (debug)
    stat_out("Return from XmGetColors()\n");
#endif

/*** Set FOREGROUND Button fg/bg colors ***/
  i = 0;
  XtSetArg(arg[i], XmNbackground, Foreground); i++;
  pixelTableIndex = PixelTableLookup (Foreground, False);
  if (PIXEL_TABLE_MONO(pixelTableIndex) == white_pixel) {
    XtSetArg(arg[i], XmNforeground, black_pixel); i++;
  }
  else {
    XtSetArg(arg[i], XmNforeground, white_pixel); i++;
  }
  XtSetValues(fgColorToggle, arg, i);

/*** Set BACKGROUND Button fg/bg colors ***/
  i = 0;
  XtSetArg(arg[i], XmNbackground, Background); i++;
  XtSetArg(arg[i], XmNforeground, Foreground); i++;
  XtSetValues(bgColorToggle, arg, i);

/*** Set TOP_SHADOW Button fg/bg colors ***/
  i = 0;
  XtSetArg(arg[i], XmNbackground, TopShadow); i++;
  if (TopShadow == Foreground)  /* B & W */
    { XtSetArg(arg[i], XmNforeground, Background); i++; }
  else
    { XtSetArg(arg[i], XmNforeground, Foreground); i++; }
  XtSetValues(tsColorToggle, arg, i);

/*** Set BOTTOM_SHADOW Button fg/bg colors ***/
  i = 0;
  XtSetArg(arg[i], XmNbackground, BottomShadow); i++;
  if (BottomShadow == Foreground) /* B & W */
    { XtSetArg(arg[i], XmNforeground, Background); i++; }
  else
    { XtSetArg(arg[i], XmNforeground, Foreground); i++; }
  XtSetValues(bsColorToggle, arg, i);

/*** Set SELECT Button fg/bg colors ***/
  i = 0;
  XtSetArg(arg[i], XmNbackground, Select); i++;
  XtSetArg(arg[i], XmNforeground, Foreground); i++;
  XtSetValues(selectColorToggle, arg, i);

/*** Set TRANSPARENT Button fg/bg colors ***/
  i = 0;
  XtSetArg(arg[i], XmNbackground, &Transparent); i++;
  XtSetArg(arg[i], XmNforeground, &transFg);     i++;
  XtGetValues(menu1, arg, i);
  i = 0;
  XtSetArg(arg[i], XmNbackground, Transparent); i++;
  XtSetArg(arg[i], XmNforeground, transFg);     i++;
  XtSetValues(transparentColorToggle, arg, i);

/*** Store the colors in the dynamic array ***/

  DynamicPen[BG_COLOR-BG_COLOR] = Background;
  DynamicPen[FG_COLOR-BG_COLOR] = Foreground;
  DynamicPen[TS_COLOR-BG_COLOR] = TopShadow;
  DynamicPen[BS_COLOR-BG_COLOR] = BottomShadow;
  DynamicPen[SELECT_COLOR-BG_COLOR] = Select;
  DynamicPen[TRANS_COLOR-BG_COLOR] = Transparent;

#ifdef DEBUG
  if (debug)
    stat_out("Backgrounds set for all 6 dynamic colors\n");
#endif

/*** STUB *********************************************************/
  DynamicMono[BG_COLOR-BG_COLOR] = black_pixel;
  DynamicMono[FG_COLOR-BG_COLOR] = white_pixel;
  DynamicMono[TS_COLOR-BG_COLOR] = white_pixel;
  DynamicMono[BS_COLOR-BG_COLOR] = black_pixel;
  DynamicMono[SELECT_COLOR-BG_COLOR] = white_pixel;
  DynamicMono[TRANS_COLOR-BG_COLOR] = Transparent;

  for (i=0; i<NUM_STATICS; i++) {
  /*** Init the 16 color values ***/
    status = XParseColor(dpy, Cmap, color_table[i][5], &exact_def);
    if (!status) {
      sprintf(err_str, "%s %d",
              GETSTR(10,30, "Unable to parse static color no."), i+1);
      Abort(err_str);
     }
    status = XAllocColor(dpy, Cmap, &exact_def);
    if (!status) {
      sprintf(err_str, "%s %d",
              GETSTR(10,32, "Unable to allocate static color no."), i+1);
      Abort(err_str);
     }
    StaticPen[i] = exact_def.pixel;
    j = 0;
    XtSetArg(arg[j], XmNbackground, StaticPen[i]); j++;
    XtSetValues(StaticWid[i], arg, j);
  /*** Init the 16 monochrome values ***/
    status = XParseColor(dpy, Cmap, color_table[i][2], &exact_def);
    if (!status) {
      sprintf(err_str, "%s %d",
              GETSTR(10,30, "Unable to parse static color no."), i+1);
      Abort(err_str);
     }
    status = XAllocColor(dpy, Cmap, &exact_def);
    if (!status) {
      sprintf(err_str, "%s %d",
              GETSTR(10,32, "Unable to allocate static color no."), i+1);
      Abort(err_str);
     }
    StaticMono[i] = exact_def.pixel;
   } /* for */
/******************************************************************/
}


/***************************************************************************
 *                                                                         *
 * Routine:   Init_Color_Table                                             *
 *                                                                         *
 * Purpose:   Complete initialization of the color_table used to specify   *
 *            color naming conventions when writing out XPM files. Then    *
 *            initialize the name and pixel fields for all the elements    *
 *            of the global array colorSymbols.                            *
 *                                                                         *
 ***************************************************************************/

void
Init_Color_Table( void )
{
  int i, j;
  XColor cval;
  char ***colorTable;

  cmap_size = XDisplayCells(dpy, screen);

/***
  for (i=NUM_STATICS; i<NUM_STATICS+5; i++) {
    if (DynamicMono[i-NUM_STATICS] == black_pixel)
      color_table[i][2] = black_string;
    else
      color_table[i][2] = white_string;
    cval.pixel = DynamicPen[i-NUM_STATICS];
    XQueryColor(dpy, Cmap, &cval);
    sprintf(dynamic_c_str[i-NUM_STATICS], "#%04X%04X%04X",
                cval.red, cval.green, cval.blue);
    color_table[i][5] = dynamic_c_str[i-NUM_STATICS];
   }
  color_table[NUM_PENS-1][2] = none_string;

  cval.pixel = DynamicPen[TRANS_COLOR-BG_COLOR];
  XQueryColor(dpy, Cmap, &cval);
  sprintf(dynamic_c_str[NUM_DYNAMICS-1], "#%04X%04X%04X",
                cval.red, cval.green, cval.blue);
  color_table[NUM_PENS-1][5] = dynamic_c_str[NUM_DYNAMICS-1];
***/

#ifdef DEBUG
  if (debug) {
    stat_out("XPM Color Table initialized:\n");
    for (i=0; i<(NUM_PENS); i++)
      stat_out("  %s %18s %18s\n", color_table[i][0], color_table[i][1],
                                color_table[i][2]);
   }
#endif

  colorSymbols = (XpmColorSymbol *) XtMalloc(NUM_PENS * sizeof(XpmColorSymbol));
  if (!colorSymbols)
    Abort(GETSTR(10,38, "No memory(0)"));
  for (i=0; i<NUM_STATICS; i++) {
    colorSymbols[i].name  = (char *) XtMalloc(strlen(color_table[i][1]) +1);
    if (!colorSymbols[i].name)
      Abort(GETSTR(10,40, "No memory(1)"));
    strcpy(colorSymbols[i].name, color_table[i][1]);
    colorSymbols[i].value = "";
    colorSymbols[i].pixel = StaticPen[i];
   }
  for (i=NUM_STATICS; i<NUM_STATICS+NUM_DYNAMICS; i++) {
    colorSymbols[i].name  = (char *) XtMalloc(strlen(color_table[i][1]) +1);
    if (!colorSymbols[i].name)
      Abort(GETSTR(10,42, "No memory(2)"));
    strcpy(colorSymbols[i].name, color_table[i][1]);
    colorSymbols[i].value = "";
    colorSymbols[i].pixel = DynamicPen[i-NUM_STATICS];
   }

#ifdef DEBUG
  if (debug) {
    stat_out("%d Color Symbols installed:\n", NUM_PENS);
    for (i=0; i<NUM_PENS; i++) {
      stat_out("    %d - %s | %s | %d\n", (i+1), colorSymbols[i].name,
                        colorSymbols[i].value, colorSymbols[i].pixel);
     }
   }
#endif

/*** now comes the ugly part, initialize the .colorTable field   ***/
/*** in xpm_WriteAttribs.  This should remain constant for the   ***/
/*** life of the executable, and so only needs to be initialized ***/
/*** once.                                                       ***/

  colorTable = (char ***) calloc(NUM_PENS, sizeof(char **));
  xpm_WriteAttribs.colorTable = (XpmColor*)colorTable;
  xpm_WriteAttribs.pixels = (Pixel *) calloc(NUM_PENS, sizeof(Pixel));
  if (!xpm_WriteAttribs.colorTable)
    Abort(GETSTR(10,44, "No memory(3)"));
  for (i=0; i<NUM_PENS; i++) {
    colorTable[i] = (char **) calloc(6, sizeof(char *));
    if (!colorTable[i])
      Abort(GETSTR(10,46, "No memory(4)"));
   } /* for(i...) */
  for (i=0; i<NUM_PENS; i++) {
    xpm_WriteAttribs.pixels[i] = colorSymbols[i].pixel;
    for (j=0; j<6; j++) {
      if (color_table[i][j] && strlen(color_table[i][j]) > 0) {
        colorTable[i][j] = (char *) XtMalloc(strlen(color_table[i][j])+1);
        if (!colorTable[i][j])
          Abort(GETSTR(10,48, "No memory(5)"));
        strcpy(colorTable[i][j], color_table[i][j]);
       } /* if */
     } /* for(j...) */
   } /* for(i...) */
  xpm_WriteAttribs.ncolors = NUM_PENS;
  xpm_WriteAttribs.hints_cmt = hints_cmt;
  xpm_WriteAttribs.colors_cmt = colors_cmt;
  xpm_WriteAttribs.pixels_cmt = pixels_cmt;
  xpm_WriteAttribs.mask_pixel = 0x80000000;

#ifdef DEBUG
  if (debug)
    Dump_AttribStruct(&xpm_WriteAttribs);
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Size_IconForm                                                *
 *                                                                         *
 * Purpose:   Given new dimensions for the Color and Mono icon widgets,    *
 *            calculate and set the new overall dimensions for the icon    *
 *            form widget (iconForm) which holds both of them and their    *
 *            labels.                                                      *
 *                                                                         *
 ***************************************************************************/

void
Size_IconForm(
        Dimension width,
        Dimension height )
{
  Arg args[10];
  int i, top_offset;
  Dimension label1_w, label1_h, label2_w, label2_h;
  Dimension form_width, form_height, junk;
  XmString label1_str, label2_str;
  XmFontList label_fontlist;
  Position diff;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Size_IconForm\n");
#endif

/*********************************************************************
 * Before re-sizing the 2 icons (up or down), make sure their parent *
 * form's width is at least wide enough to fully show the two label  *
 * gadgets (iconSize and monoLabel).  If the icon widths are smaller *
 * than this, set offsets to center them on the space provided.      *
 *********************************************************************/

/*********************************************************************
 * First, get the minimum usable widths for the two label gadgets,   *
 * and their current heights.                                        *
 *********************************************************************/
  i = 0;
  XtSetArg(args[i], XmNfontList, &label_fontlist); i++;
  XtSetArg(args[i], XmNlabelString, &label1_str); i++;
  XtGetValues(iconSize, args, i);
  i = 0;
  XtSetArg(args[i], XmNlabelString, &label2_str); i++;
  XtGetValues(monoLabel, args, i);

  XmStringExtent(label_fontlist, label1_str, &label1_w, &junk);
  XmStringExtent(label_fontlist, label2_str, &label2_w, &junk);
  i = 0;
  XtSetArg(args[i], XmNheight, &label1_h); i++;
  XtGetValues(iconSize, args, i);
  i = 0;
  XtSetArg(args[i], XmNheight, &label2_h); i++;
  XtGetValues(monoLabel, args, i);

/*********************************************************************
 * If the min. width for either label gadget is greater than the     *
 * current icon widths, use half the difference between the two      *
 * widths as a left and right offset for the two icon (drawn button) *
 * widgets.                                                          *
 *********************************************************************/
  if ((label1_w > width) || (label2_w > width))
    diff = (Position) (((label1_w > label2_w) ? label1_w : label2_w)
                - width) / 2;
  else
    diff = 0;

  i = 0;
  XtSetArg(args[i], XmNleftOffset, diff); i++;
  XtSetValues(iconImage, args, i);
  XtSetValues(monoImage, args, i);

/*********************************************************************
 * The overall form dimensions should be as follows: the form width  *
 * will be the greater of the icon widths, or the widths of the two  *
 * label gadgets.  The form height will be the sum of the two icon   *
 * heights, plus the sum of heights of the two label gadgets, plus   *
 * the vertical offset between the bottom of the first label and the *
 * top of the second icon.                                           *
 *********************************************************************/
  i = 0;
  XtSetArg(args[i], XmNtopOffset, &top_offset); i++;
  XtGetValues(monoImage, args, i);
  form_width  = max(width, (Dimension)max(label1_w, label2_w));
  form_height = (height*2) + label1_h + label2_h + top_offset;
#ifdef DEBUG
  if (debug) {
    stat_out("  form_width = %d (of %d,%d,%d)\n", form_width, width, label1_w,
                label2_w);
    stat_out("  form_height = %d\n", form_height);
   }
#endif
  i = 0;
  XtSetArg(args[i], XmNwidth, form_width); i++;
  XtSetArg(args[i], XmNheight, form_height); i++;
  XtSetValues(iconForm, args, i);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Size_IconForm\n");
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Init_Icons                                                   *
 *                                                                         *
 * Purpose:   Initialize new color and mono icons at program start-up      *
 *            and at the following times:                                  *
 *              o When 'New' is selected from the 'File' pull-down menu.   *
 *              o When 'Resize Icon' is selected from the 'Edit' pull-down *
 *                menu.                                                    *
 *              o When 'Load' is selected from the 'File' pull-down menu,  *
 *                and a new file is read in.                               *
 *              o When 'Grab Screen Image' is selected from the 'Edit'     *
 *                pull-down menu, and the image is loaded into the editor. *
 *                                                                         *
 *            This routine sizes the iconImage, monoImage, and tablet      *
 *            widgets to the new dimensions (*MagFactor, and +1 in the     *
 *            case of the tablet widget).  It also creates the application-*
 *            internal Pixmaps associated with the iconImage and monoImage.*
 *            If the 'saveFlag' flag is 'True', it is assumed that the     *
 *            current icons are being resized and that there are existing  *
 *            images that need to be retained.  In this case, the new      *
 *            Pixmaps are created and the old Pixmaps are copied onto the  *
 *            new ones, before the old Pixmaps are freed.                  *
 *                                                                         *
 ***************************************************************************/

void
Init_Icons(
        Dimension width,
        Dimension height,
        Boolean saveFlag )
{
  Pixmap tmpPix;
  Pixmap tmp_color, tmp_mono;
  char text[40];
  Arg args[10];
  int i, x_offset, y_offset;
  XmString local_str;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Init_Icons: flag=%d\n", saveFlag);
    stat_out("         Init_Icons: color=%x mono=%x\n", color_icon, mono_icon);
#endif

  tmp_color = 0;
  tmp_mono = 0;

/*** App. init or 'New Icon' ***/
  if (!saveFlag) {
    if (color_icon)
      XFreePixmap(dpy, color_icon);
    if (mono_icon)
      XFreePixmap(dpy, mono_icon);
   }
/*** Resizing the existing icon ***/
  else {
    if (color_icon)
      tmp_color = color_icon;
    if (mono_icon)
      tmp_mono = mono_icon;
    x_offset = 0;
    y_offset = 0;
   }

  color_icon = XCreatePixmap(dpy, root, width, height,
                                DefaultDepth(dpy, screen));
  mono_icon = XCreatePixmap(dpy, root, width, height,
                                DefaultDepth(dpy, screen));

  if ((color_icon == 0) || (mono_icon == 0))
    Abort(GETSTR(10,50, "Cannot initialize application icon storage"));

  XSetForeground(dpy, scratch_gc, Transparent);
  XFillRectangle(dpy, color_icon, scratch_gc, 0, 0, width, height);
  XFillRectangle(dpy, mono_icon, scratch_gc, 0, 0, width, height);

  sprintf(text, "%d x %d", width, height);
  i = 0;
  local_str = XmStringCreateLocalized(text);
  XtSetArg(args[i], XmNlabelString, local_str); i++;
  XtSetValues(iconSize, args, i);
  XmStringFree(local_str);

  i = 0;
  XtSetArg(args[i], XmNwidth, width); i++;
  XtSetArg(args[i], XmNheight, height); i++;
  XtSetValues(iconImage, args, i);
  XtSetValues(monoImage, args, i);

/*********************************************************************
 * Call Size_IconForm() to re-do the layout of the iconForm widget,  *
 * which contains the iconImage, iconSize, monoImage, and monoLabel  *
 * widgets.                                                          *
 *********************************************************************/

  Size_IconForm(width, height);

  if (saveFlag) {
    XCopyArea(dpy, tmp_color, color_icon, Color_gc, 0, 0,
                icon_width, icon_height, x_offset, y_offset);
    XCopyArea(dpy, tmp_mono, mono_icon, Mono_gc, 0, 0,
                icon_width, icon_height, x_offset, y_offset);
    if (tmp_color)
      XFreePixmap(dpy, tmp_color);
    if (tmp_mono)
      XFreePixmap(dpy, tmp_mono);
   }

  if (XtWindow(iconImage))
    XCopyArea(dpy, color_icon, XtWindow(iconImage), Color_gc,
                0, 0, width, height, 0, 0);
  if (XtWindow(monoImage))
    XCopyArea(dpy, mono_icon, XtWindow(monoImage), Mono_gc,
                0, 0, width, height, 0, 0);
  i = 0;
  XtSetArg(args[i], XmNwidth, ((width*MagFactor)+1)); i++;
  XtSetArg(args[i], XmNheight, ((height*MagFactor)+1)); i++;
  XtSetValues(tablet_wid, args, i);

  icon_width  = width;
  icon_height = height;

/* This code does not seem to be needed and since it slows down
   the grid drawing I'll take it out for now*/
/*
  if (tablet_win)
     Repaint_Exposed_Tablet();
*/

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Init_Icons\n");
#endif
}

static void
TransferCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data)
{
        DtDndTransferCallback transferInfo = (DtDndTransferCallback) call_data;

        if (transferInfo->reason == DtCR_DND_TRANSFER &&
            transferInfo->operation == XmDROP_COPY) {

                Process_DropCheckOp(w, client_data, call_data);
        } else {
                transferInfo->status = DtDND_FAILURE;
        }
}

static void
AnimateCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data)
{
   DtDndDropAnimateCallbackStruct *animateInfo =
        (DtDndDropAnimateCallbackStruct *) call_data;

   if (animateInfo->reason == DtCR_DND_DROP_ANIMATE) {

      Process_DropOp(w, client_data, call_data);
   }
}

/***************************************************************************
 *                                                                         *
 * Routine:   RegisterDropSites                                            *
 *                                                                         *
 * Purpose:   Register the tablet as a valid drop zone.                    *
 *                                                                         *
 ***************************************************************************/

void
RegisterDropSites( void )
{
    static XtCallbackRec transferCB[] = { {TransferCallback, NULL},
                                               {NULL, NULL} };
    static XtCallbackRec animateCB[] = { {AnimateCallback, NULL},
                                               {NULL, NULL} };
    Arg args[3];
    int n;
    Widget clipWin;

    XtSetArg(args[0], XmNclipWindow, &clipWin);
    XtGetValues(viewport, args, 1);

    /*
     * This code makes assumptions about the order of the arguments.
     * XmNanimationStyle is assumed to be first and
     * DtNregisterChildren is assumed to be last
     */
    n = 0;
    XtSetArg(args[n], XmNanimationStyle, XmDRAG_UNDER_NONE);  n++; /* first */
    XtSetArg(args[n], DtNdropAnimateCallback, animateCB);  n++;
    XtSetArg(args[n], DtNregisterChildren, True);  n++; /* last */

    DtDndDropRegister(clipWin, DtDND_FILENAME_TRANSFER, XmDROP_COPY,
                   (XtCallbackList) transferCB, args, n);

    DtDndDropRegister(tabletBorder, DtDND_FILENAME_TRANSFER, XmDROP_COPY,
                   (XtCallbackList) transferCB, args, n);

    DtDndDropRegister(tabletFrame, DtDND_FILENAME_TRANSFER, XmDROP_COPY,
                   (XtCallbackList) transferCB, args, n);

    DtDndDropRegister(tablet, DtDND_FILENAME_TRANSFER, XmDROP_COPY,
                   (XtCallbackList) transferCB, args, n - 1);

    /*
     * Once the drag and drop library is fixed, the following calls
     * will not be necessary. Currently the dnd library does not pass on
     * Motif resource values, in this case XmNanimationStyle.
     */

    XmDropSiteUpdate(clipWin, args, 1);
    XmDropSiteUpdate(tabletBorder, args, 1);
    XmDropSiteUpdate(tabletFrame, args, 1);
    XmDropSiteUpdate(tablet, args, 1);
}

/***************************************************************************
 *                                                                         *
 * Routine:   Abort                                                        *
 *                                                                         *
 * Purpose:   Print a fatal error message and then exit.                   *
 *                                                                         *
 ***************************************************************************/

void
Abort(
        char *str )

{
  _DtSimpleError (progName, DtError, NULL, str, NULL);
  exit(-1);
}


/***************************************************************************
 *                                                                         *
 * Routine:   stat_out                                                     *
 *                                                                         *
 * Purpose:   Generate a debug message to stderr.  Flush stdout, and then  *
 *            print the message(s) to stderr and flush stderr.  By doing   *
 *            an fflush after each fprintf, we can always determin where   *
 *            in the code the process is running.  The stat_out() routine  *
 *            is invoked like printf() with up to 7 arguments.  It is      *
 *            source-code identical to the outl() routine used in the xwd  *
 *            and xwud utilities which are part of standard X11.           *
 *                                                                         *
 *X11***********************************************************************/

void
stat_out(
        char *msg,
        char *arg0,
        char *arg1,
        char *arg2,
        char *arg3,
        char *arg4,
        char *arg5,
        char *arg6 )
{
/* static char str[1024]; */

        fflush(stdout);
        fprintf(stderr, msg, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        fflush(stderr);

/*        sprintf(str, msg, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
          _DtSimpleError ("dticon-stat_out", DtError, NULL, str, NULL);*/
}

/***************************************************************************
 *                                                                         *
 * Routine:   PixelTableClear                                              *
 *                                                                         *
 * Purpose:   Reset the pixel table to force subsequent lookups to get     *
 *            new pixel information.  Do not free up space for table.      *
 *                                                                         *
 *X11***********************************************************************/
void
PixelTableClear()
{
    pixelTable.numItems = 0;
    pixelTable.lastFound = 0;
}

/***************************************************************************
 *                                                                         *
 * Routine:   PixelTableLookup                                             *
 *                                                                         *
 * Purpose:   Return index into the Pixel Table for the pixel passed in.   *
 *            If the pixel passed in isn't already in the table, it will   *
 *            be added.  This may require allocating a larger pixel table. *
 *            In order to clear the Pixel Table (which should be done each *
 *            time a new image is being processed in order to get current  *
 *            pixel data) call PixelTableClear().                          *
 *                                                                         *
 *            For performance, save the last lookup result, and check it   *
 *            first.  This should improve performance unless image is very *
 *            "dithered".  The allocNew parameter will be set if the image *
 *            was grabbed from screen.  In this case, each "screen" pixel  *
 *            will need a new pixel allocated for it.                      *
 *                                                                         *
 *X11***********************************************************************/
int
PixelTableLookup(
        Pixel pixelIn,
        Boolean allocNew )
{
    int i, grayValue;
    XColor tmpXColor;

    /** first, check to see if the last lookup was for the same pixel **/
    if (pixelTable.lastFound < pixelTable.numItems)
        if (pixelTable.item[pixelTable.lastFound].xcolor.pixel == pixelIn)
            return pixelTable.lastFound;

    /** look through table to see if there is an entry for this pixel **/
    for (i=0; i<pixelTable.numItems; i++)
    {
        if (pixelTable.item[i].xcolor.pixel == pixelIn)
        {
            pixelTable.lastFound = i;
            return i;
        }
    }

    /**                                     **/
    /** No entry for this pixel, create one **/
    /**                                     **/

    /** Allocate larger table if needed     **/
    if (pixelTable.numItems == pixelTable.pixelTableSize)
    {
        pixelTable.pixelTableSize += PIXEL_TABLE_INC;
        pixelTable.item = (PixelTableItem *)XtRealloc((char *)pixelTable.item,
                              sizeof(PixelTableItem)*pixelTable.pixelTableSize);
    }

    /** Get color information for pixelIn   **/
    i = pixelTable.numItems;
    pixelTable.numItems++;

    pixelTable.item[i].xcolor.pixel = pixelIn;
    XQueryColor(dpy, Cmap, &(pixelTable.item[i].xcolor));

    /*---          < ESTABLISH THE GREYSCALE IMAGE >            ---*/
    /*--- The NTSC formula for converting an RGB value into the ---*/
    /*--- corresponding grayscale value is:                     ---*/
    /*--- luminosity = .299 red + .587 green + .114 blue        ---*/

    grayValue = ( (int)((pixelTable.item[i].xcolor.red*299) +
                   (pixelTable.item[i].xcolor.green*587) +
                   (pixelTable.item[i].xcolor.blue*114)) / 1000) >> 8;

    if (grayValue < GAMMA_CUTOFF)
        pixelTable.item[i].mono = black_pixel;
    else
        pixelTable.item[i].mono = white_pixel;

    /** Allocate new color cell if needed (use old for mono conversion) **/
    if (allocNew)
    {
        tmpXColor.red = pixelTable.item[i].xcolor.red;
        tmpXColor.green = pixelTable.item[i].xcolor.green;
        tmpXColor.blue = pixelTable.item[i].xcolor.blue;

        if (!XAllocColor(dpy, Cmap, &tmpXColor))
        {
            /* for lack of something better, use the old color cell */
            pixelTable.item[i].newCell = pixelTable.item[i].xcolor.pixel;
            DoErrorDialog(GETSTR(10,62,"Operation failed.\nColormap is full"));
        }
        else
            pixelTable.item[i].newCell = tmpXColor.pixel;
    }

    pixelTable.lastFound = i;
    return i;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Switch_FillSolids                                            *
 *                                                                         *
 * Purpose:   Toggles the state of the global FillSolids flag, based on    *
 *            the XmNset resource for the fillToggle widget.               *
 *                                                                         *
 *X11***********************************************************************/

void
Switch_FillSolids( void )
{
  Arg args[10];
  int i;

  i = 0;
  XtSetArg(args[i], XmNset, &FillSolids); i++;
  XtGetValues(fillToggle, args, i);
  if (FillSolids)
    Set_Gfx_Labels(FILL);
  else
    Set_Gfx_Labels(HOLLOW);
#ifdef DEBUG
  if (debug)
    stat_out("Fill_Solids toggle = %s\n", (FillSolids? "True" : "False"));
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Select_New_Pen                                               *
 *                                                                         *
 * Purpose:   Changes the fg color of the pen GC, based on the parameter   *
 *            passed in.                                                   *
 *                                                                         *
 *X11***********************************************************************/

int
Select_New_Pen(
        int n )
{
  int new_block, new_pen;

  if (n < BG_COLOR) {
    new_block = STATIC_COLOR;
    new_pen   = n;
   }
  else {
    new_block = DYNAMIC_COLOR;
    new_pen   = n - BG_COLOR;
   }
#ifdef DEBUG
  if (debug) {
    stat_out("**** n = %d, new_block = %d, new_pen = %d\n", n,
                        new_block, new_pen);
   }
#endif

/*** if the new choice is the current pen, re-set it and return ***/
  if ((new_block == ColorBlock) && (new_pen == CurrentColor)) {
    if (new_block == STATIC_COLOR)
      XmToggleButtonSetState(StaticWid[new_pen], True, False);
    else
      XmToggleButtonSetState(DynamicWid[new_pen], True, False);
    return;
   }

/*** un-set the previous choice ***/
  if (ColorBlock == STATIC_COLOR)
    XmToggleButtonSetState(StaticWid[CurrentColor], False, False);
  else
    XmToggleButtonSetState(DynamicWid[CurrentColor], False, False);

#ifdef DEBUG
  if (debug)
    stat_out("  New pen color = %d\n", n);
#endif

  ColorBlock = new_block;       /*** STATIC or DYNAMIC? ***/
  CurrentColor = new_pen;       /*** index w/i the appropriate block ***/

  XSetForeground(dpy, Color_gc,
        (ColorBlock ? DynamicPen[CurrentColor] : StaticPen[CurrentColor]));
  XSetForeground(dpy, Mono_gc,
        (ColorBlock ? DynamicMono[CurrentColor] : StaticMono[CurrentColor]));
}


/***************************************************************************
 *                                                                         *
 * Routine:   Backup_Icons                                                 *
 *                                                                         *
 * Purpose:   Copy the current contents of the color and mono icons to     *
 *            their undo storage areas, just prior to modifying them with  *
 *            the current graphics operation.                              *
 *                                                                         *
 *X11***********************************************************************/

void
Backup_Icons( void )
{
/*** If we're backing up the tablet contents, it's dirty ***/
/*** and may need to be saved to file at some point.     ***/

  Dirty = True;

/*** if the icon sizes don't match the backup sizes, or  ***/
/*** either of the backup icons don't exist, create them ***/

  if ((icon_width != backup_width)   ||
      (icon_height != backup_height) ||
      (!prev_color_icon) ||
      (!prev_mono_icon)) {
    if (prev_color_icon)
      XFreePixmap(dpy, prev_color_icon);
    if (prev_mono_icon)
      XFreePixmap(dpy, prev_mono_icon);
    prev_color_icon = XCreatePixmap(dpy, root, icon_width, icon_height,
                                DefaultDepth(dpy, screen));
    prev_mono_icon = XCreatePixmap(dpy, root, icon_width, icon_height,
                                DefaultDepth(dpy, screen));
    backup_width = icon_width;
    backup_height = icon_height;
   }

/*** now, copy the color and mono pixmap to the backup pixmaps ***/

  XCopyArea(dpy, color_icon, prev_color_icon,
        Color_gc, 0, 0, icon_width, icon_height, 0, 0);
  XCopyArea(dpy, mono_icon, prev_mono_icon,
        Mono_gc, 0, 0, icon_width, icon_height, 0, 0);
  UndoFlag = True;
  XtSetSensitive( editMenu_undo_pb, True);
}


/***************************************************************************
 *                                                                         *
 * Routine:   DoErrorDialog                                                *
 *                                                                         *
 * Purpose:   Some error has just occurred in the application.  Pop up     *
 *            the error dialog and display the message passed in.          *
 *                                                                         *
 *X11***********************************************************************/

void
DoErrorDialog(
        char *str )
{
  int i;
  Arg arg[10];
  XmString local_str;

  i = 0;
  local_str = XmStringCreateLocalized(str);
  XtSetArg(arg[i], XmNmessageString, local_str);
  i++;
  XtSetValues(stdErrDialog, arg, i);
  XmStringFree(local_str);
  XtManageChild(stdErrDialog);
}


/***************************************************************************
 *                                                                         *
 * Routine:   DoQueryDialog                                                *
 *                                                                         *
 * Purpose:   The user should be prompted on an action they're attempting. *
 *            Pop up the query dialog and display the message passed in.   *
 *                                                                         *
 *X11***********************************************************************/

void
DoQueryDialog(
        char *str )
{
  int i;
  Arg arg[10];
  XmString local_str;
  static Widget w=NULL;

  XtPopup(dtIconShell, XtGrabNone);
  XMapRaised(XtDisplay(dtIconShell), XtWindow(dtIconShell));

  if (!w)
    w = XmMessageBoxGetChild(queryDialog, XmDIALOG_CANCEL_BUTTON);

  i = 0;
  local_str = XmStringCreateLocalized(str);
  XtSetArg(arg[i], XmNmessageString, local_str); i++;
  XtSetValues(queryDialog, arg, i);
  XmStringFree(local_str);
  XtManageChild(queryDialog);

  XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Do_GrabOp                                                    *
 *                                                                         *
 * Purpose:   Switch to GRAB mode.  Grab the server and the pointer until  *
 *            the user has made a selection from the screen.  The server   *
 *            and pointer are ungrabbed in Do_ButtonOp().                  *
 *                                                                         *
 *      Note: Moved the server grab until the mouse button is pressed      *
 *            (beginning the screen grab) in order to give the windows     *
 *            time to repaint.  This was needed specifically for the       *
 *            queryDialog used when the icon image is "Dirty".             *
 *                                                                         *
 *X11***********************************************************************/

void
Do_GrabOp( void )
{
  Backup_G_Op = GraphicsOp;
  GraphicsOp  = S_GRAB;
  cursor = XCreateFontCursor(dpy, XC_crosshair);
  XGrabPointer(dpy, root, False,
                 ButtonPressMask|PointerMotionMask|ButtonReleaseMask,
                 GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);
}


/***************************************************************************
 *                                                                         *
 * Routine:   LoadGrabbedImage                                             *
 *                                                                         *
 * Purpose:   Given an [x,y] coordinate, plus width and height, grab an    *
 *            XImage from the Root window and load it into the icon        *
 *            editor.                                                      *
 *                                                                         *
 *X11***********************************************************************/
int
LoadGrabbedImage(
        int x,
        int y,
        int width,
        int height )
{
  XImage *img, *mono_img;
  int i, j;
  int pixelTableIndex;

#ifdef DEBUG
  if (debug)
    stat_out("LoadGrabbedImage: [%d,%d] - width: %d,  height: %d\n",
                x, y, width, height);
#endif

  img = NULL;

  if (width > 0 && height > 0)
    img = XGetImage(dpy, root, x, y, width, height, AllPlanes, format);

  XUngrabPointer (dpy, CurrentTime);
  XUngrabServer (dpy);
  XSync (dpy, False);

  if (!img) {
    DoErrorDialog( GETSTR(10,60, "Screen image capture failed") );
    return(False);
  }

/*********************************************************************
 * OK, now that we've got the image, we need to convert the entire   *
 * thing to pixels that the icon editor won't lose control over.     *
 * To do this, we find all the pixel values in the image and request *
 * equivalent read-only colors in the colormap (we won't be drawing  *
 * with any pen colors we don't already have).  Once the requests to *
 * the Cmap have been honored, we go thru the image one more time    *
 * and replace each old pixel with the correct new value.            *
 * NOTE: Old code assumed 256 max colors... NOT OKAY.  Added dynamic *
 *       pixel table for color lookup, which also handles allocating *
 *       the new read-only color. - julie                            *
 *********************************************************************/

/*********************************************************************
 * Dup the XImage so there'll be two identical copies: one to        *
 * update with the new color values, and one with which to create    *
 * a monochrome equivalent.                                          *
 *********************************************************************/
  mono_img = XSubImage(img, 0, 0, img->width, img->height);
  if (!mono_img) {
    DoErrorDialog( GETSTR(10,60, "Screen image capture failed") );
    return(False);
   }

  /* force new pixel lookup, in case pixel data has changed */
  PixelTableClear();
  for (i=0; i<img->width; i++)
  {
    for (j=0; j<img->height; j++)
    {
      pixelTableIndex = PixelTableLookup(XGetPixel(img, i, j), True);
      XPutPixel(img, i, j, PIXEL_TABLE_NEW_CELL(pixelTableIndex));
      XPutPixel(mono_img, i, j, PIXEL_TABLE_MONO(pixelTableIndex));
    }
  }

  Backup_Icons();
  Init_Icons(img->width, img->height, DO_NOT_SAVE);
  XPutImage(dpy, color_icon, Color_gc, img, 0, 0, 0, 0,
                        img->width, img->height);
  XPutImage(dpy, mono_icon, Mono_gc, mono_img, 0, 0, 0, 0,
                        img->width, img->height);

  XDestroyImage(img);
  XDestroyImage(mono_img);

  return(True);
}


/***************************************************************************
 *                                                                         *
 * Routine:   ParseAppArgs                                                 *
 *                                                                         *
 * Purpose:   Parse the invocation arguments, looking for '-f' and '-x'.   *
 *            This routine is invoked once by dticon.                     *
 *                                                                         *
 *            Each of the args should be accompanied by a parameter.  If   *
 *            one does not exist, abort dticon with the appropriate       *
 *            error message.  Otherwise, store the parameter in a global   *
 *            variable for processing by ProcessAppArgs() (called once     *
 *            from event.c).                                               *
 *                                                                         *
 *X11***********************************************************************/

void
ParseAppArgs(
        int num,
        char *cmd[] )
{
  int i;
  char *cmd_arg;

#ifdef DEBUG
  if (debug)
    stat_out("Entering ParseAppArgs\n");
#endif

  for (i=0; i<NUM_PARAMS; i++)
    param_flag[i] = False;
  argsNeedProcessed = False;

/*** First, figure out which arguments get used ***/

  /* don't process command line args if restoring from a session file */
  if (xrdb.session != NULL)
    argsNeedProcessed = True;
  else
  {
    for (i=1; i<num; i++)
    {
      cmd_arg = cmd[i];

      if (cmd_arg[0] == '-')          /* process command line switches */
      {
        switch (cmd_arg[1])
        {
            case 'f':
              if (++i < num) {
                if (strncpy(start_file, cmd[i], 255)) {
                  param_flag[AUTO_FILE] = True;
                  argsNeedProcessed = True;
                }
                else
                  Abort(GETSTR(10,52, "Invalid use of the '-f' parameter"));
              }
              else
                Abort(GETSTR(10,52, "Invalid use of the '-f' parameter"));
              continue;
            case 'x': if (++i < num)
              {
                if (strcpy(start_size, cmd[i])) {
                  param_flag[AUTO_SIZE] = True;
                  argsNeedProcessed = True;
                }
                else
                  Abort(GETSTR(10,54, "Invalid use of the '-x' parameter"));
              }
              else
                Abort(GETSTR(10,54, "Invalid use of the '-x' parameter"));
              continue;
        } /* switch */
      } /* if */
    } /* for */
  } /* if */


#ifdef DEBUG
  if (debug) {
    for (i=0; i<NUM_PARAMS; i++)
      stat_out("  param_flag = %s\n", (param_flag[i] ? "True" : "False"));
    stat_out("Leaving ParseAppArgs\n");
   }
#endif

}


/***************************************************************************
 *                                                                         *
 * Routine:   ProcessAppArgs                                               *
 *                                                                         *
 * Purpose:   Process the invocation arguments, '-f'and '-x'               *
 *                                                                         *
 *            If '-f' exists, the following parameter should be the name   *
 *            of a file to load at application start-up.  Invoke the       *
 *            Read_File() routine with that parameter.                     *
 *                                                                         *
 *            If '-x' exists, the following parameter should be the        *
 *            initial geometry for the icon tablet (work area).  Parse     *
 *            the string with XParseGeometry and pass the results (if      *
 *            valid) to Eval_NewSize().                                    *
 *                                                                         *
 *X11***********************************************************************/

#define MASK (WidthValue & HeightValue)

void
ProcessAppArgs( void )
{
  int result;
  int x_ret, y_ret;

#ifdef DEBUG
  if (debug)
    stat_out("Entering ProcessAppArgs\n");
#endif

  argsNeedProcessed = False;

/*** Attempt to honor the arguments, in order of priority            ***/
/*** ('-session', '-f', '-x', in that order).                        ***/
/***                                                                 ***/
/*** GetSessionInfo() will already set start_file if needed          **/

  if ( param_flag[AUTO_FILE] || (session.useSession && start_file[0] != '\0') )
  {
    fileIOMode = FILE_READ;
    if (!Read_File(start_file))
      DoErrorDialog( GETSTR(16,2,
                    "The file cannot be accessed\nor contains invalid data") );
    else {
      if (successFormat == FORMAT_XPM) {
        X_Hot = xpm_ReadAttribs.x_hotspot;
        Y_Hot = xpm_ReadAttribs.y_hotspot;
        Display_XPMFile(xpm_ReadAttribs.width, xpm_ReadAttribs.height);
      }
      else if (successFormat == FORMAT_XBM) {
        X_Hot = x_hot;
        Y_Hot = y_hot;
        Display_XBMFile(width_ret, height_ret);
      }
    } /* else */
  }
  else if(param_flag[AUTO_SIZE]) {
    result = XParseGeometry(start_size, &x_ret, &y_ret,
                            &width_ret, &height_ret);
    if ((!MASK) & result)
      Abort(GETSTR(10,64, "Invalid dimension parameter"));
    DialogFlag = NEW;
    Eval_NewSize(width_ret, height_ret);
  }

#ifdef DEBUG
  if (debug)
    stat_out("Leaving ProcessAppArgs\n");
#endif

}


/***************************************************************************
 *                                                                         *
 * Routine:   Set_Gfx_Labels                                               *
 *                                                                         *
 * Purpose:   Set the label pixmaps of the 'Rectangle', 'Polygon',         *
 *            'Circle', and 'Ellipse' graphics tool toggle to either       *
 *            Solid or Hollow, depending on the flag passed in.            *
 *                                                                         *
 *X11***********************************************************************/

void
Set_Gfx_Labels(
        Boolean flag )
{
  Arg args[10];
  int i;

  if (flag == HOLLOW) {
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, rectPix); i++;
    XtSetValues(rectangleButton, args, i);
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, circlePix); i++;
    XtSetValues(circleButton, args, i);
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, polygonPix); i++;
    XtSetValues(polygonButton, args, i);
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, ellipsePix); i++;
    XtSetValues(ellipseButton, args, i);
   }
  else {
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, rectSolidPix); i++;
    XtSetValues(rectangleButton, args, i);
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, circleSolidPix); i++;
    XtSetValues(circleButton, args, i);
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, polygonSolidPix); i++;
    XtSetValues(polygonButton, args, i);
    i = 0;
    XtSetArg(args[i], XmNlabelPixmap, ellipseSolidPix); i++;
    XtSetValues(ellipseButton, args, i);
   }
}


/***************************************************************************
 *                                                                         *
 * Routine:   jskXerrorDebug                                               *
 *                                                                         *
 * Purpose:   This routine takes the place of the default non-fatal error  *
 *            handler normally used by the X server.  If an error occurs,  *
 *            this routine simply stores the error_code in the global      *
 *            variable XErrorFlag (making it available to other sections   *
 *            of the application).  Then it returns.                       *
 *                                                                         *
 ***************************************************************************/
#define MAX_MSG_STR   1024

static int jskXerrorDebug(disp, error_event)
Display *disp;
XErrorEvent *error_event;
{
    char error_msg[MAX_MSG_STR];

#ifdef DEBUG
    if (debug)
        stat_out("\n\nX Protocol Error:\n");

    _DtPrintDefaultErrorSafe(disp, error_event, error_msg, MAX_MSG_STR);
    _DtSimpleError (progName, DtWarning, NULL, error_msg, NULL);
#endif /* DEBUG */

    return (TRUE);
}


/***************************************************************************
 *                                                                         *
 * Routine:   jskXerrorIODebug                                             *
 *                                                                         *
 * Purpose:   This routine is needed in order to get good bfa (bba) stats  *
 **************************************************************************/
static int jskXerrorIODebug(disp)
Display *disp;
{
    exit (-1);
    return (TRUE);
}


/***************************************************************************
 *                                                                         *
 * Routine:   SaveSession                                                  *
 *                                                                         *
 * Purpose:   save state information for session management                *
 **************************************************************************/
void
SaveSession( void )
{
    char *path, *name;
    int fd, n;
    char *xa_CommandStr[3];
    char *tmpStr, *tmpStr2;
    Position x,y;
    Dimension width, height;
    char bufr[1024];        /* make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;
    WM_STATE *wmState;
    Atom wmStateAtom, actualType;
    int actualFormat;
    unsigned long nitems, leftover;


#ifdef DEBUG
  if (debug)
    stat_out("SaveSession\n");
#endif

    DtSessionSavePath(dtIconShell, &path, &name);

    /*  Create the session file  */
    if ((fd = creat(path, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)) == -1)
    {
        tmpStr = GETSTR(16,24, "Couldn't save session to file");
        tmpStr2 = (char *)XtMalloc(strlen(tmpStr) + strlen(path)+ 3);
        sprintf(tmpStr2, "%s: %s\n", tmpStr, path);
        _DtSimpleErrnoError(progName, DtError, NULL, tmpStr2, NULL);
        XtFree(tmpStr2);
        XtFree ((char *)path);
        XtFree ((char *)name);
        return;
    }


    /*  Getting the WM_STATE property to see if iconified or not */

    wmStateAtom = XInternAtom(dpy, "WM_STATE", False);

    XGetWindowProperty (dpy, XtWindow(dtIconShell), wmStateAtom, 0L,
                        (long)BUFSIZ, False, wmStateAtom, &actualType,
                        &actualFormat, &nitems, &leftover,
                        (unsigned char **) &wmState);

    if (wmState->state == IconicState)
        sprintf(bufr, "*iconic: True\n");
    else
        sprintf(bufr, "*iconic: False\n");

    /*** Get and write out the geometry info for our Window ***/

    x = XtX(dtIconShell);
    y = XtY(dtIconShell);
    width = XtWidth(dtIconShell);
    height = XtHeight(dtIconShell);

    /* Modify x & y to take into account window mgr frames
     * This is pretty bogus, but I don't know a better way to do it.
     */
    extData = _XmGetWidgetExtData(dtIconShell, XmSHELL_EXTENSION);
    vendorExt = (XmVendorShellExtObject)extData->widget;
    x -= vendorExt->vendor.xOffset;
    y -= vendorExt->vendor.yOffset;

    sprintf(bufr, "%s*x: %d\n", bufr, x);
    sprintf(bufr, "%s*y: %d\n", bufr, y);
    sprintf(bufr, "%s*width: %d\n", bufr, width);
    sprintf(bufr, "%s*height: %d\n", bufr, height);
    if (last_fname[0] != '\0')
        sprintf(bufr, "%s*file: %s\n", bufr, last_fname);

    write (fd, bufr, strlen(bufr));


    n = 0;
    xa_CommandStr[n] = execName;    n++;
    xa_CommandStr[n] =  "-session"; n++;
    xa_CommandStr[n] = name;        n++;

    XSetCommand(dpy, XtWindow(dtIconShell), xa_CommandStr, n);
    XtFree ((char *)path);
    XtFree ((char *)name);

    /*  Don't exit yet, SM needs time to get the new commandStr.*/
}


/***************************************************************************
 *                                                                         *
 * Routine:   GetSessionInfo                                               *
 *                                                                         *
 * Purpose:   get dticon session information                              *
 **************************************************************************/
#define DEFAULT_WIDTH  536
#define DEFAULT_HEIGHT 477

void
GetSessionInfo( void )
{
    XrmDatabase db;
    char *tmpStr, *tmpStr2;
    char *path;
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

#ifdef DEBUG
  if (debug)
    stat_out("GetSessionInfo\n");
#endif

    if (xrdb.session == NULL)
    {
        session.useSession = False;
        return;
    }
    else
        session.useSession = True;

    /***  Open the resource database file ***/

    /* TopLevel is used because dtIconShell isn't created yet...     */
    /* okay because it only uses it to get a display, not a window    */
    if (DtSessionRestorePath(TopLevel, &path, xrdb.session) == False)
        path = xrdb.session;

    if ((db = XrmGetFileDatabase (path)) == NULL)
    {
        tmpStr = GETSTR(16,22, "Couldn't restore session from file");
        tmpStr2 = (char *)XtMalloc(strlen(tmpStr) + strlen(path)+ 3);
        sprintf(tmpStr2, "%s: %s\n", tmpStr, path);
        _DtSimpleErrnoError(progName, DtError, NULL, tmpStr2, NULL);
        XtFree(tmpStr2);
        if (path != xrdb.session)
            XtFree(path);
        session.useSession = False;
        return;
    }
    if (path != xrdb.session)
        XtFree(path);


    /*** now get the information we want from the database ***/
    /*** make sure values are at least somewhat reasonable ***/

    xrm_name[1] = '\0';

    /* get x position */
    xrm_name[0] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        session.x = (Position)atoi((char *)value.addr);
    else
        session.x = 100;
    if (session.x < 0) session.x = 0;

    /* get y position */
    xrm_name[0] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        session.y = (Position)atoi((char *)value.addr);
    else
        session.y = 100;
    if (session.y < 0) session.y = 0;

    /* get width */
    xrm_name[0] = XrmStringToQuark ("width");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        session.width = (Dimension)atoi((char *)value.addr);
    else
        session.width = DEFAULT_WIDTH;
    if (session.width < DEFAULT_WIDTH) session.width = DEFAULT_WIDTH;

    /* get height */
    xrm_name[0] = XrmStringToQuark ("height");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
        session.height = (Dimension)atoi((char *)value.addr);
    else
        session.height = DEFAULT_HEIGHT;
    if (session.height < DEFAULT_HEIGHT) session.height = DEFAULT_HEIGHT;

    /* get iconic state */
    xrm_name[0] = XrmStringToQuark ("iconic");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
    {
        if ( value.addr!=NULL && strcmp((char *)value.addr, "True")==0 )
            session.iconicState = IconicState;
        else
            session.iconicState = NormalState;
    }

    /* get file name */
    xrm_name[0] = XrmStringToQuark ("file");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value))
    {
        strncpy(start_file, value.addr, 255);
    }
    else
        start_file[0] = '\0';
}


/***************************************************************************
 *                                                                         *
 * Routine:   ChangeTitle                                                  *
 *                                                                         *
 * Purpose:   Put the file name in the window title...                     *
 ***************************************************************************/
void
ChangeTitle(
        )
{
    static char *dialogTitle = NULL;
    Arg al[10];
    int ac;
    char *name;
    char *title;
    char *tmpStr;
    Boolean freeName;

    if (!dialogTitle)
    {
        name = GETSTR(12, 1, "Icon Editor");
        dialogTitle = XtMalloc (strlen(name) + 4);
        sprintf(dialogTitle, "%s - ", name);
    }

    if (last_fname && *last_fname)
    {
        if (name = strrchr(last_fname, '/'))
            name++;
        else
            name = last_fname;
        freeName = False;
    }
    else
    {
        tmpStr = GETSTR(10, 66, "(UNTITLED)");
        name = XtNewString(tmpStr);
        freeName = True;
    }

    title = XtMalloc (strlen(dialogTitle) + strlen(name) + 1);
    sprintf(title, "%s%s", dialogTitle, name);

    ac = 0;
    XtSetArg(al[ac], XmNtitle, title); ac++;
    XtSetArg(al[ac], XmNiconName, name); ac++;
    XtSetValues(dtIconShell, al, ac);
    if(freeName == True)
        XtFree(name);
    XtFree(title);
}
