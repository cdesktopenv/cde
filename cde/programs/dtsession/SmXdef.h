/* $XConsortium: SmXdef.h /main/4 1995/10/30 09:39:25 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmXdef.h
 **
 **   Project:     DT
 **
 **   Description
 **   -----------
 **   Variables and declarations needed for
 **   Lock Handling for the session manager
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smxdef_h
#define _smxdef_h
 
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

extern void SmXdefMerge(Display *display);
extern XrmDatabase SmXdefSubtract(XrmDatabase db);


#endif /*_smxdef_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
