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
//%%  $XConsortium: mp_types_table.C /main/3 1995/10/20 16:38:58 rswiston $ 			 				
/*
 * Tool Talk Message Passer (PM) - mp_types_table.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * This file contains the definition for the class _Tt_types_table which implements a
 * hash table of otype and ptype descriptors.
 *
 */

#include "tt_options.h"
#include "mp/mp_arg.h"
#include "mp_types_table.h"
#include "util/tt_global_env.h"
#include "util/tt_iostream.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#if defined(OPT_BUG_UW_2) || defined(OPT_BUG_UXP)
extern "C" {
#ifndef __uxp__
int   putenv(char *);
#endif
FILE *popen(const char *, const char *);
FILE *fdopen(int, const char *);
int   pclose(FILE *);
}
#endif

#define  TT_TYPE_TABLE_BUCKETS  19

static int use_cpp;
static int cppline(const char *line);
static void docppline(const char *line, int &lineno, _Tt_string &fname);

/*
 *  Parse the type descriptions in file stream 'infs', and construct the hash
 *  table of otypes and ptypes.
 */

_Tt_types_table::
_Tt_types_table(FILE *infs, _Tt_parse_status& result)
{
	ptable = new _Tt_ptype_table(_tt_ptype_ptid,TT_TYPE_TABLE_BUCKETS);
	ptc.reset(ptable);
	otable = new _Tt_otype_table(_tt_otype_otid, TT_TYPE_TABLE_BUCKETS);
	otc.reset(otable);
	tmp_otype = new _Tt_otype();
	tmp_ptype = new _Tt_ptype();
	tmp_otidl = new _Tt_string_list();
	tmp_sigl = new _Tt_signature_list();
	tmp_sig = new _Tt_signature();
	tmp_arg = new _Tt_arg();
	tmp_propname = tmp_propvalue = 0;
	yyin = infs;
	result = parse_yyin();
}

/* 
 *  _Tt_types_table - parse the type descriptions in the file 'path', and
 * construct the hash table of in-memory otype and ptype descriptors.
 */
_Tt_types_table::
_Tt_types_table(_Tt_string path, _Tt_parse_status& result)
{
	ptable = new _Tt_ptype_table(_tt_ptype_ptid,TT_TYPE_TABLE_BUCKETS);
	ptc.reset(ptable);
	otable = new _Tt_otype_table(_tt_otype_otid,TT_TYPE_TABLE_BUCKETS);
	otc.reset(otable);
	tmp_otype = new _Tt_otype();
	tmp_ptype = new _Tt_ptype();
	tmp_otidl = new _Tt_string_list();
	tmp_sigl = new _Tt_signature_list();
	tmp_sig = new _Tt_signature();
	tmp_arg = new _Tt_arg();
	tmp_propname = tmp_propvalue = 0;
	has_contexts = 0;
	result = parse_path(path);
}

_Tt_otype_table_ptr & _Tt_types_table::
otypes()
{
	return otable;
}

_Tt_ptype_table_ptr & _Tt_types_table::
ptypes()
{
	return ptable;
}

_Tt_parse_status _Tt_types_table::
parse_path(_Tt_string path)
{
	int fd;
	// If cpp cannot be found, don\'t use it.
	// This may cause trouble, but it\'s the best thing to try.
	// A warning will be issued if cpp is not around.
	use_cpp = (-1 != access(OPT_CPP_PATH,X_OK));

	_file_name = path;
	if ((fd = open(path, O_RDONLY)) == -1) {
		_tt_syslog( stderr, LOG_ERR, "%s: %m",
			    path.operator const char *());
		return _TT_PARSE_ERROR;
	} else {
		if (use_cpp) {
			close(fd);
		}
	}

	if (use_cpp) {
		_Tt_string deps =_file_name.cat(".deps");
		unlink((char *)deps);
		deps = _Tt_string("SUNPRO_DEPENDENCIES=").cat(deps).cat(" ");
		putenv((char *)deps);
		char command[MAXPATHLEN+32];
		sprintf(command,
			OPT_CPP_PATH " " OPT_CPP_OPTIONS " %s %s",
			(char *)cpp_options,
			(char *)_file_name);
		yyin = popen(command, "r");/* open cpp stream for reading */
		if (!yyin) {
			_tt_syslog(stderr, LOG_ERR, "popen(\"%s\"): %m",
				   command );
			return _TT_PARSE_ERROR;
		}
	} else {
		_tt_syslog(stderr, LOG_WARNING,
			   catgets(_ttcatd, 4, 30,
				   "no preprocessing done because: %s: %s"),
			   OPT_CPP_PATH, strerror(ENOENT));
		yyin = fdopen(fd,"r");
	}
				
	return parse_yyin();
}

