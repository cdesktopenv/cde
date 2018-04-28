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
/* $TOG: ListUiItem.C /main/4 1997/04/29 16:23:46 mgreess $ */
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
#include <DtMail/ListUiItem.hh>

// ListUiItem::ListUiItem
// ListUiItem ctor
////////////////////////////////////////////////////////////////////

ListUiItem::ListUiItem(Widget w, 
		       int source, 
		       char *search_key,   
		       DtVirtArray<char *> *alias_list
		       ):PropUiItem(w, source, search_key)
{
#ifdef DEAD_WOOD
  data_source = source;
#endif /* DEAD_WOOD */
  
  if(alias_list != NULL)
     options_list_init(w, alias_list);

}

// ListUiItem::writeFromUiToSource()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void ListUiItem::writeFromUiToSource()
{
//  char 	*textfield_value;
  Widget w = this->getWidget();

//   textfield_value = options_field_get_value(w);
   
//   prop_source->setValue(textfield_value);
}

// ListUiItem::writeFromSourceToUi()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void ListUiItem::writeFromSourceToUi()
{
//  char *value;
  Widget w = this->getWidget();

//   value = (char *)prop_source->getValue();

//   options_field_set_value(w, value, this->dirty_bit);

}
void ListUiItem::AddDefaults()
{
// wrapper

}




