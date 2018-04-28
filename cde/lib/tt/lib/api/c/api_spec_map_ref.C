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
//%%  $TOG: api_spec_map_ref.C /main/4 1999/10/14 18:39:43 mgreess $ 			 				
/* -*-C++-*-
 *
 * api_spec_map_ref.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * implement a class which maps specs to tt_db_objects
 */

#include "api/c/api_error.h"
#include "api/c/api_spec_map_ref.h"
#include "db/tt_db_objid_to_key.h"

_Tt_objid_spec_table_ptr  *_Tt_api_spec_map_ref :: apiSpecMapP = (_Tt_objid_spec_table_ptr *) 0;

_Tt_api_spec_map_ref ::
_Tt_api_spec_map_ref()
{
	if (apiSpecMapP == NULL) {
		apiSpecMapP = new _Tt_objid_spec_table_ptr;
		*apiSpecMapP =
			new _Tt_objid_spec_table ((_Tt_object_table_keyfn)
					&_Tt_api_spec_map_ref::apiSpecKey);
	}
}	// end -_Tt_api_spec_map_ref()-


_Tt_api_spec_map_ref ::
~_Tt_api_spec_map_ref()
{
}	// end -~_Tt_db_spec_map_ref()-


// Call this at tt_close() only.
//
void _Tt_api_spec_map_ref ::
flush()
{
        if (apiSpecMapP != NULL) {
                delete apiSpecMapP;
                apiSpecMapP = NULL;
        }
}	// end flush()-


// addSpec is done after a C++ `new' has been done to create a spec,
// and after that spec has been used to initialize the associated
// _Tt_db_object via the _Tt_db_object.create() call.
//
Tt_status  _Tt_api_spec_map_ref ::
addSpec(_Tt_objid_spec_ptr specP)
{
	_Tt_objid_spec_ptr foundSpecP;

	foundSpecP = (*apiSpecMapP)->lookup(specP->getObjectKey());
	if (!foundSpecP.is_null()) {
		return TT_ERR_INVALID;
	}

	(*apiSpecMapP)->insert(specP);	// add spec to table
	specP->setOnDiskFlag(FALSE);

	return TT_OK;
}	// end -addSpec()-


// Delete a spec from the spec map
//
void _Tt_api_spec_map_ref ::
deleteSpec(_Tt_string objid)
{
	_Tt_objid_spec_ptr foundSpecP;
	_Tt_string objkey = _tt_db_objid_to_key(objid);

	// check memory
	foundSpecP = (*apiSpecMapP)->lookup(objkey);
	if (foundSpecP.is_null()) {
		return;
	}

	// remove spec from table
	(*apiSpecMapP)->remove(objkey);

}	// end -deleteSpec()-


// Check for memory version first, returning it if found.
// Else, look for it on disk.  If it's found on disk add
// it to memory copy, and return it.  If it's not on disk,
// return null and set apiError to reflect Tt_db_result.
//
_Tt_objid_spec_ptr _Tt_api_spec_map_ref ::
getSpec(_Tt_string objid)
{
	_Tt_string objkey = _tt_db_objid_to_key(objid);
        _Tt_objid_spec_ptr foundSpecP = (*apiSpecMapP)->lookup(objkey);

        if (!foundSpecP.is_null()) {
                if (foundSpecP->getOnDiskFlag()) {
                        foundSpecP->refresh();
                }
	} else {
		// search tt_db on disk for spec
		_Tt_objid_spec_ptr foundSpecP = new _Tt_objid_spec(objid);
		_Tt_db_results status;

		status = foundSpecP->getDBResults();
		if (_tt_get_api_error(status, _TT_API_SPEC) > TT_WRN_LAST) {
			return (_Tt_objid_spec_ptr) 0;
		}

		// found it
		addSpec(foundSpecP);		// add spec to spec table
		foundSpecP->setOnDiskFlag(TRUE);
	}
        return foundSpecP;
}	// end -getSpec()-

_Tt_string _Tt_api_spec_map_ref ::
apiSpecKey (_Tt_object_ptr &obj)
{
	return (((_Tt_objid_spec *)obj.c_pointer())->getObjectKey());
}	// end -apiSpecKey()-
