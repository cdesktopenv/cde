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
/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include "TermPrimLineFontP.h"
#include "TermPrimLineDrawP.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"

/****************************************************************************
 * 
 * This module is used to implement the line drawing functionality for
 * hpterm.  It implements a primitive (simple?)  font scaling engine that
 * converts a set of rectangles, lines, and "stipples" to a line drawing
 * font.  The font is stored as a single plane pixmap and is rendered by
 * performing a copy plane to the target drawable.
 * 
 * To increase the performance, fonts are not free'ed when the reference
 * count hits zero.  Rather, they are free'ed if the reference count has
 * hit zero and a new font is created with different parameters.
 * 
 ***************************************************************************/

static LineDrawFontData lineDrawFontDataHead = (LineDrawFontData) 0;
static LineDrawFont lineDrawFontHead = (LineDrawFont) 0;
/* static int numLineDrawFonts = 0; */

static void
ScaleCharacter(
#ifdef	USE_PIXMAPS
	GC gc, Pixmap bitmap,
	ScaledBitmapInfo renderInfo, GlyphInfo glyph,
	int width, int height
#else	/* USE_PIXMAPS */
	ScaledCharInfo renderInfo, GlyphInfo glyph, int width, int height
#endif	/* USE_PIXMAPS */
	)
{
    int j;
#ifdef	USE_PIXMAPS
    cellPositionType cellX;
    cellPositionType cellX;
    Pixmap pixmap;
#else	/* USE_PIXMAPS */
    int k;
    int numLines;
#endif	/* USE_PIXMAPS */

#ifdef	USE_PIXMAPS
    /* calculate position of pixmap into which we will draw the glyph...
     */
    cellX = (i % 16) * width;
    cellY = (i / 16) * height;
    renderInfo->cellX = cellX;
    renderInfo->cellY = cellY;
#else	/* USE_PIXMAPS */
    /* malloc storage for the line segments, rectangles, stipples...
     */
    /* count the number of lines... */
    for (j = 0, numLines = 0; j < glyph->numLines; j++) {
	numLines += glyph->lines[j].width;
    }
    if (numLines > 0) {
	/* malloc the line storage... */
	renderInfo->segs = (XSegment *)
		XtRealloc((void *) renderInfo->segs,
		numLines * sizeof(XSegment));
    }
    renderInfo->numSegs = numLines;

    /* rectangles... */
    if (glyph->numRects > 0) {
	/* malloc the rectangle storage... */
	renderInfo->rects = (XRectangle *)
		XtRealloc((void *) renderInfo->rects,
		glyph->numRects * sizeof(XRectangle));
    }
    renderInfo->numRects = glyph->numRects;

    /* stipples... */
    if (glyph->numStipples > 0) {
	/* malloc the stipple storage... */
	renderInfo->stipples = (XRectangle *)
		XtRealloc((void *) renderInfo->stipples,
		glyph->numStipples * sizeof(XRectangle));
    }
    renderInfo->numStipples = glyph->numStipples;
#endif	/* USE_PIXMAPS */

    /* draw the lines...
     */
#ifndef	USE_PIXMAPS
    /* reset the counter... */
    numLines = 0;
#endif	/* USE_PIXMAPS */

    /* lines
     */
    for (j = 0; j < glyph->numLines; j++) {
	/* draw the line segment / scale the line segment... */
	int x1;
	int x2;
	int y1;
	int y2;

	DebugF('l', 0, fprintf(stderr,
		">> line data:  x1=%d%+d  y1=%d%+d  x2=%d%+d  y2=%d%+d width=%d",
		glyph->lines[j].x1,
		glyph->lines[j].x1Offset,
		glyph->lines[j].y1,
		glyph->lines[j].y1Offset,
		glyph->lines[j].x2,
		glyph->lines[j].x2Offset,
		glyph->lines[j].y2,
		glyph->lines[j].y2Offset,
		glyph->lines[j].width));

	/* scale x1 and x2 to our width... */
	x1 = ((width - 1) * glyph->lines[j].x1) / 100;
	x2 = ((width - 1) * glyph->lines[j].x2) / 100;

	/* scale y1 and y2 to our height... */
	y1 = ((height - 1) * glyph->lines[j].y1) / 100;
	y2 = ((height - 1) * glyph->lines[j].y2) / 100;

	/* add offsets... */
	x1 += glyph->lines[j].x1Offset;
	x2 += glyph->lines[j].x2Offset;
	y1 += glyph->lines[j].y1Offset;
	y2 += glyph->lines[j].y2Offset;

#ifdef	USE_PIXMAPS
	/* add in the line width... */
	if (x1 == x2) {
	    x1 -= (glyph->lines[j].width - 1) / 2;
	    x2 += glyph->lines[j].width - 1 - (glyph->lines[j].width - 1) / 2;
	}

	/* add in the line width... */
	if (y1 == y2) {
	    y1 -= (glyph->lines[j].width - 1) / 2;
	    y2 += glyph->lines[j].width - 1 - (glyph->lines[j].width - 1) / 2;
	}

	DebugF('l', 0, fprintf(stderr,
		">> line:  x=%d  y=%d  width=%d  height=%d",
		x1, y1, x2 - x1 + 1, y2 - y1 + 1));
	/* draw the line (actually, fill the rectangle)... */
	(void) XFillRectangle(d, 		/* display		*/
		    pixmap,			/* drawable		*/
		    gc,				/* GC			*/
		    cellX + x1,			/* x			*/
		    cellY + y1,			/* y			*/
		    x2 - x1 + 1,		/* width		*/
		    y2 - y1 + 1);		/* height		*/
#else	/* USE_PIXMAPS */
	if ((x1 == x2) && (y1 != y2)) {
	    /* vertical lines... */
	    for (k = 0; k < glyph->lines[j].width; k++) {
		renderInfo->segs[numLines + k].x1 =
			renderInfo->segs[numLines + k].x2 =
			x1 - (glyph->lines[j].width / 2) + k;
		renderInfo->segs[numLines + k].y1 = y1;
		renderInfo->segs[numLines + k].y2 = y2;
	    }
	} else if ((y1 == y2) && (x1 != x2)) {
	    /* horizontal lines... */
	    for (k = 0; k < glyph->lines[j].width ; k++) {
		renderInfo->segs[numLines + k].y1 =
			renderInfo->segs[numLines + k].y2 =
			y2 - (glyph->lines[j].width / 2) + k;
		renderInfo->segs[numLines + k].x1 = x1;
		renderInfo->segs[numLines + k].x2 = x2;
	    }
	} else {
	    for (k = 0; k < glyph->lines[j].width; k++) {
		renderInfo->segs[numLines + k].y1 =
			renderInfo->segs[numLines + k].y2 =
			y2 - (glyph->lines[j].width / 2) + k;
		renderInfo->segs[numLines + k].x1 = x1 - 
			glyph->lines[j].width / 2;
		renderInfo->segs[numLines + k].x2 = x2 + 
			glyph->lines[j].width / 2;
	    }
	}

	numLines += glyph->lines[j].width;
    }
#endif	/* USE_PIXMAPS */

    /* rectangles...
     */
    for (j = 0; j < glyph->numRects; j++) {
	/* draw / stipple the rectangles... */
	int x1;
	int x2;
	int y1;
	int y2;

	/* scale x1 and x2 to our width... */
	x1 = ((width - 1) * glyph->rects[j].x1) / 100;
	x2 = ((width - 1) * glyph->rects[j].x2) / 100;

	/* scale y1 and y2 to our height... */
	y1 = ((height - 1) * glyph->rects[j].y1) / 100;
	y2 = ((height - 1) * glyph->rects[j].y2) / 100;

	/* add offsets... */
	x1 += glyph->rects[j].x1Offset;
	x2 += glyph->rects[j].x2Offset;
	y1 += glyph->rects[j].y1Offset;
	y2 += glyph->rects[j].y2Offset;

#ifdef	USE_PIXMAPS
	DebugF('l', 0, fprintf(stderr,
		">> rect:  x=%d  y=%d  width=%d  height=%d",
		x1, y1, x2 - x1 + 1, y2 - y1 + 1));
	/* fill the rectangle... */
	(void) XFillRectangle(d, 		/* display		*/
		    pixmap,			/* drawable		*/
		    gc,				/* GC			*/
		    cellX + x1,			/* x			*/
		    cellY + y1,			/* y			*/
		    x2 - x1 + 1,		/* width		*/
		    y2 - y1 + 1);		/* height		*/
#else	/* USE_PIXMAPS */
	renderInfo->rects[j].x = x1;
	renderInfo->rects[j].y = y1;
	renderInfo->rects[j].width = x2 - x1 + 1;
	renderInfo->rects[j].height = y2 - y1 + 1;
#endif	/* USE_PIXMAPS */
    }

    /* stipple in the rectangle...
     */
    for (j = 0; j < glyph->numStipples; j++) {
	/* stipple the rectangle... */
	int x1;
	int x2;
	int y1;
	int y2;
	int stippleSize;
#ifdef	USE_PIXMAPS
	int xIndex;
	int yIndex;
#endif	/* USE_PIXMAPS */

	/* scale x1 and x2 to our width... */
	x1 = ((width - 1) * glyph->stipples[j].x1) / 100;
	x2 = ((width - 1) * glyph->stipples[j].x2) / 100;

	/* scale y1 and y2 to our height... */
	y1 = ((height - 1) * glyph->stipples[j].y1) / 100;
	y2 = ((height - 1) * glyph->stipples[j].y2) / 100;

	stippleSize = glyph->stipples[j].width;

	/* add offsets... */
	x1 += glyph->stipples[j].x1Offset;
	x2 += glyph->stipples[j].x2Offset;
	y1 += glyph->stipples[j].y1Offset;
	y2 += glyph->stipples[j].y2Offset;

#ifdef	USE_PIXMAPS
	/* fill in the area... */
	for (yIndex = y1; yIndex < y2; yIndex += stippleSize) {
	    for (xIndex = x1; xIndex < x2; xIndex += stippleSize) {
		if (!(((xIndex / stippleSize) % 2) ^
			((yIndex / stippleSize) % 2))) {
		    /* fill this rectangle... */
		    DebugF('l', 0, fprintf(stderr,
			    ">> stipple:  x=%d  y=%d  width=%d  height=%d",
			    xIndex, yIndex,
			    (stippleSize <= (x2 - xIndex)) ? stippleSize :
				    x2 - xIndex,
			    (stippleSize <= (y2 - yIndex)) ? stippleSize :
				    y2 - yIndex));
		    (void) XFillRectangle(d,	/* display		*/
			    pixmap,		/* drawable		*/
			    gc,			/* GC			*/
			    cellX + xIndex,	/* x			*/
			    cellY + yIndex,	/* y			*/
			    (stippleSize <= (x2 - xIndex)) ? stippleSize :
				    x2 - xIndex,
						/* width		*/
			    (stippleSize <= (y2 - yIndex)) ? stippleSize :
				    y2 - yIndex);
						/* height		*/
		}
	    }
	}
#endif	/* USE_PIXMAPS */
    }

    /* mark this character as completed... */
#ifdef	USE_PIXMAPS
    renderInfo->scaled = True;
#else	/* USE_PIXMAPS */
    renderInfo->scaled = True;
#endif	/* USE_PIXMAPS */
}

