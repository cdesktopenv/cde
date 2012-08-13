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
/* $XConsortium: swarm.c /main/3 1995/11/02 16:08:42 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * swarm.c - swarm of bees for dtscreen, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * Revision History:
 * 31-Aug-90: Adapted from xswarm by Jeff Butterworth. (butterwo@ncsc.org)
 */

#include <stdlib.h>
#include "dtscreen.h"

#define TIMES	4		/* number of time positions recorded */
#define BEEACC	3		/* acceleration of bees */
#define WASPACC 5		/* maximum acceleration of wasp */
#define BEEVEL	11		/* maximum bee velocity */
#define WASPVEL 12		/* maximum wasp velocity */
#define BORDER	50		/* wasp won't go closer than this to the edge */

/* Macros */
#define X(t,b)	(sp->x[(t)*sp->beecount+(b)])
#define Y(t,b)	(sp->y[(t)*sp->beecount+(b)])
#define RAND(v)	((random()%(v))-((v)/2))	/* random number around 0 */

typedef struct {
    int         pix;
    long        startTime;
    int         width;
    int         height;
    int         beecount;	/* number of bees */
    XSegment   *segs;		/* bee lines */
    XSegment   *old_segs;	/* old bee lines */
    short      *x;
    short      *y;		/* bee positions x[time][bee#] */
    short      *xv;
    short      *yv;		/* bee velocities xv[bee#] */
    short       wx[3];
    short       wy[3];
    short       wxv;
    short       wyv;
}           swarmstruct;

void
initswarm(pwin)
    perwindow *pwin;
{
    XWindowAttributes xgwa;
    swarmstruct *sp;
    int         b;

    if (pwin->data) free(pwin->data);
    pwin->data = (void *)malloc(sizeof(swarmstruct));
    memset(pwin->data, '\0', sizeof(swarmstruct));
    sp = (swarmstruct *)pwin->data;
    sp->startTime = seconds();
    sp->beecount = batchcount;

    XGetWindowAttributes(dsp, pwin->w, &xgwa);
    sp->width = xgwa.width;
    sp->height = xgwa.height;

    /* Clear the background. */
    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));
    XFillRectangle(dsp, pwin->w, pwin->gc, 0, 0, sp->width, sp->height);

    /* Allocate memory. */

    sp->segs = (XSegment *) malloc(sizeof(XSegment) * sp->beecount);
    sp->old_segs = (XSegment *) malloc(sizeof(XSegment) * sp->beecount);
    sp->x = (short *) malloc(sizeof(short) * sp->beecount * TIMES);
    sp->y = (short *) malloc(sizeof(short) * sp->beecount * TIMES);
    sp->xv = (short *) malloc(sizeof(short) * sp->beecount);
    sp->yv = (short *) malloc(sizeof(short) * sp->beecount);

    /* Initialize point positions, velocities, etc. */

    /* wasp */
    sp->wx[0] = BORDER + random() % (sp->width - 2 * BORDER);
    sp->wy[0] = BORDER + random() % (sp->height - 2 * BORDER);
    sp->wx[1] = sp->wx[0];
    sp->wy[1] = sp->wy[0];
    sp->wxv = 0;
    sp->wyv = 0;

    /* bees */
    for (b = 0; b < sp->beecount; b++) {
	X(0, b) = random() % sp->width;
	X(1, b) = X(0, b);
	Y(0, b) = random() % sp->height;
	Y(1, b) = Y(0, b);
	sp->xv[b] = RAND(7);
	sp->yv[b] = RAND(7);
    }
}



