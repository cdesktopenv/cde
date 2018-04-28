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
 * $XConsortium: WXmList.h /main/4 1996/07/26 11:32:22 rcs $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

// This code is automatically generated in -*- C++ -*-
#ifndef WXmList_h
#define WXmList_h

#include "WXmPrimitive.h"
#include <Xm/List.h>

class WXmList : public WXmPrimitive {
public :
   DEFINE_INIT (WXmList, WXmPrimitive, xmListWidgetClass)
   inline void		AddItem(WXmString s,int p) const {
      XmListAddItem (widget, s, p);
   }
   inline void		AddItemUnselected(WXmString s, int p) const {
      XmListAddItemUnselected (widget, s, p);
   }
   inline void		AddItems(XmString *s, int c, int p) const {
      XmListAddItems (widget, s, c, p);
   }
#if XmVersion >= 1002
   inline void		AddItemsUnselected(XmString *s, int c, int p) const {
      XmListAddItemsUnselected (widget, s, c, p);
   }
#endif
   inline void		DeleteAllItems(void) const {
      XmListDeleteAllItems (widget);
   }
   inline void		DeleteItem(WXmString s) const {
      XmListDeleteItem (widget, s);
   }
   inline void		DeleteItems(XmString *s, int c) const {
      XmListDeleteItems (widget, s, c);
   }
   inline void		DeleteItemsPos(int c, int p) const {
      XmListDeleteItemsPos (widget, c, p);
   }
   inline void		DeletePos(int p) const {
      XmListDeletePos (widget, p);
   }
#if XmVersion >= 1002
   inline void		DeletePositions(int *p, int c) const {
      XmListDeletePositions (widget, p, c);
   }
#endif
   inline void		DeselectAllItems() const {
      XmListDeselectAllItems (widget);
   }
   inline void		DeselectItem(WXmString s) const {
      XmListDeselectItem (widget, s);
   }
   inline void		DeselectPos(int i) const {
      XmListDeselectPos (widget, i);
   }
#if XmVersion >= 1002
   inline int		GetKbdItemPos() const {
      return XmListGetKbdItemPos (widget);
   }
#endif
   inline Boolean	GetMatchPos(WXmString s, int **p, int *c) const {
      return XmListGetMatchPos (widget, s, p, c);
   }
   inline Boolean	GetSelectedPos(int **p, int *c) const {
      return XmListGetSelectedPos (widget, p, c);
   }
   inline Boolean	ItemExists(WXmString s) const {
      return XmListItemExists (widget, s);
   }
   inline int		ItemPos(WXmString s) const {
      return XmListItemPos (widget, s);
   }
#if XmVersion >= 1002
   inline Boolean	PosSelected(int p) const {
      return XmListPosSelected (widget, p);
   }
   inline Boolean	PosToBounds(int p, Position  *x, Position  *y,
					   Dimension *w, Dimension *h) const {
      return XmListPosToBounds (widget, p, x, y, w, h);
   }
#endif
   inline void		ReplaceItems(XmString *os, int c, XmString *ns) const {
      XmListReplaceItems (widget, os, c, ns);
   }
   inline void		ReplaceItemsPos(XmString *os, int c, int p) const {
      XmListReplaceItemsPos (widget, os, c, p);
   }
   inline void		ReplaceItemsPosUnselected(XmString *os, int c,
						  int p) const {
      XmListReplaceItemsPos (widget, os, c, p);
   }
#if XmVersion >= 1002
   inline void		ReplaceItemsUnselected(XmString *os, int c,
					       XmString *ns) const {
      XmListReplaceItemsUnselected (widget, os, c, ns);
   }
   inline void		ReplacePositions(int *p, XmString *s, int c) const {
      XmListReplacePositions (widget, p, s, c);
   }
#endif
   inline void		SelectItem(WXmString s,Boolean b) const {
      XmListSelectItem (widget, s, b);
   }
   inline void		SelectPos(int p, Boolean b) const {
      XmListSelectPos (widget, p, b);
   }
   inline void		SetAddMode(Boolean b) const {
      XmListSetAddMode (widget, b);
   }
   inline void		SetBottomItem(WXmString s) const {
      XmListSetBottomItem (widget, s);
   }
   inline void		SetBottomPos(int p) const {
      XmListSetBottomPos (widget, p);
   }
   inline void		SetHorizPos(int p) const {
      XmListSetHorizPos (widget, p);
   }
   inline void		SetItem(WXmString s) const {
      XmListSetItem (widget, s);
   }
#if XmVersion >= 1002
inline Boolean	SetKbdItemPos(int p) const {
      return XmListSetKbdItemPos (widget, p);
   }
#endif
   inline void		SetPos(int p) const {
      XmListSetPos (widget, p);
   }
#if XmVersion >= 1002
   inline void		UpdateSelectedList() const {
      XmListUpdateSelectedList (widget);
   }
   inline int		YToPos(Position y) const {
      return XmListYToPos (widget, y);
   }
#endif
   DEFINE_GETTER_SETTER (AutomaticSelection,Boolean,"automaticSelection")
   DEFINE_CALLBACK (BrowseSelectionCallback,"browseSelectionCallback")
   DEFINE_CALLBACK (ConvertCallback,"convertCallback")
   DEFINE_CALLBACK (DefaultActionCallback,"defaultActionCallback")
   DEFINE_GETTER_SETTER (DoubleClickInterval,int,"doubleClickInterval")
   DEFINE_CALLBACK (ExtendedSelectionCallback,"extendedSelectionCallback")
   DEFINE_GETTER_SETTER (FontList,XmFontList,"fontList")
   DEFINE_GETTER_SETTER (ItemCount,int,"itemCount")
   DEFINE_GETTER_SETTER (Items,XmStringTable,"items")
   DEFINE_GETTER_SETTER (ListMarginHeight,short,"listMarginHeight")
   DEFINE_GETTER_SETTER (ListMarginWidth,short,"listMarginWidth")
   DEFINE_GETTER_SETTER (ListSizePolicy,unsigned char,"listSizePolicy")
   DEFINE_GETTER_SETTER (ListSpacing,short,"listSpacing")
   DEFINE_CALLBACK (MultipleSelectionCallback,"multipleSelectionCallback")
   DEFINE_GETTER_SETTER (ScrollBarDisplayPolicy,unsigned char,"scrollBarDisplayPolicy")
   DEFINE_GETTER_SETTER (SelectedItemCount,int,"selectedItemCount")
   DEFINE_GETTER_SETTER (SelectedItems,XmStringTable,"selectedItems")
   DEFINE_GETTER_SETTER (SelectionPolicy,unsigned char,"selectionPolicy")
   DEFINE_CALLBACK (SingleSelectionCallback,"singleSelectionCallback")
   DEFINE_GETTER_SETTER (StringDirection,unsigned char,"stringDirection")
   DEFINE_GETTER_SETTER (TopItemPosition,int,"topItemPosition")
   DEFINE_GETTER_SETTER (VisibleItemCount,int,"visibleItemCount")
   DEFINE_GETTER_SETTER (HorizontalScrollBar,Widget,"horizontalScrollBar")
   DEFINE_GETTER_SETTER (VerticalScrollBar,Widget,"verticalScrollBar")

   // Change the font of an item in the list.  Zero indicates last item.
   Boolean ChangeItemFont( int pos, char* tag );
};

#define	NULLWXmList		WXmList((Widget)0)

#endif

