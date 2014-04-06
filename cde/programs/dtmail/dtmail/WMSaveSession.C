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
 *      $TOG: WMSaveSession.C /main/16 1998/12/09 18:29:33 mgreess $
 *
 *      @(#)WMSaveSession.C	1.12 23 May 1995
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <Dt/DtPStrings.h>
#include <Dt/Session.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/MainW.h>

#include <DtMail/IO.hh>                    // SafeAccess...
#include <RoamApp.h>
#include <RoamMenuWindow.h>
#include <SendMsgDialog.h>

#include <Dt/Session.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Wsm.h>
#include <Dt/WsmP.h>


#define INBOX	"INBOX"

// For debugging only
#ifdef LOG_SESSION

char logfile[MAXPATHLEN+1];
int  logfile_initd = 0;

#define LOG_CLOSEFILEPTR(log)	if (log) fclose(log);
#define LOG_DEFINEFILEPTR(log)	FILE *log = NULL;
#define LOG_FPRINTF(log, args)	if (log) fprintf args;
#define LOG_GETFILENAME(lfn) \
    sprintf(lfn, "%s/%s/dtmail.log", getenv("HOME"), DtPERSONAL_TMP_DIRECTORY);
#define LOG_OPENFILEPTR(log) \
    if (! logfile_initd) \
    { \
	logfile_initd = 1; \
	LOG_GETFILENAME(logfile); \
    } \
    if (! (log = fopen(logfile,"a+"))) \
      perror(logfile);

#else

#define LOG_CLOSEFILEPTR(log)
#define LOG_DEFINEFILEPTR(log)
#define LOG_FPRINTF(log, args)
#define LOG_GETFILENAME(lfn)
#define LOG_OPENFILEPTR(log)

#endif


// Data struct to hold the iconic state of a window.
typedef struct _WmState
{
    int		state;
    Window	icon;
} WmState;


/*
 * Initializes the Mailer for XSMP by registering save and die callbacks
 * on the session shell.
 */
void 
RoamApp::initSession(void)
{
    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);

#ifdef LOG_SESSION
#define MAXLOGSZ	10000
    struct stat sbuf;
    time_t	tloc;

    // If the size of the logfile exceeds a max size, truncate it.
    if (stat(logfile, &sbuf) == 0)
      if (sbuf.st_size > MAXLOGSZ)
	truncate(logfile, 0);

    (void) time(&tloc);
#endif
    
    session_fp = NULL;
    XtAddCallback(_w, XtNsaveCallback, smpSaveSessionCB, (XtPointer) this);
    XtAddCallback(_w, XtNdieCallback, smpDieCB, (XtPointer) this);

    LOG_FPRINTF(log, (log,"Session Initialized at: %s\n", ctime(&tloc)));
    LOG_CLOSEFILEPTR(log);
}

/*
 * This method takes a file name and computes the full pathname 
 * using Dt services and then opens the session file pointed
 * to by pathname.
 */
void
RoamApp::openSessionFile(char *filename)
{
    struct stat s;
    char *pathname = NULL;

    if (filename == NULL)
      return;
    
    session_fp = NULL;

    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);

    // If the session file is an absolute path, just use it.
    if (filename[0] == '/')
      pathname = strdup(filename);
    else if (DtSessionRestorePath(_w, &pathname, filename) == FALSE)
    {
        LOG_FPRINTF(log, (log,"DtSessionRestorePath Failed on: %s\n",filename));
        LOG_CLOSEFILEPTR(log);
	return;
    }

    // Sometimes the session file can be empty.
    // This can cause dtmail to exist as a zombie process on login.
    // To prevent that, we stat the session file and if necessary,
    // set the session_fp to NULL.
    SafeStat(pathname, &s);
    if (s.st_size == 0)
      session_fp = NULL;
    else
    {
	if (!(session_fp = fopen(pathname, "r")))
	{
	    perror(pathname);
	    session_fp = NULL;
	}
    }

    LOG_FPRINTF(log, (log,"Opened session file: %s\n",pathname));
    LOG_CLOSEFILEPTR(log);
    if (pathname != NULL)
      XtFree((char *)pathname);
}

