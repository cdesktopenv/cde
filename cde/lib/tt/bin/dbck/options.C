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
//%%  $TOG: options.C /main/4 1998/03/20 14:26:44 mgreess $ 			 				
/*
 *
 * options.cc
 *
 * Common option handling routines
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include "util/copyright.h"
#if defined(linux) || defined(sgi) || defined(__FreeBSD__)
#include <getopt.h>
#endif     
#include "dbck.h"     
#include "options.h"
#include "util/tt_iostream.h"
#include "util/tt_gettext.h"
     
Dbck_options::
Dbck_options()
{
	_dbdirectories = new _Tt_string_list;
	_sel_filename_p = 0;
	_sel_filename = (char *) 0;
	_sel_objid_p = 0;
	_sel_objid_key = (_Tt_db_key *) 0;
	_sel_type_p = 0;
	_sel_type = (char *) 0;
	_diag_badform_p = 0;
	_diag_exist_p = 0;
	_disp_id_p = 0;
	_disp_mand_p = 0;
	_disp_prop_p = 0;
	_repair_netisam_p = 0;
	_repair_type_p = 0;
	_repair_type = (char *) 0;
	_repair_delete_p = 0;
	_debug_level = 0;
}

int Dbck_options::
set_opts(int argc, char **argv)
{
	int c;
	char *opts = optstring();

	while (-1 != (c = getopt(argc,(char **)argv,opts))) {
		if (!set_option(c, optarg)) {
			//print help
			return 0;
		}
	}
	_dbdirectories = new _Tt_string_list;
	for (; optind<argc; ++optind) {
		_Tt_string s(argv[optind]);
		_dbdirectories->append(s);
	}
	return 1;
}


int Dbck_options::
set_common_option(int optchar, const char *optval)
{
	switch (optchar) {
	      case '?':
	      case 'h':		// treat h as illegal, forces Usage: msg
		return 0;
	      case 'v':
		_TT_PRINT_VERSIONS(progname)
		exit(0);
	      case 'd':
		_debug_level = atoi(optval);
		break;
	      case 'f':
		_sel_filename_p = 1;
		_sel_filename = optval;
		break;
	      case 'k':
		_sel_objid_p = 1;
		_sel_objid_key = new _Tt_db_key(_Tt_string(optval));
		break;
	      case 't':
		_sel_type_p = 1;
		_sel_type = optval;
		break;
	      case 'b':
		_diag_badform_p = 1;
		break;
	      case 'x':
		_diag_exist_p = 1;
		break;
	      case 'i':
		_disp_id_p = 1;
		break;
	      case 'm':
		_disp_mand_p = 1;
		break;
	      case 'p':
		_disp_prop_p = 1;
		break;
	      case 'I':
		_repair_netisam_p = 1;
		break;
	      case 'T':
		_repair_type_p = 1;
		_repair_type = optval;
		break;
	      case 'Z':
		_repair_delete_p = 1;
		break;
	      default:
		return 0;
	}
	return 1;
}

void Dbck_options::
print(FILE *f) const
{
	fprintf(f,"%s <\n",type_string());
	fprintf(f,"\nDirectories:\n");
	dbdirectories()->print(_tt_string_print,f);
	fprintf(f,"\n");
			
	if (_sel_filename_p) {
		fprintf(f,catgets(_ttcatd, 6, 7, "Select by filename: %s\n"),
			(char *)_sel_filename);
	}
	if (_sel_objid_p) {
		fprintf(f,catgets(_ttcatd, 6, 8, "Select by objid key:"));
		_sel_objid_key->print(f);
		fprintf(f,"\n");
	}
	if (_sel_type_p) {
		fprintf(f,catgets(_ttcatd, 6, 9, "Select by type: %s\n"),
			(char *)_sel_type);
	}
	if (_diag_badform_p) {
		fprintf(f,catgets(_ttcatd, 6, 10,
				  "Diagnose badly formed entities\n"));
	}
	if (_diag_exist_p) {
		fprintf(f,catgets(_ttcatd, 6, 11, "Diagnose references to "
				  "non-existent entities\n"));
	}
	if (_disp_id_p) {
		fprintf(f,catgets(_ttcatd, 6, 12, "Display ids\n"));
	}
	if (_disp_mand_p) {
		fprintf(f,catgets(_ttcatd, 6, 13, "Display mandatory data\n"));
	}
	if (_disp_prop_p) {
		fprintf(f,catgets(_ttcatd, 6, 14,
				  "Display properties and values data\n"));
	}
	if (_repair_netisam_p) {
		fprintf(f,catgets(_ttcatd, 6, 15,
				  "Invoke NetISAM isrepair() function before "
				  "inspecting\n"));
	}
	if (_repair_type_p) {
		fprintf(f,catgets(_ttcatd, 6, 16,
				  "Repair by setting to type: %s\n"),
			(char *)_repair_type);
	}	
	if (_repair_delete_p) {
		fprintf(f,catgets(_ttcatd, 6, 17, "Repair by deleting\n"));
	}
	fprintf(f,catgets(_ttcatd, 6, 18, "Debugging printout level %d\n"),
		_debug_level);
}




