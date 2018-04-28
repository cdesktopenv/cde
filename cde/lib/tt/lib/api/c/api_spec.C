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
//%%  $XConsortium: api_spec.C /main/3 1995/10/23 09:54:31 rswiston $ 			 				
/* @(#)api_spec.C	1.15 @(#)
 *
 * api_spec.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include "mp/mp_c.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "api/c/api_spec_map_ref.h"
#include "api/c/api_error.h"
#include "db/tt_db_property.h"
#include "db/tt_db_access.h"
#include "util/tt_audit.h"
#include "util/tt_path.h"

static Tt_status objid_to_spec(const char *objid, _Tt_objid_spec_ptr &specP);

/************************************************************************
* Node Functions (FSpec A.7)						*
************************************************************************/

Tt_status 
tt_spec_bprop(const char *objid, const char *propname, int i,
	      unsigned char **value, int *length)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ori", TT_SPEC_BPROP, objid,
					    propname, i);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_spec_bprop(objid, propname, i, value, length);
        audit.exit(status);

	return status;
}


Tt_status 
tt_spec_bprop_add(const char *objid, const char *propname,
		  const unsigned char *value, int length)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ornI", TT_SPEC_BPROP_ADD, objid,
                                            propname, value, length);
 
        if (status != TT_OK) { 
		audit.exit(status);
                return status; 
        } 
 
        status = _tt_spec_bprop_add(objid, propname, value, length);
        audit.exit(status); 
 
        return status;
}


Tt_status 
tt_spec_bprop_set(const char *objid, const char *propname,
		  const unsigned char *value, int length)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ornI", TT_SPEC_BPROP_SET, objid,
                                            propname, value, length);
 
        if (status != TT_OK) { 
		audit.exit(status);
                return status; 
        } 
 
        status = _tt_spec_bprop_set(objid, propname, value, length);
        audit.exit(status); 
 
        return status;
}


char           *
tt_spec_create(const char *filepath)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_SPEC_CREATE, filepath);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_spec_create(filepath);
        audit.exit(result);

	return result;
}


Tt_status 
tt_spec_destroy(const char *objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_SPEC_DESTROY, objid);
 
        if (status != TT_OK) { 
		audit.exit(status);
                return status; 
        } 
 
        status = _tt_spec_destroy(objid);
        audit.exit(status); 
 
        return status;
}


char           *
tt_spec_file(const char *objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_SPEC_FILE, objid);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_spec_file(objid);
        audit.exit(result);

	return result;
}


char           *
tt_spec_move(const char *objid, const char *newfilepath)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oC", TT_SPEC_MOVE, objid,
					    newfilepath);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_spec_move(objid, newfilepath);
        audit.exit(result);

	return result;
}


Tt_status
tt_spec_type_set(const char * objid, const char * otid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oo", TT_SPEC_TYPE_SET, objid, otid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_spec_type_set(objid, otid);
        audit.exit(status);

        return status;
}


char *
tt_spec_type(const char * objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_SPEC_PROP, objid);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_spec_type(objid);
        audit.exit(result);

        return result;
}


char           *
tt_spec_prop(const char *objid, const char *propname, int i)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ori", TT_SPEC_PROP, objid,
					    propname, i);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_spec_prop(objid, propname, i);
        audit.exit(result);

        return result;
}


Tt_status 
tt_spec_prop_add(const char *objid, const char *propname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ornI", TT_SPEC_PROP_ADD, objid,
					    propname, value,
					    (char *) 0 == value ? 0 : strlen(value));

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_spec_prop_add(objid, propname, value);
        audit.exit(status);

        return status;
}


int 
tt_spec_prop_count(const char *objid, const char *propname)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("or", TT_SPEC_PROP_COUNT, objid,
					    propname);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_spec_prop_count(objid, propname);
        audit.exit(result);

	return result;
}


Tt_status 
tt_spec_prop_set(const char *objid, const char *propname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ornI", TT_SPEC_PROP_SET, objid,
                                            propname, value,
					    (char *) 0 == value ? 0 : strlen(value));

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }
 
        status = _tt_spec_prop_set(objid, propname, value);
        audit.exit(status);
 
        return status;
}


