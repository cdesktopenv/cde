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
/* $TOG: SmSave.h /main/10 1997/03/07 10:25:36 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmSave.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Session Save for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smsave_h
#define _smsave_h
 
/* 
 *  #include statements 
 */
#include "SmDB.h"

#include <Dt/SessionP.h>
#include <Dt/SessionM.h>


/* 
 *  #define statements 
 */

/* 
 * typedef statements 
 */

/*
 *  External variables  
 */
extern SmScreenInfo        screenSaverVals;
extern SmAudioInfo         audioVals;
extern SmKeyboardInfo      keyboardVals;

/*  
 *  External Interface  
 */

extern void SetupSaveState(
        Boolean 	saveToHome,
        int 		mode);

extern void SaveState( 
	Boolean			saveToHome, 
	int 			mode,
	int                     saveType,
	Bool                    shutdown,
	int                     interactStyle,
	Bool                    fast,
	Bool                    global) ;

extern int CompleteSave ( );

extern void NotifyProxyClients ( );

#endif /*_smsave_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
