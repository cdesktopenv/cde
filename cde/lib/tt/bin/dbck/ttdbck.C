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
//%%  $XConsortium: ttdbck.C /main/3 1995/10/20 16:34:00 rswiston $ 			 				
/*
 *
 * ttdbck.cc
 * @(#)ttdbck.C	1.31 93/09/07
 *
 * ToolTalk 1.0 spec data base inspect and repair tool
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <isam.h>
#include <locale.h>
#include <memory.h>
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "util/tt_global_env.h"
#include "ttdbck.h"
#include "dbck.h"
#include "options_tt.h"
#include "binkey.h"
#include "spec.h"     
#include "util/copyright.h"
#include "dm/dm_recfmts.h"
#include "mp/mp_mp.h"    
#include "mp_s_mp.h"    
#include "tt_db_server_consts.h"
#include "tt_isstrerror.h"

TT_INSERT_COPYRIGHT

#ifdef OPT_PATCH
static char PatchID[] = "Patch Id: 100626_03.";
static int Patch_ID100626_03;
#endif

char *progname;			// from argv[0]
Dbck_specoptions *opts;
_Tt_typedb_ptr *tdb_ptr;
FILE *tstream;			// for calling print() methods in dbx

// Global variables controlling state of main merge loop.

Table_oid_prop oid_prop_record;
Table_oid_access oid_access_record;
Table_oid_container oid_container_record;
Table_docoid_path docoid_path_record;

struct keydesc docoid_path_keydesc;

int oid_prop_fd, oid_access_fd, oid_container_fd, docoid_path_fd;

Binkey this_min_key;
Binkey oid_prop_key;
Binkey oid_access_key;
Binkey oid_container_key;
Binkey last_min_key;

int oid_access_inspected;
int oid_container_inspected;

_Tt_string oid_prop_value;
_Tt_string oid_prop_name;

_Tt_string oid_prop_rootname;
_Tt_string oid_access_rootname;
_Tt_string oid_container_rootname;
_Tt_string docoid_path_rootname;

Spec_list_ptr specs_to_repair;


// TOC

int	main(int argc, char **argv);
int	process_directory(_Tt_string dirname);
void	process_spec(Spec_ptr s);
void	advance_oid_prop();
void	advance_oid_container();
void	advance_oid_access();
void	closeall();
Binkey	compute_min_key(Binkey a, Binkey b, Binkey c);
void	inspect_docoid_path(Spec_ptr p);
void	pisamerr(const char *func, const char *name);
void    check_if_file(Spec_ptr p);

// isam.h does not include function headers at all!!

extern "C" {
	int isaddindex(int, struct keydesc*);
	int isbuild(char*, int, struct keydesc*, int);
	int isclose(int);
	int iscntl(int, int, char*);
	int isdelrec(int, long);
	int iserase(char*);
	int isopen(char*, int);
	int isread(int, char*, int);
	int isrepair(const char *, int);
	int isrewrec(int, int, char*);
	int isstart(int, struct keydesc*, int, char*, int);
	int iswrite(int, char*);
}

int
main(int argc, char **argv)
{
	int status;
	
	tstream = stderr;
	opts = new Dbck_specoptions;
	progname = argv[0];
	setlocale( LC_ALL, "" );

	if (!opts->set_opts(argc, argv))
	{
		fprintf(stderr, "%s", catgets(_ttcatd, 6, 2,
"Usage:\n"
"ttdbck [-f file] [-k objkey] [-t type] [-bx] \n"
"[-impa] [-IZ] [-F newfilename] [-T newtype] [mountpoints]\n"));
		exit (1);
	}

	if (DBCK_DEBUG(1)) {
		opts->print(stderr);
	}

	if (opts->repairing_p() &&
	    !opts->selecting_p() &&
	    !opts->diagnosing_p()) {
		fprintf(stderr, "%s",
			catgets(_ttcatd, 6, 3,
				"ttdbck: you must specify a selection "
				"[-fkt] option or a diagnosis [-bx] option\n"
				"if a repair [-FTZ] option is specified\n"));
		exit (1);
	}

	if (opts->diag_badform_p()) {
		// We have to initialize just enough of the ttsession
		// server state to make the _Tt_typedb class work.
		// XXX: really the _Tt_typedb class should be independent
		// of the server.
		_tt_global = new _Tt_global;
		_tt_s_mp = new _Tt_s_mp;
		_tt_mp = (_Tt_mp *)new _Tt_s_mp;
		tdb_ptr = new _Tt_typedb_ptr;
		(*tdb_ptr) = new _Tt_typedb;

		Tt_status err;
		// Load the xdr types database
		err = (*tdb_ptr)->init_xdr();
		if (err == TT_ERR_NO_MATCH) {
			fprintf(stderr, "ttdbck: %s\n",
				catgets(_ttcatd, 6, 4,
					"Version mismatch in compiled types"));
			exit (1);
		} else if (err != TT_OK) {
			fprintf(stderr, "ttdbck: %s\n",
				catgets(_ttcatd, 6, 5,
					"Cannot read types in database"));
			exit (1);
		}
	}
			
	status = do_directories(opts->dbdirectories(),process_directory);

	// Normally UNIX programs don't make noise if nothing is wrong, but
	// in the case of ttdbck it's unlikely the admin running it is
	// familiar with it, and it's only being run if the database is
	// suspected of damage already.  So we print a reassuring message
	// if there are no errors.   If there are errors, there has
	// already been output.

	if (status==0) {
		fprintf(stderr, "%s",
			catgets(_ttcatd, 6, 25,
				"ttdbck: no errors found.\n"));
	}

	exit(status);
	return status;
}




/*
 * Main guts of ttdbck.  Called once for each directory named on the
 * command line. Returns 1 if the directory was processed without
 * detecting any errors, else 0.
 */