char           *
tt_spec_propname(const char *objid, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oi", TT_SPEC_PROPNAME, objid, n);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_spec_propname(objid, n);
        audit.exit(result);

        return result;
}


int 
tt_spec_propnames_count(const char *objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_SPEC_PROPNAMES_COUNT,
					    objid);
        int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_spec_propnames_count(objid);
        audit.exit(result);

        return result;
}


Tt_status 
tt_spec_write(const char *objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_SPEC_WRITE, objid);

        if (status != TT_OK) {
        audit.exit(status);
                return status;
        }

        status = _tt_spec_write(objid);
        audit.exit(status);

	return status;
}


int 
tt_objid_equal(const char *objid1, const char *objid2)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oo", TT_OBJID_EQUAL, objid1,
					    objid2);
        int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_objid_equal(objid1, objid2);
        audit.exit(result);

        return result;
}


char           *
tt_objid_objkey(const char *objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_OBJID_OBJKEY, objid);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_objid_objkey(objid);
        audit.exit(result);

        return result;
}
// _tt_spec_bprop() - retrive the `i'-th value, starting at 0, of
//		      the property `propname' specified.
Tt_status
_tt_spec_bprop(const char * objid, const char * propname, int i,
	       unsigned char ** value, int * length)
{
	Tt_status result;
	_Tt_string temp;
        _Tt_objid_spec_ptr specP;

	// find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return result;
	}

        _Tt_db_property_ptr propP;

	// Initialize return value
	*value = (unsigned char *)NULL;

	// get list of values for specified property
        propP = specP->getProperty(propname);
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
		case TT_ERR_NOMP:
		case TT_ERR_OBJID:
		case TT_ERR_DBAVAIL:
		case TT_ERR_DBEXIST:
		case TT_ERR_NUM:
		case TT_ERR_PROPNAME:
			break;
		case TT_OK:
		case TT_WRN_STALE_OBJID:
			if ((i < 0) || (i > propP->values->count())) {
				return TT_ERR_NUM;
			}
			if (propP->values->count() == i) {
				return result;
			}

			temp = (*(propP->values))[i];

			*length = temp.len();
			*value = (unsigned char *) _tt_strdup((char *) temp,
								*length);
			return result;
		default:
			return TT_ERR_INTERNAL;
        }
	return result;
}	// end -_tt_spec_bprop()-


Tt_status
_tt_spec_bprop_add(const char * objid, const char * propname,
		   unsigned const char * value, int length)
{
	Tt_status result;
        _Tt_objid_spec_ptr specP;
	_Tt_string temp(value, length);


        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return result;
        }

        _Tt_db_property_ptr newPropP = new _Tt_db_property;


	// load new property
	newPropP->name = propname;
	newPropP->values->append(temp);

	// add it to db
	specP->addProperty(newPropP);
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			specP->setOnDiskFlag(FALSE);
                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_PROPLEN:
                case TT_ERR_PROPNAME:
                        break;
                default:
                        return TT_ERR_INTERNAL;
        }
	return result;
}	// end -_tt_spec_bprop_add()-


Tt_status
_tt_spec_bprop_set(const char * objid, const char * propname,
		   unsigned const char * value, int length)
{
	Tt_status result;
        _Tt_objid_spec_ptr specP;
	_Tt_string temp(value, length);

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return result;
        }

        _Tt_db_property_ptr newPropP = new _Tt_db_property;
 
 
        // load new property
        newPropP->name = propname;
        newPropP->values->append(temp);

        specP->setProperty(newPropP);
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) { 
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			specP->setOnDiskFlag(FALSE);
                case TT_ERR_DBAVAIL: 
                case TT_ERR_DBEXIST: 
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_PROPLEN:
                case TT_ERR_PROPNAME:
                        break;
                default:
                        return TT_ERR_INTERNAL;
        }
	return result;
}	// end -_tt_spec_bprop_set()-


