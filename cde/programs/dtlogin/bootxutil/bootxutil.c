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
/* $XConsortium: bootxutil.c /main/4 1996/05/08 20:08:17 drk $ */
static char sccsid[] = "@(#)31  1.1  src/cde/cde1/dtlogin/bootxutil/bootxutil.c, desktop, cde41J, 9522A_c 5/31/95 18:45:53";
/*
 *
 *   COMPONENT_NAME:  desktop
 *
 *   FUNCTIONS: usage
 *		dpyinfo
 *		arrow
 *		main
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */


#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>

/*
 * Cursor definitions
 */
#define CURSOR_ARROW     XC_left_ptr

/*
 * Application modes
 */
#define MODE_UNKNOWN     0
#define MODE_DPYINFO     1
#define MODE_ARROW       2

/*
 * Option table
 */
const struct _options {
  char *arg;
  int mode;
} options[] = {
  {"-dpyinfo",         MODE_DPYINFO},
  {"-cursorarrow",     MODE_ARROW},
  { NULL,              MODE_UNKNOWN}};

/*
 * usage()
 *
 * prints a usage statement to stderr and exits
 */
void 
usage(char *name)
{
  fprintf(stderr, "usage: %s -dpyinfo|-cursorarrow\n", name);
  exit(1);
}


/*
 * dpyinfo()
 *
 * prints a string of the form:
 *
 *   let SCREEN_WIDTH=1280;
 *   let SCREEN_HEIGHT=1024;
 *
 */
void
dpyinfo(Display *display, int screen_num)
{
  printf("let SCREEN_WIDTH=%d;\nlet SCREEN_HEIGHT=%d;\n",
          DisplayWidth(display, screen_num),
          DisplayHeight(display, screen_num));
}

/*
 * arrow()
 *
 * changes the root window pointer to an arrow
 */
void
arrow(Display *display, int screen_num)
{
  Cursor cursor;

 /*
  * Create cursor
  */
  cursor = XCreateFontCursor(display, CURSOR_ARROW);
  if (cursor == NULL) exit (1);
  
 /*
  * Display cursor on root window
  */
  XDefineCursor (display, RootWindow(display, screen_num), cursor);  
}

main(int argc, char *argv[])
{
  Display *display;
  int screen_num;
  int mode = MODE_UNKNOWN;

 /*
  * Check usage.
  * 
  *   progname -dpyinfo
  *   progname -arrow
  */
  if (argc == 2) 
  {
    int i = 0;
    while (options[i].arg) 
    {
      if (strcmp(argv[1], options[i].arg) == 0)
      {
        mode = options[i].mode;
        break;
      }
      i++;
    }
  }

 /*
  * Bail on incorrect usage
  */
  if (mode == MODE_UNKNOWN)
  { 
    usage(argv[0]); 
    /* does not return */
  }

 /*
  * Open display connection using DISPLAY env value
  */
  if ((display = XOpenDisplay(NULL)) == NULL) {
    fprintf(stderr, "%s: cannot connect to display '%s'\n", 
            argv[0], XDisplayName(NULL));
    exit(1);
  }

 /*
  * Get default screen
  */
  screen_num = DefaultScreen(display);

  switch(mode)
  {
    case MODE_DPYINFO:   dpyinfo(display, screen_num); break;
    case MODE_ARROW:     arrow(display, screen_num); break;
  }

 /*
  * Flush all pending protocol to server
  */
  XFlush(display);
 
 /*
  * Our job is done.
  */
  XCloseDisplay(display);
  exit(0);
}
