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
/* $TOG: IndexedOptionMenuUiItem.C /main/4 1997/11/21 17:19:04 mgreess $ */
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
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */


#include <DtMail/options_util.h>
#include <DtMail/PropUi.hh>
#include <DtMail/IndexedOptionMenuUiItem.hh>

extern Boolean props_changed;

// IndexedOptionMenuUiItem::IndexedOptionMenuUiItem
// IndexedOptionMenuUiItem ctor
////////////////////////////////////////////////////////////////////

IndexedOptionMenuUiItem::IndexedOptionMenuUiItem(
			IndexedOptionMenu *iom,
			int source,
			char *search_key
			) : PropUiItem(iom->baseWidget(), source, search_key)
{
  _iom = iom;
  _iom->addMenuButtonCallback(
			XmNactivateCallback,
			IndexedOptionMenuUiItem::valueChangedCB,
			(XtPointer) this);
}

// IndexedOptionMenuUiItem::writeFromUiToSource()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void IndexedOptionMenuUiItem::writeFromUiToSource()
{
  char 	*value;
  Widget w;
  
  w = this->getWidget();
  value = (char*) _iom->getDataSpec();
  prop_source->setValue(value);
}

// IndexedOptionMenuUiItem::writeFromSourceToUi()
// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void IndexedOptionMenuUiItem::writeFromSourceToUi()
{
  const char *value;
  
  value = (char *)prop_source->getValue();
  _iom->setSpec((void*) value);
  if (NULL != value)
    free((void*) value);
}

// IndexedOptionMenuUiItem::valueChangedCB()
// Marks the item as having its value changed.
///////////////////////////////////////////////////////////////////
void IndexedOptionMenuUiItem::valueChangedCB(
				Widget,
				XtPointer clientdata,
				XtPointer
				)
{
    IndexedOptionMenuUiItem	*thisIomui;

    thisIomui = (IndexedOptionMenuUiItem *) clientdata;
    thisIomui->dirty_bit = TRUE;
    props_changed = TRUE;
}

