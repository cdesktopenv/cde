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
//%%  $XConsortium: SiteChange.C /main/3 1995/10/23 09:44:43 rswiston $ 			 				
/*
 * SiteChange.cc
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

#include <stdlib.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <memory.h>
#include "SiteChange.h"

bool_t
coEdPutLong( XDR *xdrStream, long *)
{
	xdrStream->x_handy += 4;
	return 1;
}

bool_t
coEdPutBytes( XDR *xdrStream, caddr_t, int len )
{
	xdrStream->x_handy += RNDUP(len);
	return 1;
}

long *
coEdInline( XDR *xdrStream, int len )
{
	//
	// Be paranoid -- some code really expects inline to
	// always succeed, so we keep a small buffer around
	// just in case.  Not too paranoid, though -- it's
	// legal to not support inline!
	//
	if ((len > 0) && (len < (int)xdrStream->x_base)) {
		xdrStream->x_handy += RNDUP(len);
		return (long *)xdrStream->x_private;
	} else {
		return 0;
	}
}

unsigned long
coEdXdrSizeOf( xdrproc_t f, void *data )
{
	static long sizeBuf[ 5 ];
#ifdef __SABER__
	static struct xdr_ops sizeOps;
	sizeOps.x_putlong = (int (*)(...))coEdPutLong;
	sizeOps.x_putbytes = (int (*)(...))coEdPutBytes;
	sizeOps.x_inline = (long *(*)(...))coEdInline;
	static XDR xdrSizeStream;
	xdrSizeStream.x_op = XDR_ENCODE;
	xdrSizeStream.x_ops = &sizeOps;
	xdrSizeStream.x_public = 0;
	xdrSizeStream.x_private = (caddr_t)sizeBuf;
	xdrSizeStream.x_base = (caddr_t)sizeof sizeBuf;
	xdrSizeStream.x_handy = 0;
#elif defined(sun) && defined(_rpc_xdr_h)
	// the old Sun C++ rpc/xdr.h include had bogus declarations of
	// the xdr functions. Fortunately we can detect its existence
	// because its file guard was lowercase!
	static struct XDR::xdr_ops sizeOps = { 0, (int (*)(...))coEdPutLong, 0,
					       (int (*)(...))coEdPutBytes, 0, 0,
					       (long *(*)(...))coEdInline, 0 };
	static XDR xdrSizeStream = { XDR_ENCODE, &sizeOps, 0, (caddr_t)sizeBuf,
				     (caddr_t)sizeof sizeBuf, 0 };
#else 
	static struct XDR::xdr_ops sizeOps = { 0, coEdPutLong, 0,
					       coEdPutBytes, 0, 0,
					       coEdInline, 0 };
	static XDR xdrSizeStream = { XDR_ENCODE, &sizeOps, 0, (caddr_t)sizeBuf,
				     (caddr_t)sizeof sizeBuf, 0 };
#endif
	xdrSizeStream.x_handy = 0;

	if ((*(bool_t(*)(XDR *, void *))f) (&xdrSizeStream, (void *)data) == 1) {
		return (unsigned long)xdrSizeStream.x_handy;
	} else {
		return 0;
	}
}

SiteChange::
SiteChange() : CoEdSiteID()
{
	_changeNum = 0;
}

SiteChange::
SiteChange( const char *procID ) : CoEdSiteID( procID )
{
	_changeNum = 0;
}

SiteChange::
SiteChange( const CoEdSiteID &id, int numChanges ) : CoEdSiteID( id )
{
	_changeNum = numChanges;
}

bool_t SiteChange::
xdr( XDR *xdrStream )
{
	if (! CoEdSiteID::xdr( xdrStream )) {
		return FALSE;
	}
	if (! xdr_int( xdrStream, &_changeNum )) {
		return FALSE;
	}
	return TRUE;
}

SiteChange *SiteChange::
copy() const
{
	SiteChange *newSiteChange = new SiteChange;
	newSiteChange->CoEdSiteID::copy( (CoEdSiteID *)this );
	newSiteChange->_changeNum = _changeNum;
	return newSiteChange;
}

int SiteChange::
cmp( const SiteChange &change ) const
{
	int idCmp = CoEdSiteID::cmp( (const CoEdSiteID &)change );
	if (idCmp != 0) {
		return 2*idCmp;
	}
	if (_changeNum == change._changeNum) {
		return 0;
	} else if (_changeNum < change._changeNum) {
		return -1;
	} else {
		return 1;
	}
}

void SiteChange::
print( FILE *f ) const
{
	fprintf( f, "(%2d, ", _changeNum );
	CoEdSiteID::print( f );
	fputs( ")", f );
}

SiteChangeList::
SiteChangeList()
{
	_head = 0;
	_tail = 0;
	_count = 0;
}

SiteChangeList::
SiteChangeList( unsigned char *data, int len, CoEdStatus &status )
{
	_head = 0;
	_tail = 0;
	_count = 0;
	status = deSerialize( data, (unsigned long)len );
}

SiteChangeList::
~SiteChangeList()
{
	_flush();
}

void SiteChangeList::
push( SiteChange *change )
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

void SiteChangeList::
append( SiteChange *change )
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

void SiteChangeList::
insertBefore( SiteChange *change1, SiteChange *change2 )
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

void SiteChangeList::
insertAfter( SiteChange *change2, SiteChange *change1 )
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

bool_t
xdrSiteChangeList( XDR *xdrStream, caddr_t *data )
{
	return ((SiteChangeList *)(data))->xdr( xdrStream );
}

CoEdStatus SiteChangeList::
serialize( unsigned char **pData, unsigned long *pLen ) const
{
	XDR		xdrStream;

	*pLen = coEdXdrSizeOf( (xdrproc_t)xdrSiteChangeList, (void *)this );
	if (*pLen == 0) {
		return CoEdErrXDR;
	}
	*pData = (unsigned char *)malloc( (unsigned int)*pLen );
	xdrmem_create( &xdrStream, (caddr_t)*pData, (unsigned int)*pLen,
		       XDR_ENCODE );
	if (! xdrSiteChangeList( &xdrStream, (caddr_t *)this )) {
		return CoEdErrXDR;
	}
	return CoEdOK;
}

CoEdStatus SiteChangeList::
deSerialize( unsigned char *data, unsigned long len )
{
	XDR		xdrStream;

	_flush();
	if ((data == 0) || (len == 0)) {
		return CoEdOK;
	}
	xdrmem_create( &xdrStream, (caddr_t)data, (unsigned int)len,
		       XDR_DECODE );
	if (! xdrSiteChangeList( &xdrStream, (caddr_t *)this )) {
		return CoEdErrXDR;
	}
	return CoEdOK;
}

bool_t SiteChangeList::
xdr( XDR *xdrStream )
{
	if (! xdr_int( xdrStream, &_count )) {
		return FALSE;
	}
	switch (xdrStream->x_op) {
	    case XDR_ENCODE:
		{
			SiteChange *curr = _head;
			while (curr != 0) {
				if (! curr->xdr( xdrStream )) {
					return FALSE;
				}
				curr = (SiteChange *)curr->_next;
			}
		}
		break;
	    case XDR_DECODE:
		{
			int howMany = _count;
			_count = 0;
			for (int i = 0; i < howMany; i++) {
				SiteChange *newChange = new SiteChange();
				if (! newChange->xdr( xdrStream )) {
					return FALSE;
				}
				append( newChange );
			}
		}
		break;
	}
	return TRUE;
}

SiteChangeList *SiteChangeList::
copy() const
{
	SiteChangeList *newList = new SiteChangeList;
	SiteChange *curr = _head;
	while (curr != 0) {
		newList->append( curr->copy() );
		curr = (SiteChange *)curr->_next;
	}
	return newList;
}

SiteChangeList &SiteChangeList::
copy( const SiteChangeList *list )
{
	_flush();
	SiteChange *curr = list->_head;
	while (curr != 0) {
		append( curr->copy() );
		curr = (SiteChange *)curr->_next;
	}
	return *this;
}

void SiteChangeList::
print( FILE *f ) const
{
	SiteChange *curr = _head;
	fprintf( f, "(" );
	while (curr != 0) {
		curr->print( f );
		if (curr->_next != 0) {
			fprintf( f, ", " );
		}
		curr = (SiteChange *)curr->_next;
	}
	fprintf( f, ")" );
}

void SiteChangeList::
_flush()
{
	SiteChange *curr = _head;
	SiteChange *prev;
	while (curr != 0) {
		prev = curr;
		curr = (SiteChange *)curr->_next;
		delete prev;
	}
}
