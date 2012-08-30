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
/* $XConsortium: session.c /main/3 1995/11/01 10:39:26 rswiston $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        session.c
**
**  Project:     DT dtpad, a memo maker type editor based on the motif
**               text widget.
**
**  Description:  Provides the functionality for saving and restoring the
**		  user's session.
**			
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are
**  reserved.  Copying or other reproduction of this program
**  except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
**
********************************************************************
**  (c) Copyright 1993, 1994 Hewlett-Packard Company
**  (c) Copyright 1993, 1994 International Business Machines Corp.
**  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
**  (c) Copyright 1993, 1994 Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#include "dtpad.h"
#include <Dt/Wsm.h> 
/*
 * This for the sleazy hack to get the window manager frame width/height
 */
#include <Xm/VendorSEP.h>

/* Copied from BaseClassI.h */
extern XmWidgetExtData _XmGetWidgetExtData( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */

#define MSG1  ((char *)GETMESSAGE(10, 1, "Check the file permissions."))
#define MSG2  ((char *)GETMESSAGE(10, 2, "%s is the file that would have been used to save your session."))

extern Widget topLevelWithWmCommand;	/* declared in main.c */
extern Editor *pPadList;
extern int numActivePads;

/*  Structure used on a save session to see if a dt is iconic  */

static Atom wm_state_atom;
typedef struct
{
   int state;
   Window icon;
} WM_STATE;


/************************************************************************
 *
 * SaveMain - saves the parameters associated with a Pad's mainWindow
 *	(called from SaveSessionCB() below).
 *
 ***********************************************************************/
void 
SaveMain(
        Editor *pPad,
        int padNum,
        int fd)
{
    char bufr[1024];
    Position x,y;
    Dimension width, height;
    Atom *pWsPresence;
    unsigned long numInfo;
    Atom actual_type;
    int  actual_format;
    unsigned long nitems;
    unsigned long leftover;
    WM_STATE * wm_state;

    if(pPad->mainWindow != (Widget)NULL) 
    {
        XmVendorShellExtObject vendorExt;
        XmWidgetExtData        extData;

        if(XtIsRealized(pPad->mainWindow))
	    sprintf(bufr,"*mainWindow%d.ismapped: True\n", padNum);

        /* Get and write out the geometry info for our Window */
        x = XtX(XtParent(pPad->mainWindow));
        y = XtY(XtParent(pPad->mainWindow));

	/*
         * Modify x & y to take into account window mgr frames
	 * This is pretty bogus, but I don't know a better way to do it.
	 */
        extData = _XmGetWidgetExtData(pPad->app_shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	width = XtWidth(XtParent(pPad->mainWindow));
	height = XtHeight(XtParent(pPad->mainWindow));

        sprintf(bufr, "%s*mainWindow%d.x: %d\n", bufr, padNum, x);
        sprintf(bufr, "%s*mainWindow%d.y: %d\n", bufr, padNum, y);
        sprintf(bufr, "%s*mainWindow%d.width: %d\n", bufr, padNum, width);
        sprintf(bufr, "%s*mainWindow%d.height: %d\n", bufr, padNum, height);

        wm_state_atom = XmInternAtom (XtDisplay(pPad->app_shell), "WM_STATE", 
                                      False);
        /*  Getting the WM_STATE property to see if iconified or not */
        XGetWindowProperty (XtDisplay(pPad->app_shell), 
                            XtWindow (pPad->app_shell),
                            wm_state_atom, 0L, (long) BUFSIZ, False,
                            wm_state_atom, &actual_type, &actual_format,
                            &nitems, &leftover, (unsigned char **) &wm_state);

        /* Write out if iconified our not */
        sprintf(bufr, "%s*mainWindow%d.iconify: ", bufr, padNum);

        if (wm_state->state == IconicState)
          sprintf (bufr, "%sTrue\n", bufr);
        else
          sprintf (bufr, "%sFalse\n", bufr);

	if(DtWsmGetWorkspacesOccupied(XtDisplay(pPad->app_shell), 
				  XtWindow(pPad->app_shell), &pWsPresence,
				  &numInfo) == Success)
	{
	    int i;
	    sprintf(bufr, "%s*mainWindow%d.workspaceList: ", bufr, padNum);
	    for(i = 0; i < numInfo; i++)
	    {
	        char *name =  XGetAtomName(XtDisplay(pPad->app_shell),
					   pWsPresence[i]);
		sprintf(bufr, "%s %s", bufr, name);
		XtFree(name);
	    }
	    sprintf(bufr, "%s\n", bufr);
	    XtFree((char *)pWsPresence);
	}

        write (fd, bufr, strlen(bufr));
    }
    if(pPad->fileStuff.fileName != (char *)NULL)
    {
	sprintf(bufr, "*mainWindow%d.fileName: %s\n", padNum, 
		pPad->fileStuff.fileName);
        write (fd, bufr, strlen(bufr));
    }
}


/************************************************************************
 *
 *  SaveSessionCB - saves the editor state (just filename) - does not save
 *	the file at this time.
 *
 *	This routines is setup as the "WM_SAVE_YOURSELF" WMProtocolCallback
 *	on the top level widget (created via XtInitialize).
 *
 ************************************************************************/
/* ARGSUSED */
void 
SaveSessionCB(
        Widget w,			/* widget id */
        caddr_t client_data,		/* data from application  */
        caddr_t call_data )		/* data from widget class */
{
    char *longpath, *fileName;
    int fd, numPadsToSave;
    char *xa_CommandStr[10];
    char *tmpStr, bufr[1024];
    Editor *pPad;
    int i;

    /* Xt may not pass a widget as advertised (??? is this needed? - hp) */
    if(!XtIsShell(w))
	w = XtParent(w);

    for(pPad = pPadList, numPadsToSave = 0; pPad != (Editor *)NULL; 
	pPad = pPad->pNextPad)
    {
	if(pPad->inUse == True)
	    numPadsToSave++;
    }
    if(numPadsToSave < 1)
    {
        xa_CommandStr[0] = (char *)NULL;
        XSetCommand(XtDisplay(w), XtWindow(w), xa_CommandStr, 1);
	return;
    }

    DtSessionSavePath(w, &longpath, &fileName);

    /*  Create the session file  */
    if ((fd = creat (longpath, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)) == -1)
    {
        tmpStr = (char *)malloc(strlen(MSG2) + strlen(longpath)+ 1);
        sprintf(tmpStr, MSG2, longpath);
        _DtSimpleErrnoError(pPad->progname, DtError, MSG1, tmpStr, NULL);
        free(tmpStr);
        XtFree ((char *)longpath);
        return;
    }

    sprintf(bufr, "*pads.numActivePads: %d\n", numPadsToSave);
    write (fd, bufr, strlen(bufr));

    for(pPad = pPadList, i = 0; pPad != (Editor *)NULL; 
	pPad = pPad->pNextPad, i++)
    {
	if(pPad->inUse == True)
	    SaveMain(pPad, i, fd);
    }

    close(fd);

    i = 0;
    xa_CommandStr[i] = pPadList->progname; i++;
    xa_CommandStr[i] =  "-session"; i++;
    xa_CommandStr[i] =  fileName; i++;

    XSetCommand(XtDisplay(topLevelWithWmCommand), 
		XtWindow(topLevelWithWmCommand), xa_CommandStr, i);

    XtFree ((char *)fileName);
}


/***********************************************************************
 *
 * closeCB - set up as the "WM_DELETE_WINDOW" WMProtocolCallback on the
 *	application shell for each Editor instance - and called when
 *	a delete window command is received from the Window Manager.
 *	Waits for CloseWindow to become false before it continues.
 *
 ***********************************************************************/
 
/* ARGSUSED */
void
closeCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;

    /* call the callback for Exit within the File Menu pulldown */
    XtCallCallbacks(pPad->ExitWid, XmNactivateCallback, (XtPointer)pPad);
}


/***********************************************************************
 *
 * restoreSession - gets the valid x and y location of where to put the
 *	Text Edit on the root window.  Sets the global varible
 *	dtpad.saveRestore to True so the rest of the program knows that
 *	a session is being restored.
 *
 ***********************************************************************/
void
restoreSession(
        Editor *pPad)
{
    XrmDatabase db;
    char *tmpStr;
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;
    char *fileName = pPad->xrdb.session;
    char *path;
    int numPadsToRestore, i;
    Boolean foundPad;

    if(DtSessionRestorePath(topLevelWithWmCommand, &path, fileName) == False)
	path = fileName;

    /*  Open the file as a resource database */
    if ((db = XrmGetFileDatabase (path)) == NULL) 
    {
      tmpStr = (char *)XtMalloc(strlen(MSG2) + strlen(path)+ 1);
      sprintf(tmpStr, MSG2, path);
      _DtSimpleErrnoError(pPad->progname, DtError, MSG1, tmpStr, NULL);
      XtFree(tmpStr);
      return;
    }

    xrm_name[0] = XrmStringToQuark ("pads");
    xrm_name[1] = XrmStringToQuark ("numActivePads");
    xrm_name[2] = 0;
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    numPadsToRestore = atoi((char *)value.addr);

    if(numPadsToRestore == 0)
    {
	/*
	 * Either it's an old (i.e. 2.01) session file,
	 * or it's bogus.  Either way, we'll create one
	 * window, taking whatever mainWindow: resources
	 * we can find.
	 */
        xrm_name[0] = XrmStringToQuark ("mainWindow");
        xrm_name[2] = 0;

        /* get x position */
        xrm_name[1] = XrmStringToQuark ("x");
        XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
        pPad->x = atoi((char *)value.addr);

        /* get y position */
        xrm_name[1] = XrmStringToQuark ("y");
        XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
        pPad->y = atoi((char *)value.addr);
 
        pPad->saveRestore = True;

	return;
    }      

    RestorePad(pPad, 0, db);

    for(i = 1; i < numPadsToRestore; i++)
    {
	foundPad = FindOrCreatePad(&pPad);
	RestorePad(pPad, i, db);

	if(foundPad == False)
	    RealizeNewPad(pPad);
	else
	    ManageOldPad(pPad, False);
    }
}


/************************************************************************
 *
 * RestoreMain - 
 *
 ***********************************************************************/
static void
RestoreMain(
        Editor *pPad,
	int padNum,
        XrmDatabase db)
{
    char * iconify = NULL;
    char buf[1024];
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    sprintf(buf, "mainWindow%d", padNum);
    xrm_name[0] = XrmStringToQuark(buf);
    xrm_name[2] = 0;

    /* get x position */
    xrm_name[1] = XrmStringToQuark ("x");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    pPad->x = atoi((char *)value.addr);

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    pPad->y = atoi((char *)value.addr);

    /* get width */
    xrm_name [1] = XrmStringToQuark ("width");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    pPad->width = atoi((char *)value.addr);

    /* get height */
    xrm_name [1] = XrmStringToQuark ("height");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    pPad->height = atoi((char *)value.addr);
 
   /*  Get and set whether the view is iconic  */
   xrm_name [1] = XrmStringToQuark ("iconify");
   XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
   /*  If there is an iconify resource and its value is True,  */
   /*  then mark the window as iconified.                      */
   if ((iconify = (char *) value.addr) != NULL &&
                                    strcmp (iconify, "True") == 0)
      pPad->iconic = True;
   else
      pPad->iconic = False;

    /* get the file name */
    xrm_name [1] = XrmStringToQuark ("fileName");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    pPad->fileStuff.fileName = strdup((char *) value.addr);

    /* get workspace list */
    xrm_name[1] = XrmStringToQuark("workspaceList");
    XrmQGetResource(db, xrm_name, xrm_name, &rep_type, &value);
    pPad->xrdb.workspaceList = strdup((char *) value.addr);

}


/************************************************************************
 *
 * RestorePad - 
 *
 ***********************************************************************/
void
RestorePad(
        Editor *pPad,
	int padNum,
        XrmDatabase db)
{
    RestoreMain(pPad, padNum, db);
    pPad->saveRestore = True;
}
