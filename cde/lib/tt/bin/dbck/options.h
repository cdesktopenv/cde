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
/*%%  $XConsortium: options.h /main/3 1995/10/20 16:25:40 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * options.h
 *
 * Class definitions for classes to parse and hold options from
 * the command lines.
 *
 * Part of the ToolTalk/Link Service data base inspect and repair tool.
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _OPTIONS_H
#define _OPTIONS_H
#include "util/tt_string.h"
#include "binkey.h"     

// Options common to both ttdbck and lsdbck

class Dbck_options : public _Tt_object {

      public:
	Dbck_options();
	int			set_opts(int argc, char **argv);

	const _Tt_string_list_ptr	&dbdirectories() const {
		return _dbdirectories;};

	// Selection options

	virtual int		selecting_p() 	const{
	        return _sel_filename_p | _sel_objid_p | _sel_type_p;
	}
	int			sel_filename_p()const{return _sel_filename_p;};
	const _Tt_string       &sel_filename()	const{return _sel_filename;};
	int			sel_objid_p()	const{return _sel_objid_p;};   
	_Tt_db_key_ptr		sel_objid_key() const{return _sel_objid_key;}; 
	int			sel_type_p()	const{return _sel_type_p;};
	const _Tt_string       &sel_type()	const{return _sel_type;};

	// Diagnosis options

	virtual int 		diagnosing_p()	const{
		return _diag_badform_p | _diag_exist_p;
	};
	int			diag_badform_p()const{return _diag_badform_p;};
	int			diag_exist_p()	{return _diag_exist_p;};

	// Display options

	virtual int		displaying_p()	const{
		return _disp_id_p | _disp_mand_p | _disp_prop_p;
	};
	int			disp_id_p()	const{return _disp_id_p;};
	int			disp_mand_p()	const{return _disp_mand_p;};
	int			disp_prop_p()	const{return _disp_prop_p;};

	// Repair options

	// Note repair_netisam_p is not included in repairing_p since
	// netisam repair occurs *before* inspection, instead of after.

	virtual int		repairing_p()	const{
		return _repair_type_p | _repair_delete_p;
	};
	int			repair_netisam_p() const{return _repair_netisam_p;};
	int			repair_type_p()	const{return _repair_type_p;};
	const _Tt_string       &repair_type()	const{return _repair_type;};
	int 			repair_delete_p() const{return _repair_delete_p;};
	int			debug_level()	const{return _debug_level;};

	virtual char *		type_string() const {
		return "Dbck_options";
	};
	virtual void		print(FILE *f = stdout) const;

      protected:
	_Tt_string_list_ptr	_dbdirectories;

	// Selection options

	int			_sel_filename_p;
	_Tt_string		_sel_filename;	// shell wildcard pattern
	int			_sel_objid_p;   
	_Tt_db_key_ptr		_sel_objid_key; 
	int			_sel_type_p;
	_Tt_string		_sel_type;	// shell wildcard pattern

	// Diagnosis options

	int			_diag_badform_p;	
	int			_diag_exist_p;

	// Display options

	int			_disp_id_p;
	int			_disp_mand_p;
	int			_disp_prop_p;

	// Repair options

	int			_repair_netisam_p;
	int			_repair_type_p;
	_Tt_string		_repair_type;
	int 			_repair_delete_p;

	int			_debug_level;
	virtual char *		optstring()=0;
	virtual int		set_option(int optchar, const char *optval)=0;
	int			set_common_option(int optchar,
						  const char *optval);
};
#endif
