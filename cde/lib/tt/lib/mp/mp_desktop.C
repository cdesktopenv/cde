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
//%%  $TOG: mp_desktop.C /main/7 1998/04/09 17:52:01 mgreess $ 			 				
/*
 *
 * @(#)mp_desktop.C	1.34 93/09/07
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */

//
// This file contains method for dealing with a "desktop" connection.
// Typically this desktop is an X11 session but this way the interface
// to the desktop is relatively free of Xisms allowing for more
// independence from X11 in the future.
//


#include "tt_options.h"
// Defining BSD_COMP gets us FIONREAD on SunOS 5.x, and shouldn\'t
// hurt in other places.
#define BSD_COMP
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#if defined(__STDC__) && !defined(linux) && !defined(CSRG_BASED)
extern "C" {  extern int ioctl 	(int, int, ...) ; };
#endif     
#include "util/tt_global_env.h"
#include "util/tt_ldpath.h"
#include "util/tt_host.h"
#include "util/tt_port.h"
#include "util/tt_Xlib.h"
#include "mp/mp_desktop.h"
#include "mp/mp_mp.h"
#include <unistd.h>
#include <stdlib.h>
#include "util/tt_gettext.h"

static int parse_Xdisplay_string(_Tt_string display,
				 _Tt_string &host,
				 pid_t &svnum,
				 _Tt_string &hostname);


jmp_buf _Tt_desktop::io_exception;

// The following private class data is declared and allocated here
// so that everybody that includes mp_desktop.h doesn't have to
// also include X11/Xlib.h.

struct _Tt_desktop_private {
	Display				*xd;
};					 

_Tt_desktop::
_Tt_desktop()
{
	priv = (_Tt_desktop_private *)malloc(sizeof(_Tt_desktop_private));
	priv->xd = (Display *)0;
}


_Tt_desktop::
~_Tt_desktop()
{
	// Ungrab the server, in case the user
	// interrupted us during a grab.
	unlock();
	close();
	free((MALLOCTYPE *)priv);
}


// 
// Initializes a desktop object. This has the effect of connecting to the
// appropiate X11 server. "dt_handle" is a string identifying which
// desktop we want to talk to. For X11, this string will be the same as
// what would be specified in the DISPLAY variable. 
//
int _Tt_desktop::
init(_Tt_string dt_handle, _Tt_dt_type /* t */)
{
	char		buf[32];
	_Tt_string	h, hostname;
	pid_t		s;
	int		parse_status;
	int		ret_val;

	if (priv->xd != (Display *)0) {
		return(1);
	}
	
	// initialize our access to Xlib
	if (! _tt_load_xlib()) {
		return(0);
	}

	_Tt_string display;

	// parse the dt_handle string to extract the host and server
	// number information.

	parse_status = parse_Xdisplay_string(dt_handle, h, s, hostname);

	// Now we examine the parse status to determine if this is a
	// local host or remote host and if we should use a unix
	// connection or not. We also enforce the use of screen 0 for
	// all connections.
	switch (parse_status) {
	      case 1: // non-local host
	      case 2: // local host
		// important to always use screen 0 for the case where
		// multiple displays are used.
		sprintf(buf,":%d.0",s);
		if (dt_handle[0] == ':') {
			display = buf;
		} else {
			display = hostname.cat(buf);
		}
		break;
	      case 3: // local host and unix connection specified
		sprintf(buf,"unix:%d.0", s);
		display = buf;
		break;
	      case 0:
		_tt_syslog( 0, LOG_ERR,
			    catgets( _ttcatd, 1, 17,
				     "could not parse X display name: \"%s\"" ),
			    (char *)dt_handle );
		return(0);
	      default:
		break;
	}
	
	ret_val = 1;
	int retries = 20;
	set_error_handler(_Tt_desktop::io_error_proc);
	if (0 == setjmp(io_exception)) {

		// now connect to the indicated X11 server
		while (retries--) {
			if (priv->xd = (Display *)
			       CALLX11(XOpenDisplay)((char *)display)) {
				// Xlib has already emitted diagnostic
				break;
			}
			sleep(1);
		}
		if (!priv->xd) ret_val = 0;
	} else {
		ret_val = 0;
	}
	restore_user_handler();
	return(ret_val);
}