int
process_directory(_Tt_string dirname)
{
	struct keydesc oid_prop_keydesc;
	struct keydesc oid_access_keydesc;
	struct keydesc oid_container_keydesc;

	Spec_ptr this_spec;

	// Initialize for main loop.

	specs_to_repair = new Spec_list;

	oid_prop_rootname = dirname.cat(TT_DB_PROPERTY_TABLE_FILE);
	oid_prop_keydesc.k_flags = ISDUPS;
	oid_prop_keydesc.k_nparts = 2;
	oid_prop_keydesc.k_part[0].kp_start =
		offsetof(Table_oid_prop,objkey);
	oid_prop_keydesc.k_part[0].kp_leng =
		sizeof(oid_prop_record.objkey);
	oid_prop_keydesc.k_part[0].kp_type = BINTYPE;
	oid_prop_keydesc.k_part[1].kp_start =
		offsetof(Table_oid_prop,propname);
	oid_prop_keydesc.k_part[1].kp_leng =
		sizeof(oid_prop_record.propname);
	oid_prop_keydesc.k_part[1].kp_type = CHARTYPE;
	
	oid_access_rootname = dirname.cat(TT_DB_ACCESS_TABLE_FILE);
	oid_access_keydesc.k_flags = ISNODUPS;
	oid_access_keydesc.k_nparts = 1;
	oid_access_keydesc.k_part[0].kp_start =
		offsetof(Table_oid_access,objkey);
	oid_access_keydesc.k_part[0].kp_leng =
		sizeof(oid_access_record.objkey);
	oid_access_keydesc.k_part[0].kp_type = BINTYPE;

	oid_container_rootname = dirname.cat(TT_DB_FILE_OBJECT_MAP_FILE);
	oid_container_keydesc.k_flags = ISNODUPS;
	oid_container_keydesc.k_nparts = 1;
	oid_container_keydesc.k_part[0].kp_start =
		offsetof(Table_oid_container,objkey);
	oid_container_keydesc.k_part[0].kp_leng =
		sizeof(oid_container_record.objkey);
	oid_container_keydesc.k_part[0].kp_type = BINTYPE;

	docoid_path_rootname = dirname.cat(TT_DB_FILE_TABLE_FILE);
	docoid_path_keydesc.k_flags = ISNODUPS;
	docoid_path_keydesc.k_nparts = 1;
	docoid_path_keydesc.k_part[0].kp_start =
		offsetof(Table_docoid_path, dockey);
	docoid_path_keydesc.k_part[0].kp_leng =
		sizeof(docoid_path_record.dockey);
	docoid_path_keydesc.k_part[0].kp_type = BINTYPE;

	last_min_key = Binkey::smallest;

	oid_prop_fd = -1;
	oid_access_fd = -1;
	oid_container_fd = -1;
	docoid_path_fd = -1;

	if (opts->repair_netisam_p()) {
		if (-1==isrepair(oid_prop_rootname, 1)) {
			pisamerr("isrepair", oid_prop_rootname);
			return 0;
		}
		if (-1==isrepair(oid_access_rootname, 1)) {
			pisamerr("isrepair", oid_access_rootname);
			return 0;
		}
		if (-1==isrepair(oid_container_rootname, 1)) {
			pisamerr("isrepair", oid_container_rootname);
			return 0;
		}
		if (-1==isrepair(docoid_path_rootname, 1)) {
			pisamerr("isrepair", docoid_path_rootname);
			return 0;
		}
	}

	oid_prop_fd = isopen(oid_prop_rootname, ISVARLEN+ISINPUT+ISMANULOCK);
	if (oid_prop_fd==-1) {
		pisamerr("isopen",oid_prop_rootname);
		closeall();
		return 0;
	}
	oid_access_fd = isopen(oid_access_rootname,
			       ISVARLEN+ISINPUT+ISMANULOCK);
	if (oid_access_fd==-1) {
		pisamerr("isopen",oid_access_rootname);
		closeall();
		return 0;
	}
	oid_container_fd = isopen(oid_container_rootname,
				  ISVARLEN+ISINPUT+ISMANULOCK);
	if (oid_container_fd==-1) {
		pisamerr("isopen",oid_container_rootname);
		closeall();
		return 0;
	}
	docoid_path_fd = isopen(docoid_path_rootname,
				ISVARLEN+ISINPUT+ISMANULOCK);
	if (docoid_path_fd==-1) {
		pisamerr("isopen",docoid_path_rootname);
		closeall();
		return 0;
	}
	if (-1==isstart(oid_prop_fd, &oid_prop_keydesc,
			0,
			(char *)&oid_prop_record,
			ISFIRST)) {
		pisamerr("isstart",oid_prop_rootname);
		closeall();
		return 0;
	}
	if (-1==isstart(oid_access_fd, &oid_access_keydesc,
			0,
			(char *)&oid_access_record,
			ISFIRST)) {
		pisamerr("isstart", oid_access_rootname);
		closeall();
		return 0;
	}
	if (-1==isstart(oid_container_fd, &oid_container_keydesc,
			0,
			(char *)&oid_container_record,
			ISFIRST)){
		pisamerr("isstart", oid_container_rootname);
		closeall();
		return 0;
	}
	if (-1==isstart(docoid_path_fd, &docoid_path_keydesc,
			0,
			(char *)&docoid_path_record,
			ISFIRST)){
		pisamerr("isstart", docoid_path_rootname);
		closeall();
		return 0;
	}

	advance_oid_prop();
	advance_oid_access();
	advance_oid_container();
	this_min_key = compute_min_key(oid_prop_key,
				       oid_access_key,
				       oid_container_key);
	while (this_min_key<Binkey::largest) {
		if (last_min_key<this_min_key) {
 			// At this point, we have read all
			// information about the previous spec.
			// Analyze it, list it if called for,
			// queue it for later repair if called
			// for.

			process_spec(this_spec);
			
			// Complain about any missing required props
			// in previous spec, and reset the required property
			// list.

			// start accumulating a new spec

			this_spec = new Spec;
			this_spec->key =
			new Binkey((unsigned char *)(char *)this_min_key);
			check_if_file(this_spec);

			if (oid_access_key>this_min_key) {
				// mark spec as no access rec
			}

			if (oid_container_key>this_min_key) {
				// mark spec as no container rec
			}
		}
		
		if (!oid_container_inspected &&
		    oid_container_key==this_min_key) {
			oid_container_inspected = 1;
			inspect_docoid_path(this_spec);
		}

		if (oid_prop_key==this_min_key) {

			// Accumulate property

			this_spec->add_prop_and_value(oid_prop_name,
						      oid_prop_value);
		}

		// Advance to next records.

		if (oid_prop_key==this_min_key) {
			advance_oid_prop();
		} else {
			if (oid_container_key==this_min_key) {
				advance_oid_container();
			}
			if (oid_access_key==this_min_key) {
				advance_oid_access();
			}
		}
		last_min_key = this_min_key;
		this_min_key = compute_min_key(oid_prop_key,
					       oid_access_key,
					       oid_container_key);
	}
	// And remember to process the last spec.

	process_spec(this_spec);

	closeall();

	if (opts->repairing_p()) {
		Spec_list_cursor c(specs_to_repair);
		while(c.next()) {
			c->repair_spec();
		}
	}
	return 1;
}
	
