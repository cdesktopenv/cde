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
 *	C++ Time Outs
 *
 *	$XConsortium: WTimeOut.h /main/3 1996/06/11 16:58:44 cde-hal $
 */

#ifndef _WTimeOut_h
#define _WTimeOut_h

class WWL;
class WTimeOut;

typedef void (WWL::* WTimeOutFunc) (WTimeOut *);

// WARNING: Do not create temporary WTimeOut objects or you'll be sorry.
//          This means that WTimeOut objects MUST be created with new!!

// WTimeOut objects are automatically deleted after the timeout proc is called.
// Delete the WTimeOut object to cancel the timeout.

// -- DJB 10/06/92

class WTimeOut {
protected:
	WWL            *object;
	WTimeOutFunc    func;
	XtIntervalId    interval_id;
	XtPointer	client_data;
	
public:
	WTimeOut (XtAppContext, unsigned long interval,
		  WWL *, WTimeOutFunc, XtPointer client = NULL);
	~WTimeOut();

	void	Call();
	
inline	WTimeOutFunc    Fun()			{ return func; }
inline	void		Fun (WTimeOutFunc f)	{ func = f; }
inline	WWL*		Obj()			{ return object; }
inline	void		Obj (WWL *o)		{ object = o; }
inline	XtPointer	ClientData()		{ return client_data; }
};

#endif
