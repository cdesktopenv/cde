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
/*
 * $XConsortium: comm.c /main/4 1996/10/04 15:05:10 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#include "oliasP.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#ifdef _IBMR2
#include <sys/select.h>
#endif

extern char *sys_errlist[];


static OliasEvent *current_event;
static int reply_status;
#define NO_REPLY 0

static void wait_for_reply (Widget);

/* **************************************************************
 * encode_display_event
 * ************************************************************** */

static void
encode_display_event (OliasDisplayEvent *event,
		      XtPointer *value, unsigned long *length, int *format)
{
  unsigned char *stream;
  int len;
  
  *length = 1 + strlen (event->infobase) + strlen (event->locator) + 2;
  /* Xt will free this memory. */
  *value = (XtPointer) malloc (*length);
  stream = (unsigned char *)*value;
  *format = 8;

  /* Encode event in packed byte format. */
  *stream++ = (unsigned char) event->type;
  len = strlen (event->infobase);
  *((char *) memcpy(stream, event->infobase, len) + len) = '\0';
  stream += len + 1;
  len = strlen (event->locator);
  *((char *) memcpy(stream, event->locator, len) + len) = '\0';
}


/* **************************************************************
 * encode_event
 * ************************************************************** */

static Boolean
encode_event (OliasEvent *event,
		XtPointer *value, unsigned long *length, int *format)
{
  switch (event->type)
    {
    case OLIAS_DISPLAY_EVENT:
      encode_display_event (&event->display_event, value, length, format);
      break;

    default:
      fprintf (stderr, "olias_send_event: Unknown event %d\n", event->type);
      return (False);
      break;
    }
  return (True);
}


/* **************************************************************
 * _olias_convert_event
 * ************************************************************** */

Boolean
_olias_convert_event (Widget w, Atom *selection, Atom *target, Atom *type,
		      XtPointer *value, unsigned long *length, int *format)
{
  ON_DEBUG (printf ("Client received conversion request.\n"));

  if (*target != _olias_event_atom)
    {
      char *target_name = XGetAtomName (XtDisplay(w), *target);
      fprintf (stderr, "_olias_convert_event: target %s is invalid\n",
	       target_name);
      XFree (target_name);
      return (False);
    }

  *type = *target;
  return (encode_event (current_event, value, length, format));
}


/* **************************************************************
 * olias_event
 * ************************************************************** */

OliasStatus
olias_send_event (Widget toplevel, OliasEvent *event)
{
  Window olias_server;
  Display *display = XtDisplay (toplevel);
  Time time = XtLastTimestampProcessed (display);
  static XClientMessageEvent client_event;
  static first_time = True;

  if (first_time)
    {
      _olias_initialize (toplevel);
      first_time = False;
    }

  current_event = event;

  olias_server = XGetSelectionOwner (display, _olias_window_id_atom);

  ON_DEBUG (printf ("OLIAS server = 0x%lx\n", olias_server));

  if (olias_server == None)
    {
      /* fprintf (stderr, "OLIAS server not running or inaccessible.\n"); */
      return (OLIAS_TIMEOUT);
    }

  client_event.window = olias_server;
  client_event.type = ClientMessage;
  client_event.message_type = _olias_event_atom;
  client_event.format = 32;
  client_event.data.l[0] = OLIAS_PROTOCOL_VERSION;  /* version            */
  client_event.data.l[1] = 0;	                    /* event minor code   */
  client_event.data.l[2] = _olias_client_atom;      /* client's selection */
  client_event.data.l[3] = time;                    /* timestamp          */
  client_event.data.l[4] = XtWindow (toplevel);	    /* client's window    */

  /* NOTE: need to handler errors! */

  XSendEvent (display, olias_server, False, 0L, (XEvent *) &client_event);
  XFlush (display);

  wait_for_reply (toplevel);
  
  return (reply_status);
}


/* **************************************************************
 * _olias_handle_reply - deal with reply client message event
 * ************************************************************** */

void
_olias_handle_reply (Widget w, XtPointer client_data,
		     XEvent *event, Boolean *cont)
{
  ON_DEBUG (puts ("got non-maskable"));
  if (event->type != ClientMessage ||
      event->xclient.message_type != _olias_reply_atom)
    return;

  reply_status = event->xclient.data.l[1];

  ON_DEBUG (puts ("got reply from OLIAS"));
}


