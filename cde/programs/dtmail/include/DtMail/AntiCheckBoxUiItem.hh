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
/* $XConsortium: AntiCheckBoxUiItem.hh /main/4 1996/04/21 19:44:20 drk $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include "CheckBoxUiItem.hh"

#ifndef _ANTICHECKBOXUIITEM_HH
#define _ANTICHECKBOXUIITEM_HH

// CLASS AntiCheckBoxUiItem
// derived class for prop sheet glue items for CheckBox
///////////////////////////////////////////////////////////
class AntiCheckBoxUiItem : public CheckBoxUiItem {
  
public:
  AntiCheckBoxUiItem(Widget w, int source, char *search_key);
  virtual ~AntiCheckBoxUiItem(){;}; // we don't alloc any memory
#ifdef DEAD_WOOD
  virtual int getType(){ return _ANTICHECKBOX_ITEM; };
  virtual int getSource(){ return data_source; };
#endif /* DEAD_WOOD */
  virtual void writeFromUiToSource();
  virtual void writeFromSourceToUi();

private:
  
#ifdef DEAD_WOOD
  int data_source;
#endif /* DEAD_WOOD */

};

#endif
