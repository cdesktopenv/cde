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
/*%%  $XConsortium: api_spec_map_ref.h /main/3 1995/10/23 09:54:47 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * api_spec_map_ref.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * define a class which maps objids to specs (tt_db_objects)
 */

#ifndef _API_SPEC_MAP_REF_H
#define _API_SPEC_MAP_REF_H

#include "api/c/api_objid_spec.h"

class _Tt_api_spec_map_ref : public _Tt_allocated {
	public:
		_Tt_api_spec_map_ref();
		~_Tt_api_spec_map_ref();

		// Add the supplied spec to the tt_db_object table.
		Tt_status addSpec(_Tt_objid_spec_ptr specP);

		// Delete the associated spec from the tt_db_object
		// table.
		void deleteSpec(_Tt_string objid);

		// Return the spec associated w/ supplied objid
		_Tt_objid_spec_ptr getSpec(_Tt_string objid);

		// de-allocate the table associated with apiSpecMapP
		// below, when tt_close() is called.
		static void flush();

                // hash function
                static _Tt_string apiSpecKey(_Tt_object_ptr &Obj);
	private:
		static _Tt_objid_spec_table_ptr *apiSpecMapP;

};

#endif // _API_SPEC_MAP_REF_H
