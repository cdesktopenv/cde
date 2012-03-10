/*
 * $XConsortium: WWMShell.h /main/3 1996/06/11 16:59:06 cde-hal $
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
#ifndef WWMShell_h
#define WWMShell_h

#include "WShell.h"
#include <X11/Shell.h>

class WWMShell : public WShell {
public :
		DEFINE_INIT (WWMShell, WShell, wmShellWidgetClass)
		DEFINE_POPUP_INIT (WWMShell, WShell, wmShellWidgetClass)
	DEFINE_GETTER_SETTER (Title,char*,"title")
	DEFINE_GETTER_SETTER (WmTimeout,int,"wmTimeout")
	DEFINE_GETTER_SETTER (Waitforwm,Boolean,"waitforwm")
	DEFINE_GETTER_SETTER (Transient,Boolean,"transient")
	DEFINE_GETTER_SETTER (MinWidth,int,"minWidth")
	DEFINE_GETTER_SETTER (MinHeight,int,"minHeight")
	DEFINE_GETTER_SETTER (MaxWidth,int,"maxWidth")
	DEFINE_GETTER_SETTER (MaxHeight,int,"maxHeight")
	DEFINE_GETTER_SETTER (WidthInc,int,"widthInc")
	DEFINE_GETTER_SETTER (HeightInc,int,"heightInc")
	DEFINE_GETTER_SETTER (MinAspectX,int,"minAspectX")
	DEFINE_GETTER_SETTER (MinAspectY,int,"minAspectY")
	DEFINE_GETTER_SETTER (MaxAspectX,int,"maxAspectX")
	DEFINE_GETTER_SETTER (MaxAspectY,int,"maxAspectY")
	DEFINE_GETTER_SETTER (Input,Boolean,"input")
	DEFINE_GETTER_SETTER (InitialState,int,"initialState")
	DEFINE_GETTER_SETTER (IconPixmap,Pixmap,"iconPixmap")
	DEFINE_GETTER_SETTER (IconWindow,Widget,"iconWindow")
	DEFINE_GETTER_SETTER (IconX,int,"iconX")
	DEFINE_GETTER_SETTER (IconY,int,"iconY")
	DEFINE_GETTER_SETTER (IconMask,Pixmap,"iconMask")
	DEFINE_GETTER_SETTER (WindowGroup,Widget,"windowGroup")
};

#define	NULLWWMShell		WWMShell((Widget)0)

#endif
