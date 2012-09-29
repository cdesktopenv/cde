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
//%%  $TOG: mp_type_comp.C /main/4 1999/10/14 18:37:58 mgreess $ 			 				
/*
 *
 * mp_type_comp.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * ToolTalk type compiler. Performs syntax and semantics checks on
 * type input file and then writes out the type table in xdr format
 * or in Classing Engine format.
 */

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#if defined(linux)
#include <unistd.h>
#endif
#if defined(sgi) || defined(CSRG_BASED)
#include <getopt.h>
#endif
#if defined(USL) || defined(__uxp__)
#include "tt_options.h"
#if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
extern "C" int getopt(int, char *const *, const char *);
#endif
#endif
#include "mp/mp_global.h"
#include "mp/mp_mp.h"
#include "mp_otype.h"
#include "mp_ptype.h"
#include "mp_types_table.h"
#include "mp_typedb.h"
#include "util/copyright.h"
#include "util/tt_global_env.h"
#include "util/tt_xdr_version.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"

#define _TT_DBCLIENT_SIDE
#include "db/db_server.h"

#if defined(ultrix)
extern "C" void xdrstdio_create(XDR *, FILE *, enum xdr_op);
#endif

typedef void (*cmd_fn)();

TT_INSERT_COPYRIGHT

#ifdef OPT_PATCH
static char PatchID[] = "Patch Id: 100626_03.";
static int Patch_ID100626_03;
#endif

_Tt_string 		ifile;
_Tt_typedbLevel		cedb = TypedbNone;
_Tt_string 		ofile;
_Tt_string_list_ptr	cargs;
int			option_xdr = 1;
int			option_remap_ptypes = 1;
_Tt_string		cpp_options("");

void
print_usage_and_exit()
{
	_tt_syslog(stderr, LOG_ERR, "%s",
		   catgets(_ttcatd, 4, 2,
"Usage:\n"
"tt_type_comp [-s] [-d db] [-mM] source_file\n"
"tt_type_comp [-s] [-d db] -r type ...\n"
"-M	merge source types into specified database, not updating existing types\n"
"-m	merge, but update existing types.  Default.\n"
"-r	remove source types from the specified database\n"
"-d db	database to operate on. One of: user, system, or network. Default: user\n"
"-G	perform garbage collection on the ToolTalk database server.\n"
"\n"
"tt_type_comp [-sE] -p|O|P [-d db]\n"
"tt_type_comp [-s]  -p|O|P compiled_file\n"
"-O	enumerate on stdout the names of all otypes read\n"
"-P	enumerate on stdout the names of all ptypes read\n"
"-p	pretty-print on stdout all the ToolTalk types read\n"
"-E	use the Classing Engine database(s) instead of the XDR database(s)\n"
"-d db	database to read from. One of: user, system, or network. Default: all\n"
"\n"
"tt_type_comp [-s] -x [-o compiled_file] source_file\n"
"-x	compile types from source_file (or stdin, if file is \"-\")\n"
"-o	write compiled types to compiled_file (or stdout, if file is \"-\")\n"
"	Default: source_file.xdr, or \"types.xdr\" if source is stdin\n"
"\n"
"tt_type_comp [-hv]\n"
"-v	print out version number\n"
"-h	print out this message\n"
"-s	do not print out any status messages.\n"
"\n"
"These cpp options will be passed through:\n"
"        -undef -Dname -Idirectory -Uname -Ydirectory"));
	exit(1);
}


void
read_types(_Tt_string file, _Tt_typedb_ptr &db)
{
	_Tt_parse_status result;
	_Tt_types_table *table;
	if (file == "-") {
		table = new _Tt_types_table(stdin, result);
	} else {
		table = new _Tt_types_table(file, result);
	}

	if (result == _TT_PARSE_OK) {
		db = new _Tt_typedb();

		if (table->check_semantics() != _TT_PARSE_OK) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 3,
					   "Semantic error in types file"));
			exit(2);
		}
		db->ptable = table->ptypes();
		db->otable = table->otypes();
	} else {
		_Tt_string msg =  catgets(_ttcatd, 4, 4,
					  "Not a valid ToolTalk types file" );
		if (file.len() > 0) {
			msg = msg.cat( ": " ).cat( file );
		}
		_tt_syslog(stderr, LOG_ERR, msg);
		exit(2);
	}
}


void
f_ce_print()
{
	_Tt_typedb_ptr	db;

	db = new _Tt_typedb();
	if (db->init_ce(cedb) != TT_OK) {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 5,
				   "Cannot read any ToolTalk types "
				   "from Classing Engine database"));
		exit(3);
	}
	db->pretty_print(stdout);
}

