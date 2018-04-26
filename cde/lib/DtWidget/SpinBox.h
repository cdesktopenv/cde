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
/* $XConsortium: SpinBox.h /main/4 1995/10/26 09:34:23 rswiston $ */
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

#ifndef _Dt_SpinBox_h
#define _Dt_SpinBox_h

#include <Xm/Xm.h>
#if defined(DT_USE_XM_SSPINBOX)
#include <Xm/SSpinB.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* Resources */

#ifndef DtNarrowLayout
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNarrowLayout		XmNarrowLayout
#  else
#  define DtNarrowLayout		"arrowLayout"
#  endif
#endif
#ifndef DtNarrowSensitivity
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNarrowSensitivity		XmNarrowSensitivity
#  else
#  define DtNarrowSensitivity		"arrowSensitivity"
#  endif
#endif
#ifndef DtNarrowSize
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNarrowSize			XmNarrowSize
#  else
#  define DtNarrowSize			"arrowSize"
#  endif
#endif
#ifndef DtNspinBoxChildType
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNspinBoxChildType		XmNspinBoxChildType
#  else
#  define DtNspinBoxChildType		"spinBoxChildType"
#  endif
#endif
#ifndef DtNposition
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNposition			XmNposition
#  else
#  define DtNposition			"position"
#  endif
#endif
#ifndef DtNtextField
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNtextField			XmNtextField
#  else
#  define DtNtextField			"textField"
#  endif
#endif
#ifndef DtNwrap
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNwrap			"wrap"
#  else
#  define DtNwrap			"wrap"
#  endif
#endif
#ifndef DtNincrementValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNincrementValue		XmNincrementValue
#  else
#  define DtNincrementValue		"incrementValue"
#  endif
#endif
#ifndef DtNmaximumValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNmaximumValue		XmNmaximumValue
#  else
#  define DtNmaximumValue		"maximumValue"
#  endif
#endif
#ifndef DtNminimumValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNminimumValue		XmNminimumValue
#  else
#  define DtNminimumValue		"minimumValue"
#  endif
#endif
#ifndef DtNnumValues
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNnumValues			XmNnumValues
#  else
#  define DtNnumValues			"numValues"
#  endif
#endif
#ifndef DtNvalues
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNvalues			XmNvalues
#  else
#  define DtNvalues			"values"
#  endif
#endif

#ifndef DtNactivateCallback
#define DtNactivateCallback	XmNactivateCallback
#endif
#ifndef DtNalignment
#define DtNalignment		XmNalignment
#endif
#ifndef DtNcolumns
#define DtNcolumns		XmNcolumns
#endif
#ifndef DtNdecimalPoints
#define DtNdecimalPoints	XmNdecimalPoints
#endif
#ifndef DtNeditable
#define DtNeditable		XmNeditable
#endif
#ifndef DtNfocusCallback
#define DtNfocusCallback	XmNfocusCallback
#endif
#ifndef DtNinitialDelay
#define DtNinitialDelay		XmNinitialDelay
#endif
#ifndef DtNlosingFocusCallback
#define DtNlosingFocusCallback	XmNlosingFocusCallback
#endif
#ifndef DtNmarginHeight
#define DtNmarginHeight		XmNmarginHeight
#endif
#ifndef DtNmarginWidth
#define DtNmarginWidth		XmNmarginWidth
#endif
#ifndef DtNmaxLength
#define DtNmaxLength		XmNmaxLength
#endif
#ifndef DtNmodifyVerifyCallback
#define DtNmodifyVerifyCallback	XmNmodifyVerifyCallback
#endif
#ifndef DtNrecomputeSize
#define DtNrecomputeSize	XmNrecomputeSize
#endif
#ifndef DtNrepeatDelay
#define DtNrepeatDelay		XmNrepeatDelay
#endif
#ifndef DtNvalueChangedCallback
#define DtNvalueChangedCallback	XmNvalueChangedCallback
#endif

