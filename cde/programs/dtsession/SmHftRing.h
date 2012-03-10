/* $XConsortium: SmHftRing.h /main/4 1995/10/30 09:35:50 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmHftRing.h
 **
 **   Project:     DT Session Manager (dtsession)
 **
 **   Description
 **   -----------
 **   This file contains the AIX 3.2.x specific declarations required to
 **   enable and disable the HFT ring. This is used for session locking.
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smhftring_h
#define _smhftring_h
 
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

extern void AixEnableHftRing(int);


#endif /*_smhftring_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
