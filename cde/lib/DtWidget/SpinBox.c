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
/* $XConsortium: SpinBox.c /main/9 1996/10/29 12:49:58 cde-hp $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
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
 * 9 Hillside Avenue, 
 * Waltham, MA  02154
 *
 * SpinBox.c (DtSpinBoxWidget):
 *
 * I wanted a margin around the widget (outside the shadow, like buttons), 
 * so that the spin_box could be made the smae size as a 
 * push-button, etc.  The bulletin-board widget always puts the shadow at 
 * the outside edge of the widget, so spin_box is a sublcass of
 * manager, and we do everything ourselves.
 * 
 * One must be carefull when using Dimension (for core width and height).
 * Dimension is an unsigned short.  This causes problems when subtracting
 * and ending up with what should be a negative number (but it doesn't).
 * All child widget positioning is done by the spin_box.  We don't
 * use any heavy-weight forms, etc. to help us out.
 * 
 * There is no padding when editable.  If using a label, give it a
 * small margin, so it doesn't run up against the side of our
 * shadow or the arrow.
 *
 * Make some of the SpinBox functions common, so they can be shared
 * with ComboBox.
 *
 * Known bugs:
 *	    Changing margin_width or margin_height resources when the
 *	    spin_box has focus will probably result in display glitches.
 */
/*
 * The DtSpinBox widget is rigged with the Motif widget binary compatibilit
 * mechanism. All Motif-specific changes for this mechanism are preceded
 * by a comment including the string "MotifBc".
 *
 * For a description of the Motif widget binary compatibility mechanism
 * see the reference manual entry on XmResolveAllPartOffsets().
 *
 */

#include <Dt/DtMsgsP.h>
#include <Xm/DrawP.h>
#include <Xm/XmP.h>
#include <Xm/RepType.h>
#include "SpinBoxP.h"
#include "DtWidgetI.h"

#include <Xm/XmPrivate.h>    /* _XmShellIsExclusive */

#ifdef I18N_MSG
#include <langinfo.h>
#endif 


/*
 * MotifBc
 */
#define DtSpinBoxIndex (XmManagerIndex + 1)
static XmOffsetPtr ipot; /* Instance part offset table */
static XmOffsetPtr cpot; /* Constraint part offset table */

static void	ClassInitialize ();
static void	Initialize (DtSpinBoxWidget request, 
			       DtSpinBoxWidget new, ArgList given_args, 
			       Cardinal *num_args);
static XmNavigability WidgetNavigable (DtSpinBoxWidget spin);
static void	_SpinBoxFocusIn (DtSpinBoxWidget spin, XEvent *event, 
				       char **params, Cardinal *num_params);
static void	_SpinBoxFocusOut (DtSpinBoxWidget spin, XEvent *event,
					char **params, Cardinal *num_params);
static void	DrawHighlight (DtSpinBoxWidget spin, Boolean clear);
static void	_SpinBoxUp (DtSpinBoxWidget spin, 
				  XEvent *event, char **params, 
				  Cardinal *num_params);
static void	_SpinBoxDown (DtSpinBoxWidget spin, 
				    XEvent *event, char **params, 
				    Cardinal *num_params);
static void	_SpinBoxLeft (DtSpinBoxWidget spin, 
				    XEvent *event, char **params, 
				    Cardinal *num_params);
static void	_SpinBoxRight (DtSpinBoxWidget spin, 
				     XEvent *event, char **params, 
				     Cardinal *num_params);
static void	_SpinBoxBeginLine (DtSpinBoxWidget spin, 
					 XEvent *event, char **params, 
					 Cardinal *num_params);
static void	_SpinBoxEndLine (DtSpinBoxWidget spin, 
				       XEvent *event, char **params, 
				       Cardinal *num_params);
static void     _SpinBoxGetFocus (DtSpinBoxWidget spin,
                                       XEvent *event, char **params,
                                       Cardinal *num_params);
static void     _SpinBoxPrevTabGroup (DtSpinBoxWidget spin,
                                       XEvent *event, char **params,
                                       Cardinal *num_params);
static void     _SpinBoxNextTabGroup (DtSpinBoxWidget spin,
                                       XEvent *event, char **params,
                                       Cardinal *num_params);

static void	CheckResources (DtSpinBoxWidget spin);
static void	Destroy (DtSpinBoxWidget spin);
static void	Resize (DtSpinBoxWidget spin);
static void	Redisplay (DtSpinBoxWidget w, XEvent *event, 
			      Region region);
static XtGeometryResult GeometryManager (Widget w, 
					    XtWidgetGeometry *request, 
					    XtWidgetGeometry *reply);
static void	SetSpinBoxSize (DtSpinBoxWidget spin);
static void	ForceChildSizes (DtSpinBoxWidget spin);
static void	CalculateSizes (DtSpinBoxWidget spin,
				Dimension *pwidth,
				Dimension *pheight,
				Dimension *parrow_width);
static void	LayoutChildren (DtSpinBoxWidget spin);
static Boolean	SetValues (DtSpinBoxWidget current, 
			      DtSpinBoxWidget request, 
			      DtSpinBoxWidget new);
static void	ClearShadow (DtSpinBoxWidget w, Boolean all);
static void	DrawShadow (DtSpinBoxWidget w);
static void	StoreResourceInfo (DtSpinBoxPart *spin_p,
				      DtSpinBoxPart *old_p,
				      Boolean do_items);
static char*	GetTextString (XmString xm_string);
static void	SetTextFieldData (DtSpinBoxWidget spin);
static void	SetMaximumLabelSize (DtSpinBoxPart *spin_p);
static void	SetLabelData (DtSpinBoxWidget spin);
static void	timer_dispatch (XtPointer client_data, XtIntervalId *id);
static void	TextFieldActivate (DtSpinBoxPart *spin_p);
static Boolean	SendCallback (DtSpinBoxWidget spin, XEvent *event,
				 Boolean value_changed, int position,
				 float current, Boolean crossed);
static void	FinishUpDown (DtSpinBoxWidget spin, 
				 XtPointer arrow_call_data, int new_position,
				 float new_current, Boolean crossed);
static void	up_cb (Widget w, XtPointer client_data, 
			  XtPointer call_data);
static void	down_cb (Widget w, XtPointer client_data, 
			    XtPointer call_data);
static void	disarm_cb (Widget w, XtPointer client_data, 
			      XtPointer call_data);
static void	grab_leave_cb (Widget w, XtPointer client_data, 
				  XEvent *event, Boolean *dispatch);
static void	text_losing_focus_cb (Widget w, XtPointer client_data,
					 XtPointer call_data);
static void	text_activate_cb (Widget w, XtPointer client_data,
				     XtPointer call_data);
static void	text_focus_cb (Widget w, XtPointer client_data,
				  XtPointer call_data);
static XmImportOperator _XmSetSyntheticResForChild (Widget widget,
						       int offset, 
						       XtArgVal * value);

static XmString InitLabel = NULL;

/*
 * MotifBc
 */
#define Label(w) XmField(w,ipot,DtSpinBox,label,Widget)
#define UpArrow(w) XmField(w,ipot,DtSpinBox,up_arrow,Widget)
#define DownArrow(w) XmField(w,ipot,DtSpinBox,down_arrow,Widget)
#define WhichArrow(w) XmField(w,ipot,DtSpinBox,which_arrow,unsigned char)
#define InitCb(w) XmField(w,ipot,DtSpinBox,init_cb,Boolean)
#define Grabbed(w) XmField(w,ipot,DtSpinBox,grabbed,Boolean)
#define Base(w) XmField(w,ipot,DtSpinBox,base,int)
#define Min(w) XmField(w,ipot,DtSpinBox,min,float)
#define Max(w) XmField(w,ipot,DtSpinBox,max,float)
#define Increment(w) XmField(w,ipot,DtSpinBox,increment,float)
#define Current(w) XmField(w,ipot,DtSpinBox,current,float)
#define FloatFormat(w) (String) &(XmField(w,ipot,DtSpinBox,float_format,char *))
#define OldWidth(w) XmField(w,ipot,DtSpinBox,old_width,Dimension)
#define OldHeight(w) XmField(w,ipot,DtSpinBox,old_height,Dimension)
#define LabelMaxLength(w) XmField(w,ipot,DtSpinBox,label_max_length,Dimension)
#define LabelMaxHeight(w) XmField(w,ipot,DtSpinBox,label_max_height,Dimension)

#define ArrowSensitivity(w) XmField(w,ipot,DtSpinBox,arrow_sensitivity,unsigned char)
#define DecimalPoints(w) XmField(w,ipot,DtSpinBox,decimal_points,short)
#define NumericIncrement(w) XmField(w,ipot,DtSpinBox,numeric_increment,int)
#define Maximum(w) XmField(w,ipot,DtSpinBox,maximum,int)
#define Minimum(w) XmField(w,ipot,DtSpinBox,minimum,int)
#define ItemCount(w) XmField(w,ipot,DtSpinBox,item_count,int)
#define Position(w) XmField(w,ipot,DtSpinBox,position,int)
#define ChildType(w) XmField(w,ipot,DtSpinBox,child_type,unsigned char)
#define Items(w) XmField(w,ipot,DtSpinBox,items,XmStringTable)
#define ActivateCallback(w) XmField(w,ipot,DtSpinBox,activate_callback,XtCallbackList)
#define Alignment(w) XmField(w,ipot,DtSpinBox,alignment,unsigned char)
#define ArrowLayout(w) XmField(w,ipot,DtSpinBox,arrow_layout,unsigned char)
#define ArrowSize(w) XmField(w,ipot,DtSpinBox,arrow_size,Dimension)
#define TextColumns(w) XmField(w,ipot,DtSpinBox,text_columns,short)
#define Editable(w) XmField(w,ipot,DtSpinBox,editable,Boolean)
#define FocusCallback(w) XmField(w,ipot,DtSpinBox,focus_callback,XtCallbackList)
#define InitialDelay(w) XmField(w,ipot,DtSpinBox,initial_delay,unsigned int)
#define LosingFocusCallback(w) XmField(w,ipot,DtSpinBox,losing_focus_callback,XtCallbackList)
#define MarginHeight(w) XmField(w,ipot,DtSpinBox,margin_height,Dimension)
#define MarginWidth(w) XmField(w,ipot,DtSpinBox,margin_width,Dimension)
#define TextMaxLength(w) XmField(w,ipot,DtSpinBox,text_max_length,int)
#define ModifyVerifyCallback(w) XmField(w,ipot,DtSpinBox,modify_verify_callback,XtCallbackList)
#define RecomputeSize(w) XmField(w,ipot,DtSpinBox,recompute_size,Boolean)
#define RepeatDelay(w) XmField(w,ipot,DtSpinBox,repeat_delay,unsigned int)
#define Text(w) XmField(w,ipot,DtSpinBox,text,Widget)
#define ValueChangedCallback(w) XmField(w,ipot,DtSpinBox,value_changed_callback,XtCallbackList)
#define Wrap(w) XmField(w,ipot,DtSpinBox,wrap,Boolean)
#define Timer(w) XmField(w,ipot,DtSpinBox,timer,XtIntervalId)

/* SpinBox and Superclass resource macros*/
#define PUnitType(w)        w->primitive.unit_type
#define MUnitType(w)        w->manager.unit_type
#define MFgPixel(w)         w->manager.foreground
#define SPIN_SHADOW(w)	    w->manager.shadow_thickness
#define CBgPixel(w)	    w->core.background_pixel
#define CBgPixmap(w)        w->core.background_pixmap
#define Width(w)	    w->core.width
#define Height(w)	    w->core.height
#define SPIN_MARGIN_W(w)    MarginWidth(w)
#define SPIN_MARGIN_H(w)    MarginHeight(w)
#define MAXINT 2147483647  /* Taken from TextF.c */
#define DEFAULT_COL 20 

