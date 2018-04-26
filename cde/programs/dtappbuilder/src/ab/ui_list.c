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

/*
 *	$XConsortium: ui_list.c /main/3 1995/11/06 17:55:59 rswiston $
 *
 * @(#)ui_list.c	1.9 01 May 1995 cde_app_builder/src/ab
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * File: ui_list.c - implements XmList functions declared in ui_util.h
 */

#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Dt/ComboBox.h>
#include <ab_private/objxm.h>
#include <ab_private/ui_util.h>

/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Data     	                                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/

Widget
ui_combobox_get_list_widget(
    Widget	combobox
)
{
    Widget	list;
    list = XtNameToWidget(combobox, "*List");
    return (list);
}

Widget
ui_combobox_get_field_widget(
    Widget      combobox
)
{
    Widget      textf;
    textf = XtNameToWidget(combobox, "*Text");
    return (textf);
}

int
ui_combobox_clear(
    Widget	combobox
)
{
    Widget	list;
    int		i, num_items;

    list = ui_combobox_get_list_widget(combobox);
    XtVaGetValues(list,
	XmNitemCount,	&num_items,
	NULL);
    
    /* REMIND: Cannot delete last item in ComboBox because of
     * bug in ComboBox widget (CMVC#4633)
     */
    for (i=0; i < num_items-1; i++)
	DtComboBoxDeletePos(combobox, 2);

    return 0;
}

int
ui_combobox_add_item(
    Widget	combobox,
    STRING	item,
    int		pos,
    BOOL	unique
)
{
    XmString	xmitem;

    xmitem = XmStringCreateLocalized(item);
    DtComboBoxAddItem(combobox, xmitem, pos, unique);
    XmStringFree(xmitem);

    return 0;
}

int
ui_combobox_select_item(
    Widget	combobox,
    STRING	item
)
{
    XmString    xmitem; 
 
    xmitem = XmStringCreateLocalized(item); 
    /* Note: no return value to tell if Item exists!! */
    DtComboBoxSelectItem(combobox, xmitem); 
    DtComboBoxSetItem(combobox, xmitem); 
    XmStringFree(xmitem); 
   
    return 0;
}

int
ui_list_add_item(
    Widget	list,
    STRING	item_str,
    int		pos
)
{
    XmString	xmitem;

    xmitem = XmStringCreateLocalized(item_str);
    XmListAddItem(list, xmitem, pos);
    XmStringFree(xmitem);
    return 0;
}


/*
 * returns ERR_NOT_FOUND if item doesn't exist
 */
int
ui_list_delete_item(
    Widget	list,
    STRING	item_str
)
{
    int		return_value = 0;
    int		rc = 0;		/* return code */
    int		item_pos = 0;

    rc = ui_list_find_item(list, item_str, &item_pos);
    if (rc < 0)
    {
	return_value = rc;
    }
    else
    {
        XmListDeletePos(list, item_pos);
    }

    return return_value;
}


/*
 * Returns ERR_NOT_FOUND if the item is not found
 */
int
ui_list_replace_item(
    Widget	list,
    STRING	old_item_str,
    STRING	new_item_str
)
{
    int		rc = 0;		/* return code */
    int		item_pos = 0;
    XmString	new_xmitem_array[1];
    int		num_items = 1;
    BOOL	selected = False;

    rc = ui_list_find_item(list, old_item_str, &item_pos);
    if (rc < 0)
    {
	return rc;
    }

    /* Mark if item being changed is currently selected */
    if (XmListPosSelected(list, item_pos))
	selected = True;

    new_xmitem_array[0] = XmStringCreateLocalized(new_item_str);
    XmListReplaceItemsPos(list, new_xmitem_array, num_items, item_pos);
    XmStringFree(new_xmitem_array[0]);

    if (selected) /* Reselect item */
	XmListSelectPos(list, item_pos, False);

    return 0;
}


/*
 */
