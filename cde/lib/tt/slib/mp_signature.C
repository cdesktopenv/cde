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
//%%  $TOG: mp_signature.C /main/4 1998/03/20 14:28:50 mgreess $ 			 				
/*
 *  Tool Talk Message Passer (MP) - mp_signature.cc
 *
 *  Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 *  Implementation of the _Tt_signature class representing a signature
 *  of a method or 
 *  a procedure.
 */
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED)
/*# include <g++/minmax.h>*/
#else
# include <macros.h>
#endif
#include "util/tt_global_env.h"
#include "mp/mp_global.h"
#include "mp/mp_arg.h"
#include "mp/mp_mp.h"
#include "mp_s_mp.h"
#include "mp_signature.h"
#include "mp_ce_attrs.h"
#include "mp/mp_context.h"
#include "util/tt_enumname.h"
#include "util/tt_trace.h"
#include "util/tt_string.h"

/* 
 * Signatures are used in both otype and ptype descriptions. They are
 * composed of a matching component together with a field-setting
 * component. The matching component, which is the scope, class, op, and
 * args, is matched against a message whereas the field-setting
 * component, which is the opnum, ptype, and reliability settings, sets
 * fields in the message.
 */


_Tt_signature::
_Tt_signature()
{
	_scope = TT_BOTH;
	_args = new _Tt_arg_list();
	_contexts = new _Tt_context_list;
	_message_class = TT_CLASS_UNDEFINED;
	_pattern_category = TT_CATEGORY_UNDEFINED;
	_reliability = TT_DISCARD;
	_opnum = -1;
	_op = 0;
	_ptid = 0;
	_otid = 0;
	_super_otid = 0;
	_mangled_args = 0;
	ce_entry = 0;
	_timestamp = 0;
}

_Tt_signature::
_Tt_signature(_Tt_signature_ptr sig)
{
	_scope = sig->_scope;
	_args = sig->_args;
	_mangled_args = sig->_mangled_args;
	_message_class = sig->_message_class;
	_pattern_category = sig->_pattern_category;
	_op = sig->_op;
	_reliability = sig->_reliability;
	_opnum = sig->_opnum;
	_ptid = sig->_ptid;
	_otid = sig->_otid;
	_super_otid = sig->_super_otid;
	_timestamp = sig->_timestamp;
}

bool_t _Tt_signature::
xdr(XDR *xdrs)
{
	if (! xdr_int(xdrs, (int *)&_scope)) {
		return(0);
	}
	if (! _args.xdr(xdrs)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_message_class)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_pattern_category)) {
		return(0);
	}
	if (! _op.xdr(xdrs)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_reliability)) {
		return(0);
	}
	if (! xdr_int(xdrs, &_opnum)) {
		return(0);
	}
	if (! _ptid.xdr(xdrs)) {
		return(0);
	}
	if (! _otid.xdr(xdrs)) {
		return(0);
	}
	if (! _super_otid.xdr(xdrs)) {
		return(0);
	}
	if (_tt_global->xdr_version() >= TT_CONTEXTS_XDR_VERSION) {
		if (! _contexts.xdr(xdrs)) {
			return 0;
		}
	}
	return(1);
}

_Tt_signature::
~_Tt_signature()
{
}

Tt_scope _Tt_signature::
scope() const
{
	return(_scope);
}

const _Tt_arg_list_ptr & _Tt_signature::
args() const
{
	return(_args);
}

const _Tt_context_list_ptr & _Tt_signature::
contexts() const
{
	return(_contexts);
}

Tt_class _Tt_signature::
message_class() const
{
	return(_message_class);
}

Tt_category _Tt_signature::
category() const
{
	return(_pattern_category);
}



const _Tt_string & _Tt_signature::
op() const
{
	return(_op);
}

int _Tt_signature::
opnum() const
{
	return(_opnum);
}

Tt_disposition _Tt_signature::
reliability() const
{
	return(_reliability);
}

const _Tt_string & _Tt_signature::
ptid() const
{
	return(_ptid);
}


const _Tt_string &_Tt_signature::
otid() const
{
	return(_otid);
}

_Tt_string _Tt_signature::
super_otid()
{
	return(_super_otid);
}

