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
/* $XConsortium: oliasmsg.c /main/4 1996/10/04 14:57:52 drk $ */
/* **************************************************************
 * 
 * This program is an example of how to use the OLIAS external
 * API to asyncronously message the Browser and tell it to fetch
 * a section at a given locator. The locator might be either of
 * MMDB format (e.g., mmdb:/n/9xihNkB23N9X3cS) or WWW URL format
 * (e.g., http://www.hal.com).
 *
 * In current form there are a couple of important items to note
 * when using the API. First, a prerequisite is that the OLIAS
 * Browser must be running on the desktop before it can process
 * an external message. Next, the API uses properties on the X
 * server to communicate between messaging client (this program)
 * and the OLIAS Browser. If the value of the selection timeout
 * is too short then the client can exit before the Browser is
 * through processing the request. The result is the Browser
 * attempts to send a reply to the client through an invalid
 * widget that was destroyed when the client passed away. Under
 * this condition the Browser crashes in Xlib. A workaround for
 * this problem (until it is fixed) is to set the selection
 * timeout value long enough to avoid this condition.
 * 
 * ************************************************************** */
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "olias.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifndef BUFSIZE
#define BUFSIZE 1024
#endif

static  char  loc_buf[BUFSIZE] = "NO_LOCATOR_SPECIFIED";
static  long  sel_timeout = 60000;  /* default is 60 secs */

/* **************************************************************
 * print program usage and exit with error code
 * ************************************************************** */

static void
print_usage_and_exit (char *pname, char *arg)
{
  if (arg)
    fprintf(stderr, "\nUnknown argument: %s\n\n", arg);
  fprintf(stderr, "Usage: %s [-to secs] locator\n", pname);
  fprintf(stderr, "   -to     : Selection timeout in seconds\n");
  fprintf(stderr, "   locator : MMDB locator or WWW URL\n");
  fprintf(stderr, "\nExample: %s http://www.hal.com\n\n", pname);
  exit (-1);
}

/* **************************************************************
 * parse program arguments
 * ************************************************************** */

static void
parse_args (int argc, char *argv[])
{
  int i. len;

  if (argc == 1)
     print_usage_and_exit (argv[0], NULL);

  for (i=1; i < argc; i++) {

    if (argv[i][0] == '-') {

      if (!(strncmp(argv[i], "-to", 2))) {
        if (argv[++i])
          sel_timeout = atoi(argv[i]) * 1000;
        else    
          print_usage_and_exit(argv[0], NULL);
      }
      else
        print_usage_and_exit(argv[0], argv[i]);
    }
    else
      len = MIN(strlen(argv[i]), BUFSIZE -1);
      *((char *) memcpy (loc_buf, argv[i], len) + len) = '\0';

  }

}

/* **************************************************************
 * main program
 * ************************************************************** */

int
main (int argc, char *argv[])
{
  Widget             toplevel;
  XtAppContext       app_context;
  OliasStatus        olias_status;
  OliasDisplayEvent  event;
  int                exit_status = 0;
  char              *p;

  /* parse input arguments */
  parse_args(argc, argv);

  /* init x toolkit internals using varargs argument style */
  toplevel = XtVaAppInitialize (&app_context, "OliasDpy", NULL,
                                0, &argc, argv, NULL, 
                                XtNmappedWhenManaged, False, NULL);

  /* make width & height nonzero; (req'd on solaris) */
  XtVaSetValues (toplevel, XtNwidth, 1, XtNheight, 1, NULL);

  /* realize the widget */
  XtRealizeWidget (toplevel);

  /* set the selection timeout */
  XtAppSetSelectionTimeout(app_context, sel_timeout);

  /* trim potential trailing space from locator */
  for (p = loc_buf; !isspace(*p) && *p != '\0'; p++);
  *p = '\0';

  /* send a display message to the olias browser */
  event.type = OLIAS_DISPLAY_EVENT;
  event.infobase = "";
  event.locator = loc_buf;
  olias_status = olias_send_event (toplevel, (OliasEvent *) &event);

  /* if not successful then report the problem */
  if (olias_status != OLIAS_SUCCESS) {
      switch (olias_status) {
	  case OLIAS_TIMEOUT:
	    fprintf (stderr, "\nWARNING, either:\n");
            fprintf (stderr, " 1. The Browser is not running at this time.\n");
            fprintf (stderr, "         OR\n");
            fprintf (stderr, " 2. A selection timeout occurred while waiting for the Browser reply.\n");
            fprintf (stderr, "\nSuggestions\n");
            fprintf (stderr, " Case 1: Start the Browser before messaging through this client.\n");
            fprintf (stderr, " Case 2: Restart Browser and then increase the selection timeout\n");
            fprintf (stderr, "         using the '-to' command line option.\n\n");
          break;

	  case OLIAS_LOCATOR_NOT_FOUND:
	    fprintf (stderr, "Locator '%s' not found.\n", loc_buf);
          break;

          default:
          break;
      }
      exit_status = -1;
  }

  /* see ya! */
  exit (exit_status);
}
