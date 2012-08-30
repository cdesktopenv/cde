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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * DtWidget/ComboBox.c
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = 
  "$XConsortium: ComboBox.c /main/15 1996/10/29 12:48:08 cde-hp $"
#endif
#endif
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
 * 9 Hillside Avenue, 
 * Waltham, MA  02154
 *
 * ComboBox.c (DtComboBoxWidget):
 *
 * I wanted a margin around the widget (outside the shadow, like buttons), 
 * so that the combo-box could be made the same size as a 
 * push-button, etc.  The bulletin-board widget always puts the shadow at 
 * the outside edge of the widget, so combo-box is a sublcass of
 * manager, and we do everything ourselves.
 * 
 * One must be carefull when using Dimension (for core width and height).
 * Dimension is an unsigned short.  This causes problems when subtracting
 * and ending up with what should be a negative number (but it doesn't).
 * All child widget positioning is done by the combo_box.  We don't
 * use any heavy-weight forms, etc. to help us out.
 *
 * There is no padding when editable.  If using a label given it a
 * small margin, so it doesn't run up against the side of our
 * shadow or the arrow.
 * 
 * Make some of the ComboBox functions common, so they can be shared
 * with SpinButton.
 *
 * The label-string resource got out of control.  Its role kept getting
 * expanded; now the whole thing is a mess.  Currently it shadows the
 * label's label-string.  If the user sets it explicitly it will 
 * take affect for as long as update-label is false.  If update-label
 * is true, it will take affect until the end-user makes a selection
 * off the list.
 * 
 * Known bugs:
 *	    Changing margin_width or margin_height resources when the
 *	    combo_box has focus will probably result in display glitches.
 *
 */
/*
 * The DtComboBox widget is rigged with the Motif widget binary compatibilit 
 * mechanism. All Motif-specific changes for this mechanism are preceded  
 * by a comment including the string "MotifBc".
 *
 * For a description of the Motif widget binary compatibility mechanism 
 * see the reference manual entry on XmResolveAllPartOffsets().
 *
 */

/* _NO_PROTO support no longer required: */

#include <Xm/XmP.h>		/* for fast subclassing in XmIsComboBox */
#include <Xm/XmosP.h>		/* for INT_MAX */
#include <Dt/DtMsgsP.h>
#include "ComboBoxP.h"
#include <Xm/DrawP.h>
#include <Xm/DisplayP.h>
#include <Xm/List.h>
#include <Xm/ComboBox.h>	/* for redirecting utility functions */
#include "DtWidgetI.h"		/* for _Dt thread-safety macros */
/* some unpublished Motif interfaces */
#include <Xm/XmPrivate.h>

/*
 * MotifBc
 */
#define DtComboBoxIndex (XmManagerIndex + 1)
static XmOffsetPtr ipot; /* Instance part offset table */
static XmOffsetPtr cpot; /* Constraint part offset table */

#define ScrollBarVisible( wid)      (wid && XtIsManaged( wid))

static void	ClassInitialize (void);
static void	Initialize (DtComboBoxWidget request, 
			       DtComboBoxWidget new, ArgList given_args, 
			       Cardinal *num_args);
static XmNavigability WidgetNavigable (DtComboBoxWidget combo);
static void	_ComboBoxFocusIn (DtComboBoxWidget combo, XEvent *event, 
				     char **params, Cardinal *num_params);
static void	_ComboBoxFocusOut (DtComboBoxWidget combo, XEvent *event,
				      char **params, Cardinal *num_params);
static void	DrawHighlight (DtComboBoxWidget combo, Boolean clear);
static void	_ComboBoxActivate (Widget w, XEvent *event, char **params,
				      Cardinal *num_params);
static void	_ComboBoxKbdCancel (Widget w, XEvent *event, char **params,
				       Cardinal *num_params);
static void	_ComboBoxPrevTabGroup (Widget w, XEvent *event, 
				char **params, Cardinal *num_params);
static void	_ComboBoxNextTabGroup (Widget w, XEvent *event, 
				char **params, Cardinal *num_params);
static void	CheckResources (DtComboBoxWidget combo);
static void	Destroy (DtComboBoxWidget combo);
static void	Resize (DtComboBoxWidget combo);
static void	Redisplay (DtComboBoxWidget w, XEvent *event, 
			      Region region);
static XtGeometryResult GeometryManager (Widget w, 
					    XtWidgetGeometry *request, 
					    XtWidgetGeometry *reply);
static void	SetComboBoxSize (DtComboBoxWidget combo);
static void	ForceChildSizes (DtComboBoxWidget combo);
static void	LayoutChildren (DtComboBoxWidget combo);
static Boolean	SetValues (DtComboBoxWidget current, 
			      DtComboBoxWidget request, DtComboBoxWidget new);
static void	ClearShadow (DtComboBoxWidget w, Boolean all);
static void	DrawShadow (DtComboBoxWidget w);
static char*	GetTextString (XmString xm_string);
static void	SetTextFieldData (DtComboBoxPart *combo_p, XmString item);
static void	SetMaximumLabelSize (DtComboBoxPart *combo_p);
static void	SetLabelData (DtComboBoxPart *combo_p, XmString item,
				 Boolean force_label_string);
static void	select_cb (Widget w, XtPointer client_data, 
			      XtPointer call_data);
static void	shell_event_handler (Widget widget, XtPointer client_data,
					XEvent* event, Boolean *dispatch);
static void	list_event_handler (Widget widget, XtPointer client_data,
				       XEvent* event, Boolean *dispatch);
static void	TextFieldActivate (DtComboBoxPart *combo_p, XtPointer call_data);
static void	activate_cb (Widget w, XtPointer client_data,
				XtPointer call_data);
static void	arrow_expose_cb (Widget w, XtPointer client_data,
				    XtPointer call_data);
static void	text_losing_focus_cb (Widget w, XtPointer client_data,
					 XtPointer call_data);
static void	text_activate_cb (Widget w, XtPointer client_data,
				     XtPointer call_data);
static void	text_focus_cb (Widget w, XtPointer client_data,
				  XtPointer call_data);
static void	SyncWithList (DtComboBoxPart *combo_p);
static XmImportOperator _XmSetSyntheticResForChild (Widget widget,
						       int offset, 
						       XtArgVal * value);
/* Converter */
static  Boolean _CvtStringToType (Display *dpy, XrmValuePtr args, 
	Cardinal *num_args, XrmValuePtr from, XrmValuePtr to, XtPointer *data);

/* Grab and Ungrab processing */
static void	input_ungrab ( DtComboBoxWidget combo, int ungrab_mask);

/* Resolution Independent Processing */
void _DtComboBoxGetArrowSize(	Widget w,
					int resource_offset,
					XtArgVal *value);
void _DtComboBoxGetListMarginHeight(	Widget w,
						int resource_offset,
						XtArgVal *value);
void _DtComboBoxGetListMarginWidth(  	Widget w,
						int resource_offset,
						XtArgVal *value);
void _DtComboBoxGetListSpacing(  Widget w,
					int resource_offset,
					XtArgVal *value);


static XmString InitLabel = NULL;

/*
 * MotifBc
 */

#define Arrow(w) XmField(w,ipot,DtComboBox,arrow,Widget)
#define Shell(w) XmField(w,ipot,DtComboBox,shell,Widget)
#define Frame(w) XmField(w,ipot,DtComboBox,frame,Widget)
#define Label(w) XmField(w,ipot,DtComboBox,label,Widget)
#define Sep(w) XmField(w,ipot,DtComboBox,sep,Widget)
#define OldWidth(w) XmField(w,ipot,DtComboBox,old_width,Dimension)
#define OldHeight(w) XmField(w,ipot,DtComboBox,old_height,Dimension)
#define LabelMaxLength(w) XmField(w,ipot,DtComboBox,label_max_length,Dimension)
#define LabelMaxHeight(w) XmField(w,ipot,DtComboBox,label_max_height,Dimension)

#define MaxShellWidth(w) XmField(w,ipot,DtComboBox,max_shell_width,Dimension)
#define MaxShellHeight(w) XmField(w,ipot,DtComboBox,max_shell_height,Dimension)

#define MarginHeight(w) XmField(w,ipot,DtComboBox,margin_height,Dimension)
#define MarginWidth(w) XmField(w,ipot,DtComboBox,margin_width,Dimension)
#define SelectedItem(w) XmField(w,ipot,DtComboBox,selected_item,XmString)
#define SelectedPosition(w) XmField(w,ipot,DtComboBox,selected_position,int)
#define SelectionCallback(w) \
		XmField(w,ipot,DtComboBox,selection_callback,XtCallbackList)
#define Type(w) XmField(w,ipot,DtComboBox,type,unsigned char)
#define ArrowSpacing(w) XmField(w,ipot,DtComboBox,arrow_spacing,Dimension)

#define ArrowSize(w) XmField(w,ipot,DtComboBox,arrow_size,Dimension)
#define ActivateCallback(w) \
		XmField(w,ipot,DtComboBox,activate_callback,XtCallbackList)
#define Alignment(w) XmField(w,ipot,DtComboBox,alignment,unsigned char)
#define ArrowType(w) XmField(w,ipot,DtComboBox,arrow_type,unsigned char)
#define TextColumns(w) XmField(w,ipot,DtComboBox,text_columns,short)
#define FocusCallback(w) XmField(w,ipot,DtComboBox,focus_callback,XtCallbackList)
#define HorizontalSpacing(w) XmField(w,ipot,DtComboBox,horizontal_spacing,Dimension)
#define ItemCount(w) XmField(w,ipot,DtComboBox,item_count,int)
#define Items(w) XmField(w,ipot,DtComboBox,items,XmStringTable)
#define ListItems(w) XmField(w,ipot,DtComboBox,list_items,XmStringTable)
#define LabelString(w) XmField(w,ipot,DtComboBox,label_string,XmString)
#define List(w) XmField(w,ipot,DtComboBox,list,Widget)
#define ListFontList(w) XmField(w,ipot,DtComboBox,list_font_list,XmFontList)
#define ListMarginHeight(w) XmField(w,ipot,DtComboBox,list_margin_height,Dimension)
#define ListMarginWidth(w) XmField(w,ipot,DtComboBox,list_margin_width,Dimension)
#define ListSpacing(w) XmField(w,ipot,DtComboBox,list_spacing,Dimension)
#define LosingFocusCallback(w) XmField(w,ipot,DtComboBox,losing_focus_callback,XtCallbackList)
#define TextMaxLength(w) XmField(w,ipot,DtComboBox,text_max_length,unsigned int)
#define MenuPostCallback(w) XmField(w,ipot,DtComboBox,menu_post_callback,XtCallbackList)
#define Orientation(w) XmField(w,ipot,DtComboBox,orientation,unsigned char)
#define PoppedUp(w) XmField(w,ipot,DtComboBox,popped_up,Boolean)
#define RecomputeSize(w) XmField(w,ipot,DtComboBox,recompute_size,Boolean)
#define Text(w) XmField(w,ipot,DtComboBox,text,Widget)
#define TopItemPosition(w) XmField(w,ipot,DtComboBox,top_item_position,int)
#define UpdateLabel(w) XmField(w,ipot,DtComboBox,update_label,Boolean)
#define VerticalSpacing(w) XmField(w,ipot,DtComboBox,vertical_spacing,Dimension)
#define VisibleItemCount(w) XmField(w,ipot,DtComboBox,visible_item_count,int)

/*
 * DtComboBoxWidget specific defines.
 */
#define PUnitType(w)	    w->primitive.unit_type
#define ShellPoppedUp(w)    w->shell.popped_up
#define MUnitType(w)	    w->manager.unit_type
#define COMBO_SHADOW(w)	    w->manager.shadow_thickness
#define LayoutDirection(w)  w->manager.string_direction
#define NavigationType(w)   w->manager.navigation_type
#define TraversalOn(w)	    w->manager.traversal_on
#define BackgroundGC(w)	    w->manager.background_GC
#define HighlightGC(w)	    w->manager.highlight_GC
#define TopShadowGC(w)	    w->manager.top_shadow_GC
#define BottomShadowGC(w)   w->manager.bottom_shadow_GC
#define BackgroundPixel(w)  w->core.background_pixel
#define X(w)		    w->core.x
#define Y(w)		    w->core.y
#define Width(w)  	    w->core.width
#define Height(w)  	    w->core.height
#define BorderWidth(w)	    w->core.border_width
#define Sensitive(w)	    w->core.sensitive
#define AncestorSensitive(w)	w->core.ancestor_sensitive
#define Parent(w)	    w->core.parent
#define COMBO_MARGIN_W(w)   MarginWidth(w)
#define COMBO_MARGIN_H(w)   MarginHeight(w)
#define COMBO_H_SPACING(w)  HorizontalSpacing(w)
#define COMBO_V_SPACING(w)  VerticalSpacing(w)

#define GRAB_POINTER        1 << 0
#define GRAB_KEYBOARD       1 << 1

#define DtNonePopup	    0
#define DtPopup		    1 << 0
#define DtButtonPressPopup  1 << 1
#define DtKeyPressPopup     1 << 2

#define LIST_EVENTS	    (ButtonReleaseMask | FocusChangeMask | EnterWindowMask)
#define SHELL_EVENTS	    (ButtonPressMask | ButtonReleaseMask)
#define INVALID_DIMENSION   (0xFFFF)


static char ComboBoxTranslationTable[] = "\
	<FocusIn>:	     ComboBoxFocusIn() \n\
	<FocusOut>:	     ComboBoxFocusOut() \n\
        <Key>osfDown:        ComboBoxActivate() \n\
        <Btn1Down>:	     ComboBoxActivate() \n\
        <Key>osfSelect:      ComboBoxActivate() \n\
        ~s ~m ~a <Key>space: ComboBoxActivate() \n\
";

static char ComboBoxLabelTranslationTable[] = "\
        <Key>osfDown:        ComboBoxActivate(label) \n\
        <Btn1Down>: 	     ComboBoxActivate(label) \n\
        <Key>osfSelect:      ComboBoxActivate(label) \n\
        ~s ~m ~a <Key>space: ComboBoxActivate(label) \n\
";

/* Keyboard Only Traversing During Editable-Mode */
static char ComboBoxTextTranslationTable[] = "\
        <Key>osfUp:          ComboBoxActivate(label) \n\
        <Key>osfDown:        ComboBoxActivate(label) \n\
";

static char ComboBoxButtonTranslationTable[] = "\
        <Key>osfDown:        ComboBoxActivate(label) \n\
        <Btn1Down>:	     ComboBoxActivate(label) \n\
	~s ~m ~a <Key>space: ComboBoxActivate(label) \n\
	s ~m ~a <Key>Tab:    ComboBoxPrevTabGroup()\n\
	~m ~a <Key>Tab:      ComboBoxNextTabGroup()\n\
";

static char ComboBoxListTranslationTable[] = "\
        <Key>osfCancel:      ListKbdCancel() ComboBoxKbdCancel() \n\
";

static XtActionsRec ComboBoxActionTable[] = {
       {"ComboBoxFocusIn",   (XtActionProc)_ComboBoxFocusIn},
       {"ComboBoxFocusOut",  (XtActionProc)_ComboBoxFocusOut},
       {"ComboBoxActivate",  (XtActionProc)_ComboBoxActivate},
       {"ComboBoxKbdCancel", (XtActionProc)_ComboBoxKbdCancel},
       {"ComboBoxPrevTabGroup", (XtActionProc)_ComboBoxPrevTabGroup},
       {"ComboBoxNextTabGroup", (XtActionProc)_ComboBoxNextTabGroup},
};


/* 
 * DtComboBoxWidget resources 
 */
#define offset(field) XtOffset(DtComboBoxWidget, field)
#define DtOffset(field) XmPartOffset(DtComboBox,field)
static XmPartResource resources[] = {
    {XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension, 
	 sizeof(Dimension), offset(manager.shadow_thickness),
	 XmRImmediate, (XtPointer)TEXT_FIELD_SHADOW},

    /* 
     * ComboBox specific resources
     */
    {DtNactivateCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(activate_callback), XmRCallback, 
	 (XtPointer)NULL},
    {DtNalignment, DtCAlignment, XmRAlignment, sizeof(unsigned char),
	 DtOffset(alignment), XmRImmediate, 
	 (XtPointer)DtALIGNMENT_END},
    {DtNarrowSpacing, DtCArrowSpacing, XmRHorizontalDimension,
	 sizeof(Dimension), DtOffset(arrow_spacing),
	 XmRImmediate, (XtPointer)0},
    {DtNarrowType, DtCArrowType, DtRArrowType, sizeof(unsigned char),
	 DtOffset(arrow_type), XmRImmediate, (XtPointer)DtMOTIF},
    {DtNcolumns, DtCColumns, XmRShort, sizeof(short),
	 DtOffset(text_columns), XmRImmediate, (XtPointer)20},
    {DtNfocusCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(focus_callback), XmRCallback, 
	 (XtPointer)NULL},
    {DtNhorizontalSpacing, DtCHorizontalSpacing, XmRHorizontalDimension,
	 sizeof(Dimension), DtOffset(horizontal_spacing),
	 XmRImmediate, (XtPointer)INVALID_DIMENSION},
    {DtNitemCount, DtCItemCount, XmRInt, sizeof(int), 
	 DtOffset(item_count), XmRImmediate, (XtPointer)0},
    /*
     * items is used only for seeing if the user changed the list.  It
     * is only a pointer that reflects the current list's items.
     */
    {DtNitems, DtCItems, XmRXmStringTable, sizeof(XmStringTable),
	 DtOffset(items), XmRImmediate, (XtPointer)NULL},
    {DtNlabelString, DtCXmString, XmRXmString, sizeof(XmString),
	 DtOffset(label_string), XmRImmediate, (XtPointer)NULL},
    {DtNlist, DtCList, XmRWidget, sizeof(Widget),
	 DtOffset(list), XmRImmediate, (XtPointer)NULL},
    {DtNlistFontList, DtCListFontList, XmRFontList, sizeof(XmFontList), 
	 DtOffset(list_font_list), XmRImmediate, (XtPointer)NULL},
    {DtNlistMarginHeight, DtCListMarginHeight, XmRVerticalDimension, 
	 sizeof(Dimension), DtOffset(list_margin_height),
	 XmRImmediate, (XtPointer)MARGIN},
    {DtNlistMarginWidth, DtCListMarginWidth, XmRHorizontalDimension,
	 sizeof(Dimension), DtOffset(list_margin_width),
	 XmRImmediate, (XtPointer)MARGIN},
    {DtNlistSpacing, DtCListSpacing, XmRVerticalDimension,sizeof(Dimension), 
	 DtOffset(list_spacing), XmRImmediate, (XtPointer)0},
    {DtNlosingFocusCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(losing_focus_callback), XmRCallback, 
	 (XtPointer)NULL},
    {DtNmarginHeight, DtCMarginHeight, XmRVerticalDimension,
	 sizeof(Dimension), DtOffset(margin_height),
	 XmRImmediate, (XtPointer)MARGIN},
    {DtNmarginWidth, DtCMarginWidth, XmRHorizontalDimension, sizeof(Dimension),
	 DtOffset(margin_width), XmRImmediate, (XtPointer)MARGIN},
    {DtNmaxLength, DtCMaxLength, XmRInt, sizeof(unsigned int),
	 DtOffset(text_max_length), XmRImmediate, (XtPointer)INT_MAX},
    {DtNmenuPostCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(menu_post_callback), XmRCallback, (XtPointer)NULL},
    {DtNorientation, DtCOrientation, XmROrientation, sizeof(unsigned char),
	 DtOffset(orientation), XmRImmediate, (XtPointer)DtRIGHT},
    {DtNpoppedUp, DtCPoppedUp, XmRBoolean, sizeof(Boolean),
	 DtOffset(popped_up), XmRImmediate, (XtPointer)FALSE},
    {DtNrecomputeSize, DtCRecomputeSize, XmRBoolean, sizeof(Boolean),
	 DtOffset(recompute_size), XmRImmediate, (XtPointer)TRUE},
    {DtNselectedItem, DtCXmString, XmRXmString, sizeof(XmString),
	 DtOffset(selected_item), XmRImmediate, (XtPointer)NULL},
    {DtNselectedPosition, DtCSelectedPosition, XmRInt, sizeof(int),
	 DtOffset(selected_position), XmRImmediate, (XtPointer)0},
    {DtNselectionCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(selection_callback), XmRCallback, (XtPointer)NULL},
    {DtNtextField, DtCTextField, XmRWidget, sizeof(Widget),
	 DtOffset(text), XmRImmediate, (XtPointer)NULL},
    {DtNtopItemPosition, DtCTopItemPosition, XmRInt, sizeof(int), 
	 DtOffset(top_item_position), XmRImmediate, (XtPointer)1},
    {DtNcomboBoxType, DtCComboBoxType, DtRComboBoxType, sizeof(unsigned char),
	 DtOffset(type), XmRImmediate,(XtPointer)DtDROP_DOWN_LIST},
    {DtNupdateLabel, DtCUpdateLabel, XmRBoolean, sizeof(Boolean),
	 DtOffset(update_label), XmRImmediate, (XtPointer)TRUE},
    {DtNvisibleItemCount, DtCVisibleItemCount, XmRInt, sizeof(int),
	 DtOffset(visible_item_count), XmRImmediate, (XtPointer)10},
    {DtNverticalSpacing, DtCVerticalSpacing, XmRVerticalDimension,
	 sizeof(Dimension), DtOffset(vertical_spacing),
	 XmRImmediate, (XtPointer)INVALID_DIMENSION},
};

/*
 * List resources (used for GetValues).
 */
static XmSyntheticResource syn_resources[] = {
    {DtNarrowSpacing, sizeof(Dimension), DtOffset(arrow_spacing), 
         XmeFromHorizontalPixels, XmeToHorizontalPixels},
    {DtNhorizontalSpacing, sizeof(Dimension), DtOffset(horizontal_spacing), 
         XmeFromHorizontalPixels, XmeToHorizontalPixels},
    {DtNverticalSpacing, sizeof(Dimension), DtOffset(vertical_spacing), 
         XmeFromVerticalPixels, XmeToVerticalPixels},
    {DtNmarginWidth, sizeof(Dimension), DtOffset(margin_width), 
         XmeFromHorizontalPixels, XmeToHorizontalPixels},
    {DtNmarginHeight, sizeof(Dimension), DtOffset(margin_height), 
         XmeFromVerticalPixels, XmeToVerticalPixels},
    {DtNarrowSize, sizeof(Dimension), DtOffset(arrow_size), 
         _DtComboBoxGetArrowSize, XmeToHorizontalPixels},
    {DtNlabelString, sizeof(XmString), DtOffset(label_string), 
         _DtComboBoxGetLabelString, _XmSetSyntheticResForChild},
    {DtNitemCount, sizeof(int), DtOffset(item_count), 
         _DtComboBoxGetListItemCount, _XmSetSyntheticResForChild},
    {DtNitems, sizeof(XmStringTable), DtOffset(items), 
         _DtComboBoxGetListItems, _XmSetSyntheticResForChild},
    {DtNlistFontList, sizeof(XmFontList), DtOffset(list_font_list), 
         _DtComboBoxGetListFontList, _XmSetSyntheticResForChild},
    {DtNlistMarginHeight, sizeof(Dimension), 
         DtOffset(list_margin_height),
         _DtComboBoxGetListMarginHeight, XmeToVerticalPixels},
    {DtNlistMarginWidth, sizeof(Dimension),DtOffset(list_margin_width),
         _DtComboBoxGetListMarginWidth, XmeToHorizontalPixels},
    {DtNlistSpacing, sizeof(Dimension), DtOffset(list_spacing),
         _DtComboBoxGetListSpacing, XmeToVerticalPixels},
    {DtNtopItemPosition, sizeof(int), DtOffset(top_item_position),
         _DtComboBoxGetListTopItemPosition, _XmSetSyntheticResForChild},
    {DtNvisibleItemCount, sizeof(int), DtOffset(visible_item_count),
         _DtComboBoxGetListVisibleItemCount, _XmSetSyntheticResForChild},
};
#undef DtOffset
#undef offset

/* Need Class Extension for widget navigation */
static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    (XtInitProc)NULL,			/* InitializePrehook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPrehook	*/
    (XtInitProc)NULL,			/* InitializePosthook	*/
    (XtSetValuesFunc)NULL,		/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    (XtInitProc)NULL,			/* secondaryCreate	*/
    (XmGetSecResDataFunc)NULL, 		/* getSecRes data	*/
    { 0 },      			/* fastSubclass flags	*/
    (XtArgsProc)NULL,			/* getValuesPrehook	*/
    (XtArgsProc)NULL,			/* getValuesPosthook	*/
    (XtWidgetClassProc)NULL,            /* classPartInitPrehook */
    (XtWidgetClassProc)NULL,            /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    (XmWidgetNavigableProc)WidgetNavigable,
					/* widgetNavigable      */
    (XmFocusChangeProc)NULL,            /* focusChange          */
    (XmWrapperData)NULL			/* wrapperData 		*/
};

