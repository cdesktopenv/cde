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
/* $XConsortium: ComboBoxObj.h /main/3 1995/11/06 09:39:32 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef COMBOBOXOBJ_H
#define COMBOBOXOBJ_H

#include "MotifUI.h"

class ComboBoxObj;

typedef void (*ComboBoxCallback) (ComboBoxObj *, char *item, int item_position);

class ComboBoxObj : public MotifUI {

  friend void SelectCB(Widget, XtPointer, XtPointer);

 private:

  static void SelectCB(Widget, XtPointer, XtPointer);

  Widget _label;
  Widget _list;
  Widget _combo_box;
  ComboBoxCallback _callback;

 public:

   ComboBoxObj(MotifUI * parent, ComboBoxCallback, char *name, 
	       char **items, int n_items);

   void Add(char *item);
   void Delete(char *item);
   char * Item(char *item);
   char * Item(int index);

   const Widget InnerWidget()         { return _combo_box; }
   const UI_Class UIClass()           { return COMBO_BOX; }
   const char *const UIClassName()    { return "ComboBox"; }

};

#endif /* COMBOBOXOBJ_H */
