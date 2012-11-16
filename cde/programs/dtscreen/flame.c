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
/* $XConsortium: flame.c /main/3 1995/11/02 16:07:01 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * flame.c - recursive fractal cosmic flames.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 27-Jun-91: vary number of functions used.
 * 24-Jun-91: fixed portability problem with integer mod (%).
 * 06-Jun-91: Written. (received from Scott Graves, spot@cs.cmu.edu).
 */

#include "dtscreen.h"
#include <math.h>
#include <stdlib.h>

#define MAXTOTAL	10000
#define MAXBATCH	10
#define MAXLEV		4

typedef struct {
    double      f[2][3][MAXLEV];/* three non-homogeneous transforms */
    int         max_levels;
    int         cur_level;
    int         snum;
    int         anum;
    int         width, height;
    int         num_points;
    int         total_points;
    int         pixcol;
    perwindow  *pwin;
    XPoint      pts[MAXBATCH];
}           flamestruct;

static short
halfrandom(mv)
    int         mv;
{
    static short lasthalf = 0;
    unsigned long r;

    if (lasthalf) {
	r = lasthalf;
	lasthalf = 0;
    } else {
	r = random();
	lasthalf = r >> 16;
    }
    return r % mv;
}

void
initflame(pwin)
    perwindow      *pwin;
{
    XWindowAttributes xwa;
    flamestruct *fs;

    if (pwin->data) free(pwin->data);
    pwin->data = (void *)malloc(sizeof(flamestruct));
    memset(pwin->data, '\0', sizeof(flamestruct));
    fs = (flamestruct *)pwin->data;

    srandom(time((time_t *) 0));

    XGetWindowAttributes(dsp, pwin->w, &xwa);
    fs->width = xwa.width;
    fs->height = xwa.height;

    fs->max_levels = batchcount;
    fs->pwin = pwin;

    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));
    XFillRectangle(dsp, pwin->w, pwin->gc, 0, 0, fs->width, fs->height);

    if (pwin->perscreen->npixels > 2) {
      fs->pixcol = halfrandom(pwin->perscreen->npixels);
      XSetForeground(dsp, pwin->gc, pwin->perscreen->pixels[fs->pixcol]);
    } else {
      XSetForeground(dsp, pwin->gc, WhitePixelOfScreen(pwin->perscreen->screen));
    }
}

static      Bool
recurse(fs, x, y, l)
    flamestruct *fs;
    register double x, y;
    register int l;
{
    int         i;
    double      nx, ny;

    if (l == fs->max_levels) {
	fs->total_points++;
	if (fs->total_points > MAXTOTAL)	/* how long each fractal runs */
	    return False;

	if (x > -1.0 && x < 1.0 && y > -1.0 && y < 1.0) {
	    fs->num_points++;
	    if (fs->num_points > MAXBATCH) {	/* point buffer size */
		XDrawPoints(dsp, fs->pwin->w, fs->pwin->gc, fs->pts,
			    fs->num_points, CoordModeOrigin);
		fs->num_points = 0;
	    }
	}
    } else {
	for (i = 0; i < fs->snum; i++) {
	    nx = fs->f[0][0][i] * x + fs->f[0][1][i] * y + fs->f[0][2][i];
	    ny = fs->f[1][0][i] * x + fs->f[1][1][i] * y + fs->f[1][2][i];
	    if (i < fs->anum) {
		nx = sin(nx);
		ny = sin(ny);
	    }
	    if (!recurse(fs, nx, ny, l + 1))
		return False;
	}
    }
    return True;
}


void
drawflame(pwin)
    perwindow *pwin;
{
    flamestruct *fs = (flamestruct *)pwin->data;

    int         i, j, k;
    static int  alt = 0;

    if (!(fs->cur_level++ % fs->max_levels)) {
	XClearWindow(dsp, fs->pwin->w);
	alt = !alt;
    } else {
	if (pwin->perscreen->npixels > 2) {
	    XSetForeground(dsp, pwin->gc,
			   pwin->perscreen->pixels[fs->pixcol]);
	    if (--fs->pixcol < 0)
		fs->pixcol = pwin->perscreen->npixels - 1;
	}
    }

    /* number of functions */
    fs->snum = 2 + (fs->cur_level % (MAXLEV - 1));

    /* how many of them are of alternate form */
    if (alt)
	fs->anum = 0;
    else
	fs->anum = halfrandom(fs->snum) + 2;

    /* 6 dtfs per function */
    for (k = 0; k < fs->snum; k++) {
	for (i = 0; i < 2; i++)
	    for (j = 0; j < 3; j++)
		fs->f[i][j][k] = ((double) (random() & 1023) / 512.0 - 1.0);
    }
    fs->num_points = 0;
    fs->total_points = 0;
    (void) recurse(fs, 0.0, 0.0, 0);
    XDrawPoints(dsp, pwin->w, pwin->gc,
		fs->pts, fs->num_points, CoordModeOrigin);
}
