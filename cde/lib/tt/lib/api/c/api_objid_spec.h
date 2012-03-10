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
