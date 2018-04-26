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
//%%  $XConsortium: api_session_prop.C /main/4 1995/11/28 19:22:54 cde-sun $ 			 				
/*
 *
 * api_session_prop.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "mp/mp_c.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "util/tt_audit.h"


char *
tt_session_prop(const char *sessid, const char *propname, int i)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Cri", TT_SESSION_PROP, sessid,
					propname, i);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_session_prop(sessid, propname, i);
        audit.exit(result);

	return result;
}


Tt_status
tt_session_prop_add(const char *sessid, const char *propname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CrnI", TT_SESSION_PROP_ADD, sessid,
					propname, value,
					(char *) 0 == value ? 0 : strlen(value));

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_prop_add(sessid, propname, value);
        audit.exit(status);

	return status;
}


int
tt_session_prop_count(const char *sessid, const char *propname)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Cr", TT_SESSION_PROP_COUNT,
					sessid, propname);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_session_prop_count(sessid, propname);
        audit.exit(result);

	return result;
}


Tt_status
tt_session_prop_set(const char *sessid, const char *propname,
		     const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CrnI", TT_SESSION_PROP_SET, sessid,
					   propname, value,
					   (char *) 0 == value ? 0 : strlen(value));

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_prop_set(sessid, propname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_session_bprop(const char *sessid, const char *propname, int i,
		 unsigned char **value, int *length)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Cri", TT_SESSION_BPROP, sessid,
					   propname, i);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_bprop(sessid, propname, i, value, length);
        audit.exit(status);

	return status;
}


Tt_status
tt_session_bprop_add(const char *sessid, const char *propname,
		     const unsigned char *value, int length)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CrnI", TT_SESSION_BPROP_ADD, sessid,
					   propname, value, length);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_bprop_add(sessid, propname, value, length);
        audit.exit(status);

	return status;
}


Tt_status
tt_session_bprop_set(const char *sessid, const char *propname,
		     const unsigned char *value, int length)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("CrnI", TT_SESSION_BPROP_ADD, sessid,
					   propname, value, length);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_session_bprop_set(sessid, propname, value, length);
        audit.exit(status);

	return status;
}


char *
tt_session_propname(const char *sessid, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Ci", TT_SESSION_PROPNAME, sessid, n);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_session_propname(sessid, n);
        audit.exit(result);

	return result;
}


int
tt_session_propnames_count(const char *sessid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_SESSION_PROPNAMES_COUNT,
					    sessid);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_session_propnames_count(sessid);
        audit.exit(result);

	return result;
}


/*
 * return the character string value of the specified property on the session
 */
char *
_tt_session_prop(const char *sessid, const char *propname, int i)
{
	unsigned char *v;
	int junk;
	Tt_status rc;

	rc = _tt_session_bprop(sessid,propname,i, &v, &junk);
	if (rc==TT_OK) {
		return (char *)v;
	} else {
		return (char *)error_pointer(rc);
	}
}

/* 
 * Return the ith value of the specified property on the session
 */
Tt_status
_tt_session_bprop(const char *sessid, const char *propname,
		  int i, unsigned char **value,  int *length)
{
	_Tt_string	val;
	Tt_status	err;
	_Tt_c_session	*d_session;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();


	PCOMMIT;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	if (!strcmp(sessid,(char *)(d_session->id()))) {
		err = d_session->c_getprop(propname, i, val);
		if (err != TT_OK) {
			return err;
		}
		if (val.len() == 0) {
			*value = (unsigned char *)tt_malloc(1);
			**value = '\0';
			*length = 0;
		} else {
			*length = val.len();
			*value = (unsigned char *)_tt_strdup(val, *length);
		}
		return TT_OK;
	} else {
		return TT_ERR_POINTER;
	}
}

/*
 * Add the character string value to the property for the session
 */
Tt_status
_tt_session_prop_add(const char *sessid, const char *propname,
		     const char *value)
{
	return _tt_session_bprop_add(sessid, propname,
				     (unsigned char *)value,
				     (char *)0==value?0:strlen(value));
}

/* 
 * Add the value to the property for the session
 */
Tt_status
_tt_session_bprop_add(const char *sessid, const char *propname,
		      unsigned const char *value, int length)
{
	_Tt_c_session	*d_session;
	_Tt_string val(value, length);
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	PCOMMIT;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	if (!strcmp(sessid, (char *)(d_session->id()))) {
		return d_session->c_addprop(propname, val);
	} else {
		return TT_ERR_POINTER;
	}
}

/* 
 * Return the number of values for the property on the session
 */
int
_tt_session_prop_count(const char *sessid, const char *propname)
{
	int cnt;
	Tt_status err;
 	_Tt_c_session	*d_session;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	PCOMMIT;

	d_session = d_procid->default_session().c_pointer();
	if (!strcmp(sessid,(char *)(d_session->id()))) {
		err = d_session->c_propcount(propname, cnt);
		if (err != TT_OK) {
			return error_int(err);
		} else {
			return cnt;
		}
	} else {
		return error_int(TT_ERR_POINTER);
	}
}


/*
 * Replace all the values of the property of the session with the
 * supplied character string value
 */
Tt_status
_tt_session_prop_set(const char *sessid, const char *propname,
		     const char *value)
{
	return _tt_session_bprop_set(sessid, propname,
				     (unsigned char *)value,
				     (char *) 0 == value ? 0 : strlen(value));
}

/* 
 * Replace all of the values of the property of the session
 */
Tt_status
_tt_session_bprop_set(const char *sessid, const char *propname,
		      unsigned const char *value, int length)
{
	_Tt_string			val(value, length);
	_Tt_c_session	*d_session;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	PCOMMIT;

	d_session = d_procid->default_session().c_pointer();
	if (!strcmp(sessid,(char *)(d_session->id()))) {
		return(d_session->c_setprop(propname, val));
	} else {
		return TT_ERR_POINTER;
	}
}

/* 
 * Return the nth propname of the session
 */
char *
_tt_session_propname(const char *sessid, int n)
{
	_Tt_string			prop;
	Tt_status			err;
	_Tt_c_session	*d_session;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	PTR_PCOMMIT(char_ptr);

	d_session = d_procid->default_session().c_pointer();
	if (!strcmp(sessid,(char *)(d_session->id()))) {
		err =  d_session->c_propname(n, prop);
		if (err != TT_OK) {
			return (char *)error_pointer(err);
		}
		return _tt_strdup(prop);
	} else {
		return (char *)error_pointer(TT_ERR_POINTER);
	}
}

/* 
 * Return the number of properties on the session
 */
int
_tt_session_propnames_count(const char *sessid)
{
	int				cnt;
	Tt_status			err;
	_Tt_c_session	*d_session;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	PCOMMIT;

	d_session = d_procid->default_session().c_pointer();
	if (!strcmp(sessid,(char *)(d_session->id()))) {
		err = d_session->c_propnames_count(cnt);
		if (err != TT_OK) {
			return error_int(err);
		} else {
			return cnt;
		}
	} else {
		return error_int(TT_ERR_POINTER);
	}
}