// I/O error handler. Longjmp back to before the error occured.
int _Tt_desktop::
io_error_proc(void *)
{
	longjmp(io_exception, 1);
	return(0);
}


// Sets the error handler function which will get invoked on any I/O
// errors received from the X11 connection
void _Tt_desktop::
set_error_handler(_Tt_dt_errfn efn)
{
	user_io_handler = (int *)
		CALLX11(XSetIOErrorHandler)((XIOErrorHandler)efn);
}


// Restore the users I/O error handler.
void _Tt_desktop::
restore_user_handler()
{
	CALLX11(XSetIOErrorHandler)((XIOErrorHandler)user_io_handler);
}


// Closes the connection to the X11 server.
int _Tt_desktop::
close()
{
	int     ret_val = 1;

	set_error_handler(_Tt_desktop::io_error_proc);
	if (0 == setjmp(io_exception)) {

		// delete all properties set and close connection to desktop
		if (priv->xd != (Display *)0) {
			CALLX11(XCloseDisplay)(priv->xd);
		}
	} else {
		ret_val = 0;
	}
	restore_user_handler();
	return(ret_val);
}


// Returns the fd to poll on when new events come from the desktop
// connection. 
int _Tt_desktop::
notify_fd()
{
	return ConnectionNumber(priv->xd);
}


//      
// Method to call when an event comes in from the desktop connection.
// This method will return 0 if the connection is broken indicating
// either a network partition or the X11 server going down.
// 
int _Tt_desktop::
process_event()
{
	XEvent		xev;
	XMappingEvent	*xm;
	int		pending;
	int		iostat;

	if (priv->xd == (Display *)0) {
		return(0);
	}

	CALLX11(XFlush)(priv->xd);
	xev.type = 0;
	iostat=ioctl(notify_fd(), FIONREAD, (char *)&pending);
	if (iostat == -1 || pending == 0) {
		// X server went down
		return(0);
	}
	if (priv->xd != (Display *)0) {
		CALLX11(XNextEvent)(priv->xd, &xev);
		if (xev.type == MappingNotify) {
			xm = (XMappingEvent *)&xev;
			CALLX11(XRefreshKeyboardMapping)(xm);
		}
	}

	return(1);
}


int _Tt_desktop::
lock()
{
	if (priv->xd != 0) {
		CALLX11(XGrabServer)(priv->xd);
		CALLX11(XFlush)(priv->xd);
	}
	return 1;
}

int _Tt_desktop::
unlock()
{
	if (priv->xd != 0) {
		CALLX11(XUngrabServer)(priv->xd);
		CALLX11(XFlush)(priv->xd);
	}
	return 1;
}

// 
// Sets the value of the indicated property name to val.
// 
int _Tt_desktop::
set_prop(_Tt_string pname, _Tt_string &val)
{
	Window			rootw;
	Atom			patom;
	char			*v1;
	const unsigned char	*v;
	
	if (priv->xd == (Display *)0) {
		return(0);
	}

	patom = CALLX11(XInternAtom)(priv->xd, (char *)pname, False);
	if (patom == None) {
		return(0);
	}
	
	rootw = DefaultRootWindow(priv->xd);
	v1 = (char *)val;
	v = (const unsigned char *)v1;
	CALLX11(XChangeProperty)(priv->xd, rootw,
				 patom, XA_STRING, 8,
				 PropModeReplace,
				 v, val.len());
	CALLX11(XFlush)(priv->xd);
	return(1);
}


// 
// Deletes the indicated property from the desktop.
// 
int _Tt_desktop::
del_prop(_Tt_string pname)
{
	Window		rootw;
	Atom		patom;
	
	if (priv->xd == (Display *)0) {
		return(0);
	}

	patom = CALLX11(XInternAtom)(priv->xd, (char *)pname, False);
	if (patom == None) {
		return(0);
	}

	rootw = DefaultRootWindow(priv->xd);
	CALLX11(XDeleteProperty)(priv->xd, rootw, patom);
	CALLX11(XFlush)(priv->xd);

	return(1);
}