/*
 * This methods parses the -session argument and
 * returns the name of the session file if present.
 */
char *RoamApp::parseSessionArg(int *argc, char **argv)
{
    LOG_DEFINEFILEPTR(log);
    char *filename = NULL;

    if (*argc<3)
      return NULL;
    
    LOG_OPENFILEPTR(log);

    for(int i=0; i<*argc; i++)
    {
	LOG_FPRINTF(log, (log,"restart argv[%d]: %s\n",i, argv[i]));
	if (!strcmp(argv[i], "-session"))
	{
	    if (i<*argc-1)
	    {
		filename = argv[i+1];
		for (int j=i+2; j < *argc; )
		  argv[i++] = argv[j++]; 
		*argc -= 2;
	    }
	    break;
	}
    }
    
    LOG_CLOSEFILEPTR(log);
    return filename;
}

/*
 *  This method implements the restore operation for RoamApp.
 *  Creates and initializes a RoamMenuWindow or SendMsgDialog
 *  for each MainWindow in the session file.
 */
void 
RoamApp::restoreSession(void)
{
    char buf[MAXPATHLEN+1];

    if (! session_fp)
      _exit(0);
    
    _mailview = NULL;
    for(;;)
    {
	if (fscanf(session_fp,"%s",buf) == EOF)
	  break;
	
	switch (buf[0])
	{
	  case 'R':
	  {
	    RoamMenuWindow	*rmw = NULL;
	    
	    rmw = RoamMenuWindow::restoreSession(buf);
	    if(_mailview == NULL)
	    {
		dtmail_mapped = 1;
		_mailview = rmw;
	    }  
	    break;
	  }
	  case 'S':
	    SendMsgDialog::restoreSession(buf);
	    break;
	  default:
	  {
	    LOG_DEFINEFILEPTR(log);
	    LOG_OPENFILEPTR(log);
            LOG_FPRINTF(log, (log, "%s contains neither an R or an S", buf));
	    LOG_CLOSEFILEPTR(log);
	    break;      
	  }
	}
    }
} 

/*
 * This method implements the SmSaveLocal SaveYourself XSMP operation for the
 * RoamApp object:
 *   1. Gets a session file name from Dt services and opens the session file. 
 *   2. Invokes a saveSessionLocal on each top level window.
 *   3. Updates the WM_COMMAND property. 
 */
void 
RoamApp::smpSaveSessionLocal(void)
{
    char *pathname = NULL, *filename = NULL;
    char **save_argv = NULL;
    int save_argc = 0;
    char **argv = NULL;
    int argc = 0;
    
    if (! DtSessionSavePath(_w, &pathname, &filename))
      return;
    
    if (! (session_fp = fopen(pathname, "w+")))
    {
	perror(pathname);
	XtFree((char *)pathname);
	XtFree((char *)filename);
	session_fp = NULL;
	return;
    }
    chmod(pathname, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);
    
    for(int i = 0; i<_numWindows; i++)
      _windows[i]->smpSaveSessionLocal();
    
    fclose(session_fp);
    session_fp = NULL;
    
    XtVaGetValues(_w, XtNrestartCommand, &argv, NULL);

#if 0
    for (i=0; NULL != argv[i]; i++) {}
    save_argv = (char**) XtMalloc((i+3)*sizeof(char*));
    for (save_argc=0; save_argc<i; save_argc++)
      save_argv[save_argc] = argv[save_argc];
#else
    save_argc = 0;
    save_argv = (char**) XtMalloc(3*sizeof(char*));
    save_argv[save_argc++] = argv[0];
#endif
    save_argv[save_argc++] = "-session";
    save_argv[save_argc++] = filename;
    save_argv[save_argc] = NULL;
    
    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);
    for (int j=0; j<save_argc; j++)
    {
        LOG_FPRINTF(log, (log, "WM_COMMAND[%d]: %s\n",j,save_argv[j]));
    }
    LOG_CLOSEFILEPTR(log);
    
    XtVaSetValues(_w, XtNrestartCommand, save_argv, NULL);
    /* There should be no code after this */
}