/* USL: Label get Focus */
static XtTranslations child_trans_label;
/* USL: Keyboard only for Text */
static XtTranslations child_trans_text;
static XtTranslations child_trans_arrow;

static XtTranslations child_trans;
static XtTranslations spin_trans;


static char const SpinBoxTranslationTable[] = "\
	<FocusIn>:		SpinBoxFocusIn() \n\
	<FocusOut>:		SpinBoxFocusOut() \n\
        <Key>osfUp:		SpinBoxUp() \n\
        <Key>osfDown:		SpinBoxDown() \n\
        <Key>osfRight:		SpinBoxRight() \n\
        <Key>osfLeft:		SpinBoxLeft() \n\
        <Key>osfBeginLine:	SpinBoxBeginLine() \n\
        <Key>osfEndLine:	SpinBoxEndLine() \n\
";

static char const SpinBoxChildTranslationTable[] = "\
        <Key>osfUp:		SpinBoxUp(child) \n\
        <Key>osfDown:		SpinBoxDown(child) \n\
        <Key>osfRight:		SpinBoxRight(child) \n\
        <Key>osfLeft:		SpinBoxLeft(child) \n\
        <Key>osfBeginLine:	SpinBoxBeginLine(child) \n\
        <Key>osfEndLine:	SpinBoxEndLine(child) \n\
";

/* #5: Label get focus */
static char const SpinBoxLabelTranslationTable[] = "\
        <Key>osfUp:             SpinBoxUp(child) \n\
        <Key>osfDown:           SpinBoxDown(child) \n\
        <Key>osfRight:          SpinBoxRight(child) \n\
        <Key>osfLeft:           SpinBoxLeft(child) \n\
        <Key>osfBeginLine:      SpinBoxBeginLine(child) \n\
        <Key>osfEndLine:        SpinBoxEndLine(child) \n\
        <Key>osfDown:        SpinBoxGetFocus() \n\
	<Btn1Down>,<Btn1Up>:  SpinBoxGetFocus() \n\
	<Key>osfSelect:       SpinBoxGetFocus() \n\
	~s ~m ~a <Key>space:  SpinBoxGetFocus() \n\
";

/* USL: Keyboard Only Traversing During Editable-Mode */
static char const SpinBoxTextTranslationTable[] = "\
        <Key>osfUp:             SpinBoxUp(child) SpinBoxRight(child)\n\
        <Key>osfDown:           SpinBoxDown(child) SpinBoxLeft(child)\n\
";
static char const SpinBoxArrowTranslationTable[] = "\
        <Key>osfUp:             SpinBoxUp(child) \n\
        <Key>osfDown:           SpinBoxDown(child) \n\
        <Key>osfRight:          SpinBoxRight(child) \n\
        <Key>osfLeft:           SpinBoxLeft(child) \n\
        <Key>osfBeginLine:      SpinBoxBeginLine(child) \n\
        <Key>osfEndLine:        SpinBoxEndLine(child) \n\
        s ~m ~a <Key>Tab:    SpinBoxPrevTabGroup()\n\
	~m ~a <Key>Tab:      SpinBoxNextTabGroup()\n\
";


static XtActionsRec SpinBoxActionTable[] = {
       {"SpinBoxFocusIn",	(XtActionProc)_SpinBoxFocusIn},
       {"SpinBoxFocusOut",	(XtActionProc)_SpinBoxFocusOut},
       {"SpinBoxUp",		(XtActionProc)_SpinBoxUp},
       {"SpinBoxDown",	(XtActionProc)_SpinBoxDown},
       {"SpinBoxRight",	(XtActionProc)_SpinBoxRight},
       {"SpinBoxLeft",	(XtActionProc)_SpinBoxLeft},
       {"SpinBoxBeginLine",	(XtActionProc)_SpinBoxBeginLine},
       {"SpinBoxEndLine",	(XtActionProc)_SpinBoxEndLine},
       {"SpinBoxGetFocus",	(XtActionProc)_SpinBoxGetFocus},
       {"SpinBoxPrevTabGroup",   (XtActionProc)_SpinBoxPrevTabGroup},
       {"SpinBoxNextTabGroup",   (XtActionProc)_SpinBoxNextTabGroup},
};

/* DtSpinBoxWidget resources */
/* MotifBc */
#define offset(field) XtOffset(DtSpinBoxWidget, field)
#define DtOffset(field) XmPartOffset(DtSpinBox,field)
static XmPartResource resources[] = {
    {XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension, 
	 sizeof(Dimension), offset(manager.shadow_thickness),
	 XmRImmediate, (XtPointer)TEXT_FIELD_SHADOW},

    /* Common resources */
    {DtNactivateCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(activate_callback), XmRCallback, 
	 (XtPointer)NULL},
    {DtNalignment, DtCAlignment, XmRAlignment, sizeof(unsigned char),
	 DtOffset(alignment), XmRImmediate, 
	 (XtPointer)DtALIGNMENT_BEGINNING},
    {DtNarrowLayout, DtCArrowLayout, DtRArrowLayout, sizeof(unsigned char),
	 DtOffset(arrow_layout), XmRImmediate, 
	 (XtPointer)DtARROWS_END},
    {DtNarrowSensitivity, DtCArrowSensitivity, DtRArrowSensitivity,
	 sizeof(unsigned char), DtOffset(arrow_sensitivity),
	 XmRImmediate, (XtPointer)DtARROWS_SENSITIVE},
    {DtNspinBoxChildType, DtCSpinBoxChildType, DtRSpinBoxChildType, 
	 sizeof(unsigned char),
	 DtOffset(child_type), XmRImmediate, (XtPointer)DtSTRING},
    {DtNcolumns, DtCColumns, XmRShort, sizeof(short),
	 DtOffset(text_columns), XmRImmediate, (XtPointer)DEFAULT_COL},
    {DtNdecimalPoints, DtCDecimalPoints, XmRShort, sizeof( short ),
	 DtOffset(decimal_points), XmRImmediate, (XtPointer)0},
    {DtNeditable, DtCEditable, XmRBoolean, sizeof(Boolean),
	 DtOffset(editable), XmRImmediate, (XtPointer)TRUE},
    {DtNfocusCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(focus_callback), XmRCallback, 
	 (XtPointer)NULL},
    {DtNincrementValue, DtCIncrementValue, XmRInt, sizeof(int),
	 DtOffset(numeric_increment), XmRImmediate, (XtPointer)1},
    {DtNinitialDelay, DtCInitialDelay, XmRInt, sizeof(unsigned int),
	 DtOffset(initial_delay), XmRImmediate, (XtPointer)250},
    {DtNnumValues, DtCNumValues, DtRNumValues, sizeof(int),
	 DtOffset(item_count), XmRImmediate, (XtPointer)0},
    {DtNvalues, DtCItems, XmRXmStringTable, sizeof(XmStringTable),
	 DtOffset(items), XmRImmediate, (XtPointer)NULL},
    {DtNlosingFocusCallback, DtCCallback, XmRCallback, sizeof(XtCallbackList),
	 DtOffset(losing_focus_callback), XmRCallback, 
	 (XtPointer)NULL},
    {DtNmarginHeight, DtCMarginHeight, XmRVerticalDimension, sizeof(Dimension),
	 DtOffset(margin_height), XmRImmediate, (XtPointer)MARGIN},
    {DtNmarginWidth, DtCMarginWidth, XmRHorizontalDimension, sizeof(Dimension),
	 DtOffset(margin_width), XmRImmediate, (XtPointer)MARGIN},
    {DtNmaximumValue, DtCMaximumValue, XmRInt, sizeof(int), 
	 DtOffset(maximum), XmRImmediate, (XtPointer)10},
    {DtNmaxLength, DtCMaxLength, XmRInt, sizeof(int),
	 DtOffset(text_max_length), XmRImmediate, (XtPointer)MAXINT},
    {DtNminimumValue, DtCMinimumValue, XmRInt, sizeof(int), 
	 DtOffset(minimum), XmRImmediate, (XtPointer)0},
    {DtNmodifyVerifyCallback, DtCCallback, XmRCallback, 
	 sizeof(XtCallbackList), DtOffset(modify_verify_callback),
	 XmRCallback, (XtPointer)NULL},
    {DtNposition, DtCPosition, XmRInt, sizeof(int),
	 DtOffset(position), XmRImmediate, (XtPointer)0},
    {DtNrecomputeSize, DtCRecomputeSize, XmRBoolean, sizeof(Boolean),
	 DtOffset(recompute_size), XmRImmediate, (XtPointer)TRUE},
    {DtNrepeatDelay, DtCRepeatDelay, XmRInt, sizeof(unsigned int),
	 DtOffset(repeat_delay), XmRImmediate, (XtPointer)200},
    {DtNtextField, DtCTextField, XmRWidget, sizeof(Widget),
	 DtOffset(text), XmRImmediate, (XtPointer)NULL},
    {DtNvalueChangedCallback, DtCCallback, XmRCallback, 
	 sizeof(XtCallbackList), DtOffset(value_changed_callback),
	 XmRCallback, (XtPointer)NULL},
    {DtNwrap, DtCWrap, XmRBoolean, sizeof(Boolean),
	 DtOffset(wrap), XmRImmediate, (XtPointer)TRUE},
};