/*
 *  Parse the file stream that yyin points to
 */

_Tt_parse_status _Tt_types_table::
parse_yyin()
{
	_yylineno = -1;
	int errorp = yyparse();
	if ((use_cpp && pclose(yyin) != 0) ||
	    (!use_cpp && fclose(yyin) != 0))
	{
		return _TT_PARSE_ERROR;
	}
	yyin = 0;
	if (errorp) {
		reset();
		return _TT_PARSE_ERROR;
	} else {
		return _TT_PARSE_OK;
	}
}

/*
 *  check_semantics - checks otypes and ptypes
 *  If no error found, returns _TT_PARSE_OK, otherwise returns _TT_PARSE_ERROR.
 */

_Tt_parse_status _Tt_types_table::
check_semantics()
{
	_Tt_parse_status oresult, presult;

	oresult = check_otypes();
	presult = check_ptypes();
	if (oresult == _TT_PARSE_ERROR || presult == _TT_PARSE_ERROR) {
		return _TT_PARSE_ERROR;
	} else {
		return _TT_PARSE_OK;
	}
}

/*
 * Check each otype
 */
_Tt_parse_status _Tt_types_table::
check_otypes()
{
	_Tt_otype_table_cursor	 otypec(otable);
	_Tt_parse_status result = _TT_PARSE_OK;

	while (otypec.next()) {
		if (check_otype(*otypec) == _TT_PARSE_ERROR) {
			result = _TT_PARSE_ERROR;
		}
	}
	return result;
}

/*
 * Checks an otype:
 *	- Ptypes referenced in otypes are defined.
 *	- Every inherited signature exists in the parent
 *	- The inheritance hierarchy is acyclic.
 */
_Tt_parse_status _Tt_types_table::
check_otype(_Tt_otype_ptr otype)
{
	_Tt_ptype_ptr ptype;
	_Tt_string ptid;
	_Tt_parse_status result = _TT_PARSE_OK;
	_Tt_otype_ptr anc_otype;
	_Tt_string super;

	if (otype->visitp < 0) {
		// already processed
		return _TT_PARSE_OK;
	}
	if (otype->visitp > 0) {
		// this otype is in progress, we have detected a loop
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 31,
				   "otype inheritance cycle involving %s"),
			   (char *) otype->otid());
		return _TT_PARSE_ERROR;
	}
	otype->visitp = 1;  // in progress
	// Check all of the parents of this otype
	_Tt_string_list_cursor anc_name(otype->_ancestors);
	while (anc_name.next()) {
		anc_otype = otable->lookup(*anc_name);
		if (anc_otype.is_null()) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 32, "ancestor %s of "
					   "otype %s does not exist"),
				   (char *)(*anc_name), (char *)otype->otid());
			result = _TT_PARSE_ERROR;
		} else {
			if (check_otype(anc_otype) == _TT_PARSE_ERROR) {
				result = _TT_PARSE_ERROR;
			}
		}
	}
	// Check the signatures
	_Tt_signature_list_cursor s1(otype->_osigs);
	int handles = 0;
	while (1) {
		// step through the observer and handler signatures
		if (! s1.next()) {
			if (! handles) {
				s1.reset(otype->_hsigs);
				handles = 1;
				if (! s1.next()) {
					break;
				}
			} else {
				break;
			}
		}
		ptid = s1->ptid();
		if (ptid.len() != 0) {
			// a normal signature (not purely inherited)
			ptype = ptable->lookup(ptid);
			if (ptype.is_null()) {
				_tt_syslog(stderr, LOG_ERR,
					   catgets(_ttcatd, 4, 33,
						   "ptype %s does not exist, "
						   "but was named as implementor"
						   "by otype %s"),
					   (char *)ptid,
					   (char *)otype->otid());
				s1->print(stdout);
				result = _TT_PARSE_ERROR;
			} else {
				if (handles) {
					ptype->append_hsig(*s1, s1->category());
				} else {
					ptype->append_osig(*s1);
				}
			}
		}
		super = s1->super_otid();
		if (super.len() != 0) {
			// Check inheritance of this signature from parent
			anc_name.reset(otype->_ancestors);
			while (anc_name.next()) {
				if (*anc_name == super) {
					break;
				}
			}
			if (anc_name.is_valid()) {
				// super otype is one of our parents
				// now look in the parent for a matching sig
				if (check_super_sig(otype, *s1, super, handles)
				    == _TT_PARSE_ERROR) {
					result = _TT_PARSE_ERROR;
				}
			}
		}
		if (handles) {
			/* Handlers with same sigs but different ptypes */
			_Tt_signature_list_cursor s2(s1);
			while (s2.next()) {
				if (s1->is_same_method(*s2)) {
					if (ptid == s2->ptid()) {
						_tt_syslog(stderr, LOG_ERR,
							   catgets(_ttcatd, 4, 34, "multiple handlers defined in otype %s"),
							   (char *)otype->_otid);
						s1->print(stdout);
						s2->print(stdout);
					}
				}
			}
		}
	}
	otype->visitp = -1;   // signifies completed
	return(result);
}