// _tt_spec_create()- create a new spec, and associated tt_db_object,
//		      in memory.
//
char *
_tt_spec_create(const char * filepath)
{
	Tt_status		result; 
	_Tt_string		objid;
	_Tt_db_access_ptr	accessP;
	_Tt_objid_spec_ptr	newSpecP = new _Tt_objid_spec; // create a new spec

	// create new tt_db_object in memory, checking for db error
	objid = newSpecP->create(filepath);
        switch (result = _tt_get_api_error(newSpecP->getDBResults(), _TT_API_SPEC)) { 
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;

                case TT_ERR_DBAVAIL: 
                case TT_ERR_DBEXIST: 
                case TT_ERR_NOMP:
                case TT_ERR_OTYPE:
                case TT_ERR_PATH:
			return (char *)error_pointer(result);
                default:
			return (char *)error_pointer(TT_ERR_INTERNAL);
        }

	// Make the object so that everyone can read and write it
	accessP = new _Tt_db_access;
	accessP->mode = (mode_t)-1;
	newSpecP->setAccess(accessP);
        switch (result = _tt_get_api_error(newSpecP->getDBResults(), _TT_API_SPEC)) {
		case TT_OK:
		case TT_WRN_STALE_OBJID:
 			break;
                case TT_ERR_DBAVAIL: 
                case TT_ERR_DBEXIST:  
                case TT_ERR_NOMP:
                case TT_ERR_OTYPE: 
                case TT_ERR_PATH:
                        return (char *)error_pointer(result);
                default: 
                        return (char *)error_pointer(TT_ERR_INTERNAL);
        } 

	_Tt_api_spec_map_ref specMap;


	// add new spec to map.
	specMap.addSpec(newSpecP);

	return _tt_strdup(objid);
}	// end -_tt_spec_create()-


// _tt_spec_destroy()- destroy a spec, and associated tt_db_object.
//
Tt_status
_tt_spec_destroy(const char * objid)
{

	Tt_status result; 
	_Tt_objid_spec_ptr specP;
	_Tt_api_spec_map_ref specMap;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return result;
        }

	specP->remove();		// remove associated tt_db_object
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
		case TT_OK:
		case TT_WRN_STALE_OBJID:
 			break;
                case TT_ERR_ACCESS:
                case TT_ERR_DBAVAIL: 
                case TT_ERR_DBEXIST:  
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                        return result;
                default: 
                        return TT_ERR_INTERNAL;
        }
	specMap.deleteSpec(objid);// delete spec from table
	return TT_OK;
}	// end -_tt_spec_destroy()-


// _tt_spec_file()- retrieves the name of the file containing the
//		    object described by the spec.
//
char *
_tt_spec_file(const char * objid)
{
	Tt_status result; 
	_Tt_string filepath;
	_Tt_objid_spec_ptr specP;
	_Tt_api_spec_map_ref specMap;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return (char *)error_pointer(result);
        }

	filepath = specP->getFile();
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
		case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;
                case TT_ERR_DBAVAIL: 
                case TT_ERR_DBEXIST:  
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                        return (char *)error_pointer(result);
                default: 
                        return (char *)error_pointer(TT_ERR_INTERNAL);
        }
	return _tt_strdup(_tt_network_path_to_local_path(filepath));
}	// end -_tt_spec_file()-


// _tt_spec_move()- tell tooltalk service ythat the object has moved
//		    to a different file
//
char *
_tt_spec_move(const char * objid, const char * newfilepath)
{
	Tt_status result; 
	_Tt_string newObjid;
	_Tt_objid_spec_ptr specP;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return (char *)error_pointer(result);
        }

	newObjid = specP->move(newfilepath);
	if ((result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) != TT_OK) {
                return (char *)error_pointer(result);
	}
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
                case TT_WRN_SAME_OBJID:
			break;

                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_PATH:
                        return (char *)error_pointer(result);
                default:
                        return (char *)error_pointer(TT_ERR_INTERNAL);
        }
	return _tt_strdup(newObjid);
}	// end -_tt_spec_move()-


Tt_status
_tt_spec_type_set(const char * objid, const char * otid)
{
	Tt_status result;
        _Tt_objid_spec_ptr specP;


	if (TT_WRN_LAST < (result = _tt_valid_otype(otid))) {
		return result;
	}

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return result;
        }

	// set otid on object
        switch (result = _tt_get_api_error(specP->setType(otid), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_READONLY:
			break;
                default:
                        return TT_ERR_INTERNAL;
        }
	return result;
}	// end -_tt_spec_type_set()-