/* Synthetic resources.  Only used for Motif API arrowSize right now */
static XmSyntheticResource syn_resources[] = {
    {DtNarrowSize, sizeof(Dimension), DtOffset(arrow_size), 
	 _DtSpinBoxGetArrowSize, _XmSetSyntheticResForChild},
    {DtNmarginHeight, sizeof(Dimension), DtOffset(margin_height),
	XmeFromVerticalPixels, XmeToVerticalPixels},
    {DtNmarginWidth, sizeof(Dimension), DtOffset(margin_width),
	XmeFromHorizontalPixels, XmeToHorizontalPixels},
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
externaldef(dtspinboxclassrec) DtSpinBoxClassRec dtSpinBoxClassRec =
{
    {		/* core_class fields      */
    (WidgetClass)&(xmManagerClassRec),		/* superclass         */    
    (String)"DtSpinBox",			/* class_name         */    
    (Cardinal)sizeof(DtSpinBoxPart),		/* widget_size        */    
    (XtProc)ClassInitialize,			/* class_initialize   */    
    (XtWidgetClassProc)NULL,			/* class_part_init    */    
    (XtEnum)FALSE,				/* class_inited       */    
    (XtInitProc)Initialize,			/* initialize         */    
    (XtArgsProc)NULL,				/* initialize_hook    */    
    (XtRealizeProc)XtInheritRealize,		/* realize            */    
    (XtActionList)SpinBoxActionTable,		/* actions	      */    
    (Cardinal)XtNumber(SpinBoxActionTable),	/* num_actions        */    
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
    {		/* spin_box_class fields */     
    (Boolean)0,
    }
};

externaldef(dtspinboxwidgetclass) WidgetClass dtSpinBoxWidgetClass =
						(WidgetClass)&dtSpinBoxClassRec;

static XmRepTypeId _DtRID_SB_ARROW_SENSITIVITY_TYPE;
static String _DtArrowSensitivityNames[] = {
	"arrows_insensitive",
	"arrows_increment_sensitive",
	"arrows_decrement_sensitive",
	"arrows_sensitive"
};

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
    XmResolveAllPartOffsets(dtSpinBoxWidgetClass, &ipot, &cpot);

    child_trans = XtParseTranslationTable(SpinBoxChildTranslationTable);
    spin_trans = XtParseTranslationTable(SpinBoxTranslationTable);

    child_trans_label=XtParseTranslationTable(SpinBoxLabelTranslationTable);
    child_trans_text=XtParseTranslationTable(SpinBoxTextTranslationTable);
    child_trans_arrow=XtParseTranslationTable(SpinBoxArrowTranslationTable);

    _DtRID_SB_ARROW_SENSITIVITY_TYPE =
      XmRepTypeRegister(DtRArrowSensitivity, _DtArrowSensitivityNames,
			NULL, XtNumber(_DtArrowSensitivityNames));

    InitLabel = XmStringCreateLocalized(SB_LABEL);
}

/*
 * SpinBox initialization function.  This builds the widgets inside
 * our widget, to get the correct layout.  If the editable resource
 * is TRUE, we create a textField; if FALSE, we create a label.  If the
 * user changes this resource later, we will create the other widget
 * (textField or Label).  We don't want to carry backage from both
 * widgets if the user never changes the editable resource.
 */
static void
Initialize(	DtSpinBoxWidget request,
		DtSpinBoxWidget new,
		ArgList given_args,
		Cardinal *num_args)
{   /* MotifBc */
    DtSpinBoxPart *spin_p = (DtSpinBoxPart*)
        &(XmField(new,ipot,DtSpinBox,label,Widget));
    char *widget_name;
    Arg args[20];
    int n;
    /* Resolution Independent */
    unsigned char unit_type = MUnitType(new);

    /* Overwrite the manager's focusIn and focusOut translations */
    XtOverrideTranslations((Widget)new, spin_trans);

    widget_name = XtMalloc(strlen(XtName((Widget)new)) + 10);

    Text(new) = (Widget)NULL;
    Label(new) = (Widget)NULL;
    OldWidth(new) = 0;
    OldHeight(new) = 0;
    InitCb(new) = TRUE;
    Grabbed(new) = FALSE;

    CheckResources(new);

    /*
     * Create the text or label depending on editable resource.
     */
    if (Editable(new)) {
	sprintf(widget_name, "%s_TF", XtName((Widget)new));
	n = 0;
	XtSetArg(args[n], XmNmaxLength, TextMaxLength(new)); n++;
	XtSetArg(args[n], XmNmarginWidth, TEXT_CONTEXT_MARGIN); n++;
	XtSetArg(args[n], XmNmarginHeight, 2); n++;
        /* Resolution Independent */
        if (unit_type != XmPIXELS) {
                XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
        }
	Text(new) = XtCreateManagedWidget(widget_name,
					     xmTextFieldWidgetClass,
					     (Widget)new, args, n);
	XtAddCallback(Text(new), XmNlosingFocusCallback, 
		      text_losing_focus_cb, (XtPointer)new);
	XtAddCallback(Text(new), XmNactivateCallback, 
		      text_activate_cb, (XtPointer)new);
	XtAddCallback(Text(new), XmNfocusCallback, 
		      text_focus_cb, (XtPointer)new);
	/* USL */
	XtOverrideTranslations((Widget)Text(new), child_trans_text);
	n = 0;
	if (TextColumns(request) == DEFAULT_COL && Width(request)) {
		Dimension width;
		CalculateSizes(new, &width, NULL, NULL);
		XtSetArg(args[n], XmNwidth, width); n++;
	}
	else {
		XtSetArg(args[n], XmNcolumns, TextColumns(new)); n++;
	}
        if (unit_type != XmPIXELS) {
                XtSetArg(args[n], XmNunitType, unit_type); n++;
        }                
        XtSetValues(Text(new), args, n);
    }
    else {
	sprintf(widget_name, "%s_Label", XtName((Widget)new));
	SPIN_SHADOW(new) = LABEL_SHADOW;
	n = 0;
	XtSetArg(args[n], XmNalignment, Alignment(new)); n++;
	XtSetArg(args[n], XmNrecomputeSize, FALSE); n++;
	XtSetArg(args[n], XmNlabelString, InitLabel); n++;
	XtSetArg(args[n], XmNmarginLeft, LABEL_PADDING); n++;
	XtSetArg(args[n], XmNmarginRight, LABEL_PADDING); n++;
	XtSetArg(args[n], XmNmarginWidth, TEXT_CONTEXT_MARGIN); n++;
	XtSetArg(args[n], XmNmarginHeight, 2); n++;
        if (unit_type != XmPIXELS) {
                XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
        }
	Label(new) = XtCreateManagedWidget(widget_name, xmLabelWidgetClass,
					      (Widget)new, args, n);
	/* USL */
	XtOverrideTranslations((Widget)Label(new), child_trans_label);
	n = 0;
        if (unit_type != XmPIXELS) {
                XtSetArg(args[n], XmNunitType, unit_type); n++;
        }                
	if (Width(new)) {
		Dimension width;
		CalculateSizes(new, &width, NULL, NULL);
		XtSetArg(args[n], XmNwidth,  width); n++;
	}
	if (n>0)
                XtSetValues(Label(new), args, n);
    }

    /*
     * Create the 2 ArrowWidgets.
     */
    sprintf(widget_name, "%s_Up", XtName((Widget)new));
    n = 0;
    if (ArrowLayout(new) == DtARROWS_SPLIT) {
	XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT); n++;
    }
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNtraversalOn, FALSE); n++;
    XtSetArg(args[n], XmNforeground, CBgPixel(new)); n++;
    UpArrow(new) = XtCreateManagedWidget(widget_name, 
					     xmArrowButtonWidgetClass,
					     (Widget)new, args, n);
    XtOverrideTranslations((Widget)UpArrow(new), child_trans_arrow);

    sprintf(widget_name, "%s_Down", XtName((Widget)new));
    if (ArrowLayout(new) == DtARROWS_SPLIT) {
	XtSetArg(args[n], XmNarrowDirection, XmARROW_LEFT); n++;
    }
    else {
	XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN); n++;
    }
    DownArrow(new) = XtCreateManagedWidget(widget_name, 
					       xmArrowButtonWidgetClass,
					       (Widget)new, args, n);
    XtOverrideTranslations((Widget)DownArrow(new), child_trans_arrow);

    /* Set sensitivity of arrows (up arrow is right arrow) */
    if ((ArrowSensitivity(new) == DtARROWS_INSENSITIVE) ||
	(ArrowSensitivity(new) == DtARROWS_DECREMENT_SENSITIVE)) 
	XtSetSensitive(UpArrow(new), FALSE);
    if ((ArrowSensitivity(new) == DtARROWS_INSENSITIVE) ||
	(ArrowSensitivity(new) == DtARROWS_INCREMENT_SENSITIVE)) 
	XtSetSensitive(DownArrow(new), FALSE);

    /* 
     * Arm causes the value to change and the timer to start.
     * Disarm (leaveNotify from grab) causes the timer to stop.
     */
    XtAddCallback(UpArrow(new), XmNarmCallback, up_cb, (XtPointer)new);
    XtAddCallback(UpArrow(new), XmNdisarmCallback, disarm_cb, 
		  (XtPointer)new);
    XtAddEventHandler(UpArrow(new), LeaveWindowMask, FALSE, grab_leave_cb, 
		      (XtPointer)new);
    XtAddCallback(DownArrow(new), XmNarmCallback, down_cb, (XtPointer)new);
    XtAddCallback(DownArrow(new), XmNdisarmCallback, disarm_cb, 
		  (XtPointer)new);
    XtAddEventHandler(DownArrow(new), LeaveWindowMask, FALSE, 
		      grab_leave_cb, (XtPointer)new);

    /* Initialize everything based on what the resource values are */
    StoreResourceInfo(spin_p, NULL, TRUE);

    /*
     * Set initial value in text or label if items was specified
     */
    if (Editable(new) == FALSE) {
	SetLabelData(new);
	SetMaximumLabelSize(spin_p);
    }
    else
    {
	SetTextFieldData(new);
    }
    SetSpinBoxSize(new);
    LayoutChildren(new);
    XtFree(widget_name);

    /* Store Converter for DtNmaximumValue, DtNminimumValue, DtNincrementValue,
     */
    XtSetTypeConverter(XmRString, DtRMaximumValue, XtCvtStringToInt, NULL, 0, 
	    XtCacheAll, NULL);
    XtSetTypeConverter(XmRString, DtRMinimumValue, XtCvtStringToInt, NULL, 0, 
	    XtCacheAll, NULL);
    XtSetTypeConverter(XmRString, DtRIncrementValue, XtCvtStringToInt, NULL, 0, 
	    XtCacheAll, NULL);
    XtSetTypeConverter(XmRString, DtRNumValues, XtCvtStringToInt, NULL, 0, 
	    XtCacheAll, NULL);
    /*
     * this is so these resources can be set uniformly through XtCreateWidget
     * or through defaults file as *spin.backgound
     */

    n=0;
    XtSetArg(args[n],XmNbackground,CBgPixel(new));n++;
    XtSetArg(args[n],XmNbackgroundPixmap,CBgPixmap(new));n++;
    XtSetArg(args[n],XmNforeground,MFgPixel(new));n++;
    if(Text(new))
        XtSetValues (Text(new),args,n);
    if(Label(new))
        XtSetValues (Label(new),args,n);
    if(UpArrow(new))
        XtSetValues (UpArrow(new),args,n);
    if(DownArrow(new))
        XtSetValues (DownArrow(new),args,n);

}


/*
 * Allow the manager to gain focus if not editable.  If editable (using
 * text-field), then let the toolkit give focus to the text-field.
 */
static XmNavigability
WidgetNavigable(DtSpinBoxWidget spin)
{   
    XmNavigationType nav_type = ((XmManagerWidget)spin)->manager.navigation_type;

    if (spin->core.sensitive &&  spin->core.ancestor_sensitive &&
	((XmManagerWidget)spin)->manager.traversal_on) {
	if ((nav_type == XmSTICKY_TAB_GROUP) ||
	    (nav_type == XmEXCLUSIVE_TAB_GROUP) ||
	    ((nav_type == XmTAB_GROUP) &&
	     !_XmShellIsExclusive((Widget)spin))) {
	    if (Editable(spin))
		return(XmDESCENDANTS_TAB_NAVIGABLE);
	    else
		return(XmTAB_NAVIGABLE);
	}
	return(XmDESCENDANTS_NAVIGABLE);
    }
    return(XmNOT_NAVIGABLE);
}

/* 
 * The spin_box gets focus.
 */
static void 
_SpinBoxFocusIn(	DtSpinBoxWidget spin,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    DrawHighlight(spin, FALSE);
}

/* 
 * The spin_box loses focus.
 */
static void 
_SpinBoxFocusOut(	DtSpinBoxWidget spin,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    DrawHighlight(spin, TRUE);
}

/*
 * This function gets called whenever we draw or clear the shadow (to
 * redraw highlight during resize, etc), as well as during focus_in
 * and focus_out events.
 */
