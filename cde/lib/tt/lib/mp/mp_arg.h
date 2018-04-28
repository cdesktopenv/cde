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
/*%%  $XConsortium: mp_arg.h /main/3 1995/10/23 10:19:02 rswiston $ 			 				 */
/* -*-C++-*-
 * Tool Talk Message Passer (MP) - mp_arg.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 *
 * _Tt_arg is an object that represents a positional argument in a
 * _Tt_message.  This object implements methods to do the XDR encoding
 * and decoding of value types (which may be user-defined). This
 * module also implements XDR encoding and decoding of _Tt_arg_list
 * objects but this will eventually be integrated into the list
 * package.
 */

#ifndef _MP_ARG_H
#define _MP_ARG_H
#include <mp/mp_global.h>
#include <mp/mp_arg_utils.h>
#include <rpc/rpc.h>

class _Tt_message;
class _Tt_arg : public _Tt_object {
      public:
	enum stored_data_type {UNSET,INT,STRING};
	_Tt_arg();
	_Tt_arg(const _Tt_arg &a);
	_Tt_arg(const _Tt_arg_ptr &p);
	_Tt_arg(const _Tt_string &t);
	_Tt_arg(Tt_mode m, _Tt_string &type);
	_Tt_arg(const char *t);
	_Tt_arg(Tt_mode m, const char *type);
	virtual ~_Tt_arg();
	Tt_status		set_data_int(int i);
	Tt_status		set_data_string(const _Tt_string &s);
	Tt_status		set_mode(Tt_mode m) {
		_mode = m;
		return TT_OK;
	}
	Tt_status		set_type(const _Tt_string &type) {
		_type = type;
		return TT_OK;
	}
	Tt_status		set_name(const _Tt_string &name) {
		_name = name;
		return TT_OK;
	}
	Tt_mode			mode() const {
		return _mode;
	}
	const _Tt_string	&type() const {
		return _type;
	}
	const _Tt_string	&name() const {
		return _name;
	}
	stored_data_type	data_type() const {
		return _data_type;
	}
	Tt_status		data_int(int &i) const;
	int			data_int() const {
		return _data_int;
	}
	Tt_status		data_string(_Tt_string &s) const;
	const _Tt_string	&data_string() const {
		return _data_string;
	}
	void			print(const _Tt_ostream &os) const;
	bool_t			xdr(XDR *xdrs);
	Tt_status		update_value(const _Tt_arg &x);
        int			match_score(const _Tt_arg &arg,
					    int &used_wildcard) const;
	int			operator==(const _Tt_arg &arg) const;

      protected:
	Tt_mode			_mode;
	_Tt_string		_type;	
	_Tt_string		_name;
	
	stored_data_type	_data_type;
	// Only one of the following two values will be set. This could
	// be a union, with _data_type as the discriminant, but it is
	// not worth the trouble.
	int			_data_int;
	_Tt_string		_data_string;
      private:
	void			constructor_common();
};
void _tt_arg_print(const _Tt_ostream &os, const _Tt_object *obj);
bool_t tt_xdr_arg(XDR *xdrs, _Tt_arg_ptr *ptr);
bool_t tt_xdr_arg_list(XDR *xdrs, _Tt_arg_list_ptr *lptr);
#endif				/* _MP_ARG_H */