static short *
GetLineDrawFontIndex(GlyphInfo glyphInfo, int numGlyphs)
{
    LineDrawFontData lineDrawFontData;
    int i;
    int j;

    for (lineDrawFontData = lineDrawFontDataHead; lineDrawFontData;
	    lineDrawFontData = lineDrawFontData->next) {
	if ((lineDrawFontData->glyphInfo == glyphInfo) &&
		lineDrawFontData->numGlyphs == numGlyphs) {
	    return(lineDrawFontData->lineDrawIndex);
	}
    }

    /* no match.  Insert one at the head of the list and fill it out...
     */
    lineDrawFontData = (LineDrawFontData) XtMalloc(sizeof(LineDrawFontDataRec));
    lineDrawFontData->next = lineDrawFontDataHead;
    lineDrawFontDataHead = lineDrawFontData;
    lineDrawFontData->glyphInfo = glyphInfo;
    lineDrawFontData->numGlyphs = numGlyphs;
    /* clear out the array... */
    for (i = 0; i < (sizeof(lineDrawFontData->lineDrawIndex) /
	    sizeof(lineDrawFontData->lineDrawIndex[0])); i++) {
	lineDrawFontData->lineDrawIndex[i] = -1;
    }

    /* fill up the array... */
    for (i = 0; i < numGlyphs; i++) {
	for (j = 0; glyphInfo[i].chars[j] > 0; j++) {
	    lineDrawFontData->lineDrawIndex[glyphInfo[i].chars[j] % 256] = i;
	}
    }
    return(lineDrawFontData->lineDrawIndex);
}