char *
_tt_spec_type(const char * objid)
{
	Tt_status result;
        _Tt_string objType;
        _Tt_objid_spec_ptr specP;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return (char *)error_pointer(result);
        }

	objType = specP->getType();
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;
                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
			return (char *)error_pointer(result);
                default:
                        return (char *)error_pointer(TT_ERR_INTERNAL);
        }
	return _tt_strdup(objType);
}	// end -_tt_spec_type()-


char *
_tt_spec_prop(const char * objid, const char * propname, int i)
{
        int len;
        Tt_status err;
        unsigned char *value;

        err = _tt_spec_bprop(objid, propname, i, &value, &len);
        switch (err) {
		case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;
		default:
			return (char *)error_pointer(err);
        }
	return _tt_strdup((const char *)value);
}	// end -_tt_spec_prop()-


Tt_status
_tt_spec_prop_add(const char * objid, const char * propname, const char * value)
{
	return _tt_spec_bprop_add(objid, propname, (unsigned char *)value,
				  (char *) 0 == value ? 0 : strlen(value));
}	// end -_tt_spec_prop_add()-


int
_tt_spec_prop_count(const char * objid, const char * propname)
{
	Tt_status result;
	_Tt_objid_spec_ptr specP;
	
        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return tt_error_int(result);
        }

        _Tt_db_property_list_ptr propListP;


        propListP = specP->getProperties();
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;
                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_PROPNAME:
                case TT_ERR_PROPLEN:
			return tt_error_int(result);
                default:
			return tt_error_int(TT_ERR_INTERNAL);
        }

	_Tt_db_property_list_cursor properties_cursor(propListP);
	while (properties_cursor.next()) {
		if ((*properties_cursor)->name == propname) {
			if (!(*properties_cursor)->values.is_null()) {
				return (*properties_cursor)->values->count();
			}
			else {
				return 0;
			}
		}
	}

	return 0;
}	// end _tt_spec_prop_count()-


Tt_status
_tt_spec_prop_set(const char * objid, const char * propname, const char * value)
{
	return _tt_spec_bprop_set(objid, propname, (unsigned char *)value,
				  (char *)0==value?0:strlen(value));
}	// end -_tt_spec_prop_set()-


char *
_tt_spec_propname(const char *objid, int n)
{
        Tt_status result;
	_Tt_string propname;
	_Tt_objid_spec_ptr specP;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return (char *)error_pointer(result);
        }

        _Tt_db_property_ptr propP;
        _Tt_db_property_list_ptr propListP;
 

	// get property list
        propListP = specP->getProperties();
        if ((result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) != TT_OK) {
                return (char *)error_pointer(result);
        }

        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;
                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_NUM:
			return (char *)error_pointer(result);
                default: 
			return (char *)error_pointer(TT_ERR_INTERNAL);
        }

	// range check index number
	if ((n < 0) || (n > propListP->count()-1)) {
		return (char *)error_pointer(TT_ERR_NUM);
	}

	// get individual property
	propP = (*propListP)[n];

	// return propname of individual property
	return (_tt_strdup((char *) propP->name));

}	// end -_tt_spec_propname()-


int
_tt_spec_propnames_count(const char * objid)
{
	Tt_status result;
	_Tt_objid_spec_ptr specP;
	_Tt_db_property_list_ptr propListP;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return tt_error_int(result);
        }

	propListP = specP->getProperties();
        switch (result = _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC)) {
                case TT_OK:
		case TT_WRN_STALE_OBJID:
			break;
                case TT_ERR_DBAVAIL:
                case TT_ERR_DBEXIST:
                case TT_ERR_NOMP:
                case TT_ERR_OBJID:
                case TT_ERR_NUM:
			return tt_error_int(result);
                default:
			return tt_error_int(TT_ERR_INTERNAL);
        }
	return propListP->count();
}	// end -_tt_spec_propnames_count()-


