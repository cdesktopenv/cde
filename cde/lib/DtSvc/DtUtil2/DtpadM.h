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
