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
/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.2
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: DNDDraw.c /main/4 1995/10/27 10:43:57 rswiston $"
#endif
#endif
/*
*  (c) Copyright 1987, 1988, 1989 HEWLETT-PACKARD COMPANY */
/*
 *    file: DNDDraw.c
 *
 *	File containing all the drawing routines needed to run DNDDemo
 *	program.
 *
 */

#include "DNDDemo.h"


/* The following character arrays hold the bits for
 * the source and state icons for both 32x32 and 16x16 drag icons.
 * The source is a color palette icon and the state is a paint brush icon.
 */
unsigned char SOURCE_ICON_BITS[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xaa, 0x02,
    0x00, 0x50, 0x55, 0x07, 0x00, 0x28, 0x00, 0x0c, 0x00, 0x94, 0x42, 0x19,
    0x00, 0xca, 0xe5, 0x33, 0x00, 0x85, 0xc6, 0x33, 0x80, 0x42, 0xe7, 0x33,
    0x40, 0x81, 0xc3, 0x31, 0xa0, 0x00, 0x00, 0x38, 0x50, 0x00, 0x00, 0x1c,
    0x28, 0x00, 0x00, 0x0e, 0x90, 0x02, 0x00, 0x07, 0xc8, 0x05, 0x80, 0x03,
    0x90, 0x07, 0xc0, 0x01, 0x48, 0x05, 0xe0, 0x00, 0x90, 0x03, 0x70, 0x00,
    0x08, 0x00, 0x30, 0x00, 0x10, 0x14, 0x30, 0x00, 0x08, 0x2a, 0x30, 0x00,
    0x10, 0x34, 0x30, 0x00, 0x28, 0x2a, 0x60, 0x00, 0x50, 0x9c, 0xe2, 0x00,
    0xa0, 0x40, 0xc4, 0x01, 0x40, 0x01, 0x84, 0x01, 0x80, 0x42, 0x84, 0x03,
    0x00, 0x85, 0x03, 0x03, 0x00, 0x0a, 0x00, 0x03, 0x00, 0xf4, 0xff, 0x03,
    0x00, 0xf8, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00};

unsigned char SOURCE_ICON_MASK[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x07, 0x00, 0xf8, 0xff, 0x0f,
    0x00, 0xfc, 0xff, 0x1f, 0x00, 0xfe, 0xff, 0x3f, 0x00, 0xff, 0xff, 0x7f,
    0x80, 0xff, 0xff, 0x7f, 0xc0, 0xff, 0xff, 0x7f, 0xe0, 0xff, 0xff, 0x7f,
    0xf0, 0xff, 0xff, 0x7f, 0xf8, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0x7f,
    0xfc, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x0f,
    0xfc, 0xff, 0xff, 0x07, 0xfc, 0xff, 0xff, 0x03, 0xfc, 0xff, 0xff, 0x01,
    0xfc, 0xff, 0xff, 0x00, 0xfc, 0xff, 0x7f, 0x00, 0xfc, 0xff, 0x7f, 0x00,
    0xfc, 0xff, 0xff, 0x00, 0xfc, 0xff, 0xff, 0x01, 0xfc, 0xff, 0xff, 0x03,
    0xf8, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x07, 0xe0, 0xff, 0xff, 0x07,
    0xc0, 0xff, 0xff, 0x07, 0x80, 0xff, 0xff, 0x07, 0x00, 0xff, 0xff, 0x07,
    0x00, 0xfe, 0xff, 0x07, 0x00, 0xfc, 0xff, 0x03};

unsigned char STATE_ICON_BITS[] = {
    0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00,
    0xf8, 0x01, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0xf8, 0x03, 0x00, 0x00,
    0xf0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0xc0, 0x0d, 0x00, 0x00,
    0x00, 0x1b, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00,
    0x00, 0xfc, 0x00, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0xf0, 0x03, 0x00,
    0x00, 0xe0, 0x07, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x00, 0x80, 0x1f, 0x00,
    0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0xfc, 0x00,
    0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0xe0, 0x03,
    0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x80, 0x0f, 0x00, 0x00, 0x00, 0x1f,
    0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x38,
    0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xc0};

unsigned char STATE_ICON_MASK[] = {
    0x3f, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00,
    0xfc, 0x03, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00,
    0xfc, 0x07, 0x00, 0x00, 0xf8, 0x07, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00,
    0xe0, 0x1f, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00,
    0x00, 0xfc, 0x00, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0xf0, 0x03, 0x00,
    0x00, 0xe0, 0x07, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0x00, 0x80, 0x1f, 0x00,
    0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xfc, 0x03,
    0x00, 0x00, 0xf8, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0xe0, 0x0f,
    0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x7f,
    0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0xfe,
    0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0xf0};

