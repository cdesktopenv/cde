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
/*******************************************************************************
**
**  session.c
**
**  $TOG: session.c /main/6 1997/06/18 17:28:49 samborn $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/XmP.h>
#include <Dt/Session.h>
#include "calendar.h"
#include "session.h"

extern Calendar *calendar;

/*  Structure used on a save session to see if a dt is iconic  */
typedef struct {
	int	state;
	Window 	icon;
} WM_STATE;

/***************************************************************************
 *                                                                         *
 * Routine:   CMSaveSessionCB                                              *
 *                                                                         *
 * Purpose:   save state information for session management                *
 **************************************************************************/
void
CMSaveSessionCB (
	Widget 		 w,
	XtPointer 	 clientData,
	XtPointer 	 callbackArg)
{
	char 		*path, 
			*command,
			*name;
	FILE 		*fp;
	int  		 n, 
			 actualFormat,
			 command_len,
			 save_session = True;
	Position 	 x, y;
	Dimension 	 width, height;
	WM_STATE  	*wmState;
	Atom wmStateAtom, actualType;
	unsigned long 	 nitems, 
			 leftover;
	Atom 		 command_atom;

	if (!DtSessionSavePath(calendar->frame, &path, &name))
		save_session = False;

	if (save_session) {

		/*  Create the session file  */
    		if (!(fp = fopen(path, "w+"))) {
			printf("fopen of %s failed. %p \n", path, fp);
        		XtFree ((char *)path);
        		XtFree ((char *)name);
        		return;
    		}

		chmod(path, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);

		/* The initial set up is done on the file.  From here on 
		   out the application should write out a set of resource 
		   definitions that will later be read in.  These resource 
		   definitions will be used to restore the syate of the tool 
		   when the session is restarted. */

    		/*  Getting the WM_STATE property to see if iconified or not */
    		wmStateAtom = XmInternAtom(XtDisplay(calendar->frame), 
							"WM_STATE", False);

		XGetWindowProperty(XtDisplay(calendar->frame), 
			XtWindow(calendar->frame), 
			wmStateAtom, 0L, (long)BUFSIZ, False, 
			wmStateAtom, &actualType, &actualFormat, 
			&nitems, &leftover, (unsigned char **) &wmState);

		if (wmState->state == IconicState)
			fprintf(fp, "*iconic: True\n");
		else
			fprintf(fp, "*iconic: False\n");

		/*** Get and write out the geometry info for our Window ***/
		x = XtX(calendar->frame);
		y = XtY(calendar->frame);
		width = XtWidth(calendar->frame);
		height = XtHeight(calendar->frame);

		fprintf(fp, "*x: %d\n", x);
		fprintf(fp, "*y: %d\n", y);
		fprintf(fp, "*width: %d\n", width);
		fprintf(fp, "*height: %d\n", height);

		if (calendar->view->glance == dayGlance)
			fprintf (fp, "*defaultView: day\n");
		else if (calendar->view->glance == weekGlance)
			fprintf (fp, "*defaultView: week\n");
		else if (calendar->view->glance == monthGlance)
			fprintf (fp, "*defaultView: month\n");
		else
			fprintf (fp, "*defaultView: year\n");

		/* This is the end of client resource writing.  At this point 
		   the client will reset the WM_COMMAND property on it's top 
		   level window to include the original starting parameters as 
		   well as the -session flag and the file to restore with. */
	}

	command_atom = XA_WM_COMMAND;
    		/* Generate the reinvoking command and add it as the 
		   property value */

	if (save_session)
    		command_len = calendar->view->wm_cmdstrlen + 
					strlen("-session") + strlen(name) + 2;
	else
    		command_len = calendar->view->wm_cmdstrlen + 2; 
					
	command = XtMalloc (sizeof (char) * command_len);

	memcpy(command, calendar->view->wm_cmdstr, 
						calendar->view->wm_cmdstrlen);
    	command_len = calendar->view->wm_cmdstrlen;

	if (save_session) {
		memcpy(command + command_len, "-session", 8);
		command_len += 8;
		command[command_len] = '\0';
		command_len++;
		memcpy(command + command_len, name, strlen(name));
		command_len += strlen(name);
	}
	
	XChangeProperty(XtDisplay(calendar->frame), XtWindow(calendar->frame), 
		command_atom, XA_STRING, 8, PropModeReplace, 
		(unsigned char *)command,
                command_len);

	free(command);

	/* Note the bogus use of XtFree here.  DtSessionSavePath requires that
	   the returned strings be freed this way.  Yick. */

	if (save_session) {
		XtFree((char *)path);
		XtFree((char *)name);
		fclose(fp);
	}
}

/***************************************************************************
 *                                                                         *
 * Routine:   GetSessionInfo                                               *
 *                                                                         *
 * Purpose:   get dticon session information                              *
 **************************************************************************/

void
GetSessionInfo(
	Calendar  	*c)
{
    XrmDatabase 	 db;
    char 		*path;
    XrmName 		 xrm_name[5];
    XrmRepresentation 	 rep_type;
    XrmValue 		 value;

    if (c->app_data->session_file == NULL)
        return;

    /***  Open the resource database file ***/

    if (DtSessionRestorePath(c->frame, &path, c->app_data->session_file) 
								    == False) {
	/* XXX: Should generate error about not being able to restore session.*/
        return;
    }

    if ((c->view->sessiondb = XrmGetFileDatabase (path)) == NULL) {
    	if (path) XtFree(path);
        return;
    }

    if (path) XtFree(path);

    xrm_name[1] = '\0';
 
    /* get default view */
    xrm_name[0] = XrmStringToQuark ("defaultView");
    if (XrmQGetResource(c->view->sessiondb, xrm_name, xrm_name, 
			&rep_type, &value)) {
	if (value.addr)
		c->app_data->default_view = strdup(value.addr);
    }
}
