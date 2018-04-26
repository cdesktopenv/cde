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
/*%%  $XConsortium: mp_otype.h /main/3 1995/10/23 11:51:41 rswiston $ 			 				 */
/*
 *  Tool Talk Message Passer (MP) - mp_otype.h
 *
 *  Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 *  Declaration of the _Tt_otype class representing an in-memory object type
 *  definition.
 */

#ifndef _MP_OTYPE_H
#define _MP_OTYPE_H

#include "mp_signature.h"
#include "util/tt_table.h"

typedef char **(*_Tt_otype_filter)(char *otid,
					   void *context,
					   void *accumulator);
class _Tt_types_table;

class _Tt_otype : public _Tt_object {
      public:
	_Tt_otype();
	_Tt_otype(_Tt_string otid);
	virtual ~_Tt_otype();
	_Tt_string_list_ptr	&children();
	_Tt_string_list_ptr	&parents();
	const _Tt_string &	otid() const {return _otid;};
	void			set_otid(_Tt_string s);
	_Tt_signature_list_ptr	&hsigs();
	_Tt_signature_list_ptr	&osigs();
	void			set_ancestors(_Tt_string_list_ptr ancs);
	void			append_inhs(_Tt_signature_ptr sig);
	void			append_osig(_Tt_signature_ptr sig);
	void			append_hsig(_Tt_signature_ptr sig,
					Tt_category category);
	void			append_osigs(_Tt_signature_list_ptr sigs);
	void			append_hsigs(_Tt_signature_list_ptr sigs,
					Tt_category category);
	void			print(const _Tt_ostream &os) const;
	void			pretty_print(const _Tt_ostream &os) const;
	bool_t			xdr(XDR *xdrs);
	void			*ce_entry;
	int			xdr_version_required() const;
	friend	class	_Tt_types_table;
      private:
	friend class _Tt_typedb;
	_Tt_string		_otid;
	_Tt_string_list_ptr	_ancestors;
	_Tt_string_list_ptr	_children;
	_Tt_signature_list_ptr	_inhs;
	_Tt_signature_list_ptr	_osigs;
	_Tt_signature_list_ptr	_hsigs;
	int			visitp;
};
_Tt_string _tt_otype_otid(_Tt_object_ptr &o);
void _tt_ptype_print(const _Tt_ostream &os, const _Tt_object *obj);

#endif				/* _MP_OTYPE_H */