static void
DrawHighlight(	DtSpinBoxWidget spin,
		Boolean clear)
{
    XRectangle rect[4] ;

    if (XtIsRealized((Widget)spin)) {
	if (clear) {
	    rect[0].x = rect[1].x = rect[2].x = 0;
	    rect[3].x = OldWidth(spin) - SPIN_MARGIN_W(spin);
	    rect[0].y = rect[2].y = rect[3].y = 0 ;
	    rect[1].y = OldHeight(spin) - SPIN_MARGIN_H(spin);
	    rect[0].width = rect[1].width = OldWidth(spin);
	    rect[2].width = rect[3].width = SPIN_MARGIN_W(spin);
	    rect[0].height = rect[1].height = SPIN_MARGIN_H(spin);
	    rect[2].height = rect[3].height = OldHeight(spin);
	    XFillRectangles(XtDisplayOfObject((Widget)spin),
			    XtWindowOfObject((Widget)spin), 
			    spin->manager.background_GC, rect, 4);
	}
	else if (XmGetFocusWidget((Widget)spin) == (Widget)spin) {
	    rect[0].x = rect[1].x = rect[2].x = 0;
	    rect[3].x = XtWidth(spin) - SPIN_MARGIN_W(spin);
	    rect[0].y = rect[2].y = rect[3].y = 0 ;
	    rect[1].y = XtHeight(spin) - SPIN_MARGIN_H(spin);
	    rect[0].width = rect[1].width = XtWidth(spin);
	    rect[2].width = rect[3].width = SPIN_MARGIN_W(spin);
	    rect[0].height = rect[1].height = SPIN_MARGIN_H(spin);
	    rect[2].height = rect[3].height = XtHeight(spin);
	    XFillRectangles(XtDisplayOfObject((Widget)spin),
			    XtWindowOfObject((Widget)spin), 
			    spin->manager.highlight_GC, rect, 4);
	}
    }
}


/*
 * osfUp virtual key hit.  Simulate hitting the up arrow.
 */
static void 
_SpinBoxUp(	DtSpinBoxWidget spin,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
    if (*num_params != 0) /* params means label or arrows */
	spin = (DtSpinBoxWidget)XtParent(spin);

    if (ArrowLayout(spin) != DtARROWS_SPLIT) {
	up_cb((Widget)UpArrow(spin), (XtPointer)spin, NULL);
	disarm_cb((Widget)UpArrow(spin), (XtPointer)spin, NULL);
    }
}

/*
 * osfDown virtual key hit.  Simulate hitting the down arrow.
 */
static void 
_SpinBoxDown(DtSpinBoxWidget spin,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
    if (*num_params != 0) /* params means label or arrows */
	spin = (DtSpinBoxWidget)XtParent(spin);

    if (ArrowLayout(spin) != DtARROWS_SPLIT) {
	down_cb((Widget)DownArrow(spin), (XtPointer)spin, NULL);
	disarm_cb((Widget)DownArrow(spin), (XtPointer)spin, NULL);
    }
}

/*
 * osfRight virtual key hit.  Simulate hitting the up arrow.
 */
static void 
_SpinBoxRight(DtSpinBoxWidget spin,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
    if (*num_params != 0) /* params means label or arrows */
	spin = (DtSpinBoxWidget)XtParent(spin);

    if (ArrowLayout(spin) == DtARROWS_SPLIT) {
	up_cb((Widget)UpArrow(spin), (XtPointer)spin, NULL);
	disarm_cb((Widget)UpArrow(spin), (XtPointer)spin, NULL);
    }
}

/*
 * osfLeft virtual key hit.  Simulate hitting the down arrow.
 */
static void 
_SpinBoxLeft(DtSpinBoxWidget spin,
		XEvent *event,
		char **params,
		Cardinal *num_params)
{
    if (*num_params != 0) /* params means label or arrows */
	spin = (DtSpinBoxWidget)XtParent(spin);

    if (ArrowLayout(spin) == DtARROWS_SPLIT) {
	down_cb((Widget)DownArrow(spin), (XtPointer)spin, NULL);
	disarm_cb((Widget)DownArrow(spin), (XtPointer)spin, NULL);
    }
}

/*
 * osfBeginLine virtual key hit.  Go to first item.
 */
static void 
_SpinBoxBeginLine(	DtSpinBoxWidget spin,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    int new_position;
    float new_current;
    
    if (*num_params != 0) /* params means label or arrows */
	spin = (DtSpinBoxWidget)XtParent(spin);

    if (ChildType(spin) == DtNUMERIC) {
	new_position = Minimum(spin);
	new_current = Min(spin);
    }
    else {
	new_position = 0;
    }
    if (SendCallback(spin, event, FALSE, new_position,
		     new_current, FALSE) == TRUE) {
	/* User said yes, so set widget values */
	Position(spin) = new_position;
	Current(spin) = new_current;
	if (Editable(spin))
	    SetTextFieldData(spin);
	else
	    SetLabelData(spin);
	
	/* send value_changed callback */
	(void)SendCallback(spin, event, TRUE, Position(spin),
			   Current(spin), FALSE);
    }
}

/*
 * osfEndLine virtual key hit.  Go to last item.
 */
static void 
_SpinBoxEndLine(	DtSpinBoxWidget spin,
			XEvent *event,
			char **params,
			Cardinal *num_params)
{
    int new_position;
    float new_current;
    
    if (*num_params != 0) /* params means label or arrows */
	spin = (DtSpinBoxWidget)XtParent(spin);

    if (ChildType(spin) == DtNUMERIC) {
	new_position = Maximum(spin);
	new_current = Max(spin);
    }
    else {
	new_position = ItemCount(spin) - 1;
    }
    if (SendCallback(spin, event, FALSE, new_position,
		     new_current, FALSE) == TRUE) {
	/* User said yes, so set widget values */
	Position(spin) = new_position;
	Current(spin) = new_current;
	if (Editable(spin))
	    SetTextFieldData(spin);
	else
	    SetLabelData(spin);
	
	/* send value_changed callback */
	(void)SendCallback(spin, event, TRUE, Position(spin),
			   Current(spin), FALSE);
    }
}

/*
 * USL: Get Focus for SpinBox when hit its label part
 */
static void
_SpinBoxGetFocus(     DtSpinBoxWidget spin,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params)
{
    XmProcessTraversal((Widget)XtParent(spin),
                        (XmTraversalDirection) XmTRAVERSE_CURRENT);
}

/*
 * USL: Process Focus Traversal for SpinBox when cursor is in its arrow part
 */
static void
_SpinBoxPrevTabGroup(DtSpinBoxWidget spin,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params)
{
    XmProcessTraversal((Widget)XtParent(spin),
                        (XmTraversalDirection) XmTRAVERSE_PREV_TAB_GROUP);
}

static void
_SpinBoxNextTabGroup(DtSpinBoxWidget spin,
                        XEvent *event,
                        char **params,
                        Cardinal *num_params)
{
    XmProcessTraversal((Widget)XtParent(spin),
                        (XmTraversalDirection) XmTRAVERSE_NEXT_TAB_GROUP);
}

/*
 * This function goes through most of the resources and makes sure 
 * they have legal values.
 */
static void
CheckResources(DtSpinBoxWidget spin)
{
    if (!XmRepTypeValidValue(_DtRID_SB_ARROW_SENSITIVITY_TYPE,
			     ArrowSensitivity(spin), (Widget)spin)) {
	XtWarning(SB_ARROW_SENSITIVE);
	ArrowSensitivity(spin) = DtARROWS_SENSITIVE;
    }
    if ((Alignment(spin) != DtALIGNMENT_CENTER) && 
	(Alignment(spin) != DtALIGNMENT_BEGINNING) &&
	(Alignment(spin) != DtALIGNMENT_END)) {
	XtWarning(SB_ALIGNMENT);
	Alignment(spin) = DtALIGNMENT_CENTER;
    }
    if (InitialDelay(spin) == 0) {
	XtWarning(SB_INIT_DELAY);
	InitialDelay(spin) = 250;
    }
    if ((ArrowLayout(spin) != DtARROWS_FLAT_BEGINNING) && 
	(ArrowLayout(spin) != DtARROWS_FLAT_END) &&
	(ArrowLayout(spin) != DtARROWS_SPLIT) &&
	(ArrowLayout(spin) != DtARROWS_BEGINNING) &&
	(ArrowLayout(spin) != DtARROWS_END)) {
	XtWarning(SB_ARROW_LAYOUT);
	ArrowLayout(spin) = DtARROWS_BEGINNING;
    }
    if (RepeatDelay(spin) == 0) {
	XtWarning(SB_REPEAT_DELAY);
	RepeatDelay(spin) = 200;
    }
    if (ItemCount(spin) < 0) {
	XtWarning(SB_ITEM_COUNT);
	ItemCount(spin) = 0;
    }
    if ((ChildType(spin) == DtSTRING) &&
	((Position(spin) < 0) ||
	 ((Position(spin) >= ItemCount(spin)) &&
	  (ItemCount(spin) > 0)))) {
	XtWarning(SB_POSITION_STRING);
	Position(spin) = 0;
    }
    if ((DecimalPoints(spin) < 0) ||
	(DecimalPoints(spin) > MAX_FLOAT_DECIMALS)) {
	XtWarning(SB_DECIMAL_POINTS);
	DecimalPoints(spin) = 0;
    }
    if (Minimum(spin) > Maximum(spin)) {
	XtWarning(SB_MIN_MAX);
	Minimum(spin) = Maximum(spin);
    }
    if ((ChildType(spin) == DtNUMERIC) &&
	((Position(spin) < Minimum(spin)) ||
	 (Position(spin) > Maximum(spin)) ||
	 ((Position(spin) % NumericIncrement(spin)) != 0))) {
	XtWarning(SB_POSITION_NUMERIC);
	Position(spin) = Minimum(spin);
    }
}


/*
 * Destroy procedure called by the toolkit.
 */
static void 
Destroy(DtSpinBoxWidget spin)
{
    int i;

    if ((int)Timer(spin))
      {
	XtRemoveTimeOut(Timer(spin));
	Timer(spin) = (XtIntervalId)NULL;
      }

    if (ItemCount(spin)>0) {
	for (i = 0; i < ItemCount(spin); i++) {
	    XmStringFree((Items(spin))[i]);
	    }
	XtFree((char*)(Items(spin)));
    }

    /*
     * Don't remove callbacks and event handlers set on the children,
     * this has already been done by Xt (children are cleaned up before
     * the parent).
     */
}


/*
 * Resize function called by toolkit.  The size of our spin-box
 * has already been changed.  That is why we must store 
 * old_width and old_height.
 */
static void
Resize(DtSpinBoxWidget spin)
{
    ClearShadow(spin, TRUE);
    LayoutChildren(spin);
    DrawShadow(spin);
    OldWidth(spin) = spin->core.width;
    OldHeight(spin) = spin->core.height;
}


/*
 * Redisplay function called by toolkit. The widget didn't change size, 
 * so just redisplay the shadow.
 */
