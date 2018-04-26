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
