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
/* $XConsortium: worm.c /main/3 1995/11/02 16:09:04 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * worm.c - draw wiggly worms.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 27-Sep-91: got rid of all malloc calls since there were no calls to free().
 * 25-Sep-91: Integrated into X11R5 contrib dtscreen.
 *
 * Adapted from a concept in the Dec 87 issue of Scientific American.
 *
 * SunView version: Brad Taylor (brad@sun.com)
 * X11 version: Dave Lemke (lemke@ncd.com)
 * xlock version: Boris Putanec (bp@cs.brown.edu)
 *
 * This code is a static memory pig... like almost 200K... but as contributed
 * it leaked at a massive rate, so I made everything static up front... feel
 * free to contribute the proper memory management code.
 * 
 */

#include "dtscreen.h"
#include <math.h>
#include <stdlib.h>

#define MAXCOLORS 64
#define MAXWORMS 64
#define CIRCSIZE 2
#define MAXWORMLEN 50

#define PI 3.14159265358979323844
#define SEGMENTS  36
static int  sintab[SEGMENTS];
static int  costab[SEGMENTS];
static int  init_table = 0;

typedef struct {
    int         xcirc[MAXWORMLEN];
    int         ycirc[MAXWORMLEN];
    int         dir;
    int         tail;
    int         x;
    int         y;
}           wormstuff;

typedef struct {
    int         xsize;
    int         ysize;
    int         wormlength;
    int         monopix;
    int         nc;
    int         nw;
    wormstuff   worm[MAXWORMS];
    XRectangle  rects[MAXCOLORS][MAXWORMS];
    int         size[MAXCOLORS];
}           wormstruct;

int
round(x)
    float       x;
{
    return ((int) floor((double) x));
}


void
worm_doit(pwin, wp, which, color)
    perwindow *pwin;
    wormstruct *wp;
    int         which;
    unsigned long color;
{
    wormstuff  *ws = &wp->worm[which];
    int         x, y;

    ws->tail++;
    if (ws->tail == wp->wormlength)
	ws->tail = 0;

    x = ws->xcirc[ws->tail];
    y = ws->ycirc[ws->tail];
    XClearArea(dsp, pwin->w, x, y, CIRCSIZE, CIRCSIZE, False);

    if (random() & 1) {
	ws->dir = (ws->dir + 1) % SEGMENTS;
    } else {
	ws->dir = (ws->dir + SEGMENTS - 1) % SEGMENTS;
    }

    x = (ws->x + costab[ws->dir] + wp->xsize) % wp->xsize;
    y = (ws->y + sintab[ws->dir] + wp->ysize) % wp->ysize;

    ws->xcirc[ws->tail] = x;
    ws->ycirc[ws->tail] = y;
    ws->x = x;
    ws->y = y;

    wp->rects[color][wp->size[color]].x = x;
    wp->rects[color][wp->size[color]].y = y;
    wp->size[color]++;
}


void
initworm(pwin)
    perwindow *pwin;
{
    int         i, j;
    wormstruct *wp;
    XWindowAttributes xwa;

    if (pwin->data) free(pwin->data);
    pwin->data = (void *)malloc(sizeof(wormstruct));
    memset(pwin->data, '\0', sizeof(wormstruct));
    wp = (wormstruct *)pwin->data;
    wp->nc = pwin->perscreen->npixels;
    if (wp->nc > MAXCOLORS)
	wp->nc = MAXCOLORS;

    wp->nw = batchcount;
    if (wp->nw > MAXWORMS)
	wp->nw = MAXWORMS;

    if (!init_table) {
	init_table = 1;
	for (i = 0; i < SEGMENTS; i++) {
	    sintab[i] = round(CIRCSIZE * sin(i * 2 * PI / SEGMENTS));
	    costab[i] = round(CIRCSIZE * cos(i * 2 * PI / SEGMENTS));
	}
    }
    XGetWindowAttributes(dsp, pwin->w, &xwa);
    wp->xsize = xwa.width;
    wp->ysize = xwa.height;

    if (xwa.width < 100) {
	wp->monopix = BlackPixelOfScreen(pwin->perscreen->screen);
	wp->wormlength = MAXWORMLEN / 10;
    } else {
	wp->monopix = WhitePixelOfScreen(pwin->perscreen->screen);
	wp->wormlength = MAXWORMLEN;
    }

    for (i = 0; i < wp->nc; i++) {
	for (j = 0; j < wp->nw / wp->nc + 1; j++) {
	    wp->rects[i][j].width = CIRCSIZE;
	    wp->rects[i][j].height = CIRCSIZE;
	}
    }
    memset(wp->size, '\0', wp->nc * sizeof(int));

    for (i = 0; i < wp->nw; i++) {
	for (j = 0; j < wp->wormlength; j++) {
	    wp->worm[i].xcirc[j] = wp->xsize / 2;
	    wp->worm[i].ycirc[j] = wp->ysize / 2;
	}
	wp->worm[i].dir = (unsigned) random() % SEGMENTS;
	wp->worm[i].tail = 0;
	wp->worm[i].x = wp->xsize / 2;
	wp->worm[i].y = wp->ysize / 2;
    }

    XClearWindow(dsp, pwin->w);
}


void
drawworm(pwin)
    perwindow *pwin;
{
    int         i;
    wormstruct *wp = (wormstruct *)pwin->data;
    unsigned int wcolor;
    static unsigned int chromo = 0;

    memset(wp->size, '\0', wp->nc * sizeof(int));

    for (i = 0; i < wp->nw; i++) {
	if (!mono && wp->nc > 2) {
	    wcolor = (i + chromo) % wp->nc;

	    worm_doit(pwin, wp, i, wcolor);
	} else
	    worm_doit(pwin, wp, i, 0);
    }

    if (!mono && wp->nc > 2) {
	for (i = 0; i < wp->nc; i++) {
	    XSetForeground(dsp, pwin->gc, pwin->perscreen->pixels[i]);
	    XFillRectangles(dsp, pwin->w, pwin->gc, wp->rects[i],
			    wp->size[i]);
	}
    } else {
	XSetForeground(dsp, pwin->gc, wp->monopix);
	XFillRectangles(dsp, pwin->w, pwin->gc, wp->rects[0],
			wp->size[0]);
    }

    if (++chromo == wp->nc)
	chromo = 0;
}