static void
Redisplay(	DtSpinBoxWidget w,
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
 * our SetSpinBoxSize() and LayoutChildren() figure out what size will
 * be allowed.
 * Returning GeometryDone was suppose to tell the toolkit
 * that we resized the child ourselves, but the text-field had trouble
 * with this (its' geometry_manager wasn't called or working right?), so
 * we return GeometryYes.
 */
static XtGeometryResult
GeometryManager(Widget w, /* child */
		XtWidgetGeometry *request,
		XtWidgetGeometry *reply)
{
    /* MotifBc */
    DtSpinBoxWidget spin = (DtSpinBoxWidget)XtParent(w);
    /* Resolution Independent */
    Arg args[4];
    Dimension width, height, border_width;
    int n;
    unsigned char unit_type;

    /* Ignore everything but text-field */
    if (w != Text(spin) ) {
	return(XtGeometryNo);
    }

    /* Only allow width/height changes */
    if (!(request->request_mode & (CWWidth | CWHeight)))
	return(XtGeometryNo);
    
    /* Resolution Independent */
    XtSetArg(args[0], XmNunitType, &unit_type);
    XtGetValues(w, args, 1);
    if ( unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(w, args, 1);
    }
    n = 0;
    /* Set the text-field to the requested size */
    if (request->request_mode & CWWidth) {
	/* MotifBc */
	XtSetArg(args[n], XmNwidth, &width); n++;
    }
    if (request->request_mode & CWHeight) {
	/* MotifBc */
	XtSetArg(args[n], XmNheight, &height); n++;
    }
    /* MotifBc */
    XtSetArg(args[n], XmNborderWidth, &border_width); n++;
    XtGetValues(w, args, n);
    XtResizeWidget(w, width, height, border_width);
    if ( unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(w, args, 1);
    }
    
    ClearShadow(spin, TRUE);
    if (RecomputeSize(spin))
	SetSpinBoxSize(spin);
    LayoutChildren(spin);
    DrawShadow(spin);
    return(XtGeometryDone);
}

/* 
 * This function sets the size of the spin_box widget based on the
 * current size of the children.  Don't worry if it doesn't work, the
 * children will be squeezed in later.
 */
static void
SetSpinBoxSize(DtSpinBoxWidget spin)
{
    Widget text_holder = Editable(spin) ? Text(spin) : Label(spin);
    Dimension shadow = SPIN_SHADOW(spin) * 2;
    Dimension arrow_width, arrow_height;
    /* MotifBc & Resolution Independent */
    Dimension height, width;
    Arg args[2];
    unsigned char unit_type = MUnitType(spin);


    /* Resolution Independent */
    if (unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues((Widget)spin, args, 1);
	XtSetValues(text_holder, args, 1);
    }
    /* MotifBc */
    XtSetArg(args[0], XmNheight, &height);
    XtSetArg(args[1], XmNwidth, &width);
    XtGetValues(text_holder, args, 2);
    /* 
     * Find out how big the arrow can be (needed to get 
     * width for text_holder).
     */
    arrow_width = (Dimension)((float)height * ARROW_MULT);
    arrow_width = (arrow_width < ARROW_MIN) ? ARROW_MIN : arrow_width;

    /* Get height based on arrow width */
    arrow_height = arrow_width;
    if ((ArrowLayout(spin) == DtARROWS_BEGINNING) ||
	(ArrowLayout(spin) == DtARROWS_END))
	arrow_height += arrow_height;

    /* Make height bigger of 2 - arrows vs text_holder */
    if (arrow_height > (Dimension)height)
	height = arrow_height;

    /* If not stacked add extra width for arrows */
    if ((ArrowLayout(spin) != DtARROWS_BEGINNING) &&
	(ArrowLayout(spin) != DtARROWS_END)) {
	arrow_width += arrow_width;
    }

    (void)XtMakeResizeRequest((Widget)spin, arrow_width +
			      width + shadow +
			      (SPIN_MARGIN_W(spin) * 2), 
			      height + shadow + (SPIN_MARGIN_H(spin) * 2), 
			      NULL, NULL);
    OldWidth(spin) = Width(spin);
    OldHeight(spin) = Height(spin);

    /* Resolution Independent */
    if (unit_type != XmPIXELS) { 
        XtSetArg(args[0], XmNunitType, unit_type); 
        XtSetValues((Widget)spin, args, 1);
        XtSetValues(text_holder, args, 1);
    } 
}

/*
 * This function makes the text_holder (label or text-field) smaller
 * if the spin_box couldn't grow to the needed full size.  It will
 * also make the text_holder grow if there is space.  The textfield will
 * grow with the spin_box, but the label will only grow to its' 
 * maximum size.  The label will also shrink down to nothing, but the
 * text-field will always keep its' core height.
 */
static void
ForceChildSizes(DtSpinBoxWidget spin)
{
    Dimension available_height, available_width, arrow_width;
    /* MotifBc & Resolution Independent */
    Arg args[3];
    unsigned char unit_type = XmPIXELS;

    /* Resolution Independent */
    if (MUnitType(spin) != XmPIXELS) {
	unit_type = MUnitType(spin);
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(UpArrow(spin), args, 1);
	XtSetValues(DownArrow(spin), args, 1);
	if (Editable(spin) == False) 
		XtSetValues(Label(spin), args, 1);
	else
		XtSetValues(Text(spin), args, 1);
    }

    CalculateSizes(spin, &available_width, &available_height, &arrow_width);

    if (Editable(spin) == FALSE) {  /** label **/
	if (available_width > LabelMaxLength(spin))
	    available_width = LabelMaxLength(spin);

	if ((available_width != (Label(spin))->core.width) ||
	    (available_height != (Label(spin))->core.height))
	    XtResizeWidget(Label(spin), available_width, available_height,
			   (Label(spin))->core.border_width);
    }
    else if ((Text(spin))->core.width != available_width)  /** TextField **/
	XtResizeWidget(Text(spin), available_width,
		       (Text(spin))->core.height,
		       (Text(spin))->core.border_width);
    if ((arrow_width != UpArrow(spin)->core.width) ||
	((UpArrow(spin))->core.height != arrow_width)) {
	available_height = (available_height < ARROW_MIN) ? ARROW_MIN : 
                            available_height;
	XtResizeWidget(UpArrow(spin), arrow_width, arrow_width,
		       (UpArrow(spin))->core.border_width);
    }
    if ((arrow_width != (DownArrow(spin))->core.width) ||
	((DownArrow(spin))->core.height != arrow_width)) {
	available_height = (available_height < ARROW_MIN) ? ARROW_MIN : 
                            available_height;
	XtResizeWidget(DownArrow(spin), arrow_width, arrow_width,
		       DownArrow(spin)->core.border_width);
    }
    /* Resolution Independent */
    if (MUnitType(spin) != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(UpArrow(spin), args, 1);
	XtSetValues(DownArrow(spin), args, 1);
	if (Editable(spin) == False) 
		XtSetValues(Label(spin), args, 1);
	else
		XtSetValues(Text(spin), args, 1);
    }
}

static void	
CalculateSizes (DtSpinBoxWidget spin,
		Dimension *pwidth,
		Dimension *pheight,
		Dimension *parrow_width)
{
    Dimension width, height, arrow_width;

    /* Calculate available height for children */
    if ((height = spin->core.height - (SPIN_SHADOW(spin) * 2) - 
	 (SPIN_MARGIN_H(spin) * 2)) <= 0)
	height = 1;

    /* Get initial available width for children */
    width = (spin->core.width - (SPIN_SHADOW(spin) * 2) - 
		       (SPIN_MARGIN_W(spin) * 2));

    /* label only grows to maximum width needed */
    if ((Editable(spin) == FALSE) && 
	(height > LabelMaxHeight(spin)))
	height = LabelMaxHeight(spin);
    else if (Editable(spin)) 
	height = (Text(spin))->core.height;
    
    /* 
     * Find out how big the arrow can be (needed to get 
     * width for text_holder).
     */
    arrow_width = (Dimension)(height * ARROW_MULT);
    arrow_width = (arrow_width < ARROW_MIN) ? ARROW_MIN : arrow_width;
	
    /* Make sure width isn't too small or too big */
    if ((width -= arrow_width) <= 0)
	width = 1;

    /* If not stacked subtract extra arrow */
    if ((ArrowLayout(spin) != DtARROWS_BEGINNING) &&
	(ArrowLayout(spin) != DtARROWS_END)) {
	width -= arrow_width;
    }

    if (pwidth) *pwidth = width;
    if (pheight) *pheight = height;
    if (parrow_width) *parrow_width = arrow_width;
}

/*
 * This function positions the children within the spin_box widget.
 * It calls ForceChildSizes() to make sure the children fit within the
 * spin_box widget, but it will not try to resize the spin_box widget.
 */
static void
LayoutChildren(DtSpinBoxWidget spin)
{
    Widget text_holder = Editable(spin) ? Text(spin) : Label(spin);
    Position start_x = SPIN_SHADOW(spin) + SPIN_MARGIN_W(spin);
    Position start_y = SPIN_SHADOW(spin) + SPIN_MARGIN_H(spin);
    Dimension available_height = spin->core.height - (start_y * 2);
    Position y, arrow_y;
    Dimension arrow_height;
    /* Resolution Independent */
    Arg args[4];
    unsigned char unit_type = XmPIXELS;
    
    ForceChildSizes(spin);

    /* Resolution Independent */
    if (MUnitType(spin) != XmPIXELS) {
	unit_type = MUnitType(spin);
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(UpArrow(spin), args, 1);
	XtSetValues(DownArrow(spin), args, 1);
	XtSetValues(text_holder, args, 1);
    }

    /* Center text_holder within spin_box */
    y = available_height - text_holder->core.height;
    y = ((y < 0) ? 0 : y)/2 + start_y;

    arrow_height = (UpArrow(spin))->core.height;
    if ((ArrowLayout(spin) == DtARROWS_BEGINNING) ||
	(ArrowLayout(spin) == DtARROWS_END))
	arrow_height += arrow_height;
    arrow_y = available_height - arrow_height;
    arrow_y = ((arrow_y < 0) ? 0 : arrow_y)/2 + start_y;

    switch (ArrowLayout(spin)) {
    case DtARROWS_FLAT_BEGINNING:
	XtMoveWidget(UpArrow(spin), start_x, arrow_y);
	start_x += (UpArrow(spin))->core.width;
	XtMoveWidget(DownArrow(spin), start_x, arrow_y);
	start_x += (DownArrow(spin))->core.width;
	XtMoveWidget(text_holder, start_x, y);
	break;
    case DtARROWS_FLAT_END:
	XtMoveWidget(text_holder, start_x, y);
	/*  
	 * This start_x places arrow right after text_holder.  With
	 * labels we want the arrow at the end of the spin_box, so
	 * the user can use recompute_size more effectively.
	 * start_x += text_holder->core.width;
	 */
	start_x = (spin->core.width - start_x - 
		   (UpArrow(spin))->core.width -
		   (DownArrow(spin))->core.width);
	XtMoveWidget(UpArrow(spin), start_x, arrow_y);
	start_x += (UpArrow(spin))->core.width;
	XtMoveWidget(DownArrow(spin), start_x, arrow_y);
	break;
    case DtARROWS_SPLIT:
	XtMoveWidget(DownArrow(spin), start_x, arrow_y);
	start_x += (DownArrow(spin))->core.width;
	XtMoveWidget(text_holder, start_x, y);
	start_x += text_holder->core.width;
	XtMoveWidget(UpArrow(spin), start_x, arrow_y);
	break;
    case DtARROWS_BEGINNING:
	XtMoveWidget(UpArrow(spin), start_x, arrow_y);
	arrow_y += (UpArrow(spin))->core.height;
	XtMoveWidget(DownArrow(spin), start_x, arrow_y);
	start_x += (DownArrow(spin))->core.width;
	XtMoveWidget(text_holder, start_x, y);
	break;
    case DtARROWS_END:
	XtMoveWidget(text_holder, start_x, y);
	/*  
	 * This start_x places arrow right after text_holder.  With
	 * labels we want the arrow at the end of the spin_box, so
	 * the user can use recompute_size more effectively.
	 * start_x += text_holder->core.width;
	 */
	start_x = spin->core.width - start_x - (UpArrow(spin))->core.width;
	XtMoveWidget(UpArrow(spin), start_x, arrow_y);
	arrow_y += (UpArrow(spin))->core.width;
	XtMoveWidget(DownArrow(spin), start_x, arrow_y);
	break;
    }
    /* Resolution Independent */
    if (MUnitType(spin) != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(UpArrow(spin), args, 1);
	XtSetValues(DownArrow(spin), args, 1);
	XtSetValues(text_holder, args, 1);
    }
}


/*
 * SetValues() routine for SpinBox widget. Most of the real work
 * is done in SetItems() and SetNumeric().  If items is set we store
 * our own XmStrings. 
 * This function was built with static spin_boxs in mind, meaning that
 * is-numeric or editable resources won't be changed constantly.  These
 * resources can be changed, but this function doesn't handle them in
 * a super-efficient manor.  for example, changing child-type will cause
 * the label to be resize even if it isn't managed.
 */
static Boolean
SetValues(	DtSpinBoxWidget current,
		DtSpinBoxWidget request,
		DtSpinBoxWidget new)
{
    DtSpinBoxPart *new_p = (DtSpinBoxPart*)
	&(XmField(new,ipot,DtSpinBox,label,Widget));
    DtSpinBoxPart *cur_p = (DtSpinBoxPart*)
	&(XmField(current,ipot,DtSpinBox,label,Widget));
    Boolean store_info;
    char *widget_name;
    Boolean label_size_changed = FALSE;
    Arg args[20];
    int n;
    /* Resolution Independent */
    unsigned char unit_type = MUnitType(new);

    CheckResources(new);

    if (Text(new) != Text(current)) {
	XtWarning(SB_TEXT);
	Text(new) = Text(current);
    }

    /*
     * Editable resource changed.  If the widget (textField or Label)
     * doesn't exist, then create it.
     */
    if (Editable(new) != Editable(current)) {
	if (Editable(new)) {
	    XtUnmanageChild(Label(new));
	    if (Text(new) == NULL) {
		widget_name = XtMalloc(strlen(XtName((Widget)new)) + 10);
		sprintf(widget_name, "%s_TF", XtName((Widget)new));
		n = 0;
		XtSetArg(args[n], XmNcolumns, TextColumns(new)); n++;
		XtSetArg(args[n], XmNmaxLength, TextMaxLength(new)); n++;
		XtSetArg(args[n], XmNmarginWidth, 2); n++;
		XtSetArg(args[n], XmNmarginHeight, 2); n++;
		if (unit_type != XmPIXELS) {
			XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
		}
		Text(new) = XtCreateManagedWidget(widget_name,
						    xmTextFieldWidgetClass,
						    (Widget)new, args, n);
		XtAddCallback(Text(new), XmNlosingFocusCallback, 
			      text_losing_focus_cb, (XtPointer)new);
		XtAddCallback(Text(new), XmNactivateCallback, 
			      text_activate_cb, (XtPointer)new);
		XtAddCallback(Text(new), XmNfocusCallback, 
			      text_focus_cb, (XtPointer)new);
		XtFree(widget_name);
                if (unit_type != XmPIXELS) {
                        XtSetArg(args[n], XmNunitType, unit_type); n++;
			XtSetValues(Text(new), args, 1);
                }
	    }
	    else
		XtManageChild(Text(new));
	}
	else {
	    XtUnmanageChild(Text(new));
	    if (Label(new) == NULL) {
		widget_name = XtMalloc(strlen(XtName((Widget)new)) + 10);
		sprintf(widget_name, "%s_Label", XtName((Widget)new));
		n = 0;
		XtSetArg(args[n], XmNalignment, Alignment(new)); n++;
		XtSetArg(args[n], XmNrecomputeSize, FALSE); n++;
		XtSetArg(args[n], XmNlabelString, InitLabel); n++;
		XtSetArg(args[n], XmNmarginLeft, LABEL_PADDING); n++;
		XtSetArg(args[n], XmNmarginRight, LABEL_PADDING); n++;
		XtSetArg(args[n], XmNmarginWidth, TEXT_CONTEXT_MARGIN); n++;
		XtSetArg(args[n], XmNmarginHeight, 0); n++;
                if (unit_type != XmPIXELS) {
                        XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
                }
		Label(new) = XtCreateManagedWidget(widget_name,
						     xmLabelWidgetClass,
						     (Widget)new, args, n);
		XtOverrideTranslations((Widget)Label(new), child_trans);
		XtFree(widget_name);
                if (unit_type != XmPIXELS) { 
                        XtSetArg(args[n], XmNunitType, unit_type); n++; 
                        XtSetValues(Label(new), args, 1); 
                } 
	    }
	    else
		XtManageChild(Label(new));
	}
	/* 
	 * Text-fields and labels have different shadows.  Only
	 * change if user didn't change the shadow resource.
	 */
	if (SPIN_SHADOW(new) == SPIN_SHADOW(current))
	    SPIN_SHADOW(new) = (Editable(new)) ? TEXT_FIELD_SHADOW :
		                                   LABEL_SHADOW;
    }

    /* Check arrow sensitivity (up arrow is right arrow)*/
    if (ArrowSensitivity(new) != ArrowSensitivity(current)) {
	XtSetSensitive(UpArrow(new), 
	       ((ArrowSensitivity(new) == DtARROWS_SENSITIVE) ||
		(ArrowSensitivity(new) == DtARROWS_INCREMENT_SENSITIVE)));
	XtSetSensitive(DownArrow(new), 
	       ((ArrowSensitivity(new) == DtARROWS_SENSITIVE) ||
		(ArrowSensitivity(new) == DtARROWS_DECREMENT_SENSITIVE)));
    }

    /*
     * Check arrow layout.  Only need to change arrows if going to or
     * from DtARROWS_SPLIT.  The LayoutChildren() routine actually
     * positions the arrows in the correct place.
     */
    if (ArrowLayout(new) != ArrowLayout(current)) {
	if (ArrowLayout(new) == DtARROWS_SPLIT) {
	    XtSetArg(args[0], XmNarrowDirection, XmARROW_RIGHT);
	    XtSetValues(UpArrow(new), args, 1);
	    XtSetArg(args[0], XmNarrowDirection, XmARROW_LEFT);
	    XtSetValues(DownArrow(new), args, 1);
	}
	else {
	    XtSetArg(args[0], XmNarrowDirection, XmARROW_UP);
	    XtSetValues(UpArrow(new), args, 1);
	    XtSetArg(args[0], XmNarrowDirection, XmARROW_DOWN);
	    XtSetValues(DownArrow(new), args, 1);
	}
    }

    if (Text(new) && (Text(new) == Text(current))) {
	n = 0;
	if (TextColumns(new) != TextColumns(current)) {
	    XtSetArg(args[n], XmNcolumns, TextColumns(new)); n++;
	}
	if (TextMaxLength(new) != TextMaxLength(current)) {
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

    store_info = ((Items(new) != Items(current)) ||
		  (ItemCount(new) != ItemCount(current)) ||
		  (DecimalPoints(new) != DecimalPoints(current)) ||
		  (Maximum(new) != Maximum(current)) ||
		  (Minimum(new) != Minimum(current)) ||
		  (NumericIncrement(new) != NumericIncrement(current)) ||
		  ((ChildType(new) == DtNUMERIC) &&
		   (Position(new) != Position(current))));
    if (store_info)
	StoreResourceInfo(new_p, cur_p, (Items(new) != Items(current)));
    
    if (Label(new) && (store_info || 
			 (Label(new) != Label(current)) ||
			 (ChildType(new) != ChildType(current)))) {
	SetMaximumLabelSize(new_p);
	label_size_changed = TRUE;
    }
    
    if (store_info ||
	(Alignment(new) != Alignment(current)) ||
	(Editable(new) != Editable(current)) ||
	(Position(new) != Position(current)) ||
	(Label(new) != Label(current)) ||
	(ChildType(new) != ChildType(current))) {
	if (Editable(new))
	    SetTextFieldData(new);
	else
	    SetLabelData(new);
    }

    /*
     * Must recalculate the spin_box and re-layout the children.
     * If this is not editable, then set the label to its' maximum
     * size; it will get chopped if it is too big.  This is needed 
     * because we shrink the label down, and SetSpinBoxSize() uses
     * the label's core sizes to figure what size to become.
     */
    if ((Editable(new) != Editable(current)) ||
	(SPIN_MARGIN_W(new) != SPIN_MARGIN_W(current)) ||
	(SPIN_MARGIN_H(new) != SPIN_MARGIN_H(current)) ||
	(SPIN_SHADOW(new) != SPIN_SHADOW(current)) ||
	(ArrowLayout(new) != ArrowLayout(current)) ||
	(!(Editable(new)) && label_size_changed)) {
	ClearShadow(current, TRUE);
	if (RecomputeSize(new))
	    SetSpinBoxSize(new);
	LayoutChildren(new);
	DrawShadow(new);
    }
    n=0;
    if(CBgPixel(new) != CBgPixel(current))
    {
        XtSetArg(args[n],XmNbackground,CBgPixel(new));n++;
    }
    if(CBgPixmap(new) != CBgPixmap(current))
    {
        XtSetArg(args[n],XmNbackgroundPixmap,CBgPixmap(new));n++;
    }
    if(MFgPixel(new) != MFgPixel(current))
    {
        XtSetArg(args[n],XmNforeground,MFgPixel(new));n++;
    }
    if(Text(new))
        XtSetValues (Text(new),args,n);
    if(Label(new))
        XtSetValues (Label(new),args,n);
    if(UpArrow(new))
        XtSetValues (UpArrow(new),args,n);
    if(DownArrow(new))
        XtSetValues (DownArrow(new),args,n);
    
    return(FALSE);
}


/*
 * This function clears the shadow around our widget.  If all is TRUE,
 * then clear all 4 sides; otherwise, only clear the right and bottom
 * sides (during resize). 
 */ 
static void
ClearShadow(	DtSpinBoxWidget w,
		Boolean all)
{
    Dimension shadow = SPIN_SHADOW(w);
    Dimension margin_w = SPIN_MARGIN_W(w);
    Dimension margin_h = SPIN_MARGIN_H(w);

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
	XClearArea(XtDisplayOfObject((Widget)w), XtWindowOfObject((Widget)w),
		   margin_w, OldHeight(w) - margin_h - shadow,
		   OldWidth(w) - (margin_w * 2), shadow, FALSE);
	XClearArea(XtDisplayOfObject((Widget)w), XtWindowOfObject((Widget)w),
		   OldWidth(w) - margin_w - shadow,
		   margin_h, shadow, 
		   OldHeight(w) - (margin_h * 2), FALSE);
    }
    DrawHighlight(w, TRUE);
}

/* 
 * This functions draws the shadow around our spin_box.
 */
static void
DrawShadow(DtSpinBoxWidget w)
{
    Dimension shadow = SPIN_SHADOW(w);
    Dimension margin_w = SPIN_MARGIN_W(w);
    Dimension margin_h = SPIN_MARGIN_H(w);
    
    if ((shadow > 0) && XtIsRealized((Widget)w)) {
	XmeDrawShadows(XtDisplayOfObject((Widget)w),
		       XtWindowOfObject((Widget)w),
		       w->manager.top_shadow_GC,
		       w->manager.bottom_shadow_GC, 
		       margin_w, margin_h,
		       w->core.width - (margin_w * 2),
		       w->core.height - (margin_h * 2),
		       shadow, XmSHADOW_OUT);
    }
    DrawHighlight(w, FALSE);
}

/*
 * This function sets up the items information for the SpinBox, as
 * well as variables needed for child_type.
 */
static void
StoreResourceInfo(	DtSpinBoxPart *spin_p,
			DtSpinBoxPart *old_p,
			Boolean do_items)
{
    XmStringTable table;
    int i, base = 1;

    if (do_items && spin_p->items) {
	/* Free up current items if needed */
	if (old_p && old_p->items) {
	    for (i = 0; i < old_p->item_count; i++) {
		XmStringFree(old_p->items[i]);
	    }
	    XtFree((char*)old_p->items);
	}
	    
	/*
	 * Loop through all the items, copy them into our space.
	 */
	table = (XmStringTable)XtMalloc(sizeof(XmString) * spin_p->item_count);
	for (i = 0; i < spin_p->item_count; i++) {
	    table[i] = XmStringCopy(spin_p->items[i]);
	}
	spin_p->items = table;
	for (i = 0; i < spin_p->item_count; i++)
	    spin_p->items[i] = table[i];
    }

    /*
     * Store the numeric information
     */

    /* get base number for convert ints to floats */
    for (i = 0; i < spin_p->decimal_points; i++)
	base *= 10;

    /* Set new initial values */
    spin_p->min = (float)spin_p->minimum/base;
    spin_p->max = (float)spin_p->maximum/base;
    spin_p->increment = (float)spin_p->numeric_increment/base;

    spin_p->current = (float)spin_p->position/base;

    /* Create format string used to build correct XmString value */
    spin_p->float_format[0] = '%';
    sprintf((char*)(spin_p->float_format+1), ".%df", spin_p->decimal_points);
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

/*
 * Take the string out of the list and put it into the text-field.
 * text-fields don't handle xm-strings, so we must get the char*
 * out of it (only getting the first segment).  This is slower than
 * storing the text-strings (char*) ourselves, but that would take
 * up a lot of memory.  Since this setting happens during a user
 * action, speed isn't a problem.
 */
static void
SetTextFieldData(DtSpinBoxWidget spin)
{
    char string[NUMERIC_LENGTH];
    XmString xm_string;
    char *text;
    Arg arg;
    
    
    if (ChildType(spin) == DtNUMERIC) {
	sprintf(string, FloatFormat(spin), (float)(Current(spin)));
	XtSetArg(arg, XmNvalue, string);
	XtSetValues(Text(spin), &arg, 1);
    }
    else {
	if (ItemCount(spin) == 0){
	    XtSetArg(arg, XmNvalue, "");
	    XtSetValues(Text(spin), &arg, 1);
	    return;
	}
	else {
	    xm_string = (Items(spin))[Position(spin)];
	    if ((text = GetTextString(xm_string))) {
		XtSetArg(arg, XmNvalue, text);
		XtSetValues(Text(spin), &arg, 1);
		XtFree(text);
	    }
	}
    }
}

/*
 * Set the maximum size of the label, depending on the
 * characteristics of the list of items.  Not very efficient
 * if switching from numeric to non-numeric.
 */
static void
SetMaximumLabelSize(DtSpinBoxPart *spin_p)
{
    XmString xm_string;
    XmFontList font_list;
    char string[NUMERIC_LENGTH];
    Dimension width, height;
    Dimension longest = 0;
    Dimension highest = 0;
    Arg args[5];
    int i;
    /* Resolution Independent */
    unsigned char unit_type;
    
    /* Get font info from the widget */
    XtSetArg(args[0], XmNfontList, &font_list);
    XtSetArg(args[1], XmNunitType, &unit_type); /* resolution Independent */
    XtGetValues(spin_p->label, args, 2);
    if ( unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, XmPIXELS);
	XtSetValues(spin_p->label, args, 1);
    }

    if (spin_p->child_type == DtNUMERIC) {
	/* Find out maximum size of the widget from min/max */
	sprintf(string, spin_p->float_format, spin_p->min);
	xm_string = XmStringCreateLocalized(string);
	XmStringExtent(font_list, xm_string, &longest, &highest);
	XmStringFree(xm_string);
	sprintf(string, spin_p->float_format, spin_p->max);
	xm_string = XmStringCreateLocalized(string);
	XmStringExtent(font_list, xm_string, &width, &height);
	XmStringFree(xm_string);
	
	longest = (width > longest) ? width : longest;
	highest = (height > highest) ? height : highest;
    }
    else if (spin_p->items) {
	/*
	 * Loop through all the items to find the biggest dimensions
	 */
	for (i = 0; i < spin_p->item_count; i++) {
	    XmStringExtent(font_list, spin_p->items[i], &width, &height);
	    longest = (width > longest) ? width : longest;
	    highest = (height > highest) ? height : highest;
	}
    }
    else {
	XmStringExtent(font_list, InitLabel, &longest, &highest);
    }

    spin_p->label_max_length = 
	( (Dimension)(longest + ( (LABEL_PADDING + TEXT_CONTEXT_MARGIN) *2) ) >
	  (Dimension)Width(spin_p->label) ) ? 
	(longest + ((LABEL_PADDING+TEXT_CONTEXT_MARGIN) * 2)) :
	Width(spin_p->label);

    spin_p->label_max_height = highest > Height(spin_p->label) ?
	highest : Height(spin_p->label);
    XtResizeWidget(spin_p->label, spin_p->label_max_length, 
		   spin_p->label_max_height,
		   spin_p->label->core.border_width);

    /* Resolution Independent */
    if ( unit_type != XmPIXELS) {
	XtSetArg(args[0], XmNunitType, unit_type);
	XtSetValues(spin_p->label, args, 1);
    }
}


/*
 * Put the current list item into the label.
 */
static void
SetLabelData(DtSpinBoxWidget spin)
{
    XmString xm_string;
    char string[NUMERIC_LENGTH];
    int index = Position(spin);
    Arg arg;

    if (ChildType(spin) == DtNUMERIC) {
	sprintf(string, FloatFormat(spin), (float)Current(spin));
	xm_string = XmStringCreateLocalized(string);
	XtSetArg(arg, XmNlabelString, xm_string);
	XtSetValues(Label(spin), &arg, 1);
    }
    else {
	/*
	 * If the item is not empty, get the current item from the list, else
	 * use InitLabel.
	 */
	xm_string = (ItemCount(spin)>0) ? (Items(spin))[index] : InitLabel;
	XtSetArg(arg, XmNlabelString, xm_string);
	XtSetValues(Label(spin), &arg, 1);
    }
}

/*
 * Timout dispatch routine.  This calls the appropriate callback function
 * to simulate up or down arrow activation.
 */
static void
timer_dispatch(	XtPointer client_data,
		XtIntervalId *id)
{
    DtSpinBoxWidget spin_w = (DtSpinBoxWidget)client_data;

    Timer(spin_w) = (XtIntervalId)NULL;
    InitCb(spin_w) = FALSE;
    if (WhichArrow(spin_w) == XmARROW_UP) {
	if (Grabbed(spin_w)) {
	    XtRemoveGrab(UpArrow(spin_w));
	    Grabbed(spin_w) = FALSE;
	}
	up_cb(NULL, client_data, NULL);
    }
    else {
	if (Grabbed(spin_w)) {
	    XtRemoveGrab(DownArrow(spin_w));
	    Grabbed(spin_w) = FALSE;
	}
	down_cb(NULL, client_data, NULL);
    }
}

static void
TextFieldActivate(DtSpinBoxPart *spin_p)
{
    XmTextFieldWidget w = (XmTextFieldWidget)(spin_p->text);
    XmAnyCallbackStruct cb;
    char string[NUMERIC_LENGTH];
    char *data = NULL;
    char *text = NULL;
    Arg arg;
    Boolean free_me = TRUE;
    
    XtSetArg(arg, XmNvalue, &data);
    XtGetValues((Widget)w, &arg, 1);

    if (spin_p->child_type == DtNUMERIC) {
	sprintf(string, spin_p->float_format, (float)spin_p->current);
	text = string;
	free_me = FALSE;
    }
    else if (spin_p->items)
	text = GetTextString(spin_p->items[spin_p->position]);

    if (text && data && (strcmp(text, data) == 0)) {
	if (free_me)
	    XtFree(text);
	return;
    }
    /* Only send callback if both are not NULL */
    else if (!((text == NULL) && (data == NULL))) {
	cb.reason = XmCR_ACTIVATE;
	cb.event  = NULL;
	/* MotifBc */
        if (XtHasCallbacks((Widget)w, XmNactivateCallback)==XtCallbackHasSome)
               XtCallCallbacks((Widget)w, XmNactivateCallback, 
                               (XtPointer) &cb);
	if (text && free_me)
	    XtFree(text);
    }
}

/*
 * This function calls the appropriate callback for the spin_box.
 * It gathers the correct arguments and fills in the callback structure.
 */
static Boolean
SendCallback(	DtSpinBoxWidget spin,
		XEvent *event,
		Boolean value_changed,
		int position,
		float current, /* Used for numeric */
		Boolean crossed)
{
    DtSpinBoxCallbackStruct cb;
    XmString xm_string = NULL;
    char string[NUMERIC_LENGTH];

    if (ChildType(spin) == DtNUMERIC) {
	sprintf(string, FloatFormat(spin), (float)current);
	xm_string = XmStringCreateLocalized(string);
    }
    else {
	xm_string = (ItemCount(spin)>0) ? (Items(spin))[position] : InitLabel;
	xm_string = XmStringCopy(xm_string);
    }

    if (event)
	cb.reason = DtCR_OK;
    else if (WhichArrow(spin) == XmARROW_UP)
	cb.reason = DtCR_SPIN_NEXT;
    else
	cb.reason = DtCR_SPIN_PRIOR;
    cb.doit = TRUE;
    cb.event = event;
    cb.widget = (Widget)spin;
    cb.position = position;
    cb.value = xm_string;
    cb.crossed_boundary = crossed;
    if (value_changed) {
	XtCallCallbackList((Widget)spin, ValueChangedCallback(spin), 
			   (XtPointer)&cb);
	cb.doit = TRUE;
    }
    else {
	XtCallCallbackList((Widget)spin, ModifyVerifyCallback(spin), 
			   (XtPointer)&cb);
    }
    XmStringFree(xm_string);

    return(cb.doit);
}


/*
 * This function gets called by the up/down arm callback functions.
 * We set up the timer and send the modify-verify and value-changed
 * callbacks. 
 * There are potential problems if the user does some weird stuff 
 * in the callbacks.  I have added protection against the case where
 * a user does a grab (with XtAddGrab/XtPopup/etc.) in the callbacks.
 * Grabbing in the callback would cause us to lose the button-release
 * (disarm), which would make the timer go on forever.  A grab is
 * done after the callbacks just to make sure we will receive the
 * button-release.  The button-release will call disarm_cb() which will
 * un-grab and disable the timer.  I have also added a leave callback
 * which helps to protect against these kinds of problems.  
 * If the callback goes into another event-loop (which I hope would
 * never happen), we would spin continuously (since our XtAddGrab never
 * get called), until the user left the window (which would call 
 * grab_leave_cb).  The grabbed flag gets set if we do the grab, so that
 * we know if we can remove the grab.  Our XtAddGrab() might not get called
 * if the callback enters another event-loop.
 *
 * The event sent in the callback will be NULL during continuous spinning.
 */
static void
FinishUpDown(	DtSpinBoxWidget spin,
		XtPointer arrow_call_data,
		int new_position,
		float new_current,
		Boolean crossed)
{
    XmArrowButtonCallbackStruct *arrow_data;
    XEvent *last_event = NULL;
    int repeat_delay = RepeatDelay(spin);

    if (InitCb(spin))    
	repeat_delay = InitialDelay(spin);
    Timer(spin) = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)spin),
			    repeat_delay, timer_dispatch, (XtPointer)spin);

    /* Try to get Xevent */
    if ((arrow_data = (XmArrowButtonCallbackStruct*)arrow_call_data))
	last_event = arrow_data->event;

    /* 
     * Send modify_verify callback.  If user says no, then
     * clear the timer and reset the state before returning.
     */
    if (SendCallback(spin, last_event, FALSE, new_position,
		     new_current, crossed) == FALSE) {
	XtRemoveTimeOut(Timer(spin));
	Timer(spin) = (XtIntervalId)NULL;
	InitCb(spin) = TRUE;
	return;
    }

    /* User said yes, so set widget values */
    Position(spin) = new_position;
    Current(spin) = new_current;
    if (Editable(spin))
	SetTextFieldData(spin);
    else
	SetLabelData(spin);

    /* send value_changed callback */
    (void)SendCallback(spin, last_event, TRUE, Position(spin),
		       Current(spin), crossed);

    /* See notes at top of function on XtAddGrab usage */
    Grabbed(spin) = TRUE;
    if (WhichArrow(spin) == XmARROW_UP)
	XtAddGrab(UpArrow(spin), FALSE, FALSE);
    else
	XtAddGrab(DownArrow(spin), FALSE, FALSE);
}

