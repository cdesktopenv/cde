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
//%%  $XConsortium: tttar_utils.C /main/3 1995/10/20 17:01:44 rswiston $ 			 				
/*
 * tttar_utils.cc - Utilities for the Link Service archive tool.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <api/c/tt_c.h>
#include <util/tt_enumname.h>
#include "tttar_utils.h"

/*
 * External Variables
 */
extern int verbosity;

/*
 * Global Variables
 */
Tt_status 	err_noted;		// Used by note_err() macro.
char	       *ptr_returned;		// Used by note_ptr_err() macro.
void	       *voidptr_returned;	// Used by note_voidptr_err() macro.
Tt_message 	msg_returned;		// Used by note_msg_err() macro.
int	 	int_returned;		// Used by note_int_err() macro.

#define make_printable_on_1_line( cp )				\
	while (*cp != '\0') {					\
		if (    (*cp == '\n')				\
		     || (*cp == '\r')				\
		     || (! isprint((unsigned char)*cp)))	\
		{						\
			*cp = '_';				\
		}						\
		cp++;						\
	}

/*
 * note_error() - Note a failed ToolTalk call on stderr.
 */
Tt_status
note_error( Tt_status error, char *expression, char *file, int line ) {
	if (! IS_TT_ERR(error)) {
		return error;
	}
	if (verbosity >= 1) {
		(void) fprintf( stderr,
				"%s: %s:%d:%s => %s %s\n",
				our_process_name, file, line, expression,
				_tt_enumname((Tt_status)error),
				tt_status_message(error)
			      );
	} else {
		(void) fprintf( stderr, "%s: %s\n", our_process_name,
			        tt_status_message(error) );
	}
	return error;
}

/*
 * my_tt_free() - Wrap tt_free().
 */
void
my_tt_free( caddr_t p ) {
	tt_free(p);
}

/*
 * my_tt_release() - Wrap tt_release().
 */
void
my_tt_release( int mark ) {
	tt_release(mark);
}

/*
 * in_list() - Is this string in this list?
 */
bool_t
in_list(  _Tt_string s, _Tt_string_list_ptr list )
{
	_Tt_string_list_cursor sc( list );
	while (sc.next()) {
		if ((*sc) == s) {
			return TRUE;
		}
	}
	return FALSE;
}
