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
//%%  $XConsortium: CoEdTextChange.C /main/3 1995/10/20 17:07:30 rswiston $ 			 				
/*
 * CoEdTextChange.cc
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
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "CoEdTextVersion.h"
#include "CoEdTextChange.h"

#undef DEBUG

CoEdTextChange::
CoEdTextChange()
{
	_start = 0;
	_end = 0;
	_text = 0;
	_appliesTo = 0;
	_causer = 0;
	_changeNum = 0;
	_charsAdded = 0;
	_next = 0;
	_prev = 0;
}

CoEdTextChange::
CoEdTextChange( long start, long end, const char *text,
	        const CoEdTextVersion *appliesTo, const CoEdSiteID *causer,
	        int changeNum )
{
	_next = 0;
	_prev = 0;
	_start = start;
	_end = end;
	if (text != 0) {
		_text = strdup( text );
	} else {
		_text = 0;
	}
	_charsAdded =   ((_text == 0) ? 0 : strlen( _text ))
		      - (_end - _start);
	_appliesTo = appliesTo->copy();
	_causer = causer->copy();
	_changeNum = changeNum;
}

CoEdTextChange::
CoEdTextChange( const CoEdTextChange &change )
{
	_next = 0;
	_prev = 0;
	_start = change._start;
	_end = change._end;
	if (change._text != 0) {
		_text = strdup( change._text );
	} else {
		_text = 0;
	}
	_charsAdded = change._charsAdded;
	_appliesTo = change._appliesTo->copy();
	_causer = change._causer->copy();
	_changeNum = change._changeNum;
}

CoEdTextChange::
CoEdTextChange( Tt_message msg, CoEdStatus &status )
{
	unsigned char  *data;
	int		len;

	_next = 0;
	_prev = 0;
	_charsAdded = 0;
	//
	// Extract arg 0: CoEdTextVersion
	//
	Tt_status ttErr = tt_message_arg_bval( msg, 0, &data, &len );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_arg_bval(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	_appliesTo = new CoEdTextVersion( data, len, status );
	if (status != CoEdOK) {
		return;
	}
	//
	// Extract arg 1: changeNum
	//
	int argVal;
	ttErr = tt_message_arg_ival( msg, 1, &argVal );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_arg_ival(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	_changeNum = argVal;
	//
	// Extract arg 2: start
	//
	ttErr = tt_message_arg_ival( msg, 2, &argVal );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_arg_ival(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	_start = argVal;
	//
	// Extract arg 3: end
	//
	ttErr = tt_message_arg_ival( msg, 3, &argVal );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_arg_ival(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	_end = argVal;
	//
	// Extract arg 4: text
	//
	char *temp = tt_message_arg_val( msg, 4 );
	ttErr = tt_ptr_error( temp );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_arg_val(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	if (temp == 0) {
		_text = 0;
	} else {
		_text = strdup( temp );
		if (_text == 0) {
			fprintf( stderr, "libCoEd: ran out of memory!\n" );
			status = CoEdErrNoMem;
			return;
		}
		tt_free( temp );
	}
	_charsAdded =   _end - _start
		      + ((_text == 0) ? 0 : strlen( _text ));
	//
	// Extract who caused the change.
	//
	temp = tt_message_sender( msg );
	ttErr = tt_ptr_error( temp );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_sender(): %s\n",
			 tt_status_message( ttErr ));
		status = (CoEdStatus)ttErr;
		return;
	}
	_causer = new CoEdSiteID( temp );
	if (_causer == 0) {
		fprintf( stderr, "libCoEd: ran out of memory!\n" );
		status = CoEdErrNoMem;
		return;
	}
	tt_free( temp );
	status = CoEdOK;
}

CoEdTextChange::
~CoEdTextChange()
{
	if (_text != 0) {
		free( (char *)_text );
	}
	if (_appliesTo != 0) {
		delete _appliesTo;
	}
	if (_causer != 0) {
		delete _causer;
	}
}

CoEdStatus CoEdTextChange::
broadcast( const char *path ) const
{
	//
	// Create the message.
	//
	Tt_message msg = tt_pnotice_create( TT_FILE, "Text_File_Changed" );
	Tt_status ttErr = tt_ptr_error( msg );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_pnotice_create(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Set the file of the message.
	//
	ttErr = tt_message_file_set( msg, path );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_file_set(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Add arg 0: CoEdTextVersion
	//
	unsigned long	len;
	unsigned char  *data;
	CoEdStatus err = _appliesTo->serialize( &data, &len );
	if (err != CoEdOK) {
		fprintf( stderr, "libCoEd: CoEdTextVersion::serialize(): %d\n",
			 (int)err );
		return err;
	}
	ttErr = tt_message_barg_add( msg, TT_IN, "CoEdTextVersion", data,
				     (int)len );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_barg_add(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Add arg 1: changeNum
	//
	ttErr = tt_message_iarg_add( msg, TT_IN, "int", (int)_changeNum );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_iarg_add(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Add arg 2: start
	//
	ttErr = tt_message_iarg_add( msg, TT_IN, "int", (int)_start );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_iarg_add(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Add arg 3: end
	//
	ttErr = tt_message_iarg_add( msg, TT_IN, "int", (int)_end );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_iarg_add(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Add arg 4: text
	//
	if (_text == 0) {
		ttErr = tt_message_arg_add( msg, TT_IN, "string", "" );
	} else {
		ttErr = tt_message_arg_add( msg, TT_IN, "string", _text );
	}
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_arg_add(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	//
	// Send the message.
	//
#ifdef DEBUG
	static Tt_message oldMsg = 0;
	if (oldMsg == 0) {
		oldMsg = msg;
		return CoEdOK;
	} else {
		ttErr = tt_message_send( oldMsg );
		if (ttErr != TT_OK) {
			fprintf( stderr, "libCoEd: tt_message_send(): %s\n",
				 tt_status_message( ttErr ));
			return (CoEdStatus)ttErr;
		}
		ttErr = tt_message_destroy( oldMsg );
		if (ttErr != TT_OK) {
			fprintf( stderr, "libCoEd: tt_message_destroy(): %s\n",
				 tt_status_message( ttErr ));
			return (CoEdStatus)ttErr;
		}
		oldMsg = 0;
	}
#endif DEBUG
	ttErr = tt_message_send( msg );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_send(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	ttErr = tt_message_destroy( msg );
	if (ttErr != TT_OK) {
		fprintf( stderr, "libCoEd: tt_message_destroy(): %s\n",
			 tt_status_message( ttErr ));
		return (CoEdStatus)ttErr;
	}
	return CoEdOK;
}

//
// This method serves two purposes.  First, it takes chng2Xlate,
// and adjusts it forward in time over this change, so that we
// will be able to apply the modified chng2Xlate to our local
// text (to which this change has already been applied).
// Second, since the _un_modified chng2Xlate is being put into the
// history list _before_ this change, we need to modify this
// change so that it makes sense when applied to a text that
// has already had the unmodified chng2Xlate applied to it.
//
void CoEdTextChange::
interTranslate( CoEdTextChange &chng2Xlate )
{
	//
	// First, adjust chng2Xlate forward in time over this change.
	//
	int directionToLean = (chng2Xlate._end > _end) ? 1 : 0;
	long xlatdStart = adjustPt( chng2Xlate._start, *chng2Xlate._causer,
				    directionToLean );
	directionToLean = (chng2Xlate._start < _start) ? -1 : 0;
	long xlatdEnd = adjustPt( chng2Xlate._end, *chng2Xlate._causer,
				  directionToLean );
	//
	// Second, adjust this change into a form that makes
	// sense if chng2Xlate had already been applied when
	// it was time to apply this change.
	//
	directionToLean = (_end > chng2Xlate._end) ? 1 : 0;
	long _startNew = chng2Xlate.adjustPt( _start, *_causer,
					      directionToLean );
	directionToLean = (_start < chng2Xlate._start) ? -1 : 0;
	long _endNew = chng2Xlate.adjustPt( _end, *_causer, directionToLean );
	//
	// Now, modify this change and chng2Xlate, as calculated.
	//
	_start = _startNew;
	_end = _endNew;
	//_appliesTo->update( *chng2Xlate._causer, chng2Xlate._changeNum );
	chng2Xlate._start = xlatdStart;
	chng2Xlate._end = xlatdEnd;
	//chng2Xlate._appliesTo->update( *_causer, _changeNum );
}

void CoEdTextChange::
translateOver( const CoEdTextChange &chng2Hurdle )
{
	int directionToLean = (_end > chng2Hurdle._end) ? 1 : 0;
	long newStart = chng2Hurdle.adjustPt( _start, *_causer,
					      directionToLean );
	directionToLean = (_start < chng2Hurdle._start) ? -1 : 0;
	long newEnd = chng2Hurdle.adjustPt( _end, *_causer,
					    directionToLean );
	//_appliesTo->update( *chng2Hurdle._causer, chng2Hurdle._changeNum );
	_start = newStart;
	_end = newEnd;
}

long CoEdTextChange::
adjustPt( long pt, const CoEdSiteID &ptOwner, int directionToLean ) const
{
	if (pt < _start) {
		return pt;
	}
	if (pt > _end) {
		return pt + charsAddedBy();
	}
	//
	// pt lies within our change, so it needs to be adjusted to
	// one end of our change.  First, check if the caller has
	// a preference.
	//
	if (directionToLean < 0) {
		return _start;
	} else if (directionToLean > 0) {
		return _start + charsAddedBy();
	} else {
		//
		// The caller has no preference, so decide by order
		// of CoEdSiteID.
		//
		if (*_causer < ptOwner) {
			return _start + charsAddedBy();
		} else {
			return _start;
		}
	}
}

void CoEdTextChange::
print( FILE *f )
{
	fprintf( f, "[%3d, %3d] %2d: <", _start, _end, _charsAdded );
	if (_text == 0) {
		fprintf( f, "%6s", ">" );
	} else {
		fprintf( f, "%-5.5s>", _text );
	}
	fprintf( f, " #%2d ", _changeNum );
	_causer->print( f );
	fputs( " ", f );
	_appliesTo->print( f );
	fputs( "\n", f );
}

CoEdTextChangeList::
CoEdTextChangeList()
{
	_head = 0;
	_tail = 0;
	_count = 0;
}

CoEdTextChangeList::
~CoEdTextChangeList()
{
	CoEdTextChange *curr = _head;
	CoEdTextChange *prev;
	while (curr != 0) {
		prev = curr;
		curr = curr->_next;
		delete prev;
	}
}

void CoEdTextChangeList::
push( CoEdTextChange *change )
{
	change->_next = _head;
	change->_prev = 0;
	if (_tail == 0) {
		_tail = change;
	} else {
		_head->_prev = change;
	}
	_head = change;
	_count++;
}

void CoEdTextChangeList::
append( CoEdTextChange *change )
{
	change->_next = 0;
	change->_prev = _tail;
	if (_head == 0) {
		_head = change;
	} else {
		_tail->_next = change;
	}
	_tail = change;
	_count++;
}

void CoEdTextChangeList::
insertBefore( CoEdTextChange *change1, CoEdTextChange *change2 )
{
	if (_head == change2) {
		push( change1 );
		return;
	}
	change1->_next = change2;
	change1->_prev = change2->_prev;
	change2->_prev->_next = change1;
	change2->_prev = change1;
	_count++;
}

void CoEdTextChangeList::
insertAfter( CoEdTextChange *change2, CoEdTextChange *change1 )
{
	if (_tail == change1) {
		append( change2 );
		return;
	}
	change2->_next = change1->_next;
	change2->_prev = change1;
	change1->_next->_prev = change2;
	change1->_next = change2;
	_count++;
}

CoEdTextChange *CoEdTextChangeList::
remove( CoEdTextChange *change )
{
	if (change == _head) {
		_head = change->_next;
	} else {
		change->_prev->_next = change->_next;
	}
	if (change == _tail) {
		_tail = change->_prev;
	} else {
		change->_next->_prev = change->_prev;
	}
	change->_next = 0;
	change->_prev = 0;
	_count--;
	return change;
}

void CoEdTextChangeList::
print( FILE *f, char *indent )
{
	CoEdTextChange *curr = _head;
	while (curr != 0) {
		fprintf( f, indent );
		curr->print( f );
		curr = curr->_next;
	}
}