Tt_status
_tt_spec_write(const char * objid)
{
	Tt_status result;
	_Tt_db_results db_results = TT_DB_OK;
	_Tt_objid_spec_ptr specP;
	_Tt_api_spec_map_ref specMap;

        // find spec for objid
	if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
                return result;
        }

        // Check the properties for special access privilege properties
	// and generate the approptiate setAccess call
	_Tt_db_property_list_ptr properties = specP->getProperties();
	_Tt_db_property_list_cursor properties_cursor(properties);

	bool_t access_prop_found = FALSE;
	uid_t user = (uid_t)-1;
	gid_t group = (gid_t)-1;
	mode_t mode = (mode_t)-1;	
	_Tt_string temp_string;
	while (properties_cursor.next()) {

		if (properties_cursor->name == TT_OBJECT_OWNER_PROPERTY) {
			access_prop_found = TRUE;

			if (!properties_cursor->is_empty() &&
			    !(*properties_cursor->values)[0].is_null()) {
				temp_string = (*properties_cursor->values)[0];
				user = (uid_t)atoi((char *)temp_string);
			}
		}
		else if (properties_cursor->name == TT_OBJECT_OWNER_PROPERTY) {
			access_prop_found = TRUE;

			if (!properties_cursor->is_empty() &&
			    !(*properties_cursor->values)[0].is_null()) {
				temp_string = (*properties_cursor->values)[0];
				group = (gid_t)atoi((char *)temp_string);
			}
		}
		else if (properties_cursor->name == TT_OBJECT_MODE_PROPERTY) {
			access_prop_found = TRUE;

			if (!properties_cursor->is_empty() &&
			    !(*properties_cursor->values)[0].is_null()) {
				temp_string = (*properties_cursor->values)[0];
				mode = (mode_t)atoi((char *)temp_string);
			}
		}
	}

	if (access_prop_found) {
		_Tt_db_access_ptr access = new _Tt_db_access;
		access->user = user;
		access->group = group;
		access->mode = mode;
		db_results = specP->setAccess(access);
	}

	if (db_results == TT_DB_OK) {
		// write associated tt_db_object to disk
		db_results = specP->write();
	}

        switch (result = _tt_get_api_error(db_results, _TT_API_SPEC)) { 
               	case TT_OK: 
		case TT_WRN_STALE_OBJID:
			specP->setOnDiskFlag(TRUE);
			break;
               	case TT_ERR_DBAVAIL:
               	case TT_ERR_DBEXIST:
               	case TT_ERR_NOMP:
               	case TT_ERR_OBJID:
               	case TT_ERR_OTYPE:
               	case TT_ERR_NUM:
                      	return result;
               	default:
                       	return TT_ERR_INTERNAL;
	}

	return TT_OK;
}	// end -tt_spec_write()-


int
_tt_objid_equal(const char *objid1, const char *objid2)
{
	Tt_status	 err;
	_Tt_objid_spec_ptr spec1, spec2;

	if ((err = objid_to_spec(objid1, spec1)) > TT_WRN_LAST) {
		return tt_error_int(err);
	}
	if ((err = objid_to_spec(objid2, spec2)) > TT_WRN_LAST) {
		return tt_error_int(err);
	}

	return spec1->getObjectKey() == spec2->getObjectKey();
}	// end -tty_objid_equal()-


char *
_tt_objid_objkey(const char *objid)
{
	Tt_status	result;
        _Tt_objid_spec_ptr specP;


        if ((result = objid_to_spec(objid, specP)) > TT_WRN_LAST) {
		return (char *)error_pointer(TT_ERR_OBJID);
        }
	return  _tt_strdup(specP->getObjectKey());
}	// end -tt_objid_key()-


/*
 * Following service functions take care of changing from externally
 * visible objid to internal node structure pointers and back.
 * This isn't really the right place for it, since the external
 * form is not specific to the C api, but it's not clear where
 * the code should really be; it doesn't really belong to 
 * _Tt_node either.
 */

Tt_status
objid_to_spec(const char * objid, _Tt_objid_spec_ptr &specP)
{
        _Tt_api_spec_map_ref specMap;


        // find spec for objid
        specP = specMap.getSpec(objid);
        if (specP.is_null()) {
                return TT_ERR_OBJID;
        }
	
	return _tt_get_api_error(specP->getDBResults(), _TT_API_SPEC);
}	// end -objid_to_spec()-
