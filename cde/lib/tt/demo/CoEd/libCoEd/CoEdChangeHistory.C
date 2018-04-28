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
//%%  $XConsortium: CoEdChangeHistory.C /main/3 1995/10/20 17:06:11 rswiston $ 			 				
/*
 * CoEdChangeHistory.cc
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

#include <string.h>
#include "CoEdChangeHistory.h"

CoEdChangeHistory::
CoEdChangeHistory() : CoEdTextChangeList()
{
}

void CoEdChangeHistory::
insert( CoEdTextChange *change )
{
	CoEdTextChange *curr = _tail;
	//
	// Find the most recent change in the history that the
	// incoming change knows of.
	//
	while (curr != 0) {
		if (change->knowsOf( *curr )) {
			break;
		}
		curr = curr->_prev;
	}
	//
	// Bump curr to point to the first change that the incoming
	// change doesn't know of.
	//
	if (curr == 0) {
		curr = _head;
	} else {
		curr = curr->_next;
	}
	//
	// The rest of the changes are mutually ignorant with the incoming
	// change.  Find the first one with a greater site id, and
	// stick this change in front of it.
	//
	while (curr != 0) {
		if (*change->_causer < *curr->_causer) {
			break;
		}
		curr = curr->_next;
	}
	if (curr == 0) {
		append( change );
	} else {
		insertBefore( change, curr );
	}
}

CoEdTextChange *CoEdChangeHistory::
translate( CoEdTextChange &change )
{
	_translateOverEarlierChgs( change );
	return _translateOverLaterChgs( change );
}

//
// Modify <change> so that it takes into account any changes ahead of
// it in the change history that it does not know about.
//
void CoEdChangeHistory::
_translateOverEarlierChgs( CoEdTextChange &change )
{
	CoEdTextChange *curr = _head;
	while (curr != &change) {
		if (! change.knowsOf( *curr )) {
			change.translateOver( *curr );
		}
		curr = curr->_next;
	}
}

//
// Take <change>, which is assumed to have been inserted into this
// ChangeHistory, and adjust the remaining changes in the history
// so that they take into account the change inserted ahead of them.
// Also, return a new CoEdTextChange that is a translated version
// of <change>, suitable for application to a textbuffer that
// has already had the remaining changes in the history applied to it.
//
CoEdTextChange *CoEdChangeHistory::
_translateOverLaterChgs( const CoEdTextChange &change )
{
	CoEdTextChange *xlatdChng = new CoEdTextChange( change );
	if (xlatdChng == 0) {
		return 0;
	}

	CoEdTextChange *curr = change._next;
	while (curr != 0) {
		curr->interTranslate( *xlatdChng );
		curr = curr->_next;
	}
	return xlatdChng;
}

