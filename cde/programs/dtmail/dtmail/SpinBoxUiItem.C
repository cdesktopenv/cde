/* $TOG: SpinBoxUiItem.C /main/5 1997/04/29 16:38:00 mgreess $ */
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
#include <DtMail/SpinBoxUiItem.hh>

// SpinBoxUiItem::SpinBoxUiItem
// SpinBoxUiItem ctor
////////////////////////////////////////////////////////////////////

SpinBoxUiItem::SpinBoxUiItem(Widget w, int source, char *search_key):PropUiItem(w, source, search_key)
{
#ifdef DEAD_WOOD
  data_source = source;
#endif /* DEAD_WOOD */

  options_spinbox_init(w, &(this->dirty_bit));

}

// SpinBoxUiItem::writeFromUiToSource()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void SpinBoxUiItem::writeFromUiToSource()
{
  int	spinbox_value;
  Widget w = this->getWidget();
  char val_str[64];

  spinbox_value = options_spinbox_get_value(w);

  sprintf(val_str, "%d", spinbox_value);

  prop_source->setValue(val_str);

}

// SpinBoxUiItem::writeFromSourceToUi()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void SpinBoxUiItem::writeFromSourceToUi()
{
  const char *value;
  Widget w = this->getWidget();

  value = prop_source->getValue();

  options_spinbox_set_value(w, atoi(value), this->dirty_bit);

  if (NULL != value)
    free((void*) value);
}