void
merge(int overwrite)
{

	int				status_ok = 1;
	int				exists;
	_Tt_typedb_ptr			db;
	_Tt_ptype_table_cursor		db_ptypes;
	_Tt_otype_table_cursor		db_otypes;
	_Tt_typedb_ptr			xdb;
	_Tt_ptype_ptr			pt;
	_Tt_otype_ptr			ot;
	Tt_status			st = TT_OK;

	if (cedb == TypedbNone) {
		cedb = TypedbUser;
	}

	if (! option_xdr) {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 6,
				   "Merging Classing Engine tables is no "
				   "longer supported"));
		exit(1);
	}

	xdb = new _Tt_typedb();
	read_types(ifile, db);

	if ((st=xdb->init_xdr(cedb)) != TT_OK) {
		// if TT_ERR_DBEXIST is returned from init_ce
		// we continue with the merge. It just means
		// we're starting up in an environment where
		// no tooltalk namespaces exist in any of the
		// Classing Engine databases.
		if (st != TT_ERR_DBEXIST) {
			if (st == TT_ERR_NO_MATCH) {
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 4, 7,
						   "Cannot read types in %s data"
						   "base - version mismatch"),
					   _Tt_typedb::level_name(cedb));
			} else {
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 4, 8,
						   "Cannot read types in %s data"
						   "base"),
					   _Tt_typedb::level_name(cedb));
			}
			/*
			 * The most common way for this to fail seems to be
			 * to not have OPENWINHOME set. Suggest this
			 * to the user.
			 */
			if (0==getenv("OPENWINHOME")) {
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 4, 9,
						   "$OPENWINHOME not set"));
			}
			exit(3);
		}
	}

	if (! xdb->begin_write(cedb)) {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 10,
				   "Cannot initialize %s database for writing"),
			   _Tt_typedb::level_name(cedb));
		exit(3);
	}

	db_otypes.reset(db->otable);
	while (db_otypes.next()) {
		ot = xdb->otable->lookup(db_otypes->otid());
		exists = (! ot.is_null());
		if (exists) {
			if (! overwrite) {
				continue;
			}
			if (! xdb->remove_otype(db_otypes->otid())) {
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 4, 11,
						   "Could not remove old "
						   "definition for %s"),
					   (char *)db_otypes->otid());
				xdb->abort_write();
				exit(3);
			}
		}
		if (! _tt_global->silent) {
			printf( "%s %s...\n",
			       	(exists)
				      ? catgets(_ttcatd, 4, 12, "Overwriting")
				      : catgets(_ttcatd, 4, 13, "Writing"),
				(char *)db_otypes->otid());
		}
		if (! xdb->insert(*db_otypes)) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 14, "Could not add "
					   "new definition for %s"),
				   (char *)db_otypes->otid());
			xdb->abort_write();
			exit(3);
		}
	}

	db_ptypes.reset(db->ptable);
	while (db_ptypes.next()) {
		pt = xdb->ptable->lookup(db_ptypes->ptid());
		exists = (! pt.is_null());
		if (exists) {
			if (! overwrite) {
				continue;
			}
			if (! xdb->remove_ptype(db_ptypes->ptid())) {
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 4, 15,
						   "Could not remove old "
						   "definition for %s"),
					   (char *)db_ptypes->ptid());
				xdb->abort_write();
			}
		}
		if (! _tt_global->silent) {
			printf("%s %s...\n",
			       (exists)
				      ? catgets(_ttcatd, 4, 16, "Overwriting")
				      : catgets(_ttcatd, 4, 17, "Writing"),
			       (char *)db_ptypes->ptid());
		}
		if (! xdb->insert(*db_ptypes)) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 18, "Could not add "
					   "new definition for %s"),
				   (char *)db_ptypes->ptid());
			xdb->abort_write();
		}
	}

	if (! xdb->end_write()) {
		// diagnostic emitted by ::end_write()
		exit(3);
	}
}

void
f_merge_overwrite()
{
	merge(1);
}


void
f_merge_no_overwrite()
{
	merge(0);
}


