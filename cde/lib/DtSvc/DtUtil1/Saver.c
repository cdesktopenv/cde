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
/* $XConsortium: Saver.c /main/8 1996/11/21 19:56:41 drk $ */
/*                                                                            *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                           *
 * (c) Copyright 1993, 1994 International Business Machines Corp.             *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                            *
 * (c) Copyright 1993, 1994 Novell, Inc.                                      *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        Saver.c
 **
 **  Description:
 **  -----------
 **  This file contains public and private screen saver utilities.
 **
 **  Public:
 **    DtSaverGetWindows() - return array of windows on which saver can draw
 **
 **  Private:
 **    _DtSaverStart() - launch specified screen saver
 **    _DtSaverStop() - kill specified screen saver
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <stdlib.h>
#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <X11/Intrinsic.h>
#include <Saver.h>
#include <SaverP.h>
#include <Dt/Action.h>
#include "DtSvcLock.h"

/*
 * Constants global to this file.
 */
#define DT_SAVER_MAX_SCREENS 10

struct saver_state {
  unsigned short serial;
  Window xid;
  struct saver_state *next; 
};

static Atom xa_saver_register;

static struct saver_state saver_list = {0, (Window)0, NULL};

/*
 * Local functions.
 */
static void RegisterSaverCB(
  Widget w, 
  XtPointer client_data, 
  XEvent *event,
  Boolean *continue_to_dispatch
);


/*************************************<->*************************************
 *
 *  _DtSaverStart() - start a screen saver
 *
 *  Description:
 *  -----------
 *  _DtSaverStart() is one of a suite of screen saver API's used in the
 *  desktop. These APIs are:
 *
 *  _DtSaverStart() starts a screen saver (private)
 *  _DtSaverStop() stops a screen saver (private)
 *  DtSaverGetWindows() return array of windows on which saver can draw
 *
 *  The _DtSaverStart() API allocates a state variable for the screen saver
 *  which contains a serial number and NIL window ID. A list of these state
 *  variables is maintained, one for each call to _DtSaverStart(). 
 *  DtSaverStart() then sets up the DTSAVERINFO environment variable containing
 *  the serial number, window count and window list provided. Finally, it
 *  launches the provided action and returns an opaque pointer to the state 
 *  variable. The action is expected to be a screen saver which makes use 
 *  of the DtSaverGetWindows() API.
 *    
 *  When the screen saver starts, it calls the DtSaverGetWindows() API. From
 *  the screen saver perspective, the API returns an array of windows on which
 *  the screen saver can draw. To to this, the API obtains the DTSAVERINFO
 *  environment variable, and parses out the window array. The API also 
 *  creates a window and sends a ClientMessage to the first DTSAVERINFO window
 *  containing the serial number and newly created window id.
 *
 *  RegisterSaverCB() is a callback called when the ClientMessage arrives from
 *  a screen saver. This callback first searches the screen saver state list
 *  by serial number. If a state variable is not found, RegisterSaverCB() 
 *  assumes the screen saver must have been stopped by a call to 
 * _DtSaverStop(), so kills the client via XKillClient() using the window id
 *  provided in the message. If the state variable is located, 
 *  RegisterSaverCB() stores the window id in the state variable.
 *  
 *  _DtSaverStop() searches the screen saver list using the serial number
 *  provided in the input state variable. It should always be found. When
 *  found, if the state variable window id is set, _DtSaverStop() kills the
 *  screen saver client via XKillClient(), deletes the state variable from
 *  the list and deallocates the state variable.
 *
 *  Inputs:
 *  ------
 *  display - display structure
 *  drawArea - array of widgets to be stored drawn upon by saver
 *  count - number of elements in drawArea array
 *  saverAction - screen saver action name to invoke
 *  wAction - widget on which possible DtActionInvoke() errors should display
 * 
 *  Outputs:
 *  -------
 * 
 *  Return:
 *  -------
 *  state - pointer to opaque state structure
 *
 *  Comments:
 *  --------
 *  This function uses DtActionInvoke() to launch an action. As a result,
 *  the caller is responsible for loading and maintaining the action database
 *  using the DtDbLoad() function and procedures. The caller
 *  must call _DtSaverStop() to terminate screen saver
 * 
 *************************************<->***********************************/