void _Tt_signature::
set_scope(Tt_scope s)
{
	_scope = s;
}

void _Tt_signature::
set_message_class(Tt_class mclass)
{
	_message_class = mclass;
}

void _Tt_signature::
set_op(_Tt_string op)
{
	_op = op;
}

void _Tt_signature::
set_opnum(int num)
{
	_opnum = num;
}

void _Tt_signature::
set_pattern_category(Tt_category k)
{
	_pattern_category = k;
}

void _Tt_signature::
set_ptid(_Tt_string ptid)
{
	_ptid = ptid;
}
	
void _Tt_signature::
set_otid(_Tt_string otid)
{
	_otid = otid;
}
	
void _Tt_signature::
set_super_otid(_Tt_string super_otid)
{
	_super_otid = super_otid;
}
	
void _Tt_signature::
set_reliability(Tt_disposition r)
{
	_reliability = (Tt_disposition)(_reliability | r);
}

void _Tt_signature::
append_arg(_Tt_arg_ptr &arg)
{
	_Tt_arg_list_ptr	argc;

	_args->append(arg);
	if (_mangled_args.len() == 0) {
		_mangled_args = "_";
	}
	_mangled_args = _mangled_args.cat(_tt_enumname(arg->mode())).cat(arg->type()).cat("_");
}

void _Tt_signature::
append_context(_Tt_context_ptr &context)
{
	_contexts->append_ordered(context);
}


/*
 * Matches a signature against a scope, class, op, arglist, and
 * contexts.  If the match succeeds then the caller determines which
 * fields in the signature to use.
 *
 * trace can be 0 if we are being called merely to do types
 * administration instead of message dispatching.
 */
int _Tt_signature::
match(Tt_scope theScope, Tt_class theClass, const _Tt_string &op,
      const _Tt_arg_list_ptr &args, const _Tt_string &otype,
      const _Tt_context_list_ptr &contexts, const _Tt_msg_trace *trace,
      int &was_exact) const
{
	int		score = 0;
	Tt_class	mclass;
	was_exact = 0;
	if (trace != 0) {
		*trace << "Tt_message & ";
		trace->incr_indent();
		if (_otid.len()) {
			*trace << "otype " << _otid;
		} else {
			*trace << "ptype " << _ptid;
		}
		*trace << " {\n";
		switch (_pattern_category) {
		    case TT_HANDLE_PUSH:
			*trace << "handle_push";
			break;
		    case TT_HANDLE_ROTATE:
			*trace << "handle_rotate";
			break;
		    case TT_HANDLE:
			*trace << "handle";
			break;
		    case TT_OBSERVE:
			*trace << "observe";
			break;
		}
		*trace << ": ";
		trace->print( _Tt_signature::pretty_print_, this );
		trace->decr_indent();
		*trace << "} ";
	}
	_Tt_string reason;
	if (_otid.len() > 0 && otype != _otid) {
		reason = "otype";
	} else if (_otid.len() == 0 && (scope() != theScope)) {
		reason = "Tt_scope";
	} else if (mclass = message_class(),
	    mclass != TT_CLASS_UNDEFINED && mclass != theClass) {
		reason = "Tt_class";
	} else {
		score = match(op, args, contexts, reason, was_exact);
	}
	if (trace != 0) {
		*trace << "== " << score << ";";
		if (score <= 0) {
			*trace << " /* " << reason << "*/";
		}
		*trace << "\n";
	}
	return score;
}

