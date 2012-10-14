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
/*
 * $TOG: WObject.h /main/6 1997/12/29 10:29:50 bill $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

// This code is automatically generated in -*- C++ -*-
#ifndef WObject_h
#define WObject_h

#include "wwl.h"
#include <X11/Object.h>

class WObject {

#ifdef __osf__
  public:
#else
  protected :
#endif
	Widget	widget;
public :
inline          WObject () { widget = NULL; }
inline		WObject (Widget w) {
		   widget = w;
		}
inline		WObject (const WObject& w) {
		   widget = w.widget;
		}
		WObject (const WComposite& super, WidgetClass c,
			 const char* name = NULL, ArgList args = NULL,
			 Cardinal card = 0);
		WObject (const WComposite& super, int automanage,
			 WidgetClass c, const char* name = NULL,
			 ArgList args = NULL, Cardinal card = 0);
		WObject (const WComposite& super, Boolean popup, WidgetClass c,
			 const char* name = NULL, ArgList args = NULL,
			 Cardinal card = 0);

inline		operator Widget () const { return widget; }
#ifndef DEC
inline	int	operator== (int i)	{ return (Widget)(size_t)i == widget; }
inline	int	operator!= (int i)	{ return (Widget)(size_t)i != widget; }
#endif
inline  WObject &operator = (const Widget w) { widget = w; return *this; }
inline	WidgetClass Class () const	{ return XtClass (widget); }
inline	void	Realize () const	{ XtRealizeWidget (widget); }
inline	void	Unrealize () const	{ XtUnrealizeWidget (widget); }
inline	Boolean	IsRealized () const	{ return XtIsRealized (widget); }
inline	void	Destroy () const	{ XtDestroyWidget (widget); }
inline  XtAppContext AppContext () const
  { return (XtWidgetToApplicationContext (widget)); }

inline	Arg&	Get (Arg& a) const {
		   XtGetValues (widget, &a, 1);
		   return a;
		}

inline	Arg&	_Get (Arg& a) const {
		   XtGetValues (widget, &a, 1);
		   return a;
		}

inline	void	Get (ArgList a, Cardinal c) const {
		   XtGetValues (widget, a, c);
		}

inline	const WArgList& Get (const WArgList& wa) const {
		   XtGetValues (widget, wa.args, wa.num_args);
		   return wa;
	        }

inline	Arg&	Set (Arg& a) const {
		   XtSetValues (widget, &a, 1);
		   return a;
		}

inline	Arg&	_Set (Arg& a) const {
		   XtSetValues (widget, &a, 1);
		   return a;
		}

inline	void	Set (ArgList a, Cardinal c) const {
		   XtSetValues (widget, a, c);
		}

inline	void	Set (const WArgList& wa) const {
		   XtSetValues (widget, wa.args, wa.num_args);
		}

inline	void	AddCallback (const char* name, XtCallbackProc proc,
			     caddr_t closure = NULL) const {
	           XtAddCallback (widget, name, proc, closure);
		}

inline	void	RemoveCallback (const char* name, XtCallbackProc proc,
				caddr_t closure = NULL) const {
		   XtRemoveCallback (widget, name, proc, closure);
		}

inline	void	AddCallbacks (const char* name, XtCallbackList callbacks)
	        const {
		   XtAddCallbacks (widget, name, callbacks);
		}

inline	void	RemoveCallbacks (const char* name, XtCallbackList callbacks)
	        const {
		   XtRemoveCallbacks (widget, name, callbacks);
		}

inline	void	RemoveAllCallbacks (const char* name) const {
		   XtRemoveAllCallbacks (widget, name);
		}

inline	void	CallCallbacks (const char* name, caddr_t call_data) const {
		   XtCallCallbacks (widget, name, call_data);
		}

inline	WCallback*	_SetCallback (const char* name, WWL* obj, WWL_FUN fun,
				      void* client=0) {
		   return new WCallback (*this, name, obj, fun, client);
		}

inline	WCallback*	SetCallback (const char* name, WWL* obj, WWL_FUN fun,
				     void* client=0) {
		   return new WCallback (*this, name, obj, fun, client);
		}
		DEFINE_CALLBACK (DestroyCallback,"destroyCallback")
};

#define	NULLWObject		WObject((Widget)0)

#endif