unsigned char INVALID_ICON_BITS[] = {
    0x00, 0xe0, 0x0f, 0x00, 0x00, 0xfc, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x01,
    0x80, 0xff, 0xff, 0x03, 0xc0, 0x1f, 0xf0, 0x07, 0xe0, 0x07, 0xc0, 0x0f,
    0xf0, 0x07, 0x00, 0x1f, 0xf8, 0x0f, 0x00, 0x3e, 0xf8, 0x1f, 0x00, 0x3c,
    0xfc, 0x3f, 0x00, 0x7c, 0x3c, 0x7f, 0x00, 0x78, 0x3c, 0xfe, 0x00, 0x78,
    0x1e, 0xfc, 0x01, 0xf0, 0x1e, 0xf8, 0x03, 0xf0, 0x1e, 0xf0, 0x07, 0xf0,
    0x1e, 0xe0, 0x0f, 0xf0, 0x1e, 0xc0, 0x1f, 0xf0, 0x1e, 0x80, 0x3f, 0xf0,
    0x1e, 0x00, 0x7f, 0xf0, 0x3c, 0x00, 0xfe, 0x78, 0x3c, 0x00, 0xfc, 0x79,
    0x7c, 0x00, 0xf8, 0x7f, 0x78, 0x00, 0xf0, 0x3f, 0xf8, 0x00, 0xe0, 0x3f,
    0xf0, 0x01, 0xc0, 0x1f, 0xe0, 0x07, 0xc0, 0x0f, 0xc0, 0x1f, 0xf0, 0x07,
    0x80, 0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0x01, 0x00, 0xfc, 0x7f, 0x00,
    0x00, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char SMALL_SOURCE_ICON_BITS[] = {
    0x80, 0x1f, 0x40, 0x60, 0x20, 0x91, 0x90, 0xaa, 0x08, 0x91, 0x08, 0x40,
    0x08, 0x20, 0x08, 0x10, 0x28, 0x10, 0x78, 0x10, 0x28, 0x20, 0x08, 0x41,
    0x90, 0x43, 0x20, 0x21, 0x40, 0x10, 0x80, 0x0f};

unsigned char SMALL_SOURCE_ICON_MASK[] = {
    0x80, 0x1f, 0xc0, 0x7f, 0xe0, 0xff, 0xf0, 0xff, 0xf8, 0xff, 0xf8, 0x7f,
    0xf8, 0x3f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xf8, 0x7f,
    0xf0, 0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0f};

unsigned char SMALL_STATE_ICON_BITS[] = {
    0x0f, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x3c, 0x00, 0x50, 0x00, 0xe0, 0x00,
    0xc0, 0x01, 0x80, 0x03, 0x00, 0x07, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x18,
    0x00, 0x20, 0x00, 0x40, 0x00, 0x80, 0x00, 0x00};

unsigned char SMALL_STATE_ICON_MASK[] = {
    0x0f, 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x3c, 0x00, 0x70, 0x00, 0xe0, 0x00,
    0xc0, 0x01, 0x80, 0x03, 0x00, 0x07, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x18,
    0x00, 0x20, 0x00, 0x40, 0x00, 0x80, 0x00, 0x00};

unsigned char SMALL_INVALID_ICON_BITS[] = {
    0xe0, 0x03, 0xf8, 0x0f, 0x1c, 0x1c, 0x1e, 0x30, 0x3e, 0x30, 0x73, 0x60,
    0xe3, 0x60, 0xc3, 0x61, 0x83, 0x63, 0x03, 0x67, 0x06, 0x3e, 0x06, 0x3c,
    0x1c, 0x1c, 0xf8, 0x0f, 0xe0, 0x03, 0x00, 0x00};


/* Globals variables */
AppInfo appInfo;


/* This is a string to pixel conversion function. */
Pixel 
GetColor(char *colorstr)
{

    XrmValue    from, to;

    from.size = strlen(colorstr) +1;
    if (from.size < sizeof(String))
        from.size = sizeof(String);
    from.addr = colorstr;
    to.addr = NULL;
    XtConvert(topLevel, XmRString, &from, XmRPixel, &to);

    if (to.addr != NULL)
        return ((Pixel) *((Pixel *) to.addr));
    else
        return ( (XtArgVal) NULL);

}


/* This procedure is used to initalize the application information structure */
void
InitializeAppInfo(void)
{

    if (!appInfo) {

        appInfo = (AppInfo) XtMalloc(sizeof(AppInfoRec));
        appInfo->rectGC = NULL;
        appInfo->currentColor = 0;
        appInfo->rectDpyTable = NULL;
        appInfo->rectsAllocd = 0;
        appInfo->numRects = 0;
        appInfo->highlightRect = NULL;
        appInfo->clearRect = NULL;
        appInfo->doMove = True;
        appInfo->creatingRect = True;
        appInfo->operation = XmDROP_MOVE;
        appInfo->maxCursorWidth = 64;
        appInfo->maxCursorHeight = 64;
        appInfo->rectX = 0;
        appInfo->rectY = 0;
        appInfo->rectX2 = 0;
        appInfo->rectY2 = 0;

    }

}


/* This procedure sets the color in the GC for drawing the rectangles
 * in a new color.
 */
void
SetColor(Display *display, Pixel color)
{

    /*
     * if the GC already has a foreground of this color,
     * it would be wasteful to reset the color
     */
    if (color != appInfo->currentColor) {
        XSetForeground(display, appInfo->rectGC, (unsigned long) color);
        appInfo->currentColor = color;
    }

}


/* This function draws the rectangle in the color provided */
static int
RectDraw(Display *display, Window window, RectPtr rect)
{

    SetColor(display, rect->color);
    XFillRectangle(display, window, appInfo->rectGC, rect->x,
                   rect->y, rect->width, rect->height);

}


/* This procedure draws the rectangle highlight in a specified color*/
static void
RectDrawHighlight( Widget w, RectPtr rect, Pixel color )
{

    Display *display = XtDisplay(w);
    Window window = XtWindow(w);
    Pixel currentColor = rect->color;
    XGCValues values;

    values.foreground = color;
    XChangeGC(display, appInfo->rectGC, GCForeground, &values);

    XDrawRectangle(display, window, appInfo->rectGC,
                   rect->x + 1, rect->y + 1,
                   rect->width - HIGHLIGHT_THICKNESS,
                   rect->height - HIGHLIGHT_THICKNESS);

    /* Return the GC to it's previous state */
    values.foreground = appInfo->currentColor = currentColor;
    XChangeGC(display, appInfo->rectGC, GCForeground, &values);

}


/* This procedure handles redrawing the rectangles.  It draws
 * them according to the order in the rectangle display table.
 * The rectangles at the top of the table are drawn first.
 */
void
RedrawRectangles(Widget w)
{

    Display *display = XtDisplay(w);
    RectPtr rect;
    Window window = XtWindow(w);
    int i;

    for (i = 0; i < appInfo->numRects; i++) {

        rect = appInfo->rectDpyTable[i];
        /* Only draw the rectangles that haven't been cleared */
        if (rect != appInfo->clearRect) {
            RectDraw(display, window, rect);
        }
        /* Draw the rectangle highlight of the highlight rectangle */
        if (rect == appInfo->highlightRect) {
            RectDrawHighlight(w, rect, GetColor(HIGHLIGHT_COLOR));
        }

    }

}


/* This procedure will clear the current rectangle and redraw any rectangles
 * that were partially cleared by the rectangle that was deleted.
 */
/* ARGSUSED */
void
RectHide(Display *display, Window window, RectPtr rect)
{

    Pixel background, oldColor;
    Arg args[1];

    /* Get the background of the drawing area. */
    XtSetArg(args[0], XmNbackground, &background);
    XtGetValues(drawingArea, args, 1);

    /* Save the old color for restoration purposes. */
    oldColor = rect->color;

    /* Clear the rectangle */
    rect->color = background;
    RectDraw(display, window, rect);
    appInfo->clearRect = rect;

    /* redraw the rest of the rectangles */
    RedrawRectangles(drawingArea);

    /* restore the rectangle color */
    rect->color = oldColor;

}

/* This procedure draws the stipple rectangle that is used in marking
 * the old rectangle position during a rectangle move operation.
 */
/* ARGSUSED */
void
RectDrawStippled(Display *display, Window window, RectPtr rect)
{

    register int x = rect->x;
    register int y = rect->y;
    register Dimension width = rect->width;
    register Dimension height = rect->height;
    XGCValues values;
    XSegment segments[4];

    /* Set the rectangle color */
    values.foreground = appInfo->currentColor = rect->color;
    XChangeGC(display, appInfo->rectGC, GCForeground , &values);

    /* Create the segments for drawing the stippled rectangle */
    segments[0].x1 = segments[2].x1 = x;
    segments[0].y1 = segments[0].y2 = y;
    segments[0].x2 = x + width - 1;
    segments[1].x1 = segments[1].x2 = x + width - 1;
    segments[1].y1 = segments[3].y1 = y;
    segments[3].y2 = y + height;
    segments[2].y1 = segments[2].y2 = y + height - 1;
    segments[3].x1 = segments[3].x2 = x;
    segments[2].x2 = x + width;
    segments[1].y2 = y + height;

    /* Set the line attributes and draw */
    XSetLineAttributes(display, appInfo->rectGC, 1, LineOnOffDash,
        CapButt, JoinMiter);
    XDrawSegments (display, window, appInfo->rectGC, segments, 4);

    /* restore the default line settings */
    values.line_width = HIGHLIGHT_THICKNESS;
    values.line_style = LineSolid;
    XChangeGC(display, appInfo->rectGC, GCLineWidth | GCLineStyle, &values);

}


/* This procedure sets the highlight rectangle and
 * redraws the rectangles.  The expose routine will draw
 * the highlight around the highlighted rectangle.
 */
/* ARGSUSED */
void
RectHighlight(Widget w, RectPtr rect)
{

    if (appInfo->highlightRect != rect) {
        appInfo->highlightRect = rect;
        RedrawRectangles(w);
    }

}

/* This procedure sets the highlight rectangle to NULL and
 * redraws the rectangles.  The expose routine will clear
 * the highlight around the highlighted rectangle.
 */
/* ARGSUSED */
void
RectUnhighlight(Widget w)
{

    if (appInfo->highlightRect) {
        appInfo->highlightRect = NULL;
        RedrawRectangles(w);
    }

}

/* This function creates and initialized a new rectangle */
RectPtr 
RectCreate(Position x, Position y, Dimension width,
Dimension height, Pixel color, Pixmap pixmap)
{

    RectPtr rect;

    rect = (RectPtr)  XtMalloc(sizeof(RectStruct));

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
    rect->pixmap = pixmap;

    return(rect);

}


/* This procedure will move the rectangle to the end of the rectangle
 * display table (effectively raising it to top of the displayed
 * rectangles).
 */
static void
RectToTop(RectPtr rect)
{

    int    i, j;


    if (rect) {

        /* Get the index to the target rectangle */
        for (i = 0; i < appInfo->numRects; i++) {
            if (appInfo->rectDpyTable[i] == rect)
                break;
        }

        /* Shift the other rectangles downward */
        for (j = i; j < appInfo->numRects - 1; j++)
            appInfo->rectDpyTable[j] = appInfo->rectDpyTable[j + 1];

        /* Place the target rectangle at the end */
        appInfo->rectDpyTable[j] = rect;

    }

}


/* This procedure raises the rectangle to the top of the drawing area */
/* ARGSUSED */
static void
RectRaise(Widget w, RectPtr rect)
{

    RectToTop(rect);
    RedrawRectangles(w);

}


/* This procedure moves the rectangle the the end of the display stack,
 * decrements the number of rectangles, and then frees the rectangle.
 */
void
RectFree(RectPtr rect)
{

    /* if the rectangle is registered */
    if (rect) {

        RectToTop(rect);
        appInfo->numRects--;
        XtFree((char *)rect);

    }

}


/* This procedure added the rectangle to the rectangle display table
 * (reallocing the table if necessary).
 */
void
RectRegister(RectPtr rect, Position x, Position y)
{

    appInfo->numRects++;

    /* rectangles can have their x and y values reset at registration time */
    rect->x = x;
    rect->y = y;

    /* realloc the table if it is too small */
    if (appInfo->numRects > appInfo->rectsAllocd) {

        /* grow geometrically */
        appInfo->rectsAllocd *= 2;
        appInfo->rectDpyTable = (RectPtr *) 
                                 XtRealloc((char *) appInfo->rectDpyTable,
                                           (unsigned) (sizeof(RectPtr) * 
                                           appInfo->rectsAllocd));

    }

    /* Add to end of display table */
    appInfo->rectDpyTable[appInfo->numRects - 1] = rect;

}


/* This function find the top most rectangle at the given x,y position */
RectPtr 
RectFind(Position x, Position y)
{

    RectPtr rect;
    int     i;

    /*
     * Search from the end of the rectangle display table
        * to find the top most rectangle.
        */
    for (i = appInfo->numRects - 1; i >= 0; i--) {

        rect = appInfo->rectDpyTable[i];
        if (rect->x <= x && rect->x + (int)rect->width >= x &&
            rect->y <= y && rect->y + (int)rect->height >= y) {
            return(rect);
        }

    }

    /* If a rectangle is not found return NULL */
    return(NULL);

}


/* This procedure sets the retangle's color */
void
RectSetColor(RectPtr rect, Display *display, Window window, Pixel color)
{

    rect->color = color;
    RectDraw(display, window, rect);

}


/* This function gets the retangle's color */
Pixel
RectGetColor(RectPtr rect)
{
    return(rect->color);
}


/* This procedure sets the retangle's pixmap. The pixmap portion of the
 * rectangle is not currently being used.
 */
/* ARGSUSED */
void
RectSetPixmap(RectPtr rect, Display *display, Window window, Pixmap pixmap)
{

    rect->pixmap = pixmap; /* not currently being looked at */
    RectDraw(display, window, rect);

}


/* This function gets the retangle's pixmap. The pixmap portion of the
 * rectangle is not currently being used.
 */
/* ARGSUSED */
static Pixmap
RectGetPixmap(RectPtr rect)
{
    return (rect->pixmap);
}


/* This procedure gets the retangle's height and width.  */
/* ARGSUSED */
static void
RectGetDimensions(RectPtr rect, Dimension *width, Dimension *height)
{

    *width = rect->width;
    *height = rect->height;

}


/* This function creates the rectangle bitmaps for the icon. */
Pixmap
GetBitmapFromRect(Widget w, RectPtr rect, Pixel background, Pixel foreground,
Dimension *widthRtn, Dimension *heightRtn)
{

    Dimension width, height, maxHeight, maxWidth;
    GC fillGC;
    Pixmap icon_pixmap;
    Display *display = XtDisplay(w);
    XGCValues values;

    RectGetDimensions(rect, &width, &height);

    /* Get the maximum allowable width and height allowed by the cursor */
    maxWidth = appInfo->maxCursorWidth;
    maxHeight = appInfo->maxCursorHeight;

    /* if the dimensions aren't within the allowable dimensions resize
   * then proportionally
   */
    if (maxWidth < width || maxHeight < height) {

        if (width > height) {
            height = (Dimension)(height * maxWidth) / (Dimension)width;
            width = appInfo->maxCursorWidth;
        } else {
            width = (Dimension)(width * maxHeight) / (Dimension)height;
            height = appInfo->maxCursorHeight;
        }

    }

    /* Create a depth 1 pixmap (bitmap) for use with the drag icon */
    icon_pixmap = XCreatePixmap(display, XtWindow(w), width, height, 1);

    /* create a GC for drawing into the bitmap */
    fillGC = XCreateGC(display, icon_pixmap, 0, (XGCValues *)NULL);

    /* fill the bitmap with 0's as a starting point */
    XFillRectangle(display, icon_pixmap, fillGC, 0, 0, width, height);

    /* Change GC to be able to create the rectangle with 1's on the bitmap */
    values.foreground = 1;
    XChangeGC(display, fillGC, GCForeground, &values);

    /*
     * This draw a filled rectangle.  If only a outline is desired
     * use the XDrawRectangle() call.  Note: the outline does not
     * produce very effect icon melting.
     */
    XFillRectangle(display, icon_pixmap, fillGC, 0, 0, width, height);

    /* Free the fill GC */
    XFreeGC(display, fillGC);

    *widthRtn = width;
    *heightRtn = height;

    return(icon_pixmap);

}


/***************************************************************************
 ***************************************************************************
                  Functions used in Drawing Outlines:
 ***************************************************************************
 ***************************************************************************/

/*
 * This procedure changes the GC to do rubberband
 * drawing of a rectangle frame .
 */
static void
SetXorGC(Widget w)
{

    unsigned long valueMask = GCFunction | GCForeground | GCLineWidth;
    XGCValues values;
    Pixel bg, fg;


    XtVaGetValues (w, XmNbackground, &bg, XmNforeground, &fg, NULL);

    values.function = GXxor;
    values.foreground = fg^bg;   /* fix 5127 */
    values.line_width = 1;
    XChangeGC(XtDisplay(w), appInfo->rectGC, valueMask, &values);

}


/* This procedure returns the GC to it's initial state.  */
static void
SetNormGC(Widget w)
{

    unsigned long valueMask = GCFunction | GCLineWidth | GCForeground;
    XGCValues values;

    values.function = GXcopy;
    values.foreground = appInfo->currentColor;
    values.line_width = HIGHLIGHT_THICKNESS;
    XChangeGC(XtDisplay(w), appInfo->rectGC, valueMask, &values);

}


/* This procedure returns the values of the current rectangle outline */
static void
OutlineGetDimensions(Position *x, Position *y, Dimension *width,
Dimension *height)
{

    if (appInfo->rectX < appInfo->rectX2) {
        *x = appInfo->rectX;
        *width = appInfo->rectX2 - *x;
    } else {
        *x = appInfo->rectX2;
        *width = appInfo->rectX - *x;
    }

    if (appInfo->rectY < appInfo->rectY2) {
        *y = appInfo->rectY;
        *height = appInfo->rectY2 - *y;
    } else {
        *y = appInfo->rectY2;
        *height = appInfo->rectY - *y;
    }

}


static void
OutlineDraw(Widget w)
{

    Position    x, y;
    Dimension    width, height;

    OutlineGetDimensions(&x, &y, &width, &height);

    XDrawRectangle(XtDisplay(w), XtWindow(w), appInfo->rectGC,
                   x, y, width, height);

}


/* This procedure sets intializes the drawing positions */
static void
OutlineSetPosition(Position x, Position y)
{

    appInfo->rectX = appInfo->rectX2 = x;
    appInfo->rectY = appInfo->rectY2 = y;

}


/* This procedure resets outline end position */
static void
OutlineResetPosition(Position x, Position y)
{

    appInfo->rectX2 = x;
    appInfo->rectY2 = y;

}


/* This action procedure begins creating a rectangle at the x,y position
 * of the button event if a rectangle doesn't already exist at
 * that position.  Otherwise is raises the rectangle to the top
 * of the drawing area.
 */
/* ARGSUSED */
void
StartRect(Widget w, XEvent *event, String *params, Cardinal *num_params)
{

    Display *display = XtDisplay(w);
    RectPtr rect;
    Position x = event->xbutton.x;
    Position y = event->xbutton.y;

    rect = RectFind(x, y);

    /* if there isn't a rectangle at this position, begin creating one */
    if (!rect) {

        appInfo->creatingRect = True;
        /* set gc for drawing rubberband outline for rectangles */
        SetXorGC(w);
        /* set the initial outline positions */
        OutlineSetPosition(x, y);
        /* Draw the rectangle */
        OutlineDraw(w);

    }
    else
        RectRaise(w, rect);

}


/* This action procedure extends the drawing of the outline
 * for the rectangle to be created.
 */
/* ARGSUSED */
void
ExtendRect(Widget w, XEvent *event, String *params, Cardinal *num_params)
{

    if (appInfo->creatingRect) {

        /* erase the old outline */
        OutlineDraw(w);
        /* set the new outline end positions */
        OutlineResetPosition(event->xbutton.x, event->xbutton.y);
        /* redraw the outline */
        OutlineDraw(w);

    }

}


/* This action procedure creates a rectangle depending on the
 * dimensions set in the StartRect and ExtendRect action procs.
 */
/* ARGSUSED */
void
EndRect(Widget w, XEvent *event, String *params, Cardinal *num_params)
{

    Position     x, y;
    Dimension    width, height;
    RectPtr     rect;


    if (appInfo->creatingRect) {

        /* erase the last outline */
        OutlineDraw(w);
        /* return GC to original state */
        SetNormGC(w);

        /* Get the outline dimensions for creating the rectangle */
        OutlineGetDimensions(&x, &y, &width, &height);

        /* we don't want to create zero width or height rectangles */
        if (width == 0 || height == 0){
            appInfo->creatingRect = False;
            return;
        }

        rect = RectCreate(x, y, width, height,
                          GetColor(RECT_START_COLOR), XmUNSPECIFIED_PIXMAP);

        RectDraw(XtDisplay(w), XtWindow(w), rect);
        RectRegister(rect, x, y);
        appInfo->creatingRect = False;

    }

}


/* The procedure assigns new translations the the given widget */
static void
SetupTranslations(Widget widget, char *new_translations)
{

    XtTranslations new_table;

    new_table = XtParseTranslationTable(new_translations);
    XtOverrideTranslations(widget, new_table);

}


/* This procedure handles exposure events and makes a call to 
 * RedrawRectangles() to redraw the rectangles
 * The rectangles at the top of the table are drawn first.
 */
/* ARGSUSED */
static void
HandleExpose(Widget w, XtPointer closure, XtPointer call_data)
{
    RedrawRectangles(w);
}


/* This procedure sets up the drawing area */
static void
CreateDrawingArea(Widget parent)
{

    static char da_translations[] = 
        "#replace <Btn2Down>: StartMove() \n\
        <Btn1Down>: StartRect() \n\
        <Btn1Motion>: ExtendRect() \n\
        <Btn1Up>: EndRect() \n\
        c <Key>t: XtDisplayTranslations()";

    Arg             args[10];
    int             n = 0;
    XtTranslations    new_table;

    new_table = XtParseTranslationTable(da_translations);

    /* create drawing area at the top of the form */
    n = 0;
    XtSetArg(args[n], XmNtranslations, new_table); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 295); n++;
    XtSetArg(args[n], XmNheight, 180); n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
    XtSetArg(args[n], XmNbackground, GetColor(DRAW_AREA_BG_COLOR)); n++;
    XtSetArg(args[n], XmNforeground, GetColor(DRAW_AREA_FG_COLOR)); n++;
    drawingArea = XmCreateDrawingArea(parent, "drawingArea", args, n);
    XtManageChild(drawingArea);

    /* add expose callback to redisplay rectangles */
    XtAddCallback(drawingArea, XmNexposeCallback, HandleExpose, 
                  (XtPointer) NULL);

}