int
ui_list_replace_item_prefix(
    Widget	list,
    STRING	old_item_prefix,
    STRING	new_item_prefix
)
{
    STRING	item_str = NULL;
    int		num_items = 0;
    XmString	*items = NULL;
    int		old_prefix_len = 0;
    char	new_item_buf[2048] = "";
    char	*new_item_buf_ptr = NULL;
    int		new_prefix_len = 0;
    XmString	new_xmitem_array[1] = {NULL};
    int		i = 0;

    if (old_item_prefix == NULL)
    {
	return ERR_BAD_PARAM;
    }
    if (new_item_prefix == NULL)
    {
	return ERR_BAD_PARAM;
    }
    old_prefix_len = strlen(old_item_prefix);
    new_prefix_len = strlen(new_item_prefix);

    XtVaGetValues(list,
	XmNitemCount, &num_items,
	XmNitems, &items,
	NULL);

    strcpy(new_item_buf, new_item_prefix);
    new_item_buf_ptr = new_item_buf + strlen(new_item_buf);

    for (i = 0; i < num_items; ++i)
    {
	item_str = objxm_xmstr_to_str(items[i]);
	if (item_str == NULL)
	{
	    continue;
	}
	if (strncmp(item_str, old_item_prefix, old_prefix_len) == 0)
	{
	    /* found a match! */
	    strcpy(new_item_buf_ptr, item_str + old_prefix_len);
            new_xmitem_array[0] = XmStringCreateLocalized(new_item_buf);
            XmListReplaceItemsPos(list, new_xmitem_array, 1, i+1);
            XmStringFree(new_xmitem_array[0]);
	    new_xmitem_array[0] = NULL;
	}
    }

    return OK;
}


/*
 * Returns ERR_NOT_FOUND and sets *pos_out to 0 if item does not exist
 */
int
ui_list_find_item(
    Widget	list,
    STRING	item_str,
    int		*pos_out
)
{
    int		return_value = 0;
    XmString	xmitem;
    int		item_pos;

    xmitem = XmStringCreateLocalized(item_str);
    item_pos = XmListItemPos(list, xmitem);
    XmStringFree(xmitem);

    if (item_pos <= 0)
    {
	return_value = ERR_NOT_FOUND;
    }

    *pos_out = item_pos;
    return return_value;
}


int
ui_list_get_info(
	Widget	list,
	int	*num_items_out,
	int	*first_visible_out,
	int	*last_visible_out
)
{
    int		num_items = 0;
    int		top_pos = 0;
    int		items_visible = 0;

    XtVaGetValues(list, 
	XmNitemCount, &num_items,
	XmNtopItemPosition, &top_pos,
	XmNvisibleItemCount, &items_visible,
	NULL);

    *num_items_out = num_items;
    *first_visible_out = top_pos;
    *last_visible_out = top_pos + items_visible - 1;
    return 0;
}

int
ui_list_select_item(
    Widget	list,
    STRING	item_str,
    BOOL	invoke_callbacks
)
{
    int		item_pos = 0;
    int		iRet = 0;

    iRet = ui_list_find_item(list, item_str, &item_pos);
    if (iRet != ERR_NOT_FOUND)
	iRet = ui_list_select_pos(list, item_pos, invoke_callbacks);	
    return iRet;
}

int
ui_list_select_pos(
    Widget	list,
    int		item_pos,
    BOOL	invoke_callbacks
)
{
    int         num_items = 0;
    int         first_vis = 0;
    int         last_vis = 0;
    int         items_vis = 0;

    ui_list_get_info(list, &num_items, &first_vis, &last_vis);
    if (item_pos > num_items)
	return -1;

    items_vis = 0;
    if (num_items > 0)
    {
        items_vis = last_vis - first_vis + 1;
    }

    /* Ensure Item being selected is Visible
     */
   if (item_pos > last_vis)
   {
        /* Item is below the last item showing -
         * so make the Item the last one showing
	 */
	    XtVaSetValues(list,
		XmNtopItemPosition, (item_pos - items_vis) + 1,
		NULL);

    }
    else if (item_pos < first_vis)
    {
	    /* Item is above the first item showing -
	     * so make the Item the first one showing
	     */
   	    XtVaSetValues(list,
		XmNtopItemPosition, item_pos,
		NULL);
    }
    /* Else Item already visible */

    XmListSelectPos(list, item_pos, invoke_callbacks);
    return 0;
}
/*
 * Return the position of the *First* item selected in a List
 */
int
ui_list_get_selected_pos(
   Widget	list
)
{
    int	sel_item_count;
    int *sel_item_num;
    int select_pos = -1;

    XmListGetSelectedPos(list, &sel_item_num, &sel_item_count);

    if (sel_item_count > 0)
    {
	select_pos = sel_item_num[0];
	util_free(sel_item_num);
    }
    return(select_pos);
}
