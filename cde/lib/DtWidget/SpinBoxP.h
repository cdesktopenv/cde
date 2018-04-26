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
/* $XConsortium: SpinBoxP.h /main/4 1996/03/26 19:54:14 drk $ */
/*
 * SpinBoxP.h, Interleaf, 16aug93 2:37pm Version 1.1.
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
 * 9 Hillside Avenue, Waltham, MA  02154
 *
 * SpinBoxP.h:
 *
 * Private header file for DtSpinBoxWidget.
 */
#ifndef _SpinBoxP_h
#define _SpinBoxP_h

#include <Xm/Label.h>
#include <Xm/TextFP.h>
#include <Xm/ArrowB.h>
#include <Xm/ManagerP.h>
#include "SpinBox.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * External definitions of syn_resources for our list widget.
 */
#define SYN_RESOURCE_AA (Widget w, int resource_offset, XtArgVal *value)
extern void _DtSpinBoxGetArrowSize	SYN_RESOURCE_AA;

#define ARROW_MULT	    .45
#define ARROW_MIN	    13
#define MAX_FLOAT_DECIMALS  6
#define NUMERIC_LENGTH      128
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

#define SB_ARROW_SENSITIVE	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_ARROW_SENSITIVE, _DtMsgSpinBox_0000)

#define SB_ALIGNMENT	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_ALIGNMENT, _DtMsgSpinBox_0001)

#define SB_INIT_DELAY	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_INIT_DELAY, _DtMsgSpinBox_0002)

#define SB_MARGIN_HEIGHT	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_MARGIN_HEIGHT, _DtMsgSpinBox_0003)

#define SB_MARGIN_WIDTH	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_MARGIN_WIDTH, _DtMsgSpinBox_0004)

#define SB_ARROW_LAYOUT	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_ARROW_LAYOUT, _DtMsgSpinBox_0005)

#define SB_REPEAT_DELAY	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_REPEAT_DELAY, _DtMsgSpinBox_0006)

#define SB_ITEM_COUNT	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_ITEM_COUNT, _DtMsgSpinBox_0007)

#define SB_POSITION_STRING	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_POSITION_STRING, _DtMsgSpinBox_0008)

#define SB_POSITION_NUMERIC	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_POSITION_NUMERIC, _DtMsgSpinBox_0009)

#define SB_DECIMAL_POINTS	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_DECIMAL_POINTS, _DtMsgSpinBox_0010)

#define SB_MIN_MAX	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_MIN_MAX, _DtMsgSpinBox_0011)

#define SB_TEXT	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_TEXT, _DtMsgSpinBox_0012)

#define SB_SET_ITEM	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_SET_ITEM, _DtMsgSpinBox_0013)

#define SB_LABEL	DTWIDGET_GETMESSAGE( \
                          MS_SpinBox, SPIN_LABEL, _DtMsgSpinBox_0014)
/* 
 * Class Records
 */
typedef struct {
    Boolean junk;
} DtSpinBoxClassPart;

typedef struct _DtSpinBoxClassRec {
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    DtSpinBoxClassPart		spin_box_class;
} DtSpinBoxClassRec;

extern DtSpinBoxClassRec dtSpinBoxClassRec;


/*
 * Instance Record.
 */
typedef struct _DtSpinBoxPart {
    /* Private data */
    Widget label;
    Widget up_arrow;
    Widget down_arrow;
    unsigned char which_arrow;
    Boolean init_cb;
    Boolean grabbed;
    int base;
    float min, max;
    float increment, current;
    char float_format[10];
    Dimension old_width;
    Dimension old_height;
    Dimension label_max_length;
    Dimension label_max_height;

    /* Resource-related data */
    unsigned char arrow_sensitivity;
    short decimal_points;
    int numeric_increment;
    int maximum;
    int minimum;
    int item_count;
    int position;
    unsigned char child_type;
    XmStringTable items;
    XtCallbackList activate_callback;
    unsigned char alignment;
    unsigned char arrow_layout;
    Dimension arrow_size;
    short text_columns;
    Boolean editable;
    XtCallbackList focus_callback;
    unsigned int initial_delay;
    XtCallbackList losing_focus_callback;
    Dimension margin_height;
    Dimension margin_width;
    int text_max_length;
    XtCallbackList modify_verify_callback;
    Boolean recompute_size;
    unsigned int repeat_delay;
    Widget text;
    XtCallbackList value_changed_callback;
    Boolean wrap;

    /* Spin timer. */
    XtIntervalId timer;

    /* String list related resources */

    /* Numeric related resources */

    /* TextField resources */
} DtSpinBoxPart;


/* Full instance record declaration */
typedef struct _DtSpinBoxRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    XmManagerPart	manager;
    DtSpinBoxPart	spin_box;
} DtSpinBoxRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _SpinBoxP_h */
