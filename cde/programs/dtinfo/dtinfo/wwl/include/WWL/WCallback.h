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
 *	WWL		(c) Copyright LRI 1990
 *      Copyright (c) 1990, 1991 Jean-Daniel Fekete
 *      Copyright (c) 1990, 1991 LRI, Universitee de Paris-Sud (France)
 *
 *      Permission to use, copy, modify and distribute this software
 *      and its documentation for any purpose and without fee is hereby
 *      granted, provided that the above copyright notice appears in
 *      all copies of the software. This software is provided "as-is"
 *      without express or implied warranty.
 *
 *	C++ Callbacks
 *
 *	$XConsortium: WCallback.h /main/4 1996/06/11 16:58:00 cde-hal $
 */

#ifndef _WCallback_h
#define _WCallback_h

class WWL;
class WCallback;
class WObject;
typedef void (WWL::* WWL_FUN) (WCallback*);

class WWL {
protected:
	WCallback*	cbList;
public:
	WWL ();
	virtual ~WWL ();
	
	void		AddCb (WCallback *);
	Boolean		RemoveCb (WCallback *);
	void		removeCallbacks();
private:

	virtual void __dummy();
	/* Without a virtual function in the WWL base class, the C++
	   translator cfront will not generate the correct code to
	   call virtual callback functions because it doesn't think
	   that virtuals can be called and only generates code to
	   call non-virtual memebers. */
};

class WCallback {
protected:
	WCallback*	next;
	Widget		widget;
	const char*	name;
	WWL*		object;
	WWL_FUN		fun;
	caddr_t		call_data;
	caddr_t		client_data;
	WObject*	wobject;
	
public:
	WCallback (Widget, const char*, WWL*, WWL_FUN, void* client=0);
	WCallback (WObject&, const char*, WWL*, WWL_FUN, void* client=0);
	WCallback (Widget, Atom, WWL*, WWL_FUN, void* client=0);
	WCallback (WObject&, Atom, WWL*, WWL_FUN, void* client=0);
	~WCallback ();
	
	void	Call (caddr_t);
	
inline	WWL_FUN	Fun ()			{ return fun; }
inline	void		Fun (WWL_FUN f)	{ fun = f; }
inline	WWL*		Obj ()			{ return object; }
inline	void		Obj (WWL* o)		{ o->AddCb (this); }
inline	caddr_t		CallData ()		{ return call_data; }
inline	caddr_t		ClientData ()		{ return client_data; }
inline	Widget		GetWidget ()		{ return widget; }
//  inline Widget	Widget()		{ return widget; }	
inline  WObject*	GetWObject ()		{ return wobject; }
//  inline  WObject*	WObject()		{ return wobject; }
friend	class WWL;
};

#endif
