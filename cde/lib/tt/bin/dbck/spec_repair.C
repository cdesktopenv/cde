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
//%%  $XConsortium: spec_repair.C /main/3 1995/10/20 16:26:51 rswiston $ 			 				
/*
 *
 * spec_repair.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#if defined(_AIX)
#include <time.h>
#endif
#include <sys/stat.h>
#include "spec.h"
#include "options_tt.h"
#include "ttdbck.h"
#include "db/tt_db_object.h"
#include "db/tt_db_object_utils.h"
#include "db/tt_db_key.h"
#include "db/tt_db_object.h"
#include "db/tt_db_results.h"
#include "db/tt_db_access.h"
#include "tt_db_server_consts.h"     
     
#define MKERR(msg, err) fprintf(stderr, \
				"ttdbck: error %s spec: %s\n", \
				msg, \
				tt_status_message(err))

const char* MP_TYPE_PROP = "_NODE_TYPE";

     void Spec::
     repair_spec()
{
	
	// by this point all the information about the spec is collected
	// in the Spec instance.  We make any requested changes to
	// the Spec instance, then completely replace the
	// info on disk with the info from the Spec instance.
	
	if (opts->repair_type_p()) {
		type = opts->repair_type();
	}
	
	if (opts->repair_filename_p()) {
		filename = opts->repair_filename();
	}

	_Tt_db_object_ptr oldobj = new _Tt_db_object();
	switch(oldobj->getDBResults()) {
	    case TT_DB_OK:
		break;
	    default:
		MKERR("creating", TT_ERR_INTERNAL);
		return;
	}

	_Tt_string objid = oldobj->create(filename, key->key()->string());
	switch(oldobj->getDBResults()) {
	    case TT_DB_OK:
		break;
	    default:
		MKERR("creating", TT_ERR_INTERNAL);
		return;
	}

	_Tt_db_access_ptr access = oldobj->getAccess();
	switch(oldobj->getDBResults()) {
	    case TT_DB_OK:
		break;
	    default:
		MKERR("creating", TT_ERR_INTERNAL);
		return;
	}
	
	switch(oldobj->remove()) {
	    case TT_DB_OK:
	    case TT_DB_ERR_NO_SUCH_OBJECT:
	    case TT_DB_ERR_NO_SUCH_PROPERTY:
	    case TT_DB_WRN_FORWARD_POINTER:
		break;
	    case TT_DB_ERR_ACCESS_DENIED:
		MKERR("destroying", TT_ERR_ACCESS);
		return;
	    default:
		MKERR("destroying", TT_ERR_DBAVAIL);
		return;
	}

	if (opts->repair_delete_p()) {
		// don't recreate the spec.
	} else {

		// Should ensure we are running under a particular namespace
		// for this?  Like an override namespace to force into the
		// db under repair?
		
		_Tt_db_object_ptr newobj = new _Tt_db_object();
		switch(newobj->getDBResults()) {
		    case TT_DB_OK:
			break;
		    default:
			MKERR("creating", TT_ERR_INTERNAL);
			return;
		}

		_Tt_string newobjid = newobj->create(filename,
						     key->key()->string());
		switch(newobj->getDBResults()) {
		    case TT_DB_OK:
			break;
		    default:
			MKERR("creating", TT_ERR_INTERNAL);
			return;
		}

		newobj->setType(type);
		newobj->setAccess(access);

		// Re-create the properties
		
		_Tt_string_list_cursor c;
		Prop_ptr sp;
		_Tt_string_list_cursor v;
		_Tt_db_property_ptr dbprop = new _Tt_db_property();
		uid_t euid;
		gid_t group;
		mode_t mode;
		int owner_written = 0;
		int group_written = 0;
		int mode_written = 0;

		c.reset(propnames);
		while(c.next()) {
			sp = props->lookup(*c);
			v.reset(sp->_values);
			if (!v.next()) {
				// No values for the property.  This is
				// theoretically impossible -- delete the
				// property.
			} else if (sp->_name == TT_DB_OBJECT_TYPE_PROPERTY) {
				// this is the special prop that holds the
				// type name. Don't try to set it by that name.
			} else {

				if (sp->_name ==
				    TT_OBJECT_OWNER_PROPERTY) {
				
					// This is the special user field
					// Note that the access info is
					// already written out above.
					// Re-write it here anyway, in
					// case it was corrupt in the
					// old DB.

					memcpy((char *)&euid,
					       (char *)(*v), sizeof(uid_t));
					owner_written = 1;
				}
				else if (sp->_name ==
					 TT_OBJECT_GROUP_PROPERTY) {
				
					// This is the special group field
					// See note for owner field above

					memcpy((char *)&group,
					       (char *)(*v), sizeof(gid_t));
					group_written = 1;
				}
				else if (sp->_name ==
					 TT_OBJECT_MODE_PROPERTY) {
				
					// This is the special mode field
					// See note for owner field above

					memcpy((char *)&mode,
					       (char *)(*v), sizeof(mode_t));
					mode_written = 1;
				}
				else {

					// Ordinary property -- create
					// a new _Tt_db_property record

					dbprop->name = sp->_name;
					dbprop->values->push(*v);

					while(v.next()) {

						// Add any remaining values

						dbprop->values->push(*v);
					}

				}
			}

			newobj->addProperty(dbprop);
		}

		if (owner_written && group_written && mode_written) {

			// We have complete access info from the old prop
			// list, so we may as well use it.

			access->user = euid;
			access->group = group;
			access->mode = mode;
			newobj->setAccess(access);
		}

		// There\'s not much we can do if this call doesn\'t work
		
		newobj->write();
	}
}
