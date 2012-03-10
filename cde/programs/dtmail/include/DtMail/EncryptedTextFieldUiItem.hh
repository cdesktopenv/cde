/* $TOG: EncryptedTextFieldUiItem.hh /main/1 1997/11/07 15:44:39 mgreess $ */
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

#ifndef _ENCRYPTEDTEXTFIELDUIITEM_HH
#define _ENCRYPTEDTEXTFIELDUIITEM_HH

#include <stdlib.h>
#include <X11/Intrinsic.h>

#include <DtMail/DtMailTypes.h>

// derived class for prop sheet glue items for textfield
////////////////////////////////////////////////////////
class EncryptedTextFieldUiItem : public PropUiItem {
  
public:
  EncryptedTextFieldUiItem(
		Widget w,
		int source,
		char *search_key,
		PropUiCallback validator = NULL,
		void * validator_data = NULL);
  virtual ~EncryptedTextFieldUiItem() {if (NULL!=_text) free(_text);};

  void		writeAllowed(DtMailBoolean);
  virtual void	writeFromUiToSource();
  virtual void	writeFromSourceToUi();

private:
  DtMailBoolean	_loading;
  int		_maxtextlen;
  char		*_text;
  DtMailBoolean	_writeAllowed;

  void		validateLength(int);
  void		verify(XmTextVerifyPtr);
  static void	verifyCB(Widget, XtPointer, XtPointer);
};

#endif
