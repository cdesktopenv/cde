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
/* $XConsortium: ComboBoxP.h /main/3 1995/10/26 09:29:58 rswiston $ */
/*
 * DtWidget/ComboBoxP.h
 */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */
/***********************************************************
Copyright 1993 Interleaf, Inc.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose without fee is granted,
provided that the above copyright notice appear in all copies
and that both copyright notice and this permission notice appear
in supporting documentation, and that the name of Interleaf not
be used in advertising or publicly pertaining to distribution of
the software without specific written prior permission.

Interleaf makes no representation about the suitability of this
software for any purpose. It is provided "AS IS" without any
express or implied warranty. 
******************************************************************/

/*
 * (C) Copyright 1991,1992, 1993
 * Interleaf, Inc.
 * Nine Hillside Avenue, Waltham, MA  02154
 *
 * ComboBoxP.h:
 * 
 * Private header file for DtComboBoxWidget.
 */
#ifndef _ComboBoxP_h
#define _ComboBoxP_h

#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/DrawnB.h>
#include <Xm/ArrowB.h>
#include <Xm/TextFP.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/ListP.h>
#include <Xm/Separator.h>
#include <Xm/ScrolledWP.h>
#include <Xm/ScrollBarP.h>

#include <Xm/ManagerP.h>
#include "ComboBox.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * External definitions of syn_resources for our list widget.
 */
#define SYN_RESOURCE_AA (Widget w, int resource_offset, XtArgVal *value)
extern void _DtComboBoxGetArrowSize		SYN_RESOURCE_AA;
extern void _DtComboBoxGetLabelString		SYN_RESOURCE_AA;
extern void _DtComboBoxGetListItemCount		SYN_RESOURCE_AA;
extern void _DtComboBoxGetListItems		SYN_RESOURCE_AA;
extern void _DtComboBoxGetListFontList		SYN_RESOURCE_AA;
extern void _DtComboBoxGetListMarginHeight	SYN_RESOURCE_AA;
extern void _DtComboBoxGetListMarginWidth	SYN_RESOURCE_AA;
extern void _DtComboBoxGetListSpacing		SYN_RESOURCE_AA;
extern void _DtComboBoxGetListTopItemPosition	SYN_RESOURCE_AA;
extern void _DtComboBoxGetListVisibleItemCount	SYN_RESOURCE_AA;

#define ARROW_MULT	    .45
#define ARROW_MIN	    13
#define MARGIN		    2
#define LABEL_PADDING	    2
#define LABEL_SHADOW	    2
#define TEXT_FIELD_SHADOW   1
#define TEXT_CONTEXT_MARGIN 4

/****************************************************************
 *
 *	Message	Defines
 *
 ****************************************************************/

#define CB_ALIGNMENT	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_ALIGNMENT, _DtMsgComboBox_0000)

#define CB_MARGIN_HEIGHT	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_MARGIN_HEIGHT, _DtMsgComboBox_0001)

#define CB_MARGIN_WIDTH	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_MARGIN_WIDTH, _DtMsgComboBox_0002)

#define CB_HORIZONTAL_SPACING	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_HORIZONTAL_SPACING, _DtMsgComboBox_0003)

#define CB_VERTICAL_SPACING	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_VERTICAL_SPACING, _DtMsgComboBox_0004)

#define CB_ORIENTATION	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_ORIENTATION, _DtMsgComboBox_0005)

#define CB_ITEM_COUNT	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_ITEM_COUNT, _DtMsgComboBox_0006)

#define CB_VISIBLE_ITEM	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_VISIBLE_ITEM, _DtMsgComboBox_0007)

#define CB_TEXT		DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_TEXT, _DtMsgComboBox_0008)

#define CB_SET_ITEM		DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_SET_ITEM, _DtMsgComboBox_0009)

#define CB_SELECT_ITEM		DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_SELECT_ITEM, _DtMsgComboBox_0010)

#define CB_RESIZE	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_RESIZE, _DtMsgComboBox_0011)

#define CB_LABEL	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_LABEL, _DtMsgComboBox_0012)

#define CB_CVTSTRING	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_CVTSTRING, _DtMsgComboBox_0013)

#define CB_DEL_POS	DTWIDGET_GETMESSAGE( \
                          MS_ComboBox, COMBO_DEL_POS, _DtMsgComboBox_0014)
/* 
 * Class Records
 */
typedef struct {
    Boolean junk;  /* Need something */
} DtComboBoxClassPart;

typedef struct _DtComboBoxClassRec {
    CoreClassPart	    core_class;
    CompositeClassPart	    composite_class;
    ConstraintClassPart	    constraint_class;
    XmManagerClassPart	    manager_class;
    DtComboBoxClassPart	    combo_box_class;
} DtComboBoxClassRec;

extern DtComboBoxClassRec dtComboBoxClassRec;


/*
 * Instance Record.
 */
typedef struct _DtComboBoxPart {
    /* Private data */
    Widget arrow;
    Widget shell;
    Widget frame;
    Widget label;
    Widget sep;
    Dimension old_width;
    Dimension old_height;
    Dimension label_max_length;
    Dimension label_max_height;
    /* 
     * max_shell_width is the width that is needed to hold the
     * list if the longest item was visible.  We then use this
     * width to figure out if the shell is not wide enough,
     * when it gets popped on the screen.  This is needed in case the
     * combo-box resizes, or if items changes (list will resize).
     * Sometimes we change the size of the shell to fit on the screen, or
     * to make it at least as large as the combo_box.  The next time we pop
     * the shell up the size may be different; therefore, we set the shell
     * size to the maximum everytime it gets popped up, then we will
     * make adjustments, only if needed.
     * This value gets saved every time the user updates DtNitems.
     */
    Dimension max_shell_width;
    Dimension max_shell_height;

    /* ComboBox specific public resources */
    Dimension margin_height;
    Dimension margin_width;
    XmString selected_item;
    int selected_position;
    XtCallbackList selection_callback;
    unsigned char type;
    Dimension arrow_spacing;

    /* ComboBox specific private resources */
    Dimension arrow_size;
    XtCallbackList activate_callback;
    unsigned char alignment;
    unsigned char arrow_type;
    short text_columns;
    XtCallbackList focus_callback;
    Dimension horizontal_spacing;
    int item_count;
    XmStringTable items;
    XmStringTable list_items;
    XmString label_string;
    Widget list;
    XmFontList list_font_list;
    Dimension list_margin_height;
    Dimension list_margin_width;
    Dimension list_spacing;
    XtCallbackList losing_focus_callback;
    unsigned int text_max_length;
    XtCallbackList menu_post_callback;
    unsigned char orientation;
    Boolean popped_up;
    Boolean recompute_size;
    Widget text;
    int top_item_position;
    Boolean update_label;
    Dimension vertical_spacing;
    int visible_item_count;
} DtComboBoxPart;


typedef struct _DtComboBoxRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    XmManagerPart	manager;
    DtComboBoxPart	combo_box;
} DtComboBoxRec;



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmComboBoxP_h */