void *
_DtSaverStart(
  Display *display,
  Widget *drawArea,
  int count,
  char *saverAction,
  Widget wAction)
{
  static char envdata[(DT_SAVER_MAX_SCREENS * 12) + 20];
  struct saver_state *state;
  struct saver_state *p;
  int i;

  _DtSvcProcessLock();
 /*
  * If first time in, insert envdata in process environment.
  */
  if (saver_list.serial == 0)
  {
#if !defined(__linux__) && !defined(CSRG_BASED)
    /* JET - how can this ever work anyway? */
    putenv(envdata);
    envdata[0] = '\0';
#endif
    xa_saver_register = XInternAtom(display, "_DT_SAVER_REGISTER", False);
  }

 /* 
  * Add event handler (it might already be there - that's ok).
  */
  XtAddEventHandler(drawArea[0], 0, True, RegisterSaverCB, NULL);

 /*
  * Allocate state structure for this saver.
  */
  if (!(state = (struct saver_state *)malloc(sizeof(struct saver_state))))
  {
    _DtSvcProcessUnlock();
    return(NULL);
  }

 /*
  * Initialize state structure and append to saver_list.
  */
  state->serial = saver_list.serial++;
  state->xid = (Window)0;
  state->next = NULL;
  p = &saver_list;
  while (p->next != NULL)
  {
    p = p->next;
  }
  p->next = state;

 /*
  * Set up environment. It will look like:
  *   DTSAVERINFO="<serial> <count> <win0> <win1> ... <winN>"
  */
  sprintf(envdata, "DTSAVERINFO=%u %i %lx",
          state->serial, count, XtWindow(drawArea[0]));
  for (i = 1; i < count; i++)
  {
    char *pe = envdata + strlen(envdata);
    sprintf(pe, " %lx", XtWindow(drawArea[i]));
  }

#if defined(__linux__) || defined(CSRG_BASED)
  putenv(envdata);
#endif

  _DtSvcProcessUnlock();

  /*
  * Launch saver.
  */
  DtActionInvoke(wAction, saverAction, NULL, 0,
                 NULL, NULL, NULL, 0, NULL, NULL);

 /*
  * Return array as state information.
  */
  return((void *)state);
}


/*************************************<->*************************************
 *
 *  _DtSaverStop() - stop a screen saver
 *
 *  Description:
 *  -----------
 *  Stop an external screen saver started with DtStartSaver(). 
 *
 *  _DtSaverStop() searches the screen saver list using the serial number
 *  provided in the input state variable. It should always be found. When
 *  found, if the state variable window id is set, _DtSaverStop() kills the
 *  screen saver client via XKillClient(), deletes the state variable from
 *  the list and deallocates the state variable.
 *
 *  Inputs:
 *  ------
 *  display - display structure
 *  state - state returned from _DtSaverStart()
 *
 *  Outputs:
 *  -------
 * 
 *  Return:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

void
_DtSaverStop(
  Display *display,
  void *pstate)
{
  struct saver_state *state = (struct saver_state *)pstate;
  struct saver_state *p;

  _DtSvcProcessLock();
 /*
  * Unlink from saver_list.
  */
  p = &saver_list;
  while (p->next != state)
  {
    p = p->next;
  }
  p->next = state->next; 
  _DtSvcProcessUnlock();

 /*
  * Kill client using window id provided by RegisterSaverCB().
  */
  if (state->xid != (Window)0)
  {
    XKillClient(display, state->xid);
  }
  
 /*
  * Free state allocated by _DtSaverStart();
  */
  free(pstate);
}


/*************************************<->*************************************
 *
 *  DtSaverGetWindows() - return array of windows on which saver can draw
 *
 *  Description:
 *  -----------
 *
 *  This is a PUBLIC API.
 *
 *  When the screen saver starts, it calls the DtSaverGetWindows() API. From
 *  the screen saver perspective, the API returns an array of windows on which
 *  the screen saver can draw. To to this, the API obtains the DTSAVERINFO
 *  environment variable, and parses out the window array. The API also
 *  creates a window and sends a ClientMessage to the first DTSAVERINFO window
 *  containing the serial number and newly created window id.
 *
 *  Inputs:
 *  ------
 *  display - display structure
 *  window - pointer to memory in which to place pointer to array
 *  count - pointer to memory in which to place count
 *
 *  Outputs:
 *  -------
 *  *window - pointer to array
 *  *count - count
 * 
 *  Return:
 *  -------
 *  True - window list returned 
 *  False - window list not returned
 *  
 *  Comments:
 *  --------
 *  The array memory should be freed by the caller via free().
 *
 *************************************<->***********************************/

