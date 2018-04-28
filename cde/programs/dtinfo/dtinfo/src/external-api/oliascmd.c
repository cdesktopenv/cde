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
/* $XConsortium: oliascmd.c /main/3 1996/06/11 16:48:04 cde-hal $ */
/*
 * $id: oliascmd.c,v 1.2 1992/10/07 16:38:43 brennan Exp $
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

#include <stdlib.h>
#include "olias.h"
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/Separator.h>
#include <Xm/TextF.h>
#include <X11/Xmu/Editres.h>

Widget toplevel, ibase, loc;

/* **************************************************************
 * activate
 * ************************************************************** */

static void
activate (Widget w, XtPointer client_data, XtPointer call_data)
{
  static OliasDisplayEvent event;

  event.type = OLIAS_DISPLAY_EVENT;
  XtVaGetValues (ibase, XmNvalue, &event.infobase, NULL);
  XtVaGetValues (loc,   XmNvalue, &event.locator,  NULL);

/*  printf ("Infobase = <%s>, locator = <%s>\n", event.infobase, event.locator);
*/
/*  puts ("** Calling API **"); */
  olias_send_event (toplevel, (OliasEvent *) &event);
/*  puts ("** Returned from API **"); */
}


/* **************************************************************
 * main
 * ************************************************************** */

int
main (int argc, char *argv[])
{
  XtAppContext app_context;
  Widget form, lform, fform;
  Widget sep, panel, send, quit;
  Dimension width, height;

  toplevel =
   XtVaAppInitialize (&app_context, "OliasCmd", NULL, 0, &argc, argv, NULL, 0);

  XtAddEventHandler(toplevel, (EventMask) 0, TRUE,
		    (XtEventHandler) _XEditResCheckMessages, NULL);

#define WIDGET  XtVaCreateWidget
#define WIDGETM XtVaCreateManagedWidget

  form  = WIDGET  ("form",      xmFormWidgetClass,       toplevel, NULL);
  panel = WIDGET  ("panel",     xmFormWidgetClass,       form,     NULL);
  send  = WIDGETM ("send",      xmPushButtonWidgetClass, panel,    NULL);
  quit  = WIDGETM ("quit",      xmPushButtonWidgetClass, panel,    NULL);
  sep   = WIDGETM ("separator", xmSeparatorWidgetClass,  form,     NULL);
  lform = WIDGET  ("lform",     xmFormWidgetClass,       form,     NULL);
          WIDGETM ("infobase",  xmLabelWidgetClass,      lform,    NULL);
          WIDGETM ("locator",   xmLabelWidgetClass,      lform,    NULL);
  fform = WIDGET  ("fform",     xmFormWidgetClass,       form,     NULL);
  ibase = WIDGETM ("ib_field",  xmTextFieldWidgetClass,  fform,    NULL);
  loc   = WIDGETM ("loc_field", xmTextFieldWidgetClass,  fform,    NULL);
  
  XtAddCallback (send, XmNactivateCallback, activate, toplevel);
  XtAddCallback (quit, XmNactivateCallback, (XtCallbackProc) exit, toplevel);

  XtVaSetValues (form, XmNdefaultButton, send, NULL);

  XtManageChild (panel);
  XtManageChild (lform);
  XtManageChild (fform);
  XtVaSetValues (form, XmNshadowThickness, 0, NULL);
  XtManageChild (form);

  XtRealizeWidget (toplevel);

  XtVaGetValues (toplevel, XmNwidth, &width, XmNheight, &height, NULL);
  XtVaSetValues (toplevel, XmNminWidth, (int) width,
		 XmNminHeight, (int) height,
		 XmNmaxHeight, (int) height,
		 NULL);
  
  XtAppMainLoop (app_context);

  exit (0);
}