/* 
 * Check that the given signature exists in the supertype
 */
_Tt_parse_status _Tt_types_table::
check_super_sig(_Tt_otype_ptr otype, _Tt_signature_ptr sig, _Tt_string super,
		int handles)
{
	_Tt_otype_ptr super_otype = otable->lookup(super);
	if (super_otype.is_null()) {
		// the otype doesn't exist, nothing to check
		return _TT_PARSE_ERROR;
	}
	_Tt_signature_list_cursor super_sigc;
	if (handles) {
		super_sigc.reset(super_otype->_hsigs);
	} else {
		super_sigc.reset(super_otype->_osigs);
	}
	while (super_sigc.next()) {
		if (sig->is_same_method(*super_sigc)) {
			break;
		}
	}
	if (! super_sigc.is_valid()) {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 35, "inherited signature in "
				   "otype %s does not exist in parent"),
				   (char *)otype->otid());
		sig->print(stdout);
		return _TT_PARSE_ERROR;
	}
	if (sig->ptid().len() == 0) {
		// pure inheritance
		_Tt_signature_ptr super_sig = *super_sigc;	
		_Tt_string super_ptid = super_sig->ptid();
		_Tt_ptype_ptr super_ptype = ptable->lookup(super_ptid);
		if (super_ptype.is_null()) {
			// super ptype doesn't exist
			return _TT_PARSE_ERROR;
		}
		// replace the fields of the signature with the super sig
		sig->set_scope(super_sig->scope());
		sig->set_message_class(super_sig->message_class());
		sig->set_op(super_sig->op());
		sig->set_reliability(super_sig->reliability());
		sig->set_opnum(super_sig->opnum());
		sig->set_ptid(super_sig->ptid());
		// create a duplicated of the parent signature and
		//   replace the otid
		_Tt_signature_ptr new_sig(new _Tt_signature(super_sig));
		new_sig->set_otid(otype->otid());
		new_sig->set_super_otid(super);
		// install this signature on the super ptype
		if (handles) {
			super_ptype->append_hsig(new_sig, new_sig->category());
		} else {
			super_ptype->append_osig(new_sig);
		}
	}
	return _TT_PARSE_OK;
}

/*
 * Checks ptypes:
 *	- Each ptype handler function has a unique signature.
 */
