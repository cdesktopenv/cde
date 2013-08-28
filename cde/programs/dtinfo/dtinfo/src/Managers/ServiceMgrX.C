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
 * $TOG: ServiceMgrX.C /main/12 1997/06/18 17:32:19 samborn $
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

#include "UAS.hh"

#define L_Agents

#define C_ServiceMgr
#define C_NodeMgr
#define C_ClientEventMgr
#define C_MessageMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

#include <string.h>

#include "external-api/olias.h"
#include "utility/mmdb_exception.h"

LONG_LIVED_CC(ServiceMgr,service_manager)

#define OLIAS_PROTOCOL_VERSION  1

#define OLIAS_WINDOW_ID_ATOM    "__OLIAS_WINDOW_ID"
#define OLIAS_EVENT_ATOM        "__OLIAS_EVENT"
#define OLIAS_REPLY_ATOM        "__OLIAS_REPLY"

Atom _XA_OLIAS_WINDOW_ID;
Atom _XA_OLIAS_EVENT;
Atom _XA_OLIAS_REPLY;

extern bool g_scroll_to_locator;
extern char g_top_locator[];

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

ServiceMgr::ServiceMgr()
{
#if 0
  // We need to wait for the window to be mapped before doing anything
  XtAddEventHandler (window_system().toplevel(), StructureNotifyMask, False,
		     (XtEventHandler) handle_toplevel_mapped, this);
#endif
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

ServiceMgr::~ServiceMgr()
{
  // Need to stop being the OLIAS server if it's us.

  //  XGrabServer (window_system().display());

  Window current_server =
    XGetSelectionOwner (window_system().display(), _XA_OLIAS_WINDOW_ID);

  ON_DEBUG(printf ("Current OLIAS server is window 0x%lx\n", current_server));
  if (current_server == XtWindow (window_system().toplevel()))
    {
      ON_DEBUG(printf ("Done being the OLIAS server...\n"));
      ON_DEBUG(fflush (stdout));

      // OK to use CurrentTime because there is no chance of contention.
      XSetSelectionOwner (window_system().display(), _XA_OLIAS_WINDOW_ID,
			  None, CurrentTime);
    }
      
  //  XUngrabServer (window_system().display());
}

// /////////////////////////////////////////////////////////////////
// create_atoms
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::create_atoms()
{
  static Boolean created = False;

  if (!created)
    {
      enum { XA_OLIAS_WINDOW_ID_ATOM, XA_OLIAS_EVENT_ATOM,
	     XA_OLIAS_REPLY_ATOM, NUM_ATOMS }; 
      static const char *atom_names[] = { OLIAS_WINDOW_ID_ATOM,
	     OLIAS_EVENT_ATOM, OLIAS_REPLY_ATOM };

      Atom atoms[XtNumber(atom_names)];

      XInternAtoms (window_system().display(), (char**)atom_names,
		    XtNumber((char**)atom_names), False, atoms);

      _XA_OLIAS_WINDOW_ID = atoms[XA_OLIAS_WINDOW_ID_ATOM];
      _XA_OLIAS_EVENT = atoms[XA_OLIAS_EVENT_ATOM];
      _XA_OLIAS_REPLY = atoms[XA_OLIAS_REPLY_ATOM];

      created = True;
    }
}

// /////////////////////////////////////////////////////////////////
// establish_server - see if there's a server, become one if not
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::establish_server()
{
  create_atoms();

  // Operations in this routine must be atomic to avoid race conditions,
  // hence the server grab.

  XGrabServer (window_system().display());

  Window current_server =
    XGetSelectionOwner (window_system().display(), _XA_OLIAS_WINDOW_ID);

  ON_DEBUG(printf ("Current OLIAS server is window 0x%lx\n", current_server));

  // If no current server, we'll become the server.
  if (current_server == None)
    {
      ON_DEBUG(printf ("Attempting to become OLIAS server..."));
      ON_DEBUG(fflush (stdout));
      current_server = XtWindow (window_system().toplevel());

      // OK to use CurrentTime because there is no chance of contention.
      XSetSelectionOwner (window_system().display(), _XA_OLIAS_WINDOW_ID,
			  current_server, CurrentTime);

      // Make sure it worked.
      current_server = XGetSelectionOwner (window_system().display(),
					   _XA_OLIAS_WINDOW_ID);

      if (current_server == XtWindow (window_system().toplevel()))
	{
	  client_event_manager().register_handler(this, window_system().toplevel());
	  ON_DEBUG(puts ("successful!"));
	}
      else
	{
	  ON_DEBUG(puts ("failed!"));
	}
    }

  XUngrabServer (window_system().display());
}

// /////////////////////////////////////////////////////////////////
// get_olias_event
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::get_olias_event (XEvent *event)
{
  // Make sure it's the right event first. 
  if (event->type != ClientMessage ||
      event->xclient.message_type != _XA_OLIAS_EVENT)
    return;

  Atom selection = event->xclient.data.l[2];
  Time time      = event->xclient.data.l[3];

  /* -------- Ask for the client the event from the client. -------- */
  ON_DEBUG(puts ("OLIAS: Received a client display event!"));

  XtGetSelectionValue (window_system().toplevel(), selection,
		       _XA_OLIAS_EVENT,
		       (XtSelectionCallbackProc) receive_olias_event,
		       this, time);		       
}


// /////////////////////////////////////////////////////////////////
// handle_event - dispatch handle application request
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::handle_event(Widget, XEvent *event, Boolean *)
{
  get_olias_event(event);
}

// /////////////////////////////////////////////////////////////////
// process_olias_event
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::process_olias_event (Window client,
				 unsigned char *stream, unsigned long)
{
  /* Return immediately if the selection couldn't be converted. */
  if (stream == NULL)
    return;

  unsigned char event_type = *stream++;
  int len, bufferlen;
  char *locator;		// NOTE: make fixed width and add "mmdb:" to
				// front ? Should eventually go into calling
				// program to determine doc type.

  if (event_type != 1)
    {
      ON_DEBUG(fprintf (stderr, "received unknown event"));
      return;
    }

  // Skip over the defunct infobase name.
  // NOTE:  It should be removed from the olias api.  DJB 
  while (*stream != '\0')
    stream++;
  stream++;
  locator = (char *) stream;

  UAS_Pointer<UAS_Common> d;
  mtry
    {
      if (strchr (locator, ':'))
	{
	  d = UAS_Common::create(locator);
	}
      else
	{
	  bufferlen = strlen("mmdb:LOCATOR=") + strlen(locator) + 1;
	  char *buffer = new char[bufferlen];
	  snprintf (buffer, bufferlen, "mmdb:LOCATOR=%s", locator);
	  d = UAS_Common::create (buffer);
	  if (d != (const int)NULL)
	    {
	      // (evil hack alert) 
	      g_scroll_to_locator = TRUE;
	      len = MIN(strlen(locator), 4096 - 1);
	      *((char *) memcpy(g_top_locator, locator, len) + len) = '\0';
	    }
	  delete [] buffer;
	}
    }
  mcatch (demoException&, demo)
    {
      message_mgr().demo_failure(demo);
    }
  mcatch_any()
    {
      d = NULL;
    }
  end_try;

  if (d != (const int)NULL)
    {
      d->retrieve();
      reply_to_client(client, OLIAS_SUCCESS);
    }
  else
    {
      reply_to_client(client, OLIAS_LOCATOR_NOT_FOUND);
      ON_DEBUG(cerr << "Document not found." << endl);
    }
}


// /////////////////////////////////////////////////////////////////
// receive_olias_event
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::receive_olias_event (Widget, XtPointer client_data,
				 Atom *selection, Atom * /* type */,
				 XtPointer value, unsigned long *length,
				 int * /* format */)
{
  // NOTE: probably should verify type (olias_event_atom) and format 
  Window client =
    XGetSelectionOwner (window_system().display(),*selection);
  ((ServiceMgr *) client_data)->
    process_olias_event (client, (unsigned char *) value, *length);
}


// /////////////////////////////////////////////////////////////////
// reply_to_client
// /////////////////////////////////////////////////////////////////

void
ServiceMgr::reply_to_client (Window client, OliasStatus reply_code)
{
  static XClientMessageEvent client_event;

  // rtp - 19-Apr-95
  //   Before we send a reply, we must determine if the client window
  //   is an existing (valid) window. This is done below by searching
  //   for it's window ID in the list of immediate children of the root
  //   window. If the client window is no longer valid then we don't
  //   send a reply. This method seems overly complex to me but after
  //   consulting with our X gurus I determined that this is the best
  //   (only?) way to accomplish this. This code fixes defect ID 16933.

  //  rCs - 20Sept96
  //  a better way might be to do an XSynch, install an error handler
  //  try a X Command to see if an error is generated, do another,
  //  XSynch, then restore the original Xlib error handler

  Window        root, parent, *children;
  unsigned int  i, nchildren;
  Boolean       client_exists = False;

  // Query root window for all of its immediate children.
  if (XQueryTree(window_system().display(),
                 DefaultRootWindow(window_system().display()),
                 &root, &parent, &children, &nchildren)) {
     // Search for the client window ID in root's list of children.
     // Assumption: Window was created as a toplevel widget within
     // external API client.
     for (i = 0; i < nchildren; i++) {
        if (children[i] == client) {
           client_exists = True;
           break;
        }
     }
     // Free up the children array.
     XFree(children);
  }

  // If not a valid window then punt sending the reply message
  if (client == None || !client_exists)
    return;

  client_event.window = client;
  client_event.type = ClientMessage;
  client_event.message_type = _XA_OLIAS_REPLY;
  client_event.format = 32;
  client_event.data.l[0] = OLIAS_PROTOCOL_VERSION;
  client_event.data.l[1] = reply_code;

  ON_DEBUG(printf ("sending reply to 0x%lx\n", client));

  XSendEvent (window_system().display(), client, False,
	      0L, (XEvent *) &client_event);
  XFlush (window_system().display());
}


// /////////////////////////////////////////////////////////////////
// olias_send_event - internal version for olias-to-olias comm
// /////////////////////////////////////////////////////////////////

OliasStatus
olias_send_event (Widget, OliasEvent *event)
{
  char *buffer = NULL;
  char *locator;
  int len, bufferlen;
  UAS_Pointer<UAS_Common> d;

  switch (event->type)
    {
      case OLIAS_NOOP_EVENT:
        break;

      case OLIAS_DISPLAY_EVENT:
	locator = ((OliasDisplayEvent *) event)->locator;
	ON_DEBUG (printf (">>> external-api display <%s>\n", locator));
	mtry
	  {
	    if (strchr (locator, ':'))
	      {
		d = UAS_Common::create (locator);
	      }
	    else
	      {
		bufferlen = strlen("mmdb:LOCATOR=") + strlen(locator) + 1;
		buffer = new char[bufferlen];
		snprintf (buffer, bufferlen, "mmdb:LOCATOR=%s", locator);
		d = UAS_Common::create (buffer);
		delete [] buffer;
		if (d != (const int)NULL)
		  {
		    // (evil hack alert) 
		    if (locator == NULL)
		      return (OLIAS_TIMEOUT);
		    ON_DEBUG(printf(">>> g_top_locator = %p\n", g_top_locator));
		    if (g_top_locator == NULL)
		      return (OLIAS_TIMEOUT);
                    g_scroll_to_locator = TRUE;
                    len = MIN(strlen(locator), 4096 - 1);
                    *((char *) memcpy(g_top_locator, locator, len) +len) = '\0';
		  }
	      }
	  }
	mcatch (demoException&, demo)
	  {
	    message_mgr().demo_failure(demo);
	  }
	mcatch_any()
	  {
	    d = NULL;
	  }
	end_try;
	if (d != (const int)NULL)
	  {
	    d->retrieve();
	    return (OLIAS_SUCCESS);
	  }
	else
	  {
	    message_mgr().info_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 46, "File a Bug")));
	    return (OLIAS_SUCCESS);
	    // return (OLIAS_LOCATOR_NOT_FOUND);   until help lib fixed
	  }
	break;

      default:
	return (OLIAS_TIMEOUT);
    }

    return (OLIAS_LOCATOR_NOT_FOUND);
}