// the advance_*() routines try to read the next record in the table;
// when eof is hit, they set the record key to the largest possible
// key.  This makes all the merge comparisons work right.

void
advance_oid_prop()
{
	if (-1==isread(oid_prop_fd, (char *)&oid_prop_record, ISNEXT)) {
		switch (iserrno) {
		      case EENDFILE:
			break;
		      default:
			pisamerr("isread", oid_prop_rootname);
			break;
		}
		oid_prop_key = Binkey::largest;
	} else {
		int l;
		oid_prop_key = oid_prop_record.objkey;
		l = sizeof(oid_prop_record.propname);
		if (oid_prop_record.propname[l-1]==NULL_CHAR) {
			// strip nulls
			l = strlen(oid_prop_record.propname);
		}
		oid_prop_name.set((unsigned char *)oid_prop_record.propname,l);
		oid_prop_value.set((unsigned char *)oid_prop_record.propval,
				   isreclen-offsetof(Table_oid_prop,propval));
	}
}
void
advance_oid_access()
{
	if (-1==isread(oid_access_fd, (char *)&oid_access_record, ISNEXT)) {
		switch (iserrno) {
		      case EENDFILE:
			break;
		      default:
			pisamerr("isread", oid_access_rootname);
			break;
		}
		oid_access_key = Binkey::largest;
	} else {
		oid_access_key = oid_access_record.objkey;
	}
	oid_access_inspected = 0;
}
void
advance_oid_container()
{
	if (-1==isread(oid_container_fd,
		       (char *)&oid_container_record, ISNEXT)) {
		switch (iserrno) {
		      case EENDFILE:
			break;
		      default:
			pisamerr("isread", oid_container_rootname);
			break;
		}
		oid_container_key = Binkey::largest;
	} else {
		oid_container_key = oid_container_record.objkey;
	}
	oid_container_inspected = 0;
}