/* This procedure sets up the area for obtaining rectangle colors */
static void
CreateColorPushButtons(Widget parent, Widget separator)
{

    static char label_translations[] = "<Btn2Down>: ColorRect()";
    Widget         bulletinBoard;
    Widget         children[6];
    XmString    csString;
    Arg         args[10];
    int         n = 0;

    /* Creating an empty compound string so the labels will have no text. */
    csString = XmStringCreateSimple("");

    /* Creating 6 color labels */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, separator); n++;
    XtSetArg(args[n], XmNtopOffset, 2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 295); n++;
    bulletinBoard = XmCreateBulletinBoard(parent, "buletinBoard", args, n);
    XtManageChild(bulletinBoard);

    n = 0;
    XtSetArg(args[n], XmNx, BOX_X_MARGIN); n++;
    XtSetArg(args[n], XmNy, BOX_Y_MARGIN); n++;
    XtSetArg(args[n], XmNwidth, BOX_WIDTH); n++;
    XtSetArg(args[n], XmNheight, BOX_HEIGHT); n++;
    XtSetArg(args[n], XmNlabelString, csString); n++;
    XtSetArg(args[n], XmNbackground, GetColor(LABEL1_COLOR)); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    children[0] = XmCreatePushButton(bulletinBoard, "PushButton1", args, n);

    /* add translations for manipulating rectangles */
    SetupTranslations(children[0], label_translations);

    n = 0;
    XtSetArg(args[n], XmNx, BOX_X_MARGIN + BOX_X_OFFSET); n++;
    XtSetArg(args[n], XmNy, BOX_Y_MARGIN); n++;
    XtSetArg(args[n], XmNwidth, BOX_WIDTH); n++;
    XtSetArg(args[n], XmNheight, BOX_HEIGHT); n++;
    XtSetArg(args[n], XmNlabelString, csString); n++;
    XtSetArg(args[n], XmNbackground, GetColor(LABEL2_COLOR)); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    children[1] = XmCreatePushButton(bulletinBoard, "PushButton1", args, n);

    /* add translations for manipulating rectangles */
    SetupTranslations(children[1], label_translations);

    n = 0;
    XtSetArg(args[n], XmNx, BOX_X_MARGIN + (2 * BOX_X_OFFSET)); n++;
    XtSetArg(args[n], XmNy, BOX_Y_MARGIN); n++;
    XtSetArg(args[n], XmNwidth, BOX_WIDTH); n++;
    XtSetArg(args[n], XmNheight, BOX_HEIGHT); n++;
    XtSetArg(args[n], XmNlabelString, csString); n++;
    XtSetArg(args[n], XmNbackground, GetColor(LABEL3_COLOR)); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    children[2] = XmCreatePushButton(bulletinBoard, "PushButton3", args, n);

    /* add translations for manipulating rectangles */
    SetupTranslations(children[2], label_translations);

    n = 0;
    XtSetArg(args[n], XmNx, BOX_X_MARGIN); n++;
    XtSetArg(args[n], XmNy, BOX_Y_MARGIN + BOX_Y_OFFSET); n++;
    XtSetArg(args[n], XmNwidth, BOX_WIDTH); n++;
    XtSetArg(args[n], XmNheight, BOX_HEIGHT); n++;
    XtSetArg(args[n], XmNlabelString, csString); n++;
    XtSetArg(args[n], XmNbackground, GetColor(LABEL4_COLOR)); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    children[3] = XmCreatePushButton(bulletinBoard, "PushButton4", args, n);

    /* add translations for manipulating rectangles */
    SetupTranslations(children[3], label_translations);

    n = 0;
    XtSetArg(args[n], XmNx, BOX_X_MARGIN + BOX_X_OFFSET); n++;
    XtSetArg(args[n], XmNy, BOX_Y_MARGIN + BOX_Y_OFFSET); n++;
    XtSetArg(args[n], XmNwidth, BOX_WIDTH); n++;
    XtSetArg(args[n], XmNheight, BOX_HEIGHT); n++;
    XtSetArg(args[n], XmNtopWidget, children[0]); n++;
    XtSetArg(args[n], XmNlabelString, csString); n++;
    XtSetArg(args[n], XmNbackground, GetColor(LABEL5_COLOR)); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    children[4] = XmCreatePushButton(bulletinBoard, "PushButton5", args, n);

    /* add translations for manipulating rectangles */
    SetupTranslations(children[4], label_translations);

    n = 0;
    XtSetArg(args[n], XmNx, BOX_X_MARGIN + (2 * BOX_X_OFFSET)); n++;
    XtSetArg(args[n], XmNy, BOX_Y_MARGIN + BOX_Y_OFFSET); n++;
    XtSetArg(args[n], XmNwidth, BOX_WIDTH); n++;
    XtSetArg(args[n], XmNheight, BOX_HEIGHT); n++;
    XtSetArg(args[n], XmNlabelString, csString); n++;
    XtSetArg(args[n], XmNbackground, GetColor(LABEL6_COLOR)); n++;
    XtSetArg(args[n], XmNborderWidth, 1); n++;
    children[5] = XmCreatePushButton(bulletinBoard, "PushButton6", args, n);

    /* add translations for manipulating rectangles */
    SetupTranslations(children[5], label_translations);

    /* Managing the children all at once helps performance */
    XtManageChildren(children, 6);

    /* Freeing compound string.  It is no longer necessary. */
    XmStringFree(csString);

}


