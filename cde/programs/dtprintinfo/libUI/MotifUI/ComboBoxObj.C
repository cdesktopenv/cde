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
/* $XConsortium: ComboBoxObj.C /main/4 1995/11/06 09:39:21 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "ComboBoxObj.h"

#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/ComboBox.h>

ComboBoxObj::ComboBoxObj(MotifUI *parent, ComboBoxCallback callback,
                         char *name, 
			 char **items, 
			 int n_items)
	: MotifUI(parent, name, NULL)
{
   XmString *items_list;
   int itemCount;
   if (items && n_items)
    {
      items_list = new XmString [n_items];
      int i;
      if (n_items > 8)
	 itemCount = 8;
      else
	 itemCount = n_items;
      for (i = 0; i < n_items; i++)
         items_list[i] = StringCreate(items[i]);
    }
   else
      items_list = NULL;
   _callback = callback;
   if (name)
    {
      XmString xm_string = StringCreate(name);
      _w = XtVaCreateManagedWidget("form", xmFormWidgetClass, 
				   parent->InnerWidget(), NULL);
      _label = XtVaCreateManagedWidget("form", xmLabelWidgetClass, 
				       _w, XmNlabelString, xm_string,
				       XmNtopAttachment, XmATTACH_FORM, 
				       XmNbottomAttachment, XmATTACH_FORM, 
				       XmNleftAttachment, XmATTACH_FORM, NULL); 
      _combo_box = XtVaCreateManagedWidget("combo_box", xmComboBoxWidgetClass, 
				           _w, XmNleftWidget, _label,
				           XmNleftAttachment, XmATTACH_WIDGET,
				           XmNrightAttachment, XmATTACH_FORM,
				           XmNtopAttachment, XmATTACH_FORM, 
				           XmNbottomAttachment, XmATTACH_FORM, 
					   XmNcomboBoxType, XmDROP_DOWN_LIST,
					   (items_list ? XmNitems : NULL),
					      items_list,
					   XmNitemCount, n_items,
					   XmNvisibleItemCount, itemCount, 
					   NULL); 
      StringFree(xm_string);
    }
   else
    {
      _w = XtVaCreateManagedWidget("combo_box", xmComboBoxWidgetClass, 
				   parent->InnerWidget(),
				   XmNcomboBoxType, XmDROP_DOWN_LIST,
				   (items_list ? XmNitems : NULL), items_list,
				   XmNitemCount, n_items,
		                   XmNvisibleItemCount, itemCount, NULL); 
      _label = NULL;
      _combo_box = _w;
    }
   _list = XtNameToWidget(_combo_box, "*List");
   XtAddCallback(_combo_box, XmNselectionCallback, &ComboBoxObj::SelectCB, 
		 (XtPointer)this);
   if (items_list)
    {
      int i;
      for (i = 0; i < n_items; i++)
         StringFree(items_list[i]);
      delete items_list;
    }
}

char * ComboBoxObj::Item(int item)
{
   XmString *items;
   int n_items;
   XtVaGetValues(_list, XmNitems, &items, XmNitemCount, &n_items, NULL);
   if (item < 0 || item > n_items - 1)
      return NULL;
   else
      return StringExtract(items[item + 1]);
}

char * ComboBoxObj::Item(char *item)
{
   XmString value = StringCreate(item);
   int n = XmListItemExists(_list, value);
   StringFree(value);
   if (n)
      return item;
   else
      return NULL;
}

void ComboBoxObj::Add(char *item)
{
   XmString value = StringCreate(item);
   XmComboBoxAddItem(_combo_box, value, 0, True);
   StringFree(value);
}

void ComboBoxObj::Delete(char *item)
{
   XmString value = StringCreate(item);
   int n = XmListItemPos(_list, value);
   if (n)
      XmComboBoxDeletePos(_combo_box, n);
   StringFree(value);
}

void ComboBoxObj::SelectCB(Widget, XtPointer client_data, XtPointer call_data)
{
   ComboBoxObj *obj = (ComboBoxObj *) client_data;
   XmComboBoxCallbackStruct * cb = (XmComboBoxCallbackStruct *)call_data;
   if (obj->_callback)
    {
      char *item = obj->StringExtract(cb->item_or_text);
      (*obj->_callback)(obj, item, cb->item_position);
      XtFree(item);
    }
}
