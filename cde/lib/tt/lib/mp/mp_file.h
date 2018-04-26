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
/*%%  $XConsortium: mp_file.h /main/3 1995/10/23 10:23:40 rswiston $ 			 				 */
/*
 * Tool Talk Message Passer (MP) - mp_file.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#ifndef _MP_FILE_H
#define _MP_FILE_H

#include <mp/mp_global.h>
#include <mp/mp_file_utils.h>
#include <mp/mp_procid_utils.h>
#include <mp/mp_message_utils.h>
#include <mp/mp_session_utils.h>
#include <db/tt_db_file.h>
#include <util/tt_int_rec.h>

#define TT_QMSG_INFO_VERSION	1
#define TT_FSCOPE_INFO_VERSION	1

typedef Tt_filter_action	(*_Tt_file_callback)(
					Tt_filter_function	fn,
					_Tt_string		nodeid,
					void		       *context,
					void		       *accumulator
				);

class _Tt_qmsg_info : public _Tt_object {
      public:
	_Tt_qmsg_info();
	virtual ~_Tt_qmsg_info();
	int			id;
	int			nparts;
	int			size;
	_Tt_int_rec_list_ptr	categories;
	_Tt_string_list_ptr	ptypes;
	bool_t			xdr(XDR *xdrs);
	int			version;
	int			m_id;		// message id
	_Tt_string		sender;		// message sender procid
};

class _Tt_file : public _Tt_db_file {
      public:
	_Tt_file();
	_Tt_file( const _Tt_string &path );
	virtual ~_Tt_file();

	Tt_status		query(
					_Tt_file_callback   fn,
					Tt_filter_function  filter,
					void		   *context,
					void		   *accumulator
				);
	static _Tt_string	networkPath_(
					_Tt_object_ptr	   &obj
				);
};

#endif				/* _MP_FILE_H */