/* This procedure initalizes the rectangle display table */
void
InitializeRectDpyTable(void)
{
    /*
     * Initialize display table.  This is used to maintain the
     * order in which the rectangles are displayed
     */
    appInfo->rectDpyTable = (RectPtr *) XtMalloc((unsigned)sizeof(RectPtr));

    /* Initialize rectangle counter.  This is used in reallocing the tables */
    appInfo->rectsAllocd = 1;

}


/* This procedure creates the components to be displayed */
void
CreateLayout(void)
{

    Widget    mainWindow, form, separator;
    Arg     args[10];
    int     n = 0;

    /* Create main window */
    mainWindow = XmCreateMainWindow(topLevel, "mainWindow", args, n);
    XtManageChild(mainWindow);

    /* Create form for hold drawing area, separator, and color labels */
    n = 0;
    XtSetArg(args[n], XmNwidth, 300); n++;
    form = XmCreateForm(mainWindow, "form", args, n);
    XtManageChild(form);

    /* Create area for drawing rectangles */
    CreateDrawingArea(form);

    /* Create separator to separate drawing area from color labels */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, drawingArea); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 300); n++;
    separator = XmCreateSeparatorGadget(form, "separator", args, n);
    XtManageChild(separator);

    /* Create color labels for changing colors of buttons */
    CreateColorPushButtons(form, separator);

    /* Make form the work window of the main window */
    n = 0;
    XtSetArg(args[n], XmNworkWindow, form); n++;
    XtSetValues(mainWindow, args, n);

}


/* This procedure initializes the GC for drawing rectangles */
void
CreateRectGC(void)
{

    XGCValues    values;

    values.line_style = LineSolid;
    values.line_width = HIGHLIGHT_THICKNESS;
    values.foreground = appInfo->currentColor = GetColor(RECT_START_COLOR);
    appInfo->rectGC = XCreateGC(XtDisplay(topLevel), XtWindow(drawingArea),
                      GCLineStyle | GCLineWidth | GCForeground, &values);

}
