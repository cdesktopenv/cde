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
 *+SNOTICE
 *
 *	$TOG: Icon.h /main/6 1998/01/16 11:21:49 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef ICON_H
#define ICON_H

#include "UIComponent.h"
#include "Attachment.h"

#include <DtMail/DtMail.hh>

#include <Xm/Xm.h>

#include <Dt/Dts.h>
#include <Dt/IconFile.h>
extern "C" {
#include <Dt/Icon.h>
}

class Icon : public UIComponent {
    private:
	Boolean	 _is_selected;
	Boolean	 _is_armed;
	Pixel	_cur_fg, _cur_bg;
	void  invert(void);
	void  arm(void);
	void  disarm(void);
        int 	_indx;	/* user data */
	static void	dragMotionHandler(Widget, XtPointer, XEvent*);
	static void	iconCallback(Widget, XtPointer, XtPointer);
    protected:
	Attachment     *_parent;	// associated attachemnt

    public:
	// Constructor and destructor
	Icon (Attachment*, char*, XmString label, unsigned short, Widget, int);
	virtual		~Icon();

	// AV callback methods
	void select(void);
	void primitiveSelect(void);
	void defaultAction(void);
	void unselect();

	// associated attachment
	Attachment* 	parent() 		{ return ( _parent ); }
	virtual const char *const className() 	{ return ( "Icon" ); }

	// static functions
	static int	maxIconWidth() { return 38; }
	static int	maxIconHeight() { return 38; }
};
#endif