_Tt_parse_status _Tt_types_table::
check_ptypes()
{
	_Tt_parse_status result = _TT_PARSE_OK;

	_Tt_ptype_table_cursor ptc1(ptable);
	while (ptc1.next()) {
		/* check for duplicated signatures */
		if (ptc1->check_semantics()) {
			result = _TT_PARSE_ERROR;
		}
		/* check for unique handler signatures between ptypes */
		_Tt_ptype_table_cursor ptc2(ptc1);
		while (ptc2.next()) {
			_Tt_signature_list_cursor s1(ptc1->hsigs());
			while (s1.next()) {
				_Tt_signature_list_cursor s2(ptc2->hsigs());
				while (s2.next()) {
					if (**s1 == **s2) {
						_tt_syslog(stderr, LOG_ERR,
							   catgets(_ttcatd, 4, 36, "two ptypes, %s and %s, with the same handler"),
							   (char *)ptc1->ptid(),
							   (char *)ptc2->ptid());
						s1->pretty_print(stdout);
					}
				}
			}
		}
	}
	return result;
}

/* 
 *  check_cycle - check for a cyclic inheritance hierarchy.  Also check
 * for uniquely defined inherited method (i.e., an inherited method M
 * cannot be from more than one ancestors whose all have M identically
 * defined); only give WARNING because user specifically indicates which
 * ancestor the inherited method comes from.  Probably needs to add a
 * flag for controlling the printing of the warning.
 */
_Tt_parse_status _Tt_types_table::
check_cycle(_Tt_otype_ptr otype, _Tt_otype **path, int top)
{
	_Tt_parse_status result = _TT_PARSE_OK;
	otype->visitp = 1;
	path[top++] = otype.c_pointer();
	_Tt_string_list_cursor anc_name(otype->_ancestors);
	while (anc_name.next()) {	/* depth-first traversal */
		_Tt_otype_ptr anc_otype = otable->lookup(*anc_name);
		if (anc_otype.is_null()) {
			_tt_syslog(stderr, LOG_ERR,
				   catgets(_ttcatd, 4, 37, "ancestor %s of "
					   "otype %s does not exist"),
				   (char *)(*anc_name), (char *)otype->otid());
			result = _TT_PARSE_ERROR;
		} else {
			for (int i = 0; i <= top-1; i++) {
				if (path[i] == anc_otype.c_pointer()) {
					fprintf(stderr, "%s",
						catgets(_ttcatd, 4, 38, "cyclic otype inheritance hierarchy -\n  {"));
					for (int j = i; j <= top-1; j++) {
						fprintf(stderr,"  %s", (char *) path[j]->otid());
					}
					fprintf(stderr,"  %s  }\n",
					       (char *) anc_otype->otid());
					result = _TT_PARSE_ERROR;
				}
			}
			if (!anc_otype->visitp) {
				result = (_Tt_parse_status)
					 (result +
					  check_cycle(anc_otype, path, top));
			}
		}
			
	}
	return result;
}

void _Tt_types_table::
insert_otype(_Tt_otype_ptr &type)
{
	if (otable->lookup(type->otid()).is_null()) {
		otable->insert(type);
	} else {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 39,
				   "duplicated definitions of otype %s"),
			   (char *)type->otid());
	}
}

_Tt_otype_ptr _Tt_types_table::
lookup_otype(_Tt_string otid) const
{
	return otable->lookup(otid);
}

int _Tt_types_table::
next_otype(_Tt_otype_ptr &o)
{
	if (otc.next()) {
		o = *otc;
		return(1);
	} else {
		otc.reset(otable);
		return(0);
	}
}

void _Tt_types_table::
reset_otype_cursor()
{
	otc.reset(otable);
}

void _Tt_types_table::
insert_ptype(_Tt_ptype_ptr &type)
{
	if (ptable->lookup(type->ptid()).is_null()) {
		ptable->insert(type);
	} else {
		_tt_syslog(stderr, LOG_ERR,
			   catgets(_ttcatd, 4, 40,
				   "duplicated definitions of ptype %s"),
			   (char *)type->ptid());
	}
}

_Tt_ptype_ptr _Tt_types_table::
lookup_ptype(_Tt_string ptid) const
{
	return ptable->lookup(ptid);
}

int _Tt_types_table::
next_ptype(_Tt_ptype_ptr &p)
{
	if (ptc.next()) {
		p = *ptc;
		return(1);
	} else {
		ptc.reset(ptable);
		return(0);
	}
}