/*
 *
 */
int
RoamApp::smpSaveSessionGlobal(void)
{
    int	cancel;

    for(int i = 0; i<_numWindows; i++)
      if (_windows[i]->smpSaveSessionGlobal())
	cancel = 1;
    
    return cancel;
}

/*
 * Restores a RoamMenuWindow from a RoamMenuWindow::SaveSessionLocal string.
 */
RoamMenuWindow *
RoamMenuWindow::restoreSession(char *buf)
{
    RoamMenuWindow *rmw = NULL;
    char *workspaces = new char[256];
    int iconic = 0; 
    int x = 0, y = 0; 
    int width = 0, height = 0;
    char *ptr;
    Atom *workspace_atoms = NULL;
    int num_workspaces = 0;

    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);
    
    if (!strncmp(buf,"RoamMenu", 8))
    { 
	FILE	*fp = theRoamApp.sessionFile();

	LOG_FPRINTF(log, (log, "Is a RMW"));
	if (fscanf(
		fp, "%s%s%d%d%d%d%d",
		buf,workspaces,&iconic,&x,&y,&width,&height) != 7)
	{
	    fscanf(fp,"[\n]",buf);
    	    LOG_CLOSEFILEPTR(log);
	    return NULL;
	}

	LOG_FPRINTF(log, (log, "Restore: RoamMenu %s %s %d %d %d %d %d\n",
			  buf,workspaces,iconic,x,y,width,height));
		
	if (0 == strncmp(buf, INBOX, strlen(INBOX)))
	{
	    DtMail::Session	*d_session;
	    DtMailEnv		error;
	    DtMailObjectSpace	space;
	    char		*inboxname;
		    
	    d_session = theRoamApp.session()->session();
    	    d_session->queryImpl(error,
                       		d_session->getDefaultImpl(error),
                       		DtMailCapabilityInboxName,
                       		&space,
                       		&inboxname);

	    strncpy(buf, inboxname, MAXPATHLEN);
	    buf[MAXPATHLEN] = '\0';
	    if (NULL != inboxname)
	      free(inboxname);
	}

	rmw =  new RoamMenuWindow(buf);
	rmw->initialize();

	// Use the appropriate DtWsm* method to restore the window
	// to the workspace it belonged to.
	// A problem though: the DtWsm* methods require the widget
	// to be realized.  We normally realize the window only when
	// we call manage.  If we call manage now, the window will
	// pop up in the default workspace and then *move* to the
	// appropriate workspace later.  To avoid this, we realize,
	// but not manage, the widget, call the DtWsm* method,
	// and then manage the widget.  This will cause the window
	// to appear only in the appropriate workspace.
	Widget bw = rmw->baseWidget();
	XtRealizeWidget(bw);
	XmUpdateDisplay(bw);

	// If the arguments are all valid, use them.
	// Else let the values in app-defaults take over.
	if (((iconic == 0) || (iconic == 1)) &&
	    x >= 0 &&
	    y >= 0 &&
	    width > 0 &&
	    height > 0)
	{
	    XtVaSetValues(bw,
			XtNinitialState, 
			(iconic ? IconicState: NormalState), 
			XtNx, (Position)x, XtNy, (Position)y, 
			XtNwidth, (Dimension)width, XtNheight, 
			(Dimension)height, NULL);
  
	    if (workspaces)
	    {
		do
		{
		    ptr = strchr (workspaces, '*');
		    if (ptr != NULL) *ptr = 0;
				
		    workspace_atoms = (Atom*) XtRealloc(
					(char*) workspace_atoms, 
					sizeof(Atom) * (num_workspaces+1));

		    workspace_atoms[num_workspaces] = 
		      XmInternAtom (XtDisplay(bw), workspaces, True);
					
		    num_workspaces++;

		    if (ptr != NULL)
		    {
			*ptr = '*';
			workspaces = ptr + 1;
		    }
		} while (ptr != NULL);

		DtWsmSetWorkspacesOccupied(
					XtDisplay(bw),
					XtWindow (bw),
					workspace_atoms,
					num_workspaces);

		XtFree((char*) workspace_atoms);
		workspace_atoms = NULL;
			  
	    }
		
	}

	// Manage the RMW now.  This will cause the window
	// to appear in the correct workspace.
	rmw->manage();
    }

    LOG_CLOSEFILEPTR(log);
    return rmw;
}