int _Tt_signature::
match(
	const _Tt_string &op,
	const _Tt_arg_list_ptr &args,
	const _Tt_context_list_ptr &contexts,
	_Tt_string &reason,
	int &was_exact
) const
{
	was_exact = 0;
	if (_op != op) {
		reason = "op";
		return 0;
	}
	/* according to the spec if there are no arguments in */
	/* the signature then args should be ignored in the   */
	/* match. (see p.22)				      */
	if (_args->count() == 0) {
		if (args.is_null() || (args->count() == 0)) {
			was_exact = 1;
		}
		return 1;
	}
	/* for each arg in args match arg_kind and type */
	/* against corresponding arg in signature.      */

	if (! args.is_null() && args->count() != _args->count()) {
		reason = "args";
		return 0;
	}

	_Tt_arg_list_cursor	margc(args);
	_Tt_arg_list_cursor	argc(_args);
	int score = 1;
	was_exact = 1;
	while (argc.next() && margc.next()) {
		int used_wildcard;
		int points = argc->match_score( **margc, used_wildcard );
		if (used_wildcard) {
			was_exact = 0;
		}
		if (points <= 0) {
			reason = "args";
			return 0;
		}
		score += points;
	}

	if (_contexts->count() == 0) {
		return score;
	}
	/* for each context in contexts match slotname against */
	/* corresponding arg in signature.		       */

	_Tt_context_list_cursor	mc(contexts);
	_Tt_context_list_cursor	sc(_contexts);

        mc.next();
	sc.next();

	while (mc.is_valid() && sc.is_valid()) {
		int cmpres = mc->slotName().cmp(sc->slotName());
		if (cmpres<0) { // target less than signature
			// The target has a context slot which is not in 
			// the signature.  If we are matching a message 
			// for possible delivery, this still could be a 
			// match, but if we are matching another signature
			// for replacement in a tt_type_comp merge,
			// we should only replace exact matches.
			// Continue on to look for other mismatches,
			// but return partial mismatch to caller so it can
			// determine what to do.
			was_exact = 0;
			mc.next();
		} else if (cmpres==0) { // target same as signature
			// Context in target matches context in sig.
			mc.next();
			sc.next();
		} else  {	// target greater than signature
			// The signature has a context slot which is not
			// in the target.  This match fails.
			was_exact = 0;
			reason = "contexts";
			return 0;
		}
	}

	if (mc.is_valid() && !sc.is_valid()) {

		// Unconsumed context slots in target.
		was_exact = 0;

	} else if (!mc.is_valid() && sc.is_valid()) {

		// Unsatisfied context slots in signature
		was_exact = 0;
		reason = "contexts";
		return 0;

	} // else both are invalid, or both are valid. result stands..

	return score;
}

int _Tt_signature::
operator==(const _Tt_signature &sig) const
{
	if (_otid != sig.otid()) {
		return 0;
	}
	if (category() != sig.category()) {
		return 0;
	}
	int was_exact;
	if (0 >= match(sig.scope(), sig.message_class(), sig.op(),
		       sig.args(), sig.otid(), sig.contexts(), 0, was_exact))
	{
		return 0;
	}
	return was_exact;
		
}

//
// True if the two _Tt_signatures overload the same op in the
// same way (i.e. with the same arguments and contexts)
//
int _Tt_signature::
is_same_method(const _Tt_signature &sig) const
{
	int was_exact;
	_Tt_string r;
	if (match(sig.op(), sig.args(), sig.contexts(), r, was_exact) <= 0) {
		return 0;
	}
	if (! was_exact) {
		return 0;
	}
	return 1;
}

int _Tt_signature::
is_ce_equal(_Tt_signature &sig)
{
	_Tt_arg_list_cursor	argc;

	if (_op != sig._op) {
		return(0);
	}
	if (_otid != sig._otid) {
		return(0);
	}
	Tt_mode		md;
	if (_mangled_args.len() == 0) {
		_mangled_args = "_";
		argc.reset(_args);
		while (argc.next()) {
			md = argc->mode();
			_mangled_args = _mangled_args.cat(_tt_enumname(md)).cat(argc->type()).cat("_");
		}
	}
	if (sig._mangled_args.len() == 0) {
		sig._mangled_args = "_";
		argc.reset(sig._args);
		while (argc.next()) {
			md = argc->mode();
			sig._mangled_args = sig._mangled_args.cat(_tt_enumname(md)).cat(argc->type()).cat("_");
		}
	}

	return(_mangled_args == sig._mangled_args);
}


	