/* **************************************************************
 * check_event - return true if event needs to be dispatched
 * ************************************************************** */

static Boolean
check_event (Display *display, XEvent *event, XtPointer client_data)
{
  /* printf ("%d\n", event->type); */
  /* Select events concerning communication with OLIAS or exposures. */
  switch (event->type)
    {
      case SelectionRequest:
      case ClientMessage:
      case Expose:
      case GraphicsExpose:
      case NoExpose:
      case MapNotify:
      case UnmapNotify:
      case PropertyNotify:
      case VisibilityNotify:
      case ResizeRequest:
      case CirculateNotify:
      case CirculateRequest:
        return (True);
      default:
        return (False);
    }
}


/* **************************************************************
 * wait_for_reply
 * ************************************************************** */

static void
wait_for_reply (Widget toplevel)
{
  Display *display = XtDisplay (toplevel);
  int status;
  int width;
  fd_set readfds;
  struct timeval timeout, current_time, start_time, time_spent;
  /* selection timeout is in milliseconds */
  unsigned long stimeout =
    XtAppGetSelectionTimeout (XtWidgetToApplicationContext (toplevel));
  XEvent event;

  width = ConnectionNumber (display) + 1;
  
  FD_ZERO (&readfds);
  FD_SET (ConnectionNumber (display), &readfds);

  /* convert to seconds */
  timeout.tv_sec = stimeout / 1000;
  /* convert remainder to microseconds */
  timeout.tv_usec = (stimeout % 1000) * 1000;

  gettimeofday (&start_time, NULL);
  
  /* -------- Wait for some activity on the connection. -------- */

  reply_status = NO_REPLY;
  while (reply_status == NO_REPLY)
    {
      errno = 0;
      status = select (width, &readfds, NULL, NULL, &timeout);

      /* Check for timeout. */
      if (status == 0)
	{
	  ON_DEBUG (puts (">> timed out waiting for reply"));
	  reply_status = OLIAS_TIMEOUT;
	  break;
	}

      /* Check for error conditions. */
      if (status == -1)
	{
	  /* Adjust the timeout value first. */
	  gettimeofday (&current_time, NULL);

	  /* See if we need to borrow from seconds. */
	  if (current_time.tv_usec < start_time.tv_usec)
	    {
	      time_spent.tv_sec = current_time.tv_sec - start_time.tv_sec - 1;
	      time_spent.tv_usec =
		1000000 + current_time.tv_usec - start_time.tv_usec;
	    }
	  else
	    {
	      time_spent.tv_sec = current_time.tv_sec - start_time.tv_sec;
	      time_spent.tv_usec = current_time.tv_usec - start_time.tv_usec;
	    }

	  start_time = current_time;

	  /* Check for time expired. */
	  if (time_spent.tv_sec > timeout.tv_sec ||
	      (time_spent.tv_sec == timeout.tv_sec &&
	       time_spent.tv_usec > timeout.tv_sec))
	    {
	      ON_DEBUG (puts (">> timed out waiting for reply"));
	      reply_status = OLIAS_TIMEOUT;
	      break;
	    }

	  /* Subtract time spent from timeout. */
	  if (timeout.tv_usec < time_spent.tv_usec)
	    {
	      timeout.tv_sec = timeout.tv_sec - time_spent.tv_sec - 1;
	      timeout.tv_usec =
		1000000 + timeout.tv_usec - time_spent.tv_usec;
	    }
	  else
	    {
	      timeout.tv_sec = timeout.tv_sec - time_spent.tv_sec;
	      timeout.tv_usec = timeout.tv_usec - time_spent.tv_usec;
	    }

	  if (errno != EINTR)
	    {
	      Cardinal num_params = 1;
	      XtAppWarningMsg (XtWidgetToApplicationContext (toplevel),
			       "communicationsError", "select",
			       "Olias API", "Select failed: %s",
			       &sys_errlist[errno], &num_params);
	      continue;
	    }
	  continue;
	}

      /* Something came in on the socket. */
      while (reply_status == NO_REPLY &&
	     XCheckIfEvent (display, &event, (Bool(*)()) check_event, NULL))
	{
	  /* puts ("d"); */
	  XtDispatchEvent (&event);
	  /* Stop this madness if something updated the reply_status. */
	}
      XFlush (display);
    }
}
