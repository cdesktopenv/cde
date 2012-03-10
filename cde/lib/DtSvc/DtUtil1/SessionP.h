/* $TOG: SessionP.h /main/5 1998/07/30 12:11:06 mgreess $ */
/*                                                                            *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                           *
 * (c) Copyright 1993, 1994 International Business Machines Corp.             *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                            *
 * (c) Copyright 1993, 1994 Novell, Inc.                                      *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SessionP.h
 **
 **  Description:  Private header for Session Management routines
 **  -----------
 **
 *******************************************************************
 **  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
 **  reserved.  Copying or other reproduction of this program      
 **  except for archival purposes is prohibited without prior      
 **  written consent of Hewlett-Packard Company.		     
 ********************************************************************
 **
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/

#ifndef _SessionP_h
#define _SessionP_h

/*
 *  include statements
 */

/*
 * define statements
 */

/*
 * typedef statements
 */

/*
 * Definition for the _DT_SM_WINDOW_INFO property. 
 */
typedef struct
{
    unsigned long flags;
    unsigned long smWindow;
} PropDtSmWindowInfo;

/*
 * Definition for the _DT_SM_STATE_INFO property
 */
typedef struct
{
    unsigned long flags;
    unsigned long smStartState;
    unsigned long smConfirmMode;
    unsigned long smCompatMode;
    unsigned long smSendSettings;
    unsigned long smCoverScreen;
    unsigned long smLockOnTimeout;
    unsigned long smLockOnTimeoutStatus;
    unsigned long smCycleTimeout;
    unsigned long smLockTimeout;
    unsigned long smSaverTimeout;
    unsigned long smRandom;
    unsigned long smDisplaySpecific;
} PropDtSmStateInfo;

/*
 * Definition for the _DT_SM_SCREEN_INFO property
 */
typedef struct
{
    unsigned long flags;
    unsigned long smTimeout;
    unsigned long smInterval;
    unsigned long smPreferBlank;
    unsigned long smAllowExp;
} PropDtSmScreenInfo;

/*
 * Definition for the _DT_SM_AUDIO_INFO property
 */
typedef struct
{
    unsigned long flags;
    unsigned long smBellPercent;
    unsigned long smBellPitch;
    unsigned long smBellDuration;
} PropDtSmAudioInfo;


/*
 * Definition for the _DT_SM_KEYBOARD_INFO property
 */
typedef struct
{
    unsigned long flags;
    unsigned long smKeyClickPercent;
    unsigned long smGlobalAutoRepeat;
} PropDtSmKeyboardInfo;


#endif /* _SessionP_h */
/* Do not add anything after this endif. */