void _Tt_signature::
print(const _Tt_ostream &os) const
{
	_Tt_arg_list_cursor	argc;

	//
	// XXX This is only used for Classing Engine, never for
	// tracing, so we can be lazy and write directly to the
	// FILE * of the _Tt_ostream.
	//
	FILE *fs = os.theFILE();
	fprintf(fs,"(\n");
	fprintf(fs,"\t\t(%s,%s,<",
		_tt_ce_attr_string(_TYPE_NAME),
		_tt_ce_attr_string(_TT_TOOLTALK_TYPE));
	if (_otid.len()) {
		_otid.print(fs);
	} else {
		_ptid.print(fs);
	}
	fputs("::", fs);
	_op.print(fs);
	if (0==_mangled_args.len()) {
		fputc('_',fs);
	} else {
		_mangled_args.print(fs);
	}
	fputs(">)\n", fs);
	fprintf(fs,"\t\t(%s,string,<%s>)\n",
		_tt_ce_attr_string(_TT_TOOLTALK_TYPE),
		_tt_ce_attr_string(_TT_TOOLTALK_SIGNATURE));
	fprintf(fs,"\t\t(%s,string,<", _tt_ce_attr_string(_TT_OP));
	_op.print(fs);
	fputs(">)\n", fs);
	fprintf(fs,"\t\t(%s,string,<%s>)\n",
		_tt_ce_attr_string(_TT_CATEGORY),
		_tt_enumname(_pattern_category));
	argc.reset(_args);
	int argn=0;
	while (argc.next()) {
		fprintf(fs,"\t\t(%s%d,string,<%s ",
			_tt_ce_attr_string(_TT_ARG), argn,
			_tt_enumname(argc->mode()));
		argc->type().print(fs);
		fputs(" ", fs);
		argc->name().print(fs);
		fputs(">)\n", fs);
		argn++;
	}
	fprintf(fs,"\t\t(%s,string,<%s>)\n",
		(_otid.len()) ? _tt_ce_attr_string(_TT_MSET_SCOPE) :
		_tt_ce_attr_string(_TT_SCOPE),
		_tt_enumname(_scope));
	fprintf(fs,"\t\t(%s,string,<%s>)\n",
		_tt_ce_attr_string(_TT_CLASS),
		_tt_enumname(_message_class));
	switch (_reliability) {
	    case TT_START+TT_QUEUE:
#ifdef NOT_BACKWARD_COMPATIBLE
		fprintf(fs,"\t\t(%s,string,<TT_START+TT_QUEUE>)\n",
			_tt_ce_attr_string(_TT_MSET_DISPOSITION));
#else
		//
		// Version 1.0 can't deal with start+queue, so we
		// hide it in the typename, which version 1.0 doesn't
		// check.
		//
		fprintf(fs,"\t\t(%s,string:TT_START+TT_QUEUE,<TT_START>)\n",
			_tt_ce_attr_string(_TT_MSET_DISPOSITION));
#endif /*NOT_BACKWARD_COMPATIBLE*/
		break;
	    case TT_QUEUE:
		fprintf(fs,"\t\t(%s,string,<TT_QUEUE>)\n",
			_tt_ce_attr_string(_TT_MSET_DISPOSITION));
		break;
	    case TT_START:
		fprintf(fs,"\t\t(%s,string,<TT_START>)\n",
			_tt_ce_attr_string(_TT_MSET_DISPOSITION));
		break;
	    case TT_DISCARD:
	    default:
		fprintf(fs,"\t\t(%s,string,<TT_DISCARD>)\n",
			_tt_ce_attr_string(_TT_MSET_DISPOSITION));
		break;
	}
	fprintf(fs,"\t\t(%s,string,<%d>)\n",
		_tt_ce_attr_string(_TT_MSET_OPNUM), _opnum);
	fprintf(fs,"\t\t(%s,string,<",
		_tt_ce_attr_string(_TT_MSET_HANDLER_PTYPE));
	_ptid.print(fs);
	fputs(">)\n", fs);
	if (_otid.len()) {
		fprintf(fs,"\t\t(%s,string,<",
			_tt_ce_attr_string(_TT_MSET_OTYPE));
		_otid.print(fs);
		fputs(">)\n", fs);
	}
	fprintf(fs,"\t)");
}

void _Tt_signature::
pretty_print_(const _Tt_ostream &os, const _Tt_object *obj)
{
	((const _Tt_signature *)obj)->pretty_print( os );
}

