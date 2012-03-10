/* $XConsortium: SmUI.h /main/14 1996/05/31 09:18:17 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmUI.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   User Interface for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smui_h
#define _smui_h
 
/* 
 *  #include statements 
 */
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include "SmXSMP.h"

/* 
 *  #define statements 
 */

/* 
 * typedef statements 
 */

typedef struct
{
    Widget              confExit;               /* Exit confirmation dialogs*/
    Widget              qExit;
    Widget              compatExit;
    Widget              lockDialog;             /* lock-login shell */
    Widget              lockCoverDialog;        /* lock-login shell */
    Widget              coverDialog[10];        /* Cover shell*/
    Widget              coverDrawing[10];       /* Cover drawing area */
    Widget              matte[2];               /* lock dialog pieces */
    Widget              loginMatte[2];
    Widget              loginForm[2];
    Widget              indLabel[2];
    Widget              deadWid;                /* Bms-dead dialog */
    Widget		clientReasons;		/* For client "reasons" */
    Widget              confirmSession;         /* Confirm session selection */
    Widget              saveSession;         	/* Error dialog for Save_Session
						   ToolTalk message */
    XmString            okString, cancelString, helpString;
    XmString            okLogoutString, cancelLogoutString;
    Widget              noStart;                /* Cant start dt dialog */
#ifdef __osf__
    Widget              newProfile;             /* new dtprofile dialog */
#endif
    Widget              smHelpDialog;   /* Help dialog for all topics */
} DialogData;

/*
 *  External variables  
 */
extern DialogData       smDD;
extern Arg              uiArgs[20];


/*  
 *  External Interface  
 */
extern Widget CreateLockDialog( void ) ;
extern void ExitSession( Tt_message ) ;
extern int WarnMsgFailure( void ) ;
extern Widget CreateLockDialogWithCover( Widget ) ;
extern Widget CreateCoverDialog( int, Boolean ) ;
extern void ImmediateExit( int, Tt_message, Boolean ) ;
extern void ShowWaitState( Boolean ) ; 
extern Boolean InitCursorInfo( void ) ; 
extern void UpdatePasswdField( int );
extern int WarnNoStartup( void );
extern void DialogUp( Widget, XtPointer, XtPointer ) ;
#ifdef __osf__
extern int WarnNewProfile( void );
#endif
void PostSaveSessionErrorDialog ( void );
extern int ConfirmExit( Tt_message, Boolean );

extern Boolean ConfirmSessionCreation ( 
	short 			session_type,
	unsigned int		argc,
	char			**argv);

extern void PostXSMPFailureDialog (
	XSMPFailure 		failure_code, 
	Boolean 		check_errorlog);

extern void PostReasonsDialog (
	char			* progName,
	int			numMsgs,
	char			** message,
	Boolean			waitForResponse);

#endif /*_smui_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
