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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tttar_spec.C /main/3 1995/10/20 17:00:42 rswiston $ 			 				
/*
 * tttar_spec.cc - Implementation of specs for Link Service/ToolTalk archiving
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#if defined(__osf__) || defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <unistd.h>
#else
#if defined (USL) || defined(__uxp__)
#include "tt_options.h"
#if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
#include <unistd.h>
#else
#include <osfcn.h>
#endif /* if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP) */
#else
#include <osfcn.h>
#endif
#endif /* __osf__ */
#include "api/c/tt_c.h"
#include "util/tt_iostream.h"
#include "tttar_utils.h"
#include "tttar_spec.h"

/*
 * Lstar_spec::Lstar_spec()
 */
Lstar_spec::
Lstar_spec()
{
	_props = new Lstar_spec_prop_list();
}

/*
 * Lstar_spec::Lstar_spec() - 
 */
Lstar_spec::
Lstar_spec(  _Tt_string id, _Tt_string path )
{
	_id = id;
	_path = path;
	_props = new Lstar_spec_prop_list();
}

/*
 * Lstar_spec::~Lstar_spec()
 */
Lstar_spec::
~Lstar_spec()
{
}

/*
 * Lstar_spec::xdr()
 */
bool_t Lstar_spec::
xdr( XDR *xdrs )
{
	if (! this->_id.xdr(xdrs)) {
		return FALSE;
	}
	if (! this->_path.xdr(xdrs)) {
		return FALSE;
	}
	if (! this->_type.xdr(xdrs)) {
		return FALSE;
	}
	if (! this->_props.xdr(xdrs)) {
		return FALSE;
	}
	return TRUE;
}

/*
 * Lstar_spec::read_self() - Read from ToolTalk everything we need to know
 *	about ourselves in order to archive ourself.
 */
Tt_status Lstar_spec::
read_self()
{
	/*
	 * Get the spec's type.
	 */
	note_ptr_err( tt_spec_type( (char *)_id ));
	if (IS_TT_ERR(err_noted)) {
		return err_noted;
	}
	_type = ptr_returned;
	/*
	 * Get how many properties are on the spec.
	 */
	note_int_err( tt_spec_propnames_count( (char *)_id ));
	if (IS_TT_ERR(err_noted)) {
		return err_noted;
	}
	int num_props = int_returned;
	/*
	 * Push the spec's properties onto our list in reverse order,
	 * to preserve the admittedly meaningless order they had.
	 */
	for (int n = num_props - 1; n >= 0; n--) {
		note_ptr_err( tt_spec_propname( (char *)_id, n ));
		switch (err_noted) {
		    case TT_ERR_OBJID:
		    case TT_ERR_DBAVAIL:
			return err_noted;
		    case TT_ERR_NUM:
			continue;
		    case TT_ERR_DBEXIST:
		    default:
			if (IS_TT_ERR(err_noted)) {
				return err_noted;
			}
		}
		_Tt_string		propname = ptr_returned;
		Lstar_spec_prop_ptr	prop_ptr;

		prop_ptr = new Lstar_spec_prop( _id, propname );
		this->_props->push( prop_ptr );
	}
	return err_noted;
}

/*
 * Lstar_spec::write_self() - Recreate a spec like the one we are, returning
 *	the id of the spec created.  The string returned must be freed
 *	using tt_free().
 */
char * Lstar_spec::
write_self( _Tt_string where, bool_t preserve__props, Tt_status *err )
{
	char	       *spec_created = NULL;
	_Tt_string	path;

	if (this->_path.len() <= 0) {
		*err = TT_ERR_PATH;
		return NULL;
	}
	/*
	 * TO_DO: tt_spec_create() won't convert /./ to / in a path
	 * if the path doesn't exist.
	 */
	if (this->_path.left(2) == "./") {
		this->_path = this->_path.right( _path.len() - 2 );
	}
	/*
	 * If the archived path is absolute, ignore <where>.
	 */
	if (this->_path[0] == '/') {
		path = this->_path;
	} else {
		path = where.cat( "/" ).cat( this->_path );
	}
	note_ptr_err( tt_spec_create( (char *)path ));
	*err = err_noted;
	spec_created = ptr_returned;
	if (IS_TT_ERR(err_noted)) {
		return spec_created;
	}
	note_err( tt_spec_type_set( spec_created, (char *)this->_type ));
	*err = err_noted;
	if (IS_TT_ERR(err_noted)) {
		return spec_created;
	}
	Lstar_spec_prop_list_cursor prop_cursor( this->_props );
	while (prop_cursor.next()) {
		*err = prop_cursor->write_self( spec_created, preserve__props );
	}
	note_err( tt_spec_write( spec_created ));
	*err = err_noted;
	return spec_created;
}