/*
 * Define Class Record.
 */
externaldef(dtcomboBoxclassrec) DtComboBoxClassRec dtComboBoxClassRec =
{
    {		/* core_class fields      */
    (WidgetClass)&(xmManagerClassRec),		/* superclass         */    
    (String)"DtComboBox",			/* class_name         */    
    (Cardinal)sizeof(DtComboBoxPart),		/* widget_size        */    
    (XtProc)ClassInitialize,			/* class_initialize   */    
    (XtWidgetClassProc)NULL,			/* class_part_init    */    
    (XtEnum)FALSE,				/* class_inited       */    
    (XtInitProc)Initialize,			/* initialize         */    
    (XtArgsProc)NULL,				/* initialize_hook    */    
    (XtRealizeProc)XtInheritRealize,		/* realize            */    
    (XtActionList)ComboBoxActionTable,		/* actions	       */    
    (Cardinal)XtNumber(ComboBoxActionTable),	/* num_actions        */    
    (XtResourceList)resources,			/* resources          */    
    (Cardinal)XtNumber(resources),		/* num_resources      */    
    (XrmClass)NULLQUARK,			/* xrm_class          */    
    (Boolean)TRUE,				/* compress_motion    */    
    (XtEnum)XtExposeCompressMaximal,		/* compress_exposure  */    
    (Boolean)TRUE,				/* compress_enterleave*/    
    (Boolean)FALSE,				/* visible_interest   */    
    (XtWidgetProc)Destroy,			/* destroy            */    
    (XtWidgetProc)Resize,			/* resize             */    
    (XtExposeProc)Redisplay,			/* expose             */    
    (XtSetValuesFunc)SetValues,			/* set_values         */    
    (XtArgsFunc)NULL,				/* set values hook    */    
    (XtAlmostProc)XtInheritSetValuesAlmost,	/* set values almost  */    
    (XtArgsProc)NULL,				/* get values hook    */    
    (XtAcceptFocusProc)NULL,			/* accept_focus       */    
    (XtVersionType)XtVersionDontCheck,		/* Version            */    
    (XtPointer)NULL,				/* PRIVATE cb list    */
    (String)XtInheritTranslations,		/* tm_table           */
    (XtGeometryHandler)XtInheritQueryGeometry,	/* query_geom         */
    (XtStringProc)XtInheritDisplayAccelerator,	/* display_accelerator*/
    (XtPointer)&baseClassExtRec			/* extension	      */
    },
    {		/* composite_class fields */
    (XtGeometryHandler)GeometryManager,		/* geometry_manager   */     
    (XtWidgetProc)XtInheritChangeManaged,	/* change_managed     */     
    (XtWidgetProc)XtInheritInsertChild,		/* insert_child	      */     
    (XtWidgetProc)XtInheritDeleteChild,		/* delete_child	      */     
    (XtPointer)NULL				/* extension	      */     
    },
    {		/* constraint_class fields */
    (XtResourceList)NULL,			/* resources	      */     
    (Cardinal)0,				/* num_resources      */     
    (Cardinal)0,				/* constraint_size    */     
    (XtInitProc)NULL,				/* initialize	      */     
    (XtWidgetProc)NULL,				/* destroy	      */     
    (XtSetValuesFunc)NULL,			/* set_values	      */     
    (XtPointer)NULL				/* extension          */     
    },
    {		/* manager class     */
    (String)XtInheritTranslations,		/* translations       */     
    (XmSyntheticResource*)syn_resources,	/* syn resources      */     
    (int)XtNumber(syn_resources),		/* num syn_resources  */     
    (XmSyntheticResource*)NULL,			/* get_cont_resources */     
    (int)0,					/* num_get_cont_resources */ 
    (XmParentProcessProc)XmInheritParentProcess,/* parent_process     */     
    (XtPointer)NULL				/* extension          */     
    },
    {		/* combo_box_class fields */     
    (Boolean)0,
    }
};

externaldef(dtcomboBoxwidgetclass) WidgetClass dtComboBoxWidgetClass =
					(WidgetClass)&dtComboBoxClassRec;
/* Parse the translation tables only once for the whole class
 */
static XtTranslations trans;
static XtTranslations list_trans; 
static XtTranslations label_trans; 
static XtTranslations text_trans; 
static XtTranslations button_trans; 

/* 
 * Must set up the record type for the class extensions to work.
 */
static void
ClassInitialize(void)
{
    baseClassExtRec.record_type = XmQmotif;
/*
 * MotifBc
 */
    XmResolveAllPartOffsets(dtComboBoxWidgetClass, &ipot, &cpot);
/* Parse the translation tables here
 */
    trans = 	  XtParseTranslationTable(ComboBoxTranslationTable);
    list_trans =  XtParseTranslationTable(ComboBoxListTranslationTable);
    label_trans = XtParseTranslationTable(ComboBoxLabelTranslationTable);
    text_trans =  XtParseTranslationTable(ComboBoxTextTranslationTable);
    button_trans =XtParseTranslationTable(ComboBoxButtonTranslationTable);

/* Add a type converter for String to DtRComboBoxType
 */
    XtSetTypeConverter("String","ComboBoxType", _CvtStringToType, NULL, 0, 
	XtCacheAll, NULL);

    InitLabel = XmStringCreateLocalized(CB_LABEL);
}

/*
 * ComboBox initialization function.  This builds the widgets inside
 * our widget, to get the correct layout.  If the type resource
 * is DtDROP_DOWN_COMBO_BOX, we create a textField; if FALSE, we create a
 * label.  If the user changes this resource later, we will create the
 * other widget (textField or Label).  We don't want to carry backage from
 * both widgets if the user never changes the type resource.
 */
