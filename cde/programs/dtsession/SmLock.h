/* $XConsortium: SmLock.h /main/4 1995/10/30 09:36:10 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmLock.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Lock Handling for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smlock_h
#define _smlock_h
 
/* 
 *  #include statements 
 */


/* 
 *  #define statements 
 */

/*
 * Type of lock states possible during session
 */
#define		UNLOCKED		0
#define 	LOCKED			1


/* 
 * typedef statements 
 */
/*
 *  External variables  
 */

/*  
 *  External Interface  
 */

extern void LockDisplay( Boolean locknow ) ;
extern void CreateLockCursor( void ) ;



#endif /*_smlock_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
