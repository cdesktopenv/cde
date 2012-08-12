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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: CoEdFile.C /main/3 1995/10/20 17:06:45 rswiston $ 			 				
/*
 * CoEdFile.cc
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
#include <sys/param.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "CoEdFile.h"
#include "CoEdGlobals.h"
#include "CoEdChangeHistory.h"
#include "CoEdChangeQueue.h"
#include "CoEdTextVersion.h"

#undef DEBUG

CoEdFile::
CoEdFile( const char *path, CoEdTextBuffer *textBuf, CoEdStatus &status,
	  int timeOutSec )
{
	Tt_status err;

	_next = 0;
	_numLocalChanges = 0;
	_joining = 1;
	if (path == 0) {
		fprintf( stderr, "libCoEd: can't join null file\n" );
		status = CoEdErrFile;
		return;
	}
	_appliedChanges = new CoEdChangeHistory;
	if (_appliedChanges == 0) {
		status = CoEdErrNoMem;
		return;
	}
	_textBuf = textBuf;
	if (_textBuf == 0) {
		status = CoEdErrBadPointer;
		return;
	}
	_unAppliedChanges = new CoEdChangeQueue;
	if (_unAppliedChanges == 0) {
		status = CoEdErrNoMem;
		return;
	}
	_version = new CoEdTextVersion;
	if (_version == 0) {
		status = CoEdErrNoMem;
		return;
	}
	_versionInQ = new CoEdTextVersion;
	if (_versionInQ == 0) {
		status = CoEdErrNoMem;
		return;
	}
	_coEditors = new CoEdSiteIDList;
	if (_coEditors == 0) {
		status = CoEdErrNoMem;
		return;
	}
	//
	// Join the file.
	//
	err = tt_file_join( path );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: %s: %s\n", path,
			 tt_status_message( err ));
		status = (CoEdStatus)err;
		return;
	}
	//
	// Trick ToolTalk into translating the path into the canonical
	// path that it will use to label messages about this file.
	//
	char *oldDefaultFile = tt_default_file();
	err = tt_ptr_error( oldDefaultFile );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_default_file(): %s\n",
			 tt_status_message( err ));
		status = (CoEdStatus)err;
		return;
	}
	err = tt_default_file_set( path );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_default_file_set(\"%s\"): %s\n",
			 path, tt_status_message( err ));
		status = (CoEdStatus)err;
		return;
	}
	char *temp = tt_default_file();
	err = tt_ptr_error( temp );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_default_file(): %s\n",
			 tt_status_message( err ));
		status = (CoEdStatus)err;
		return;
	}
	_path = strdup( temp );
	if (_path == 0) {
		status = CoEdErrNoMem;
		return;
	}
	tt_free( temp );
	if (oldDefaultFile != 0) {
		//
		// Reset the default file to what it was.
		//
		Tt_status err = tt_default_file_set( oldDefaultFile );
		if (err != TT_OK) {
			fprintf( stderr, "libCoEd: tt_default_file_set(\"%s\")"
				 ": %s\n", oldDefaultFile,
				 tt_status_message( err ));
			status = (CoEdStatus)err;
			return;
		}
	}
	tt_free( oldDefaultFile );
	//
	// Ask to join the file.
	//
	Tt_message msg = tt_prequest_create( TT_FILE, "Text_File_Join" );
	Tt_status ttErr = tt_ptr_error( msg );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_prequest_create(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	//
	// Set the file of the message.
	//
	ttErr = tt_message_file_set( msg, _path );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_file_set(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	//
	// Send the message.
	//
	ttErr = tt_message_send( msg );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_send(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	//
	// Add ourselves to the list of files joined.
	//
	coEdFiles->append( this );
	//
	// Wait for the reply.
	//
	status = CoEdOK;
	time_t start = time(0);
	struct rlimit nofile;
	getrlimit( RLIMIT_NOFILE, &nofile );
	struct pollfd fds[ 1 ];
	fds[ 0 ].fd = coEdTtFd;
	fds[ 0 ].events = POLLIN;
	while (_joining && (status == CoEdOK)) {
		if ((timeOutSec > 0) && (time(0) - start > timeOutSec)) {
			status = CoEdWarnTimeout;
			break;
		}
		int activeFDs = poll( fds, 1, (1000*timeOutSec) );
		if (activeFDs > 0) {
			if (fds[ 0 ].revents & POLLIN) {
				status = coEdHandleActiveFD( coEdTtFd );
			}
		} else if (activeFDs == 0) {
			status = CoEdWarnTimeout;
		} else {
			perror( "libCoEd" );
			status = CoEdErrFailure;
		}
	}
}

CoEdFile::
~CoEdFile()
{
	// XXX unjoin from the file, remove from coEdFiles
	if (_appliedChanges != 0) {
		delete _appliedChanges;
	}
	if (_unAppliedChanges != 0) {
		delete _unAppliedChanges;
	}
	if (_version != 0) {
		delete _version;
	}
	if (_versionInQ != 0) {
		delete _versionInQ;
	}
	if (_path != 0) {
		free( _path );
	}
}

CoEdStatus CoEdFile::
insertText( long start, long end, const char *text )
{
	CoEdTextChange *change;
	_numLocalChanges++;
	change = new CoEdTextChange( start, end, text, _version, coEdSiteID,
				     _numLocalChanges );
	_version   ->update( *coEdSiteID, _numLocalChanges );
	_versionInQ->update( *coEdSiteID, _numLocalChanges );
	if (change == 0) {
		return CoEdErrNoMem;
	}
	_appliedChanges->insert( change );
	return change->broadcast( _path );
}

CoEdStatus CoEdFile::
_handleMsg( Tt_message msg )
{
	Tt_class theClass = tt_message_class( msg );
	Tt_status err = tt_int_error( theClass );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_class(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	switch (theClass) {
	    case TT_REQUEST:
		return _handleRequest( msg );
	    case TT_NOTICE:
		return _handleNotice( msg );
	    default:
		fprintf( stderr, "libCoEd: bad Tt_class!\n" );
		return CoEdOK;
	}
}

CoEdStatus CoEdFile::
_handleRequest( Tt_message msg )
{
	CoEdStatus val2Return;
	char *op = tt_message_op( msg );
	Tt_status err = tt_ptr_error( op );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_op(): %s\n",
			 tt_status_message( err ));
		val2Return = (CoEdStatus)err;
	}
	if (op == 0) {
		fprintf( stderr, "libCoEd: msg has null op!\n" );
		val2Return = CoEdErrBadMsg;
	}
	if (! strcmp( op, "Text_File_Join" )) {
		val2Return = _handleJoin( msg );
	} else if (! strcmp( op, "Text_File_Version_Vote" )) {
		//val2Return = _handleVersionVote( msg );
	} else {
		fprintf( stderr, "libCoEd: unknown msg op \"%s\"\n", op );
		val2Return = CoEdErrBadMsg;
	}
	return val2Return;
}

CoEdStatus CoEdFile::
_handleJoin( Tt_message msg )
{
	Tt_status ttErr;

	Tt_state state = tt_message_state( msg );
	ttErr = tt_int_error( state );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_state(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	switch (state) {
		char *sender;
		Tt_status ttErr;
	    case TT_FAILED:
		//
		// Nobody handled our request, so we must be the
		// first process to have joined the file.
		//
		_joining = 0;
		return CoEdOK;
	    case TT_SENT:
		//
		// If the Text_File_Join request was sent by us, we
		// don't care about it.
		//
		sender = tt_message_sender( msg );
		ttErr = tt_ptr_error( sender );
		if (ttErr != TT_OK) {
			fprintf( stderr, "libCoEd: tt_message_sender(): %s\n",
				 tt_status_message( ttErr ));
			return (CoEdStatus)ttErr;
		}
		if (sender == 0) {
			return CoEdErrBadMsg;
		}
		if (! strcmp( sender, coEdProcID )) {
			tt_free( sender );
			//
			// The request was made by us, so we reject it, in
			// order to give someone in the know a chance to
			// handle it.
			//
			ttErr = tt_message_reject( msg );
			if (ttErr != TT_OK) {
				fprintf( stderr,
					 "libCoEd: tt_message_reject(): %s\n",
					 tt_status_message( ttErr ));
				return (CoEdStatus)ttErr;
			}
			return CoEdOK;
		}
		tt_free( sender );
		// XXX Quiesce the file, and ship 'em a copy.
		tt_message_fail( msg );
		return CoEdOK;
	    default:
		fprintf( stderr, "msg state: %d!\n", (int)state );
		tt_message_reject( msg );
		break;
	}
	return CoEdOK;

} /* CoEdFile::_handleJoin() */

