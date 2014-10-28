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
 * $TOG: wwl.h /main/4 1998/04/17 11:46:09 mgreess $
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

#ifndef wwl_h
#define wwl_h

// Widget Wrapper Library by:
//
//    ___    0  Jean-Daniel Fekete            uucp  : jdf@lri.lri.fr
//   /   \  /   LRI - Bat 490                 bitnet: jdf@FRLRI61.bitnet
//  /   _/ /    Universite de Paris-Sud       voice : +33 (1) 69 41 65 91
// /__   \/     F-91405 ORSAY Cedex                   +33 (1) 69 41 66 29

#include <X11/Intrinsic.h>
#if !defined(linux) && !defined(CSRG_BASED) && !defined(sun)
#include <generic.h>
#else
#define name2(__n1,__n2)	__paste2(__n1,__n2)
#define __paste2(__p1,__p2)	__p1##__p2
#endif

#include <Xm/Xm.h>
#include "WXmString.h"

#include "WCallback.h"
#include "WTimeOut.h"
#include "WArgList.h"

#ifdef __STDC__
# define _stringify(x)	#x
#else
# define _stringify(x)	"x"
#endif

class WComposite;

#ifndef _XtintrinsicP_h
 typedef void	(* XtProc)();
#endif

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#define CASTRNAM (char*)
#define CASTVAL  (void*)(size_t)
#else
#define CASTRNAM
#define CASTVAL
#endif

#define	DEFINE_GETTER(rsc,typ,rnam) \
inline typ rsc() const \
{ Arg a; typ __value; \
  a.name = CASTRNAM rnam; a.value = (XtArgVal)&__value; \
  _Get(a); return __value; }

#define	DEFINE_SETTER(rsc,typ,rnam) \
inline void rsc(typ val) const \
{ Arg a; a.name = CASTRNAM rnam; a.value = (XtArgVal)val; _Set(a); }

#define	DEFINE_ARG(rsc,typ,rnam) \
inline WArgList& rsc(typ val, WArgList& _w) const \
{ return _w.Add (CASTRNAM rnam, CASTVAL val); }

#define	DEFINE_GETTER_SETTER(rsc,typ,rnam) \
	DEFINE_GETTER(rsc,typ,rnam) \
	DEFINE_SETTER(rsc,typ,rnam) \
	DEFINE_ARG(rsc, typ,rnam)


/*----------------------------------------------------------------------
 * This special macro is used to prevent the dangerous action of setting
 * an arg using a WXmString.  Since the member XmString of the WXmString
 * gets deleted when the WXmString gets deleted, an implicit cast to a
 * WXmString can cause a temporary to be created whose scope is undefined.
 * By using this macro, we require an XmString to be explicitly created
 * which can be freed when the arg list is no longer needed
 */

#define	DEFINE_STRING_GETSET(resource,rname)	\
	DEFINE_GETTER(resource,WXmString,rname)	\
	DEFINE_SETTER(resource,WXmString,rname)	\
	DEFINE_ARG(resource,XmString,rname)

#define	DEFINE_INIT(class_name, base_name, class) \
 \
inline  class_name() {} \
/* Constructor with widget */ \
inline	class_name (Widget w) : base_name (w) {} \
 \
/* Unmanaged Widget Constructors */	\
inline	class_name (const WComposite&	father,			\
		    WidgetClass		c	= class,	\
		    const char		*name	= _stringify(class_name), \
		    ArgList		args	= NULL,		\
		    Cardinal		card	= 0)		\
	: base_name (father, c, name, args, card) {}		\
 \
inline	class_name (const WComposite&	father,			\
		    const char		*name,			\
		    ArgList		args = NULL,		\
		    Cardinal		card = 0)		\
        : base_name (father, class, name, args, card) {}	\
 \
inline	class_name (const WComposite&	father,			\
		    const char		*name,			\
		    WArgList		args)			\
        : base_name (father, class, name, args.Args(), args.NumArgs()) {} \
 \
/* Managed Widget Constructors */ \
inline	class_name (const WComposite&	father,			\
		    int			automanage,		\
		    WidgetClass		c	= class,	\
		    const char		*name	= _stringify(class_name), \
		    ArgList		args	= NULL,		\
		    Cardinal		card	= 0)		\
        : base_name (father, automanage, c, name, args, card) {} \
 \
inline	class_name (const WComposite&	father,			\
		    const char		*name,			\
		    int			automanage,		\
		    ArgList		args = NULL,		\
		    Cardinal		card = 0)		\
        : base_name (father, automanage, class, name, args, card) {} \
 \
inline	class_name (const WComposite&	father,			\
		    const char		*name,			\
		    int			automanage,		\
		    WArgList		args)			\
        : base_name (father, automanage, class, name, args.Args(), \
		     args.NumArgs()) {} \
 \
inline	class_name& operator=(const class_name& w) { \
		       widget = w.widget; \
		       return *this; \
		    } \


#define DEFINE_POPUP_INIT(class_name, base_name, class)			  \
inline	class_name (const WComposite& father,			  \
		    Boolean popup,				  \
		    WidgetClass c = (WidgetClass)class,		  \
		    const char *name = _stringify(class_name),	  \
		    ArgList args = NULL, Cardinal card = 0)	  \
		: base_name (father, popup, c, name, args, card) {}  	  \
inline	class_name (const WComposite& father,		  	  \
		    Boolean popup,				  \
		    const char *name,				  \
		    ArgList args = NULL, Cardinal card = 0)	  \
		: base_name (father, popup, (WidgetClass)class, name, args, card) {} \
inline	class_name (const WComposite& father,		  	  \
		    Boolean popup,				  \
		    const char *name,				  \
		    WArgList args)				  \
		: base_name (father, popup, (WidgetClass)class, name, args.Args(), args.NumArgs()) {} \
inline	void Popup (XtGrabKind grab = XtGrabNone) 		  \
		{ XtPopup(widget,grab); }	       	  	  \
inline	void Popdown () { XtPopdown(widget); }

#define	DEFINE_CINIT(class_name, base_name, class)		  \
inline	class_name (Widget w) : base_name (w) {}		  \
inline	class_name (WObject& o) : base_name (o) {}


#define DEFINE_CALLBACK(cb_name, cb_res)			      \
inline	void	name2(Add,cb_name) (XtCallbackProc proc, caddr_t closure = 0) const \
		{ XtAddCallback(widget, cb_res, proc, closure); }     \
inline	void	name2(Remove,cb_name) (XtCallbackProc proc, caddr_t closure = 0) const \
		{ XtRemoveCallback(widget, cb_res, proc, closure); }     \
inline	void	name2(Call,cb_name) (caddr_t call_data) const \
		{ XtCallCallbacks(widget, cb_res, call_data); }     \
inline	void	name2(RemoveAll,cb_name) () const \
		{ XtRemoveAllCallbacks(widget, cb_res); } \
	DEFINE_GETTER_SETTER (cb_name, XtCallbackList, cb_res) \
inline	WCallback*	name2(Set,cb_name) (WWL* obj, WWL_FUN fun, void* client=0) { return SetCallback (cb_res, obj, fun, client); }

#define NULLWidget	((Widget)0)

#endif
