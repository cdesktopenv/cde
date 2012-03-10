/* $XConsortium: MoveMenuListUiItem.hh /main/3 1996/04/21 19:45:22 drk $ */
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

#ifndef _MOVEMENULISTUIITEM_HH
#define _MOVEMENULISTUIITEM_HH

void handleMMSelection(Widget, XtPointer, XtPointer);

// CLASS MoveMenuListUiItem
// derived class for prop sheet glue items for textfield
///////////////////////////////////////////////////////////
class MoveMenuListUiItem : public ListUiItem {
  
public:
  MoveMenuListUiItem(Widget w, int source, char *search_key, Widget w_list);
  virtual ~MoveMenuListUiItem(){;}; // we don't alloc any memory
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
