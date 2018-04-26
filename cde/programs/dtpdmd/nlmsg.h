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
/* $XConsortium: nlmsg.h /main/1 1996/10/30 19:10:36 cde-hp $ */
/*
 * dtpdmd/nlmsg.h
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _PdmdMsgs_h
#define _PdmdMsgs_h

#ifdef I18N_MSG
#include <nl_types.h>
#define DTPDMD_GETMESSAGE(set, number, string) \
    DtPdmdGetMessage(set, number, string)

#else /* I18N_MSG */
#define DTPDMD_GETMESSAGE(set, number, string) (string)
#endif /* I18N_MSG */

/*** const causes the HP compiler to complain.  Remove reference until   ***
 *** the build and integration people can figure out how to get this to  ***
 *** work.                                                               ***/

/* #define CONST	const */
#define CONST

#define PDMD_MSG_1 DTPDMD_GETMESSAGE(1, 1, DtPdmdMsg_0001)
#define PDMD_MSG_2 DTPDMD_GETMESSAGE(1, 2, DtPdmdMsg_0002)
#define PDMD_MSG_3 DTPDMD_GETMESSAGE(1, 3, DtPdmdMsg_0003)
#define PDMD_MSG_4 DTPDMD_GETMESSAGE(1, 4, DtPdmdMsg_0004)
#define PDMD_MSG_5 DTPDMD_GETMESSAGE(1, 5, DtPdmdMsg_0005)
#define PDMD_MSG_6 DTPDMD_GETMESSAGE(1, 6, DtPdmdMsg_0006)
#define PDMD_MSG_7 DTPDMD_GETMESSAGE(1, 7, DtPdmdMsg_0007)
#define PDMD_MSG_8 DTPDMD_GETMESSAGE(1, 8, DtPdmdMsg_0008)
#define PDMD_MSG_9 DTPDMD_GETMESSAGE(1, 9, DtPdmdMsg_0009)
#define PDMD_MSG_10 DTPDMD_GETMESSAGE(1, 10, DtPdmdMsg_0010)
#define PDMD_MSG_11 DTPDMD_GETMESSAGE(1, 11, DtPdmdMsg_0011)
#define PDMD_MSG_12 DTPDMD_GETMESSAGE(1, 12, DtPdmdMsg_0012)
#define PDMD_MSG_13 DTPDMD_GETMESSAGE(1, 13, DtPdmdMsg_0013)
#define PDMD_MSG_14 DTPDMD_GETMESSAGE(1, 14, DtPdmdMsg_0014)
#define PDMD_MSG_15 DTPDMD_GETMESSAGE(1, 15, DtPdmdMsg_0015)
#define PDMD_MSG_16 DTPDMD_GETMESSAGE(1, 16, DtPdmdMsg_0016)

extern CONST char DtPdmdMsg_0001[];
extern CONST char DtPdmdMsg_0002[];
extern CONST char DtPdmdMsg_0003[];
extern CONST char DtPdmdMsg_0004[];
extern CONST char DtPdmdMsg_0005[];
extern CONST char DtPdmdMsg_0006[];
extern CONST char DtPdmdMsg_0007[];
extern CONST char DtPdmdMsg_0008[];
extern CONST char DtPdmdMsg_0009[];
extern CONST char DtPdmdMsg_0010[];
extern CONST char DtPdmdMsg_0011[];
extern CONST char DtPdmdMsg_0012[];
extern CONST char DtPdmdMsg_0013[];
extern CONST char DtPdmdMsg_0014[];
extern CONST char DtPdmdMsg_0015[];
extern CONST char DtPdmdMsg_0016[];

#ifdef I18N_MSG

/* Message IDs */

extern const char * DtPdmGetMessage(
				    int set,
				    int n,
				    const char * s);

#endif /* I18N_MSG */

#endif /* _PdmdMsgs_h */
