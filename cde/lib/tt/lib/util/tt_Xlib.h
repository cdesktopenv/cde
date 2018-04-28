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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_Xlib.h /main/3 1995/10/23 10:36:35 rswiston $ 			 				 */
//%% 	$XConsortium: tt_Xlib.h /main/3 1995/10/23 10:36:35 rswiston $							
//%% 									
//%%  	RESTRICTED CONFIDENTIAL INFORMATION:                            
//%% 									
//%% 	The information in this document is subject to special		
//%% 	restrictions in a confidential disclosure agreement between	
//%% 	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this	
//%% 	document outside HP, IBM, Sun, USL, SCO, or Univel without	
//%% 	Sun's specific written approval.  This document and all copies	
//%% 	and derivative works thereof must be returned or destroyed at	
//%% 	Sun's request.							
//%% 									
//%% 	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.	
//%% 									
/* -*-C++-*-
 *
 * @(#)tt_Xlib.h	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */

#ifndef _TT_XLIB_H
#define _TT_XLIB_H

#include "tt_options.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>

typedef int (*_Tt_xfn_ptr)(...);

typedef struct {
	_Tt_xfn_ptr	XOpenDisplay;
	_Tt_xfn_ptr	XCloseDisplay;
	_Tt_xfn_ptr	XFlush;
	_Tt_xfn_ptr	XChangeProperty;
	_Tt_xfn_ptr	XDeleteProperty;
	_Tt_xfn_ptr	XGetWindowProperty;
	_Tt_xfn_ptr	XSetIOErrorHandler;
	_Tt_xfn_ptr	XInternAtom;
	_Tt_xfn_ptr	XNextEvent;
	_Tt_xfn_ptr	XRefreshKeyboardMapping;
	_Tt_xfn_ptr	XGrabServer;
	_Tt_xfn_ptr	XUngrabServer;
	_Tt_xfn_ptr	XGetWindowAttributes;
	_Tt_xfn_ptr	XTranslateCoordinates;
	_Tt_xfn_ptr	XMoveResizeWindow;
	_Tt_xfn_ptr	XRaiseWindow;
	_Tt_xfn_ptr	XLowerWindow;
	_Tt_xfn_ptr	XIconifyWindow;
	_Tt_xfn_ptr	XMapWindow;
	_Tt_xfn_ptr	XWithdrawWindow;
	_Tt_xfn_ptr	XSendEvent;
	_Tt_xfn_ptr	XScreenNumberOfScreen;
	_Tt_xfn_ptr	XFree;
} _Tt_xlib;

typedef struct {
	_Tt_xfn_ptr	XtVaGetValues;
	_Tt_xfn_ptr	XtVaSetValues;
	_Tt_xfn_ptr	XtRemoveInput;
	_Tt_xfn_ptr	XtDisplay;
	_Tt_xfn_ptr	XtWindow;
	_Tt_xfn_ptr	XtIsRealized;
	_Tt_xfn_ptr	XtIsSubclass;
	_Tt_xfn_ptr	XtIsSensitive;
	_Tt_xfn_ptr	XtSetSensitive;
	_Tt_xfn_ptr	XtAppPending;
	_Tt_xfn_ptr	XtAppAddTimeOut;
	_Tt_xfn_ptr	XtRemoveTimeOut;
	_Tt_xfn_ptr	XtAppProcessEvent;
	void	       *applicationShellWidgetClass;
} _Tt_xt;

#ifdef OPT_DLOPEN_X11

// 
// If this option is turned on then we create a struct containing
// function pointers to all the X11 functions we use. If they are needed
// then this functions will be dlsym'ed from a libX11.so library. This
// allows executables based on this mechanism to not require libX11.so to
// be linked in.
//
// The macro CALLX11 below should be used to call an X11 function.  If
// OPT_DLOPEN_X11 is turned on then it will dereference the appropiate
// member in the struct and invoke it as a function. Otherwise it just
// calls the X11 function directly.

#	define CALLX11(fn) (*_tt_xlib . fn)
#	define CALLXT(fn) (*_tt_xt . fn)
#	define XTSYM(sym) (_tt_xt . sym)
	extern _Tt_xlib	_tt_xlib;
	extern _Tt_xt	_tt_xt;
#else
#	define CALLX11(fn) fn
#	define CALLXT(fn) fn
#	define XTSYM(sym) sym
#endif

extern int		_tt_load_xlib();
extern int		_tt_load_xt();

#endif