static void
Initialize(	DtComboBoxWidget request,
		DtComboBoxWidget new,
		ArgList given_args,
		Cardinal *num_args)
{
   /* use the address of the first element of DtComboBoxPart structure 
    * as of DtComboBoxPart 
    */    
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(new,ipot,DtComboBox,arrow,Widget)); 
    Boolean force_label_string = FALSE;
    Arg args[15];
    int n;
    /* Resolution Independent */
    unsigned char unit_type = MUnitType(new);

    /* Overwrite the manager's focusIn and focusOut translations */
    XtOverrideTranslations((Widget)new, trans);


    /* 
     * force_label_string usage if it is specified and items is not.
     * This will be the perminant label string only if update-label
     * is false, else it is only used until the user picks something
     * new off the list.
     */
    if (LabelString(new) == NULL)
	LabelString(new) = InitLabel;
    else if (!Items(new))
	force_label_string = TRUE;

    /* Copy given label-string. */
    if (LabelString(new))
	LabelString(new) = XmStringCopy(LabelString(new));

    Text(new) = (Widget)NULL;
    Label(new) = (Widget)NULL;
    Sep(new) = (Widget)NULL;
    OldWidth(new) = 0;
    OldHeight(new) = 0;

    CheckResources(new);

    /*
     * Create the text or label depending on the type resource.
     * When part of X-Designer, we create both at initialization to
     * avoid later crashes.
     */
    if (Type(new) == DtDROP_DOWN_COMBO_BOX)
    {
	n = 0;
	XtSetArg(args[n], XmNcolumns, TextColumns(new)); n++;
	XtSetArg(args[n], XmNmaxLength, TextMaxLength(new)); n++;
	XtSetArg(args[n], XmNmarginWidth, TEXT_CONTEXT_MARGIN); n++;
	XtSetArg(args[n], XmNmarginHeight, 2); n++;
	/* Resolution Independent */
	if (unit_type != XmPIXELS) {
		XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
	}
	Text(new) = XtCreateManagedWidget("Text", 
					      xmTextFieldWidgetClass,
					      (Widget)new, args, n);
	XtAddCallback(Text(new), XmNlosingFocusCallback, 
		      text_losing_focus_cb, (XtPointer)new);
	XtAddCallback(Text(new), XmNactivateCallback, 
		      text_activate_cb, (XtPointer)new);
	XtAddCallback(Text(new), XmNfocusCallback, 
		      text_focus_cb, (XtPointer)new);

        XtOverrideTranslations(Text(new), text_trans);

	if (HorizontalSpacing(new) == INVALID_DIMENSION)
	    HorizontalSpacing(new) = 0;
	if (VerticalSpacing(new) == INVALID_DIMENSION)
	    VerticalSpacing(new) = 0;
        if (unit_type != XmPIXELS) {
                XtSetArg(args[0], XmNunitType, unit_type);
		XtSetValues(Text(new), args, 1);
        }                
    }
    else
    {
        XmStringDirection new_direction = 
	  XmDirectionToStringDirection(LayoutDirection(new));

	COMBO_SHADOW(new) = LABEL_SHADOW;
	n = 0;
	XtSetArg(args[n], XmNalignment, Alignment(new)); n++;
	XtSetArg(args[n], XmNrecomputeSize, FALSE); n++;
	XtSetArg(args[n], XmNlabelString, InitLabel); n++;
	XtSetArg(args[n], XmNmarginLeft, LABEL_PADDING); n++;
	XtSetArg(args[n], XmNmarginRight, LABEL_PADDING); n++;
	XtSetArg(args[n], XmNmarginWidth, TEXT_CONTEXT_MARGIN); n++;
	XtSetArg(args[n], XmNmarginHeight, 0); n++;
	XtSetArg(args[n], XmNstringDirection, new_direction); n++;
        /* Resolution Independent */
        if (unit_type != XmPIXELS) {
                XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
        }
	Label(new) = XtCreateManagedWidget("Label", 
					       xmLabelWidgetClass,
					       (Widget)new, args, n);
	XtOverrideTranslations(Label(new), label_trans);
	if (HorizontalSpacing(new) == INVALID_DIMENSION)
	    HorizontalSpacing(new) = 1;
	if (VerticalSpacing(new) == INVALID_DIMENSION)
	    VerticalSpacing(new) = 2;
        if (unit_type != XmPIXELS) {
                XtSetArg(args[0], XmNunitType, unit_type);
                XtSetValues(Label(new), args, 1);
        }                
    }

    /*
     * Create the separator used if non-editable combo-box.
     */
    if (Type(new) == DtDROP_DOWN_LIST) 
    {
	n = 0;
	XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
        /* Resolution Independent */
        if (unit_type != XmPIXELS) {
                XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
        }
	Sep(new) = XtCreateManagedWidget("ComboBoxSeparator", 
					     xmSeparatorWidgetClass,
					     (Widget)new, args, n);

	XtOverrideTranslations((Widget)Sep(new), button_trans);
        if (unit_type != XmPIXELS) {
                XtSetArg(args[0], XmNunitType, unit_type);
                XtSetValues(Sep(new), args, 1);
        }                
    }

    /*
     * Create the ArrowWidget.
     */
    n = 0;
    XtSetArg(args[n], XmNtraversalOn, FALSE); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    if (ArrowType(new) == DtMOTIF) {
	XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN); n++;
	XtSetArg(args[n], XmNforeground, BackgroundPixel(new)); n++;
	Arrow(new) = XtCreateManagedWidget("ComboBoxArrow", 
					       xmArrowButtonWidgetClass,
					       (Widget)new, args, n);
    }
    else {
	Arrow(new) = XtCreateManagedWidget("ComboBoxArrow", 
					       xmDrawnButtonWidgetClass,
					       (Widget)new, args, n);
	XtAddCallback(Arrow(new), XmNexposeCallback, arrow_expose_cb, 
		      (XtPointer)new);
    }
    XtAddCallback(Arrow(new), XmNactivateCallback, activate_cb, 
		  (XtPointer)new);

    XtOverrideTranslations((Widget)Arrow(new), button_trans); 

    /*
     *  Create the shell and associated list widgets.
     */
    n = 0;
    XtSetArg(args[n], XtNoverrideRedirect, TRUE); n++;
    XtSetArg(args[n], XtNallowShellResize, TRUE); n++;
    XtSetArg(args[n], XtNsaveUnder, TRUE); n++;
    Shell(new) = XtCreatePopupShell("ComboBoxMenuShell", 
					topLevelShellWidgetClass, 
					(Widget)new, args, n);
    
    n = 0;
    Frame(new) = XtCreateManagedWidget("ComboBoxRowColumn",
					   xmFrameWidgetClass,
					   Shell(new), args, n);

    n = 0;
    /* Store combo widget in list for later use */
    XtSetArg(args[n], XmNuserData, (XtPointer)new); n++;
    if (ListFontList(new)) {
	XtSetArg(args[n], XmNfontList, ListFontList(new)); n++;
    }
    /* to disable double click */
    XtSetArg(args[n], XmNdoubleClickInterval, 0); n++;
    XtSetArg(args[n], XmNitemCount, ItemCount(new)); n++;
    XtSetArg(args[n], XmNitems, Items(new)); n++;
    XtSetArg(args[n], XmNlistMarginHeight, ListMarginHeight(new)); n++;
    XtSetArg(args[n], XmNlistMarginWidth, ListMarginWidth(new)); n++;
    XtSetArg(args[n], XmNlistSpacing, ListSpacing(new)); n++;
    {
      XmStringDirection new_direction =
	XmDirectionToStringDirection(LayoutDirection(new));
      XtSetArg(args[n], XmNstringDirection, new_direction); n++;
    }
    XtSetArg(args[n], XmNtopItemPosition, TopItemPosition(new)); n++;
    XtSetArg(args[n], XmNvisibleItemCount, VisibleItemCount(new)); n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE); n++;
    XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
    /* Resolution Independent */
    if (unit_type != XmPIXELS) {
        XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
    }
    List(new) = XmCreateScrolledList(Frame(new), "List", args, n);
    XtOverrideTranslations((Widget)List(new), list_trans);

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, (XtArgVal) 0); n++;
    XtSetValues(XtParent(List(new)), args, n);

    /* selected_item resource used before selected_position */
    if (SelectedItem(new) && XmeStringIsValid(SelectedItem(new)) ) {
	SelectedItem(new) =XmStringCopy(SelectedItem(new));
	DtComboBoxSelectItem((Widget)new, SelectedItem(new));
    }
    else {
	SelectedItem(new) = (XmString) NULL;
	if (SelectedPosition(new)<0 || SelectedPosition(new)>=ItemCount(new))
	    SelectedPosition(new) = 0;
	if ( ItemCount(new))
	    XmListSelectPos(List(new), SelectedPosition(new) + 1, FALSE);
    }
    if (unit_type != XmPIXELS) {
            XtSetArg(args[0], XmNunitType, unit_type);
            XtSetValues(List(new), args, 1);
    }                

    SyncWithList(combo_p);
    XtManageChild(List(new));
    XtRealizeWidget(Shell(new));

    MaxShellWidth(new) = Width(((Widget) Shell(new)));
    MaxShellHeight(new) = Height(((Widget) Shell(new)));

    XtAddCallback(List(new), XmNdefaultActionCallback, select_cb, new);
    XtAddCallback(List(new), XmNbrowseSelectionCallback, select_cb, new);

    /*
     * Set up event handlers needed for handling grab states.
     */
    XtInsertEventHandler(List(new), LIST_EVENTS, TRUE,
			 (XtEventHandler)list_event_handler, 
			 (XtPointer)new, XtListHead);
    XtInsertEventHandler(Shell(new), SHELL_EVENTS, TRUE,
			 (XtEventHandler)shell_event_handler, 
			 (XtPointer)new, XtListHead);

    /*
     * Set initial value in text or label if items was specified
     */
    if (Type(new) == DtDROP_DOWN_LIST) {
	SetMaximumLabelSize(combo_p);
	SetLabelData(combo_p, NULL, force_label_string);
    }
    else
	SetTextFieldData(combo_p, NULL);

    SetComboBoxSize(new);
    LayoutChildren(new);
}


/*
 * Allow the manager to gain focus if not editable.  If editable (using
 * text-field), then let the toolkit give focus to the text-field.
 */
static XmNavigability
WidgetNavigable(DtComboBoxWidget combo)
{   
    XmNavigationType nav_type = NavigationType(((XmManagerWidget)combo));

    if (Sensitive(combo) &&  AncestorSensitive(combo) &&
	TraversalOn(((XmManagerWidget)combo))) {
	if ((nav_type == XmSTICKY_TAB_GROUP) ||
	    (nav_type == XmEXCLUSIVE_TAB_GROUP) ||
	    ((nav_type == XmTAB_GROUP) && 
	     !_XmShellIsExclusive((Widget)combo))) {
	    if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
		return(XmDESCENDANTS_TAB_NAVIGABLE);
	    else
		return(XmTAB_NAVIGABLE);
	}
	return(XmDESCENDANTS_NAVIGABLE);
    }
    return(XmNOT_NAVIGABLE);
}

/* 
 * The combo_box gets focus.
 */
static void 
_ComboBoxFocusIn(	DtComboBoxWidget combo,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    DrawHighlight(combo, FALSE);
}

/* 
 * The combo_box loses focus. Only happens if not editable.
 */
static void 
_ComboBoxFocusOut(	DtComboBoxWidget combo,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    DrawHighlight(combo, TRUE);
}

/*
 * This function gets called whenever we draw or clear the shadow (to
 * redraw highlight during resize, etc), as well as during focus_in
 * and focus_out events.
 */
static void
DrawHighlight(  DtComboBoxWidget combo,
		Boolean clear)
{
    XRectangle rect[4] ;

    if (XtIsRealized((Widget)combo)) {
	if (clear) {
	    rect[0].x = rect[1].x = rect[2].x = 0;
	    rect[3].x = OldWidth(combo) - COMBO_MARGIN_W(combo);
	    rect[0].y = rect[2].y = rect[3].y = 0 ;
	    rect[1].y = OldHeight(combo) - COMBO_MARGIN_H(combo);
	    rect[0].width = rect[1].width = OldWidth(combo);
	    rect[2].width = rect[3].width = COMBO_MARGIN_W(combo);
	    rect[0].height = rect[1].height = COMBO_MARGIN_H(combo);
	    rect[2].height = rect[3].height = OldHeight(combo);
	    XFillRectangles(XtDisplayOfObject((Widget)combo),
			    XtWindowOfObject((Widget)combo), 
			    BackgroundGC(combo), rect, 4);
	}
	else if (XmGetFocusWidget((Widget)combo) == (Widget)combo) {
	    rect[0].x = rect[1].x = rect[2].x = 0;
	    rect[3].x = XtWidth(combo) - COMBO_MARGIN_W(combo);
	    rect[0].y = rect[2].y = rect[3].y = 0 ;
	    rect[1].y = XtHeight(combo) - COMBO_MARGIN_H(combo);
	    rect[0].width = rect[1].width = XtWidth(combo);
	    rect[2].width = rect[3].width = COMBO_MARGIN_W(combo);
	    rect[0].height = rect[1].height = COMBO_MARGIN_H(combo);
	    rect[2].height = rect[3].height = XtHeight(combo);
	    XFillRectangles(XtDisplayOfObject((Widget)combo),
			    XtWindowOfObject((Widget)combo), 
			    HighlightGC(combo), rect, 4);
	}
    }
}

/* Add a global variable to avoid the handler to process the
 * the ButtonPress which popup the shell
 */
static int popup_shell_init=DtNonePopup;

/*
 * osfSelect virtual key hit.  Simulate hitting the arrow.
 */
static void 
_ComboBoxActivate(	Widget w,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    DtComboBoxWidget combo;
    XmAnyCallbackStruct cb;

    if (*num_params == 0) /* no params means combo */
	combo = (DtComboBoxWidget)w;
    else /* params means label */
    {
	combo = (DtComboBoxWidget)XtParent(w);
    }

    _DtProcessLock();
    if (event->type == KeyPress)
        popup_shell_init = DtKeyPressPopup;
    else if (event->type == ButtonPress)
        popup_shell_init = DtButtonPressPopup;
    else
        popup_shell_init = DtPopup;
    _DtProcessUnlock();

    cb.reason = XmCR_ACTIVATE;
    cb.event  = event;
    activate_cb((Widget)Arrow(combo), (XtPointer)combo, (XtPointer)&cb);
}

/*
 * osfCancel virtual key hit.
 */
static void 
_ComboBoxKbdCancel(	Widget w,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    DtComboBoxWidget combo;
    XtPointer data;
    Widget new;
    Arg args[1];

    /* Get combo-box off list data */
    XtSetArg(args[0], XmNuserData, &data);
    XtGetValues(w, args, 1);
    
    combo = (DtComboBoxWidget)data;

    input_ungrab( combo, GRAB_POINTER | GRAB_KEYBOARD );
}

/* #7
 * Tab Group Action for Buttons
 */
static void 
_ComboBoxPrevTabGroup(	Widget w,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
   XmProcessTraversal ((Widget)XtParent(w), XmTRAVERSE_PREV_TAB_GROUP);
}
static void 
_ComboBoxNextTabGroup(	Widget w,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
   XmProcessTraversal ((Widget)XtParent(w), XmTRAVERSE_NEXT_TAB_GROUP);
}


/*
 * This function goes through most of the resources and makes sure 
 * they have legal values.
 */
static void
CheckResources(	DtComboBoxWidget combo)
{
    if ((Alignment(combo) != DtALIGNMENT_CENTER) && 
	(Alignment(combo) != DtALIGNMENT_BEGINNING) &&
	(Alignment(combo) != DtALIGNMENT_END)) {
	XtWarning(CB_ALIGNMENT);
	Alignment(combo) = DtALIGNMENT_CENTER;
    }
    if ((Orientation(combo) != DtLEFT) &&
	(Orientation(combo) != DtRIGHT)) {
	XtWarning(CB_ORIENTATION);
	Orientation(combo) = DtRIGHT;
    }
    if (ItemCount(combo) < 0) {
	XtWarning(CB_ITEM_COUNT);
	ItemCount(combo) = 0;
    }
    if ((SelectedPosition(combo) < 0) ||
	((SelectedPosition(combo) >= ItemCount(combo)) && 
	 (ItemCount(combo) > 0))) {
	XtWarning(CB_VISIBLE_ITEM);
	SelectedPosition(combo) = 0;
    }
    /* NEW: to adjust the size of the list to its content *
    if (ItemCount(combo) < VisibleItemCount(combo)) 
	VisibleItemCount(combo) = ItemCount(combo); */
}

/*
 * Destroy procedure called by the toolkit.  Free local resources
 */
