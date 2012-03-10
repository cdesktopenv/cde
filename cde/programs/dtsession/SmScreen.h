/* $XConsortium: SmScreen.h /main/4 1995/10/30 09:38:16 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmScreen.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   managing external screen savers
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smscreen_h
#define _smscreen_h
 
/* 
 *  #include statements 
 */


/* 
 *  #define statements 
 */

/* 
 * typedef statements 
 */

/*
 *  External variables  
 */


/*  
 *  External Interface  
 */

extern void StartScreenSaver(void);
extern void StopScreenSaver(void);
extern void *SmSaverParseSaverList(char *);


#endif /*_smscreen_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