Boolean
DtSaverGetWindows(
  Display *display,
  Window **window,
  int *count)
{
  char               *envdata, *p, *q;
  unsigned short      serial;
  int                 envcount;
  XClientMessageEvent event;
  Window              xid_window;
  _Xstrtokparams      strtok_buf;
  _DtSvcDisplayToAppContext(display); 

  _DtSvcAppLock(app);
  *window = NULL;
  *count = 0;

  _DtSvcProcessLock();
  xa_saver_register = XInternAtom(display, "_DT_SAVER_REGISTER", False);

 /*
  * Get invocation information from environment.
  */
  envdata = getenv("DTSAVERINFO");
  if (!envdata)
  {
    _DtSvcProcessUnlock();
    _DtSvcAppUnlock(app);
    return(False);
  }

 /*
  * Copy string for later strtok() use.
  */
  p = strdup(envdata); 
  if (!p)
  {
    _DtSvcProcessUnlock();
    _DtSvcAppUnlock(app);
    return(False);
  }

 /*
  * Extract serial.
  */
  q = _XStrtok(p, " ", strtok_buf);
  serial = (unsigned short)strtoul(q, NULL, 10);

 /*
  * Extract envcount.
  */
  q = _XStrtok(NULL, " ", strtok_buf);
  envcount = (int)strtoul(q, NULL, 10);

 /*
  * Allocate memory for window array.
  */
  *window = (Window *)malloc((envcount)*sizeof(Window *));
  if (!*window)
  {
    free(p);
    _DtSvcProcessUnlock();
    _DtSvcAppUnlock(app);
    return(False);
  }

 /*
  * Populate result array and envcount.
  */
  for (*count = 0; *count < envcount; (*count)++)
  {
    q = _XStrtok(NULL, " ", strtok_buf);
    (*window)[*count] = (Window)strtoul(q, NULL, 16);
  }

 /*
  * Free temp copy of envdata.
  */
  free(p);

 /*
  * Create dummy window to obtain XID.
  */
  xid_window = XCreateWindow(display, DefaultRootWindow(display),
                             0, 0, 1, 1, 0,
                             CopyFromParent, InputOutput, CopyFromParent,
                             0, NULL);

  if (xid_window == (Window)0)
  {
   /*
    * Could not create dummy window.
    */
    free((char *)*window);
    *window = NULL;
    *count = 0;
    _DtSvcProcessUnlock();
    _DtSvcAppUnlock(app);
    return(False);
  }

 /*
  * Send client message to win0 to register.
  */
  event.type = ClientMessage;
  event.window = (*window)[0];
  event.message_type = xa_saver_register;
  event.format = 32;
  event.data.l[0] = (Atom)0;
  event.data.l[1] = (long)serial;
  event.data.l[2] = (long)xid_window;
  event.data.l[3] = CurrentTime;
  XSendEvent(display, (*window)[0], False, NoEventMask,
                        (XEvent *) &event);

 _DtSvcProcessUnlock();
 /*
  * Ensure window creation and client message have been processed by
  * the server before continuing.
  */
  XSync(display, False);

  _DtSvcAppUnlock(app);
  return(True);
}


/*************************************<->*************************************
 *
 *  RegisterSaverCB() - register a screen saver
 *
 *  Description:
 *  -----------
 *  RegisterSaverCB() is a callback called when the ClientMessage arrives from
 *  a screen saver. This callback first searches the screen saver state list
 *  by serial number. If a state variable is not found, RegisterSaverCB()
 *  assumes the screen saver must have been stopped by a call to
 * _DtSaverStop(), so kills the client via XKillClient() using the window id
 *  provided in the message. If the state variable is located,
 *  RegisterSaverCB() stores the window id in the state variable.
 *
 *  Inputs:
 *  ------
 *  w - widget from which we derive the display
 *  client_data - pointer to client data (unused)
 *  event - ClientMessage event structure
 *  continue_to_dispatch - dispatch to remaining event handlers (unused)
 * 
 *  Outputs:
 *  -------
 * 
 *  Return:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

static void
RegisterSaverCB(
  Widget w,
  XtPointer client_data,
  XEvent *event,
  Boolean *continue_to_dispatch)
{
  if (event->type == ClientMessage)
  {
    XClientMessageEvent *cEvent = (XClientMessageEvent *) event;

    _DtSvcProcessLock();
    if (cEvent->message_type == xa_saver_register)
    {
      unsigned short serial = (unsigned short)cEvent->data.l[1];
      Window win = (Window)cEvent->data.l[2];
      struct saver_state *state;
  
     /*
      * Find event in saver list.
      */
      state = saver_list.next;
      while (state != NULL && state->serial != serial)
      {
        state = state->next;
      }
   
      if (state != NULL) 
      {
       /*
        * _DtSaverStop() not yet called for this saver. Store xid in 
        * saver's state for _DtSaverStop()'s use.
        */
        state->xid = win;
      }
      else
      {
       /*
        * _DtSaverStop() has already been called for this saver, but at the
        * time, the saver had not yet registered. Kill the saver client.
        */
        XKillClient(XtDisplay(w), win);
      }
    }
    _DtSvcProcessUnlock();
  }
}
