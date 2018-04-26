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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_procid.C /main/4 1997/07/30 15:43:53 samborn $ 			 				
/*
 *
 * mp_procid.cc -- methods for communicating with active processes
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <errno.h>
#include <fcntl.h>
#include "util/tt_host.h"
#include "mp/mp_file.h"
#include "mp/mp_message.h"
#include "mp/mp_mp.h"
#include "mp/mp_pattern.h"
#include "mp/mp_procid.h"
#include "mp/mp_session.h"
#include "mp/mp_stream_socket.h"
#include "util/tt_int_rec.h"
#include "util/tt_global_env.h"
#include <arpa/inet.h>

#if defined(sgi)
#include <CC/libc.h>
#endif


_Tt_procid::
_Tt_procid()
{
	_fd = -1;
	_pid = 0;
	_version = 0;
	_flags = 0;
	_mxdr_stream = (XDR *)0;
	_program = 0;
}

     
_Tt_procid::
~_Tt_procid()
{
}


// 
// Returns the fd which will be used to signal this procid if there are
// new messages for it. -1 means error. This method is only used in
// client mode. This method caches the fd returned from the
// _Tt_stream_socket::fd method. This method has the side-effect that it
// will accept the connection on the socket if not already done. See the
// comments in _Tt_c_procid::set_fd_channel for more details.
// 
int _Tt_procid::
fd()
{
	if (_socket.is_null()) {
		return(-1);
	} else if (_fd == -1) {
		_fd = _socket->fd();
	}
	
	return(_fd);
}


//
// Returns the string version of the id for this procid. See
// _Tt_c_procid::init() for a description of the format for this id.
//
//  XXX: Warning! Do not inline the following. Causes weirdness with
//  the table package.
//
const _Tt_string & _Tt_procid::
id() const
{
	// return string representation
	return(_id);
}


//
// Returns 1 if this procid's id is the same as the one given.
//
int _Tt_procid::
is_equal(_Tt_string &i)
{
	if (_id.len()) {
		return(_id == i);
	} else  {
		return(0);
	}
}


//
// Returns the TCP port to connect to to signal a procid for a new
// message. This method is only used in server mode.
//
int _Tt_procid::
port() const
{
	if (_socket.is_null()) {
		return(-1);
	} else {
		return(_socket->port());
	}
}


//
// XDR encodes/decodes a procid.
//
bool_t _Tt_procid::
xdr(XDR *xdrs)
{
	long localpid;
	
	if (! _id.xdr(xdrs)) {
		return 0;
	}
	
	if (xdrs->x_op == XDR_ENCODE) {
		localpid = _pid;
	}
	if (! xdr_long(xdrs, &localpid)) {
		return 0;
	}
	if (xdrs->x_op == XDR_DECODE) {
		_pid = (pid_t)localpid;
	}
	
	return(_proc_host_ipaddr.xdr(xdrs));
}


void _Tt_procid::
print(const _Tt_ostream &os) const
{
	os << _id << " [" << _pid;
	if (! _proc_host.is_null()) {
		os << "@" << _proc_host->name();
	}
	os << "]\n";
}


//
// Returns 1 if this procid is handling m or is voting on it.
//
int _Tt_procid::
processing(const _Tt_message &m)
{
	switch (m.state()) {
	      case TT_FAILED:
	      case TT_HANDLED:
	      case TT_RETURNED:
		// if message state is already final then it can't be
		// handled by anyone.
		return 0;
	      default:
		break;
	}
	if (!m.handler().is_null()) {
		return m.handler()->is_equal( this );
	} else if (m.message_class() == TT_OFFER) {
		return 1;
	} else {
		return 0;
	}
}


//
// Wrapper to invoke the _Tt_procid_ptr::xdr method.
//
bool_t
tt_xdr_procid(XDR *xdrs, _Tt_procid_ptr *patp)
{
	return((*patp).xdr(xdrs));
}


//
// Wrapper to invoke the _Tt_message_list_ptr::xdr method.
//
bool_t
tt_xdr_message_list(XDR *xdrs, _Tt_message_list_ptr *mptr)
{
	return((*mptr).xdr(xdrs));
}


//
// Add the given path to the list of files this procid object has
// joined. 
//
void _Tt_procid::
add_joined_file(_Tt_string fpath)
{
	if (joined_to_file(fpath)) {
		return;
	}
	if (_joined_files.is_null()) {
		_joined_files = new _Tt_string_list();
	}
	_joined_files->push(fpath);
}


// 
// Delete the given path from the list of files this procid object has
// joined. 
//
void _Tt_procid::
del_joined_file(_Tt_string fpath)
{
	if (_joined_files.is_null()) {
		return;
	}
	_Tt_string_list_cursor	jc(_joined_files);
	
	while (jc.next()) {
		if (*jc == fpath) {
			jc.remove();
		}
	}
}


//
// Return 1 if the given path represents a file this procid object has 
// joined. 
//
int _Tt_procid::
joined_to_file(_Tt_string fpath)
{
	if (_joined_files.is_null()) {
		return(0);
	}
	
	_Tt_string_list_cursor	jc(_joined_files);
	
	while (jc.next()) {
		if (*jc == fpath) {
			return(1);
		}
	}
	return(0);
}


// 
// Invokes _Tt_procid::id method on o which is assumed to point to a
// _Tt_procid. Used by the table package to generate keys.
// 
_Tt_string
_tt_procid_id(_Tt_object_ptr &o)
{
	return(((_Tt_procid *)o.c_pointer())->id());
}




#ifdef OPT_ADDMSG_DIRECT
//
// If this option is turned on, these functions are used to read/write
// a given counted buffer to the given _Tt_stream_socket object.
// "iohandle" is really a pointer to this _Tt_stream_socket object (it
// is given to xdrrec_create, and then XDR will give it to these
// functions to do the read/writes to an xdr stream. See
// _Tt_s_procid::signal_new_message(_Tt_message_ptr &m), and
// _Tt_c_procid::next_message.) 
//



int
_tt_xdr_readit(char *iohandle, char *buf, int nbytes)
{
	int			rval;
	fd_set			readfds;
	timeval			tmout;
	int			fd;
	_Tt_stream_socket	*sptr = (_Tt_stream_socket *)iohandle;

	fd = sptr->fd();
	FD_ZERO(&readfds);
	FD_SET(fd,&readfds);
	tmout.tv_sec = 0;
	tmout.tv_usec = 0;
	if (select(FD_SETSIZE, &readfds, 0, 0, &tmout) <= 0) {
		if (errno == EBADF) {
			_tt_syslog( 0, LOG_ERR, "_tt_xdr_readit(): %m" );
			return(-1);
		}
	}
	if (!FD_ISSET(fd, &readfds)) {
		_tt_syslog( 0, LOG_ERR, "_tt_xdr_readit(): !FD_ISSET()" );
		return(0);
	}
	rval = sptr->recv(buf, nbytes);
	return((rval) ? rval : -1);
}


int
_tt_xdr_writeit(char *iohandle, char *buf, int nbytes)
{
	return(((_Tt_stream_socket *)iohandle)->send(buf,nbytes));
}
#endif	// OPT_ADDMSG_DIRECT
