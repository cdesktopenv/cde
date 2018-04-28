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
 * $XConsortium: WXmFileSelectionBox.h /main/3 1996/06/11 17:00:01 cde-hal $
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
#ifndef WXmFileSelectionBox_h
#define WXmFileSelectionBox_h

#include "WXmSelectionBox.h"
#include <Xm/FileSB.h>

class WXmFileSelectionBox : public WXmSelectionBox {
public :
   DEFINE_INIT (WXmFileSelectionBox, WXmSelectionBox,
	        xmFileSelectionBoxWidgetClass)
inline	Widget	GetChild (unsigned char child) const	{ return XmFileSelectionBoxGetChild (widget, child); }
   DEFINE_GETTER_SETTER (AutoUnmanage,Boolean,"autoUnmanage")
   DEFINE_STRING_GETSET (ListLabelString,"listLabelString")
   DEFINE_STRING_GETSET (ApplyLabelString,"applyLabelString")
   DEFINE_STRING_GETSET (DirSpec,"dirSpec")
   DEFINE_GETTER_SETTER (DialogType,unsigned char,"dialogType")
   DEFINE_STRING_GETSET (FilterLabelString,"filterLabelString")
   DEFINE_STRING_GETSET (DirMask,"dirMask")
   DEFINE_GETTER_SETTER (FileSearchProc,XtProc,"fileSearchProc")
   DEFINE_GETTER_SETTER (ListUpdated,Boolean,"listUpdated")
   DEFINE_STRING_GETSET (Pattern,"pattern")
};

#define	NULLWXmFileSelectionBox		WXmFileSelectionBox((Widget)0)

class CXmFileSelectionBox : public CXmSelectionBox {
public :
   DEFINE_CINIT (CXmFileSelectionBox, CXmSelectionBox,
		 xmFileSelectionBoxWidgetClass)
};

#endif
