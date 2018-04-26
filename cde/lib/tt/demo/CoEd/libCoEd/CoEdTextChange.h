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
/*%%  $XConsortium: CoEdTextChange.h /main/3 1995/10/23 09:43:46 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * CoEdTextChange.h
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

#ifndef CoEdTextChange_h
#define CoEdTextChange_h

#define InLibCoEd

#include "CoEd.h"
#include "CoEdTextVersion.h"

class CoEdTextChange {
	friend class CoEdTextChangeList;
	friend class CoEdChangeHistory;
	friend class CoEdChangeQueue;

    public:
	CoEdTextChange();
	CoEdTextChange( long start, long end, const char *text,
		        const CoEdTextVersion *appliesTo,
		        const CoEdSiteID *causer, int changeNum );
	CoEdTextChange( const CoEdTextChange &change );
	CoEdTextChange( Tt_message msg, CoEdStatus &status );
	~CoEdTextChange();

	CoEdStatus	broadcast( const char *path )			const;
	int		knowsOfNewerChangesThan(
				const CoEdTextVersion &version )	const
				{ return _appliesTo->knowsOfNewerChangesThan(
							version ); }
	int		knowsOfNewerChangesThan(
				const CoEdTextChange &change )		const
				{ return _appliesTo->knowsOfNewerChangesThan(
							*change._appliesTo ); }
	int		knowsOf( const CoEdTextChange &change )		const
				{ return _appliesTo->knowsOf( *change._causer,
						        change._changeNum ); }
	void		interTranslate( CoEdTextChange &chng2Xlate );
	void		translateOver( const CoEdTextChange &chng2Hurdle );
	long		adjustPt( long pt, const CoEdSiteID &ptOwner,
				  int directionToLean )			const;
	void		print( FILE *f );
	long		charsAddedBy()	const { return _charsAdded; }
	long		start()		const { return _start; }
	long		end()		const { return _end; }
	const char     *text()		const { return _text; }
	const CoEdSiteID &causer()	const { return *_causer; }
	int		changeNum()	const { return _changeNum; }

    private:
	long			_start;
	long			_end;
	const char	       *_text;
	CoEdTextVersion	       *_appliesTo;
	CoEdSiteID	       *_causer;
	int			_changeNum;
	long			_charsAdded;
	CoEdTextChange	       *_next;
	CoEdTextChange	       *_prev;
};

class CoEdTextChangeList {
	friend class CoEdChangeHistory;
	friend class CoEdChangeQueue;

    public:
	CoEdTextChangeList();
	~CoEdTextChangeList();

	void		push( CoEdTextChange *change );
	void		append( CoEdTextChange *change );
	void		insertBefore( CoEdTextChange *change1,
				      CoEdTextChange *change2 );
	void		insertAfter( CoEdTextChange *change2,
				     CoEdTextChange *change1 );
	CoEdTextChange *remove( CoEdTextChange *change );
	void		print( FILE *f, char *indent = "" );

    private:
	CoEdTextChange *_head;
	CoEdTextChange *_tail;
	int		_count;
};

#endif CoEdTextChange_h