static void 
Destroy(DtComboBoxWidget combo)
{
    if (LabelString(combo))
	XmStringFree(LabelString(combo));
    if (SelectedItem(combo))
	XmStringFree( SelectedItem(combo) );
}

/*
 * Resize function called by toolkit.  The size of our combo-box
 * has already been changed.  That is why we must store 
 * old_width and old_height.
 */
static void
Resize(DtComboBoxWidget combo)
{
    ClearShadow(combo, TRUE);
    LayoutChildren(combo);
    DrawShadow(combo);
    OldWidth(combo) = Width(combo);
    OldHeight(combo) = Height(combo);
}


/*
 * Redisplay function called by toolkit. The widget didn't change size, 
 * so just redisplay the shadow.
 */
static void
Redisplay(	DtComboBoxWidget w,
		XEvent *event,
		Region region)
{
    DrawShadow(w);
}


/*
 * GeometryManager function called by toolkit when a child resizes/moves.
 * We are not allowing any changes but width/height of the text-field.
 * this is because the user can retrieve the text-field and make changes
 * that we want to honor.  If they mess around with the label or arrow,
 * then we won't honor the request.
 * If the text-field requests a change, then make the change, and allow
 * our SetComboBoxSize() and LayoutChildren() figure out what size will
 * be allowed.  
 * Returning GeometryDone was suppose to tell the toolkit
 * that we resized the child ourselves, but the text-field had trouble
 * with this (its' geometry_manager wasn't called or working right?), so
 * we return GeometryYes.
 */
static XtGeometryResult
GeometryManager(Widget w,
		XtWidgetGeometry *request,
		XtWidgetGeometry *reply)
{
    DtComboBoxWidget combo = (DtComboBoxWidget) Parent(w);

    /* Ignore everything but text-field */
    if (w != Text(combo))
	return(XtGeometryNo);

    /* Only allow width/height changes */
    if (!(request->request_mode & (CWWidth | CWHeight)))
	return(XtGeometryNo);
    
    /* Set the text-field to the requested size */
    if (request->request_mode & CWWidth)
	Width(w) = request->width;
    if (request->request_mode & CWHeight)
	Height(w) = request->height;
    XtResizeWidget(w, Width(w), Height(w), BorderWidth(w));
    
    ClearShadow(combo, TRUE);
    if (RecomputeSize(combo))
	SetComboBoxSize(combo);
    LayoutChildren(combo);
    DrawShadow(combo);
    return(XtGeometryYes);
}

/* 
 * This function sets the size of the combo_box widget based on the
 * current size of the children.  Don't worry if it doesn't work, the
 * children will be squeezed in later.
 */
static void
SetComboBoxSize(DtComboBoxWidget combo)
{
    Widget text_holder = ((Type(combo) == DtDROP_DOWN_COMBO_BOX) ? 
			  Text(combo) : Label(combo));
    Dimension shadow = COMBO_SHADOW(combo) * 2;
    Dimension h_spacing = COMBO_H_SPACING(combo) * 2;
    Dimension v_spacing = COMBO_V_SPACING(combo) * 2;
    Dimension arrow_width, text_width, text_height;
    Dimension sep_width = 0;
	XtGeometryResult ResizeResult;
    Arg args[3];
    unsigned char unit_type = XmPIXELS;

    /* Resolution Independent */
    if (MUnitType(combo) != XmPIXELS) {
	unit_type = MUnitType(combo);
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(text_holder, args, 1);
	if (Type(combo) == DtDROP_DOWN_LIST)
		XtSetValues((Widget)Sep(combo), args, 1);	
    }

    /* 
     * Find out how big the arrow can be (needed to get 
     * available_width for text_holder).
     */
    /* MotifBc */
    XtSetArg(args[0], XmNwidth, &text_width);
    XtSetArg(args[1], XmNheight, &text_height);
    XtGetValues(text_holder, args, 2);

    arrow_width = (Dimension)((float)text_height * ARROW_MULT);
    arrow_width = (arrow_width < ARROW_MIN) ? ARROW_MIN : arrow_width;

    if (Type(combo) == DtDROP_DOWN_LIST) {
	XtSetArg(args[0], XmNwidth, &sep_width);
	XtGetValues((Widget)Sep(combo), args, 1);
    }

    ResizeResult=XtMakeResizeRequest((Widget)combo, arrow_width + sep_width +
			      ArrowSpacing(combo) +
			      text_width + shadow + h_spacing +
			      (COMBO_MARGIN_W(combo) * 2), 
			      text_height + shadow + v_spacing +
			      (COMBO_MARGIN_H(combo) * 2), 
			      NULL, NULL);
    if (ResizeResult==XtGeometryNo || ResizeResult==XtGeometryAlmost) {
	XtWarning(CB_RESIZE);
    }
    OldWidth(combo) = Width(combo);
    OldHeight(combo) = Height(combo);

    /* Resolution Independent */
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(text_holder, args, 1);
	if (Type(combo) == DtDROP_DOWN_LIST)
		XtSetValues((Widget)Sep(combo), args, 1);	
    }
}

/*
 * This function makes the text_holder (label or text-field) smaller
 * if the combo_box couldn't grow to the needed full size.  It will
 * also make the text_holder grow if there is space.  The textfield will
 * grow with the combo_box, but the label will only grow to its' 
 * maximum size.  The label will also shrink down to nothing, but the
 * text-field will always keep its' core height.
 */
static void
ForceChildSizes(DtComboBoxWidget combo)
{
    Dimension full_available_height, available_height, available_width;
    Dimension arrow_width;
    Dimension sep_width = 0;
    Dimension tmp_width, tmp_height, tmp_borderwidth;
    Arg args[3];
    unsigned char unit_type = XmPIXELS;

    /* Resolution Independent */
    if (MUnitType(combo) != XmPIXELS) {
	unit_type = MUnitType(combo);
	XtSetArg(args[0], XmNunitType, XmPIXELS);
        if (Type(combo) == DtDROP_DOWN_LIST) {
		XtSetValues(Sep(combo), args, 1);
		XtSetValues(Label(combo), args, 1);
	}
	else
		XtSetValues(Text(combo), args, 1);
	XtSetValues(Arrow(combo), args, 1);
    }
    /* Calculate available height for children */
    if ((available_height = Height(combo) - (COMBO_SHADOW(combo) * 2) - 
	 (COMBO_MARGIN_H(combo) * 2) - (COMBO_V_SPACING(combo) * 2)) <= 0) {
	full_available_height = available_height = 1;
    }
    else {
	/* Seperator need available_height plus the vertical_spacing */
	full_available_height = (available_height + 
				 (COMBO_V_SPACING(combo) * 2));
    }

    /* Get initial available width for children */
    available_width = (Width(combo) - (COMBO_SHADOW(combo) * 2) - 
		       (COMBO_MARGIN_W(combo) * 2) - 
		       (COMBO_H_SPACING(combo) * 2));
    
    /* label only grows to maximum width needed */
    if ((Type(combo) == DtDROP_DOWN_LIST) && 
	((int)available_height > (int)LabelMaxHeight(combo)))
	available_height = LabelMaxHeight(combo);
    else if (Type(combo) == DtDROP_DOWN_COMBO_BOX) {
	XtSetArg(args[0], XmNheight, &available_height);
	XtGetValues((Widget)Text(combo), args, 1);
    }
    
    /* 
     * Find out how big the arrow can be (needed to get 
     * available_width for text_holder).
     */
    arrow_width = (Dimension)((float)available_height * ARROW_MULT);
    arrow_width = (arrow_width < ARROW_MIN) ? ARROW_MIN : arrow_width;
	
    if (Type(combo) == DtDROP_DOWN_LIST) {
        XtSetArg(args[0], XmNwidth, &sep_width);
        XtGetValues((Widget)Sep(combo), args, 1);
    }

    /* Make sure width isn't too small or too big */
    if ((available_width -= 
	 (arrow_width + sep_width + ArrowSpacing(combo) )) <= (Dimension)0)
	available_width = 1;

    /* Motif BC */
    XtSetArg(args[0], XmNwidth, &tmp_width);
    XtSetArg(args[1], XmNheight, &tmp_height);
    XtSetArg(args[2], XmNborderWidth, &tmp_borderwidth);
    if (Type(combo) == DtDROP_DOWN_LIST) {  /** label **/
	if ((int)available_width > (int)LabelMaxLength(combo))
	    available_width = LabelMaxLength(combo);

	/* Motif BC */
	XtGetValues((Widget)Label(combo), args, 3);
	if ((available_width != tmp_width) ||
	    (available_height != tmp_height))
	 	XtResizeWidget(Label(combo), available_width, available_height,
				tmp_borderwidth);
        /* Motif BC */ 
        XtGetValues((Widget)Sep(combo), args, 3); 
        if (full_available_height != tmp_height)
            XtResizeWidget(Sep(combo), tmp_width, full_available_height,
                           tmp_borderwidth);
    }
    else {
        /* Motif BC */
        XtGetValues((Widget)Text(combo), args, 3);
	if ( Width(((Widget)Text(combo))) != available_width) /** TextField **/
	XtResizeWidget(Text(combo), available_width,
		       tmp_height, tmp_borderwidth);
    }

    /* Motif BC */
    XtGetValues((Widget)Arrow(combo), args, 3);
    if ((arrow_width != tmp_width) || (tmp_height != available_height)) {
	available_height = (available_height < ARROW_MIN) ? ARROW_MIN : 
                            available_height;
	XtResizeWidget(Arrow(combo), arrow_width, available_height,
		       tmp_borderwidth);
    }

    /* Resolution Independent */
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
        if (Type(combo) == DtDROP_DOWN_LIST) {
		XtSetValues(Sep(combo), args, 1);
		XtSetValues(Label(combo), args, 1);
	}
	else
		XtSetValues(Text(combo), args, 1);
	XtSetValues(Arrow(combo), args, 1);
    }
}

/*
 * This function positions the children within the combo_box widget.
 * It calls ForceChildSizes() to make sure the children fit within the
 * combo_box widget, but it will not try to resize the combo_box widget.
 */
static void
LayoutChildren(DtComboBoxWidget combo)
{
    Widget text_holder = ((Type(combo) == DtDROP_DOWN_COMBO_BOX)
			  ? Text(combo) : Label(combo));
    Position start_x = (COMBO_SHADOW(combo) + COMBO_MARGIN_W(combo) +
			COMBO_H_SPACING(combo));
    Position start_y = (COMBO_SHADOW(combo) + COMBO_MARGIN_H(combo) +
			COMBO_V_SPACING(combo));
    short available_height = Height(combo) - (start_y * 2);
    Position y, arrow_y;
    unsigned char unit_type = XmPIXELS;
    Dimension tmp_width, tmp_height, sep_width;
    Arg args[3];
    
    ForceChildSizes(combo);

    /* Resolution Independent */
    if (MUnitType(combo) != XmPIXELS) {
	unit_type = MUnitType(combo);
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(text_holder, args, 1);
	XtSetValues(Arrow(combo), args, 1);
        if (Type(combo) == DtDROP_DOWN_LIST) 
		XtSetValues(Sep(combo), args, 1);
    }
    /* Center text_holder within combo_box */
    /* MotifBc */
    XtSetArg(args[0], XmNheight, &tmp_height);
    XtGetValues(text_holder, args, 1);
    y = available_height - tmp_height;
    y = ((y < 0) ? 0 : y)/2 + start_y;
 
    /* Center arrow within combo_box */
    /* MotifBc */
    XtSetArg(args[1], XmNwidth, &tmp_width);
    XtGetValues(Arrow(combo), args, 2);
    arrow_y = available_height - tmp_height;
    arrow_y = ((arrow_y < 0) ? 0 : arrow_y)/2 + start_y;

    /* MotifBc */
    if (Type(combo) == DtDROP_DOWN_LIST) {
    	XtSetArg(args[0], XmNwidth, &sep_width);
    	XtGetValues(Sep(combo), args, 1);
    }
    if (Orientation(combo) == DtLEFT) {
	XtMoveWidget(Arrow(combo), start_x, arrow_y);
	start_x += tmp_width;
	if (Type(combo) == DtDROP_DOWN_LIST) {
	    XtMoveWidget(Sep(combo), start_x, start_y - 
			 COMBO_V_SPACING(combo));
	    start_x += sep_width;
	}
	start_x += ArrowSpacing(combo);
	XtMoveWidget(text_holder, start_x, y);
    }
    else {
	XtMoveWidget(text_holder, start_x, y);
	/*  
	 * We want the arrow at the end of the combo_box, so
	 * the user can use recompute_size more effectively.
	 */
	start_x = Width(combo) - start_x - tmp_width;
	if (Type(combo) == DtDROP_DOWN_LIST) {
	    start_x -= sep_width;
	    XtMoveWidget(Sep(combo), start_x, start_y -
			 COMBO_V_SPACING(combo));
	    start_x += sep_width;
	}
	XtMoveWidget(Arrow(combo), start_x, arrow_y);
    }

    /* Resolution Independent */
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(text_holder, args, 1);
	XtSetValues(Arrow(combo), args, 1);
        if (Type(combo) == DtDROP_DOWN_LIST) 
		XtSetValues(Sep(combo), args, 1);
    }
}

/*
 * SetValues() routine for ComboBox widget. 
 */
