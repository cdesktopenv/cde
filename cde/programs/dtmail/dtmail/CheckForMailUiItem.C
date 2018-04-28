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
/* $TOG: CheckForMailUiItem.C /main/1 1998/02/17 15:19:56 mgreess $ */
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

#include <X11/Intrinsic.h>
#include <Xm/XmAll.h>
#include <DtMail/options_util.h>
#include <DtMail/PropUi.hh>
#include <DtMail/CheckForMailUiItem.hh>
#include <DtMail/SpinBoxUiItem.hh>

int				  CheckForMailUiItem::_initialized = 0;
DtVirtArray<CheckForMailUiItem*> *CheckForMailUiItem::_checkformail_ui = NULL;

CheckForMailUiItem::CheckForMailUiItem(Widget w, int source, char *search_key):
SpinBoxUiItem(w, source, search_key)
{
    Widget textfield;

    if (! _initialized)
    {
	_initialized = 1;
	_checkformail_ui = new DtVirtArray<CheckForMailUiItem*> (3);
    }

    options_spinbox_init(w, &(this->dirty_bit));
    XtVaGetValues(w, XmNtextField, &textfield, NULL);
    XtAddCallback(
		textfield,
		XmNvalueChangedCallback,
		CheckForMailUiItem::valueChangedCB,
		this);

    _checkformail_ui->append(this);
}

//
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void CheckForMailUiItem::valueChangedCB(Widget w, XtPointer client, XtPointer)
{
    CheckForMailUiItem *cui = (CheckForMailUiItem*) client;
    cui->_valueChanged = TRUE;
}

//
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void CheckForMailUiItem::writeFromUiToSource()
{
    int	spinbox_value;
    Widget w = this->getWidget();
    char val_str[64];

    if (!_valueChanged) return;

    spinbox_value = options_spinbox_get_value(w);
    sprintf(val_str, "%d", spinbox_value);
    prop_source->setValue(val_str);

    for (int i = 0; i < _checkformail_ui->length(); i++)
    {
	CheckForMailUiItem *cui = (*_checkformail_ui)[i];
	if (cui != this) cui->writeFromSourceToUi();
    }
    _valueChanged = FALSE;
}

//
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void CheckForMailUiItem::writeFromSourceToUi()
{
    const char *value;
    Widget w = this->getWidget();

    value = prop_source->getValue();
    options_spinbox_set_value(w, atoi(value), this->dirty_bit);
    _valueChanged = FALSE;

    free((void*) value);
}
