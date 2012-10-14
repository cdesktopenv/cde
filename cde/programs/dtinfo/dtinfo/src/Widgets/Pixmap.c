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
/* $XConsortium: Pixmap.c /main/3 1996/06/11 16:45:33 cde-hal $ */
#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#include "PixmapP.h"

#ifdef DEBUG
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif

static Boolean SetValues();
static void Initialize();
static void Destroy();
static void Resize();
static void ReDisplay();
static XtGeometryResult QueryGeometry();

static char *defaultTranslations[] = {""};

/*
static XtActionsRec actions[] =
{
};
*/

static XtResource resources[] = {
  { XtNpixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
      XtOffset(PixmapWidget, pixmap.pixmap), XtRPixmap, 0},
};

PixmapClassRec  pixmapClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &coreClassRec,
    /* class_name		*/	"Pixmap",
    /* widget_size		*/	sizeof(PixmapRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	FALSE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	ReDisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	0, /* defaultTranslations*/
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
    {
      /* Pixmap class fields */
      0,			/* ignore */
    }
};

WidgetClass pixmapWidgetClass = (WidgetClass)&pixmapClassRec;

static void
Initialize(req, new, args, nargs )
PixmapWidget req, new ;
ArgList args ;
Cardinal *nargs ;
{
  XtGCMask  value_mask ;
  XGCValues values ;


  ON_DEBUG(fprintf(stderr, "PixmapWidget:\tInitialize()\n"));

  if ((new->core.width  == 0) ||
      (new->core.height == 0)){

    Dimension width, height ;
    XtVaGetValues(XtParent(new),
		  XtNwidth, &width,
		  XtNheight, &height,
		  (char*)0);

    if (new->core.width == 0)
      new->core.width = width ;

    if (new->core.height == 0)
      new->core.height = height ;
  }

  /* avoid bogus Pixmap value */
  new->pixmap.pixmap = 0 ;

  /* get gc */
  value_mask = GCFunction | GCBackground ;
  values.function = GXcopy ;
  values.background = new->core.background_pixel ;
  new->pixmap.gc = XtGetGC((Widget)new, value_mask, &values);
}

static void
Resize(w)
PixmapWidget w ;
{
  ON_DEBUG(fprintf(stderr, "PixmapWidget: RESIZE (%d, %d)\n",
		   w->core.width, w->core.height));
}

static void
Destroy(w)
PixmapWidget w;
{
  if (w->pixmap.pixmap != 0)
    {
      ON_DEBUG(fprintf(stderr, "Hey, free the pixmap\n"));
    }
}

static Boolean
SetValues(current, request, new, args, num_args)
PixmapWidget current, request, new ;
ArgList args ;
Cardinal *num_args ;
{
  Window root ;
  int x, y ;
  unsigned int width, height, border_width, depth ;
  Boolean needs_redisplay=FALSE ;

  ON_DEBUG(fprintf(stderr, "PixmapWidget: SetValues\n"));

  if (current->pixmap.pixmap != new->pixmap.pixmap)
    {
      XtWidgetGeometry request, reply ;
      /* get pixmap size */
      XGetGeometry(XtDisplay(new),
		   new->pixmap.pixmap,
		   &root, 
		   &x, &y, 
		   &width, &height,
		   &border_width,
		   &depth);
      new->pixmap.width = width ;
      new->pixmap.height = height ;

      request.request_mode = CWWidth | CWHeight ;
      request.width = width ;
      request.height = height ;
      XtMakeGeometryRequest((Widget)new, &request, &reply);

    }
  return needs_redisplay ;
}


static XtGeometryResult
QueryGeometry(w, proposed, answer)
PixmapWidget w ;
XtWidgetGeometry *proposed, *answer;
{
  ON_DEBUG(fprintf(stderr, "PixmapWidget:\tQueryGeometry()\n"));
#ifdef DEBUG
  if (proposed->request_mode & CWX)
    fprintf(stderr, "X: %d ==> %d\n", w->core.x, proposed->x);
  if (proposed->request_mode & CWY)
    fprintf(stderr, "Y: %d ==> %d\n", w->core.y, proposed->y);
  if (proposed->request_mode & CWWidth)
    fprintf(stderr, "Width: %d ==> %d\n", w->core.width, proposed->width);
  if (proposed->request_mode & CWHeight)
    fprintf(stderr, "Height: %d ==> %d\n", w->core.height, proposed->height);

#endif

  memcpy(proposed, answer, sizeof(XtWidgetGeometry));

  return XtGeometryYes ;
}

#ifndef DEBUG
#define DEBUG
#endif

static void
ReDisplay(w, event)
PixmapWidget w ;
XExposeEvent *event ;
{
  ON_DEBUG(fprintf(stderr, "PixmapWidget: EXPOSE\n"));

  if (w->pixmap.pixmap != 0)
    {
      Display *display = XtDisplay(w);
      XRectangle rect[1] ;
      rect[0].x = event->x ;
      rect[0].y = event->y ;
      rect[0].width = event->width ;
      rect[0].height = event->height ;

      ON_DEBUG(fprintf(stderr, "XCopyArea(%d, %d, %d, %d)\n",
		       event->x, event->y,
		       event->width, event->height));

      /* wastefull...could only copy required data - jbm */
      XSetClipRectangles(display,
			 w->pixmap.gc, /* gc */
			 0,	       /* clip x origin  */
			 0,	       /* clip y origin  */
			 rect,	       /* rect[]  */
			 1,	       /* num rects  */
			 Unsorted);    /* ordering  */

      XCopyArea(XtDisplay(w),	/* display */
		w->pixmap.pixmap, /* src     */
		XtWindow(w),	  /* dest    */
		w->pixmap.gc,	  /* gc	   */
		0, 0,		  /* src_x, src_y */
		w->pixmap.width,  /* pixmap width  */
		w->pixmap.height, /* pixmap height */
		0, 0);		  /* dest_x, dest_y */

      /* unset clip mask */
      XSetClipMask(XtDisplay(w), w->pixmap.gc, None);
    }
}

void
PixmapWidgetDimensions(Widget w, Dimension *width, Dimension *height)
{
  if (IsPixmapWidget(w))
    {
      PixmapWidget pw = (PixmapWidget) w ;
      *width  = pw->pixmap.width ;
      *height = pw->pixmap.height ;
    }
  else
    {
      *width = 0 ;
      *height = 0 ;
    }
}
