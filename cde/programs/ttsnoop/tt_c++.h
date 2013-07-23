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
//%%  $XConsortium: tt_c++.h /main/3 1995/11/06 17:14:37 rswiston $

#ifndef TT_CXX_H
#define TT_CXX_H

#if defined(linux) || defined(CSRG_BASED)
#include <iostream>
using namespace std;
#else
#include <iostream.h>
#endif

#include <Tt/tt_c.h>

ostream &	operator<<( ostream &, Tt_status );
ostream &	operator<<( ostream &, Tt_scope );
ostream &	operator<<( ostream &, Tt_class );
ostream &	operator<<( ostream &, Tt_mode );
ostream &	operator<<( ostream &, Tt_address );
ostream &	operator<<( ostream &, Tt_callback_action );
char *		tt_status_string( Tt_status );
Tt_message	tt_message_copy( Tt_message );
Tt_message	tttk_message_create( const char * );
int		tt_procid_fd( const char * );

#endif
