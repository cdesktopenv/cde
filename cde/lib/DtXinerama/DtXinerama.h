/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2013, The Open Group. All rights reserved.
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
 * Jon Trulson, Xi Graphics 4/11/2001
 *
 * $XiGId: DtXinerama.h,v 1.1 2001/04/12 03:01:14 jon Exp $
 *
 * A Xinerama wrapper for CDE
 *
 */

#ifndef DTXINERAMA_H_INCLUDED
#define DTXINERAMA_H_INCLUDED

#include <stdio.h>
#include <X11/Xfuncs.h>

#include <X11/extensions/Xinerama.h>

typedef struct _DtXineramaInfo 
{
  int numscreens;	
  XineramaScreenInfo *ScreenInfo;
} DtXineramaInfo_t, *DtXineramaInfoPtr_t;


DtXineramaInfo_t *_DtXineramaInit(Display *dpy);
Bool _DtXineramaGetScreen(DtXineramaInfo_t *, unsigned int screen,
			  unsigned int *w, unsigned int *h, 
			  unsigned int *xorg, unsigned int *yorg);

#endif /* DTXINERAMA_H_INCLUDED */
