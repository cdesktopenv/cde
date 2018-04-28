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
/* $XConsortium: messageParam.h /main/3 1995/11/01 10:38:54 rswiston $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        messageParam.h
**
**  Project:     DT dtpad, a memo maker type editor based on the motif
**               1.1 widget.
**
**  Description: Contains the codes used in communication between the
**		 "client" and "server" dtpad.  These are used in place
**		 of strings to reduce the computational overhead of
**		 parsing the parameters on the receiving (server) end.
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1992.  All rights are
**  reserved.  Copying or other reproduction of this program
**  except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
**
********************************************************************
**  (c) Copyright 1993, 1994 Hewlett-Packard Company
**  (c) Copyright 1993, 1994 International Business Machines Corp.
**  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
**  (c) Copyright 1993, 1994 Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#include <Tt/tt_c.h>

/*
 * The codes are consecutive byte values to give the compiler the best
 * shot at optimizing the resulting "switch" statement.
 */

#define NETFILE			(char)'?'

/* non Text Editor specific options */
#define GEOMETRY		(char)'A'

/* basic options */
#define STATUSLINE		(char)'B'
#define WINDOWWORDWRAP		(char)'C'
#define OVERSTRIKE		(char)'D'
#define SAVEONCLOSE		(char)'E'
#define MISSINGFILEWARNING	(char)'F'
#define NOREADONLYWARNING	(char)'G'
#define NONAMECHANGE		(char)'H'
#define VIEWONLY		(char)'I'
#define WORKSPACELIST		(char)'J'
#define SESSION			(char)'K'

/* client/server control options */
#define BLOCKING		(char)'L'

/* field nos. for DTPAD_OPEN_FILE_MSG and DTPAD_RUN_SESSION_MSG fields
 * (DT_MSG_DATA_? are defined in lib1/DtSvc/DtUtil2/Message.h) */
#define FILENAME_MSG_FIELD	DT_MSG_DATA_1
#define NETFILE_ID_MSG_FIELD	DT_MSG_DATA_3
#define PROCESS_ID_MSG_FIELD	DT_MSG_DATA_5

/* field nos. for DTPAD_DONE message fields */
#define CHANNEL_MSG_FIELD	DT_MSG_DATA_1
#define RETURN_STATUS_MSG_FIELD	DT_MSG_DATA_2
