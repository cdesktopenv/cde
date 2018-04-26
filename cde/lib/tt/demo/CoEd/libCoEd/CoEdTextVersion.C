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
//%%  $XConsortium: CoEdTextVersion.C /main/3 1995/10/23 09:43:59 rswiston $ 			 				
/*
 * CoEdTextVersion.cc
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

#include "CoEdTextVersion.h"

CoEdTextVersion::
CoEdTextVersion() : SiteChangeList()
{
}

CoEdTextVersion::
CoEdTextVersion( unsigned char *data, int len, CoEdStatus &status )
	: SiteChangeList( data, len, status )
{
}

void CoEdTextVersion::
update( const CoEdSiteID &site, int numChanges )
{
	SiteChange *curr = _head;
	int found = 0;
	while (curr != 0) {
		if ( *(CoEdSiteID *)curr == site ) {
			curr->_changeNum = numChanges;
			found = 1;
		}
		curr = (SiteChange *)curr->_next;
	}
	if (! found) {
		curr = new SiteChange( site, numChanges );
		insert( curr );
	}
}

CoEdTextVersion *CoEdTextVersion::
copy() const
{
	CoEdTextVersion *newVersion = new CoEdTextVersion;
	newVersion->SiteChangeList::copy( (const SiteChangeList *)this );
	return newVersion;
}

//
// This method assumes that the CoEdSiteID of <change> is _not_ already
// in the list of SiteChanges!
//
void CoEdTextVersion::
insert( SiteChange *change )
{
	SiteChange *curr = _head;
	int inserted	 = 0;
	while ((curr != 0) && (! inserted)) {
		if (*change < *curr) {
			insertBefore( change, curr );
			inserted = 1;
		}
		curr = (SiteChange *)curr->_next;
	}
	if (! inserted) {
		append( change );
	}
}

int CoEdTextVersion::
knowsOfNewerChangesThan( const CoEdTextVersion &version ) const
{
	SiteChange *curr1 = _head;
	SiteChange *curr2 = version._head;
	while ((curr1 != 0) && (curr2 != 0)) {
		int wait = 0;
		int diff = curr1->cmp( *curr2 );
		switch (diff) {
		    case -2:
			// I know of a change from a site that he
			// has not even heard from.
		    case 1:
			// I know of a change from this site that
			// is later than the one he knows.
			return 1;
		    case -1:
			// He knows of a change from this site that
			// is later than the one I know. Big deal.
			break;
		    case 2:
			// He knows of a change from a site that I
			// have not even heard from.  So wait until
			// my next site comes up in his list.
			wait = 1;
			break;
		    case 0:
			break;
		}
		if (! wait) {
			curr1 = (SiteChange *)curr1->_next;
		}
		curr2 = (SiteChange *)curr2->_next;
	}
	if ((curr1 != 0) && (curr2 == 0)) {
		//
		// I still have a change from a site that he has
		// never even heard from.
		//
		return 1;
	}
	return 0;
}

int CoEdTextVersion::
knowsOf( const CoEdSiteID &site, int numChanges ) const
{
	SiteChange *curr = _head;
	while (curr != 0) {
		if ( *(CoEdSiteID *)curr == site ) {
			return (curr->_changeNum >= numChanges);
		}
		curr = (SiteChange *)curr->_next;
	}
	return 0;
}
