/* $XConsortium: SmXrm.h /main/4 1995/10/30 09:39:43 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmXrm.h
 **
 **   Project:     DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Xrm database management
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smxrm_h
#define _smxrm_h

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

XrmDatabase SmXrmSubtractDatabase(
  XrmDatabase source_db,
  XrmDatabase target_db);


#endif /*_smxrm_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
