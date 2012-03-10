/* $TOG: AntiCheckBoxUiItem.C /main/4 1997/04/29 15:57:39 mgreess $ */
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
#include <DtMail/AntiCheckBoxUiItem.hh>

// AntiCheckBoxUiItem::AntiCheckBoxUiItem
// AntiCheckBoxUiItem ctor
////////////////////////////////////////////////////////////////////

AntiCheckBoxUiItem::AntiCheckBoxUiItem(
				       Widget w, 
				       int source, 
				       char *search_key
				       ):CheckBoxUiItem(w, source, search_key)
{
  ;
}

// AntiCheckBoxUiItem::writeFromUiToSource()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void AntiCheckBoxUiItem::writeFromUiToSource()
{
  Boolean	checkbox_value;
  Widget w = this->getWidget();
  PropSource 	*p_s = this->getPropSource();

  checkbox_value = options_checkbox_get_value(w);
   
  if(checkbox_value == TRUE) // make sure the value is in the table
    {
      p_s->setValue("f");
    }	
  else
    {
      p_s->setValue("");
    }
}

// AntiCheckBoxUiItem::writeFromSourceToUi()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void AntiCheckBoxUiItem::writeFromSourceToUi()
{
  char *value = NULL;
  Widget w = this->getWidget();

  PropSource 	*p_s = this->getPropSource();

  value = (char *)p_s->getValue();

  // 
  // this will have to be made more robust... 
  //
  // This assumes that a non-null value means that the 
  // value is set and that a non-null means turn on the CB
  if (strcmp(value, "f") == 0)
    options_checkbox_set_value(w, TRUE, this->dirty_bit);
  else if (value == NULL || strcmp(value, "") == 0)
    options_checkbox_set_value(w, FALSE, this->dirty_bit);
  
  if (NULL != NULL)
    free((void*) value);
}



