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
