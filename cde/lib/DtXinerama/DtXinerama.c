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
 * $XiGId: DtXinerama.c,v 1.1 2001/04/12 03:01:14 jon Exp $
 *
 * A Xinerama wrapper for CDE
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "DtXinerama.h"

/* return a DtXineramaInfo_t (or NULL if no Xinerama) available */

DtXineramaInfo_t *_DtXineramaInit(Display *dpy)
{
  DtXineramaInfo_t *tmpDtXI = NULL;
  XineramaScreenInfo *XinerScrnInfo = NULL;
  int number = 0;

  if (!dpy)
    return(NULL);

  XinerScrnInfo = XineramaQueryScreens(dpy, &number);

  if (number <= 0 || XinerScrnInfo == NULL) /* then we don't have it */
    return(NULL);

				/* allocate some space for it */
  if ((tmpDtXI = (DtXineramaInfo_t *)malloc(sizeof(DtXineramaInfo_t))) == NULL)
    {				/* malloc failure */
#ifdef DEBUG
      fprintf(stderr, "_DtXineramaInit: malloc failed\n");
#endif
      
      free(XinerScrnInfo);
      return(NULL);
    }

  tmpDtXI->numscreens = number;
  tmpDtXI->ScreenInfo = XinerScrnInfo;

  return(tmpDtXI);
}


/* Return w, h, xorg, and yorg for the specified screen.  Return True */
/* if a valid screen, False otherwise */
Bool _DtXineramaGetScreen(DtXineramaInfo_t *DtXI, unsigned int screen,
			  unsigned int *w, unsigned int *h, 
			  unsigned int *xorg, unsigned int *yorg)
{

  if (DtXI == NULL)
    return(False);

  if (DtXI->numscreens == 0)
    return(False);		/* no screens or no Xinerama */

  if (screen < 0 || screen >= DtXI->numscreens)
    return(False);		/* invalid screen */

				/* now get the info from the XinerInfo */
				/* struct and return it */

  if (w != NULL)
    *w = (DtXI->ScreenInfo[screen]).width;
  if (h != NULL)
    *h = (DtXI->ScreenInfo[screen]).height;
  if (xorg != NULL)
    *xorg = (DtXI->ScreenInfo[screen]).x_org;
  if (yorg != NULL)
    *yorg = (DtXI->ScreenInfo[screen]).y_org;

  return(True);
}
  
