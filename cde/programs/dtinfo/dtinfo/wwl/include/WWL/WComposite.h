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
 * $TOG: WComposite.h /main/4 1998/04/17 11:45:54 mgreess $
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
#ifndef WComposite_h
#define WComposite_h

#include "wwl.h"
#include <X11/Object.h>
#include "WObject.h"
#include <X11/RectObj.h>
#include "WRect.h"

#if 0
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#else
#include <X11/Core.h>
#include <X11/Composite.h>
#endif

#include "WCore.h"

class WComposite : public WCore {
public :
		DEFINE_INIT (WComposite, WCore, compositeWidgetClass)
		DEFINE_POPUP_INIT (WComposite, WCore, compositeWidgetClass)
inline	void	ManageChild (WRect w) const	{ XtManageChild(Widget(w)); }
	void	ManageChildren () const;
inline	void	UnmanageChild (WRect w) const	{ XtUnmanageChild(Widget(w)); }
	void	UnmanageChildren () const;
                DEFINE_GETTER (Children, WidgetList, "children")
		DEFINE_GETTER (NumChildren, Cardinal, "numChildren")
		DEFINE_GETTER_SETTER (InsertPosition,XtOrderProc,"insertPosition")
};

#define	NULLWComposite		WComposite((Widget)0)

#endif