void _Tt_types_table::
reset_ptype_cursor()
{
	ptc.reset(ptable);
}

void _Tt_types_table::
print(FILE *fs) const
{
	otable->print(_tt_otype_print, fs);
	ptable->print(_tt_ptype_print, fs);
}

void _Tt_types_table::
reset()
{
	
	ptable = new _Tt_ptype_table(_tt_ptype_ptid,TT_TYPE_TABLE_BUCKETS);
	ptc.reset(ptable);
	otable = new _Tt_otype_table(_tt_otype_otid,TT_TYPE_TABLE_BUCKETS);
	otc.reset(otable);
	reset_tmps();
}

void _Tt_types_table::
reset_tmps()
{
	tmp_otype = new _Tt_otype();
	tmp_ptype = new _Tt_ptype();
	tmp_sigl = new _Tt_signature_list();
	tmp_sig = new _Tt_signature();
	tmp_arg = new _Tt_arg();
	tmp_propname = 0;
	tmp_propvalue = 0;
}

void _Tt_types_table::
set_tmp_propname(_Tt_string name)
{
	tmp_propname = name;
}

void _Tt_types_table::
append_sig(_Tt_signature_ptr &sig)
{
	tmp_sigl->append(sig);
}

void _Tt_types_table::
yyerror(const char *s)
{
	if (_file_name.len() > 0) {
		fprintf(stderr,"\"%s\", line %d: %s\n",
			(char *)_file_name,
			_yylineno, s);
	} else {
		fprintf(stderr,"\"%s\", line %d: %s\n",
			"<standard input>",
			_yylineno, s);
	}
}

int _Tt_types_table::
input()
{
	int c;
	// very first line is a special case.  Pretend it was preceded by`
	// a newline.

	if (_yylineno == -1) {
		c = '\n';
		_yylineno = 0;
	} else {
		c = getc(yyin);
	}
	while (c == '\n') {
		_yylineno++;
		c = getc(yyin);
		while (c == '#') {
			char buf[MAXPATHLEN+100];
			// Apparently fgets can't be used because ungetc is
			int i;
			for (i=0;i<sizeof(buf)-1;i++){
				buf[i] = getc(yyin);
				if (buf[i] == '\n') break;
			}
			buf[i] = 0;
			docppline(buf, _yylineno, _file_name);
			// cpp line directives are invisible
			c = getc(yyin);
		}
	}
	return (c == EOF) ? 0 : c;
}

void _Tt_types_table::
unput(int c)
{
	if (c == '\n') {
		_yylineno--;
	}
	ungetc(c, yyin);
}

void _Tt_types_table::
output(int /* c */)
{
/*	putc(c, yyout);  do nothing */
}

void _Tt_types_table::
skip_c_comments()
{
	int c;

	for (;;) {
		while ((c=input())!='*') {
			;
		}
		if ((c=input())!='/') {
			unput(c);
		} else {
			break;
		}
	}
}
	
void _Tt_types_table::
skip_cplusplus_comments()
{
	int c;
	int savelineno;

	// input() never returns a newline because it has # line
	// processing imbedded in it, which eats the newlines.
	// so we tell when input() has gone past a newline by watching
	// for _yylineno to change.

	savelineno = _yylineno;
	while(savelineno==_yylineno) {
		c = input();
	}
	unput(c);
}


int _Tt_types_table::
yywrap()
{
	return 1;
}

static void
docppline(const char *line, int &lineno, _Tt_string &fname)
{
	char *file;
	int num;
	char *p;

	while (isspace(*line)) {
		line++;
	}
	num = atoi(line);
	while (isdigit(*line)) {
		line++;
	}
	while (isspace(*line)) {
		line++;
	}
	if (*line != '"') {
		fname = "<Unknown, preprocessor error.>";
	}
	line++;
	file = (char *)malloc(strlen(line) + 1);
	p = file;
	while (*line && *line != '"') {
		*p++ = *line++;
	}
	*p++ = 0;
	if (*line == 0) {
		fname = "<Unknown, preprocessor error.>";
	} else {
		fname = file;
	}		
	free(file);
	lineno = num;
}
