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
/*
 * File:         CommandM.h $XConsortium: CommandM.h /main/3 1995/10/26 15:18:58 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _Dt_CommandM_h
#define _Dt_CommandM_h

/*
 * Command Invoker execution window types.  Note that a success or
 * failure notification will be sent for each request.  For
 * failures, the only data returned is an error message.
 */
#define DtNO_STDIO			"NO_STDIO"
#define DtTERMINAL			"TERMINAL"
#define DtPERM_TERMINAL			"PERM_TERMINAL"
#define DtOUTPUT_ONLY			"OUTPUT_ONLY"
#define DtSHARED_OUTPUT			"SHARED_OUTPUT"

/*
 * When the session manager starts, it needs a window type (NO-STDIO),
 * but if a failure occurs, it does NOT want an error message, but
 * instead wants to receive the execution host and the execution
 * string.  The following define is for this type of request.
 */
#define DtSTART_SESSION			"START-SESSION"

#endif /* _Dt_CommandM_h */
/* Do not add anything after this endif. */
