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
/*%%  $XConsortium: tttar_utils.h /main/3 1995/10/20 17:01:53 rswiston $ 			 				 */
/*
 * tttar_utils.h - Utilities for the ToolTalk archive tool.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef	_LSTAR_UTILS_H
#define	_LSTAR_UTILS_H

#include <api/c/tt_c.h>
#include <util/tt_string.h>


/*
 * External Variables
 */

extern Tt_status	err_noted;		// Used by note_err() macro.
extern char	       *ptr_returned;		// Used by note_ptr_err() macro.
extern void	       *voidptr_returned;	// Used by note_voidptr_err() macro.
extern Tt_message	msg_returned;		// Used by note_msg_err() macro.
extern int		int_returned;		// Used by note_int_err() macro.
extern char		our_process_name[];

/*
 * Constants
 */

/*
 * STRING_EQUAL() - Check 2 strings for equality.
 */
#define STRING_EQUAL(s1,s2) ( strcmp( s1, s2 ) == 0 )

/*
 * NULL_OR_EMPTY() - Is this string ptr NULL or empty?
 */
#define NULL_OR_EMPTY(p) (( (char *)p == NULL ) || ( *(char *)p == '\0' ))

/*
 * IS_TT_ERR() - Is this Tt_status neither TT_OK nor a TT_WARN?
 */
#define IS_TT_ERR(err)	( err > TT_WRN_LAST )

/*
 * stringify() - Turn a macro argument into a string.
 *	gross Reiserism, use # in ANSI C
 */
#if defined(__STDC__)
#define stringify(s) #s
#else
/* gross Reiserism, use # in ANSI C */
#define stringify(s) "s"
#endif

/*
 * note_err() - Note any error corresponding to this Tt_status
 *	expression, returning the error code
 */
#define note_err(expression)					\
	(   ( TT_OK == ( err_noted = expression))		\
	  ? TT_OK						\
	  : note_error( err_noted, stringify(expression),	\
			__FILE__, __LINE__ ))


/*
 * note_ptr_err() - Assign the (char *) expression to ptr_returned,
 *	noting any error on stderr if error isn't TT_OK, and
 *	return the error code
 */
#define note_ptr_err(expression)					\
	(   (    TT_OK							\
	      == ( err_noted =		 				\
		   tt_pointer_error(ptr_returned = expression)))	\
	  ? TT_OK							\
	  : note_error( err_noted, stringify(expression),		\
			__FILE__, __LINE__ ))

/*
 * note_int_err() - Assign the (int) expression to int_returned,
 *	noting any error on stderr if error isn't TT_OK, and
 *	return the error code
 */
#define note_int_err(expression)					\
	(   (    TT_OK							\
	      == ( err_noted =		 				\
		   tt_int_error(int_returned = expression)))		\
	  ? TT_OK							\
	  : note_error( err_noted, stringify(expression),		\
			__FILE__, __LINE__ ))

/*
 * note_msg_err() - Assign the expression to msg_returned,
 *	noting any error on stderr if error isn't TT_OK, and
 *	return the error code
 */
#define note_msg_err(expression)					\
	(   (    TT_OK							\
	      == ( err_noted =		 				\
		   tt_pointer_error(msg_returned = expression)))	\
	  ? TT_OK							\
	  : note_error( err_noted, stringify(expression),		\
			__FILE__, __LINE__ ))

/*
 * note_voidptr_err() - Assign the expression to voidptr_returned,
 *	noting any error on stderr if error isn't TT_OK, and
 *	return the error code
 */
#define note_voidptr_err(expression)					\
	(   (    TT_OK							\
	      == ( err_noted =		 				\
		   tt_pointer_error(voidptr_returned = expression)))	\
	  ? TT_OK							\
	  : note_error( err_noted, stringify(expression),		\
			__FILE__, __LINE__ ))

/*
 * Procedure declarations
 */

Tt_status	note_error( Tt_status error, char *expression, 
			    char *file, int line );
void		my_tt_free( caddr_t p );
void		my_tt_release( int mark );
bool_t		in_list( _Tt_string s, _Tt_string_list_ptr list );

#endif /* _LSTAR_UTILS_H */
