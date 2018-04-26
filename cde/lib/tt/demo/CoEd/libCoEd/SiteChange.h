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
/*%%  $XConsortium: SiteChange.h /main/3 1995/10/23 09:44:55 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * SiteChange.h - A SiteChange is a record that associates a SiteID
 *	with an integer indicating how many changes from that site
 *	have been seen so far.
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

#ifndef SiteChange_h
#define SiteChange_h

#include <rpc/types.h>
#include <rpc/xdr.h>

#define InLibCoEd

#include "CoEd.h"
#include "CoEdSiteID.h"

class SiteChange : public CoEdSiteID {
	friend class SiteChangeList;
	friend class CoEdTextVersion;

    public:
	SiteChange();
	SiteChange( const char *procID );
	SiteChange( const CoEdSiteID &id, int numChanges );

	bool_t		xdr( XDR *xdrStream );
	SiteChange     *copy()						const;
	int		cmp( const SiteChange &change )			const;
	int		operator==( const SiteChange &change )		const
				{ return (cmp( change ) == 0); }
	int		operator<( const SiteChange &change ) 		const
				{ return (cmp( change ) < 0); }
	void		print( FILE *f )				const;

    private:
	int		_changeNum;
};

class SiteChangeList {
	friend class CoEdTextVersion;

    public:
	SiteChangeList();
	SiteChangeList( unsigned char *data, int len, CoEdStatus &status );
	~SiteChangeList();

	void		push( SiteChange *change );
	void		append( SiteChange *change );
	void		insertBefore( SiteChange *change1,
				      SiteChange *change2 );
	void		insertAfter( SiteChange *change2,
				     SiteChange *change1 );
	CoEdStatus	serialize( unsigned char **pData,
				   unsigned long *pLen )	const;
	CoEdStatus	deSerialize( unsigned char *data,
				     unsigned long len );
	bool_t		xdr( XDR *xdrStream );
	SiteChangeList *copy()					const;
	SiteChangeList &copy( const SiteChangeList *list );
	void		print( FILE *f )			const;

    private:
	SiteChange     *_head;
	SiteChange     *_tail;
	int		_count;

	void		_flush();
};

#endif SiteChange_h
