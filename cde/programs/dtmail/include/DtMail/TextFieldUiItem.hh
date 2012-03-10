/* $TOG: TextFieldUiItem.hh /main/6 1997/11/07 15:45:26 mgreess $ */
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

#ifndef _TEXTFIELDUIITEM_HH
#define _TEXTFIELDUIITEM_HH

// derived class for prop sheet glue items for textfield
///////////////////////////////////////////////////////////
class TextFieldUiItem : public PropUiItem
{
public:
  TextFieldUiItem(
		Widget w,
		int source,
		char *search_key,
		PropUiCallback validator = NULL,
		void * validator_data = NULL);
  virtual ~TextFieldUiItem() {;}; // we don't alloc any memory

  virtual void	writeFromUiToSource();
  virtual void	writeFromSourceToUi();
};
#endif
