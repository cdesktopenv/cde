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
/*%%  $XConsortium: CoEdSiteID.h /main/3 1995/10/20 17:07:21 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * CoEdSiteID.h
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

#ifndef CoEdSiteID_h
#define CoEdSiteID_h

#include <rpc/types.h>
#include <rpc/xdr.h>

class CoEdSiteID {
	friend class CoEdSiteIDList;
	friend class SiteChangeList;
	friend class CoEdTextVersion;

    public:
	CoEdSiteID();
	CoEdSiteID( const char *procID );
	CoEdSiteID( const CoEdSiteID &id );
	~CoEdSiteID();

	bool_t		xdr( XDR *xdrStream );
	CoEdSiteID     *copy()						const;
	CoEdSiteID     &copy( const CoEdSiteID *id2Copy );
	int		cmp( const CoEdSiteID &id )			const;
	int		operator==( const CoEdSiteID &id )		const
				{ return (cmp( id ) == 0); }
	int		operator<( const CoEdSiteID &id )		const
				{ return (cmp( id ) < 0); }
	void		print( FILE *f )				const;

    private:
	char	       *_procID;
	CoEdSiteID     *_next;
	CoEdSiteID     *_prev;
};

class CoEdSiteIDList {

    public:
	CoEdSiteIDList();
	~CoEdSiteIDList();

	void		push( CoEdSiteID *change );
	void		append( CoEdSiteID *change );
	void		insertBefore( CoEdSiteID *change1,
				      CoEdSiteID *change2 );
	void		insertAfter( CoEdSiteID *change2,
				     CoEdSiteID *change1 );
	int		isEmpty()					const
				{ return (_count == 0); }

    private:
	CoEdSiteID     *_head;
	CoEdSiteID     *_tail;
	int		_count;

	void		_flush();
};

#endif CoEdSiteID_h
