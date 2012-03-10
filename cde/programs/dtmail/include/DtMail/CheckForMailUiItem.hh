/* $TOG: CheckForMailUiItem.hh /main/1 1998/02/17 15:19:39 mgreess $ */
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
#include <Xm/Xm.h>

#include <DtMail/SpinBoxUiItem.hh>

#ifndef _CHECKFORMAILUIITEM_HH
#define _CHECKFORMAILUIITEM_HH

// CLASS CheckForMailUiItem
// derived class for prop sheet glue items for SpinBox
///////////////////////////////////////////////////////////
class CheckForMailUiItem : public SpinBoxUiItem {
  
public:
  static DtVirtArray<CheckForMailUiItem *> *_checkformail_ui;
  static int				 _initialized;
  int					 _valueChanged;

  CheckForMailUiItem(Widget w, int source, char *search_key);
  virtual ~CheckForMailUiItem(){;}; // we don't alloc any memory
  virtual void writeFromUiToSource();
  virtual void writeFromSourceToUi();
  static void valueChangedCB(Widget, XtPointer, XtPointer);

private:
  DtVirtArray<PropUiItem*> *_linked_pui;
};

#endif
