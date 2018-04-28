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
/* $XConsortium: oliasdpy.c /main/3 1996/06/11 16:48:09 cde-hal $ */
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

Widget toplevel;

/* **************************************************************
 * display
 * ************************************************************** */

static void
display (char *locator)
{
  OliasStatus status;
  char *p = locator;
  static OliasDisplayEvent event;

  /* trim trailing space from locator */
  while (!isspace(*p) && *p != '\0')
    p++;
  *p = '\0';

  /* send a display message to the olias browser */
  event.type = OLIAS_DISPLAY_EVENT;
  event.infobase = "";
  event.locator = locator;
  status = olias_send_event (toplevel, (OliasEvent *) &event);
  if (status != OLIAS_SUCCESS)
    {
      switch (status)
	{
	  case OLIAS_TIMEOUT:
	    fprintf (stderr, "display call timed out\n");
	    break;
	  case OLIAS_LOCATOR_NOT_FOUND:
	    fprintf (stderr, "locator not found\n");
	    break;
	}
    }
}


/* **************************************************************
 * parse_cmd
 * ************************************************************** */

static void
parse_cmd (char *command)
{
  char *p = command, *arg;

  /* skip leading space */
  while (isspace (*p) && *p != '\0')
    p++;
  if (*p == '\0')
    return;
  command = p;

  /* skip chars to next space */
  while (!isspace(*p) && *p != '\0')
    p++;
  if (*p != '\0')
    *p++ = '\0';

  /* skip space folling the command */
  while (isspace(*p) && *p != '\0')
    p++;
  arg = p;

  /* handle command */
  if (strcmp (command, "display") == 0)
    {
      if (*arg != '\0')
	display (arg);
      else
	fprintf (stderr, "usage: display locator\n");
    }
  else if (strcmp (command, "help") == 0)
    {
      fprintf (stderr, "You think you need help?\n");
      fprintf (stderr, "  (hint: try the display command)\n");
    }
  else if (strcmp (command, "quit") == 0)
    {
      exit (0);
    }
  else
    fprintf (stderr, "unknown command: %s\n", command);
}


/* **************************************************************
 * get_input
 * ************************************************************** */

static void
get_input (XtPointer client_data, int *fid, XtInputId *id)
{
  static char buf[BUFSIZE];
  static int pos = 0;
  int size;

  if (size = read (*fid, buf+pos, BUFSIZE-pos))
    {
      static int i;
      pos += size;
      /* look for a newline in the input */
      while (i < pos)
	{
	  while (buf[i] != '\n' && i < pos)
	    i++;
	  if (buf[i] == '\n')
	    {
	      buf[i] = '\0';
	      parse_cmd (buf);
	      /* shift the buffer back if more data remains */
	      if (i + 1 < pos)
		memcpy (buf, buf+i+1, pos-i);
              pos = pos - i - 1;
	      i = 0;
	    }
	}
    }
}


/* **************************************************************
 * main
 * ************************************************************** */

int
main (int argc, char *argv[])
{
  XtAppContext app_context;

  toplevel =
   XtVaAppInitialize (&app_context, "OliasDpy", NULL, 0, &argc, argv, NULL, 
                      XtNmappedWhenManaged, False, NULL);

  XtAppAddInput (app_context, fileno(stdin), (XtPointer) XtInputReadMask, 
                 get_input, NULL);

  XtRealizeWidget (toplevel);

  XtAppMainLoop (app_context);

  exit (0);
}
