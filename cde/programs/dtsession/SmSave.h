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
