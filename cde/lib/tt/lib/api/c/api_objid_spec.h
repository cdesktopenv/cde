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
/*%%  $XConsortium: api_objid_spec.h /main/3 1995/10/23 09:54:02 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * api_objid_spec.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 */

#ifndef _API_OBJID_SPEC_H  
#define _API_OBJID_SPEC_H

#include "db/tt_db_object_utils.h"

class _Tt_objid_spec : public _Tt_db_object {
	public:
		_Tt_objid_spec();
		_Tt_objid_spec(_Tt_string objid);
		~_Tt_objid_spec();

		void setOnDiskFlag(bool_t flag);
		bool_t getOnDiskFlag();

	private:
		bool_t onDiskFlag; // memory refresh flag
};

declare_derived_ptr_to(_Tt_objid_spec, _Tt_db_object)
declare_table_of(_Tt_objid_spec)

#endif // _API_OBJID_SPEC_H
