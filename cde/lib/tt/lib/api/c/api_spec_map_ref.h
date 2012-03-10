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