LineDrawFont
_DtTermPrimLineDrawCreateFont(Widget w, GlyphInfo glyphInfo, int numGlyphs,
	int width, int ascent, int descent)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int fontNumber;
    LineDrawFont lineDrawFont;
    int i;
    int height = ascent + descent;
#ifdef	USE_PIXMAPS
    GC gc;
    Pixmap pixmap;
#endif	/* USE_PIXMAPS */

    _DtTermProcessLock();
    /* let's look for a line draw font that is already made...
     */
    for (lineDrawFont = lineDrawFontHead; lineDrawFont;
	    lineDrawFont = lineDrawFont->next) {
	if ((lineDrawFont->fontValid) &&
		(lineDrawFont->width == width) &&
		(lineDrawFont->height == height) &&
		(lineDrawFont->ascent == ascent) &&
		(lineDrawFont->display == XtDisplay(w)) &&
		(lineDrawFont->glyphInfo == glyphInfo) &&
		(lineDrawFont->numGlyphs == numGlyphs)) {
	    /* we found a match...
	     */
	    /* bump the reference count... */
	    (void) lineDrawFont->refCount++;
	    /* and return this "font"... */
	    _DtTermProcessUnlock();
	    return (lineDrawFont);
	}
    }

    /* no match -- we will have to build the font...
     */
    /* look for a free slot... */
    for (lineDrawFont = lineDrawFontHead; lineDrawFont;
	    lineDrawFont = lineDrawFont->next) {
	if ((lineDrawFont->refCount == 0) &&
		(lineDrawFont->glyphInfo == glyphInfo) &&
		(lineDrawFont->numGlyphs == numGlyphs)) {
	    /* found a free slot... */
	    /* free any valid but unused font in this slot... */
	    if (lineDrawFont->fontValid) {
#ifdef	USE_PIXMAPS
		(void) XFreePixmap(lineDrawFont->display,
						/* Display		*/
			lineDrawFont->pixmap);	/* drawable		*/
#endif	/* USE_PIXMAPS */
		lineDrawFont->fontValid = False;
	    }
	    break;
	}
    }

    /* did we hit the end of the linked list... */
    if (!lineDrawFont) {
	/* add a new entry to the head of the linked list... */
	lineDrawFont = (LineDrawFont)
		XtMalloc(sizeof(LineDrawFontRec));
	(void) memset(lineDrawFont, '\0', sizeof(LineDrawFontRec));
	lineDrawFont->next = lineDrawFontHead;
	lineDrawFontHead = lineDrawFont;
    }

    lineDrawFont->refCount = 1;
    lineDrawFont->fontValid = True;
    lineDrawFont->width = width;
    lineDrawFont->height = height;
    lineDrawFont->ascent = ascent;
    lineDrawFont->width = width;
    lineDrawFont->display = XtDisplay(w);
    lineDrawFont->glyphInfo = glyphInfo;
    lineDrawFont->numGlyphs = numGlyphs;
    lineDrawFont->glyphIndex = GetLineDrawFontIndex(glyphInfo, numGlyphs);

