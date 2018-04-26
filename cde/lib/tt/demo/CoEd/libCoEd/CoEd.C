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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: CoEd.C /main/3 1995/10/20 17:05:50 rswiston $ 			 				
/*
 * CoEd.cc
 *
 * Copyright (c) 1991 by Sun Microsystems.  All Rights Reserved.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the names of Sun
 * Microsystems and its subsidiaries not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Sun Microsystems and its
 * subsidiaries make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * Sun Microsystems and its subsidiaries disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness.  In no event shall Sun Microsystems or
 * its subsidiaries be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss
 * of use, data or profits, whether in an action of contract,
 * negligence or other tortious action, arising out of or in
 * connection with the use or performance of this software.
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <desktop/tt_c.h>

#define InLibCoEd

#include "CoEd.h"
#include "CoEdFile.h"
#include "CoEdGlobals.h"
//#include "../libticccm/eclipse.h"

char	       *coEdProcID	= 0;
CoEdSiteID     *coEdSiteID	= 0;
int		coEdTtFd	= -1;
int		coEdTtStackMark	= 0;
CoEdFileList   *coEdFiles	= 0;

const char *
coEdStatusMessage(
	CoEdStatus status
)
{
	switch (status) {
	    case CoEdWarnTimeout:
		return "Timed out";
	    case CoEdErrFile:
		return "Invalid file";
	    case CoEdErrNoMem:
		return "Out of memory";
	    case CoEdErrBadPointer:
		return "Invalid pointer";
	    case CoEdErrXDR:
		return "XDR failed";
	    case CoEdErrBadMsg:
		return "Invalid message";
	    case CoEdErrFailure:
		return "System error";
	    default:
		if ((status >= TT_OK) && (status < TT_ERR_APPFIRST)) {
			return tt_status_message( (Tt_status)status );
		} else {
			return "Invalid CoEdStatus";
		}
	}
}

CoEdStatus
coEdInit( char* &returnProcID, int &pFd2Watch )
{
	if (coEdProcID != 0) {
		return CoEdOK;
	}
	coEdFiles = new CoEdFileList;
	if (coEdFiles == 0) {
		return CoEdErrNoMem;
	}
	//
	// Initialize ToolTalk
	//
	coEdProcID = tt_open();
	Tt_status err = tt_ptr_error( coEdProcID );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_open(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	returnProcID = strdup(coEdProcID);

	coEdSiteID = new CoEdSiteID( coEdProcID );
	if (coEdSiteID == 0) {
		return CoEdErrNoMem;
	}

	//
	// Get the file descriptor that ToolTalk will use to tell us
	// of new messages.
	//
	coEdTtFd = tt_fd();
	err = tt_int_error( coEdTtFd );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_fd(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	if (pFd2Watch == 0) {
		return CoEdErrBadPointer;
	}
	pFd2Watch = coEdTtFd;
	//
	// Register a file-scoped pattern for the notices in the
	// CoEd protocol.
	//
	Tt_pattern pat = tt_pattern_create();
	err = tt_ptr_error( pat );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_create(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat, "Text_File_Changed" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat, "Text_File_Poll_Version" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat, "Text_File_Quiesce" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat, "Text_File_Saved" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat, "Text_File_Reverted" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat, "Text_File_Renamed" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_scope_add( pat, TT_FILE );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_scope_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_category_set( pat, TT_OBSERVE );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_category_set(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_register( pat );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_register(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	//
	// Register a file-scoped pattern for the requests in the
	// CoEd protocol.
	//
	Tt_pattern pat2 = tt_pattern_create();
	err = tt_ptr_error( pat2 );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_create(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_op_add( pat2, "Text_File_Version_Vote" );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_op_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_scope_add( pat2, TT_FILE );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_scope_add(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_category_set( pat2, TT_HANDLE );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_category_set(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	err = tt_pattern_register( pat2 );
	if (tt_is_err( err )) {
		fprintf( stderr, "libCoEd: tt_pattern_register(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	return CoEdOK;
}

CoEdStatus
coEdFini()
{
	delete coEdFiles;
	tt_close();
	return CoEdOK;
}

CoEdStatus
coEdHandleActiveFD( int )
{
	Tt_status status;
//	TtDtProcIDEclipse eclipse( coEdProcID, &status );
//	if (status != TT_OK) {
//		return (CoEdStatus)status;
//	}
	Tt_message msg = tt_message_receive();
	if (msg == 0) {
		return CoEdOK;
	}
	status = tt_ptr_error( msg );
	if (tt_is_err( status )) {
		fprintf( stderr, "libCoEd: tt_message_receive(): %s\n",
			 tt_status_message( status ));
		return (CoEdStatus)CoEdOK;
	}
	char *msgFile = tt_message_file( msg );
	status = tt_ptr_error( msgFile );
	if (tt_is_err( status )) {
		fprintf( stderr, "libCoEd: tt_message_file(): %s\n",
			 tt_status_message( status ));
		return (CoEdStatus)CoEdOK;
	}
	if (msgFile == 0) {
		return CoEdOK;
	}
	CoEdStatus retVal = coEdFiles->handleMsg( msgFile, msg );
	tt_free( msgFile );
	return retVal;
}