/*
 * This method implements the save yourself operation for
 * RoamMenuWindow object. The steps involved are:
 *	1. Get the session file pointer from theRoamApp.
 *	2. Get the iconic state, mail folder, x, y, width, height and 
 *	   save it. 
 */
void
RoamMenuWindow::smpSaveSessionLocal(void)
{
    WmState	*iconic_state = NULL;
    FILE	*fp = theRoamApp.sessionFile();
    Display	*display = theRoamApp.display();
    int		initialstate = 0;
    Atom	wm_state, actual_type;
    unsigned long nitems, leftover;
    int		actual_format;
    Atom	*ws_presence = NULL;
    char	*workspace_name=NULL;
    unsigned long num_workspaces = 0;
    char	*all_workspace_names;
    char	*mailboxname;
    
    if (fp == NULL)
      return;
    
    _mailbox->save();
    wm_state = XmInternAtom(display, "WM_STATE", False);
    XGetWindowProperty(
		display,XtWindow(_w), wm_state, 0L,
		(long)BUFSIZ, False, wm_state, &actual_type,
		&actual_format, &nitems, &leftover,
		(unsigned char **) &iconic_state);
    
    initialstate = (iconic_state->state == IconicState ? 1: 0);

    // Get the workspaces this window is present.
    if (DtWsmGetWorkspacesOccupied(
				display, XtWindow(_w),
                                &ws_presence, &num_workspaces) == Success)
    {
         for (int j = 0; j < num_workspaces; j++)
         {
            workspace_name = XGetAtomName (display, ws_presence[j]);
	    if (j == 0)
	    {
		all_workspace_names = (char*) malloc((size_t) MAXPATHLEN + 1);
		strcpy(all_workspace_names, (char*) workspace_name);
	    }
	    else {
		(void) strcat(all_workspace_names, "*");
		(void) strcat(all_workspace_names, workspace_name);
	    }
	    
            XtFree ((char *) workspace_name);
         }

         XFree((char *)ws_presence);
      }

    if (_inbox)
      mailboxname = INBOX;
    else
      mailboxname = _mailbox_fullpath;

    (void) fprintf(
		fp, "RoamMenu %s %s %d %d %d %d %d\n",
		mailboxname,
		all_workspace_names, initialstate,
		(int)_x, (int)_y, 
		(int)_width, (int)_height);

    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);
    LOG_FPRINTF(log, (log, "Save: RoamMenu %s %s %s %d %d %d %d %d\n",
	    	      XtName(_w), mailboxname,
		      all_workspace_names, initialstate,
	    	      (int)_x, (int)_y,
		      (int)_width, (int)_height));
    LOG_CLOSEFILEPTR(log);
}

/*
 *
 */
int
RoamMenuWindow::smpSaveSessionGlobal(void)
{
    return queryExpunge();
}