#ifdef	USE_PIXMAPS
    /* malloc the cell position for this font (if necessary)... */
    if (!lineDrawFont->scaledBitmapInfo) {
	lineDrawFont->scaledBitmapInfo = (ScaledBitmapInfo)
		XtMalloc(lineDrawFont->numGlyphs * sizeof(ScaledBitmapInfoRec));
	/* initialize it... */
	(void) memset(lineDrawFont->scaledBitmapInfo, '\0',
		lineDrawFont->numGlyphs * sizeof(scaledBitmapInfo));
    } else {
	/* mark all the characters in the old font as unscaled... */
	for (i = 0; i < lineDrawFont->numGlyphs; i++) {
	    lineDrawFont->scaledBitmapInfo[i].scaled = False;
	}
    }
#else	/* USE_PIXMAPS */
    /* malloc the line, rectangle, stipple storage (if necessary)... */
    if (!lineDrawFont->scaledCharInfo) {
	lineDrawFont->scaledCharInfo = (ScaledCharInfo)
		XtMalloc(lineDrawFont->numGlyphs * sizeof(ScaledCharInfoRec));
	(void) memset(lineDrawFont->scaledCharInfo, '\0',
		lineDrawFont->numGlyphs * sizeof(ScaledCharInfoRec));
    } else {
	/* mark all the characters in the old font as unscaled... */
	for (i = 0; i < lineDrawFont->numGlyphs; i++) {
	    lineDrawFont->scaledCharInfo[i].scaled = False;
	}
    }