// 
// Gets the value of the indicated property from the desktop.
// 
int _Tt_desktop::
get_prop(_Tt_string pname, _Tt_string &pval)
{
	Atom			anatom;
	int			format;
	/* X11 requires these to longs and not ints */
	unsigned long           items;
	unsigned long           left_to_read;
	unsigned char		*val = (unsigned char *)0;
	Atom			tt_xatom;
	
	if (priv->xd == (Display *)0) {
		return(0);
	}

	tt_xatom = CALLX11(XInternAtom)(priv->xd, (char *)pname, False);
	if (tt_xatom == None) {
		return(0);
	}
	CALLX11(XGetWindowProperty)(priv->xd,  DefaultRootWindow(priv->xd),
				    tt_xatom,
				    0, 20, False, XA_STRING,
				    &anatom, &format, &items, &left_to_read,
				    &val);	
	if (val == (unsigned char *)0) {
		return(0);
	}
	pval = (char *)val;
	CALLX11(XFree)((caddr_t)val);
	return(1);
}


// 
// Parse a string in X11 "display" format and return the host and server
// number found. If parse errors occur return 0, else return 1 if the
// host is not the local host and 2 if the host is the local host. If the
// host is specified as "unix" which means use the local host with a Unix
// socket connection then return 3.
//
// XXX - the "hostname" paramater is an artifact of the fix for bug 1118012.
//	 if/when session IDs are re-worked to use only the process tree
//	 format internally, instead of the additional use of X session IDs,
//	 the parse_Xdisplay_string() function should be re-written to
//	 remove this.
// 
static int
parse_Xdisplay_string(_Tt_string display, _Tt_string &host, pid_t &svnum,_Tt_string &hostname)
{
	int		status = 1;
	int	   	offset;
	_Tt_host_ptr	h;
	_Tt_host_ptr	localh;
	char		*dstr;
	
	offset = display.index(':');
	if (offset == -1) {
		return(0);
	}
	if (offset == 0) {

		// use local host
		(void)_tt_global->get_local_host(h);
		host = h->stringaddr();
		hostname = h->name();
		status = 2;
	} else {
		// Get the hostid portion of an X display of the
		// format "hostid:X"
		//
		host = display.mid(0, offset);

		if (host == "unix") {
			(void)_tt_global->get_local_host(h);
			host = h->stringaddr();
			hostname = h->name();
			status = 3;

		} else {

			if (! _tt_global->find_host_byname(host, h)) {
				if (! _tt_global->find_host(host, h, 1)) {
					return(0);
				}
			} 
			host = h->stringaddr();
			hostname = h->name();
			if (_tt_global->get_local_host(localh)) {
				if (localh->stringaddr() == host) {
					status = 2;
				}
			}
		}
	}
	// now get the server number
	dstr = (char *)display + offset + 1;
	if (*dstr == ':') {
		// ugh, a decnet connection
		dstr++;
	}

	long long_svnum;
	if (1 != sscanf(dstr, "%ld", &long_svnum)) {
	  	svnum = (pid_t)long_svnum;
		return(0);
	} else {
		svnum = (pid_t)long_svnum;
	}

	return(status);
}


// 
// Returns a suitable name for this desktop session. Changing the value
// returned from this method should be done with care because it could
// compromise older versions of tooltalk clients/servers from
// communicating since a canonical name for a tooltalk desktop session
// contains the desktop session name in it and tooltalk session names
// indicate to clients how to contact the session.
// 
_Tt_string _Tt_desktop::
session_name(_Tt_string dt_handle)
{
	char		cid[BUFSIZ];
	_Tt_string	h, hostname;
	pid_t		s;
	_Tt_string	name = (char *)0;

	if (0==parse_Xdisplay_string(dt_handle, h, s, hostname)) {
		return((char *)0);
	}
	sprintf(cid, "X %s %d", (char *)h, s);
	name = cid;
	return(name);
}

_Tt_desktop_lock::
_Tt_desktop_lock()
{
}

_Tt_desktop_lock::
_Tt_desktop_lock( const _Tt_desktop_ptr &dt )
{
	_dt = dt;
	if (! _dt.is_null()) {
		_dt->lock();
	}
}

_Tt_desktop_lock::
~_Tt_desktop_lock()
{
	if (! _dt.is_null()) {
		_dt->unlock();
	}
}
