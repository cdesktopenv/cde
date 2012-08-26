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
//%%  $XConsortium: spec.C /main/3 1995/10/20 16:26:34 rswiston $ 			 				
/*
 *
 * spec.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>

#if defined(ultrix)
#include <sys/types.h>
#endif
#include <sys/stat.h>     
#include "spec.h"
#include "options_tt.h"
#include "util/tt_gettext.h"
#include "ttdbck.h"
#include "tt_db_server_consts.h"

#if !defined(OPT_STRERROR)
// No strerror(), fake it
char *
strerror(int e)
{
	return ((e<sys_nerr) ? sys_errlist[e] : "unknown");
}
#endif

Spec::
Spec()     
{
	key = (Binkey *) 0;
	filename = "";
	type = "";
	is_filespec = 0;
	props = new Prop_table(_tt_prop_name);
	propnames = new _Tt_string_list;
}

void Spec::
add_prop_and_value(_Tt_string propname, _Tt_string value)
{
	// the reason we need to keep the list of names in 
	// propnames is that we want to preserve the order in
	// which they were encountered, so that adding them
	// back in goes smoothly.  A tt_table method that
	// returned a sorted list of keys would be nice, but
	// right now I don't have time... RFM 1/10/91

	Prop_ptr sp = props->lookup(propname);

	if (sp.is_null()) {
		sp = new Prop(propname);
		props->insert(sp);
		propnames->append(propname);
	}

	sp->_values->append(value);
}

void Spec::
print(FILE *f) const
{
	print_key(f);
	print_mand(f);
	print_props(f);
}
void Spec::
print_key(FILE *f) const
{
	fprintf(f,
		"-----------\n"
		"objkey:       ");
	key->print(f);
	fprintf(f, "\n");
}

void Spec::
print_mand(FILE *f) const
{
	if (is_filespec) {
		fprintf(f, "file: %s\n", (char *)filename);
	} else {
		fprintf(f,
			"type: %s\nfile: %s\n",
			(char *)type, (char *)filename);
	}
}

void Spec::
print_props(FILE *f) const
{
	_Tt_string_list_cursor c;
	Prop_ptr sp;
	_Tt_string_list_cursor v;
	int i;
	
	c.reset(propnames);
	while(c.next()) {
		sp = props->lookup(*c);
		v.reset(sp->_values);
		i = 0;
		while(v.next()) {
			fprintf(f,
				"prop %32s[%2d] = ",
				(char *)sp->_name, i++);
			v->print(f);
			fprintf(f,"\n");
		}
	}
}
	

//
// Called when all info for a spec has been accumulated; see if the
// spec is selected under the current selection options, diagnose
// according to diagnostic options, print according to printing
// options.
//
void Spec::
process_spec()
{
	// Possible things to go wrong.  Done as a bitmap not so much
	// to save space, but to make the determination whether any
	// bad things were found easy, by seeing if bad_flags is nonzero.

	int bad_flags = 0;
	const int BAD_NOFILE = 1;
	const int BAD_NOTYPE = 2;
	const int BAD_MULTITYPE = 4;
	const int BAD_FILE_STAT = 8;
	const int BAD_TYPE = 0x10;
	const int BAD_TYPED_FILESPEC = 0x20;

	int save_errno = 0;

	Prop_ptr sp;	
	_Tt_otype_ptr ot;
	_Tt_string_list_ptr type_list;

	// All the props have been accumulated, now we can pick out
	// the type if it's there.
	
	sp = props->lookup(_Tt_string(TT_DB_OBJECT_TYPE_PROPERTY));
	if (!sp.is_null()) {
		type_list = sp->_values;
	} else {
		type_list = new _Tt_string_list;
	}

	if (type_list->count()==1) {
		type = type_list->top();
	} else {
		type = "";
	}

	_Tt_string filehostname, filelocalpath;
	filelocalpath = filename.split(':',filehostname);

	if (opts->selecting_p()) {
		if (opts->sel_filename_p()) {
			if (!filelocalpath.sh_match(opts->sel_filename())) {
				return;
			}
		}

		if (opts->sel_type_p()) {
			if (!type.sh_match(opts->sel_type())) {
				return;
			}
		}

		if (opts->sel_objid_p()) {
			if (*key != *opts->sel_objid_key()) {
				return;
			}
		}
	}

	if (opts->diagnosing_p()) {
		if (opts->diag_badform_p()) {
			if (filename=="") {
				bad_flags |= BAD_NOFILE;
			}
			switch (type_list->count()) {
			      case 0:
				// It's OK if docoids have no type.
				if (!is_filespec) {
					bad_flags |= BAD_NOTYPE;
				}
				break;
			      case 1:
				ot = (*tdb_ptr)->otable->lookup(type);
				if (ot.is_null()) {
					bad_flags |= BAD_TYPE;
				} else if (is_filespec) {
					// docoids should not have a type,
					// it's an error if they do, even
					// if it's a valid type
					bad_flags |= BAD_TYPED_FILESPEC;
				}
				break;
			      default:
				bad_flags |= BAD_MULTITYPE;
			}
		}
				
		if (opts->diag_exist_p() && filename!="") {
			struct stat statbuf;

			// HACK: ought to check that filehostname is
			// same as localhost, but that's expensive
			// due to aliases, possibly domain-qualified
			// names, etc.
			if (-1==stat((char *)filelocalpath,&statbuf)) {
				save_errno = errno;
				bad_flags |= BAD_FILE_STAT;
			}
		}
	}

	// If the spec has a forward pointer, then it's OK for
	// it to be missing information...
        sp = props->lookup(_Tt_string(TT_DB_FORWARD_POINTER_PROPERTY));
       	if (!sp.is_null()) {
		bad_flags = 0;
	}

	// a spec is eligible for display or repair only if it passes all
	// selection options (if not, we returned earlier) and if either the
	// spec is bad somehow, or we aren't diagnosing in which case we
	// always display or repair all selected specs.

	if (bad_flags || !opts->diagnosing_p()) {
		if (opts->displaying_p() || bad_flags) {
			print_key(stdout);
		}

		if (opts->disp_mand_p()) {
			print_mand(stdout);
		}

		if (opts->disp_prop_p()) {
			print_props(stdout);
		}

		// if we are diagnosing, now is the time to print the
		// diagnostics. Note that bad_flags will be zero if
		// we aren't diagnosing, but we still must fall all
	        // way through and put the spec on the repair list.

		if ((bad_flags & BAD_NOFILE)!=0) {
			printf("%s", catgets(_ttcatd, 6, 20,
				       "Error: no file for spec.\n"));
		}
		if ((bad_flags & BAD_NOTYPE)!=0) {
			printf("%s", catgets(_ttcatd, 6, 21,
				       "Error: no type for spec.\n"));
		}
		if ((bad_flags & BAD_TYPE)!=0) {
			printf(catgets(_ttcatd, 6, 22,"Error: \"%s\" is not "
				       "an installed otype.\n"),
			       type.operator const char *());
		}
		if ((bad_flags & BAD_MULTITYPE)!=0) {
			printf("%s", catgets(_ttcatd, 6, 23,"Error: spec has multiple "
				       "values for type property.\n"));
		}
		if ((bad_flags & BAD_FILE_STAT)!=0) {
			printf("%s", catgets(_ttcatd, 6, 24,"Error: "));
			printf("%s: %s", (char *)filename, strerror(save_errno));
		}

		if ((bad_flags & BAD_TYPED_FILESPEC)!=0) {
			printf("%s", catgets(_ttcatd, 6, 26,"Error: "));
			printf("%s: internal spec for file has an otype.\n",
			       (char *)filename);
		}

		// put the spec on the list for later processing, unless
		// it is a filespec, which cannot be repaired (hope we
		// never get a bad filespec!)

		if (!is_filespec && opts->repairing_p()) {
			specs_to_repair->append(this);
		}
	}
}