void
f_list_types(int otypes)
{
	_Tt_typedb_ptr		db;
	Tt_status		status;
	int			checkOW = 0;

	if (cedb == TypedbNone) {
		cedb = TypedbAll;
	}
	db = new _Tt_typedb();
	if (option_xdr) {
		if (ifile.len() > 0) {
			if (ifile == "-") {
				status = db->init_xdr( stdin );
			} else {
				status = db->init_xdr( ifile );
			}
		} else {
			status = db->init_xdr( cedb );
		}
	} else {
		status = db->init_ce( cedb );
	}
	if (status != TT_OK) {
		if (status == TT_ERR_NO_MATCH) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 19, "Version mismatch "
					   "in compiled types"));
		} else {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 20, "Cannot read types "
					   "in database"));
		}
		//
		// The most common way for this to fail seems to be
		// to not have OPENWINHOME set. Suggest this
		// to the user.
		//
		if (0==getenv("OPENWINHOME")) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 21,
					   "$OPENWINHOME not set"));
		}
		exit(3);
	}
	if (otypes) {
		_Tt_otype_table_cursor	db_otypes;
		db_otypes.reset(db->otable);
		while (db_otypes.next()) {
			printf("%s\n", (char *)db_otypes->otid());
		}
	} else {
		_Tt_ptype_table_cursor	db_ptypes;
		db_ptypes.reset(db->ptable);
		while (db_ptypes.next()) {
			printf("%s\n", (char *)db_ptypes->ptid());
		}
	}
}

void
f_list_ptypes()
{
	f_list_types( 0 );
}

void
f_list_otypes()
{
	f_list_types( 1 );
}

void
f_remove_types()
{
	_Tt_string_list_cursor	argc;
	_Tt_typedb_ptr		db;
	_Tt_otype_ptr		ot;
	_Tt_ptype_ptr		pt;
	int			db_changed = 0;
	Tt_status		err;

	if (cedb == TypedbNone) {
		cedb = TypedbUser;
	}
	db = new _Tt_typedb();
	if ((option_xdr && (err = db->init_xdr(cedb)) != TT_OK) ||
	    (!option_xdr && (err = db->init_ce(cedb)) != TT_OK)) {
		if (err == TT_ERR_NO_MATCH) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 22,
					   "Cannot read types in %s data"
					   "base - version mismatch"),
				   _Tt_typedb::level_name(cedb));
		} else {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 23,
					   "Cannot read types in %s database"),
				   _Tt_typedb::level_name(cedb));
		}
		/*
 		 * The most common way for this to fail seems to be
		 * to not have OPENWINHOME set. Suggest this
		 * to the user.
		 */
		if (0==getenv("OPENWINHOME")) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 24,
					   "$OPENWINHOME not set"));
		}
		exit(3);
	}

	if (! db->begin_write(cedb)) {
		// diagnostic emitted
		exit(3);
	}

	argc.reset(cargs);
	while (argc.next()) {
		pt = db->ptable->lookup(*argc);
		if (pt.is_null()) {
			ot = db->otable->lookup(*argc);
			if (! ot.is_null()) {
				if (! _tt_global->silent) {
					printf(catgets(_ttcatd, 4, 25,
						       "Removing otype %s\n"),
					       (char *)(*argc));
				}
				db_changed = 1;
				db->remove_otype(*argc);
			}
		} else {
			if (! _tt_global->silent) {
				printf(catgets(_ttcatd, 4, 26,
					       "Removing ptype %s\n"),
				       (char *)(*argc));
			}
			db->remove_ptype(*argc);
			db_changed = 1;
		}
	}
	if (db_changed ) {
		// write out changes
		if (! db->end_write()) {
			exit(3);
		}
	} else {
		db->abort_write();
	}
}


void
f_xdr_file()
{
        _Tt_typedb_ptr  db;

        if (ofile.len() == 0) {
                if (ifile == "-") {
                        ofile = "types.xdr";
                } else {
                        ofile = ifile.cat(".xdr");
                }
        }
        read_types(ifile, db);

	Tt_status status;
        if (ofile == "-") {
		status = db->write( stdout );
        } else {
		status = db->write( ofile );
	}
	if (status != TT_OK) {
		exit( 3 );
	}
        if (ofile != "-") {
		_Tt_typedb::send_saved( ofile );
                printf(catgets(_ttcatd, 4, 27, "output written to %s\n"),
		       (char *)ofile);
        }
}
 

void
f_xdr_print()
{
	_Tt_typedb_ptr	db;
	Tt_status	status;
	
	db = new _Tt_typedb();
	if (ifile.len() != 0) {
		if (ifile == "-") {
			status = db->init_xdr( stdin );
		} else {
			status = db->init_xdr( ifile );
		}
	} else {
		if (cedb == TypedbNone) {
			cedb = TypedbAll;
		}
		status = db->init_xdr(cedb);
	}
	if (status != TT_OK) {
		// diagnostic emitted
		exit(3);
	}
	if (!db.is_null()) {
		db->pretty_print(stdout);
	}
}

