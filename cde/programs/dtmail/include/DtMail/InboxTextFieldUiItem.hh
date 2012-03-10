/* $TOG: InboxTextFieldUiItem.hh /main/1 1998/02/17 12:28:13 mgreess $ */
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

#ifndef _INBOXTEXTFIELDUIITEM_HH
#define _INBOXTEXTFIELDUIITEM_HH

// derived class for prop sheet glue items for textfield
///////////////////////////////////////////////////////////
class InboxTextFieldUiItem : public PropUiItem
{
public:
  InboxTextFieldUiItem(
		Widget w,
		int source,
		char *search_key,
		PropUiCallback validator = NULL,
		void * validator_data = NULL);
  virtual ~InboxTextFieldUiItem() {;}; // we don't alloc any memory

  virtual void	writeFromUiToSource();
  virtual void	writeFromSourceToUi();
};
#endif
