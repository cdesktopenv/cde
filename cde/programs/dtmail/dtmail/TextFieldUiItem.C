/* $TOG: TextFieldUiItem.C /main/7 1997/11/07 15:48:41 mgreess $ */
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

#include <DtMail/options_util.h>
#include <DtMail/PropUi.hh>
#include <DtMail/TextFieldUiItem.hh>

////////////////////////////////////////////////////////////////////
TextFieldUiItem::TextFieldUiItem(
				Widget w,
				int source,
				char *search_key,
				PropUiCallback validator,
				void * validator_data)
: PropUiItem(w, source, search_key, validator, validator_data)
{
  options_field_init(w, &(this->dirty_bit));
}

// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void TextFieldUiItem::writeFromUiToSource()
{
  char 	*textfield_value;
  Widget w = this->getWidget();

  textfield_value = options_field_get_value(w);
  prop_source->setValue(textfield_value);
}

// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void TextFieldUiItem::writeFromSourceToUi()
{
  char *value;
  Widget w = this->getWidget();

  value = (char *)prop_source->getValue();
  options_field_set_value(w, value, this->dirty_bit);
  if (NULL != value) free((void*) value);
}
