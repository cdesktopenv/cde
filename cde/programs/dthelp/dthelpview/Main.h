/* $XConsortium: Main.h /main/3 1995/11/08 09:22:29 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Main.h
 **
 **   Project:     helpviewerr 3.0
 **
 **   Description: Structures and defines needed by all of the files.
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
#ifndef _Main_h
#define _Main_h


#include "UtilI.h"

#define       NOSUCCESS     0
#define       SUCCESS       1


/* Close callback types used by our Quick help sutff */
#define	EXIT_ON_CLOSE       1
#define NO_EXIT_ON_CLOSE    2


/* Run Mode Defines */
#define	HELP_TOPIC_SESSION     1
#define	SINGLE_MAN_SESSION     2
#define	MULTI_MAN_SESSION      3
#define	SINGLE_FILE_SESSION    4



/* Global Variables Used to maintain our cache list of help dialogs */
CacheListStruct *pCacheListHead; 
CacheListStruct *pCacheListTale;  
int             totalCacheNodes;

/* Global Variables */
Widget topLevel;
Widget viewWidget;
Widget manWidget;
Widget manBtn;
Widget manText;
Widget manForm;
Widget closeBtn;


/* General global variables */
int runMode;
char      *helpClass;
Display	  *appDisplay;
char      *startCommand;



#endif /* _Main_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */



