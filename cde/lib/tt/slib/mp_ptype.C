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
//%%  $TOG: mp_ptype.C /main/4 1998/03/20 14:27:56 mgreess $ 			 				
#include <stdlib.h>
#if defined(linux) || defined(CSRG_BASED)
/*# include <g++/minmax.h>*/
#else
# include <macros.h>
#endif
#include "util/tt_global_env.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "mp/mp_session.h"
#include "mp_ce_attrs.h"
#include "mp_signature.h"
#include "mp_ptype.h"
#include "mp_s_mp.h"
#include "mp_s_message.h"
#include "util/tt_enumname.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// 
// _Tt_ptype_prop represents a ptype property. Some common properties
// include the start string, per_session, and per_file properties.
// 
//  XXX: In fact there are only three properties in use. Perhaps it would
//  be better to just add those fields to the ptype object than have a
//  full attribute/value list.
//


_Tt_ptype_prop::
_Tt_ptype_prop()
{
}


_Tt_ptype_prop::
_Tt_ptype_prop(_Tt_string aname, _Tt_string avalue)
{
	_name = aname;	/* shared */
	_value = avalue;
}

_Tt_ptype_prop::
~_Tt_ptype_prop()
{
}

bool_t _Tt_ptype_prop::
xdr(XDR *xdrs)
{
	return(_name.xdr(xdrs) && _value.xdr(xdrs));
}

_Tt_string _Tt_ptype_prop::
name()
{
	return _name;
}

_Tt_string  _Tt_ptype_prop::
value()
{
	return _value;
}


// 
// Prints out a ptype prop. Note that this method is used to print out
// ptype properties in a way compatible with a Classing Engine format
// file. Therefore, changing the output of this method could hurt
// compatibility with CE databases.
// 
void _Tt_ptype_prop::
print(const _Tt_ostream &os) const
{
	os << "\t\t(" << _name << ",string,<" << _value << ">)\n";
}


// 
// Prints out a ptype property in "source" format. It is used by
// _Tt_ptype::pretty_print to print out a ptype definition in such a way
// that it can be parsed by tt_type_comp. 
// 
void _Tt_ptype_prop::
pretty_print(const _Tt_ostream &os) const
{
	if (_name == "start") {
		os << _name << "   \"" << _value;
		// if the start string ends with a double-quote then
		// just print it out without a double-quote at the
		// end. Otherwise print out the double-quote at the
		// end. This nonsense is made necessary because
		// version 1 tt_type_comp would parse the start string
		// so that the ending double-quote was made part of
		// the string. 
		if (_value[_value.len() - 1] != '\"') {
			os << '"';
		}
		os << ";\n";
	} else {
		os << _name << "   " << _value << ";\n";
	}
}



_Tt_ptype::
_Tt_ptype()
{
	if (_tt_mp && _tt_mp->in_server()) {
		_queued_messages = new _Tt_s_message_list();
	}
	_props =  new _Tt_ptype_prop_list();
	_osigs = new _Tt_signature_list();
	_hsigs = new _Tt_signature_list();
	_start_pid = 0;
	_launching = 0;
	ce_entry = 0;
}


_Tt_ptype::
~_Tt_ptype()
{
}


// 
// Returns the value of the given property name for this ptype.
// 
int  _Tt_ptype::
getprop(_Tt_string propname, _Tt_string &value)
{
	_Tt_ptype_prop_list_cursor	propc;

	propc.reset(_props);
	while (propc.next()) {
		if (propc->name() == propname) {
			value = propc->value();
			return(1);
		}
	}
	return(0);
}


// 
// Launches a new instance of this ptype. The message that caused the
// start is also given so that selected fields from the message can be
// defined as environment variables.
// 
// The design of the starting mechanism is biased towards starting the
// fewest number of applications per message. This is done as follows:
//  
// - If there are no messages queued on this ptype then a new instance of
// the ptype is launched and the message is put on the queue of messages
// for this ptype. If not done already, a start token is assigned to this
// ptype. (see _Tt_ptype::launch for details)
// 
// - If there are messages queued on this ptype then the message is
// simply added to the end of the queue. 
// 
// The messages get dequeued as follows:
// 
// - When the procid that was started joins this session, it will send us
// the start token back. This tells us which procid corresponds to the
// ptype instance that we just started. We then take the first message on
// the ptype queue and send it to the procid (without
// pattern-matching!). See _Tt_ptype::proc_started for details. 
//
// - Eventually, the procid will accept or reply to this message.  At this
// point we know all the patterns that this procid will register in order to
// further accept messages queued for its ptype. So we proceed to attempt
// delivery of the rest of the queued messages on this ptype. This is done
// by trying the first message, if it is delivered then we continue with the
// next message. If the message causes another start then it is put at the
// front of the queue and things proceed the same way as for the first
// message that caused a start. None of the other messages are dequeued.
// See _Tt_ptype::proc_replied for details.
// 
// What this algorithm accomplishes is starting the minimum amount of
// ptype instances per queue of messages. However, it doesn't handle the
// case that before the ptype instance that we started, another instance
// gets started (perhaps by the user) and registers patterns before the
// one we are waiting on.
// 
// Cycle detection/Launch failure
// 
// If a newly-started ptype fails (ie. it core-dumps while starting or
// else gets aborted) then we have to be careful not to keep starting the
// same ptype. There is the danger that if we're too conservative and
// just immediately fail all the messages, then some messages won't end
// up getting sent. On the other hand if we keep trying to deliver the
// messages we may end up in a cycle continually trying to fork a bad
// application.
// 
// The position taken here leans towards the conservative side:
// 
// - messages are re-delivered against the current patterns but they are
// tagged so that they won't cause any more starts of this ptype.
// 
// The idea behind this compromise is that either an app died abnormally
// in which case the safest option is to not try to start it again or
// else it was terminated by the user in which case we should also not
// start another instance. For requests, the sender will get a TT_FAILED
// state back along with a status field indicating the app was
// terminated. In these cases the sender can take appropiate action.
// Notifications that are important (such as a "content_changed" message
// for LinkService) should be queued in any case so that they will
// eventually get delivered.
// 
// See _Tt_ptype::launch_failed for details.
//
Tt_status _Tt_ptype::
start(_Tt_s_message_ptr &m, const _Tt_msg_trace &trace)
{
	Tt_status		status;

	_queued_messages->append(m);
	if ((! launching()) && _queued_messages->count() == 1) {
		// m was the first message for this ptype, launch a
		// new instance
		status = launch(m, trace);
		if (status != TT_OK) {
			_tt_syslog(0, LOG_ERR, "_Tt_ptype::launch(): %s",
				   _tt_enumname( status ));
			// launching the ptype failed. dequeue the
			// message from this ptype since it is going
			// to get failed at a higher-layer.

			(void)_queued_messages->pop();
			return(status);
		}
	}

	return(TT_OK);
}


//
// Launches an instance of this ptype. Sets up some environment
// variables (documented below) crucial to the start process. This
// method should never be used directly but rather it is used only by
// _Tt_ptype::start if that method determines that launching a new
// instance of this ptype is really necessary.
//
Tt_status _Tt_ptype::
launch(_Tt_s_message_ptr &m, const _Tt_msg_trace &trace)
{
	_Tt_string			start_string_prop("start");
	_Tt_string			start_cmd = (char *)0;
	_Tt_ptype_prop_list_cursor	propc;
	char				*cargv[4];
	int				maxfds;
	pid_t				pid;



	//
	// Find out what the start string for this ptype is.
	//
	if (! getprop(start_string_prop, start_cmd)) {
		// no start string specified in ptype
		// XXX:  should the type compiler catch this
		// condition? 
		return(TT_ERR_PTYPE);
	}
	
	//
	// set the _launching flag. This flag will only get cleared
	// when it is known that the procid corresponding to this
	// starting ptype has appeared (see _Tt_ptype::proc_replied).
	//
	_launching = 1;

	//
	// fork and exec the start string for this ptype.
	//
	switch ((pid = fork())) {
	      case -1:
		_tt_syslog(0, LOG_ERR, "fork(): %m");
		return(TT_ERR_INTERNAL);
	      case 0:
		// 
		// Establish the environment for the new process. Note that
		// the actual names of the variables are defined in
		// mp_session.h 
		//



		// The TT_CDE_FILE_HINT or TT_FILE_HINT environment variable
		//is used by the started application only. It points to the
		//file field of the start message.

		if (0 != m->file().len() &&
		    _tt_put_all_env_var( 2,
					 _tt_network_path_to_local_path(m->file()),
					 TT_CDE_FILE_HINT, TT_FILE_HINT)
		    != 2) {
			exit(1);
		}
		
		// The TT_CDE_START_TOKEN or TT_START_TOKEN environment
		// variable is used by the client-side library. It is a
		// signal to the client-side method
		// _Tt_mp::create_new_procid that the launched process is an
		// instance of the ptype pointed to by this environment
		// variable. This token is then handed back to the server by
		// _Tt_c_procid::set_fd_channel so that the server can match
		// up the actual procid with the instance of the ptype it
		// was launching.

		if (_tt_put_all_env_var(2, _ptid,
					TT_CDE_START_TOKEN,
					TT_START_TOKEN)
		    != 2) {
			exit(1);
		}
		// The TT_CDE_XATOM_NAME and TT_XATOM_NAME environment
		// variables point to the address string for the server
		// session. These will get used by the client-side library
		// to connect to this session.

		if (_tt_put_all_env_var(2,
					_tt_mp->initial_session->address_string(), 
					TT_CDE_XATOM_NAME, TT_XATOM_NAME)
		    != 2) {
			exit(1);
		}

		// The TT_CDE_START_SID or TT_START_SID environment variable
		// points to the server session's id. Note that since the
		// TT_CDE_XATOM_NAME and TT_XATOM_NAME environment variables
		// are set above to the address, the client-side library
		// would think this it is starting in a process-tree
		// session. However, if TT_CDE_START_SID or TT_START_SID is
		// also set then it will get used to set the id of the
		// client session (which should always be the same as the
		// one for the server session). See _Tt_session::set_id
		
		if (_tt_put_all_env_var(2, _tt_mp->initial_session->id(),
					TT_CDE_START_SID, 
					TT_START_SID)
		    != 2) {
			exit(1);
		}

		// set up DISPLAY variable so started application will start
		// up in the same X display as this one.

		if (0!=_tt_mp->initial_session->displayname().len() &&
		    _tt_putenv("DISPLAY",
			       _tt_mp->initial_session->displayname())
		    == 0) {
			exit(1);
		}

		m->set_start_env();
		maxfds = _tt_global->maxfds();
		int i;
		for (i = 3; i < maxfds; i++) {
			// fcntl(i, F_SETFD, 1);
			close(i);
		}
		_tt_restoredtablesize();
#ifdef _POWER
		cargv[0] = "/bin/bsh";
		cargv[1] = "-c";
		cargv[2] = (char *)start_cmd;
		cargv[3] = (char *)0;
		trace << "execv(\"/bin/bsh -c " << start_cmd << "\");";
		execv("/bin/bsh", cargv);
#else
		cargv[0] = "/bin/sh";
		cargv[1] = "-c";
		cargv[2] = (char *)start_cmd;
		cargv[3] = (char *)0;
		trace << "execv(\"/bin/sh -c " << start_cmd << "\");";
		execv("/bin/sh", cargv);
#endif
		exit(1);
	      default:
		_start_pid = pid;
		return(TT_OK);
	}
}


// 
// Returns the value of the _launching flag which, if true, indicates
// that an instance of this ptype has be launched but its associated
// procid hasn't connected to the server yet.
// 
int _Tt_ptype::
launching() const
{
	return(_launching);
}


// 
// Returns the pid of the process launched for this ptype.
// 
pid_t _Tt_ptype::
start_pid() const
{
	return(_start_pid);
}


// 
// Called to notify this ptype that the procid that corresponds to the
// start instance of this ptype replied to its start message. This means
// that we can start delivering the messages queued on this ptype object.
// 
void _Tt_ptype::
proc_replied()
{
	_Tt_s_message_list_cursor		qmsgs;

	// if the procid called tt_message_accept, which turns into
	// _Tt_s_procid::unblock_ptype on this side, then proc_replied
	// will have already been done and _launching will be zero.
	// In that case, we don\'t have to do anything here, so we bail
	// out early.

	if (!_launching) {
		return;
	}

	_startmsg = (_Tt_s_message *)0;
	_launching = 0;
	// re-deliver the rest of the messages until one of them fails
	// to get delivered.
	qmsgs.reset(_queued_messages);
	while (qmsgs.next()) {
		qmsgs->set_state(TT_SENT);

		// note that _Tt_s_message::deliver won't re-queue
		// this message on the ptype because formally this
		// message has already been in the TT_STARTED phase.
		// Thus if deliver returns 0 (which means that it
		// wasn't delivered to anyone) then we just launch a
		// new ptype to handle the message and return. (See
		// comment for _Tt_ptype::start).
		//
		_Tt_msg_trace trace( **qmsgs, TTDR_MESSAGE_ACCEPT );
		if (! qmsgs->deliver( trace )) {
			// msg couldn't be delivered without causing
			// another start, so launch another instance
			// of this ptype to handle the rest of the
			// messages.
			Tt_status status = launch(*qmsgs, trace);
			if (status != TT_OK) {
				_tt_syslog(0, LOG_ERR,
					   "_Tt_ptype::launch(): %s",
					   _tt_enumname( status ));
			}
			_launching = 1;
			return;
		}
		qmsgs.remove();
	}
}


// 
// Called to notify this ptype what the procid is that corresponds to the
// started instance of this ptype.
// 
void _Tt_ptype::
proc_started(const _Tt_s_procid_ptr &pr)
{
	if (_queued_messages->count() == 0) {
		return;
	}
	_startmsg = _queued_messages->top();
	(void) _queued_messages->pop();

	if (_tt_global->xdr_version() == 1) {
		// FCS 1.0 library always expects handler to be set
		_startmsg->set_handler_procid(pr);
	} else {
		// Only set handler_procid if pr is meant to
		// be a handler. This is true if _startmsg doesn't
		// have a static observer set.
		if (_startmsg->is_handler_copy()) {
			_startmsg->set_handler_procid(pr);
		}
	}
	_startmsg->set_start_message();
	if (pr->add_message(_startmsg)) {
		// clear _startmsg since it's been sent already
		_startmsg = (_Tt_s_message *)0;
		return;
	} else {
		// send failed for startmsg, fail all messages. We
		// can't try to launch again because the ptype we're
		// trying to start may be coming up and crashing right
		// away. If so, then we would cause an infinite loop
		// trying to restart it every time. Our only choice
		// is to be pessimistic and just fail all the
		// messages. 
		launch_failed();
		return;
	}
}


// 
// Called in order to notify this object that the process that was forked
// to launch this ptype exited without connecting to its default session
// first. The method then proceeds to fail all messages that were queued
// for this ptype (since it isn't known that retrying the launch would
// succeed and if it doesn't it could lead to excessive fork activity.)
// 
void _Tt_ptype::
launch_failed()
{
	_Tt_s_message_list_cursor	qmsgs;
	if (! _startmsg.is_null()) {
		_startmsg->set_status((int)TT_ERR_PTYPE_START);
		_Tt_msg_trace trace( *_startmsg, TTDR_ERR_PTYPE_START );
		_startmsg->change_state( 0, TT_FAILED, trace );
	}
	// fail the rest of the messages
	qmsgs.reset(_queued_messages);
	while (qmsgs.next()) {
		qmsgs->set_status((int)TT_ERR_PTYPE_START);
		_Tt_msg_trace trace( **qmsgs, TTDR_ERR_PTYPE_START );
		qmsgs->change_state( 0, TT_FAILED, trace );
		qmsgs.remove();
	}
	_launching = 0;
}


// 
// Used by the generic table package to generate keys. Should not be used
// otherwise. 
//
_Tt_string
_tt_ptype_ptid(_Tt_object_ptr &o)
{
	return(((_Tt_ptype *)o.c_pointer())->ptid());
}


void _Tt_ptype::
set_ptid(_Tt_string ptid)
{
	_ptid = ptid;
}


_Tt_signature_list_ptr & _Tt_ptype::
osigs()
{
	return(_osigs);
}


_Tt_signature_list_ptr & _Tt_ptype::
hsigs()
{
	return(_hsigs);
}


// 
// Appends a list of observer signatures to this ptype.
// 
void _Tt_ptype::
append_osigs(_Tt_signature_list_ptr &sigs)
{
	_Tt_signature_list_cursor	sc(sigs);

	while (sc.next()) {
		sc->set_pattern_category(TT_OBSERVE);
	}
	if (_osigs.is_null()) {
		_osigs = sigs;
	} else {
		_osigs->append_destructive(sigs);
	}
}


// 
// Appends a list of handler signatures to this ptype.
// 
void _Tt_ptype::
append_hsigs(_Tt_signature_list_ptr &sigs, Tt_category category)
{
	_Tt_signature_list_cursor	sc(sigs);

	while (sc.next()) {
		sc->set_pattern_category(category);
	}
	if (_hsigs.is_null()) {
		_hsigs = sigs;
	} else {
		_hsigs->append_destructive(sigs);
	}
}


// 
// Appends a single observer signature to this ptype.
// 
void _Tt_ptype::
append_osig(_Tt_signature_ptr sig)
{
	_Tt_signature_list_cursor	obsigs;

	sig->set_pattern_category(TT_OBSERVE);
	obsigs.reset(_osigs);
	while (obsigs.next()) {
		if (**obsigs == *sig) {
			return;
		}
	}
	_osigs->append(sig);
}


// 
// Appends a single handler signature to this ptype.
// 
void _Tt_ptype::
append_hsig(_Tt_signature_ptr sig, Tt_category category)
{
	_Tt_signature_list_cursor	obsigs;

	sig->set_pattern_category(category);
	obsigs.reset(_hsigs);
	while (obsigs.next()) {
		if (**obsigs == *sig) {
			return;
		}
	}
	_hsigs->append(sig);
}


void _Tt_ptype::
appendprop(_Tt_string name, _Tt_string value)
{
	_props->append(new _Tt_ptype_prop(name, value));
}


// 
// Prints out this ptype object in such a way that the resulting output
// can be reparsed by tt_type_comp to yield another ptype object with the
// same characteristics as this one. As such the output resulting from
// this method is strictly dependent on the syntax for ptype definitions
// and should not be changed lightly.
// 
void _Tt_ptype::
pretty_print(const _Tt_ostream &os) const
{
	_Tt_ptype_prop_list_cursor	props;

	os << "ptype " << _ptid << " {\n";
	props.reset(_props);
	while (props.next()) {
		props->pretty_print(os);
	}
	_Tt_signature::pretty_print_( os, _hsigs, TT_HANDLE, 1 );
	_Tt_signature::pretty_print_( os, _hsigs, TT_HANDLE_PUSH, 1 );
	_Tt_signature::pretty_print_( os, _hsigs, TT_HANDLE_ROTATE, 1 );
	_Tt_signature::pretty_print_( os, _osigs, TT_OBSERVE, 1 );
	os << "};\n";
}


int _Tt_ptype::
xdr_version_required() const
{
	int version = _Tt_signature::xdr_version_required_( _hsigs );
//	return max(version, _Tt_signature::xdr_version_required_( _osigs ));
	if(version > _Tt_signature::xdr_version_required_( _osigs )) {
		return version;
	} else {
		return _Tt_signature::xdr_version_required_( _osigs );
	}
}


// 
// Used by the generic list and table packages to print out elements.
// Should not be used otherwise.
// 
void
_tt_print_ptype_prop(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_ptype_prop *)obj)->print(os);
}


// 
// Used by the generic list and table packages to print out elements.
// Should not be used otherwise.
// 
void
_tt_print_signature(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_signature *)obj)->print(os);
}


// 
// Used to print out this ptype definition so that it can be incorporated
// into a Classing Engine database. This method should not be changed
// lightly since it could compromise compatibility with older type
// databases. 
// 
void _Tt_ptype::
print(const _Tt_ostream &os) const
{
	os << "\t(\n\t\t(" << _tt_ce_attr_string(_TYPE_NAME) << ','
	   << _tt_ce_attr_string(_TT_TOOLTALK_PTYPE) << ",<"
	   << _ptid << ">)\n";
	os << "\t\t(" << _tt_ce_attr_string(_TT_TOOLTALK_TYPE)
	   << ",string,<" << _tt_ce_attr_string(_TT_TOOLTALK_PTYPE)
	   << ">)\n";
	if (_props->count()) {
		_props->print(_tt_print_ptype_prop, os);
	}
	os << "\t)";
	if (_hsigs->count()) {
		_hsigs->print(_tt_print_signature, os);
	}
	if (_osigs->count()) {
		_osigs->print(_tt_print_signature, os);
	}
}


bool_t _Tt_ptype::
xdr(XDR *xdrs)
{
	if (! _ptid.xdr(xdrs)) {
		return(0);
	}
	if (! _props.xdr(xdrs)) {
		return(0);
	}
	if (! _osigs.xdr(xdrs)) {
		return(0);
	}
	if (! _hsigs.xdr(xdrs)) {
		return(0);
	}
	return(1);
}


// 
// Check that ptype contains no duplicate signatures.
// Return 1 if duplicate found, else 0.
// 
int _Tt_ptype::
check_semantics()
{
	int result = 0;

	_Tt_signature_list_cursor s1(_hsigs);
	while (s1.next()) {
		_Tt_signature_list_cursor s2(s1);
		while (s2.next()) {
			if (**s1 == **s2) {
				_tt_syslog(stdout, LOG_ERR,
					   catgets(_ttcatd, 2, 2,
						   "indistinguishable signatures"
						   "found for ptype %s -"),
					   (char *) ptid());
				s1->pretty_print(stdout);
				s2->pretty_print(stdout);
				result = 1;
			}
		}
	}
	return result;
}


// 
// Used to merge a new ptype definition represented by "pt" with this
// ptype. This method is used by tt_type_comp to merge new definitions of
// existing ptypes. The behavior supported is that any signatures in the
// old definition that are not in the new definition remain, any
// signatures in the new definition are appended to the definition, and
// any signatures in the new definition that are in the old definition
// replace the ones in the old definition. Note that the last case
// means that only attribute changes are possible for the new
// signature (eg. different disposition value, etc.) since the op and
// args are by definition the same.
//
/*
void _Tt_ptype::
merge_with(_Tt_ptype_ptr &pt)
{
	_Tt_ptype_prop_list_cursor	propc;
	_Tt_ptype_prop_list_cursor	oldpropc;
	_Tt_signature_list_cursor	oldsigs;
	_Tt_signature_list_cursor	newsigs;
	_Tt_string			oname;


	// merge attributes
	oldpropc.reset(_props);
	while (oldpropc.next()) {
		propc.reset(pt->_props);
		while (propc.next()) {
			oname = oldpropc->name();
			if (oname == propc->name()) {
				oldpropc.remove();
				break;
			}
		}
	}
	_props->append_destructive(pt->_props);

	// merge observer signatures
	oldsigs.reset(_osigs);
	while (oldsigs.next()) {
		newsigs.reset(pt->_osigs);
		while (newsigs.next()) {
			if (newsigs->is_equal(*oldsigs)) {
				oldsigs.remove();
				break;
			}
		}
	}
	_osigs->append_destructive(pt->_osigs);
	
	// merge handler signatures
	oldsigs.reset(_hsigs);
	while (oldsigs.next()) {
		newsigs.reset(pt->_hsigs);
		while (newsigs.next()) {
			if (newsigs->is_equal(*oldsigs)) {
				oldsigs.remove();
				break;
			}
		}
	}
	_hsigs->append_destructive(pt->_hsigs);
}
*/

int
operator==(_Tt_ptype &a, _Tt_ptype& b)
{
	// Two ptypes are equal if they are the same object, or their
	// ptids are equal strings.
	if (&a == &b) {
		return 1;
	} else if (a.ptid() == b.ptid()) {
		return 1;
	} else {
		return 0;
	}
}

int 
operator!=(_Tt_ptype &a, _Tt_ptype &b)
{
	return !(a==b);
}
		


// 
// Used by the generic list and table packages to print elements. Should
// not be used otherwise.
// 
void
_tt_ptype_print(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_ptype *)obj)->print(os);
}
