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
/*%%  $XConsortium: CoEd.h /main/3 1995/10/20 17:06:02 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * CoEd.h
 *
 * Copyright (c) 1991,1993 by Sun Microsystems.  All Rights Reserved.
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

#ifndef CoEd_h
#define CoEd_h

#include <desktop/tt_c.h>

typedef enum {
	CoEdOK			= TT_OK,
	CoEdWarnTimeout		= TT_WRN_APPFIRST, // 512
	CoEdErrFile		= TT_ERR_APPFIRST, // 1536
	CoEdErrNoMem,
	CoEdErrBadPointer,
	CoEdErrXDR,
	CoEdErrBadMsg,
	CoEdErrFailure
} CoEdStatus;

#ifdef InLibCoEd

//
// CoEdTextBuffer is a class that clients of libCoEd implement.
// libCoEd invokes appropriate CoEdTextBuffer methods when it detects
// operations on the CoEdTextBuffer's associated text file.
//
class CoEdTextBuffer {
    public:
	CoEdStatus	insertText( long start, long end, const char *text );
	CoEdStatus	save();
	CoEdStatus	revert();
	CoEdStatus	rename( const char *newPath );
};

#else

class CoEdTextBuffer;

#endif InLibCoEd

class CoEdTextVersion;
class CoEdChangeHistory;
class CoEdChangeQueue;
class CoEdTextChange;
class CoEdSiteIDList;

//
// CoEdFile is the class that libCoEd implements for its clients.
// Clients invoke appropriate CoEdFile methods after they have
// performed the corresponding user operation on the file.
//
class CoEdFile {
	friend class CoEdFileList;
    public:
	CoEdFile( const char *path, CoEdTextBuffer *textBuf,
		  CoEdStatus &status, int timeOutSec = 0 );
	~CoEdFile();

	CoEdStatus	insertText( long start, long end, const char *text );
	CoEdStatus	save();
	CoEdStatus	revert();
	CoEdStatus	rename( const char *newPath );

    private:
	char		       *_path;
	CoEdTextBuffer	       *_textBuf;
	CoEdTextVersion	       *_version;
	CoEdTextVersion	       *_versionInQ;
	CoEdSiteIDList	       *_coEditors;
	CoEdChangeHistory      *_appliedChanges;
	CoEdChangeQueue        *_unAppliedChanges;
	int			_numLocalChanges;
	int			_joining;
	CoEdFile	       *_next;
	CoEdFile	       *_prev;

	CoEdStatus		_handleMsg( Tt_message msg );
	CoEdStatus		_handleRequest( Tt_message msg );
	CoEdStatus		_handleJoin( Tt_message msg );
	CoEdStatus		_handleVersionVote( Tt_message msg );
	CoEdStatus		_handleNotice( Tt_message msg );
	CoEdStatus		_handleChanged( Tt_message msg );
	CoEdStatus		_handlePollVersion( Tt_message msg );
	CoEdStatus		_handleChange( CoEdTextChange *change,
					       int changeIsFromQueue = 0 );
};

CoEdStatus	coEdInit( char* &returnProcID, int &pFd2Watch );
CoEdStatus	coEdFini();
CoEdStatus	coEdHandleActiveFD( int fd );
const char     *coEdStatusMessage( CoEdStatus status );

#endif CoEd_h
