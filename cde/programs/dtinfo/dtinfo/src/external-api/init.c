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
 * $TOG: init.c /main/4 1997/06/18 17:32:40 samborn $
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

Atom _olias_window_id_atom;
Atom _olias_event_atom;
Atom _olias_reply_atom;
Atom _olias_client_atom;

/* **************************************************************
 * _olias_initialize - initialize olias interface
 * ************************************************************** */

void
_olias_initialize (Widget toplevel)
{
  enum { XA_OLIAS_WINDOW_ID_ATOM, XA_OLIAS_EVENT_ATOM,
	 XA_OLIAS_REPLY_ATOM, NUM_ATOMS }; 
  static char* atom_names[] = { OLIAS_WINDOW_ID_ATOM, OLIAS_EVENT_ATOM,
				OLIAS_REPLY_ATOM }; 
  static char buf[32];

  Atom atoms[XtNumber(atom_names)];
    
  /* -------- Create standard communication properties. -------- */

  XInternAtoms(XtDisplay (toplevel), atom_names, XtNumber(atom_names),
	       False, atoms);
  _olias_window_id_atom = atoms[XA_OLIAS_WINDOW_ID_ATOM];
  _olias_event_atom = atoms[XA_OLIAS_EVENT_ATOM];
  _olias_reply_atom = atoms[XA_OLIAS_REPLY_ATOM];

  /* -------- Create a unique selection property for this client. -------- */

  snprintf (buf, sizeof(buf), OLIAS_CLIENT_ATOM_FMT, XtWindow (toplevel));
  _olias_client_atom = XInternAtom (XtDisplay (toplevel), buf, False);

  /* -------- Set up communication handling routines. -------- */

  /* Ok to use CurrentTime because contention not possible. */
  XtOwnSelection (toplevel, _olias_client_atom, CurrentTime,
		  _olias_convert_event, NULL, NULL);

  XtAddEventHandler (toplevel, None, True, _olias_handle_reply, NULL);
}
