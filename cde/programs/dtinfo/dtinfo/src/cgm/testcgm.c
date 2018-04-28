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
/* $XConsortium: testcgm.c /main/6 1996/06/11 16:10:02 cde-hal $ */
/* test module to try the CGM =->pixmap library */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "spec.h"
#include "GraphicsP.h"

main(int argc, char **argv)
{
  XGCValues myStruct;
  unsigned long mask;
  Display *display;
  int scr_num;
  Window win;
  XWindowAttributes wa;
  Dimension ret_width = 0, ret_height = 0;
  Pixel *ret_colors;
  int ret_number, pageNo;
  GC myGC;
  Pixmap myPixmap = NULL;
  char *fileName;
  int i;
  _DtGrStream stream ;
  int status ;
  _DtGrContext gr_context ;

  if (!(display = XOpenDisplay(NULL))) {
    fprintf(stderr, "couldn't open X display\n");
    exit(1);
  }
  scr_num = DefaultScreen(display);
  win = XCreateSimpleWindow(display, RootWindow(display, scr_num), 0, 0,
			    512, 512, 2,
			    BlackPixel(display, scr_num),
			    WhitePixel(display, scr_num));
  XSetStandardProperties(display, win, "VCGM", "VCGM", NULL, NULL, 0,
			 NULL);
  XMapWindow(display, win);
  /* find out what we actually got */
  XGetWindowAttributes(display, win, &wa);
  XFlush(display);
  /* make a reasonable GC */
  mask = GCForeground | GCBackground;
  myStruct.foreground = BlackPixel(display, scr_num);
  myStruct.background = WhitePixel(display, scr_num);
  myGC = XCreateGC(display, win, mask, &myStruct);
  /* check on the filename */
  fileName = (argc > 1) ? argv[1] : "test.cgm";
  /* and pagenumber */
  if ((argc < 3) || !sscanf(argv[2], " %d", &pageNo)) pageNo = 1;

  /* first register our converter */
  _DtGrRegisterConverter("CGM",  processCGM, NULL, NULL, NULL);


  /* set up our stream */
  stream.type = _DtGrFILE ;
  stream.source.file.filename = fileName ;
  stream.source.file.fileptr = fopen(stream.source.file.filename, "r") ;

  gr_context.image_type = NULL;
  gr_context.context = NULL;

  /* get the pixmap we want */
  status = _DtGrLoad (&stream, NULL,
		      ScreenOfDisplay(display, scr_num),
		      wa.depth,
		      wa.colormap,
		      wa.visual,
		      BlackPixelOfScreen(ScreenOfDisplay(display, scr_num)),
		      WhitePixelOfScreen(ScreenOfDisplay(display, scr_num)),
		      myGC,
		      _DtGrCOLOR, True,
		      &ret_width,
		      &ret_height,
		      100,
		      &myPixmap, NULL,
		      &ret_colors,
		      &ret_number, 
		      &gr_context);

  /* exit if didn't get a Pixmap */
  if ((status != _DtGrSUCCESS) || !myPixmap) {
    fprintf(stderr, "no Pixmap returned!\n");
    exit(2);
  } else {
    fprintf(stderr, "got Pixmap, width = %d, height = %d\n", ret_width,
	    ret_height);
  }

  /* wait for some tty input before we copy the pixmap  */
  fprintf(stderr, "hit return to see Pixmap!");
  scanf("%*c");
  /* display the Pixmap */
  XCopyArea(display, myPixmap, win, myGC, 0, 0, ret_width, ret_height,
	    0, 0);
  XFlush(display);
  /* wait for some tty input before we terminate */
  fprintf(stderr, "hit return to terminate!");
  scanf("%*c");
}


