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
 * $XConsortium: WXmLabelGadget.h /main/3 1996/06/11 17:00:30 cde-hal $
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
#ifndef WXmLabelGadget_h
#define WXmLabelGadget_h

#include "WXmGadget.h"
#include <Xm/LabelG.h>

class WXmLabelGadget : public WXmGadget {
public :
		DEFINE_INIT (WXmLabelGadget, WXmGadget, xmLabelGadgetClass)
inline	void Accelerator(char* key, WXmString s) const	{ Accelerator(key); AcceleratorText(s); }
	DEFINE_GETTER_SETTER (ShadowThickness,short,"shadowThickness")
	DEFINE_GETTER_SETTER (Alignment,unsigned char,"alignment")
	DEFINE_GETTER_SETTER (LabelType,unsigned char,"labelType")
	DEFINE_GETTER_SETTER (MarginWidth,short,"marginWidth")
	DEFINE_GETTER_SETTER (MarginHeight,short,"marginHeight")
	DEFINE_GETTER_SETTER (MarginLeft,short,"marginLeft")
	DEFINE_GETTER_SETTER (MarginRight,short,"marginRight")
	DEFINE_GETTER_SETTER (MarginTop,short,"marginTop")
	DEFINE_GETTER_SETTER (MarginBottom,short,"marginBottom")
	DEFINE_GETTER_SETTER (FontList,XmFontList,"fontList")
	DEFINE_GETTER_SETTER (LabelPixmap,Pixmap,"labelPixmap")
	DEFINE_GETTER_SETTER (LabelInsensitivePixmap,Pixmap,"labelInsensitivePixmap")
	DEFINE_STRING_GETSET (LabelString,"labelString")
	DEFINE_GETTER_SETTER (Mnemonic,char,"mnemonic")
	DEFINE_GETTER_SETTER (Accelerator,char*,"accelerator")
	DEFINE_STRING_GETSET (AcceleratorText,"acceleratorText")
	DEFINE_GETTER_SETTER (RecomputeSize,Boolean,"recomputeSize")
	DEFINE_GETTER_SETTER (StringDirection,unsigned char,"stringDirection")
};

#define	NULLWXmLabelGadget		WXmLabelGadget((Widget)0)

#endif