#endif	/* USE_PIXMAPS */

#ifdef	USE_PIXMAPS
    /* create a pixmap that is 16 characters wide and
     * lineDrawFont->numGlyphs / 16 characters high...
     */
    pixmap = XCreatePixmap(d,
						/* Display		*/
	    DefaultRootWindow(d),		/* reference window	*/
	    width * 16,				/* width		*/
	    (lineDrawFont->numGlyphs + 15) / 16 * (height),
						/* height		*/
	    1);					/* planes		*/
    lineDrawFont->pixmap = pixmap;

    /* we need a GC...
     */
    gc = XCreateGC(d,				/* Display		*/
	    pixmap,				/* drawable		*/
	    (unsigned long) (0),		/* value mask		*/
	    (XGCValues *) 0);			/* values		*/

    /* clear the pixmap... */
    (void) XSetForeground(d, gc, 0);
    (void) XFillRectangle(d,			/* display		*/
	    pixmap,				/* drawable		*/
	    gc,					/* GC			*/
	    0,					/* x			*/
	    0,					/* y			*/
	    width * 16,				/* width		*/
	    (lineDrawFont->numGlyphs + 15) / 16 * (height));
						/* height		*/

    /* reset the GC so we can fill in the lines and rectangles... */
    (void) XSetForeground(d, gc, 1);
#endif	/* USE_PIXMAPS */

    _DtTermProcessUnlock();
    return(lineDrawFont);
}

void
_DtTermPrimLineDrawFreeFont(LineDrawFont lineDrawFont)
{
    _DtTermProcessLock();

    /* perform a sanity check... */
    if (lineDrawFont->refCount <= 0) {
        _DtTermProcessUnlock();
	return;
    }

    /* decrement the reference count... */
    (void) lineDrawFont->refCount--;

    /* we will free the storage if this font is used... */
    _DtTermProcessUnlock();
    return;
}


