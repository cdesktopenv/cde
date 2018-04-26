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
/*%%  $XConsortium: tttar_spec.h /main/3 1995/10/20 17:00:53 rswiston $ 			 				 */
/*
 * tttar_spec.h - Interface to specs for the Link Service/ToolTalk archive tool
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#ifndef _LSTAR_SPEC_H
#define _LSTAR_SPEC_H

declare_list_of(Lstar_spec_prop)

class Lstar_spec : public _Tt_object {
    public:
	Lstar_spec();
	Lstar_spec(  _Tt_string id, _Tt_string path );
	~Lstar_spec();

	_Tt_string		id() {return _id;}
	_Tt_string		path() {return _path;}
	void			path_set( _Tt_string p ) {_path = p;}
	bool_t			xdr( XDR *xdrs );
	Tt_status		read_self();
	char		       *write_self( _Tt_string where,
					    bool_t preserve__props,
					    Tt_status *err );
	void			print(FILE *fs = stdout) const;

        static void 		do_print(const _Tt_ostream &os,
					 const _Tt_object *obj);
    private:
	_Tt_string		_id;
	_Tt_string		_type;
	_Tt_string		_path;
	Lstar_spec_prop_list_ptr	_props;
};

class Lstar_spec_prop : public _Tt_object {
    public:
	Lstar_spec_prop();
	Lstar_spec_prop( _Tt_string spec_id, _Tt_string propname );
	~Lstar_spec_prop();

	bool_t			xdr( XDR *xdrs );
	Tt_status		write_self( char *spec_id,
					    bool_t preserve__props);
	void			print(FILE *fs = stdout) const;
        static void 		do_print(FILE *fs, const _Tt_object *obj);

    private:
	_Tt_string		_propname;
	_Tt_string_list_ptr	_values;
};

#endif /* _LSTAR_SPEC_H */
