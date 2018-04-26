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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_trace_parse.h /main/3 1995/10/23 10:46:24 rswiston $ 			 				 */
//%% 	$XConsortium: tt_trace_parse.h /main/3 1995/10/23 10:46:24 rswiston $							
//%% 									
//%%  	RESTRICTED CONFIDENTIAL INFORMATION:                            
//%% 									
//%% 	The information in this document is subject to special		
//%% 	restrictions in a confidential disclosure agreement between	
//%% 	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this	
//%% 	document outside HP, IBM, Sun, USL, SCO, or Univel without	
//%% 	Sun's specific written approval.  This document and all copies	
//%% 	and derivative works thereof must be returned or destroyed at	
//%% 	Sun's request.							
//%% 									
//%% 	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.	
//%% 									
/*
 *
 * tt_trace_parse.h
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#ifndef _TT_TRACE_PARSE_H
#define _TT_TRACE_PARSE_H

typedef enum {
	_TT_TRACE_VERSION = 1,	// Don't want a "false" value
	_TT_TRACE_FOLLOW,
	_TT_TRACE_WRITEFILE,
	_TT_TRACE_APPENDFILE,
	_TT_TRACE_FUNCTIONS,
	_TT_TRACE_ATTRIBUTES,
	_TT_TRACE_STATES,
	_TT_TRACE_OPS,
	_TT_TRACE_SENDER_PTYPES,
	_TT_TRACE_HANDLER_PTYPES,
#ifdef _OPT_TIMERS_SUNOS
	_TT_TRACE_TIMERS,
#endif

	// The above group is the keywords
	
	_TT_TRACE_NUMBER,
	_TT_TRACE_OFF,
	_TT_TRACE_ON,
	_TT_TRACE_FILENAME,
	_TT_TRACE_FUNC_VAL,
	_TT_TRACE_ALL,
	_TT_TRACE_NONE,
	_TT_TRACE_EDGE,
	_TT_TRACE_DELIVER,
	_TT_TRACE_DISPATCH,
	_TT_TRACE_STRING,
	_TT_TRACE_IDENTIFIER,
	_TT_TRACE_STATE_CREATED,
	_TT_TRACE_STATE_SENT,
	_TT_TRACE_STATE_HANDLED,
	_TT_TRACE_STATE_FAILED,
	_TT_TRACE_STATE_QUEUED,
	_TT_TRACE_STATE_STARTED,
	_TT_TRACE_STATE_REJECTED,
	_TT_TRACE_STATE_RETURNED,
	_TT_TRACE_STATE_ACCEPTED,
	_TT_TRACE_STATE_ABSTAINED,

	// The above group is the values

	_TT_TRACE_ENDLINE,	// Should be self-explanatory
	_TT_TRACE_ENDFILE,	// Ditto
	_TT_TRACE_UNKNOWN	// Ditto
	
} _Tt_trace_tokens;

#endif