CoEdStatus CoEdFile::
_handleNotice( Tt_message msg )
{
	Tt_state state = tt_message_state( msg );
	Tt_status err = tt_int_error( state );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_state(): %s\n",
			 tt_status_message( err ));
		return (CoEdStatus)err;
	}
	if (state != TT_SENT) {
		return CoEdOK;
	}
	CoEdStatus val2Return;
	char *op = tt_message_op( msg );
	err = tt_ptr_error( op );
	if (err != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_op(): %s\n",
			 tt_status_message( err ));
		val2Return = (CoEdStatus)err;
	}
	if (op == 0) {
		fprintf( stderr, "libCoEd: msg has null op!\n" );
		val2Return = CoEdErrBadMsg;
	}
	if (! strcmp( op, "Text_File_Changed" )) {
		val2Return = _handleChanged( msg );
	} else if (! strcmp( op, "Text_File_Poll_Version" )) {
		val2Return = _handlePollVersion( msg );
	} else {
		fprintf( stderr, "libCoEd: unknown msg op \"%s\"\n", op );
		val2Return = CoEdErrBadMsg;
	}
	tt_message_destroy( msg );
	return val2Return;
}

CoEdStatus CoEdFile::
_handleChanged( Tt_message msg )
{
	if (_joining) {
		fprintf( stderr, "libCoEd: warning: got a change while "
			 "joining \"%s\"\n", _path );
		return CoEdOK;
	}
	//
	// If the Text_File_Changed notice was sent by us, we don't care
	// about it.
	//
	char *sender = tt_message_sender( msg );
	Tt_status ttErr = tt_ptr_error( sender );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_sender(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	if (sender == 0) {
		return CoEdErrBadMsg;
	}
	if (! strcmp( sender, coEdProcID )) {
		tt_free( sender );
		return CoEdOK;
	}
	tt_free( sender );
	//
	// It was not sent by us.  Process it.
	//
	CoEdStatus	err;
	CoEdTextChange *change = new CoEdTextChange( msg, err );
	if (err != CoEdOK) {
		return CoEdOK;
	}
	return _handleChange( change );
}

CoEdStatus CoEdFile::
_handlePollVersion( Tt_message msg )
{
	//
	// If the Text_File_Changed notice was sent by us, we don't care
	// about it.
	//
	char *sender = tt_message_sender( msg );
	Tt_status ttErr = tt_ptr_error( sender );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_sender(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	if (sender == 0) {
		return CoEdErrBadMsg;
	}
	if (! strcmp( sender, coEdProcID )) {
		tt_free( sender );
		return CoEdOK;
	}
	tt_free( sender );
	//
	// It was not sent by us.  Respond.
	//
	Tt_message response = tt_prequest_create( TT_FILE,
						  "Text_File_Version_Vote" );
	ttErr = tt_ptr_error( response );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_prequest_create(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Set the file of the response.
	//
	ttErr = tt_message_file_set( response, _path );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_file_set(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Aim the response back at the sender
	//
	ttErr = tt_message_handler_set( response, sender );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_handler_set(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Send the message.
	//
	ttErr = tt_message_send( response );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_send(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// We don't expect or care about a reply, so destroy the
	// message now.
	//
	return CoEdOK;
}

CoEdStatus CoEdFile::
_handleChange( CoEdTextChange *change, int changeIsFromQueue )
{
	if (change->knowsOfNewerChangesThan( *_version )) {
		if (changeIsFromQueue) {
			fprintf( stderr, "Re-queuing change!\n" );
			abort();
		}
		_unAppliedChanges->insert( change );
		_versionInQ->update( change->causer(), change->changeNum() );
	} else {
		_appliedChanges->insert( change );
		CoEdTextChange *translatedChange =
			_appliedChanges->translate( *change );
		if (translatedChange != 0) {
			CoEdStatus err;
			err = _textBuf->insertText( translatedChange->start(),
						    translatedChange->end(),
						    translatedChange->text() );
			if (err != CoEdOK) {
				fprintf( stderr, "libCoEd: CoEdTextBuffer::"
					 "insertText(): %d!  Failed change: ");
				translatedChange->print( stderr );
			}
			delete translatedChange;
		}
		_version->update( change->causer(), change->changeNum() );
		if (! changeIsFromQueue) {
			_versionInQ->update( change->causer(),
					     change->changeNum() );
		}
	}
	CoEdTextChange *newlyEligibleChange =
		_unAppliedChanges->deQEligibleChng( *_version );
	if (newlyEligibleChange != 0) {
		return _handleChange( newlyEligibleChange, 1 );
	} else {
		return CoEdOK;
	}
}

CoEdFileList::
CoEdFileList()
{
	_head = 0;
	_tail = 0;
	_count = 0;
}

CoEdFileList::
~CoEdFileList()
{
	CoEdFile *curr = _head;
	CoEdFile *prev;
	while (curr != 0) {
		prev = curr;
		curr = curr->_next;
		delete prev;
	}
}

void CoEdFileList::
push( CoEdFile *file )
{
	file->_next = _head;
	file->_prev = 0;
	if (_tail == 0) {
		_tail = file;
	} else {
		_head->_prev = file;
	}
	_head = file;
	_count++;
}

void CoEdFileList::
append( CoEdFile *file )
{
	file->_next = 0;
	file->_prev = _tail;
	if (_head == 0) {
		_head = file;
	} else {
		_tail->_next = file;
	}
	_tail = file;
	_count++;
}

CoEdStatus CoEdFileList::
handleMsg( const char *path, Tt_message msg )
{
	if (path == 0) {
		fprintf( stderr, "libCoEd: got msg for null file!\n" );
		return CoEdErrFile;
	}
	CoEdFile *curr = _head;
	while (curr != 0) {
		if ((curr->_path != 0) && (! strcmp( path, curr->_path))) {
			return curr->_handleMsg( msg );
		}
		curr = curr->_next;
	}
	fprintf( stderr, "libCoEd: \"%s\" is not a file being CoEdited.\n",
		 path );
	return CoEdErrFile;
}