void
drawswarm(pwin)
    perwindow *pwin;
{
    swarmstruct *sp;
    int         b;

    sp = (swarmstruct *)pwin->data;
    /* <=- Wasp -=> */
    /* Age the arrays. */
    sp->wx[2] = sp->wx[1];
    sp->wx[1] = sp->wx[0];
    sp->wy[2] = sp->wy[1];
    sp->wy[1] = sp->wy[0];
    /* Accelerate */
    sp->wxv += RAND(WASPACC);
    sp->wyv += RAND(WASPACC);

    /* Speed Limit Checks */
    if (sp->wxv > WASPVEL)
	sp->wxv = WASPVEL;
    if (sp->wxv < -WASPVEL)
	sp->wxv = -WASPVEL;
    if (sp->wyv > WASPVEL)
	sp->wyv = WASPVEL;
    if (sp->wyv < -WASPVEL)
	sp->wyv = -WASPVEL;

    /* Move */
    sp->wx[0] = sp->wx[1] + sp->wxv;
    sp->wy[0] = sp->wy[1] + sp->wyv;

    /* Bounce Checks */
    if ((sp->wx[0] < BORDER) || (sp->wx[0] > sp->width - BORDER - 1)) {
	sp->wxv = -sp->wxv;
	sp->wx[0] += sp->wxv;
    }
    if ((sp->wy[0] < BORDER) || (sp->wy[0] > sp->height - BORDER - 1)) {
	sp->wyv = -sp->wyv;
	sp->wy[0] += sp->wyv;
    }
    /* Don't let things settle down. */
    sp->xv[random() % sp->beecount] += RAND(3);
    sp->yv[random() % sp->beecount] += RAND(3);

    /* <=- Bees -=> */
    for (b = 0; b < sp->beecount; b++) {
	int         distance,
	            dx,
	            dy;
	/* Age the arrays. */
	X(2, b) = X(1, b);
	X(1, b) = X(0, b);
	Y(2, b) = Y(1, b);
	Y(1, b) = Y(0, b);

	/* Accelerate */
	dx = sp->wx[1] - X(1, b);
	dy = sp->wy[1] - Y(1, b);
	distance = abs(dx) + abs(dy);	/* approximation */
	if (distance == 0)
	    distance = 1;
	sp->xv[b] += (dx * BEEACC) / distance;
	sp->yv[b] += (dy * BEEACC) / distance;

	/* Speed Limit Checks */
	if (sp->xv[b] > BEEVEL)
	    sp->xv[b] = BEEVEL;
	if (sp->xv[b] < -BEEVEL)
	    sp->xv[b] = -BEEVEL;
	if (sp->yv[b] > BEEVEL)
	    sp->yv[b] = BEEVEL;
	if (sp->yv[b] < -BEEVEL)
	    sp->yv[b] = -BEEVEL;

	/* Move */
	X(0, b) = X(1, b) + sp->xv[b];
	Y(0, b) = Y(1, b) + sp->yv[b];

	/* Fill the segment lists. */
	sp->segs[b].x1 = X(0, b);
	sp->segs[b].y1 = Y(0, b);
	sp->segs[b].x2 = X(1, b);
	sp->segs[b].y2 = Y(1, b);
	sp->old_segs[b].x1 = X(1, b);
	sp->old_segs[b].y1 = Y(1, b);
	sp->old_segs[b].x2 = X(2, b);
	sp->old_segs[b].y2 = Y(2, b);
    }

    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));
    XDrawLine(dsp, pwin->w, pwin->gc,
	      sp->wx[1], sp->wy[1], sp->wx[2], sp->wy[2]);
    XDrawSegments(dsp, pwin->w, pwin->gc, sp->old_segs, sp->beecount);

    XSetForeground(dsp, pwin->gc, WhitePixelOfScreen(pwin->perscreen->screen));
    XDrawLine(dsp, pwin->w, pwin->gc,
	      sp->wx[0], sp->wy[0], sp->wx[1], sp->wy[1]);
    if (!mono && pwin->perscreen->npixels > 2) {
	XSetForeground(dsp, pwin->gc, pwin->perscreen->pixels[sp->pix]);
	if (++sp->pix >= pwin->perscreen->npixels)
	    sp->pix = 0;
    }
    XDrawSegments(dsp, pwin->w, pwin->gc, sp->segs, sp->beecount);
}
