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
