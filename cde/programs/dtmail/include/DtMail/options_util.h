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
/* $XConsortium: options_util.h /main/4 1996/04/21 19:45:54 drk $ */
#ifndef _OPTIONS_UTIL_H_
#define _OPTIONS_UTIL_H_

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include "DtVirtArray.hh"
#include "ListUiItem.hh"

void options_checkbox_init(
    Widget		checkbox,
    Boolean		*dirty_bit
);

int options_checkbox_set_value(
    Widget		checkbox,
    Boolean		b_value,
    Boolean		set_dirty_bit
);

void options_list_init(Widget list_w, DtVirtArray<char *> *item_list);

Boolean options_checkbox_get_value(Widget checkbox);

void options_radiobox_init(Widget radiobox,
			   int	num_items,
			   WidgetList items,
			   int *item_values,
			   Boolean *dirty_bit);


int options_radiobox_get_value(Widget radiobox);

int options_radiobox_set_value(Widget radiobox,
			       int value,
			       Boolean set_dirty_bit);

/*
 * Callback: choice value has changed...turnon dirty bit
 */
void options_setting_chgCB(Widget widget,
			   XtPointer clientdata,
			   XtPointer calldata);

/*
 * Callback: item in exclusive choice has been set...store it's
 *	     value in the parent radiobox
 */
void options_radiobox_itemCB(Widget item,
			XtPointer clientdata,
			XtPointer calldata);

typedef enum
{
        OPTIONS_LOAD,
        OPTIONS_EDIT,
        OPTIONS_NUM_VALUES
} OPTIONS_FIELD_STATE;

void options_field_init(Widget field,
			Boolean *dirty_bit);

String options_field_get_value(Widget field); 

int options_field_set_value(Widget field,
			    String value,
			    Boolean set_dirty_bit);

void options_field_chgCB(Widget widget,
		    XtPointer clientdata,
		    XtPointer calldata);

void options_spinbox_init(Widget spinbox,
			  Boolean *dirty_bit);

int options_spinbox_get_value(Widget spinbox);

int options_spinbox_set_value(Widget spinbox, 
			      int value, 
			      Boolean set_dirty_bit);

int options_scale_get_value(Widget scale);

int options_scale_set_value(Widget scale, 
			      char *value, 
			      Boolean set_dirty_bit);

void listAddButtonCB(
     Widget widget,
     XtPointer clientdata,
     XtPointer calldata);

void listDeleteButtonCB(
     Widget widget,
     XtPointer clientdata,
     XtPointer calldata);

void listChangeButtonCB(
     Widget widget,
     XtPointer clientdata,
     XtPointer calldata);

void add_cbs_to_list(ListUiItem *list,
		     Widget add_butt,
		     Widget del_butt,
		     Widget chg_butt);
/*
** The following structs are used as the glue to stick together
** the props dialog and the back end.  
*/

typedef enum {
  SPINBOX_ITEM,
  CHECKBOX_ITEM,
  TEXTFIELD_ITEM,
  LIST_ITEM,
  SCALE_ITEM
  } PROP_ITEM_TYPE;

#endif