void
_DtTermPrimLineDrawImageString(Display *display, Drawable d,
	LineDrawFont lineDrawFont,
	GC gc, GC clearGC, int x, int y, unsigned char *string, int width)
{
    int glyph;
#ifndef	USE_PIXMAPS
    XSegment segs[20];
    XRectangle rects[20];
    int i;
#endif	/* USE_PIXMAPS */

    _DtTermProcessLock();
    if (!lineDrawFont || (lineDrawFont->refCount < 0)) {
        _DtTermProcessUnlock();
	return;
    }

#ifndef	USE_PIXMAPS
    /* clear the area... */
    (void) XFillRectangle(display,		/* Display		*/
	    d,					/* Window		*/
	    clearGC,				/* GC			*/
	    x,					/* x			*/
	    y - lineDrawFont->ascent,		/* y			*/
	    lineDrawFont->width * width,	/* width		*/
	    lineDrawFont->height);		/* height		*/
#endif	/* USE_PIXMAPS */

    /* render the characters... */
    for (; width > 0; width--, string++) {
	/* look up glyph through the glyph table... */
	/* check for valid glyph... */
	glyph = lineDrawFont->glyphIndex[*string];

	/* if it is invalid, then let's default to space... */
	if (glyph < 0)
	    glyph = lineDrawFont->glyphIndex[' '];

	/* if it is still invalid, then let's skip this character... */
	if (glyph < 0) {
	    x += lineDrawFont->width;
	    continue;
	}

#ifdef	USE_PIXMAPS
	/* render this character... */
	(void) XCopyPlane(display,		/* Display		*/
		lineDrawFont->pixmap,		/* src			*/
		d,				/* dest			*/
		gc,				/* GC			*/
		lineDrawFont->cellX[glyph],	/* src x		*/
		lineDrawFont->cellY[glyph],	/* src y		*/
		lineDrawFont->width,		/* width		*/
		lineDrawFont->height,		/* height		*/
		x,				/* dest x		*/
		y - lineDrawFont->ascent,	/* dest y		*/
		1);				/* plane		*/

#else	/* USE_PIXMAPS */
	if (!lineDrawFont->scaledCharInfo[glyph].scaled) {
	    /* first time, scale this character... */
	    (void) ScaleCharacter(&(lineDrawFont->scaledCharInfo[glyph]),
		    &(lineDrawFont->glyphInfo[glyph]), lineDrawFont->width, lineDrawFont->height);
	}

	if (lineDrawFont->scaledCharInfo[glyph].numSegs > 0) {
	    for (i = 0; i < lineDrawFont->scaledCharInfo[glyph].numSegs; i++) {
		segs[i].x1 = x + lineDrawFont->scaledCharInfo[glyph].segs[i].x1;
		segs[i].x2 = x + lineDrawFont->scaledCharInfo[glyph].segs[i].x2;
		segs[i].y1 =
			y + lineDrawFont->scaledCharInfo[glyph].segs[i].y1 -
			lineDrawFont->ascent;
		segs[i].y2 =
			y + lineDrawFont->scaledCharInfo[glyph].segs[i].y2 -
			lineDrawFont->ascent;
	    }
	    (void) XDrawSegments(display,	/* Display		*/
		d,				/* dest			*/
		gc,				/* GC			*/
		segs,				/* segments		*/
		lineDrawFont->scaledCharInfo[glyph].numSegs);
						/* num segs		*/
	}

	if (lineDrawFont->scaledCharInfo[glyph].numRects > 0) {
	    for (i = 0; i < lineDrawFont->scaledCharInfo[glyph].numRects; i++) {
		rects[i].x = x + lineDrawFont->scaledCharInfo[glyph].rects[i].x;
		rects[i].y =
			y + lineDrawFont->scaledCharInfo[glyph].rects[i].y -
			lineDrawFont->ascent;
		rects[i].width =
			lineDrawFont->scaledCharInfo[glyph].rects[i].width;
		rects[i].height =
			lineDrawFont->scaledCharInfo[glyph].rects[i].height;
	    }
	    (void) XFillRectangles(display,	/* Display		*/
		d,				/* dest			*/
		gc,				/* GC			*/
		rects,				/* rectangles		*/
		lineDrawFont->scaledCharInfo[glyph].numRects);
						/* num rects		*/
	}
#endif	/* USE_PIXMAPS */
	/* slide over one character... */
	x += lineDrawFont->width;
    }

    _DtTermProcessUnlock();
    return;
}
