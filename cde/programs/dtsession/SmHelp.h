/* $XConsortium: SmHelp.h /main/8 1996/02/09 16:01:30 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmHelp.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Help for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smhelp_h
#define _smhelp_h
 
#include <Xm/Xm.h>

/* 
 *  #define statements 
 */
#define	HELP_LOGOUT_STR			"ConfirmLogoutDE"
#define HELP_LOGOUT_QUERY_STR		"SEConfirmLogoutWithChoiceDE"
#define HELP_LOGOUT_COMPAT_MODE_STR	"logoutCompatModeHelp"
#define HELP_BMS_DEAD_STR		"bmsDeadHelp"
#define HELP_CONFIRMATION_SESSION_STR	"ConfirmSessionCreation"
#define HELP_XSMP_INIT_FAILURE_STR	"SessionInitFailure"
#define HELP_SESSION_SAVE_ERROR_STR	"SessionSaveError"
#define HELP_APP_CLOSE_REASONS_STR	"ApplicationCloseReasons"

/* 
 * typedef statements 
 */


/*
 *  External variables  
 */

/*  
 *  External Interface
 */

extern void TopicHelpRequested( Widget, XtPointer, XtPointer ) ;



#endif /*_smhelp_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
