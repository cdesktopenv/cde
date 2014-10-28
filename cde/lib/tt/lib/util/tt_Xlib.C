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
//%%  $XConsortium: tt_Xlib.C /main/3 1995/10/23 10:36:22 rswiston $ 			 				
/*
 * @(#)tt_Xlib.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */

#include "util/tt_Xlib.h"
#if defined(OPT_DLOPEN_X11)
#include <dlfcn.h>
#endif
#include "util/tt_string.h"
#include "util/tt_port.h"


#ifdef OPT_DLOPEN_X11

/************************************************************************** 
 * This section contains functions for dynamically loading in the
 * necessary X11 functions. This avoids the need for applications to link
 * in libX11. Note that all of this section is ifdefd with OPT_DLOPEN_X11 so
 * only functions relevant to dynamic loading of X11 should be included.
 **************************************************************************/
_Tt_xlib _tt_xlib;
_Tt_xt _tt_xt;

// 
// Dlopens the X11 library and attempts to dlsym the needed functions
// from it. 
// 
int
_tt_load_xlib()
{
	static int	loaded = 0;

	if (loaded > 0) {
		return 1;
	}

	int		load_xlib_fns_from_handle(void *xlib_handle);
	int		full_ldpath(_Tt_string libname, _Tt_string &path);

	// open libX11.so and fill in _tt_xlib with pointers to
	// functions
	void		*xlib_handle;
	_Tt_string	xlib_path;

	// first try opening the current executable to see if the
	// functions have already been defined
	xlib_handle = dlopen((char *)0, 1);
	if (load_xlib_fns_from_handle(xlib_handle)) {
		loaded++;
		return(1);
	}

#if defined(OPT_BUG_SUNOS_4)
	// While it's not strictly a bug that SunOS 4 doesn't do path
	// searching right, it's certainly a pain special to 4.x.
	// have to dlopen libX11.so from somewhere
	if (! tt_ldpath("libX11.so", xlib_path)) {
		return(0);
	}
#else
	// SVR4 version of dlopen does the path searching for us
	// (yay!). 
	xlib_path = "libX11.so";
#endif				// !SVR4

	xlib_handle = dlopen((char *)xlib_path, 1);

	if (xlib_handle == (void *)0) {
		_tt_syslog( 0, LOG_ERR, "dlopen( \"%s\", 1 ): %s",
			    xlib_path.operator const char *(),
			    dlerror() );
		return(0);
	}

	loaded = load_xlib_fns_from_handle(xlib_handle);
	if (! loaded) {
		_tt_syslog( 0, LOG_ERR, "dlsym(): %s", dlerror() );
	}
	return loaded;
}

int
_tt_load_xt()
{
	static int	loaded = 0;

	if (loaded > 0) {
		return 1;
	}

	int		load_xt_fns_from_handle(void *xlib_handle);
	int		full_ldpath(_Tt_string libname, _Tt_string &path);

	// open libXt.so and fill in _tt_xt with pointers to
	// functions
	void		*xt_handle;
	_Tt_string	xt_path;

	// first try opening the current executable to see if the
	// functions have already been defined
	xt_handle = dlopen((char *)0, 1);
	if (load_xt_fns_from_handle(xt_handle)) {
		loaded++;
		return(1);
	}

#if defined(OPT_BUG_SUNOS_4)
	// While it's not strictly a bug that SunOS 4 doesn't do path
	// searching right, it's certainly a pain special to 4.x.
	// have to dlopen libXt.so from somewhere
	if (! tt_ldpath("libXt.so", xt_path)) {
		return(0);
	}
#else
	// SVR4 version of dlopen does the path searching for us
	// (yay!). 
	xt_path = "libXt.so";
#endif				// !SVR4

	xt_handle = dlopen((char *)xt_path, 1);

	if (xt_handle == (void *)0) {
		_tt_syslog( 0, LOG_ERR, "dlopen( \"%s\", 1 ): %s",
			    xt_path.operator const char *(),
			    dlerror() );
		return(0);
	}

	loaded = load_xt_fns_from_handle(xt_handle);
	if (! loaded) {
		_tt_syslog( 0, LOG_ERR, "dlsym(): %s", dlerror() );
	}
	return loaded;
}


