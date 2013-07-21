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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/* -*-C-*-
**************************************************************************
*
* File:         Chkpnt.h
* Description:  CDE Private header file. Private API for sending checkpoint
*		messages between compliant clients and the checkpoint service
*		provider. This API is designed for use by performance
*		measurement programs.
*
* Created:      Mon Sep  6 09:00 1993
* Language:     C
*
* $XConsortium: ChkpntP.h /main/4 1995/10/26 15:18:33 rswiston $
*
* (C) Copyright 1993, Hewlett-Packard, all rights reserved.
*
**************************************************************************
*/

#ifndef _Dt_Perf_Checkpoint_P_h
#define _Dt_Perf_Checkpoint_P_h

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>

/*************************************************************************/
/************* Data types ************************************************/
/*************************************************************************/

/* The following definition is ONLY meant for union that follows */
typedef struct {
    char	*pname;		/* Client program name			*/
    char	*window;	/* Window Id for client			*/
    char 	*type;		/* Type of message			*/
    char	*count;		/* Running count of messages		*/
    char        *seconds;	/* Time in seconds from gettimeofday()	*/
    char	*message;	/* Actual message			*/
} _DtChkpntMsgFormat;

#define DT_PERF_CHKPNT_MSG_SIZE (sizeof(_DtChkpntMsgFormat) / sizeof(char *))
/* Use the following union for actual message declaration */
typedef union {
    _DtChkpntMsgFormat record;
    char               *array[DT_PERF_CHKPNT_MSG_SIZE];
} DtChkpntMsg;

#define DT_PERF_CHKPNT_MSG_INIT  	"Init"
#define DT_PERF_CHKPNT_MSG_CHKPNT	"Chkpnt"
#define DT_PERF_CHKPNT_MSG_END		"End"

/*************************************************************************/
/************* Atom Names ************************************************/
/*************************************************************************/
/* Selection for ICCCM style interaction of client and listener*/
/* This selection is owned by the listener */
#define DT_PERF_CHKPNT_SEL		"_DT_PERF_CHKPNT_SEL"

/* Properties attached to the client: Used for message transmission */
#define DT_PERF_CLIENT_CHKPNT_PROP	"_DT_PERF_CHKPNT_PROP"

/*************************************************************************/
/************* Client Functions ******************************************/
/*************************************************************************/

/* Note: It is expected that users will invoke the following functions within
   #ifdef DT_PERFORMANCE directives. The DT_PERFORMANCE flag should be set,
   in the build environment, for the performance-test-enabled builds.
*/

/* Initialize the checkpointing mechanism */
extern int _DtPerfChkpntInit(
    Display	   *display,	   /* Display pointer			   */
    Window	   parentwin,	   /* Parent window id                     */
    char           *prog_name,	   /* Name of the client program (argv[0]) */
    Boolean        bChkpnt 	   /* Boolean: True or False		   */
);

/* Send a checkpoint message to the listener */
extern void _DtPerfChkpntMsgSend(
    char           *message	   /* Acual message for transmission     */
);

/* End the checkpointing message delivery */
extern int _DtPerfChkpntEnd(
);

/*************************************************************************/
/************* Listener Service Functions ********************************/
/*************************************************************************/

/* Initialize the listener */
extern int _DtPerfChkpntListenInit(
    Display *display,	/* Current display */
    Window parentwin	/* Parent of window associated with listener */
);

/* Fetch a message from message queue */
extern Bool _DtPerfChkpntMsgReceive(
DtChkpntMsg *dtcp_msg,		/* Above message available as a structure */
Bool        bBlock		/* Block until a message is received ?    */
);

#endif /*_Dt_Perf_Checkpoint_h*/
/* Do not add anything after this endif. */
