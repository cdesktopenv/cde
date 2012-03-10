/* $XConsortium: SrvPalette.h /main/4 1995/10/30 09:40:38 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SrvPalette.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Session Restoration for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _srvpalette_h
#define _srvpalette_h
 
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


extern int InitializeDtcolor( Display *, short ) ;
extern int CheckMonitor( Display *) ;


#endif /*_srvpalette_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
