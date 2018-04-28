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

  free((void*) value);
}



