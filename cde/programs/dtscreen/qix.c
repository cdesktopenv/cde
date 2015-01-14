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
/* $XConsortium: qix.c /main/3 1995/11/02 16:08:11 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * qix.c - Vector swirl for dtscreen, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 29-Jul-90: support for multiple screens.
 *	      made check_bounds_?() a macro.
 *	      fixed initial parameter setup.
 * 15-Dec-89: Fix for proper skipping of {White,Black}Pixel() in colors.
 * 08-Oct-89: Fixed bug in memory allocation in initqix().
 *	      Moved seconds() to an extern.
 * 23-Sep-89: Switch to random() and fixed bug w/ less than 4 lines.
 * 20-Sep-89: Lint.
 * 24-Mar-89: Written.
 */

#include "dtscreen.h"
#include <stdlib.h>

typedef struct {
    int         x;
    int         y;
}           point;

typedef struct {
    int         pix;
    long        startTime;
    int         first;
    int         last;
    int         dx1;
    int         dy1;
    int         dx2;
    int         dy2;
    int         x1;
    int         y1;
    int         x2;
    int         y2;
    int         offset;
    int         delta;
    int         width;
    int         height;
    int         nlines;
    point      *lineq;
}           qixstruct;

void
initqix(perwindow *pwin)
{
    XWindowAttributes xgwa;
    qixstruct  *qp;

    if (pwin->data) free(pwin->data);
    pwin->data = (void *)malloc(sizeof(qixstruct));
    memset(pwin->data, '\0', sizeof(qixstruct));
    qp = (qixstruct *)pwin->data;
    qp->startTime = seconds();
    qp->nlines = (batchcount + 1) * 2;

    qp->lineq = (point *) malloc(qp->nlines * sizeof(point));
    memset(qp->lineq, '\0', qp->nlines * sizeof(point));

    XGetWindowAttributes(dsp, pwin->w, &xgwa);
    qp->width = xgwa.width;
    qp->height = xgwa.height;
    qp->delta = 16;

    if (qp->width < 100) {	/* icon window */
	qp->nlines /= 4;
	qp->delta /= 4;
    }
    qp->offset = qp->delta / 3;
    qp->last = 0;
    qp->pix = 0;
    qp->dx1 = random() % qp->delta + qp->offset;
    qp->dy1 = random() % qp->delta + qp->offset;
    qp->dx2 = random() % qp->delta + qp->offset;
    qp->dy2 = random() % qp->delta + qp->offset;
    qp->x1 = random() % qp->width;
    qp->y1 = random() % qp->height;
    qp->x2 = random() % qp->width;
    qp->y2 = random() % qp->height;
    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));
    XFillRectangle(dsp, pwin->w, pwin->gc, 0, 0, qp->width, qp->height);
}

#define check_bounds(qp, val, del, max)				\
{								\
    if ((val) < 0) {						\
	*(del) = (random() % (qp)->delta) + (qp)->offset;	\
    } else if ((val) > (max)) {					\
	*(del) = -(random() % (qp)->delta) - (qp)->offset;	\
    }								\
}

void
drawqix(perwindow *pwin)
{
    qixstruct  *qp = (qixstruct  *)pwin->data;

    qp->first = (qp->last + 2) % qp->nlines;

    qp->x1 += qp->dx1;
    qp->y1 += qp->dy1;
    qp->x2 += qp->dx2;
    qp->y2 += qp->dy2;
    check_bounds(qp, qp->x1, &qp->dx1, qp->width);
    check_bounds(qp, qp->y1, &qp->dy1, qp->height);
    check_bounds(qp, qp->x2, &qp->dx2, qp->width);
    check_bounds(qp, qp->y2, &qp->dy2, qp->height);
    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));
    XDrawLine(dsp, pwin->w, pwin->gc,
	      qp->lineq[qp->first].x, qp->lineq[qp->first].y,
	      qp->lineq[qp->first + 1].x, qp->lineq[qp->first + 1].y);
    if (!mono && pwin->perscreen->npixels > 2) {
	XSetForeground(dsp, pwin->gc, pwin->perscreen->pixels[qp->pix]);
	if (++qp->pix >= pwin->perscreen->npixels)
	    qp->pix = 0;
    } else
	XSetForeground(dsp, pwin->gc, WhitePixelOfScreen(pwin->perscreen->screen));

    XDrawLine(dsp, pwin->w, pwin->gc, qp->x1, qp->y1, qp->x2, qp->y2);

    qp->lineq[qp->last].x = qp->x1;
    qp->lineq[qp->last].y = qp->y1;
    qp->last++;
    if (qp->last >= qp->nlines)
	qp->last = 0;

    qp->lineq[qp->last].x = qp->x2;
    qp->lineq[qp->last].y = qp->y2;
    qp->last++;
    if (qp->last >= qp->nlines)
	qp->last = 0;
}
