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
/*%%  $XConsortium: mp_typedb.h /main/3 1995/10/23 12:02:24 rswiston $ 			 				 */
/* 
 * @(#)mp_typedb.h	1.25 @(#)
 * mp_typedb.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _MP_TYPEDB_H
#define _MP_TYPEDB_H

#include <sys/stat.h>
#include "util/tt_object.h"
#include "mp/mp_global.h"
#include "mp_otype_utils.h"
#include "mp_ptype_utils.h"
#include "mp_signature_utils.h"
#include "mp_typedb_utils.h"

#define TT_NS_NAME "SUN_TOOLTALK_TYPES"

/*
 * This is also used by tt_type_comp.
 */
int	_tt_map_xdr_dbpaths(_Tt_string &udb, _Tt_string &sdb, _Tt_string &ndb);

typedef enum {
	TypedbUser,
	TypedbSystem,
	TypedbNetwork,
	TypedbAll,
	TypedbNone
} _Tt_typedbLevel;

class _Tt_typedb : public _Tt_object {

      public:
	_Tt_typedb(char *ce_dir = (char *)0);
	virtual ~_Tt_typedb();
	int				abort_write();
	int				begin_write(_Tt_typedbLevel db);
	int				end_write();
	Tt_status			write(const _Tt_string &outfile);
	Tt_status			write(FILE *outfile);

	static Tt_status		send_saved(const _Tt_string &savedfile);

	int				remove_otype(_Tt_string otid);
	int				remove_ptype(_Tt_string ptid);
	int				insert(_Tt_ptype_ptr &pt);
	int				insert(_Tt_otype_ptr &ot);
	Tt_status			init_xdr(const _Tt_string &binary_file);
	Tt_status			init_xdr(FILE *f);
	Tt_status			init_xdr(_Tt_typedbLevel db= TypedbAll);
	Tt_status			init_ce( _Tt_typedbLevel db= TypedbAll);
	static Tt_status		merge_from(const _Tt_string &xdr_file,
						   _Tt_typedb_ptr &tdb);
	static Tt_status		merge_from(FILE *xdr_file,
						   _Tt_typedb_ptr &tdb,
						   int &version);
	static Tt_status		merge_from(XDR *xdrs,
						   _Tt_typedb_ptr &tdb,
						   int &version);
	static Tt_status		ce2xdr();
	static _Tt_typedbLevel		level( const _Tt_string &level_name );
	static const char	       *level_name( _Tt_typedbLevel db );
	static _Tt_string_list	       *tt_path();
	void				print(const _Tt_ostream &os) const;
	void				pretty_print(
						const _Tt_ostream &os) const;
	int				xdr_version_required() const;
	bool_t				xdr(XDR *xdrs);

	_Tt_ptype_table_ptr		ptable;
	_Tt_otype_table_ptr		otable;
	_Tt_string			user_db;
	_Tt_string			system_db;
	_Tt_string			network_db;
	_Tt_typedbLevel			ceDB2Use;
      private:
#ifdef OPT_CLASSING_ENGINE
	void				*make_ce_entry(_Tt_ptype_ptr &pt);
	void				*make_ce_entry(_Tt_otype_ptr &ot);
	void				*make_ce_entry(_Tt_signature_ptr &st);
	_Tt_string			_ce_dir;
#endif				/* OPT_CLASSING_ENGINE */
	int				write_ce_header(const _Tt_ostream& os) const;
	int				_flags;
	_Tt_string			_lock_file;
};

#endif				/* _MP_TYPEDB_H */