static Boolean
SetValues(	DtComboBoxWidget current,
		DtComboBoxWidget request,
		DtComboBoxWidget new)
{
    DtComboBoxPart *new_p = (DtComboBoxPart*)
	&(XmField(new,ipot,DtComboBox,arrow,Widget));
    DtComboBoxPart *cur_p = (DtComboBoxPart*)
	&(XmField(current,ipot,DtComboBox,arrow,Widget));
    Boolean label_size_changed = FALSE;
    Boolean force_label_string = FALSE;
    Arg args[10];
    int n;
    unsigned char new_unit_type = XmPIXELS, curr_unit_type = XmPIXELS;

    CheckResources(new);

    /* Resolution Independent */
    if (MUnitType(new) != XmPIXELS) {
	new_unit_type = MUnitType(new);
        XtSetArg(args[0], XmNunitType, XmPIXELS);
	if (Arrow(new)) XtSetValues(Arrow(new), args, 1);
	if (List(new)) XtSetValues(List(new), args, 1);
	if (Shell(new)) XtSetValues(Shell(new), args, 1);
	if (Label(new)) XtSetValues(Label(new), args, 1);
	if (Text(new)) XtSetValues(Text(new), args, 1);
	if (Sep(new)) XtSetValues(Sep(new), args, 1);
    }
    if (MUnitType(current) != XmPIXELS) {
	curr_unit_type = MUnitType(current);
        XtSetArg(args[0], XmNunitType, XmPIXELS);
	if (Arrow(current)) XtSetValues(Arrow(current), args, 1);
	if (List(current)) XtSetValues(List(current), args, 1);
	if (Shell(current)) XtSetValues(Shell(current), args, 1);
	if (Label(current)) XtSetValues(Label(current), args, 1);
	if (Text(current)) XtSetValues(Text(current), args, 1);
	if (Sep(current)) XtSetValues(Sep(current), args, 1);
    }

    if (Text(new) != Text(current)) {
	XtWarning(CB_TEXT);
	Text(new) = Text(current);
    }

    /*
     * Pass any list specific resources on to our List Widget.
     * Check each one, since it's too costly to always set them.
     */
    n = 0;
    if (ItemCount(new) != ItemCount(current)){
	if (Items(new) && (ItemCount(new) < 0)) {
	    XtWarning(CB_ITEM_COUNT);
	    ItemCount(new) = 0;
	}
	XtSetArg(args[n], XmNitemCount, ItemCount(new)); n++;
    }
    if (Items(new) != ListItems(current)) {
	XtSetArg(args[n], XmNitems, Items(new)); n++;
	/* Make sure itemCount will get sent to list */
	if (ItemCount(new) == ItemCount(current)) {
	    XtSetArg(args[n], XmNitemCount, ItemCount(new)); n++;
	}
    }
    if (ListFontList(new) != ListFontList(current)) {
	XtSetArg(args[n], XmNfontList, ListFontList(new)); n++;
    }
    if (ListMarginHeight(new) != ListMarginHeight(current)) {
	XtSetArg(args[n], XmNlistMarginHeight, ListMarginHeight(new)); n++; 
    }
    if (ListMarginWidth(new) != ListMarginWidth(current)) {
	XtSetArg(args[n], XmNlistMarginWidth, ListMarginWidth(new)); n++;
    }
    if (ListSpacing(new) != ListSpacing(current)) {
	XtSetArg(args[n], XmNlistSpacing, ListSpacing(new)); n++;
    }
    if (LayoutDirection(new) != LayoutDirection(current)) {
        XmStringDirection new_direction =
	  XmDirectionToStringDirection(LayoutDirection(new));
	XtSetArg(args[n], XmNstringDirection, new_direction); n++;
    }
    if (TopItemPosition(new) != TopItemPosition(current)) {
	XtSetArg(args[n], XmNtopItemPosition, TopItemPosition(new)); n++;
    }
    if (VisibleItemCount(new) != VisibleItemCount(current)) {
	XtSetArg(args[n], XmNvisibleItemCount, VisibleItemCount(new)); n++;
    }
    if (n > 0) {
	/* MotifBc */
	Arg tmp_arg[2];
	Dimension tmp_width, tmp_height;

	XtSetValues(List(new), args, n);
	/* MotifBc */
	XtSetArg(tmp_arg[0], XmNwidth, &tmp_width);
        XtSetArg(tmp_arg[1], XmNheight, &tmp_height);
	XtGetValues(((Widget) Shell(new)), tmp_arg, 2);
	MaxShellWidth(new) = tmp_width;
	MaxShellHeight(new) = tmp_height;
    }

    /* If arrow type changes delete the old one and create the new one */
    if (ArrowType(new) != ArrowType(current)) {
	XtRemoveCallback(Arrow(new), XmNactivateCallback, activate_cb, 
			 (XtPointer)new);
	if (ArrowType(current) == DtWINDOWS)
	    XtRemoveCallback(Arrow(new), XmNexposeCallback,
			     arrow_expose_cb, (XtPointer)new);
	XtDestroyWidget(Arrow(new));

	n = 0;
	XtSetArg(args[n], XmNtraversalOn, FALSE); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	XtSetArg(args[n], XmNshadowThickness, 0); n++;
	if (ArrowType(new) == DtMOTIF) {
	    XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN); n++;
	    XtSetArg(args[n], XmNforeground, BackgroundPixel(new)); n++;
	    Arrow(new) = XtCreateManagedWidget("ComboBoxArrow", 
						 xmArrowButtonWidgetClass,
						 (Widget)new, args, n);
	}
	else {
	    Arrow(new) = XtCreateManagedWidget("ComboBoxArrow", 
						 xmDrawnButtonWidgetClass,
						 (Widget)new, args, n);
	    XtAddCallback(Arrow(new), XmNexposeCallback, arrow_expose_cb, 
			  (XtPointer)new);
	}
	XtAddCallback(Arrow(new), XmNactivateCallback, activate_cb, 
		      (XtPointer)new);
    }

    /*
     * Type resource changed.  If the widget (textField or Label)
     * doesn't exist, then create it.  Always reset orientation
     * constraint resources when type changes; otherwise, the
     * text_holder widget positioning could be screwed up.  We don't
     * reset both widgets if the orientation changes (because we might
     * not have created both widgets).
     * If label must be created, also create the separator widget.
     */
    if (Type(new) != Type(current)) {
	if (Type(new) == DtDROP_DOWN_COMBO_BOX) {
	    if (Text(new) == NULL) {
		n = 0;
		XtSetArg(args[n], XmNcolumns, TextColumns(new)); n++;
		XtSetArg(args[n], XmNmaxLength, TextMaxLength(new)); n++;
		XtSetArg(args[n], XmNmarginWidth, 2); n++;
		XtSetArg(args[n], XmNmarginHeight, 2); n++;
		Text(new) = XtCreateWidget("ComboBoxTextField", 
					     xmTextFieldWidgetClass,
					     (Widget)new, args, n);
		XtAddCallback(Text(new), XmNlosingFocusCallback, 
			      text_losing_focus_cb, (XtPointer)new);
		XtAddCallback(Text(new), XmNactivateCallback, 
			      text_activate_cb, (XtPointer)new);
		XtAddCallback(Text(new), XmNfocusCallback, 
			      text_focus_cb, (XtPointer)new);
		if (HorizontalSpacing(new) == HorizontalSpacing(current))
		    HorizontalSpacing(new) = 0;
		if (VerticalSpacing(new) == VerticalSpacing(current))
		    VerticalSpacing(new) = 0;
	    }
	    XtUnmanageChild(Sep(new));
	    XtUnmanageChild(Label(new));
	    XtManageChild(Text(new));
	}
	else {
	    if (Label(new) == NULL) {
	        XmStringDirection new_direction =
		  XmDirectionToStringDirection(LayoutDirection(new));

		XtTranslations label_trans = 
		    XtParseTranslationTable(ComboBoxLabelTranslationTable);

		n = 0;
		XtSetArg(args[n], XmNalignment, Alignment(new)); n++;
		XtSetArg(args[n], XmNrecomputeSize, FALSE); n++;
		XtSetArg(args[n], XmNmarginLeft, LABEL_PADDING); n++;
		XtSetArg(args[n], XmNmarginRight, LABEL_PADDING); n++;
	        XtSetArg(args[n], XmNmarginWidth, TEXT_CONTEXT_MARGIN); n++;
		XtSetArg(args[n], XmNmarginHeight, 0); n++;
		XtSetArg(args[n], XmNstringDirection, new_direction); n++;
		Label(new) = XtCreateWidget("ComboBoxLabel", 
					      xmLabelWidgetClass,
					      (Widget)new, args, n);
		XtOverrideTranslations((Widget)Label(new), label_trans);
		if (HorizontalSpacing(new) == HorizontalSpacing(current))
		    HorizontalSpacing(new) = 1;
		if (VerticalSpacing(new) == VerticalSpacing(current))
		    VerticalSpacing(new) = 2;

		n = 0;
		XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
		Sep(new) = XtCreateWidget("ComboBoxSeparator", 
					    xmSeparatorWidgetClass,
					    (Widget)new, args, n);
	    }
	    else if (LayoutDirection(new) != LayoutDirection(current)) {
	        XmStringDirection new_direction =
		  XmDirectionToStringDirection(LayoutDirection(new));
		XtSetArg(args[0], XmNstringDirection, new_direction);
		XtSetValues(Label(new), args, 1);
	    }
	    XtUnmanageChild(Text(new));
	    XtManageChild(Label(new));
	    XtManageChild(Sep(new));
	}
	/* 
	 * Text-fields and labels have different shadows.  Only
	 * change if user didn't change the shadow resource.
	 */
	if (COMBO_SHADOW(new) == COMBO_SHADOW(current))
	    COMBO_SHADOW(new) = ((Type(new) == DtDROP_DOWN_COMBO_BOX) ?
				 TEXT_FIELD_SHADOW : LABEL_SHADOW);
    }

    if (Text(new) && (Text(new) == Text(current))) {
	n = 0;
	if (TextColumns(new) != TextColumns(current)) {
	    XtSetArg(args[n], XmNcolumns, TextColumns(new)); n++;
	}
	if (TextMaxLength(new) != TextMaxLength(current) ) {
	    XtSetArg(args[n], XmNmaxLength, TextMaxLength(new)); n++;
	}
	if (n > 0)
	    XtSetValues(Text(new), args, n);
    }

    /*
     * LabelWidget alignment has changed.
     */
    if (Label(new) && (Alignment(new) != Alignment(current))) {
	XtSetArg(args[0], XmNalignment, Alignment(new));
	XtSetValues(Label(new), args, 1);
    }

    if (Label(new) && ((Items(new) != ListItems(current)) || 
			 (ItemCount(new) != ItemCount(current)) ||
			 (Label(new) != Label(current)))) {
	SetMaximumLabelSize(new_p);
	label_size_changed = TRUE;
    }

    /* Copy and free label-string */
    if (LabelString(new) != LabelString(current)) {
	if (LabelString(new))
	    LabelString(new) = XmStringCopy(LabelString(new));
	if (LabelString(current))
	    XmStringFree(LabelString(current));	
	/* 
	 * force_label_string usage if it is specified and items is not.
	 * This will be the perminant label string only if update-label
	 * is false, else it is only used until the user picks something
	 * new off the list.
	 */
	if (Items(new) == ListItems(current))
	    force_label_string = TRUE;
    }

    if ((Items(new) != ListItems(current)) ||
	(Alignment(new) != Alignment(current)) ||
	(Type(new) != Type(current)) ||
	(ItemCount(new) != ItemCount(current)) ||
	(SelectedPosition(new) != SelectedPosition(current)) ||
	(XmStringByteCompare(SelectedItem(new), SelectedItem(current))!=True) ||
	(Label(new) != Label(current)) ||
	(UpdateLabel(new) != UpdateLabel(current)) ||
	(LabelString(new) != LabelString(current))) {

	/* selected_item resource used before selected_position */
	if (SelectedItem(new) && XmeStringIsValid(SelectedItem(new)) &&
	    (XmStringByteCompare(SelectedItem(new), 
				 SelectedItem(current))!=True) ){
	    if (SelectedItem(current)) {
		XmStringFree(SelectedItem(current));
		SelectedItem(current) = NULL;
	    }
	    SelectedItem(new) = XmStringCopy(SelectedItem(new));
	    DtComboBoxSelectItem((Widget)new, SelectedItem(new));
	}
	else {
	    if (SelectedPosition(new)<0||SelectedPosition(new)>=ItemCount(new))
		SelectedPosition(new) = 0;
	    if ((ItemCount(new)) && 
		(SelectedPosition(new) != SelectedPosition(current))) {
		
	        XmListSelectPos(List(new), SelectedPosition(new) + 1, FALSE);
	        if (SelectedItem(current)) {
		    
		    XmStringFree(SelectedItem(current));
		    SelectedItem(current) = NULL;
		}
		SyncWithList(new_p);
		SelectedItem(new) = 
		    XmStringCopy((ListItems(new))[SelectedPosition(new)]);
	    }

	    /* Else, same item selected as last time */
	    else {
		SelectedItem(new) = XmStringCopy(SelectedItem(new));
		if ((SelectedItem(new) != NULL) && (ListItems(new) != NULL))
		    DtComboBoxSelectItem((Widget)new, SelectedItem(new));
	        if (SelectedItem(current)) {
		    XmStringFree(SelectedItem(current));
		    SelectedItem(current) = NULL;
		}
	    }
	}

	if (Type(new) == DtDROP_DOWN_COMBO_BOX)
	    SetTextFieldData(new_p, NULL);
	else
	    SetLabelData(new_p, NULL, force_label_string);
    }

    /* Else, same item selected as last time */
    else {
	SelectedItem(new) = XmStringCopy(SelectedItem(new));
	if ((SelectedItem(new) != NULL) && (ListItems(new) != NULL))
	    DtComboBoxSelectItem((Widget)new, SelectedItem(new));
	if (SelectedItem(current)) {
	    XmStringFree(SelectedItem(current));
	    SelectedItem(current) = NULL;
	}
    }

    /*
     * Must recalculate the combo_box and re-layout the children.
     * If this is not editable, then set the label to its' maximum
     * size; it will get chopped if it is too big.  This is needed 
     * because we shrink the label down, and SetComboBoxSize() uses
     * the label's core sizes to figure what size to become.
     */
    if ((Type(new) != Type(current)) ||
	(ArrowType(new) != ArrowType(current)) ||
	(COMBO_MARGIN_W(new) != COMBO_MARGIN_W(current)) ||
	(COMBO_MARGIN_H(new) != COMBO_MARGIN_H(current)) ||
	(COMBO_H_SPACING(new) != COMBO_H_SPACING(current)) ||
	(COMBO_V_SPACING(new) != COMBO_V_SPACING(current)) ||
	(COMBO_SHADOW(new) != COMBO_SHADOW(current)) ||
	(Orientation(new) != Orientation(current)) ||
	(ArrowSpacing(new) != ArrowSpacing(current)) ||
	((Type(new) == DtDROP_DOWN_LIST) && label_size_changed)) {
	ClearShadow(current, TRUE);
	if (RecomputeSize(new))
	    SetComboBoxSize(new);
	LayoutChildren(new);
	DrawShadow(new);
    }

    SyncWithList(new_p);
    /* Resolution Independent */
    if (new_unit_type != XmPIXELS) {
        XtSetArg(args[0], XmNunitType, new_unit_type);
	if (Arrow(new)) XtSetValues(Arrow(new), args, 1);
	if (List(new)) XtSetValues(List(new), args, 1);
	if (Shell(new)) XtSetValues(Shell(new), args, 1);
	if (Label(new)) XtSetValues(Label(new), args, 1);
	if (Text(new)) XtSetValues(Text(new), args, 1);
	if (Sep(new)) XtSetValues(Sep(new), args, 1);
    }
    if (curr_unit_type != XmPIXELS) {
        XtSetArg(args[0], XmNunitType, curr_unit_type);
	if (Arrow(current)) XtSetValues(Arrow(current), args, 1);
	if (List(current)) XtSetValues(List(current), args, 1);
	if (Shell(current)) XtSetValues(Shell(current), args, 1);
	if (Label(current)) XtSetValues(Label(current), args, 1);
	if (Text(current)) XtSetValues(Text(current), args, 1);
	if (Sep(current)) XtSetValues(Sep(current), args, 1);
    }
    return(FALSE);
}


/*
 * This function clears the shadow around our widget.  If all is TRUE,
 * then clear all 4 sides; otherwise, only clear the right and bottom
 * sides (during resize). 
 */ 
static void
ClearShadow(	DtComboBoxWidget w,
		Boolean all)
{
    Dimension shadow = COMBO_SHADOW(w);
    Dimension margin_w = COMBO_MARGIN_W(w);
    Dimension margin_h = COMBO_MARGIN_H(w);

    if ((shadow > 0) && XtIsRealized((Widget)w)) {
	if (all) {
	    XClearArea(XtDisplayOfObject((Widget)w),
		       XtWindowOfObject((Widget)w), 
		       margin_w, margin_h,
		       OldWidth(w) - (margin_w * 2),
		       shadow, FALSE);
	    XClearArea(XtDisplayOfObject((Widget)w),
		       XtWindowOfObject((Widget)w), 
		       margin_w, margin_h, shadow, 
		       OldHeight(w) - (margin_h * 2), FALSE);
	}
	XClearArea(XtDisplayOfObject((Widget)w), 
		   XtWindowOfObject((Widget)w), margin_w,
		   OldHeight(w) - margin_h - shadow,
		   OldWidth(w) - (margin_w * 2), shadow, FALSE);
	XClearArea(XtDisplayOfObject((Widget)w), XtWindowOfObject((Widget)w),
		   OldWidth(w) - margin_w - shadow,
		   margin_h, shadow, 
		   OldHeight(w) - (margin_h * 2), FALSE);
    }
    DrawHighlight(w, TRUE);
}

/* 
 * This functions draws the shadow around our combo-box.
 */
static void
DrawShadow(DtComboBoxWidget w)
{
    Dimension shadow = COMBO_SHADOW(w);
    Dimension margin_w = COMBO_MARGIN_W(w);
    Dimension margin_h = COMBO_MARGIN_H(w);
    
    if ((shadow > 0) && XtIsRealized((Widget)w)) {
	XmeDrawShadows(XtDisplayOfObject((Widget)w),
		       XtWindowOfObject((Widget)w),
		       TopShadowGC(w),
		       BottomShadowGC(w), 
		       margin_w, margin_h,
		       Width(w) - (margin_w * 2),
		       Height(w) - (margin_h * 2),
		       shadow, XmSHADOW_OUT);
    }
    DrawHighlight(w, FALSE);
}