//
// This sends messages to the 'default' ToolTalk files.
// This forces an attempt to contact the sessions registered for
// each file, if they are dead, then a deleteSession() is sent
// to clear out (garbage collect) the information for that dead
// session. Dead sessions can occur in the dbserver if the system
// that had registered died without un-registering with dbserver
// for a file.
//
void
f_garbage_collect()
{
	_Tt_db_client			dbClient;
        _Tt_string			sessionId;

	if (dbClient.getConnectionResults() == TT_DB_OK) {

		_Tt_string_list		*sessions;
		sessions = dbClient.get_all_sessions();

		if (sessions != NULL && sessions->count() > 0) {
	    
			// Delete the list of sessions that are dead.
			do {
				Tt_status	ttstatus;

				sessionId = sessions->top();
				if ((ttstatus
				     = tt_default_session_set(sessionId))
				    != TT_OK) {
					dbClient.delete_session(sessionId);
				}
				sessions->pop();
			} while(sessions->count() > 0);
		}
		dbClient.garbage_collect_in_server();
	}
	return;
}

void
process_args(int argc, char **argv)
{
	extern char 	*optarg;
	extern int	optind;
	extern int	opterr;
	int		c;
	int		args_left;
	cmd_fn		fn;
	int		fn_set = 0;

	// default function is to merge types
	fn = f_merge_overwrite;

	// Need to parse out the cpp options specially because they
	// don't work with getopt
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' &&
		    (argv[i][1] == 'u' || argv[i][1] == 'D' ||
		     argv[i][1] == 'I' || argv[i][1] == 'U' ||
		     argv[i][1] == 'Y')) {
			if (argv[i][1] == 'u' &&
			    strcmp(argv[i], "-undef") != 0) {
				print_usage_and_exit();
			}
			cpp_options = cpp_options.cat(argv[i]).cat(" ");
			// getopt stops processing on an empty string, so
			// need a fake option for below
			argv[i] = "-Y";
		}
	}
	int need_file = 1;
	int only_one_input = 0;
	while ((c = getopt(argc, argv, "XEGOPsd:hmMo:prvxY")) != -1) {
		switch (c) {
		      case 'X':
			option_xdr = 1;
			break;
		      case 'E':
			option_xdr = 0;
			break;
		      case 'O':
			fn_set++;
			fn = f_list_otypes;
			need_file = 0;
			only_one_input = 1;
			break;
		      case 'P':
			fn_set++;
			fn = f_list_ptypes;
			need_file = 0;
			only_one_input = 1;
			break;
		      case 's':
			_tt_global->silent = 1;
			break;
		      case 'd':
			cedb = _Tt_typedb::level( optarg );
			if (cedb == TypedbNone) {
				_tt_syslog( stderr, LOG_ERR,
					    catgets(_ttcatd, 4, 28,
						    "Invalid database: %s"),
					    optarg );
				print_usage_and_exit();
			}
			break;
		      case 'h':
			print_usage_and_exit();
			break;
		      case 'm':
			fn_set++;
			fn = f_merge_overwrite;
			break;
		      case 'M':
			fn_set++;
			fn = f_merge_no_overwrite;
			break;
		      case 'o':
			ofile = optarg;
			break;
		      case 'p':
			fn_set++;
			need_file = 0;
			only_one_input = 1;
			if (option_xdr) {
				fn = f_xdr_print;
			} else {
				fn = f_ce_print;
			}
			break;
		      case 'r':
			fn_set++;
			fn = f_remove_types;
			break;
		      case 'v':
			_TT_PRINT_VERSIONS((char *)_tt_global->progname)
			exit(0);
		      case 'x':
			fn_set++;
			fn = f_xdr_file;
			break;
		      case 'Y':
			// A cpp option was handled, ignore this
			break;
		      case 'G':
			// Garbage collect.
			need_file = 0;
			fn = f_garbage_collect;
			break;
		      case '?':
		      default:
			print_usage_and_exit();
			break;
		}
	}
	if (fn_set > 1) {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 29,
				   "Specify only one of the options "
				   "-O -P -m -M -p -r -x"));
		print_usage_and_exit();
	}
	//
	// Extra args are always either a filename or types to remove
	//
	ifile = argv[optind];
	if (ifile.len() == 0) {
		if (need_file) {
			print_usage_and_exit();
		}
	} else {
		if (only_one_input && (cedb != TypedbNone)) {
			print_usage_and_exit();
		}
	}
	if ((ofile.len() > 0) && (fn != f_xdr_file)) {
		print_usage_and_exit();
	}
	args_left = optind;
	cargs = new _Tt_string_list();
	while (argv[args_left]) {
		cargs->append(_Tt_string(argv[args_left++]));
	}
	(*fn)();
}

int main(int argc, char **argv)
{
	_tt_global = new _Tt_global();
	_tt_global->progname = argv[0];
	setlocale( LC_ALL, "" );
	_tt_openlog( _tt_global->progname, LOG_NOWAIT, 0 );
	process_args(argc, argv);
	return((int) 0);
}