void
closeall()
{
	if (oid_container_fd!=-1)	isclose(oid_container_fd);
	if (oid_access_fd!=-1)		isclose(oid_access_fd);
	if (oid_prop_fd!=-1)		isclose(oid_prop_fd);
	if (docoid_path_fd!=-1)		isclose(docoid_path_fd);
}

// compute_min_key finds the least key

Binkey
compute_min_key(Binkey a, Binkey b, Binkey c)
{
	Binkey result = a;
	if (b<result) result = b;
	if (c<result) result = c;
	return result;
}

// process spec is called when the next key is encountered, or at
// end of file.  The spec data accumulated in spec s is inspected,
// printed, and/or queued for later repair.

void
process_spec(Spec_ptr s)
{
	// The first time through the loop there is no accumulated spec
	// so s will be null.
	
	if (!s.is_null()) {
		s->process_spec();
	}
}		


// inspect_docoid_path retrieves the docoid_path record for the current
// spec and fills the file name into this_spec.

void
inspect_docoid_path(Spec_ptr this_spec)
{
	memset((char *)&docoid_path_record, 0, sizeof(docoid_path_record));
	memcpy(docoid_path_record.dockey, oid_container_record.dockey,
	       sizeof(docoid_path_record.dockey));
	
	if (-1==isread(docoid_path_fd, (char *)&docoid_path_record, ISEQUAL)) {
		switch (iserrno) {
		      case ENOREC:
			// oid doesn't have a file!
			this_spec->filename = "";
			return;
		      default:
			pisamerr("isread", oid_container_rootname);
			break;
		}
	}

	if (docoid_path_record.filepath[MAX_KEY_LEN-1] == '\0') {
		// strip padding.
		this_spec->filename = (char *)docoid_path_record.filepath;
	} else {
		this_spec->filename.set((unsigned char *)
					docoid_path_record.filepath,
					isreclen-offsetof(Table_docoid_path,
							 filepath));
	}
}

/*
 * Print ISAM error message. Some error codes (well, one)
 * are special cased to give more "helpful" messages.
 */

void
pisamerr(const char *func, const char *name)
{
	const char *msg = _tt_isstrerror(iserrno);
	if (msg) {
		fprintf(stderr,"ttdbck: %s(\"%s\"): %s\n", func, name,
			msg);
	} else {
		fprintf(stderr,"ttdbck: %s(\"%s\"): %d\n", func, name,
			iserrno);
	}
	switch (iserrno) {
	      case EBADFILE:
		fprintf(stderr, "%s",
			catgets(_ttcatd, 6, 6,
				"ttdbck: try 'ttdbck -I'.\n"));
		break;
	      default:
		break;
	}
}

void
check_if_file(Spec_ptr this_spec)
{
	memset((char *)&docoid_path_record, '\0', sizeof(docoid_path_record));
	memcpy((char *)docoid_path_record.dockey,
	       (char *)(*this_spec->key), OID_KEY_LENGTH);

        if (-1==isread(docoid_path_fd, (char *)&docoid_path_record, ISEQUAL)) {
		switch (iserrno) {
		      case ENOREC:
			break;
		      default:
			pisamerr("isread", oid_container_rootname);
			break;
		}
		this_spec->is_filespec = 0;
		return;
        }

	// This is the docoid for a file. Set the filename in the spec
	// so the -f option will select the docoid for the file as well.

	this_spec->is_filespec = 1;

	if (docoid_path_record.filepath[MAX_KEY_LEN-1] == '\0') {
		// strip padding.
		this_spec->filename = (char *)docoid_path_record.filepath;
	} else {
		this_spec->filename.set((unsigned char *)
					docoid_path_record.filepath,
					isreclen-offsetof(Table_docoid_path,
							 filepath));
	}
}
