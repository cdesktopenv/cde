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
//%%  $XConsortium: CoEdSiteID.C /main/3 1995/10/20 17:07:11 rswiston $ 			 				
/*
 * CoEdSiteID.cc
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
#include <stdlib.h>
#include "CoEdSiteID.h"

CoEdSiteID::
CoEdSiteID()
{
	_procID = 0;
	_next = 0;
	_prev = 0;
}

CoEdSiteID::
CoEdSiteID( const char *procID )
{
	if (procID == 0) {
		_procID = 0;
	} else {
		_procID = strdup( procID );
	}
	_next = 0;
	_prev = 0;
}

CoEdSiteID::
CoEdSiteID( const CoEdSiteID &id )
{
	if (id._procID == 0) {
		_procID = 0;
	} else {
		_procID = strdup( id._procID );
	}
	_next = 0;
	_prev = 0;
}

CoEdSiteID::
~CoEdSiteID()
{
	if (_procID != 0) {
		free( _procID );
	}
}

bool_t CoEdSiteID::
xdr( XDR *xdrStream )
{
#ifndef WRAPSTRING_FIXED
	int len;
	switch (xdrStream->x_op) {
	    case XDR_ENCODE:
		if (_procID == 0) {
			len = 0;
		} else {
			len = strlen(_procID);
		}
		if (! xdr_int( xdrStream, &len )) {
			return FALSE;
		}
		if (_procID != 0) {
			if (! xdr_vector( xdrStream, _procID, (u_int)len,
					  sizeof(char), (xdrproc_t)xdr_char ))
			{
				return FALSE;
			}
		}
		break;
	    case XDR_DECODE:
		if (! xdr_int( xdrStream, &len )) {
			return FALSE;
		}
		if (_procID != 0) {
			free( _procID );
		}
		_procID = 0;
		if (len > 0) {
			_procID = (char *)malloc( len + 1 );
			if (_procID == 0) {
				return FALSE;
			}
			if (! xdr_vector( xdrStream, _procID, (u_int)len,
					  sizeof(char), (xdrproc_t)xdr_char ))
			{
				free( _procID );
				return FALSE;
			}
			_procID[ len ] = '\0';
		}
		break;
	}
	return TRUE;
#else
	char *string;
	if (! xdr_wrapstring( xdrStream, &string )) {
		return FALSE;
	}
	_procID = strdup( string );
	xdr_free( xdr_wrapstring, string );
	return TRUE;
#endif
}

CoEdSiteID *CoEdSiteID::
copy() const
{
	return new CoEdSiteID( _procID );
}

CoEdSiteID &CoEdSiteID::
copy( const CoEdSiteID *id2Copy )
{
	if (_procID != 0) {
		free( _procID );
	}
	if (id2Copy->_procID == 0) {
		_procID = 0;
	} else {
		_procID = strdup( id2Copy->_procID );
	}
	return *this;
}

int CoEdSiteID::
cmp( const CoEdSiteID &id ) const
{
	if (_procID == id._procID) {
		return 0;
	}
	if (_procID == 0) {
		return -1;
	}
	if (id._procID == 0) {
		return 1;
	}
	return strcmp( _procID, id._procID );
}

void CoEdSiteID::
print( FILE *f ) const
{
	if (_procID != 0) {
		fprintf( f, "%-3.3s", _procID );
	}
}

CoEdSiteIDList::
CoEdSiteIDList()
{
	_head = 0;
	_tail = 0;
	_count = 0;
}

CoEdSiteIDList::
~CoEdSiteIDList()
{
	_flush();
}

void CoEdSiteIDList::
push( CoEdSiteID *site )
{
	site->_next = _head;
	site->_prev = 0;
	if (_tail == 0) {
		_tail = site;
	} else {
		_head->_prev = site;
	}
	_head = site;
	_count++;
}

void CoEdSiteIDList::
append( CoEdSiteID *site )
{
	site->_next = 0;
	site->_prev = _tail;
	if (_head == 0) {
		_head = site;
	} else {
		_tail->_next = site;
	}
	_tail = site;
	_count++;
}

void CoEdSiteIDList::
insertBefore( CoEdSiteID *site1, CoEdSiteID *site2 )
{
	if (_head == site2) {
		push( site1 );
		return;
	}
	site1->_next = site2;
	site1->_prev = site2->_prev;
	site2->_prev->_next = site1;
	site2->_prev = site1;
	_count++;
}

void CoEdSiteIDList::
insertAfter( CoEdSiteID *site2, CoEdSiteID *site1 )
{
	if (_tail == site1) {
		append( site2 );
		return;
	}
	site2->_next = site1->_next;
	site2->_prev = site1;
	site1->_next->_prev = site2;
	site1->_next = site2;
	_count++;
}

void CoEdSiteIDList::
_flush()
{
	CoEdSiteID *curr = _head;
	CoEdSiteID *prev;
	while (curr != 0) {
		prev = curr;
		curr = curr->_next;
		delete prev;
	}
}
