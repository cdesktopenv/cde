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
/*%%  $XConsortium: mp_ptype.h /main/3 1995/10/23 11:52:31 rswiston $ 			 				 */
/*
 *  Tool Talk Message Passer (MP) - mp_ptype.h
 *
 *  Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 *  Declaration of the _Tt_ptype class representing an in-memory process type
 *  definition.
 */

#ifndef _MP_PTYPE_H
#define _MP_PTYPE_H

#include "mp/mp_global.h"
#include "mp/mp_trace.h"
#include "mp_s_message_utils.h"
#include "mp_s_procid_utils.h"
#include "mp_signature_utils.h"
#include "util/tt_table.h"


/*
 * _Tt_ptype is an in-memory representation of a process type
 * definition in a ptype database.
 */

declare_list_of(_Tt_ptype_prop)

class _Tt_ptype_prop : public _Tt_object {
      public:
	_Tt_ptype_prop();
	_Tt_ptype_prop(_Tt_string name, _Tt_string value);
	virtual ~_Tt_ptype_prop();
	_Tt_string		name();
	_Tt_string		value();
	void			print(const _Tt_ostream &os) const;
	void			pretty_print(const _Tt_ostream &os) const;
	bool_t			xdr(XDR *xdrs);
      private:
	_Tt_string		_name;
	_Tt_string		_value;
};

class _Tt_ptype_ptr;
class _Tt_ptype : public _Tt_object {
      public:
	_Tt_ptype();
	virtual ~_Tt_ptype();
	_Tt_signature_list_ptr	&osigs();
	_Tt_signature_list_ptr	&hsigs();
	void			proc_replied();
	void			proc_started(const _Tt_s_procid_ptr &pr);
	const _Tt_string &	ptid() const {return _ptid;};
	int			getprop(_Tt_string propname,
					_Tt_string &value);
	Tt_status		start(_Tt_s_message_ptr &m,
					const _Tt_msg_trace &trace);
	void			set_ptid(_Tt_string ptid);
	void			append_osigs(_Tt_signature_list_ptr &sigs);
	void			append_hsigs(_Tt_signature_list_ptr &sigs,
					Tt_category category);
	void			append_osig(_Tt_signature_ptr sig);
	void			append_hsig(_Tt_signature_ptr sig,
					Tt_category category);
	void			appendprop(_Tt_string name, _Tt_string value);
	void			print(const _Tt_ostream &os) const;
	void			pretty_print(const _Tt_ostream &os) const;
	int			check_semantics();
	bool_t			xdr(XDR *xdrs);
	pid_t			start_pid() const;
	int			launching() const;
	void			launch_failed();
	void			merge_with(_Tt_ptype_ptr &pt);
	void			*ce_entry;
	int			xdr_version_required() const;
      private:
	friend class  _Tt_typedb;
	Tt_status		launch(_Tt_s_message_ptr &m,
					const _Tt_msg_trace &trace);
	int			_launching;
	pid_t			_start_pid;
	_Tt_string		_ptid;
	_Tt_ptype_prop_list_ptr	_props;
	_Tt_s_message_list_ptr	_queued_messages;
	_Tt_s_message_ptr	_startmsg;
	_Tt_signature_list_ptr	_osigs;
	_Tt_signature_list_ptr	_hsigs;
      
	friend int		operator==(_Tt_ptype&,_Tt_ptype&);
	friend int		operator!=(_Tt_ptype&,_Tt_ptype&);
};

_Tt_string _tt_ptype_ptid(_Tt_object_ptr &o);
void _tt_otype_print(const _Tt_ostream &os, const _Tt_object *obj);
#endif				/* _MP_PTYPE_H */
