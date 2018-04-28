/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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



