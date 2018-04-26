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
/*%%  $XConsortium: mp_types_table.h /main/3 1995/10/20 16:39:07 rswiston $ 			 				 */
/*
 * Tool Talk Message Passer (PM) - mp_types_table.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * This file contains the declaration for the class _TT_types_table which implements
 * a hash table of otype and ptype descriptors.
 *
 */

#ifndef  _TT_MP_TYPES_TABLE_H
#define  _TT_MP_TYPES_TABLE_H
#include "util/tt_object.h"
#include "mp/mp_global.h"
#include "mp_otype_utils.h"
#include "mp_otype.h"
#include "mp_ptype.h"
#include "mp_ptype_utils.h"
#include "mp_signature.h"
#include "mp_signature_utils.h"
#include "mp/mp_context.h"
#include "util/tt_list.h"
#include "util/tt_string.h"
#include "util/tt_table.h"
#include <stdio.h>

/*  Hack for JLE systems */

#ifdef input
#undef input
#endif
#ifdef output
#undef output
#endif

/* Hack for flex */
#ifdef	FLEX_SCANNER
#ifdef	yywrap
#undef	yywrap
#endif
#endif	/* FLEX_SCANNER */

enum _Tt_parse_status {
	_TT_PARSE_OK,
	_TT_PARSE_ERROR
};

extern _Tt_string cpp_options;

class _Tt_types_table : public _Tt_object {
      public:
	_Tt_types_table(FILE *infs, _Tt_parse_status &result);
	_Tt_types_table(_Tt_string types_def_path, _Tt_parse_status &result);
	_Tt_parse_status	parse_path(_Tt_string path);
	_Tt_parse_status	parse_yyin();
	_Tt_parse_status	check_semantics();
	_Tt_otype_table_ptr	&otypes();
	_Tt_otype_ptr		lookup_otype(_Tt_string otid) const;
	int			next_otype(_Tt_otype_ptr &o);
	void			reset_otype_cursor();
	_Tt_ptype_table_ptr	&ptypes();
	_Tt_ptype_ptr		lookup_ptype(_Tt_string ptid) const;
	int			next_ptype(_Tt_ptype_ptr &p);
	void			reset_ptype_cursor();
	virtual void		print(FILE *fs = stdout) const;
	int			has_contexts;
      private:
	void			insert_otype(_Tt_otype_ptr &otype);
	void			insert_ptype(_Tt_ptype_ptr &ptype);
	void			reset();
	void			reset_tmps();
	void			append_sig(_Tt_signature_ptr &sig);
	void			set_tmp_propname(_Tt_string name);
	_Tt_parse_status	check_cycle(_Tt_otype_ptr otype, _Tt_otype **path,
					    int top);
	_Tt_parse_status	check_otypes();
	_Tt_parse_status	check_ptypes();
	_Tt_parse_status	check_otype(_Tt_otype_ptr otype);
	_Tt_parse_status	check_super_sig(_Tt_otype_ptr otype,
						_Tt_signature_ptr sig,
						_Tt_string super,
						int handles);
	int			yyparse(void);
	void			yyerror(const char *s);
	int			yylex(void);
	int			yylook();
	int			yyback(int *p, int m);
	int			input();
	void			unput(int c);
	void			output(int c);
	int			yywrap(void);
	void			skip_c_comments(void);
	void			skip_cplusplus_comments(void);

	_Tt_ptype_table_ptr		ptable;
	_Tt_ptype_table_cursor		ptc;
	_Tt_otype_table_ptr		otable;
	_Tt_otype_table_cursor		otc;
	FILE			*yyin;
	_Tt_string		_file_name;
	int			_yylineno;
	_Tt_otype_ptr		tmp_otype;
	_Tt_ptype_ptr		tmp_ptype;
	_Tt_string_list_ptr	tmp_otidl;
	_Tt_string		tmp_otid;
	_Tt_signature_list_ptr	tmp_sigl;
	_Tt_signature_ptr	tmp_sig;
	_Tt_arg_ptr		tmp_arg;
	_Tt_string		tmp_propname;
	_Tt_string		tmp_propvalue;
};

#endif  /* _TT_MP_TYPES_TABLE_H */