/*
 * Show the next value in the SpinBox.  If numeric, just add the
 * increment value.  If using string-table, get the next one in the
 * table.  This function takes care of wrap around.  Set the arrow
 * type.  This is needed for the timer_dispatch function.  up_cb
 * gets called the first time the button is pressed, and each time the
 * timer goes off.
 * All widget internals are expected to be correct here; they
 * get verified when set by the user.
 */
static void
up_cb(	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;
    DtSpinBoxPart *spin_p = (DtSpinBoxPart*)
	&(XmField(spin,ipot,DtSpinBox,label,Widget));
    Boolean crossed_boundary = FALSE;
    int new_position = Position(spin);
    float new_current = Current(spin);

   /* Getting Focus */
    if ( !_XmFocusIsHere( (Widget)spin) )
        XmProcessTraversal((Widget)spin,
			  (XmTraversalDirection) XmTRAVERSE_CURRENT);

    if (Editable(spin))
	TextFieldActivate(spin_p);

    /*
     * If non-numeric and no items then ignore the user activate event.
     */
    if ((ChildType(spin) == DtSTRING) && (ItemCount(spin) == 0))
	return;
    
    if (ChildType(spin) == DtNUMERIC) {
	if ((new_current + Increment(spin)) > Max(spin)) {
	    if (Wrap(spin)) {
		new_position = Minimum(spin);
		new_current = Min(spin);
		crossed_boundary = TRUE;
	    }
	    else
		XBell(XtDisplayOfObject((Widget)spin), 0);
	}
	else {
	    new_position += NumericIncrement(spin);
	    new_current += Increment(spin);
	}
    }
    else if (ItemCount(spin) >0) {
	if (new_position == (ItemCount(spin) - 1)) {
	    if (Wrap(spin)) {
		new_position = 0;
		crossed_boundary = TRUE;
	    }
	    else
		XBell(XtDisplayOfObject((Widget)spin), 0);
	}
	else
	    new_position++;
    }

    WhichArrow(spin) = XmARROW_UP;
    FinishUpDown(spin, call_data, new_position, new_current, crossed_boundary);
}