// 
// Dlsym the needed functions from the handle. This is done by way of a
// macro which will just return 0 if it fails. Note that if a new
// function is needed then it should be added to the _tt_xlib struct and
// also added below as well.
// 
static int
load_xlib_fns_from_handle(void *xlib_handle)
{
#if defined(__STDC__)
#define	DLINK_FN(fn)\
	_tt_xlib . fn = (_Tt_xfn_ptr)dlsym(xlib_handle, #fn);		\
	if ( _tt_xlib . fn  == (_Tt_xfn_ptr)0) {			\
		return(0);						\
	}								
#else /* __STDC__ */
#define	DLINK_FN(fn)\
	_tt_xlib . fn = (_Tt_xfn_ptr)dlsym(xlib_handle, "fn");		\
	if ( _tt_xlib . fn  == (_Tt_xfn_ptr)0) {			\
		return(0);						\
	}								
#endif /* __STDC__ */

	DLINK_FN(XOpenDisplay)
	DLINK_FN(XCloseDisplay)
	DLINK_FN(XFlush)
	DLINK_FN(XChangeProperty)
	DLINK_FN(XDeleteProperty)
	DLINK_FN(XGetWindowProperty)
	DLINK_FN(XSetIOErrorHandler)
	DLINK_FN(XInternAtom)
	DLINK_FN(XNextEvent)
	DLINK_FN(XRefreshKeyboardMapping)
	DLINK_FN(XGrabServer)
	DLINK_FN(XUngrabServer)
	DLINK_FN(XGetWindowAttributes)
	DLINK_FN(XTranslateCoordinates)
	DLINK_FN(XMoveResizeWindow)
	DLINK_FN(XRaiseWindow)
	DLINK_FN(XLowerWindow)
	DLINK_FN(XIconifyWindow)
	DLINK_FN(XMapWindow)
	DLINK_FN(XWithdrawWindow)
	DLINK_FN(XSendEvent)
	DLINK_FN(XScreenNumberOfScreen)
	DLINK_FN(XFree)

	return(1);
#undef DLINK_FN
}

// 
// Dlsym the needed functions from the handle. This is done by way of a
// macro which will just return 0 if it fails. Note that if a new
// function is needed then it should be added to the _tt_xt struct and
// also added below as well.
// 
static int
load_xt_fns_from_handle(void *xt_handle)
{
#if defined(__STDC__)
#define	DLINK_FN(fn)\
	_tt_xt . fn = (_Tt_xfn_ptr)dlsym(xt_handle, #fn);		\
	if ( _tt_xt . fn  == (_Tt_xfn_ptr)0) {				\
		return(0);						\
	}								
#define	DLINK_FNV(fn)\
	_tt_xt . fn = (void*)(_Tt_xfn_ptr)dlsym(xt_handle, #fn);	\
	if ( _tt_xt . fn  == (_Tt_xfn_ptr)0) {				\
		return(0);						\
	}
#else /* __STDC__ */
#define	DLINK_FN(fn)\
	_tt_xt . fn = (_Tt_xfn_ptr)dlsym(xt_handle, "fn");		\
	if ( _tt_xt . fn  == (_Tt_xfn_ptr)0) {				\
		return(0);						\
	}								
#endif /* __STDC__ */

	DLINK_FN(XtVaGetValues)
	DLINK_FN(XtVaSetValues)
	DLINK_FN(XtRemoveInput)
	DLINK_FN(XtDisplay)
	DLINK_FN(XtWindow)
	DLINK_FN(XtIsRealized)
	DLINK_FN(XtIsSubclass)
	DLINK_FN(XtIsSensitive)
	DLINK_FN(XtSetSensitive)
	DLINK_FN(XtAppPending)
	DLINK_FN(XtAppAddTimeOut)
	DLINK_FN(XtRemoveTimeOut)
	DLINK_FN(XtAppProcessEvent)
	DLINK_FNV(applicationShellWidgetClass)
	return(1);
}

#else				// OPT_DLOPEN_X11
/*
 * Instead of putting ifdefs around every call to _tt_load_xlib and
 * _tt_load_xt, we simply provide null versions that always return success
 * if OPT_DLOPEN_X11 is turned off.
 */

int
_tt_load_xlib()
{
	return 1;
}

int
_tt_load_xt()
{
	return 1;
}

#endif				// OPT_DLOPEN_X11