void
SendMsgDialog::restoreSession(char *buf)
{
    char *workspaces = new char[256];
    int iconic = 0; 
    int x = 0, y = 0; 
    int width = 0, height = 0;
    char *ptr = NULL;
    Atom *workspace_atoms = NULL;
    int num_workspaces=0;
    SendMsgDialog *smd = NULL;

    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);

    if (!strncmp(buf,"SendMsg", 7))
    {
	FILE	*fp = theRoamApp.sessionFile();

	LOG_FPRINTF(log, (log, "Is a SMD"));

	if (fscanf(
		fp, "%s%s%d%d%d%d%d",
		buf,workspaces,&iconic,&x,&y,&width,&height) != 7)
	{
	    fscanf(fp,"[\n]",buf);
    	    LOG_CLOSEFILEPTR(log);
	    return;
	}

	LOG_FPRINTF(log, (log, "Restore: SendMsg %s %s %d %d %d %d %d\n",
			  buf,workspaces,iconic,x,y,width,height));
		
	smd =  theCompose.getWin();
	smd->loadDeadLetter(buf);

	// Use the appropriate DtWsm* method to restore the window
	// to the workspace it belonged to.
	// A problem though: the DtWsm* methods require the widget
	// to be realized.  We normally realize the window only when
	// we call manage.  If we call manage now, the window will
	// pop up in the default workspace and then *move* to the
	// appropriate workspace later.  To avoid this, we realize,
	// but not manage, the widget, call the DtWsm* method,
	// and then manage the widget.  This will cause the window
	// to appear only in the appropriate workspace.
	Widget bw = smd->baseWidget();
	XtRealizeWidget(bw);
	XmUpdateDisplay(bw);

		
	// If the values are valid, use them.
	// Else let the ones in the app-defaults take over.
	if (((iconic == 0) || (iconic == 1)) &&
	    x >= 0 &&
	    y >= 0 &&
	    width > 0 &&
	    height > 0)
	{
	    XtVaSetValues(bw,
			XtNinitialState, 
			(iconic ? IconicState: NormalState), 
			XtNx, (Position)x, XtNy, (Position)y, 
			XtNwidth, (Dimension)width, XtNheight, 
			(Dimension)height, NULL);
	    if (workspaces)
	    {
	        do
		{
		    ptr = strchr(workspaces, '*');
		    if (ptr != NULL) *ptr = 0;
				
		    workspace_atoms = (Atom*) XtRealloc(
					(char*) workspace_atoms, 
					sizeof(Atom)*(num_workspaces+1));

		    workspace_atoms[num_workspaces] = 
		      XmInternAtom(XtDisplay(bw), workspaces, True);

		    num_workspaces++;

		    if (ptr != NULL)
		    {
		        *ptr = '*';
		        workspaces = ptr + 1;
		    }
		} while (ptr != NULL);

		DtWsmSetWorkspacesOccupied(
					XtDisplay(bw),
					XtWindow (bw),
					workspace_atoms,
					num_workspaces);

		XtFree((char*) workspace_atoms);
		workspace_atoms = NULL;
	    }
	}

	// Manage the SMD now.  This will cause the window
	// to appear in the correct workspace.
	smd->manage();
    }
    
    LOG_CLOSEFILEPTR(log);
}

/*
 * This method implements the save yourself operation for
 * SendMsgDialog object. The steps involved are:
 *	1. Get the session file pointer from theRoamApp.
 *	2. Get the iconic state, mail folder, x, y, width, height and 
 *	   save it. 
 */
void
SendMsgDialog::smpSaveSessionLocal(void)
{
    WmState	*iconic_state = NULL;
    FILE	*fp = theRoamApp.sessionFile();
    Display	*display = theRoamApp.display();
    int		initialstate = 0;
    Atom	wm_state, actual_type;
    unsigned long nitems, leftover;
    int		actual_format;
    Position	x=0, y=0;
    Dimension	width = 0, height = 0;
    Atom	*ws_presence = NULL;
    char	*workspace_name = NULL;
    unsigned long num_workspaces = 0;
    char	*all_workspace_names;
    char	*save_filename;
    int j;

    
    if(fp == NULL)
	return;
    
    // If tthe SMD is not being used, return.
    if (!_msgHandle)
	return;
    
    // Create a dead letter if this one is currently in use
    if ((save_filename = tempnam(_auto_save_path, "session")) == NULL) 
    {
        for (int suffix = 1; ; suffix++) {
            save_filename = (char*) malloc((size_t) MAXPATHLEN + 1);
	    sprintf(save_filename, "%s/session.%d", _auto_save_path, suffix);
	    if (SafeAccess(save_filename, F_OK) != 0)
	      break;
        }
    }
    doAutoSave(save_filename);
    
    wm_state = XmInternAtom(display, "WM_STATE", False);
    XGetWindowProperty(
		display, XtWindow(_w), wm_state, 0L,
		(long)BUFSIZ, False, wm_state, &actual_type,
		&actual_format, &nitems, &leftover,
		(unsigned char **) &iconic_state);
    
    initialstate = (iconic_state->state == IconicState ? 1: 0);

    // Get the workspaces this window is present.
    if (DtWsmGetWorkspacesOccupied(
				display, XtWindow (_w),
                                &ws_presence, &num_workspaces) == Success)
    {
        for (j = 0; j < num_workspaces; j++)
        {
            workspace_name = XGetAtomName (display, ws_presence[j]);
	    if (j == 0)
	    {
		all_workspace_names = (char*) malloc((size_t) MAXPATHLEN + 1);
		strcpy(all_workspace_names, (char*) workspace_name);
	    }
	    else
	    {
		(void) strcat(all_workspace_names, "*");
		(void) strcat(all_workspace_names, workspace_name);
	    }
	    
            XtFree((char*) workspace_name);
        }

         XFree((char *)ws_presence);
    }

    XtVaGetValues(
		_w,
		XtNx, &x,
		XtNy, &y,
		XtNwidth, &width,
		XtNheight, &height,
		NULL);

    (void) fprintf(
		fp,"SendMsg %s %s %d %d %d %d %d\n",
		save_filename,
		all_workspace_names, initialstate,
		(int)x, (int)y, 
		(int)width, (int)height);

    LOG_DEFINEFILEPTR(log);
    LOG_OPENFILEPTR(log);
    LOG_FPRINTF(log, (log, "Save: SendMsg %s %s %s %d %d %d %d %d\n",
	    	      XtName(_w), save_filename,
		      all_workspace_names, initialstate,
	    	      (int)x, (int)y,
		      (int)width, (int)height));
    LOG_CLOSEFILEPTR(log);
    
    free(save_filename);
}

/*
 *
 */
int
SendMsgDialog::smpSaveSessionGlobal(void)
{
    if (checkDirty())
    {
	if (isIconified())
	    MainWindow::manage();

	// Enquire if user really wants this window to go away
	Boolean really_quit = handleQuitDialog();
	if (! really_quit)
	  return 1;
	
	_takeDown = TRUE;
	quit();
    }
    return 0;
}

/*
 *  Session Shell save callback.
 */
void 
RoamApp::smpSaveSessionCB(Widget , XtPointer client, XtPointer call)
{
    RoamApp		*roamapp = (RoamApp*) client;
    XtCheckpointToken	cpToken = (XtCheckpointToken) call;

    if (! roamapp->_firstSaveYourselfArrived)
    {
	// Skip the first one since things aren't ready yet.
	roamapp->_firstSaveYourselfArrived = TRUE;
	return;
    }

    if (cpToken->cancel_shutdown)
    {
	roamapp->unsetQuitQuickly();
	roamapp->unsetQuitSilently();
	return;
    }

    if (cpToken->save_type == SmSaveLocal ||
	cpToken->save_type == SmSaveBoth)
      roamapp->smpSaveSessionLocal();
    
    if (cpToken->shutdown)
    {
        if (cpToken->fast)
          roamapp->setQuitQuickly();

        if (cpToken->interact_style == SmInteractStyleNone ||
            cpToken->interact_style == SmInteractStyleErrors)
          roamapp->setQuitSilently();
	else if (! cpToken->fast)
          XtAddCallback(
			roamapp->baseWidget(),
			XtNinteractCallback, smpInteractCB,
			(XtPointer) roamapp);
    }
}

/*
 *  Session Shell die callback.
 */
void 
RoamApp::smpDieCB(Widget, XtPointer client, XtPointer)
{
    RoamApp *roamapp = (RoamApp*) client;
    roamapp->closeAllWindows();
}

/*
 *  Session Shell interact callback.
 */
void 
RoamApp::smpInteractCB(Widget, XtPointer client, XtPointer call)
{
    RoamApp *roamapp = (RoamApp*) client;
    XtCheckpointToken	cpToken = (XtCheckpointToken) call;

    if (cpToken->save_type == SmSaveGlobal ||
	cpToken->save_type == SmSaveBoth)
      cpToken->cancel_shutdown = roamapp->smpSaveSessionGlobal();
}