#ifndef DtCArrowLayout
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCArrowLayout		XmCArrowLayout
#  else
#  define DtCArrowLayout		"ArrowLayout"
#  endif
#endif
#ifndef DtCArrowSensitivity
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCArrowSensitivity		XmCArrowSensitivity
#  else
#  define DtCArrowSensitivity		"ArrowSensitivity"
#  endif
#endif
#ifndef DtCArrowSize
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCArrowSize			XmCArrowSize
#  else
#  define DtCArrowSize			"ArrowSize"
#  endif
#endif
#ifndef DtCSpinBoxChildType
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCSpinBoxChildType		XmCSpinBoxChildType
#  else
#  define DtCSpinBoxChildType		"SpinBoxChildType"
#  endif
#endif
#ifndef DtCPosition
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCPosition			XmCPosition
#  else
#  define DtCPosition			"Position"
#  endif
#endif
#ifndef DtCTextField
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCTextField			XmCTextField
#  else
#  define DtCTextField			"TextField"
#  endif
#endif
#ifndef DtCWrap
#  define DtCWrap			"Wrap"
#endif
#ifndef DtCIncrementValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCIncrementValue		XmCIncrementValue
#  else
#  define DtCIncrementValue		"incrementValue"
#  endif
#endif
#ifndef DtCMaximumValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCMaximumValue		XmCMaximumValue
#  else
#  define DtCMaximumValue		"maximumValue"
#  endif
#endif
#ifndef DtCMinimumValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCMinimumValue		XmCMinimumValue
#  else
#  define DtCMinimumValue		"minimumValue"
#  endif
#endif
#ifndef DtCNumValues
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCNumValues			XmCNumValues
#  else
#  define DtCNumValues			"numValues"
#  endif
#endif
#ifndef DtCValues
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCValues			XmCValues
#  else
#  define DtCValues			"values"
#  endif
#endif

#ifndef DtCAlignment
#define DtCAlignment		XmCAlignment
#endif
#ifndef DtCCallback
#define DtCCallback		XmCCallback
#endif
#ifndef DtCColumns
#define DtCColumns		XmCColumns
#endif
#ifndef DtCDecimalPoints
#define DtCDecimalPoints	XmCDecimalPoints
#endif
#ifndef DtCEditable
#define DtCEditable		XmCEditable
#endif
#ifndef DtCInitialDelay
#define DtCInitialDelay		XmCInitialDelay
#endif
#ifndef DtCItems
#define DtCItems		XmCItems
#endif
#ifndef DtCMarginHeight
#define DtCMarginHeight		XmCMarginHeight
#endif
#ifndef DtCMarginWidth
#define DtCMarginWidth		XmCMarginWidth
#endif
#ifndef DtCMaxLength
#define DtCMaxLength		XmCMaxLength
#endif
#ifndef DtCPosition
#define DtCPosition		XmCPosition
#endif
#ifndef DtCRecomputeSize
#define DtCRecomputeSize	XmCRecomputeSize
#endif
#ifndef DtCRepeatDelay
#define DtCRepeatDelay		XmCRepeatDelay
#endif

/* Representation types */

#ifndef DtRIncrementValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRIncrementValue		XmRint
#  else
#  define DtRIncrementValue		"IncrementValue"
#  endif
#endif
#ifndef DtRMaximumValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRMaximumValue		XmRint
#  else
#  define DtRMaximumValue		"MaximumValue"
#  endif
#endif
#ifndef DtRMinimumValue
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRMinimumValue		XmRint
#  else
#  define DtRMinimumValue		"MinimumValue"
#  endif
#endif
#ifndef DtRNumValues
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRNumValues			XmRint
#  else
#  define DtRNumValues			"NumValues"
#  endif
#endif
#ifndef DtRValues
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRValues			XmRXmStringTable
#  else
#  define DtRValues			"Values"
#  endif
#endif
#ifndef DtRArrowSensitivity
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRArrowSensitivity		XmRArrowSensitivity
#  else
#  define DtRArrowSensitivity		"ArrowSensitivity"
#  endif
#endif
#ifndef DtRArrowLayout
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRArrowLayout		XmRArrowLayout
#  else
#  define DtRArrowLayout		"ArrowLayout"
#  endif
#endif
#ifndef DtRSpinBoxChildType
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtRSpinBoxChildType		XmRSpinBoxChildType
#  else
#  define DtRSpinBoxChildType		"SpinBoxChildType"
#  endif
#endif

/* DtNarrowLayout values */

#ifndef DtARROWS_FLAT_BEGINNING
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_FLAT_BEGINNING	XmARROWS_FLAT_BEGINNING
#  else
#  define DtARROWS_FLAT_BEGINNING	4
#  endif
#endif
#ifndef DtARROWS_FLAT_END
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_FLAT_END		XmARROWS_FLAT_END
#  else
#  define DtARROWS_FLAT_END		3
#  endif
#endif
#ifndef DtARROWS_SPLIT
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_SPLIT		XmARROWS_SPLIT
#  else
#  define DtARROWS_SPLIT		2
#  endif
#endif
#ifndef DtARROWS_BEGINNING
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_BEGINNING		XmARROWS_BEGINNING
#  else
#  define DtARROWS_BEGINNING		1
#  endif
#endif
#ifndef DtARROWS_END
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_END			XmARROWS_END
#  else
#  define DtARROWS_END			0
#  endif
#endif

