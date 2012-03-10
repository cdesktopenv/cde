/* $XConsortium: ManPageI.h /main/4 1995/11/08 09:22:48 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ManPageI.h
 **
 **   Project:     helpviewer 3.0
 **
 **   Description: Structures and defines supported in ManPage.c 
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _ManPageI_h
#define _ManPageI_h



/****************************************************************************
 * Function:	    static void PostManDialog();
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Creates and manages a simple prompt dialog that allows a 
 *                  user to type in and display any manpage on the system.
 *
 ****************************************************************************/
void PostManDialog(
    Widget parent,
    int     argc,
    char    **argv);



#endif /* _ManPageI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */





