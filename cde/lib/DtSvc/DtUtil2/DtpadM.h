/* $XConsortium: DtpadM.h /main/3 1995/10/26 15:20:56 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        DtpadM.h
 **
 **   Project:     HP-DT "dtpad" text editor
 **
 **   Description: Defines for the tool class and messages for the
 **		   dtpad text editor
 **
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _DtpadM_h
#define _DtpadM_h

#define DTPAD_TOOL_CLASS		"DTPAD"

/*
 * Request Messages which the editor understands
 */
#define DTPAD_RUN_SESSION_MSG	"RUN_SESSION"
#define DTPAD_OPEN_FILE_MSG	"OPEN_FILE"

/*
 * Notification Messages which the Editor sends
 */
#define DTPAD_DONE		"DONE"

#endif /*_DtpadM_h*/