/*
 * Show the previous value in the SpinBox.  If numeric, just decrement
 * the increment value.  If using string-table, get the previous one in the
 * table.  This function takes care of wrap around. Set the arrow
 * type.  This is needed for the timer_dispatch function.  down_cb
 * gets called the first time the button is pressed, and each time the
 * timer goes off.
 * All widget internals are expected to be correct here; they
 * get verified when set by the user.
 */
static void
down_cb(Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;
    DtSpinBoxPart *spin_p = (DtSpinBoxPart*)
	&(XmField(spin,ipot,DtSpinBox,label,Widget));
    Boolean crossed_boundary = FALSE;
    int new_position = Position(spin);
    float new_current = Current(spin);
    
   /* #5: Getting Focus */
    if ( !_XmFocusIsHere( (Widget)spin) )
      XmProcessTraversal((Widget)spin,
			 (XmTraversalDirection) XmTRAVERSE_CURRENT);

    if (Editable(spin))
	TextFieldActivate(spin_p);

    /*
     * If non-numeric and no items then ignore the user activate event.
     */
    if ((ChildType(spin) == DtSTRING) && (ItemCount(spin) == 0))
	return;

    if (ChildType(spin) == DtNUMERIC) {
	if ((new_current - Increment(spin)) < Min(spin)) {
	    if (Wrap(spin)) {
		new_current = Max(spin);
		new_position = Maximum(spin);
		crossed_boundary = TRUE;
	    }
	    else
		XBell(XtDisplayOfObject((Widget)spin), 0);
	}
	else {
	    new_current -= Increment(spin);
	    new_position -= NumericIncrement(spin);
	}
    }
    else if (ItemCount(spin)>0) {
	if (new_position == 0) {
	    if (Wrap(spin)) {
		new_position = ItemCount(spin) - 1;
		crossed_boundary = TRUE;
	    }
	    else
		XBell(XtDisplayOfObject((Widget)spin), 0);
	}
	else
	    new_position--;
    }

    WhichArrow(spin) = XmARROW_DOWN;
    FinishUpDown(spin, call_data, new_position, new_current, crossed_boundary);
}