/*
 * Take the string out of the list and put it into the text-field.
 * text-fields don't handle xm-strings, so we must get the char*
 * out of it (only getting the first segment).  This is slower than
 * storing the text-strings (char*) ourselves, but that would take
 * up a lot of memory.  Since this setting happens during a user
 * action, speed isn't a problem.
 */
static void
SetTextFieldData(DtComboBoxPart *combo_p, XmString item)
{
    XmListWidget list = (XmListWidget)combo_p->list;
    XmStringContext context;
    Boolean done = FALSE;
    Arg args[2];
    XmStringComponentType type;
    char *text;
    XmStringCharSet charset;
    XmStringDirection direction;
    XmStringComponentType unknown_tag;
    unsigned short ul;
    unsigned char *uv;
    XmStringTable list_items;
    int item_count;
    Boolean isItemCopied = FALSE;
/* MotifBc 
 */
    XtSetArg(args[0], XmNitemCount, &item_count);
    XtSetArg(args[1], XmNitems, &list_items);
    XtGetValues((Widget)list, args, 2); 
    if (item_count && item_count>combo_p->selected_position && 
	!item && list_items) {
	item = XmStringCopy(list_items[combo_p->selected_position]);
	isItemCopied = TRUE;
    }

    if (!item) {
	combo_p->selected_item = NULL;
	XtSetArg(args[0], XmNvalue, "");
	XtSetValues(combo_p->text, args, 1);
    }
    else {
        if (combo_p->selected_item != item) 
	  {
	    if (combo_p->selected_item)
	      XmStringFree (combo_p->selected_item);
	    combo_p->selected_item = XmStringCopy(item);
	  }
	XmStringInitContext(&context, item);
	
	/* Loop until 1st char* found */
	while (!done) {
	    type = XmStringGetNextComponent(context, &text, &charset,
					    &direction, &unknown_tag, 
					    &ul, &uv);
	    switch (type) {
	    case XmSTRING_COMPONENT_END:
		done = TRUE;
		break;
	    case XmSTRING_COMPONENT_TEXT:
	    case XmSTRING_COMPONENT_LOCALE_TEXT:
		XtSetArg(args[0], XmNvalue, text);
		XtSetValues(combo_p->text, args, 1);
		XtFree(text);
		done = TRUE;
		break;
	    default:
		break;
	    }
	}
	XmStringFreeContext(context);
	if (isItemCopied)
	    XmStringFree(item);
    }
}


/*
 * Set the maximum size of the label, depending on the
 * characteristics of the list of items.
 */
static void
SetMaximumLabelSize(DtComboBoxPart *combo_p)
{
    XmListWidget list = (XmListWidget)combo_p->list;
    XmFontList font_list;
    Dimension width, height, border_width;
    Dimension longest = 0;
    Dimension highest = 0;
    Arg args[5];
    int i, item_count;
    XmStringTable list_items;
    unsigned char unit_type = XmPIXELS;

    /* Resolution Independent */
    XtSetArg(args[0], XmNunitType, &unit_type);
    XtGetValues(combo_p->list, args, 1); /* Assume list/Combo has same uniType*/
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(combo_p->label, args, 1);
	XtSetValues(combo_p->list, args, 1);
    }
	
    /* Get font info from the widget */
    XtSetArg(args[0], XmNfontList, &font_list);
    XtGetValues(combo_p->label, args, 1);
/* MotifBc
 */
    XtSetArg(args[0], XmNitems, &list_items);
    XtSetArg(args[1], XmNitemCount, &item_count);
    XtGetValues((Widget)list, args, 2);

    if ( item_count && item_count >= combo_p->item_count &&
	 list_items && combo_p->update_label) {
	/*
	 * Loop through all the items to find the biggest dimensions
	 */
	for (i = 0; i < combo_p->item_count; i++) {
	    XmStringExtent(font_list, list_items[i], &width, &height);
	    longest = (width > longest) ? width : longest;
	    highest = (height > highest) ? height : highest;
	 }
    }
    else {
	XmStringExtent(font_list, combo_p->label_string, &longest, &highest);
    }
	
    combo_p->label_max_length = longest + ((LABEL_PADDING+TEXT_CONTEXT_MARGIN) * 2);
    combo_p->label_max_height = highest;
    /* MotifBc */
    XtSetArg(args[0], XmNborderWidth, &border_width);
    XtGetValues(combo_p->label, args, 1);
    XtResizeWidget(combo_p->label, combo_p->label_max_length, highest, 
		   border_width);

    /* Resolution Independent */
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(combo_p->label, args, 1);
	XtSetValues(combo_p->list, args, 1);
    }
}


/*
 * Put the current list item into the label.
 * This could probably be faster if we see if the label is the
 * same as the new item?
 */
static void
SetLabelData(	DtComboBoxPart *combo_p,
		XmString item,
		Boolean force_label_string)
{
    XmListWidget list = (XmListWidget)combo_p->list;
    int index = combo_p->selected_position;
    Arg args[2];
    XmStringTable list_items;
    int item_count;

    /*
     * If the item is empty, get the current item from the list, or
     * use label_string if update_label is FALSE.  If that is empty, 
     * use InitLabel.
     */
    if (force_label_string || (combo_p->update_label == FALSE))
	item = combo_p->label_string ? combo_p->label_string : InitLabel;
    else {
	if (!item) {
/* MotifBc
 */
	    XtSetArg(args[0], XmNitemCount, &item_count);
	    XtSetArg(args[1], XmNitems, &list_items);
	    XtGetValues((Widget)list, args, 2);
	    if (item_count && item_count>index && list_items)
		item = list_items[index];
	    else
		item = InitLabel;
	}
	
	/* Keep label_string in sync with item picked */
	if (combo_p->label_string)
	    XmStringFree(combo_p->label_string);
	combo_p->label_string = XmStringCopy(item);
    }

    combo_p->selected_item = XmStringCopy(item);
    XtSetArg(args[0], XmNlabelString, item);
    XtSetValues(combo_p->label, args, 1);
}

/*
 * This is the browseSelect and defaultAction callback handler for the
 * ListWidget.  If using the textWidget, we only take the first 
 * segment of the XmString (TextWidgets don't handle XmStrings).  If we
 * are using a label, then just set the labelString resource.
 */
static void
select_cb(	Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    DtComboBoxWidget combo_w = (DtComboBoxWidget)client_data;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo_w,ipot,DtComboBox,arrow,Widget));
    XmListCallbackStruct *info = (XmListCallbackStruct*)call_data;
    DtComboBoxCallbackStruct cb;
    
    SelectedPosition(combo_w) = info->item_position - 1;
    if (SelectedItem(combo_w))
	XmStringFree(SelectedItem(combo_w));
    SelectedItem(combo_w) = XmStringCopy(info->item);
    if (Type(combo_w) == DtDROP_DOWN_COMBO_BOX) {
	SetTextFieldData(combo_p, SelectedItem(combo_w));
    }
    else {	/* Set the labelWidget string */
	SetLabelData(combo_p, SelectedItem(combo_w), FALSE);
    }
    
    /*
     * Only popdown if this is the defaultAction callback.  We don't
     * want to popdown with browseSelect callback; that would cause the
     * menu to popdown when the user moved selection with the keyboard.
     * Doing it this way, allows the menu to stay up during 
     * keyboard navigation.  When menu goes away, make sure input
     * focus goes back into the textField (if editable).
     */
    /* This is one of three places to popdown the menu and here is 
     * to pop it down by Keyboard input: KSelect
     * Another place using keyboard input to popdown the menu is 
     * in _ComboBoxKbdCancel() by <Key>osfEsc
     * the last place is in shell_event_handler and that is for
     * poping down through Button operation: BSelect
     */
    if (info->reason == XmCR_DEFAULT_ACTION) 
        input_ungrab( combo_w, GRAB_POINTER | GRAB_KEYBOARD );

    if ( ShellPoppedUp(((ShellWidget)Shell(combo_w))) == FALSE) {
	/* The list will free info->item */
	/* The semantic of a DtNselectionCallback is:
	 * a callback to here + ShellPoppedUp==False
	 * In reality, there are two scenario for this:
	 *     1. XmList's XmNdefaultActionCallback which is activated by
	 *        KSelect, will first pop down the shell (see above) and
	 *        and then get here.
	 *     2. BSelect on XmList, which first call the list_event_handler()
	 *        (for the handler is registered as LIST_HEAD) to pop down
	 *        the shell, and then call XmList's XmNbrowseSelectionCallback
	 *	  which in turn get here
	 */
	cb.reason = DtCR_SELECT;
	cb.event = info->event;
	cb.item_or_text = XmStringCopy(info->item);
	cb.item_position = SelectedPosition(combo_w);
	XtCallCallbackList((Widget)combo_w, SelectionCallback(combo_w), 
			   (XtPointer)&cb);
	if (cb.item_or_text != NULL) {
	    XmStringFree(cb.item_or_text);
	    cb.item_or_text = NULL;
	}
    }
}


/*
 * This is the event_handler for our shell widget.  The grab happens
 * on the shell while the user is not doing anything inside the list.
 * This allows us to know if the user pressed a button outside our
 * application.  If the user pressed a button anywhere but inside
 * the shell, then popdown the menu and ungrab everything.
 */

static void
shell_event_handler( 	Widget widget,
			XtPointer client_data,
			XEvent* event,
			Boolean *dispatch)
{
    DtComboBoxWidget combo_w = (DtComboBoxWidget)client_data;
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget)XtParent(List(combo_w));
    XmScrollBarWidget v_scrollbar;
    XmScrollBarWidget h_scrollbar;
    Window window = event->xbutton.window;
    Arg args[2];

    _DtProcessLock();
    if (popup_shell_init) {
	if (popup_shell_init != DtKeyPressPopup) {
	    /* Menu popped up by ButtonPress */
	    popup_shell_init = DtNonePopup;
	    _DtProcessUnlock();
	    return;
	}
	else
	    /* Menu popped up by KActivate */
	    popup_shell_init = DtNonePopup;
    }
    _DtProcessUnlock();

    /* MotifBc */
    XtSetArg(args[0], XmNverticalScrollBar, &v_scrollbar);
    XtSetArg(args[1], XmNhorizontalScrollBar, &h_scrollbar);
    XtGetValues((Widget)sw, args, 2);

    /* condition : Shell is popped up;
     *             ButtonPress event;
     *             the cursor is Not inside the XmList area,
     *             the horizontal scrollbar or the vertical one;
     * action:     remove all the grabs by DtComboBox
     */
    if ( ShellPoppedUp(((ShellWidget)Shell(combo_w))) &&
	 (event->type == ButtonPress &&
	  (window != XtWindowOfObject(List(combo_w))) &&
          !(ScrollBarVisible((Widget)h_scrollbar) && 
	    (window == XtWindowOfObject((Widget)h_scrollbar))) &&
          !(ScrollBarVisible((Widget)v_scrollbar) && 
	    (window == XtWindowOfObject((Widget)v_scrollbar))))
       )
        input_ungrab( combo_w, GRAB_POINTER | GRAB_KEYBOARD );

    /* condition : ButtonRelease event
     *             the cursor is outside the scrollbar(horizontal or vertical)
     * action:     call action routine "Release" of ScrollBar to release
     *             possible pressed slider arrow (up/down)
     */
    if (event->type == ButtonRelease) {
        if ( ScrollBarVisible((Widget)h_scrollbar) &&
             window != XtWindowOfObject((Widget)h_scrollbar) )
            XtCallActionProc((Widget)h_scrollbar, "Release", event,NULL,0);
        if ( ScrollBarVisible((Widget)v_scrollbar) &&
             window != XtWindowOfObject((Widget)v_scrollbar) )
            XtCallActionProc((Widget)v_scrollbar, "Release", event,NULL,0);
    }
}


/*
 * This is the event_handler for our list widget.  Getting the pointer
 * grabbing to work correctly was not very easy.  In order for everything
 * to work correctly, we only do grab-pointer, for the shell, while not
 * doing anything inside the list.  If doing something inside the list
 * we remove the grab on the shell.  This is the only way that the 
 * list will get the ButtonRelease if doing browse while outside the
 * list.  The toolkit automatically does a grab in between ButtonPress
 * and ButtonRelease; therefore, the shell ungrab can't be done inside
 * the ButtonPress event.
 */
static void
list_event_handler(	Widget widget,
			XtPointer client_data,
			XEvent* event,
			Boolean *dispatch)  /* call remaining event handlers */
{
    DtComboBoxWidget combo_w = (DtComboBoxWidget)client_data;
    Arg		args[5];
    int		num_args;
    int		top_item_position;
    int		visible_item_count;
    int		item_count;
    XmScrolledWindowWidget sw = (XmScrolledWindowWidget)XtParent(widget);
    XmScrollBarWidget v_scrollbar;
    XmScrollBarWidget h_scrollbar;

    switch (event->type) {
    case ButtonRelease:
        /*
         * Pop down the menu when Button is released inside the menu.
         */
	/* Doing the popdown here upon receiving ButtonRelease event, 
	 * rather than in shell_even_hanlder(), is
	 * essential to allow select_cb() to recognize that this 
	 * XmNbrowseSelectionCallback is the last one in this popup session
	 * and must treat it as a DtNselectionCallback
	 */
        input_ungrab( combo_w, GRAB_KEYBOARD );

        /* condition : ButtonRelease event
         *             cursor is outside the scrollbar(horizontal or vertical)
         * action:     call action routine "Release" of ScrollBar to release
         *             possible pressed slider arrow (up/down)
         */
        /* MotifBc */
        XtSetArg(args[0], XmNverticalScrollBar, &v_scrollbar);
        XtSetArg(args[1], XmNhorizontalScrollBar, &h_scrollbar);
        XtGetValues((Widget)sw, args, 2);
        if ( ScrollBarVisible((Widget)h_scrollbar) )
            XtCallActionProc((Widget)h_scrollbar, "Release", event,NULL,0);
        if ( ScrollBarVisible((Widget)v_scrollbar) )
            XtCallActionProc((Widget)v_scrollbar, "Release", event,NULL,0);

	break;
    case FocusOut:
	/*
	 * There is interaction conflict between the list and the
	 * scrollbars in terms of Focus.  We always want our list
	 * to have focus, so grab it back if we lose it.
	 */
	if ( ShellPoppedUp(((ShellWidget)(Shell(combo_w)))) ) {
	    _XmGrabKeyboard(widget, False, GrabModeAsync, GrabModeAsync, 
			    CurrentTime);
	    XtSetKeyboardFocus(List(combo_w), RevertToNone);
	}
	break;
    case EnterNotify:
	/* call ListBeginSelect() action directly to simulate Btn1Down*/
	{Display *disp = XtDisplay(List(combo_w));
	 Window  win   = XtWindow(List(combo_w));
	 Window  root;
	 Window  child;
	 XmListWidget  list;
	 int     root_x, root_y, win_x, win_y, item_pos;
	 unsigned int keys_buttons;
	 unsigned char unit_type = XmPIXELS;

	 /* Resolution Independent */
	 if ( MUnitType(combo_w) != XmPIXELS) {
		unit_type = MUnitType(combo_w);
		XtSetArg(args[0], XmNunitType, XmPIXELS);
		XtSetValues(List(combo_w), args, 1);
	 }
	 root  = XDefaultRootWindow( disp );
	 XQueryPointer ( disp, win, &root,
					&child, &root_x, &root_y,
					&win_x, &win_y, &keys_buttons );
	 if ( keys_buttons & Button1Mask ) {
		item_pos = XmListYToPos(List(combo_w), event->xbutton.y);
		list = (XmListWidget)List(combo_w);
/* MotifBc
 */
		num_args = 0;
		XtSetArg(args[num_args],XmNtopItemPosition,&top_item_position);
		num_args ++;
		XtSetArg(args[num_args], XmNvisibleItemCount, 
			&visible_item_count);
		num_args ++;
		XtSetArg(args[num_args], XmNitemCount, &item_count);
		num_args ++;
		XtGetValues((Widget)list, args, num_args);
                /* Adjust top_item_position value to its' internal form */
                if (top_item_position)
                       top_item_position--;
                else
                       top_item_position = item_count - 1;

		if((item_pos<1 ) ||
		   (item_pos>=(top_item_position + visible_item_count))||
		   (item_pos < top_item_position) ||
		   (item_pos >= item_count) )
		{
			Position item_x, item_y;
			Dimension item_height, item_width;
			
			XmListPosToBounds((Widget)list, 
				((top_item_position + visible_item_count )> 
				 item_count)? item_count:
				 (top_item_position + visible_item_count ),
				&item_x, &item_y, &item_width, &item_height);

			if ( item_pos ==0 && event->xbutton.y<(int)(item_height/2) )
				event->xbutton.y = item_height/2;
			else
				event->xbutton.y = item_y + item_height/2;
			event->xbutton.x = item_x;
		}
		
		XtCallActionProc(List(combo_w), "ListBeginSelect",
						/* XEvent */ event,
						/* params */NULL,
						/* num_params */0);
	 }
	 /* Resolution Independent */
	 if ( unit_type != XmPIXELS) {
		XtSetArg(args[0], XmNunitType, unit_type);
		XtSetValues(List(combo_w), args, 1);
	 }
	}
	break;
    }
}


