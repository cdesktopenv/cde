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
/* $XConsortium: image.c /main/4 1995/11/02 14:05:39 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company	
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/******************************************************************************
 **  Program:		dticon
 **
 **  Description:	X11-based multi-color icon editor
 **
 **  File:		Image.c, containing the following subroutines/functions:
 **                       Mirror_Image()
 **                       Block_Rotate()
 **                       Scale_Image()
 **                       Flood_Region()
 **                       Flood_Fill()
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
#include "externals.h"

int flood_min_x, flood_min_y, flood_max_x, flood_max_y;


/***************************************************************************
 *                                                                         *
 * Routine:   Mirror_Image                                                 *
 *                                                                         *
 * Purpose:   Pick "horizontal" or "vertical" from a submenu.  Then pick   *
 *            the rectangle to be flopped.  Create a mirror image (either  *
 *            top-to-bottom or left-to-right) and prompt the user for      *
 *            placement of the result.                                     *
 *                                                                         *
 *X11***********************************************************************/

int
Mirror_Image(
        int orientation )
{
  XRectangle tmp_box;
  XImage *new_image, *old_image, *new_mono, *old_mono;
  unsigned long n;
  int i, j;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Mirror_Image\n");
#endif

  if (!Selected)
    return (False);
/*--- get src. & dst. images from both color and monochrome icons ---*/

  old_image = XGetImage(dpy, color_icon, select_box.x, select_box.y,
		select_box.width, select_box.height, AllPlanes, format);
  if (old_image == NULL)
    return (False);
  new_image  = XGetImage(dpy, color_icon, select_box.x, select_box.y,
		select_box.width, select_box.height, AllPlanes, format);
  if (new_image == NULL) {
    XDestroyImage(old_image);
    return (False);
   }

  old_mono = XGetImage(dpy, mono_icon, select_box.x, select_box.y,
		select_box.width, select_box.height, AllPlanes, format);
  if (old_mono == NULL) {
    XDestroyImage(old_image);
    XDestroyImage(new_image);
    return (False);
   }
  new_mono  = XGetImage(dpy, mono_icon, select_box.x, select_box.y,
		select_box.width, select_box.height, AllPlanes, format);
  if (new_mono == NULL) {
    XDestroyImage(old_image);
    XDestroyImage(new_image);
    XDestroyImage(old_mono);
    return (False);
   }

#ifdef DEBUG
  if (debug)
    stat_out("  - got the images\n");
#endif

  for (i=0; i<(int)select_box.width; i++)
    for (j=0; j<(int)select_box.height; j++) {
      if (orientation == VERTICAL) {
	n = XGetPixel(old_image, i, j);
	XPutPixel(new_image, i, (select_box.height-1)-j, n);
	n = XGetPixel(old_mono, i, j);
	XPutPixel(new_mono, i, (select_box.height-1)-j, n);
       }
      else {
	n = XGetPixel(old_image, i, j);
	XPutPixel(new_image, (select_box.width-1)-i, j, n);
	n = XGetPixel(old_mono, i, j);
	XPutPixel(new_mono, (select_box.width-1)-i, j, n);
       }
     } /* for(j...) */
  XFlush(dpy);

  XPutImage(dpy, color_icon, Color_gc, new_image, 0, 0,
		select_box.x, select_box.y,
		select_box.width, select_box.height);
  XPutImage(dpy, XtWindow(iconImage), Color_gc, new_image, 0, 0,
		select_box.x, select_box.y,
		select_box.width, select_box.height);

  XPutImage(dpy, mono_icon, Mono_gc, new_mono, 0, 0,
		select_box.x, select_box.y,
		select_box.width, select_box.height);
  XPutImage(dpy, XtWindow(monoImage), Mono_gc, new_mono, 0, 0,
		select_box.x, select_box.y,
		select_box.width, select_box.height);
  Transfer_Back_Image(select_box.x, select_box.y,
			(select_box.x+select_box.width-1),
			(select_box.y+select_box.height-1),
			FILL);

  XDestroyImage(new_image);
  XDestroyImage(old_image);
  XDestroyImage(new_mono);
  XDestroyImage(old_mono);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Mirror_Image\n");
#endif

  return (True);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Block_Rotate                                                 *
 *                                                                         *
 * Purpose:   Given source and destination pixmaps of the correct size,    *
 *            and the type of rotation to do, do a block rotation (90      *
 *            degrees clockwise or counterclockwise) from the source to    *
 *            the destination.                                             *
 *                                                                         *
 *X11***********************************************************************/

int
Block_Rotate(
        XImage *src_image,
        XImage *dst_image,
        int rtype )
{
  int i, j, width, height;
  unsigned long n;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Block_Rotate\n");
#endif

  width  = src_image->width;
  height = src_image->height;

  switch (rtype) {
    case ROTATE_L   : for (i=0; i<width; i++)
			for (j=0; j<height; j++) {
			  n = XGetPixel(src_image, i, j);
			  XPutPixel(dst_image, j, (width-1)-i, n);
			 }
		      break;
    case ROTATE_R   : for (i=0; i<width; i++)
			for (j=0; j<height; j++) {
			  n = XGetPixel(src_image, i, j);
			  XPutPixel(dst_image, (height-1)-j, i, n);
			 }
		      break;
   } /* switch */
  XFlush(dpy);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Block_Rotate\n");
#endif

  return (True);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Scale_Image                                                  *
 *                                                                         *
 * Purpose:   Given a scr. and dst. XImage pair, scale the src. image to   *
 *            the size of the dst. image.                                  *
 *                                                                         *
 *X11***********************************************************************/

void
Scale_Image( void )
{
  XImage *old_img, *old_mono;
  int  old_x, old_y, new_x, new_y;
  int  old_width, old_height, new_width, new_height;
  int min_x, min_y, max_x, max_y;
   
#ifdef DEBUG
  if (debug)
    stat_out("Entering Scale_Image\n");
#endif

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

  old_img = XGetImage(dpy, color_icon, select_box.x, select_box.y,
			select_box.width, select_box.height,
			AllPlanes, format);
  old_mono = XGetImage(dpy, mono_icon, select_box.x, select_box.y,
			select_box.width, select_box.height,
			AllPlanes, format);

  Scale = XGetImage(dpy, color_icon, min_x, min_y,
			(max_x-min_x+1), (max_y-min_y+1), AllPlanes, format);
  Scale_mono = XGetImage(dpy, mono_icon, min_x, min_y,
			(max_x-min_x+1), (max_y-min_y+1), AllPlanes, format);

  old_width  = old_img->width;
  old_height = old_img->height;
  new_width  = Scale->width;
  new_height = Scale->height;
  for (new_y=0;  new_y<new_height;  new_y++) {
    old_y = (old_height * new_y) / new_height;
    for (new_x=0;  new_x<new_width;  new_x++) {
      old_x = (old_width * new_x) / new_width;
      XPutPixel(Scale, new_x, new_y, XGetPixel(old_img, old_x, old_y));
      XPutPixel(Scale_mono, new_x, new_y, XGetPixel(old_mono, old_x, old_y));
    }
  }

  XDestroyImage(old_img);
  XDestroyImage(old_mono);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Scale_Image\n");
#endif

}


/***************************************************************************
 *                                                                         *
 * Routine:   Flood_Region                                                 *
 *                                                                         *
 * Purpose:   Pick the (rectangular) region to be flooded by a new color.  *
 *            Then pick an old color (pixel) in that region to be replaced *
 *            by the new color (left button = current foreground, right    *
 *            button = current background).                                *
 *                                                                         *
 *X11***********************************************************************/

int
Flood_Region(
        int flood_x,
        int flood_y )
{
  XImage *ImagePix, *MonoPix;
  unsigned long new_pixel, new_mono;

#ifdef DEBUG
  int i, j;
  unsigned long old_pixel;

  if (debug)
    stat_out("Entering Flood_Region\n");
#endif

/*--- get the image from the (adjusted) box ---*/

  ImagePix = XGetImage(dpy, color_icon, 0, 0, icon_width, icon_height,
		AllPlanes, format);
  if (ImagePix == NULL)
    return (False);
  MonoPix = XGetImage(dpy, mono_icon, 0, 0, icon_width, icon_height,
		AllPlanes, format);
  if (MonoPix == NULL)
    return (False);

#ifdef DEBUG
  if (debug)
    stat_out("  - got the image\n");
#endif

  if (ColorBlock == STATIC_COLOR) {
    new_pixel = StaticPen[CurrentColor];
    new_mono  = StaticMono[CurrentColor];
   }
  else {
    new_pixel = DynamicPen[CurrentColor];
    new_mono  = DynamicMono[CurrentColor];
   }

#ifdef DEBUG
  if (debug) {
    for (i=0; i<icon_width; i++)
      for (j=0; j<icon_height; j++) {
	old_pixel = XGetPixel(ImagePix, i, j);
	if ((old_pixel < 0) || (old_pixel > 255))
	  stat_out("      BAD PIXEL VALUE (%d) AT [%d,%d]\n", old_pixel, i, j);
       }
    stat_out("  SUCCESSFULLY accessed each pixel in the image\n");
   }
#endif

  flood_min_x = icon_width;
  flood_min_y = icon_height;
  flood_max_x = 0;
  flood_max_y = 0;

  Flood_Fill(ImagePix, MonoPix, flood_x, flood_y, ImagePix->width,
	ImagePix->height, new_pixel, new_mono);
  XFlush(dpy);

  Backup_Icons();
  XPutImage(dpy, color_icon, Color_gc, ImagePix, 0, 0, 0, 0,
		icon_width, icon_height);
  XPutImage(dpy, XtWindow(iconImage), Color_gc, ImagePix,
		0, 0, 0, 0, icon_width, icon_height);
  XPutImage(dpy, mono_icon, Mono_gc, MonoPix, 0, 0, 0, 0,
		icon_width, icon_height);
  XPutImage(dpy, XtWindow(monoImage), Mono_gc, MonoPix,
		0, 0, 0, 0, icon_width, icon_height);
  Transfer_Back_Image(flood_min_x, flood_min_y,
			flood_max_x, flood_max_y, FILL);
  XDestroyImage(ImagePix);
  XDestroyImage(MonoPix);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Flood_Region\n");
#endif

  return (True);

}


/***************************************************************************
 *                                                                         *
 * Routine:   Set_FloodLimits                                              *
 *                                                                         *
 * Purpose:   Given the current [x,y] of a pixel about to be modified by   *
 *            a flood-fill operation, compare it's location against the    *
 *            limits of the area already affected by the flood-fill.  If   *
 *            the pixel is outside the already modified area, adjust the   *
 *            flood_min_x, flood_min_y, flood_max_x, and flood_min_y, so   *
 *            that the current pixel is within the area defined by those   *
 *            four variables.  When the flood-fill is completed, the       *
 *            final values for those four variables will be used (by the   *
 *            Transfer_Back_Image() call in Flood_Region()) to minimize    *
 *            the size of the sub-image tranferred back to the tablet      *
 *            from the color icon pixmap.  This process slows down the     *
 *            actual flooding operation, but can significantly speed up    *
 *            the transfer_back operation, so there is a net performance   *
 *            gain (potentially, a large one).                             *
 *                                                                         *
 *X11***********************************************************************/

void
Set_FloodLimits(
        int x,
        int y )
{
  if (x < flood_min_x)
    flood_min_x = x;
  if (x > flood_max_x)
    flood_max_x = x;
  if (y < flood_min_y)
    flood_min_y = y;
  if (y > flood_max_y)
    flood_max_y = y;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Flood_Fill                                                   *
 *                                                                         *
 * Purpose:   Interatively examine each pixel within a bounded area,       *
 *            replacing the old-colored pixels encountered with            *
 *            new-colored pixels.                                          *
 *                                                                         *
 ***************************************************************************
 * one page seed fill program, 1 channel frame buffer version              *
 *                                                                         *
 * doesn't read each pixel twice like the BASICFILL algorithm in           *
 *	Alvy Ray Smith, "Tint Fill", SIGGRAPH '79                          *
 *                                                                         *
 * Paul Heckbert                   13 Sept 1982, 28 Jan 1987               *
 * PIXAR                           415-499-3600                            *
 * P.O. Box 13719                  UUCP: {sun,ucbvax}!pixar!ph             *
 * San Rafael, CA 94913            ARPA: ph%pixar.uucp@ucbvax.berkeley.edu *
 *X11***********************************************************************/

/*
 * segment of scan line y for xl<=x<=xr was filled,
 * now explore adjacent pixels in scan line y+dy
 */
struct seg {short y, xl, xr, dy;};
/*********************************************************
#define MAX 10000
*********************************************************/
#define MAX 20000		/* max depth of stack */

#define PUSH(Y, XL, XR, DY) \
    if (sp<stack+MAX && Y+(DY)>=0 && Y+(DY)<height) \
    {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}

#define POP(Y, XL, XR, DY) \
    {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

#ifdef DEBUG
  int local_debug=False, p_cnt;
#endif

int
Flood_Fill(
        XImage *color_image,
        XImage *mono_image,
        int x,
        int y,
        int width,
        int height,
        unsigned long new_pixel,
        unsigned long new_mono )
{
  int l, x1, x2, dy;
  unsigned long old_pixel;
  struct seg stack[MAX], *sp = stack;

  old_pixel = XGetPixel(color_image, x, y);	/* read pv at seed point */
  if (old_pixel==new_pixel || x<0 || x>width || y<0 || y>height) return 0;
  PUSH(y, x, x, 1);			/* needed in some cases */
  PUSH(y+1, x, x, -1);		/* seed segment (popped 1st) */

#ifdef DEBUG
  if (local_debug)
    p_cnt = 0;
#endif
  while (sp>stack) {
    /* pop segment off stack and fill a neighboring scan line */
    POP(y, x1, x2, dy);
    for (x=x1; x>=0 && XGetPixel(color_image, x, y)==old_pixel; x--)
	{
#ifdef DEBUG
  if (local_debug) {
    stat_out("+[%d,%d] ", x, y);
    p_cnt++;
    if (p_cnt == 8) {
      stat_out("\n");
      p_cnt = 0;
     }
   }
#endif
	Set_FloodLimits(x, y);
        XPutPixel(color_image, x, y, new_pixel);
        XPutPixel(mono_image, x, y, new_mono);
	}
    if (x>=x1) goto skip;
    l = x+1;
    if (l<x1) PUSH(y, l, x1-1, -dy);		/* leak on left? */
    x = x1+1;
    do {
        for (; x<width && XGetPixel(color_image, x, y)==old_pixel; x++)/**TAG**/
	{
#ifdef DEBUG
  if (local_debug) {
    stat_out("-[%d,%d] ", x, y);
    p_cnt++;
    if (p_cnt == 8) {
      stat_out("\n");
      p_cnt = 0;
     }
   }
#endif
	Set_FloodLimits(x, y);
    	XPutPixel(color_image, x, y, new_pixel);
    	XPutPixel(mono_image, x, y, new_mono);
	}
        PUSH(y, l, x-1, dy);
        if (x>x2+1) PUSH(y, x2+1, x-1, -dy);	/* leak on right? */
skip:   for (x++; x<=x2 && XGetPixel(color_image, x, y)!=old_pixel; x++)
	{
#ifdef DEBUG
  if (local_debug) {
    stat_out(" [%d,%d] ", x, y);
    p_cnt++;
    if (p_cnt == 8) {
      stat_out("\n");
      p_cnt = 0;
     }
   }
#endif
	}
        l = x;
    } while (x<=x2);
  }
}
