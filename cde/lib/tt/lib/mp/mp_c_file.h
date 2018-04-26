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
/*%%  $XConsortium: mp_c_file.h /main/3 1995/10/23 10:19:40 rswiston $ 			 				 */
/*
 * Tool Talk Message Passer (MP) - mp_c_file.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declaration of the _Tt_c_file class.
 */

#ifndef _MP_C_FILE_H
#define _MP_C_FILE_H
#include <mp/mp_file.h>
#include <mp/mp_c_file_utils.h>

class _Tt_c_file : public _Tt_file {
      public:
	_Tt_c_file();
	_Tt_c_file(
		_Tt_string path
	);
	virtual ~_Tt_c_file();

	Tt_status		c_join(
					_Tt_procid_ptr &p
				);
	Tt_status		c_quit(
					_Tt_procid_ptr &p
				);
	Tt_status		process_message_queue(
					int dispatch = 1
				);
};

#endif				/* _MP_C_FILE_H */