/* DtNarrowSensitivity values */

#ifndef DtARROWS_SENSITIVE
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_SENSITIVE		XmARROWS_SENSITIVE
#  else
#  define DtARROWS_SENSITIVE		3
#  endif
#endif
#ifndef DtARROWS_DECREMENT_SENSITIVE
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_DECREMENT_SENSITIVE	XmARROWS_DECREMENT_SENSITIVE
#  else
#  define DtARROWS_DECREMENT_SENSITIVE	2
#  endif
#endif
#ifndef DtARROWS_INCREMENT_SENSITIVE
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_INCREMENT_SENSITIVE	XmARROWS_INCREMENT_SENSITIVE
#  else
#  define DtARROWS_INCREMENT_SENSITIVE	1
#  endif
#endif
#ifndef DtARROWS_INSENSITIVE
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtARROWS_INSENSITIVE		XmARROWS_INSENSITIVE
#  else
#  define DtARROWS_INSENSITIVE		0
#  endif
#endif

/* DtNspinBoxChildType values */

#ifndef DtNUMERIC
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtNUMERIC			XmNUMERIC
#  else
#  define DtNUMERIC			3
#  endif
#endif

#ifndef DtSTRING
#  define DtSTRING			XmSTRING
#endif

/* DtNalignment values */

#ifndef DtALIGNMENT_BEGINNING
#define DtALIGNMENT_BEGINNING		XmALIGNMENT_BEGINNING
#endif
#ifndef DtALIGNMENT_CENTER
#define DtALIGNMENT_CENTER		XmALIGNMENT_CENTER
#endif
#ifndef DtALIGNMENT_END
#define DtALIGNMENT_END			XmALIGNMENT_END
#endif

/* Callback reasons */

#ifndef DtCR_OK
#  define DtCR_OK			XmCR_OK
#endif
#ifndef DtCR_SPIN_NEXT
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCR_SPIN_NEXT		XmCR_SPIN_NEXT
#  else
#  define DtCR_SPIN_NEXT		62
#  endif
#endif
#ifndef DtCR_SPIN_PRIOR
#  if defined(DT_USE_XM_SSPINBOX)
#  define DtCR_SPIN_PRIOR		XmCR_SPIN_PRIOR
#  else
#  define DtCR_SPIN_PRIOR		63
#  endif
#endif


/*
 * Types
 */
#if defined(DT_USE_XM_SSPINBOX)
#  define DtSpinBoxCallbackStruct	XmSpinBoxCallbackStruct
#else
typedef struct {
	int		reason;
	XEvent		*event;
	Widget		widget;
	Boolean		doit;
	int		position;
	XmString	value;
	Boolean		crossed_boundary;
} DtSpinBoxCallbackStruct;
#endif

/* Widget class and instance */
#if defined(DT_USE_XM_SSPINBOX)
#  define _DtSpinBoxClassRec		_XmSimpleSpinBoxClassRec
#  define _DtSpinBoxRec			_XmSimpleSpinBoxRec
#  define DtSpinBoxWidgetClass		XmSimpleSpinBoxWidgetClass
#  define DtSpinBoxWidget		XmSimpleSpinBoxWidget
#else
typedef struct _DtSpinBoxClassRec *DtSpinBoxWidgetClass;
typedef struct _DtSpinBoxRec      *DtSpinBoxWidget;
#endif

/*
 * Data
 */

/* Widget class record */
#if defined(DT_USE_XM_SSPINBOX)
#  define dtSpinBoxWidgetClass		xmSimpleSpinBoxWidgetClass
#else
externalref WidgetClass dtSpinBoxWidgetClass;
#endif


/*
 * Functions
 */

#if defined(DT_USE_XM_SSPINBOX)
#  define DtCreateSpinBox		XmCreateSimpleSpinBox
#  define DtSpinBoxAddItem		XmSimpleSpinBoxAddItem
#  define DtSpinBoxDeletePos		XmSimpleSpinBoxDeletePos
#  define DtSpinBoxSetItem		XmSimpleSpinBoxSetItem
#else
extern Widget DtCreateSpinBox(
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

extern void DtSpinBoxAddItem(
		Widget		widget,
		XmString	item,
		int		pos);

extern void DtSpinBoxDeletePos(
		Widget		widget,
		int		pos);

extern void DtSpinBoxSetItem(
		Widget		widget,
		XmString	item);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* _Dt_SpinBox_h */