static void
disarm_cb(	Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;

    if ((int)Timer(spin)) {
	InitCb(spin) = TRUE;
	XtRemoveTimeOut(Timer(spin));
	Timer(spin) = (XtIntervalId)NULL;
	if (Grabbed(spin)) {
	    XtRemoveGrab(w);
	    Grabbed(spin) = FALSE;
	}
    }
}

static void
grab_leave_cb(	Widget w,
		XtPointer client_data,
		XEvent *event,
		Boolean *dispatch)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;

    if ((int)Timer(spin) && 
	((event->xcrossing.mode == NotifyGrab) ||
	 (event->xcrossing.mode == NotifyUngrab) ||
	 (!(event->xcrossing.state & Button1Mask)))) {
	InitCb(spin) = TRUE;
	XtRemoveTimeOut(Timer(spin));
	Timer(spin) = (XtIntervalId)NULL;
	if (Grabbed(spin)) {
	    XtRemoveGrab(w);
	    Grabbed(spin) = FALSE;
	}
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
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;

    if (LosingFocusCallback(spin))
	XtCallCallbackList((Widget)spin, 
			   LosingFocusCallback(spin), 
			   (XtPointer)call_data);
}

/*
 * We get the text-field activate callback, so pass it on to
 * the user if they requested it.  Our activate callback 
 * is just a convenience callback, so that the user doesn't
 * have to get the text-field first.  This make our integration
 * with XDesigner a little easier.
 */
static void
text_activate_cb(Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;
    
    if (ActivateCallback(spin))
	XtCallCallbackList((Widget)spin, 
			   ActivateCallback(spin), 
			   (XtPointer)call_data);
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
    DtSpinBoxWidget spin = (DtSpinBoxWidget)client_data;

    if (FocusCallback(spin))
	XtCallCallbackList((Widget)spin, 
			   FocusCallback(spin), 
			   (XtPointer)call_data);
}


/*
 * Synthetic resource get functions.
 */

static XmImportOperator
_XmSetSyntheticResForChild(	Widget widget,
				int offset,
				XtArgVal * value)
{ 
    return(XmSYNTHETIC_LOAD);
}

void
_DtSpinBoxGetArrowSize(	Widget w,
				int resource_offset,
				XtArgVal *value)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)w;
    Dimension data;
    Arg arg;

    /* MotifBc */
    XtSetArg(arg, XmNheight, &data);
    XtGetValues(UpArrow(spin), &arg, 1);
    *value = (XtArgVal)data;
}

/*
 * Routines which manipulate the SpinBox list.  These are external
 * for use by users of our widget.
 */

Widget 
DtCreateSpinBox(	Widget parent,
			char *name,
			Arg *arglist,
			Cardinal num_args)
{
    return(XtCreateWidget(name, dtSpinBoxWidgetClass, parent,
			  arglist, num_args));
}

void
DtSpinBoxAddItem(	Widget spinw,
			XmString item,
			int pos)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)spinw;
    DtSpinBoxPart *spin_p;
    XmString old, new_str, tmp;
    int total_items;
    _DtWidgetToAppContext(spinw);
    _DtAppLock(app);

    spin_p = (DtSpinBoxPart*) &(XmField(spin,ipot,DtSpinBox,label,Widget));

    total_items = ItemCount(spin) + 1;
    Items(spin) = (XmString *)XtRealloc((char*)Items(spin), 
					  (sizeof(XmString) * total_items));
    new_str = XmStringCopy(item);

    pos--;  /* User gives pos starting at 1 (0 means end of list) */

    if ((pos < 0) || (pos > ItemCount(spin)))
	pos = ItemCount(spin);

    if (pos >= ItemCount(spin))
        (Items(spin))[pos] = new_str;
    else {
        old = (Items(spin))[pos];
        (Items(spin))[pos] = new_str;
	for (pos++; pos < total_items; pos++) {
	    tmp = (Items(spin))[pos];
	    (Items(spin))[pos] = old;
	    old = tmp;
        }
    }
    ItemCount(spin) = total_items;

    if (Label(spin)) {
	SetMaximumLabelSize(spin_p);
	if (Editable(spin) == FALSE) {
	    ClearShadow(spin, TRUE);
	    if (RecomputeSize(spin))
		SetSpinBoxSize(spin);
	    LayoutChildren(spin);
	    DrawShadow(spin);
	}
    }

    /* Update the text-field or label */
    if (Editable(spin))
	SetTextFieldData(spin);
    else
	SetLabelData(spin);

    _DtAppUnlock(app);
}



void
DtSpinBoxDeletePos(	Widget spinw,
			int pos)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)spinw;
    DtSpinBoxPart *spin_p;
    int total_items;
    _DtWidgetToAppContext(spinw);
    _DtAppLock(app);

    if (ItemCount(spin) < 1)
      {
	_DtAppUnlock(app);
	return;
      }

    spin_p = (DtSpinBoxPart*) &(XmField(spin,ipot,DtSpinBox,label,Widget));

    pos--;
    if ((pos < 0) || (pos > ItemCount(spin)))
	pos = ItemCount(spin) - 1;

    total_items = ItemCount(spin) - 1;
    XmStringFree((Items(spin))[pos]);

    /* To keep Position of SpinBox up to date */
    if (Position(spin) > 0 && 
	((Position(spin) >= total_items) || pos < Position(spin)) )
	Position(spin) = Position(spin) - 1;

    if (pos < ItemCount(spin)) {
	for (; pos < total_items; pos++)
	    (Items(spin))[pos] = (Items(spin))[pos+1];
    }
    if (total_items > 0)
        Items(spin) = (XmString *)XtRealloc((char*)Items(spin),
					  (sizeof(XmString) * total_items));
    else {
	XtFree((char *)Items(spin));
	Items(spin) = (XmString *)NULL;
    }
    ItemCount(spin) = total_items;

    if (Label(spin)) {
	SetMaximumLabelSize(spin_p);
	if (Editable(spin) == FALSE) {
	    ClearShadow(spin, TRUE);
	    if (RecomputeSize(spin))
		SetSpinBoxSize(spin);
	    LayoutChildren(spin);
	    DrawShadow(spin);
	}
    }

    /* Update the text-field or label */
    if (Editable(spin))
	SetTextFieldData(spin);
    else
	SetLabelData(spin);

    _DtAppUnlock(app);
}

/*
 * Make the given item the currently visible item in the
 * text-field or label.
 */
void
DtSpinBoxSetItem(	Widget spinw,
			XmString item)
{
    DtSpinBoxWidget spin = (DtSpinBoxWidget)spinw;
    int i;
    _DtWidgetToAppContext(spinw);
    _DtAppLock(app);

    if (item && ItemCount(spin)>0) {
	for (i = 0; i < ItemCount(spin); i++)
	    if (XmStringCompare(item, (Items(spin))[i]))
		break;
	if (i < ItemCount(spin)) {
	    Position(spin) = i;
	    if (Editable(spin))
		SetTextFieldData(spin);
	    else
		SetLabelData(spin);
	}
	else
	    XtWarning(SB_SET_ITEM);
    }
    else
	XtWarning(SB_SET_ITEM);

    _DtAppUnlock(app);
}