/*
 * Lstar_spec::print()
 */
void Lstar_spec::
print( FILE *fs ) const
{
	fprintf( fs, "spec id: "  );
	this->_id.print( fs );
	fprintf( fs, "\nspec type: " );
	this->_type.print( fs );
	fprintf( fs, "\nspec path: " );
	this->_path.print( fs );
	fprintf( fs, "\n" );
	this->_props->print(Lstar_spec::do_print, fs );
	fprintf( fs, "\n" );
}

implement_list_of(Lstar_spec_prop)

/*
 * Lstar_spec_prop::Lstar_spec_prop()
 */
Lstar_spec_prop::
Lstar_spec_prop()
{
}

/*
 * Lstar_spec_prop::Lstar_spec_prop()
 */
Lstar_spec_prop::
Lstar_spec_prop( _Tt_string id, _Tt_string propname )
{
	_propname = propname;
	_values = new _Tt_string_list();
	/*
	 * Get how many values are in the spec's property.
	 */
	note_int_err( tt_spec_prop_count( (char *)id, (char *)propname ));
	switch (err_noted) {
	    case TT_ERR_PROPNAME:
	    case TT_ERR_OBJID:
	    case TT_ERR_DBAVAIL:
		return;
	    case TT_ERR_DBEXIST:
	    default:
		if (IS_TT_ERR(err_noted)) {
			return;
		}
	}
	int num_values = int_returned;
	/*
	 * Push the property's values onto our list in reverse order,
	 * to preserve the order they had.
	 */
	for (int n = num_values - 1; n >= 0; n--) {
		int		len;
		unsigned char  *value;

		note_err( tt_spec_bprop( (char *)id, (char *)propname, n, &value, &len ));
		switch (err_noted) {
		    case TT_ERR_PROPNAME:
		    case TT_ERR_OBJID:
		    case TT_ERR_DBAVAIL:
			return;
		    case TT_ERR_NUM:
			continue;
		    case TT_ERR_DBEXIST:
		    default:
			if (IS_TT_ERR(err_noted)) {
				return;
			}
		}
		_Tt_string val( value, len );
		this->_values->push( val );
	}
}

/*
 * Lstar_spec_prop::~Lstar_spec_prop()
 */
Lstar_spec_prop::
~Lstar_spec_prop()
{
}

/*
 * Lstar_spec_prop::xdr()
 */
bool_t Lstar_spec_prop::
xdr( XDR *xdrs )
{
	if (! this->_propname.xdr(xdrs)) {
		return FALSE;
	}
	if (! this->_values.xdr(xdrs)) {
		return FALSE;
	}
	return TRUE;
}

/*
 * Lstar_spec_prop::write_self() - Write this prop onto the given spec.
 */
Tt_status Lstar_spec_prop::
write_self( char *spec_id, bool_t preserve__props )
{
	/*
	 * If we're not root and this is a blessed property,
	 * do not attempt to write it.
	 * Insert link here to policy statement about prop names in tt_c.h.
	 */
	if (    (_propname[0] == '_')
	     && (    (! preserve__props)
	          || (getuid() != 0)))
	{
		return TT_OK;
	}
	_Tt_string_list_cursor value_cursor( this->_values );
	char	*val;
	int	len;
	while (value_cursor.next()) {
		val = (char *)(*value_cursor);
		len = (*value_cursor).len();
		note_err( tt_spec_bprop_add( spec_id, (char *)_propname,
					    (unsigned char *)val, len));
		if (IS_TT_ERR(err_noted)) {
			return err_noted;
		}
	}
	return err_noted;
}

/*
 * Lstar_spec_prop::print()
 */
void Lstar_spec_prop::
print( FILE *fs ) const
{
	this->_propname.print( fs );
	fprintf( fs, ": " );
	this->_values->print(_tt_string_print,fs);
	fprintf( fs, "\n" );
}

void Lstar_spec::
do_print(const _Tt_ostream &os, const _Tt_object *obj)
{
	((Lstar_spec *)obj)->print(os.theFILE());
}

void Lstar_spec_prop::
do_print(FILE *fs, const _Tt_object *obj)
{
	((Lstar_spec_prop *)obj)->print(fs);
}

