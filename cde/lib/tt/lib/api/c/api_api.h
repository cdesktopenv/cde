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
/*%%  $XConsortium: api_api.h /main/3 1995/10/23 09:51:46 rswiston $ 			 				 */
/*-*-C++-*-
 *
 * api_api.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declarations for "pure api" functions.
 */
#if !defined(_TT_API_API_H)
#define _TT_API_API_H
#include "api/c/tt_c.h"
#include "util/tt_string.h"
extern char _tt_api_status_page[(int)TT_STATUS_LAST];

int		 _tt_mark(void);
void		 _tt_release(int mark);
caddr_t		 _tt_malloc(size_t s);
char		*_tt_strdup(const _Tt_string &);
char		*_tt_strdup(const char * s);
char		*_tt_strdup(const char * s, int len);
void		 _tt_free(caddr_t p);
caddr_t		 _tt_take(caddr_t p);
char		*_tt_status_message(Tt_status ttrc);
Tt_status	 _tt_errno_status(int err_no);
Tt_status	 _tt_pointer_error(void *pointer);
Tt_status	 _tt_int_error(int n);
void		*_tt_error_pointer(Tt_status s);
int		 _tt_error_int(Tt_status s);
void		 _prepend_P_to_sessid(const char *sessid,
					_Tt_string &sessid_with_P);
#define error_pointer(ttrc) ((void *)(_tt_api_status_page+(int)(ttrc)))
#define error_int(ttrc) (-(int)(ttrc))

//
// Macro to commit the default_procid to its default session. Should
// be called before any operation that requires communicating with the
// default session.
//

// cpp complains about using (char *) as an argument to a macro
typedef char *char_ptr;

#define PCOMMIT \
if (d_procid->commit() != TT_OK) \
   return(TT_ERR_SESSION);

#define PTR_PCOMMIT(tcast) \
if (d_procid->commit() != TT_OK) \
   return((tcast)error_pointer(TT_ERR_SESSION));

#endif