/* Caller must free string */
static char*
GetTextString(XmString xm_string)
{
    XmStringContext context;
    XmStringComponentType type;
    XmStringCharSet charset;
    XmStringDirection direction;
    XmStringComponentType unknown_tag;
    unsigned short ul;
    unsigned char *uv;
    char *text = NULL;
    Boolean done = FALSE;

    XmStringInitContext(&context, xm_string);
    
    /* Loop until 1st char* found */
    while (!done) {
	type = XmStringGetNextComponent(context, &text, &charset,
					&direction, &unknown_tag, 
					&ul, &uv);
	switch (type) {
	case XmSTRING_COMPONENT_END:
	    done = TRUE;
	    break;
	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	    done = TRUE;
	    break;
	default:
	    break;
	}
    }
    XmStringFreeContext(context);
    return(text);
}

static void
TextFieldActivate(DtComboBoxPart *combo_p, XtPointer call_data)
{
    XmTextFieldWidget w = (XmTextFieldWidget)(combo_p->text);
    XmListWidget list = (XmListWidget)combo_p->list;
    XmAnyCallbackStruct cb;
    char *data = NULL;
    char *text = NULL;
    Arg arg;
    XmStringTable list_items;
    
    XtSetArg(arg, XmNvalue, &data);
    XtGetValues((Widget)w, &arg, 1);
/* MotifBc
 */
    XtSetArg(arg, XmNitems, &list_items);
    XtGetValues((Widget)list, &arg, 1);

    if ( list_items)
	text = GetTextString(list_items[combo_p->selected_position]);

    if (text && data && (strcmp(text, data) == 0)) {
	XtFree(text);
    }
    /* Only send callback if both are not NULL */
    else if (!((text == NULL) && (data == NULL))) {
	XmAnyCallbackStruct *local_cb = (XmAnyCallbackStruct *)call_data;

	cb.reason = XmCR_ACTIVATE;
	cb.event  = local_cb ? local_cb->event : NULL;
/* MotifBc
 */
        if (XtHasCallbacks((Widget)w, XmNactivateCallback)==XtCallbackHasSome)
               XtCallCallbacks((Widget)w, XmNactivateCallback, 
                               (XtPointer) &cb);
	if (text)   
	    XtFree(text);
    }

    if ( data )
	XtFree (data);
}

/*
 * This is the activate callback for the arrow button.  This 
 * sets the shell position and width, does the correct grabs, and
 * puts the shell on the screen.
 */
static void
activate_cb(	Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    DtComboBoxWidget combo_w = (DtComboBoxWidget)client_data;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo_w,ipot,DtComboBox,arrow,Widget));
    Display *disp = XtDisplayOfObject((Widget)combo_w);
    XmDisplay xmdisp = (XmDisplay) XmGetXmDisplay(disp);
    int screen;
    Dimension width, height;
    Dimension disp_width, disp_height;
    Position root_x, root_y, root_y0;
    Arg args[5];
    int n;
    XmStringTable list_items;
    unsigned char unit_type = XmPIXELS;

    /* Resolution Independent Handling */
    XtSetArg(args[0], XmNunitType, &unit_type);
    XtGetValues((Widget)combo_w, args, 1);
   /* Getting Focus */
   if ( !_XmFocusIsHere( (Widget)combo_w) )
    XmProcessTraversal((Widget)combo_w, 
			(XmTraversalDirection) XmTRAVERSE_CURRENT);

    if (Type(combo_w) == DtDROP_DOWN_COMBO_BOX)
	TextFieldActivate(combo_p, call_data);

    /*
     * Don't popup if no items in the list.
     */
/* MotifBc
 */
    XtSetArg(args[0], XmNitemCount, &n);
    XtGetValues(((Widget)List(combo_w)), args, 1);
    if ( n != ItemCount(combo_w) )
	SyncWithList(combo_p);
    if ( !n )
	return;

    screen = DefaultScreen(disp);
    disp_width = DisplayWidth(disp, screen);
    disp_height = DisplayHeight(disp, screen);

    /*
     * Call the menu-post callback if requested.  This allows the
     * user to change the items, instead of using the losing-focus callback.
     * If the user used the losing-focus callback to change the items, the
     * size of the list/shell will change while it is popped up.  We
     * could disallow SetValues while the menu is posted, but let's see
     * how things go first.
     */
    if (MenuPostCallback(combo_w)) {
	XmAnyCallbackStruct info;

	info.reason = DtCR_MENU_POST;
	info.event = (XEvent*)NULL;
	XtCallCallbackList((Widget)combo_w, MenuPostCallback(combo_w), 
			   (XtPointer)&info);
    }

    width = MaxShellWidth(combo_w);
    height = MaxShellHeight(combo_w);


    /* Get root coords of ComboBox */
    XtTranslateCoords((Widget)combo_w, X(combo_w), Y(combo_w),
		      &root_x, &root_y);
	root_y0 = root_y;

    /*
     * Make necessary adjustments for offset of our widget 
     * inside its' parent.  Calculate the width of the shell.
     * This must be done every time the shell gets popped up, because 
     * the x/y can change as well as the width (from list's visibleItemCount 
     * or geometry management changes).
     */
    root_x -= X(combo_w);
    root_y -= Y(combo_w);
    root_y += (Height(combo_w) - COMBO_MARGIN_H(combo_w));

    /*
     * Make sure the shell is at least as big as our combo-box, and
     * make sure it stays on the screen.
     */
	/* to enforce a scrollbar with Shell */
    if (width < Width(combo_w))
	{
		Dimension sb_width, spacing, list_width;
		Widget sb;
		n = 0;
		XtSetArg(args[n], XmNverticalScrollBar, &sb); n++;
		XtSetArg(args[n], XmNspacing, &spacing); n++;
		XtGetValues(XtParent(List(combo_w)), args, n);
		/* Resolution Independent Resource */
		if ( unit_type != XmPIXELS) {
			n = XmConvertUnits(List(combo_w), XmHORIZONTAL, 
				(int)unit_type, spacing, XmPIXELS);	
			spacing = n;
		}
		if (sb && XtIsWidget(sb) ) {
		    n = 0;
		    XtSetArg(args[n], XmNwidth, &sb_width); n++;
		    XtGetValues(sb, args, n);
		    /* Resolution Independent Resource */
                    if (unit_type != XmPIXELS) { 
                        n = XmConvertUnits(sb, XmHORIZONTAL, 
                                (int)unit_type, sb_width, XmPIXELS);
                        sb_width = n;
                    } 
		}
		else
			sb_width = 0;

		n = 0;
		width = Width(combo_w);
		list_width = ( ScrollBarVisible(sb) ?
                                (width - sb_width - spacing)
				:(width - spacing) );

		if (unit_type != XmPIXELS) {
                        n = XmConvertUnits(sb, XmHORIZONTAL,
                                XmPIXELS, list_width, (int)unit_type);
                        list_width = n; n=0;
                }
		XtSetArg(args[n], XmNwidth, list_width); n++;
		XtSetValues(List(combo_w), args, n);
	}
    if ((int)(root_x + width) > (int)disp_width)
	root_x = (disp_width - width);
	else if (root_x <=0)
	root_x = 0;
	else 
	  ;

    if ((int)(root_y + height) > (int)disp_height)
	root_y = root_y0 - Y(combo_w) - height;

    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(Shell(combo_w), args, 1);
    }
    n = 0;
    XtSetArg(args[n], XmNx, root_x); n++;
    XtSetArg(args[n], XmNy, root_y); n++;
    XtSetValues(Shell(combo_w), args, n);
    if (unit_type != XmPIXELS) {
        XtSetArg(args[0], XmNunitType, unit_type); 
        XtSetValues(Shell(combo_w), args, 1); 
    } 


    PoppedUp(combo_w) = TRUE;
    XtPopup(Shell(combo_w), XtGrabNone);

    /*
     * Set up the grab for the shell and list.  The shell gets the
     * pointer grab so that events will go into the list and scrollbars
     * correctly, but events outside the shell will go to the shell.
     * See shell and list event handlers for details about grabs.
     */
    _XmAddGrab(Shell(combo_w), True, True);
    _XmGrabPointer(Shell(combo_w), True, ButtonPressMask |ButtonReleaseMask,
			GrabModeAsync, GrabModeAsync, None, 
		   XmGetMenuCursor(disp), CurrentTime);
    _XmGrabKeyboard(List(combo_w), False, GrabModeAsync, GrabModeAsync, 
		   CurrentTime);
    xmdisp->display.userGrabbed = True;
	
    /*
     * Where to define the cursor for the list widget.  It would be
     * nice to do it in the list's realize function, but that's not
     * possible.  We can't use the ComboBox realize function, because
     * the list isn't realized at that point.  This is the simpliest
     * way to get this done.  This is needed to make sure the list has the
     * correct menu during browse scrolling, etc.
     */
    XDefineCursor(disp, XtWindowOfObject(Shell(combo_w)), 
		  XmGetMenuCursor(disp));
}


/*
 * Make sure arrow is symetrical. 45 degree angle.  I'm not sure how
 * inefficient get/releasing the GC every time is (they are cached by
 * the toolkit)?
 */
static void
arrow_expose_cb(Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    DtComboBoxWidget combo_w = (DtComboBoxWidget)client_data;
    Display *disp = XtDisplayOfObject(w);
    Window win = XtWindowOfObject(w);
    XGCValues values;
    short center_w;
    short center_h;
    XPoint points[10];
    short min;
    Arg args[3];
    GC gc;
    Dimension arrow_width, arrow_height;
    unsigned char unit_type = XmPIXELS;

    /* Resolution Independent */
    if ( MUnitType(combo_w) != XmPIXELS ) {
	unit_type = MUnitType(combo_w);
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(w, args, 1);
    }
    /* MotifBc */
    XtSetArg(args[0], XmNwidth, &arrow_width);
    XtSetArg(args[1], XmNheight, &arrow_height);
    XtSetArg(args[2], XmNforeground, &(values.foreground));
    XtGetValues(w, args, 3);
    center_w = arrow_width/2;
    center_h = (int) (arrow_height -3.)/2;

    values.line_width = 0;
    values.line_style = LineSolid;
    values.fill_style = FillSolid;
    gc = XtGetGC(w, GCForeground | GCFillStyle | GCLineStyle | GCLineWidth,
		 &values);

    XDrawLine(disp, win, gc, 1, center_h + center_w + 1, Width(w) - 2,
	      center_h + center_w + 1);

    /* A - bottom point */
    points[0].x = center_w;
    points[0].y = center_h + (int)(center_w * .8);

    /* B - far left point */
    points[1].x = center_w - (int)(center_w * .8);
    points[1].y = center_h;

    /* C  - inner left point */
    points[2].x = center_w - (int)(center_w * .3);
    points[2].y = points[1].y;

    /* D - top left point */
    points[3].x = points[2].x;
    points[3].y = center_h - (int)(center_w * .8);

    /* E - top right point */
    points[4].x = center_w + (int)(center_w * .3);
    points[4].y = points[3].y;

    /* F - inner right point */
    points[5].x = points[4].x;
    points[5].y = points[1].y;

    /* G - far right point */
    points[6].x = center_w + (int)(center_w * .8);
    points[6].y = points[1].y;

    /* A - bottom point */
    points[7].x = points[0].x;
    points[7].y = points[0].y;

    XDrawLines(disp, win, gc, points, 8, CoordModeOrigin);
    XFillPolygon(disp, win, gc, points, 8, Convex, CoordModeOrigin);
    XtReleaseGC(w, gc);

    /* Resolution Independent */
    if ( unit_type != XmPIXELS ) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(w, args, 1);
    }
}

/*
 * We get the text-field losing-focus callback, so pass it on to
 * the user if they requested it.  Our losing-focus callback 
 * is just a convenience callback, so that the user doesn't
 * have to get the text-field first.  This make our integration
 * with XDesigner a little easier.
 */
static void
text_losing_focus_cb(	Widget w,
			XtPointer client_data,
			XtPointer call_data)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)client_data;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo,ipot,DtComboBox,arrow,Widget));
    XmTextFieldWidget wt=(XmTextFieldWidget) Text(combo);
    XmString xmStr;
    Arg arg;
    String text_string;
    int i, need_sync_flag=1;
/* MotifBc
 */
    XtSetArg(arg, XmNvalue, &text_string);
    XtGetValues((Widget)wt, &arg, 1);
    xmStr = (XmString) XmStringCreateLocalized(text_string?text_string:"");
    if ( text_string )
	XtFree (text_string);

    if (LosingFocusCallback(combo))
	XtCallCallbackList((Widget)combo, LosingFocusCallback(combo), 
			   (XtPointer)call_data);

    /* To synchronize the ComboBox record with XmList */
    XtVaGetValues(List(combo), XmNitemCount, &i, NULL);
    if (i != ItemCount(combo) ) {
	SyncWithList(combo_p);
	need_sync_flag = 0;
    }
	
    for (i = 0; i < ItemCount(combo); i++) {
	if ( need_sync_flag && 
	     !( (ListItems(combo))[i] && XmeStringIsValid((ListItems(combo))[i]) )) {
	    SyncWithList(combo_p);
	    need_sync_flag = 0;
	}
	if (XmStringCompare(xmStr, (ListItems(combo))[i]))
	    break;
    }

    if (i < ItemCount(combo) && i != SelectedPosition(combo) ) {
	SelectedPosition(combo) = i;
	if (SelectedItem(combo))
	    XmStringFree(SelectedItem(combo));
	SelectedItem(combo) = xmStr;
	if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
	    SetTextFieldData(combo_p, NULL);
	else
	    SetLabelData(combo_p, NULL, FALSE);
	XmListSetPos(List(combo), SelectedPosition(combo) + 1);
	XmListSelectPos(List(combo), SelectedPosition(combo) + 1, True);
	
        XtVaGetValues(List(combo), XmNtopItemPosition,
		      &(combo_p->top_item_position), NULL);
    }
    else
	XmStringFree( xmStr );
}

/*
 * We get the text-field activate callback, so pass it on to
 * the user if they requested it.  Our activate callback 
 * is just a convenience callback, so that the user doesn't
 * have to get the text-field first.  This make our integration
 * with XDesigner a little easier.
 */
