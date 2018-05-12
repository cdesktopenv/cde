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
/* $XConsortium: AlternatesListUiItem.hh /main/3 1996/04/21 19:44:17 drk $ */
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

#ifndef _ALTERNATESLISTUIITEM_HH
#define _ALTERNATESLISTUIITEM_HH

// CLASS AlternatesListUiItem
// derived class for prop sheet glue items for textfield
///////////////////////////////////////////////////////////
class AlternatesListUiItem : public ListUiItem {
  
public:
  AlternatesListUiItem(Widget w, int source, char *search_key, Widget w_list);
  virtual ~AlternatesListUiItem(){;}; // we don't alloc any memory
  virtual void writeFromUiToSource();
  virtual void writeFromSourceToUi();
  Widget getEntryFieldWidget(){ return entry_field_widget;};
  DtVirtArray<PropStringPair *> *getItemList(){ return list_items; };
  
  virtual void handleAddButtonPress();
  virtual void handleChangeButtonPress();
  virtual void handleDeleteButtonPress();

private:

  DtVirtArray<PropStringPair *> *list_items;
  DtVirtArray<char *> *deleted_items;
  Widget entry_field_widget;
};

#endif
