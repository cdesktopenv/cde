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
/*%%  $XConsortium: tt_gettext.h /main/3 1995/10/23 10:39:05 rswiston $ 			 				 */
/*
 *
 * tt_gettext.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#if !defined(_TT_GETTEXT_H)
#define _TT_GETTEXT_H

//
// This is a binary runtime interface to the sets in our message catalog.
// These symbols are not used in the source, however, because the
// message extraction tools do not understand enums (or cpp macros).
//
typedef enum {
	TTMSET_TT_STATUS	= 0,
	TTMSET_LIBTT		= 1,
	TTMSET_SLIB		= 2,
	TTMSET_TTSESSION	= 3,
	TTMSET_TT_TYPE_COMP	= 4,
	TTMSET_DBSERVER		= 5,
	TTMSET_DBCK		= 6,
	TTMSET_TTTAR		= 7,
	TTMSET_FUTIL		= 8,
	TTMSET_TTTRACE		= 9,
	TTMSET_TTSNOOP		= 10
} _Tt_msg_set;

char     *_tt_catgets(
			int		set_num,
			int		msg_num,
			const char     *default_string
		);

//
// The following interface is used so that our code can hold calls
// to catgets(), which is the only interface that SunSoft's message
// extraction tools understand.
//
const int _ttcatd = 0;

inline char *
catgets(
	int		,		// pass _ttcatd in here
	int		set_num,
	int		msg_num,
	const char     *default_string
)
{
	return _tt_catgets( set_num, msg_num, default_string );
}

#endif