static void
text_activate_cb(	Widget w,
			XtPointer client_data,
			XtPointer call_data)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)client_data;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo,ipot,DtComboBox,arrow,Widget));
    XmTextFieldWidget wt=(XmTextFieldWidget) Text(combo);
    XmString xmStr;
    Arg arg;
    String text_string;
    int i, need_sync_flag=1;
/* MotifBc
 */
    XtSetArg(arg, XmNvalue, &text_string);
    XtGetValues((Widget)wt, &arg, 1);
    xmStr = (XmString) XmStringCreateLocalized(text_string?text_string:"");
    if ( text_string )
	XtFree (text_string);

    if (ActivateCallback(combo))
	XtCallCallbackList((Widget)combo, 
			   ActivateCallback(combo),
			   (XtPointer)call_data);

    /* To synchronize the ComboBox record with XmList */
    XtVaGetValues(List(combo), XmNitemCount, &i, NULL);
    if (i != ItemCount(combo) ) {
	SyncWithList(combo_p);
	need_sync_flag = 0;
    }
	
    for (i = 0; i < ItemCount(combo); i++) {
	if ( need_sync_flag && 
	     !( (ListItems(combo))[i] && 
	       XmeStringIsValid((ListItems(combo))[i]) )) {
	    SyncWithList(combo_p);
	    need_sync_flag = 0;
	}
	if (XmStringCompare(xmStr, (ListItems(combo))[i]))
	    break;
    }

    if (i < ItemCount(combo) && i != SelectedPosition(combo) ) {
	SelectedPosition(combo) = i;
	if (SelectedItem(combo))
	    XmStringFree(SelectedItem(combo));
	SelectedItem(combo) = xmStr;
	if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
	    SetTextFieldData(combo_p, NULL);
	else
	    SetLabelData(combo_p, NULL, FALSE);
	XmListSetPos(List(combo), SelectedPosition(combo) + 1);
	XmListSelectPos(List(combo), SelectedPosition(combo) + 1, True);
        XtVaGetValues(List(combo), XmNtopItemPosition,
		      &(combo_p->top_item_position), NULL);
    }
    else
	XmStringFree( xmStr );
}


/*
 * We get the text-field focus callback, so pass it on to
 * the user if they requested it.  Our focus callback 
 * is just a convenience callback, so that the user doesn't
 * have to get the text-field first.  This make our integration
 * with XDesigner a little easier.
 */
static void
text_focus_cb(	Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)client_data;

    if (FocusCallback(combo))
	XtCallCallbackList((Widget)combo, FocusCallback(combo), 
			   (XtPointer)call_data);
}

/*
 * Try and keep our list related rsources in sync with the list widget.
 * This is not always possible, depending on if the programmer makes
 * list widget calls directly.  If we get out of sync with the
 * list widget, our SetValues() may not work correctly (when the
 * comparisons are done).  Should do get values in case list widget
 * names are changed?
 */
static void
SyncWithList(DtComboBoxPart *combo_p)
{
    XmListWidget list = (XmListWidget)combo_p->list;
    Arg		args[10];
    int		num_args;
    unsigned char unit_type = XmPIXELS;

/* Resolution Independent */
    XtSetArg(args[0],XmNunitType,&unit_type); 
    XtGetValues((Widget)list, args, 1);
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0],XmNunitType,XmPIXELS); 
	XtSetValues((Widget)list, args, 1);
    }
/* MotifBc
 */
    num_args = 0;
    XtSetArg(args[num_args],XmNitems,&(combo_p->list_items)); num_args ++;
    XtSetArg(args[num_args],XmNitems,&(combo_p->items)); num_args ++;
    XtSetArg(args[num_args],XmNitemCount,&(combo_p->item_count)); num_args ++;
    XtSetArg(args[num_args],XmNfontList,&(combo_p->list_font_list));
    num_args ++;
    XtSetArg(args[num_args],XmNlistMarginHeight,&(combo_p->list_margin_height));
    num_args ++;
    XtSetArg(args[num_args],XmNlistMarginWidth,&(combo_p->list_margin_width));
    num_args ++;
    XtSetArg(args[num_args],XmNlistSpacing,&(combo_p->list_spacing));
    num_args ++;
    XtSetArg(args[num_args],XmNtopItemPosition,&(combo_p->top_item_position));
    num_args ++;
    XtSetArg(args[num_args],XmNvisibleItemCount,&(combo_p->visible_item_count));
    num_args ++;
    XtGetValues((Widget)list, args, num_args);
    /* Adjustment with empty list */
    if (! combo_p->item_count) {
	combo_p->items = NULL;
	combo_p->list_items = NULL;
    }

/* Resolution Independent */
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0],XmNunitType,unit_type); 
	XtSetValues((Widget)list, args, 1);
    }
}

/*
 * Routines which manipulate the ComboBox list.  These are external
 * for use by users of our widget.
 */
Widget 
DtCreateComboBox(Widget parent,
		char *name,
		ArgList arglist,
		Cardinal num_args)
{
    return(XtCreateWidget(name, dtComboBoxWidgetClass, parent,
			  arglist, num_args));
}

void
DtComboBoxAddItem(	Widget combow,
			XmString item,
			int pos,
			Boolean unique)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)combow;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo,ipot,DtComboBox,arrow,Widget));
    XmStringTable list_items;
    int i;
    Arg arg;
    _DtWidgetToAppContext(combow);
    _DtAppLock(app);

    /* Try to help confused applications. */
    if (XmIsComboBox(combow))
      {
	XmComboBoxAddItem(combow, item, pos, unique);
	_DtAppUnlock(app);
	return;
      }

/* MotifBc
 */
    XtSetArg(arg, XmNitems, &list_items);
    XtGetValues(((Widget)List(combo)), &arg, 1);

    if (item && list_items) {
	for (i = 0; i < ItemCount(combo); i++)
	    if (XmStringCompare(item, list_items[i]))
		break;
	if ((i < ItemCount(combo)) && unique)
	  {
	    _DtAppUnlock(app);
	    return;
	  }
    }

    XmListAddItemUnselected(List(combo), item, pos);
    SyncWithList(combo_p);

    if (Label(combo)) {
	SetMaximumLabelSize(combo_p);
	if (Type(combo) == DtDROP_DOWN_LIST) {
	    ClearShadow(combo, TRUE);
	    if (RecomputeSize(combo))
		SetComboBoxSize(combo);
	    LayoutChildren(combo);
	    DrawShadow(combo);
	}
    }
    if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
	SetTextFieldData(combo_p, NULL);
    else
	SetLabelData(combo_p, NULL, FALSE);

    _DtAppUnlock(app);
}



void
DtComboBoxDeletePos(	Widget combow,
			int pos)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)combow;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo,ipot,DtComboBox,arrow,Widget));
    int selection_changed = 0;
    _DtWidgetToAppContext(combow);
    _DtAppLock(app);

    /* Try to help confused applications. */
    if (XmIsComboBox(combow))
      {
	XmComboBoxDeletePos(combow, pos);
	_DtAppUnlock(app);
	return;
      }

    if ( pos <= 0 || pos >combo_p->item_count ) {
	XtWarning(CB_DEL_POS);
	_DtAppUnlock(app);
	return;
    }

    XmListDeletePos(List(combo), pos);
    if ( pos == (combo_p->selected_position+1) ) {
	/* Since we delete the current item, we have to set one */
	if ( pos == combo_p->item_count && combo_p->selected_position>0) {
	    combo_p->selected_position--;
	    selection_changed ++;
	}
	XmListSelectPos(List(combo), combo_p->selected_position+1, True);
    }
    else if ( pos <= combo_p->selected_position) {
	combo_p->selected_position--;
	selection_changed ++;
    }	
    SyncWithList(combo_p);
    if ( selection_changed ) {
        if ( combo_p->selected_item )
	    XmStringFree(combo_p->selected_item );
        combo_p->selected_item = 
	    ItemCount(combo)?
		XmStringCopy(combo_p->list_items[combo_p->selected_position]):
		(XmString) NULL;
    }

    if (Label(combo)) {
	SetMaximumLabelSize(combo_p);
	if (Type(combo) == DtDROP_DOWN_LIST) {
	    ClearShadow(combo, TRUE);
	    if (RecomputeSize(combo))
		SetComboBoxSize(combo);
	    LayoutChildren(combo);
	    DrawShadow(combo);
	}
    }
    if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
	SetTextFieldData(combo_p, NULL);
    else
	SetLabelData(combo_p, NULL, FALSE);

    _DtAppUnlock(app);
}

void
DtComboBoxSetItem(	Widget combow,
			XmString item)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)combow;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo,ipot,DtComboBox,arrow,Widget));
    XmStringTable list_items;
    int i;
    Arg arg;
    _DtWidgetToAppContext(combow);
    _DtAppLock(app);

    /* Try to help confused applications. */
    if (XmIsComboBox(combow))
      {
	XmComboBoxSetItem(combow, item);
	_DtAppUnlock(app);
	return;
      }

/* MotifBc
 */
    XtSetArg(arg, XmNitems, &list_items);
    XtGetValues(((Widget)List(combow)), &arg, 1);

    if (item && list_items) {
	for (i = 0; i < ItemCount(combo); i++)
	    if (XmStringCompare(item, list_items[i]))
		break;
	if (i < ItemCount(combo)) {
	    SelectedPosition(combo) = i;
	    if (SelectedItem(combo))
		XmStringFree(SelectedItem(combo));
	    SelectedItem(combo) = XmStringCopy(item);
	    if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
		SetTextFieldData(combo_p, NULL);
	    else
		SetLabelData(combo_p, NULL, FALSE);
	    XmListSetItem(List(combo), item);
	    XmListSelectItem(List(combo), item, FALSE);
	    SyncWithList(combo_p);
	}
	else
	    XtWarning(CB_SET_ITEM);
    }
    else
	XtWarning(CB_SET_ITEM);

    _DtAppUnlock(app);
}

void
DtComboBoxSelectItem(	Widget combow,
			XmString item)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)combow;
    DtComboBoxPart *combo_p = (DtComboBoxPart*)
	&(XmField(combo,ipot,DtComboBox,arrow,Widget));
    XmStringTable list_items;
    XmString tmpStr;
    int i;
    Arg arg;
    _DtWidgetToAppContext(combow);
    _DtAppLock(app);

    /* Try to help confused applications. */
    if (XmIsComboBox(combow))
      {
	XmComboBoxSelectItem(combow, item);
	_DtAppUnlock(app);
	return;
      }

/* MotifBc
 */
    XtSetArg(arg, XmNitems, &list_items);
    XtGetValues(((Widget)List(combo)), &arg, 1);

    if (item && list_items) {
	for (i = 0; i < ItemCount(combo); i++)
	    if (XmStringCompare(item, list_items[i]))
		break;
	if (i < ItemCount(combo)) {
	    SelectedPosition(combo) = i;
	    tmpStr=SelectedItem(combo);
	    SelectedItem(combo) = XmStringCopy(item);
	    if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
		SetTextFieldData(combo_p, NULL);
	    else
		SetLabelData(combo_p, NULL, FALSE);
	    XmListDeselectAllItems(List(combo));
	    XmListSelectItem(List(combo), item, FALSE);
	    if(tmpStr)
		XmStringFree(tmpStr);
	    SyncWithList(combo_p);
	}
	else
	    XtWarning(CB_SELECT_ITEM);
    }
    else
	XtWarning(CB_SELECT_ITEM);

    _DtAppUnlock(app);
}


/*
 * Synthetic GetValues for List resources.
 */

static XmImportOperator
_XmSetSyntheticResForChild(	Widget widget,
				int offset,
				XtArgVal * value)
{ 
    return(XmSYNTHETIC_LOAD);
}

void
_DtComboBoxGetArrowSize(Widget w,
			int resource_offset,
			XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    Dimension data;
    Arg arg;

    XtSetArg(arg, XmNheight, &data);
    XtGetValues(Arrow(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetLabelString(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;

    if (LabelString(combo))
	*value = (XtArgVal)XmStringCopy(LabelString(combo));
    else
	*value = (XtArgVal)NULL;
}

void
_DtComboBoxGetListItemCount(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    int data;
    Arg arg;

    XtSetArg(arg, XmNitemCount, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListItems(Widget w,
			int resource_offset,
			XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    XmStringTable data;
    Arg arg;

    XtSetArg(arg, XmNitems, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListFontList(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    XmFontList data;
    Arg arg;

    XtSetArg(arg, XmNfontList, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListMarginHeight(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    Dimension data;
    Arg arg;

    XtSetArg(arg, XmNmarginHeight, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListMarginWidth(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    Dimension data;
    Arg arg;

    XtSetArg(arg, XmNmarginWidth, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListSpacing(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    Dimension data;
    Arg arg;

    XtSetArg(arg, XmNspacing, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListTopItemPosition(	Widget w,
					int resource_offset,
					XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    int data;
    Arg arg;

    XtSetArg(arg, XmNtopItemPosition, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

void
_DtComboBoxGetListVisibleItemCount(	Widget w,
					int resource_offset,
					XtArgVal *value)
{
    DtComboBoxWidget combo = (DtComboBoxWidget)w;
    int data;
    Arg arg;

    XtSetArg(arg, XmNvisibleItemCount, &data);
    XtGetValues(List(combo), &arg, 1);
    *value = (XtArgVal)data;
}

static Boolean
_CvtStringToType(
Display *dpy,
XrmValuePtr args,
Cardinal *num_args,
XrmValuePtr from, 
XrmValuePtr to,
XtPointer *data)
{
    char *from_str;
    static unsigned char to_value;

    if(*num_args !=0 ) {
	XtError(CB_CVTSTRING);
    }

    from_str = (char *)from->addr;
    if(strcmp(from_str, "DROP_DOWN_LIST")==0 ||
       strcmp(from_str, "drop_down_list")==0 ||
       strcmp(from_str, "DtDROP_DOWN_LIST")==0)
	to_value = DtDROP_DOWN_LIST;
    else if (strcmp(from_str, "DROP_DOWN_COMBO_BOX")==0 ||
             strcmp(from_str, "drop_down_combo_box")==0 ||
             strcmp(from_str, "DtDROP_DOWN_COMBO_BOX")==0)
	to_value = DtDROP_DOWN_COMBO_BOX;
    else {
	XtDisplayStringConversionWarning(dpy, from->addr, "ComboBoxType");
	return False;
    }

    if (to->addr == NULL) to->addr = (caddr_t) &to_value;
    else if (to->size <sizeof(unsigned char)) {
	XtDisplayStringConversionWarning(dpy, from->addr, "ComboBoxType");
	return False;
    }
    else
	*(unsigned char *) to->addr = to_value;

    to->size = sizeof(unsigned char);

    return True;
}

static void	
input_ungrab ( DtComboBoxWidget combo, int ungrab_mask)
{
    XmDisplay disp = (XmDisplay) XmGetXmDisplay(XtDisplay(combo));

    if ( ungrab_mask & GRAB_POINTER )
        XtUngrabPointer(Shell(combo), CurrentTime);
    if ( ungrab_mask & GRAB_KEYBOARD )
        XtUngrabKeyboard(List(combo), CurrentTime);
    _XmRemoveGrab(Shell(combo));
    
    /* We move XtPopdown() here and do _XmRemoveGrab first.
     * This is a fix to solve an MR in Desktop which over a time period
     * develope a possible core-dump due to duplicated removeGrab() calls
     */
    PoppedUp(combo) = FALSE;
    XtPopdown(Shell(combo));
    if (Type(combo) == DtDROP_DOWN_COMBO_BOX)
        XmProcessTraversal(Text(combo), XmTRAVERSE_CURRENT);

    /* This is to enable the Drag-and-Drop operation */
    disp->display.userGrabbed = False;
}
