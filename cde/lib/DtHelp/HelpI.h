/* $XConsortium: HelpI.h /main/3 1995/10/26 12:24:43 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpI.h
 **
 **   Project:     Display Area library
 **
 **  
 **   Description: Private header file for macros.
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
#ifndef	_DtHelpI_h
#define	_DtHelpI_h





/* <<<<<<<<<<<<<<<<<<< Help Dialog Stuff >>>>>>>>>>>>>>>>>>>>*/

#define VALID_STATUS      0
#define NON_VALID_STATUS -1


/* Cleanup Type Defines */

#define DtCLEAN_FOR_DESTROY       1
#define DtCLEAN_FOR_REUSE         2



/* Help Volume Search Path Location Environment Variables */

#define DtSYS_FILE_SEARCH_ENV          "DTHELPSEARCHPATH"
#define DtUSER_FILE_SEARCH_ENV         "DTHELPUSERSEARCHPATH"



#endif /* _DtHelpI_h */





