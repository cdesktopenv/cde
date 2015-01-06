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
 *+SNOTICE
 *
 *	$TOG: RoamApp.h /main/20 1999/07/13 08:41:18 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef ROAMAPP_H
#define ROAMAPP_H

#include <stdio.h>

#include "Application.h"
#include "MailSession.hh"
#include <DtMail/DtVirtArray.hh>

class Cmd;
class VacationCmd;
class DtMailGenDialog;
class RoamMenuWindow;

class RoamApp : public Application
{
  private:
    DtVirtArray<Display*> _activePrintDisplays;
    int			_busy_count;
    DtMailGenDialog	*_dialog;
    Display		*_errorPrintDisplay;
    Boolean		_firstSaveYourselfArrived;
    Cmd			*_options;
    char		*_optionsHandle;
    RoamMenuWindow	*_mailview;
    Boolean		_quitSilently;
    Boolean		_quitQuickly;
    static XtResource	_resources[];
    VacationCmd		*_vacation;
    FILE		*session_fp;
    XtWorkProcId	_shutdownWorkprocID;

    void		initSession(void);
    void		openSessionFile(char *filename);
    char		*parseSessionArg(int *argc, char **argv);
    void		restoreSession(void);
    static Boolean	shutdownWorkproc(XtPointer);
    static void		smpDieCB(Widget, XtPointer, XtPointer);
    static void		smpInteractCB(Widget, XtPointer, XtPointer);
    static void		smpSaveSessionCB(Widget, XtPointer, XtPointer);
    virtual int		smpSaveSessionGlobal(void);
    virtual void	smpSaveSessionLocal(void);

    
  protected:
    XtIntervalId 	_appTimeoutId;
    char		*_default_mailbox;
    char		*_glyph_font;	// Font for attchment glyph
    char		*_glyph_name;	// Font for attchment glyph
    char		*_mailfiles_folder;
    MailSession 	*_mail_session;
    DtMail::Transport	*_mail_transport;
    char		*_print_script;
    char		*_system_font;	// Variable width font
    XmFontList		_system_fontlist;
    int			_tt_fd;
    char		*_user_font;	// Fixed width font
    XmFontList		_user_fontlist;
    
    static void		applicationTimeout ( XtPointer, XtIntervalId * );
    static void		disableGroupPrivileges(void *);
    static void		enableGroupPrivileges(void *);
    static long		lastInteractiveEventTime(void *);
    static void		setBusyState(DtMailEnv &, DtMailBusyState, void *);
    static void		showBusyState(DtMailEnv &, DtMailBusyState, void *);
    void		timeout(XtIntervalId *);

  public:
    RoamApp(char*);
    virtual ~RoamApp();     

    void		busyAllWindows(const char * msg = NULL);
    virtual const char *const
			className()
			    { return "RoamApp"; }
    void		closeAllWindows(void);
    char		*default_mailbox()
			    { return _default_mailbox; }
    DtMail::Transport	*default_transport(void)
			    { return _mail_transport; }
    MainWindow		*defaultStatusWindow();
    int			(*_default_x_error_handler)(Display*, XErrorEvent*);
    DtMailGenDialog	*genDialog();
    Display		*getErrorPrintDisplay(void)
			    { return _errorPrintDisplay; }
    void		globalAddToCachedContainerList(char*);
    void		globalPropChange(void);
    char		*glyphName(void)
			    { return _glyph_name; }
    RoamMenuWindow	*inboxWindow();
    virtual void	initialize( int *, char ** );  
    Boolean		isActivePrintDisplay(Display *display)
			  { return (_activePrintDisplays.indexof(display)>=0); }
    Cmd			*mailOptions(void)
			    { return _options; };
    char		*mail_folder()
			    { return _mailfiles_folder; }
    RoamMenuWindow	*nextRoamMenuWindow(RoamMenuWindow*);
    void		closeInactiveRoamMenuWindows(void);
    void		reopenRoamMenuWindows(void);
    virtual void	open_catalog();  
    char		*optionsDialog(void)
			    { return _optionsHandle; }
    char		*print_script(){ return _print_script; }
    Boolean		quitSilently(void)
			    { return _quitSilently; }
    Boolean		quitQuickly(void)
			    { return _quitQuickly; }
    void		registerActivePrintDisplay(Display *display)
			    { _activePrintDisplays.append(display); }
    MailSession		*session(void) { return _mail_session; }
    FILE		*sessionFile(void) { return session_fp;}
    void		setErrorPrintDisplay(Display *display)
			    { _errorPrintDisplay = display; }
    void		setOptionsDialog(char *oHandle)
			    { _optionsHandle = oHandle; }
    void		setQuitSilently(void) { _quitSilently = TRUE; }
    void		setQuitQuickly(void) { _quitQuickly = TRUE; }
    void		setSession(MailSession *);
    virtual void	shutdown();
    void		checkForShutdown();
    Boolean		startVacation(Widget, Widget);
    static void		statusCallback(DtMailOperationId, DtMailEnv&, void*);
    void		stopVacation();
    void		unbusyAllWindows(void);
    void		unregisterActivePrintDisplay(Display *display)
			    { _activePrintDisplays.remove(display); }
    void		unsetQuitSilently(void) { _quitSilently = FALSE; }
    void		unsetQuitQuickly(void) { _quitQuickly = FALSE; }
    VacationCmd* vacation();
};

// This method will parse a colon/space tuples that are used in
// the mail properties.
//
struct PropStringPair {
    char *	label;
    char *	value;

    PropStringPair(void);
    PropStringPair(const PropStringPair &);
    ~PropStringPair(void);
};

char *formatPropPair(char * key, const void * data);
void parsePropString(const char * input, DtVirtArray<PropStringPair *> & result);
char* getPropStringValue(DtVirtArray<PropStringPair *> &result, const char *value);

extern RoamApp theRoamApp;

// This variable indicates whether RoamMenuWindow is mapped or not.
// If a RMW is mapped, then Self_destruct will not be called by Compose
// if Compose was started by ToolTalk.
extern int dtmail_mapped;

#endif // ROAMAPP_H