void _Tt_signature::
pretty_print(const _Tt_ostream &os) const
{
	_Tt_arg_list_cursor	argc;
	int			argn;
	int			is_osig = _otid.len() > 0;

	if (! is_osig) {
		switch (_scope) {
		    case TT_FILE:		os << "file";		break;
		    case TT_SESSION:		os << "session";	break;
		    case TT_FILE_IN_SESSION:	os << "file_in_session";break;
		}
	}
	os << " " << _op << "(";
	argc.reset(_args);
	argn = _args->count();
	while (argc.next()) {
		switch (argc->mode()) {
		    case TT_IN:		os << "in";	break;
		    case TT_OUT:	os << "out";	break;
		    case TT_INOUT:	os << "inout";	break;
		}
		os << " " << argc->type() << " " << argc->name();
		if (--argn) {
			os << ", ";
		}
	}
	os << ")";
	if (_contexts->count() > 0) {
		_Tt_context_list_cursor ctxts(_contexts);
		os << " context(";
		ctxts.next();
		os << ctxts->slotName();
		while(ctxts.next()) {
			os << ", " << ctxts->slotName();
		}
		os << ")";
	}
	if (is_osig) {
		os << " => " << _ptid << " ";
		switch (_scope) {
		    case TT_FILE:		os << "file";		break;
		    case TT_SESSION:		os << "session";	break;
		    case TT_FILE_IN_SESSION:	os << "file_in_session";break;
		}
	} else {
		if (   (_reliability&TT_START) || (_reliability&TT_QUEUE)
		    || (_opnum != -1))
		{
			// We are psig and have right hand side, so need =>
			os << " =>";
		}
	}
	if (_reliability & TT_START) {
		os << " start";
	}
	if (_reliability & TT_QUEUE) {
		os << " queue";
	}
	if (_opnum != -1) {
		os << " opnum=" << _opnum;
	}
	if (is_osig && _super_otid.len()) {
		os << " from " << _super_otid;
	}
	os << ";\n";
}

void _Tt_signature::
pretty_print_(const _Tt_ostream &os, const _Tt_signature_list_ptr &sigs,
	     Tt_category category, int skip_sigs_with_otid)
{
	if (sigs.is_null()) {
		return;
	}
	if (sigs->count()) {
		int print_header = 1;
		_Tt_signature_list_cursor sigC(sigs);
		while (sigC.next()) {
			if (sigC->otid().len() && skip_sigs_with_otid) {
				continue;
			}
			if (sigC->category() != category) {
				continue;
			}
			if (print_header) {
				print_header = 0;
				switch (category) {
				    case TT_HANDLE:
					os << "handle:\n";
					break;
				    case TT_HANDLE_PUSH:
					os << "handle_push:\n";
					break;
				    case TT_HANDLE_ROTATE:
					os << "handle_rotate:\n";
					break;
				    case TT_OBSERVE:
					os << "observe:\n";
					break;
				}
			}
			os << "\t";
			sigC->pretty_print(os);
		}
	}
}

int _Tt_signature::
xdr_version_required() const
{
	int version = TT_TYPESDB_DEFAULT_XDR_VERSION;
	if (_contexts->count() > 0) {
		if(TT_CONTEXTS_XDR_VERSION > version) {
			version = TT_CONTEXTS_XDR_VERSION;
		}
//		version = max(version, TT_CONTEXTS_XDR_VERSION);
	}
	switch (_pattern_category) {
	    case TT_HANDLE_PUSH:
	    case TT_HANDLE_ROTATE:
		if(TT_PUSH_ROTATE_XDR_VERSION > version) {
			version = TT_PUSH_ROTATE_XDR_VERSION;
		}
//		version = max(version, TT_PUSH_ROTATE_XDR_VERSION);
		break;
	}
	return version;
}

int _Tt_signature::
xdr_version_required_(const _Tt_signature_list_ptr &sigs)
{
	int version = TT_TYPESDB_DEFAULT_XDR_VERSION;
	_Tt_signature_list_cursor	sigC( sigs );
	while (sigC.next()) {
		if(sigC->xdr_version_required() > version) {
			version = sigC->xdr_version_required();
		}
//		version = max(version, sigC->xdr_version_required());
	}
	return version;
}
