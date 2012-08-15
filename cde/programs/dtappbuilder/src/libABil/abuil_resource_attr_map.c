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
 *	$XConsortium: abuil_resource_attr_map.c /main/3 1995/11/06 18:21:24 rswiston $
 *
 * @(#)abuil_resource_attr_map.c	1.16 06 Oct 1994	cose/unity1/cde_app_builder/src/libABil
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
 * This file contains the mapping between uil resources and ABObj attributes
 */
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "abuil_loadP.h"


typedef enum {
    VAL_ARG,
    VAL_CONST,
    VAL_CONTEXT,
    VAL_PROC
} AttrValueType;

typedef BOOL (*VAL_PROC_HANDLER)(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
		);

typedef struct _AttrMapRec
{
    unsigned long	obj_mask;
    Cardinal            attr_offset;
    Cardinal            attr_size;
    AttrValueType	value_type;
    XtPointer		const_value;
} AttrMap;

typedef struct _ResourceAttrMapRec
{
    char	*resource_name;
    AttrMap	*attr_maps;
    Cardinal	num_maps;
} ResourceAttrMap;


typedef struct _ObjRefRec
{
    BOOL	is_resolved;
    void	*reference;
} ObjRef;

typedef struct _AnyContextRec	AnyContext;


typedef void  (*CONTEXT_STORE_HANDLER)(
		    ABObj,
		    AnyContext *
	      );
typedef void  (*CONTEXT_INIT_HANDLER)(
		    AnyContext *
	      );

typedef struct _ABuilStrIntMapRec
{
	char *string;
	int  value;
} ABuilStrIntMap;

struct _AnyContextRec
{
    CONTEXT_INIT_HANDLER	init_handler;
    CONTEXT_STORE_HANDLER	store_handler;
};

typedef struct _AttachmentContextRec
{
    CONTEXT_INIT_HANDLER	init_handler;
    CONTEXT_STORE_HANDLER	store_handler;

    ISTRING			att_type;
    ObjRef			*att_object;
    int				att_position;
    int				att_offset;
} AttachmentContext;


static void	attach_context_init(
		    AnyContext	*attach_context
		);
static void	north_attach_context_store(
		   ABObj		ab_widget,
		   AnyContext	*attach_context
		);
static void	south_attach_context_store(
		   ABObj		ab_widget,
		   AnyContext	*attach_context
		);
static void	east_attach_context_store(
		   ABObj		ab_widget,
		   AnyContext	*attach_context
		);
static void	west_attach_context_store(
		   ABObj		ab_widget,
		   AnyContext	*attach_context
		);

static void	attach_context_store(
		   ABObj		ab_widget,
		   AB_COMPASS_POINT	dir,		     
		   AnyContext	*attach_context
		);

static BOOL 	val_proc_debug(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
		);


/* Specialized VAL_PROC handlers to set object attributes from resources */

static BOOL val_set_label_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_labeltype_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_rows_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_columns_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_maximum_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_minimum_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_numcols_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_orientation_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_fg_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_bg_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_sensitive_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_mnemonic_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_oklabel_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_directory_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_pattern_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_autounmanage_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_filetype_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_maxlen_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_panemax_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_panemin_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_intvalue_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_strvalue_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_increment_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_decpts_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_showval_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_scrollh_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_scrollv_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_linestyle_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_wordwrap_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_border_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);
static BOOL val_set_packing_proc(ABObj, STRING, AB_ARG_TYPE, XtPointer);



static AttachmentContext
NorthAttach =
{
    attach_context_init, north_attach_context_store
};

static AttachmentContext
SouthAttach =
{
    attach_context_init, south_attach_context_store
};

static AttachmentContext
EastAttach =
{
    attach_context_init, east_attach_context_store
};

static AttachmentContext
WestAttach =
{
    attach_context_init, west_attach_context_store
};


static AnyContext *
AbuilP_attr_contexts[] =
{
    (AnyContext *)&NorthAttach,
    (AnyContext *)&SouthAttach,
    (AnyContext *)&EastAttach,
    (AnyContext *)&WestAttach
};




#define Ofs(FIELD)        XtOffsetOf(AB_OBJ, FIELD)

static AttrMap
accelerator_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
acceleratorText_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
accelerators_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
adjustLast_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
adjustMargin_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
alignment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
allowOverlap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
allowResize_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
allowShellResize_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
ancestorSensitive_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
applyLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
armColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
armPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
arrowDirection_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
audibleWarning_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
autoShowCursorPosition_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
autoUnmanage_map[] =
{
    { BMASK(AB_TYPE_FILE_CHOOSER),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_autounmanage_proc },
};

static AttrMap
automaticSelection_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
background_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_bg_proc },
};

static AttrMap
backgroundPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
baseHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
baseWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
blinkRate_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
borderColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
borderPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
borderWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
bottomAttachment_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_type),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&SouthAttach },
};

static AttrMap
bottomOffset_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_offset),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&SouthAttach },
};

static AttrMap
bottomPosition_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_position),	sizeof(int),
    	VAL_CONTEXT,	(XtPointer)&SouthAttach },
};

static AttrMap
bottomShadowColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
bottomShadowPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
bottomWidget_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_object),	sizeof(ObjRef *),
    	VAL_CONTEXT,	(XtPointer)&SouthAttach },
};

static AttrMap
buttonFontList_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
cancelButton_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
cancelLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
cascadePixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
childHorizontalAlignment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
childHorizontalSpacing_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
childPlacement_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
childType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
childVerticalAlignment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
clipWindow_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
colormap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
columns_map[] =
{
    { BMASK(AB_TYPE_TERM_PANE) | BMASK(AB_TYPE_TEXT_FIELD) | BMASK(AB_TYPE_TEXT_PANE) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_columns_proc },
};

static AttrMap
command_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
commandWindow_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
commandWindowLocation_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
createPopupChildProc_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
cursorPosition_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
cursorPositionVisible_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
decimalPoints_map[] =
{
    { BMASK(AB_TYPE_SCALE) | BMASK(AB_TYPE_SPIN_BOX) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_decpts_proc },
};

static AttrMap
defaultButton_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
defaultButtonShadowThickness_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
defaultButtonType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
defaultFontList_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
defaultPosition_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
deleteResponse_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
depth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dialogStyle_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dialogTitle_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dialogType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dirListItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dirListItems_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dirListLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dirMask_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dirSearchProc_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
dirSpec_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
directory_map[] =
{
    { BMASK(AB_TYPE_FILE_CHOOSER),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_directory_proc },
};

static AttrMap
doubleClickInterval_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
editMode_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
editable_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
entryAlignment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
entryBorder_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
entryClass_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
entryVerticalAlignment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fileListItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fileListItems_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fileListLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fileSearchProc_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fileTypeMask_map[] =
{
    { BMASK(AB_TYPE_FILE_CHOOSER),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_filetype_proc },
};

static AttrMap
fillOnArm_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fillOnSelect_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
filterLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
fontList_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
foreground_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_fg_proc },
};

static AttrMap
fractionBase_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
geometry_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
height_map[] =
{
    { ((unsigned long)~0L),	Ofs(height),	sizeof(int),
    	VAL_ARG,	0 },
};

static AttrMap
heightInc_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
helpLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
highlightColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
highlightOnEnter_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
highlightPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
highlightThickness_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
historyItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
historyItems_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
historyMaxItems_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
historyVisibleItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
horizontalScrollBar_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
horizontalSpacing_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconMask_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconName_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconNameEncoding_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconWindow_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconX_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconY_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
iconic_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
increment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
indicatorOn_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
indicatorSize_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
indicatorType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
initialDelay_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
initialFocus_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
initialResourcesPersistent_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
initialState_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
input_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
inputMethod_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
insertPosition_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
isAligned_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
isHomogeneous_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
itemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
items_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
keyboardFocusPolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
labelFontList_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
labelInsensitivePixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
labelPixmap_map[] =
{
    { BMASK(AB_TYPE_LABEL) | BMASK(AB_TYPE_BUTTON),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_label_proc },
    { BMASK(AB_TYPE_LABEL) | BMASK(AB_TYPE_BUTTON),
	    Ofs(label_type),	sizeof(AB_LABEL_TYPE),
    	VAL_CONST,	(XtPointer)AB_LABEL_GLYPH },
};

static AttrMap
labelString_map[] =
{
    { BMASK(AB_TYPE_LABEL) | BMASK(AB_TYPE_BUTTON),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_label_proc },
    { BMASK(AB_TYPE_LABEL) | BMASK(AB_TYPE_BUTTON),
	    Ofs(label_type),	sizeof(AB_LABEL_TYPE),
    	VAL_CONST,	(XtPointer)AB_LABEL_STRING },
};

static AttrMap
labelType_map[] =
{
    { BMASK(AB_TYPE_LABEL) | BMASK(AB_TYPE_BUTTON),
	    0,	1,
    	VAL_PROC,	(XPointer)val_set_labeltype_proc },
};

static AttrMap
leftAttachment_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_type),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&WestAttach },
};

static AttrMap
leftOffset_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_offset),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&WestAttach },
};

static AttrMap
leftPosition_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_position),	sizeof(int),
    	VAL_CONTEXT,	(XtPointer)&WestAttach },
};

static AttrMap
leftWidget_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_object),	sizeof(ObjRef *),
    	VAL_CONTEXT,	(XtPointer)&WestAttach },
};

static AttrMap
listItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listItems_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listMarginHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listMarginWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listSizePolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listSpacing_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listUpdated_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
listVisibleItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mainWindowMarginHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mainWindowMarginWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mappedWhenManaged_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mappingDelay_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
margin_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
marginBottom_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
marginHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
marginLeft_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
marginRight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
marginTop_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
marginWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
maxAspectX_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
maxAspectY_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
maxHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
maxLength_map[] =
{
    { BMASK(AB_TYPE_TEXT_FIELD) | BMASK(AB_TYPE_TEXT_PANE) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_maxlen_proc },
};

static AttrMap
maxWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
maximum_map[] =
{
    { BMASK(AB_TYPE_SCALE),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_maximum_proc },
};

static AttrMap
menuAccelerator_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
menuBar_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
menuHelpWidget_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
menuHistory_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
menuPost_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
messageAlignment_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
messageString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
messageWindow_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
minAspectX_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
minAspectY_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
minHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
minWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
minimizeButtons_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
minimum_map[] =
{
    { BMASK(AB_TYPE_SCALE),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_minimum_proc },
};

static AttrMap
mnemonic_map[] =
{
    { BMASK(AB_TYPE_ITEM) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_mnemonic_proc },
};

static AttrMap
mnemonicCharSet_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
multiClick_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mustMatch_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mwmDecorations_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mwmFunctions_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mwmInputMode_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
mwmMenu_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
navigationType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
noMatchString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
noResize_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
numColumns_map[] =
{
    { BMASK(AB_TYPE_TERM_PANE) | BMASK(AB_TYPE_TEXT_FIELD) | BMASK(AB_TYPE_TEXT_PANE) | BMASK(AB_TYPE_CHOICE) | BMASK(AB_TYPE_CONTAINER) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_numcols_proc },
};

static AttrMap
okLabelString_map[] =
{
    { BMASK(AB_TYPE_FILE_CHOOSER),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_oklabel_proc },
};

static AttrMap
orientation_map[] =
{
    { BMASK(AB_TYPE_SEPARATOR) | BMASK(AB_TYPE_SCALE) | BMASK(AB_TYPE_CHOICE),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_orientation_proc },
};

static AttrMap
overrideRedirect_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
packing_map[] =
{
    { BMASK(AB_TYPE_CONTAINER) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_packing_proc },
};

static AttrMap
pageIncrement_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
paneMaximum_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_panemax_proc },
};

static AttrMap
paneMinimum_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_panemin_proc },
};

static AttrMap
pattern_map[] =
{
    { BMASK(AB_TYPE_FILE_CHOOSER),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_pattern_proc },
};

static AttrMap
pendingDelete_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
popupEnabled_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
positionIndex_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
postFromCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
postFromList_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
preeditType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
processingDirection_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
promptString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
pushButtonEnabled_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
qualifySearchDataProc_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
radioAlwaysOne_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
radioBehavior_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
recomputeSize_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
refigureMode_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
repeatDelay_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
resizable_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
resizeHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
resizePolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
resizeWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
rightAttachment_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_type),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&EastAttach },
};

static AttrMap
rightOffset_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_offset),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&EastAttach },
};

static AttrMap
rightPosition_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_position),	sizeof(int),
    	VAL_CONTEXT,	(XtPointer)&EastAttach },
};

static AttrMap
rightWidget_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_object),	sizeof(ObjRef *),
    	VAL_CONTEXT,	(XtPointer)&EastAttach },
};

static AttrMap
rowColumnType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
rows_map[] =
{
    { BMASK(AB_TYPE_TERM_PANE) | BMASK(AB_TYPE_TEXT_PANE) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_rows_proc },
};

static AttrMap
rubberPositioning_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
sashHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
sashIndent_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
sashShadowThickness_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
sashWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
saveUnder_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scaleHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scaleMultiple_map[] =
{
    { BMASK(AB_TYPE_SCALE) | BMASK(AB_TYPE_SPIN_BOX) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_increment_proc },
};

static AttrMap
scaleWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
screen_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrollBarDisplayPolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrollBarPlacement_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrollHorizontal_map[] =
{
    { BMASK(AB_TYPE_TEXT_PANE) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_scrollh_proc },
};

static AttrMap
scrollLeftSide_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrollTopSide_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrollVertical_map[] =
{
    { BMASK(AB_TYPE_TEXT_PANE) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_scrollv_proc },
};

static AttrMap
scrolledWindowMarginHeight_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrolledWindowMarginWidth_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
scrollingPolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectInsensitivePixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectThreshold_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectedItemCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectedItems_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectionArray_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectionArrayCount_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectionLabelString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
selectionPolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
sensitive_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_sensitive_proc },
};

static AttrMap
separatorOn_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
separatorType_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_linestyle_proc },
};

static AttrMap
set_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
shadowThickness_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
shadowType_map[] =
{
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_border_proc },
    { ((unsigned long)~0L),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_proc_debug },
};

static AttrMap
shellUnitType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
showArrows_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
showAsDefault_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
showSeparator_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
showValue_map[] =
{
    { BMASK(AB_TYPE_TERM_PANE) | BMASK(AB_TYPE_TEXT_PANE) ,
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_showval_proc },
};

static AttrMap
skipAdjust_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
sliderSize_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
source_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
spacing_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
stringDirection_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
subMenuId_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
symbolPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
tearOffModel_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
textAccelerators_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
textColumns_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
textFontList_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
textString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
textTranslations_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

/* 
** We use the ABObj's label resource to hold the title string for main windows,
** custom dialogs and file choosers.
*/
static AttrMap
title_map[] =
{
    { BMASK(AB_TYPE_BASE_WINDOW) | BMASK(AB_TYPE_DIALOG) | BMASK(AB_TYPE_FILE_CHOOSER),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_label_proc },
};

static AttrMap
titleEncoding_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
titleString_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
topAttachment_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_type),	sizeof(ISTRING),
    	VAL_CONTEXT,	(XtPointer)&NorthAttach },
};

static AttrMap
topCharacter_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
topItemPosition_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
topOffset_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_offset),	sizeof(int),
    	VAL_CONTEXT,	(XtPointer)&NorthAttach },
};

static AttrMap
topPosition_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_position),	sizeof(int),
    	VAL_CONTEXT,	(XtPointer)&NorthAttach },
};

static AttrMap
topShadowColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
topShadowPixmap_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
topWidget_map[] =
{
    { ((unsigned long)~0L),
	XtOffsetOf(AttachmentContext, att_object),	sizeof(ObjRef *),
    	VAL_CONTEXT,	(XtPointer)&NorthAttach },
};

static AttrMap
transient_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
transientFor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
translations_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
traversalOn_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
troughColor_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
unitType_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
unpostBehavior_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
useAsyncGeometry_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
userData_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
value_map[] =
{
    { BMASK(AB_TYPE_TEXT_PANE) | BMASK(AB_TYPE_TEXT_FIELD),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_strvalue_proc },
    { BMASK(AB_TYPE_SCALE),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_intvalue_proc },
};

static AttrMap
valueWcs_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
verifyBell_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
verticalScrollBar_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
verticalSpacing_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
visibleItemCount_map[] =
{
    { BMASK(AB_TYPE_LIST),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_rows_proc },
};

static AttrMap
visibleWhenOff_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
visual_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
visualPolicy_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
waitForWm_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
whichButton_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
width_map[] =
{
    { ((unsigned long)~0L),	Ofs(width),	sizeof(int),
    	VAL_ARG,	0 },
};

static AttrMap
widthInc_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
winGravity_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
windowGroup_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
wmTimeout_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
wordWrap_map[] =
{
    { BMASK(AB_TYPE_TEXT_PANE),
	    0,		1,  /* size field must != 0, but unused for VAL_PROC */
    	VAL_PROC,	(XtPointer) val_set_wordwrap_proc },
};

static AttrMap
workWindow_map[] =
{
    { ((unsigned long)~0L),	0,	0,
    	VAL_ARG,	0 },
};

static AttrMap
x_map[] =
{
    { ((unsigned long)~0L),	Ofs(x),	sizeof(int),
    	VAL_ARG,	0 },
};

static AttrMap
y_map[] =
{
    { ((unsigned long)~0L),	Ofs(y),	sizeof(int),
    	VAL_ARG,	0 },
};

#undef Ofs

static ResourceAttrMap
resource_attr_map[] =
{
    { "XmNaccelerator",
	accelerator_map, XtNumber(accelerator_map) },
    { "XmNacceleratorText",
	acceleratorText_map, XtNumber(acceleratorText_map) },
    { "XmNaccelerators",
	accelerators_map, XtNumber(accelerators_map) },
    { "XmNadjustLast",
	adjustLast_map, XtNumber(adjustLast_map) },
    { "XmNadjustMargin",
	adjustMargin_map, XtNumber(adjustMargin_map) },
    { "XmNalignment",
	alignment_map, XtNumber(alignment_map) },
    { "XmNallowOverlap",
	allowOverlap_map, XtNumber(allowOverlap_map) },
    { "XmNallowResize",
	allowResize_map, XtNumber(allowResize_map) },
    { "XmNallowShellResize",
	allowShellResize_map, XtNumber(allowShellResize_map) },
    { "XmNancestorSensitive",
	ancestorSensitive_map, XtNumber(ancestorSensitive_map) },
    { "XmNapplyLabelString",
	applyLabelString_map, XtNumber(applyLabelString_map) },
    { "XmNarmColor",
	armColor_map, XtNumber(armColor_map) },
    { "XmNarmPixmap",
	armPixmap_map, XtNumber(armPixmap_map) },
    { "XmNarrowDirection",
	arrowDirection_map, XtNumber(arrowDirection_map) },
    { "XmNaudibleWarning",
	audibleWarning_map, XtNumber(audibleWarning_map) },
    { "XmNautoShowCursorPosition",
	autoShowCursorPosition_map, XtNumber(autoShowCursorPosition_map) },
    { "XmNautoUnmanage",
	autoUnmanage_map, XtNumber(autoUnmanage_map) },
    { "XmNautomaticSelection",
	automaticSelection_map, XtNumber(automaticSelection_map) },
    { "XmNbackground",
	background_map, XtNumber(background_map) },
    { "XmNbackgroundPixmap",
	backgroundPixmap_map, XtNumber(backgroundPixmap_map) },
    { "XmNbaseHeight",
	baseHeight_map, XtNumber(baseHeight_map) },
    { "XmNbaseWidth",
	baseWidth_map, XtNumber(baseWidth_map) },
    { "XmNblinkRate",
	blinkRate_map, XtNumber(blinkRate_map) },
    { "XmNborderColor",
	borderColor_map, XtNumber(borderColor_map) },
    { "XmNborderPixmap",
	borderPixmap_map, XtNumber(borderPixmap_map) },
    { "XmNborderWidth",
	borderWidth_map, XtNumber(borderWidth_map) },
    { "XmNbottomAttachment",
	bottomAttachment_map, XtNumber(bottomAttachment_map) },
    { "XmNbottomOffset",
	bottomOffset_map, XtNumber(bottomOffset_map) },
    { "XmNbottomPosition",
	bottomPosition_map, XtNumber(bottomPosition_map) },
    { "XmNbottomShadowColor",
	bottomShadowColor_map, XtNumber(bottomShadowColor_map) },
    { "XmNbottomShadowPixmap",
	bottomShadowPixmap_map, XtNumber(bottomShadowPixmap_map) },
    { "XmNbottomWidget",
	bottomWidget_map, XtNumber(bottomWidget_map) },
    { "XmNbuttonFontList",
	buttonFontList_map, XtNumber(buttonFontList_map) },
    { "XmNcancelButton",
	cancelButton_map, XtNumber(cancelButton_map) },
    { "XmNcancelLabelString",
	cancelLabelString_map, XtNumber(cancelLabelString_map) },
    { "XmNcascadePixmap",
	cascadePixmap_map, XtNumber(cascadePixmap_map) },
    { "XmNchildHorizontalAlignment",
	childHorizontalAlignment_map, XtNumber(childHorizontalAlignment_map) },
    { "XmNchildHorizontalSpacing",
	childHorizontalSpacing_map, XtNumber(childHorizontalSpacing_map) },
    { "XmNchildPlacement",
	childPlacement_map, XtNumber(childPlacement_map) },
    { "XmNchildType",
	childType_map, XtNumber(childType_map) },
    { "XmNchildVerticalAlignment",
	childVerticalAlignment_map, XtNumber(childVerticalAlignment_map) },
    { "XmNclipWindow",
	clipWindow_map, XtNumber(clipWindow_map) },
    { "XmNcolormap",
	colormap_map, XtNumber(colormap_map) },
    { "XmNcolumns",
	columns_map, XtNumber(columns_map) },
    { "XmNcommand",
	command_map, XtNumber(command_map) },
    { "XmNcommandWindow",
	commandWindow_map, XtNumber(commandWindow_map) },
    { "XmNcommandWindowLocation",
	commandWindowLocation_map, XtNumber(commandWindowLocation_map) },
    { "XmNcreatePopupChildProc",
	createPopupChildProc_map, XtNumber(createPopupChildProc_map) },
    { "XmNcursorPosition",
	cursorPosition_map, XtNumber(cursorPosition_map) },
    { "XmNcursorPositionVisible",
	cursorPositionVisible_map, XtNumber(cursorPositionVisible_map) },
    { "XmNdecimalPoints",
	decimalPoints_map, XtNumber(decimalPoints_map) },
    { "XmNdefaultButton",
	defaultButton_map, XtNumber(defaultButton_map) },
    { "XmNdefaultButtonShadowThickness",
	defaultButtonShadowThickness_map, XtNumber(defaultButtonShadowThickness_map) },
    { "XmNdefaultButtonType",
	defaultButtonType_map, XtNumber(defaultButtonType_map) },
    { "XmNdefaultFontList",
	defaultFontList_map, XtNumber(defaultFontList_map) },
    { "XmNdefaultPosition",
	defaultPosition_map, XtNumber(defaultPosition_map) },
    { "XmNdeleteResponse",
	deleteResponse_map, XtNumber(deleteResponse_map) },
    { "XmNdepth",
	depth_map, XtNumber(depth_map) },
    { "XmNdialogStyle",
	dialogStyle_map, XtNumber(dialogStyle_map) },
    { "XmNdialogTitle",
	dialogTitle_map, XtNumber(dialogTitle_map) },
    { "XmNdialogType",
	dialogType_map, XtNumber(dialogType_map) },
    { "XmNdirListItemCount",
	dirListItemCount_map, XtNumber(dirListItemCount_map) },
    { "XmNdirListItems",
	dirListItems_map, XtNumber(dirListItems_map) },
    { "XmNdirListLabelString",
	dirListLabelString_map, XtNumber(dirListLabelString_map) },
    { "XmNdirMask",
	dirMask_map, XtNumber(dirMask_map) },
    { "XmNdirSearchProc",
	dirSearchProc_map, XtNumber(dirSearchProc_map) },
    { "XmNdirSpec",
	dirSpec_map, XtNumber(dirSpec_map) },
    { "XmNdirectory",
	directory_map, XtNumber(directory_map) },
    { "XmNdoubleClickInterval",
	doubleClickInterval_map, XtNumber(doubleClickInterval_map) },
    { "XmNeditMode",
	editMode_map, XtNumber(editMode_map) },
    { "XmNeditable",
	editable_map, XtNumber(editable_map) },
    { "XmNentryAlignment",
	entryAlignment_map, XtNumber(entryAlignment_map) },
    { "XmNentryBorder",
	entryBorder_map, XtNumber(entryBorder_map) },
    { "XmNentryClass",
	entryClass_map, XtNumber(entryClass_map) },
    { "XmNentryVerticalAlignment",
	entryVerticalAlignment_map, XtNumber(entryVerticalAlignment_map) },
    { "XmNfileListItemCount",
	fileListItemCount_map, XtNumber(fileListItemCount_map) },
    { "XmNfileListItems",
	fileListItems_map, XtNumber(fileListItems_map) },
    { "XmNfileListLabelString",
	fileListLabelString_map, XtNumber(fileListLabelString_map) },
    { "XmNfileSearchProc",
	fileSearchProc_map, XtNumber(fileSearchProc_map) },
    { "XmNfileTypeMask",
	fileTypeMask_map, XtNumber(fileTypeMask_map) },
    { "XmNfillOnArm",
	fillOnArm_map, XtNumber(fillOnArm_map) },
    { "XmNfillOnSelect",
	fillOnSelect_map, XtNumber(fillOnSelect_map) },
    { "XmNfilterLabelString",
	filterLabelString_map, XtNumber(filterLabelString_map) },
    { "XmNfontList",
	fontList_map, XtNumber(fontList_map) },
    { "XmNforeground",
	foreground_map, XtNumber(foreground_map) },
    { "XmNfractionBase",
	fractionBase_map, XtNumber(fractionBase_map) },
    { "XmNgeometry",
	geometry_map, XtNumber(geometry_map) },
    { "XmNheight",
	height_map, XtNumber(height_map) },
    { "XmNheightInc",
	heightInc_map, XtNumber(heightInc_map) },
    { "XmNhelpLabelString",
	helpLabelString_map, XtNumber(helpLabelString_map) },
    { "XmNhighlightColor",
	highlightColor_map, XtNumber(highlightColor_map) },
    { "XmNhighlightOnEnter",
	highlightOnEnter_map, XtNumber(highlightOnEnter_map) },
    { "XmNhighlightPixmap",
	highlightPixmap_map, XtNumber(highlightPixmap_map) },
    { "XmNhighlightThickness",
	highlightThickness_map, XtNumber(highlightThickness_map) },
    { "XmNhistoryItemCount",
	historyItemCount_map, XtNumber(historyItemCount_map) },
    { "XmNhistoryItems",
	historyItems_map, XtNumber(historyItems_map) },
    { "XmNhistoryMaxItems",
	historyMaxItems_map, XtNumber(historyMaxItems_map) },
    { "XmNhistoryVisibleItemCount",
	historyVisibleItemCount_map, XtNumber(historyVisibleItemCount_map) },
    { "XmNhorizontalScrollBar",
	horizontalScrollBar_map, XtNumber(horizontalScrollBar_map) },
    { "XmNhorizontalSpacing",
	horizontalSpacing_map, XtNumber(horizontalSpacing_map) },
    { "XmNiconMask",
	iconMask_map, XtNumber(iconMask_map) },
    { "XmNiconName",
	iconName_map, XtNumber(iconName_map) },
    { "XmNiconNameEncoding",
	iconNameEncoding_map, XtNumber(iconNameEncoding_map) },
    { "XmNiconPixmap",
	iconPixmap_map, XtNumber(iconPixmap_map) },
    { "XmNiconWindow",
	iconWindow_map, XtNumber(iconWindow_map) },
    { "XmNiconX",
	iconX_map, XtNumber(iconX_map) },
    { "XmNiconY",
	iconY_map, XtNumber(iconY_map) },
    { "XmNiconic",
	iconic_map, XtNumber(iconic_map) },
    { "XmNincrement",
	increment_map, XtNumber(increment_map) },
    { "XmNindicatorOn",
	indicatorOn_map, XtNumber(indicatorOn_map) },
    { "XmNindicatorSize",
	indicatorSize_map, XtNumber(indicatorSize_map) },
    { "XmNindicatorType",
	indicatorType_map, XtNumber(indicatorType_map) },
    { "XmNinitialDelay",
	initialDelay_map, XtNumber(initialDelay_map) },
    { "XmNinitialFocus",
	initialFocus_map, XtNumber(initialFocus_map) },
    { "XmNinitialResourcesPersistent",
	initialResourcesPersistent_map, XtNumber(initialResourcesPersistent_map) },
    { "XmNinitialState",
	initialState_map, XtNumber(initialState_map) },
    { "XmNinput",
	input_map, XtNumber(input_map) },
    { "XmNinputMethod",
	inputMethod_map, XtNumber(inputMethod_map) },
    { "XmNinsertPosition",
	insertPosition_map, XtNumber(insertPosition_map) },
    { "XmNisAligned",
	isAligned_map, XtNumber(isAligned_map) },
    { "XmNisHomogeneous",
	isHomogeneous_map, XtNumber(isHomogeneous_map) },
    { "XmNitemCount",
	itemCount_map, XtNumber(itemCount_map) },
    { "XmNitems",
	items_map, XtNumber(items_map) },
    { "XmNkeyboardFocusPolicy",
	keyboardFocusPolicy_map, XtNumber(keyboardFocusPolicy_map) },
    { "XmNlabelFontList",
	labelFontList_map, XtNumber(labelFontList_map) },
    { "XmNlabelInsensitivePixmap",
	labelInsensitivePixmap_map, XtNumber(labelInsensitivePixmap_map) },
    { "XmNlabelPixmap",
	labelPixmap_map, XtNumber(labelPixmap_map) },
    { "XmNlabelString",
	labelString_map, XtNumber(labelString_map) },
    { "XmNlabelType",
	labelType_map, XtNumber(labelType_map) },
    { "XmNleftAttachment",
	leftAttachment_map, XtNumber(leftAttachment_map) },
    { "XmNleftOffset",
	leftOffset_map, XtNumber(leftOffset_map) },
    { "XmNleftPosition",
	leftPosition_map, XtNumber(leftPosition_map) },
    { "XmNleftWidget",
	leftWidget_map, XtNumber(leftWidget_map) },
    { "XmNlistItemCount",
	listItemCount_map, XtNumber(listItemCount_map) },
    { "XmNlistItems",
	listItems_map, XtNumber(listItems_map) },
    { "XmNlistLabelString",
	listLabelString_map, XtNumber(listLabelString_map) },
    { "XmNlistMarginHeight",
	listMarginHeight_map, XtNumber(listMarginHeight_map) },
    { "XmNlistMarginWidth",
	listMarginWidth_map, XtNumber(listMarginWidth_map) },
    { "XmNlistSizePolicy",
	listSizePolicy_map, XtNumber(listSizePolicy_map) },
    { "XmNlistSpacing",
	listSpacing_map, XtNumber(listSpacing_map) },
    { "XmNlistUpdated",
	listUpdated_map, XtNumber(listUpdated_map) },
    { "XmNlistVisibleItemCount",
	listVisibleItemCount_map, XtNumber(listVisibleItemCount_map) },
    { "XmNmainWindowMarginHeight",
	mainWindowMarginHeight_map, XtNumber(mainWindowMarginHeight_map) },
    { "XmNmainWindowMarginWidth",
	mainWindowMarginWidth_map, XtNumber(mainWindowMarginWidth_map) },
    { "XmNmappedWhenManaged",
	mappedWhenManaged_map, XtNumber(mappedWhenManaged_map) },
    { "XmNmappingDelay",
	mappingDelay_map, XtNumber(mappingDelay_map) },
    { "XmNmargin",
	margin_map, XtNumber(margin_map) },
    { "XmNmarginBottom",
	marginBottom_map, XtNumber(marginBottom_map) },
    { "XmNmarginHeight",
	marginHeight_map, XtNumber(marginHeight_map) },
    { "XmNmarginLeft",
	marginLeft_map, XtNumber(marginLeft_map) },
    { "XmNmarginRight",
	marginRight_map, XtNumber(marginRight_map) },
    { "XmNmarginTop",
	marginTop_map, XtNumber(marginTop_map) },
    { "XmNmarginWidth",
	marginWidth_map, XtNumber(marginWidth_map) },
    { "XmNmaxAspectX",
	maxAspectX_map, XtNumber(maxAspectX_map) },
    { "XmNmaxAspectY",
	maxAspectY_map, XtNumber(maxAspectY_map) },
    { "XmNmaxHeight",
	maxHeight_map, XtNumber(maxHeight_map) },
    { "XmNmaxLength",
	maxLength_map, XtNumber(maxLength_map) },
    { "XmNmaxWidth",
	maxWidth_map, XtNumber(maxWidth_map) },
    { "XmNmaximum",
	maximum_map, XtNumber(maximum_map) },
    { "XmNmenuAccelerator",
	menuAccelerator_map, XtNumber(menuAccelerator_map) },
    { "XmNmenuBar",
	menuBar_map, XtNumber(menuBar_map) },
    { "XmNmenuHelpWidget",
	menuHelpWidget_map, XtNumber(menuHelpWidget_map) },
    { "XmNmenuHistory",
	menuHistory_map, XtNumber(menuHistory_map) },
    { "XmNmenuPost",
	menuPost_map, XtNumber(menuPost_map) },
    { "XmNmessageAlignment",
	messageAlignment_map, XtNumber(messageAlignment_map) },
    { "XmNmessageString",
	messageString_map, XtNumber(messageString_map) },
    { "XmNmessageWindow",
	messageWindow_map, XtNumber(messageWindow_map) },
    { "XmNminAspectX",
	minAspectX_map, XtNumber(minAspectX_map) },
    { "XmNminAspectY",
	minAspectY_map, XtNumber(minAspectY_map) },
    { "XmNminHeight",
	minHeight_map, XtNumber(minHeight_map) },
    { "XmNminWidth",
	minWidth_map, XtNumber(minWidth_map) },
    { "XmNminimizeButtons",
	minimizeButtons_map, XtNumber(minimizeButtons_map) },
    { "XmNminimum",
	minimum_map, XtNumber(minimum_map) },
    { "XmNmnemonic",
	mnemonic_map, XtNumber(mnemonic_map) },
    { "XmNmnemonicCharSet",
	mnemonicCharSet_map, XtNumber(mnemonicCharSet_map) },
    { "XmNmultiClick",
	multiClick_map, XtNumber(multiClick_map) },
    { "XmNmustMatch",
	mustMatch_map, XtNumber(mustMatch_map) },
    { "XmNmwmDecorations",
	mwmDecorations_map, XtNumber(mwmDecorations_map) },
    { "XmNmwmFunctions",
	mwmFunctions_map, XtNumber(mwmFunctions_map) },
    { "XmNmwmInputMode",
	mwmInputMode_map, XtNumber(mwmInputMode_map) },
    { "XmNmwmMenu",
	mwmMenu_map, XtNumber(mwmMenu_map) },
    { "XmNnavigationType",
	navigationType_map, XtNumber(navigationType_map) },
    { "XmNnoMatchString",
	noMatchString_map, XtNumber(noMatchString_map) },
    { "XmNnoResize",
	noResize_map, XtNumber(noResize_map) },
    { "XmNnumColumns",
	numColumns_map, XtNumber(numColumns_map) },
    { "XmNokLabelString",
	okLabelString_map, XtNumber(okLabelString_map) },
    { "XmNorientation",
	orientation_map, XtNumber(orientation_map) },
    { "XmNoverrideRedirect",
	overrideRedirect_map, XtNumber(overrideRedirect_map) },
    { "XmNpacking",
	packing_map, XtNumber(packing_map) },
    { "XmNpageIncrement",
	pageIncrement_map, XtNumber(pageIncrement_map) },
    { "XmNpaneMaximum",
	paneMaximum_map, XtNumber(paneMaximum_map) },
    { "XmNpaneMinimum",
	paneMinimum_map, XtNumber(paneMinimum_map) },
    { "XmNpattern",
	pattern_map, XtNumber(pattern_map) },
    { "XmNpendingDelete",
	pendingDelete_map, XtNumber(pendingDelete_map) },
    { "XmNpopupEnabled",
	popupEnabled_map, XtNumber(popupEnabled_map) },
    { "XmNpositionIndex",
	positionIndex_map, XtNumber(positionIndex_map) },
    { "XmNpostFromCount",
	postFromCount_map, XtNumber(postFromCount_map) },
    { "XmNpostFromList",
	postFromList_map, XtNumber(postFromList_map) },
    { "XmNpreeditType",
	preeditType_map, XtNumber(preeditType_map) },
    { "XmNprocessingDirection",
	processingDirection_map, XtNumber(processingDirection_map) },
    { "XmNpromptString",
	promptString_map, XtNumber(promptString_map) },
    { "XmNpushButtonEnabled",
	pushButtonEnabled_map, XtNumber(pushButtonEnabled_map) },
    { "XmNqualifySearchDataProc",
	qualifySearchDataProc_map, XtNumber(qualifySearchDataProc_map) },
    { "XmNradioAlwaysOne",
	radioAlwaysOne_map, XtNumber(radioAlwaysOne_map) },
    { "XmNradioBehavior",
	radioBehavior_map, XtNumber(radioBehavior_map) },
    { "XmNrecomputeSize",
	recomputeSize_map, XtNumber(recomputeSize_map) },
    { "XmNrefigureMode",
	refigureMode_map, XtNumber(refigureMode_map) },
    { "XmNrepeatDelay",
	repeatDelay_map, XtNumber(repeatDelay_map) },
    { "XmNresizable",
	resizable_map, XtNumber(resizable_map) },
    { "XmNresizeHeight",
	resizeHeight_map, XtNumber(resizeHeight_map) },
    { "XmNresizePolicy",
	resizePolicy_map, XtNumber(resizePolicy_map) },
    { "XmNresizeWidth",
	resizeWidth_map, XtNumber(resizeWidth_map) },
    { "XmNrightAttachment",
	rightAttachment_map, XtNumber(rightAttachment_map) },
    { "XmNrightOffset",
	rightOffset_map, XtNumber(rightOffset_map) },
    { "XmNrightPosition",
	rightPosition_map, XtNumber(rightPosition_map) },
    { "XmNrightWidget",
	rightWidget_map, XtNumber(rightWidget_map) },
    { "XmNrowColumnType",
	rowColumnType_map, XtNumber(rowColumnType_map) },
    { "XmNrows",
	rows_map, XtNumber(rows_map) },
    { "XmNrubberPositioning",
	rubberPositioning_map, XtNumber(rubberPositioning_map) },
    { "XmNsashHeight",
	sashHeight_map, XtNumber(sashHeight_map) },
    { "XmNsashIndent",
	sashIndent_map, XtNumber(sashIndent_map) },
    { "XmNsashShadowThickness",
	sashShadowThickness_map, XtNumber(sashShadowThickness_map) },
    { "XmNsashWidth",
	sashWidth_map, XtNumber(sashWidth_map) },
    { "XmNsaveUnder",
	saveUnder_map, XtNumber(saveUnder_map) },
    { "XmNscaleHeight",
	scaleHeight_map, XtNumber(scaleHeight_map) },
    { "XmNscaleMultiple",
	scaleMultiple_map, XtNumber(scaleMultiple_map) },
    { "XmNscaleWidth",
	scaleWidth_map, XtNumber(scaleWidth_map) },
    { "XmNscreen",
	screen_map, XtNumber(screen_map) },
    { "XmNscrollBarDisplayPolicy",
	scrollBarDisplayPolicy_map, XtNumber(scrollBarDisplayPolicy_map) },
    { "XmNscrollBarPlacement",
	scrollBarPlacement_map, XtNumber(scrollBarPlacement_map) },
    { "XmNscrollHorizontal",
	scrollHorizontal_map, XtNumber(scrollHorizontal_map) },
    { "XmNscrollLeftSide",
	scrollLeftSide_map, XtNumber(scrollLeftSide_map) },
    { "XmNscrollTopSide",
	scrollTopSide_map, XtNumber(scrollTopSide_map) },
    { "XmNscrollVertical",
	scrollVertical_map, XtNumber(scrollVertical_map) },
    { "XmNscrolledWindowMarginHeight",
	scrolledWindowMarginHeight_map, XtNumber(scrolledWindowMarginHeight_map) },
    { "XmNscrolledWindowMarginWidth",
	scrolledWindowMarginWidth_map, XtNumber(scrolledWindowMarginWidth_map) },
    { "XmNscrollingPolicy",
	scrollingPolicy_map, XtNumber(scrollingPolicy_map) },
    { "XmNselectColor",
	selectColor_map, XtNumber(selectColor_map) },
    { "XmNselectInsensitivePixmap",
	selectInsensitivePixmap_map, XtNumber(selectInsensitivePixmap_map) },
    { "XmNselectPixmap",
	selectPixmap_map, XtNumber(selectPixmap_map) },
    { "XmNselectThreshold",
	selectThreshold_map, XtNumber(selectThreshold_map) },
    { "XmNselectedItemCount",
	selectedItemCount_map, XtNumber(selectedItemCount_map) },
    { "XmNselectedItems",
	selectedItems_map, XtNumber(selectedItems_map) },
    { "XmNselectionArray",
	selectionArray_map, XtNumber(selectionArray_map) },
    { "XmNselectionArrayCount",
	selectionArrayCount_map, XtNumber(selectionArrayCount_map) },
    { "XmNselectionLabelString",
	selectionLabelString_map, XtNumber(selectionLabelString_map) },
    { "XmNselectionPolicy",
	selectionPolicy_map, XtNumber(selectionPolicy_map) },
    { "XmNsensitive",
	sensitive_map, XtNumber(sensitive_map) },
    { "XmNseparatorOn",
	separatorOn_map, XtNumber(separatorOn_map) },
    { "XmNseparatorType",
	separatorType_map, XtNumber(separatorType_map) },
    { "XmNset",
	set_map, XtNumber(set_map) },
    { "XmNshadowThickness",
	shadowThickness_map, XtNumber(shadowThickness_map) },
    { "XmNshadowType",
	shadowType_map, XtNumber(shadowType_map) },
    { "XmNshellUnitType",
	shellUnitType_map, XtNumber(shellUnitType_map) },
    { "XmNshowArrows",
	showArrows_map, XtNumber(showArrows_map) },
    { "XmNshowAsDefault",
	showAsDefault_map, XtNumber(showAsDefault_map) },
    { "XmNshowSeparator",
	showSeparator_map, XtNumber(showSeparator_map) },
    { "XmNshowValue",
	showValue_map, XtNumber(showValue_map) },
    { "XmNskipAdjust",
	skipAdjust_map, XtNumber(skipAdjust_map) },
    { "XmNsliderSize",
	sliderSize_map, XtNumber(sliderSize_map) },
    { "XmNsource",
	source_map, XtNumber(source_map) },
    { "XmNspacing",
	spacing_map, XtNumber(spacing_map) },
    { "XmNstringDirection",
	stringDirection_map, XtNumber(stringDirection_map) },
    { "XmNsubMenuId",
	subMenuId_map, XtNumber(subMenuId_map) },
    { "XmNsymbolPixmap",
	symbolPixmap_map, XtNumber(symbolPixmap_map) },
    { "XmNtearOffModel",
	tearOffModel_map, XtNumber(tearOffModel_map) },
    { "XmNtextAccelerators",
	textAccelerators_map, XtNumber(textAccelerators_map) },
    { "XmNtextColumns",
	textColumns_map, XtNumber(textColumns_map) },
    { "XmNtextFontList",
	textFontList_map, XtNumber(textFontList_map) },
    { "XmNtextString",
	textString_map, XtNumber(textString_map) },
    { "XmNtextTranslations",
	textTranslations_map, XtNumber(textTranslations_map) },
    { "XmNtitle",
	title_map, XtNumber(title_map) },
    { "XmNtitleEncoding",
	titleEncoding_map, XtNumber(titleEncoding_map) },
    { "XmNtitleString",
	titleString_map, XtNumber(titleString_map) },
    { "XmNtopAttachment",
	topAttachment_map, XtNumber(topAttachment_map) },
    { "XmNtopCharacter",
	topCharacter_map, XtNumber(topCharacter_map) },
    { "XmNtopItemPosition",
	topItemPosition_map, XtNumber(topItemPosition_map) },
    { "XmNtopOffset",
	topOffset_map, XtNumber(topOffset_map) },
    { "XmNtopPosition",
	topPosition_map, XtNumber(topPosition_map) },
    { "XmNtopShadowColor",
	topShadowColor_map, XtNumber(topShadowColor_map) },
    { "XmNtopShadowPixmap",
	topShadowPixmap_map, XtNumber(topShadowPixmap_map) },
    { "XmNtopWidget",
	topWidget_map, XtNumber(topWidget_map) },
    { "XmNtransient",
	transient_map, XtNumber(transient_map) },
    { "XmNtransientFor",
	transientFor_map, XtNumber(transientFor_map) },
    { "XmNtranslations",
	translations_map, XtNumber(translations_map) },
    { "XmNtraversalOn",
	traversalOn_map, XtNumber(traversalOn_map) },
    { "XmNtroughColor",
	troughColor_map, XtNumber(troughColor_map) },
    { "XmNunitType",
	unitType_map, XtNumber(unitType_map) },
    { "XmNunpostBehavior",
	unpostBehavior_map, XtNumber(unpostBehavior_map) },
    { "XmNuseAsyncGeometry",
	useAsyncGeometry_map, XtNumber(useAsyncGeometry_map) },
    { "XmNuserData",
	userData_map, XtNumber(userData_map) },
    { "XmNvalue",
	value_map, XtNumber(value_map) },
    { "XmNvalueWcs",
	valueWcs_map, XtNumber(valueWcs_map) },
    { "XmNverifyBell",
	verifyBell_map, XtNumber(verifyBell_map) },
    { "XmNverticalScrollBar",
	verticalScrollBar_map, XtNumber(verticalScrollBar_map) },
    { "XmNverticalSpacing",
	verticalSpacing_map, XtNumber(verticalSpacing_map) },
    { "XmNvisibleItemCount",
	visibleItemCount_map, XtNumber(visibleItemCount_map) },
    { "XmNvisibleWhenOff",
	visibleWhenOff_map, XtNumber(visibleWhenOff_map) },
    { "XmNvisual",
	visual_map, XtNumber(visual_map) },
    { "XmNvisualPolicy",
	visualPolicy_map, XtNumber(visualPolicy_map) },
    { "XmNwaitForWm",
	waitForWm_map, XtNumber(waitForWm_map) },
    { "XmNwhichButton",
	whichButton_map, XtNumber(whichButton_map) },
    { "XmNwidth",
	width_map, XtNumber(width_map) },
    { "XmNwidthInc",
	widthInc_map, XtNumber(widthInc_map) },
    { "XmNwinGravity",
	winGravity_map, XtNumber(winGravity_map) },
    { "XmNwindowGroup",
	windowGroup_map, XtNumber(windowGroup_map) },
    { "XmNwmTimeout",
	wmTimeout_map, XtNumber(wmTimeout_map) },
    { "XmNwordWrap",
	wordWrap_map, XtNumber(wordWrap_map) },
    { "XmNworkWindow",
	workWindow_map, XtNumber(workWindow_map) },
    { "XmNx",
	x_map, XtNumber(x_map) },
    { "XmNy",
	y_map, XtNumber(y_map) },
};


#ifdef TEST

main(
    int		argc,
    char	**argv
)
{
    int	i;

    for (i = 0; i < XtNumber(resource_attr_map); i++)
    {
	int		j;
	ResourceAttrMap	*this_map = &(resource_attr_map[i]);

	printf("%s[%d]\n", this_map->resource_name, this_map->num_maps);

	for (j = 0; j < this_map->num_maps; j++)
	{
	    printf("	mask 0x%lx -> offset %d size %d\n",
		this_map->attr_maps[j].obj_mask,
		this_map->attr_maps[j].attr_offset,
		this_map->attr_maps[j].attr_size);
	}
    }
}

#endif /* TEST */


static int
compare_maps(
    const void	*a,
    const void	*b
)
{
    return(strcmp(
	    ((ResourceAttrMap *)a)->resource_name,
	    ((ResourceAttrMap *)b)->resource_name));
}


extern BOOL
abuilP_store_attr_in_abobj(
    ABObj	obj,
    STRING	res_name,
    AB_ARG_TYPE	res_type,
    XtPointer	res_value
)
{
    BOOL		ret_val	= FALSE;
    ResourceAttrMap	key_map;
    ResourceAttrMap	*match;

    key_map.resource_name = res_name;

    /* REMIND: fprintf(stderr,"Checking resource %s\n",res_name); */
    match = (ResourceAttrMap*)bsearch((void *)&key_map,
		resource_attr_map, XtNumber(resource_attr_map),
		sizeof(ResourceAttrMap), compare_maps);

    if (match != (ResourceAttrMap *)NULL && match->num_maps > 0)
    {
	AB_OBJECT_TYPE	ab_type = obj_get_type(obj);
	unsigned long	obj_mask = BMASK(ab_type);
	AttrMap		*maps = match->attr_maps;
	register int	i;

	/* REMIND: fprintf(stderr,"Searching matched maps\n"); */
	for (i = 0; i < match->num_maps; i++)
	{
	    /* REMIND:
	    fprintf(stderr,"\tmask[%d] = %ld, obj = %ld\n", i, 
		maps[i].obj_mask, obj_mask);
	    */
	    if (maps[i].obj_mask & obj_mask && maps[i].attr_size > 0)
	    {
		/* REMIND:
		fprintf(stderr,"Handling resource as type %d\n",
			maps[i].value_type);
		*/
		ret_val = TRUE;
		switch(maps[i].value_type)
		{
		  case VAL_ARG:
		    switch(res_type)
		    {
		      case AB_ARG_STRING:
		      case AB_ARG_XMSTRING:
		      case AB_ARG_LITERAL:
			{
			    ISTRING istr_val = istr_create((STRING)res_value);

			    memmove((char *)obj + maps[i].attr_offset,
					&istr_val, maps[i].attr_size);
			}
			break;
		      default:
			memmove((char *)obj + maps[i].attr_offset,
				&res_value, maps[i].attr_size);
			break;
		    }
		    break;

		  case VAL_CONST:
		    memmove((char *)obj + maps[i].attr_offset,
				&(maps[i].const_value), maps[i].attr_size);
		    break;

		  case VAL_PROC:
		    {

			VAL_PROC_HANDLER handler =
			    (VAL_PROC_HANDLER)maps[i].const_value;

			if (handler != (VAL_PROC_HANDLER)NULL)
			    ret_val = (*handler)(obj,
						res_name, res_type, res_value);
		    }
		    break;

		  case VAL_CONTEXT:
		    {
			void	*ctx_addr = (void *)maps[i].const_value;
			switch(res_type)
			{
			  case AB_ARG_STRING:
			  case AB_ARG_XMSTRING:
			  case AB_ARG_LITERAL:
			    {
				ISTRING istr_val =
				    istr_create((STRING)res_value);

				memmove((char *)ctx_addr + maps[i].attr_offset,
					&istr_val, maps[i].attr_size);
			    }
			    break;
			  case AB_ARG_WIDGET:
			  case AB_ARG_VOID_PTR:
			    {
				ObjRef *ref =
				    (ObjRef *)XtMalloc(sizeof(ObjRef));

				ref->reference = (void *)res_value;
				if (res_type == AB_ARG_WIDGET)
				    ref->is_resolved = FALSE;
				else
				    ref->is_resolved = TRUE;
				memmove((char *)ctx_addr + maps[i].attr_offset,
					&ref, maps[i].attr_size);
			    }
			    break;
			  case AB_ARG_INT:
			    memmove((char *)ctx_addr + maps[i].attr_offset,
				    &res_value, maps[i].attr_size);
			    break;
			}
		    }

		}
	    }
	}
    }
    return(ret_val);
}

extern void	
abuilP_init_context_attrs(
    void
)
{
    int	i;
	
    for (i = 0; i < XtNumber(AbuilP_attr_contexts); i++)
    {
	CONTEXT_INIT_HANDLER handler = AbuilP_attr_contexts[i]->init_handler;

	if (handler != NULL)
	    (*handler)(AbuilP_attr_contexts[i]);
    }
}

extern void
abuilP_store_context_attrs(
    ABObj	ab_widget
)
{
    int	i;
	
    for (i = 0; i < XtNumber(AbuilP_attr_contexts); i++)
    {
	CONTEXT_STORE_HANDLER handler = AbuilP_attr_contexts[i]->store_handler;

	if (handler != NULL)
	    (*handler)(ab_widget, AbuilP_attr_contexts[i]);
    }
}

static void
attach_context_init(
    AnyContext	*context
)
{
    AttachmentContext	*attach_context = (AttachmentContext *)context;
    
    attach_context->att_type = (ISTRING)NULL;
    attach_context->att_object = (ObjRef *)NULL;
    attach_context->att_position = 0;
    attach_context->att_offset   = 0;
}

static void
north_attach_context_store(
    ABObj	ab_widget,
    AnyContext	*attach_context
)
{
    attach_context_store(ab_widget, AB_CP_NORTH, attach_context);
}
static void
south_attach_context_store(
    ABObj	ab_widget,
    AnyContext	*attach_context
)
{
    attach_context_store(ab_widget, AB_CP_SOUTH, attach_context);
}
static void
east_attach_context_store(
    ABObj	ab_widget,
    AnyContext	*attach_context
)
{
    attach_context_store(ab_widget, AB_CP_EAST, attach_context);
}
static void
west_attach_context_store(
    ABObj	ab_widget,
    AnyContext	*attach_context
)
{
    attach_context_store(ab_widget, AB_CP_WEST, attach_context);
}

static void
attach_context_store(
    ABObj		ab_widget,
    AB_COMPASS_POINT	dir,
    AnyContext		*context
)
{
    AttachmentContext	*attach_context = (AttachmentContext *)context;
    STRING		attach_str;
    AB_ATTACH_TYPE	att_type;
    long		att_pos = 0, att_off = 0, offset = 0;
    void		*value  = NULL;
    BOOL		need_val = TRUE;
    ObjRef		*obj_ref;
    
    /* Fetch attachment position out of appropriate context */
    att_pos = attach_context->att_position;
    att_off = attach_context->att_offset;

    /* If no attachment, just return */
    if (attach_context->att_type == (ISTRING)NULL) return;

    /* Get attachment type string */
    attach_str = istr_string(attach_context->att_type);

    /* 
    ** Set up attachment attributes based on attachment type
    **
    ** XmATTACH_NONE           : do nothing - just return
    ** XmATTACH_FORM           : attach directly to the form, plus offset
    **	    value = form widget, offset = XmNfooOffset value (w.r.t. form)
    ** XmATTACH_OPPOSITE_FORM  : attach to opposite side of form, + offset
    **	    value = form widget, offset = XmNfooOffset value (w.r.t. form)
    ** XmATTACH_WIDGET         : attach to opposite side of widget specified 
    **                         : in attach_context
    **	    value = att_object widget, offset = XmNfooOffset (w.r.t. target)
    ** XmATTACH_OPPOSITE_WIDGET: attach to same side of widget specified
    **                         : in attach_context
    **	    value = att_object widget, offset = XmNfooOffset (w.r.t. target)
    ** XmATTACH_POSITION       : attach to position set in attach_context
    **      value = att_position, offset = att_offset
    ** XmATTACH_SELF           :
    */

    if (strcmp(attach_str, "XmATTACH_NONE") == 0)
	return;
    else if (strcmp(attach_str, "XmATTACH_FORM") == 0)
    {
	att_type = AB_ATTACH_OBJ;
	value = obj_get_parent(ab_widget); need_val = FALSE;
	offset = att_off;
    }
    else if (strcmp(attach_str, "XmATTACH_OPPOSITE_FORM") == 0)
    {
	att_type = AB_ATTACH_ALIGN_OBJ_EDGE;
	value = obj_get_parent(ab_widget); need_val = FALSE;
	offset = att_off;
    }
    else if (strcmp(attach_str, "XmATTACH_WIDGET") == 0)
    {
	att_type = AB_ATTACH_OBJ;
	offset = att_off;
    }
    else if (strcmp(attach_str, "XmATTACH_OPPOSITE_WIDGET") == 0)
    {
	att_type = AB_ATTACH_ALIGN_OBJ_EDGE;
	offset = att_off;
    }
    else if (strcmp(attach_str, "XmATTACH_POSITION") == 0)
    {
	att_type = AB_ATTACH_GRIDLINE;
	value = (void *)att_pos; need_val = FALSE;
	offset = att_off;
    }
    else if (strcmp(attach_str, "XmATTACH_SELF") == 0)
	return;


    if (need_val == TRUE && (obj_ref = attach_context->att_object) != NULL)
    {
	if (!obj_ref->is_resolved)
	{
	    RefResolve	*rr = (RefResolve *)XtMalloc(sizeof(RefResolve));
	    
	    rr->obj   = ab_widget;
	    rr->dir   = dir;
	    abuil_add_unresolved_ref((void *)rr);
	}
	value = obj_ref->reference;
	XtFree((char *)obj_ref);
    }
    obj_set_attachment(ab_widget, dir, att_type, value, offset);
}

static BOOL 		
val_set_label_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_label(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_labeltype_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    /* Convert XmPIXMAP or XmSTRING to AB_LABEL_* values */
    if(util_streq((STRING)res_value,"XmPIXMAP")) {
	ret_val = obj_set_label_type(obj,AB_LABEL_GLYPH);
    }
    else {
	ret_val = obj_set_label_type(obj,AB_LABEL_STRING);
    }
    if(ret_val) return(FALSE);
    else        return(TRUE);
}

static BOOL 		
val_set_rows_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_num_rows(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_columns_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_num_columns(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_maximum_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_max_value(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_minimum_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_min_value(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_numcols_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_num_columns(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_orientation_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int 	ret_val;

    /* Convert XmPIXMAP or XmSTRING to AB_LABEL_* values */
    if(util_streq((STRING)res_value,"XmVERTICAL")) {
	ret_val = obj_set_orientation(obj,AB_ORIENT_VERTICAL);
    }
    else {
	ret_val = obj_set_orientation(obj,AB_ORIENT_HORIZONTAL);
    }
    if(ret_val) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_fg_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_fg_color(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_bg_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_bg_color(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_sensitive_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    if(res_value) {
	ret_val = obj_set_is_initially_active(obj,TRUE);
    }
    else {
	ret_val = obj_set_is_initially_active(obj,FALSE);
    }
    if(ret_val) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_mnemonic_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_mnemonic(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_oklabel_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_ok_label(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_autounmanage_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    if(res_value) ret_val = obj_set_auto_dismiss(obj,TRUE);
    else          ret_val = obj_set_auto_dismiss(obj,FALSE);

    if(ret_val) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_directory_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_directory(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_pattern_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_filter_pattern(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_filetype_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    AB_FILE_TYPE_MASK 	ftm = AB_FILE_TYPE_MASK_UNDEF;

    if(util_streq((STRING)res_value,"XmFILE_REGULAR")) {
	ftm = AB_FILE_REGULAR;
    }
    else {
	if(util_streq((STRING)res_value,"XmFILE_DIRECTORY")) {
	    ftm = AB_FILE_DIRECTORY;
	}
        else {
	   if(util_streq((STRING)res_value,"XmFILE_ANY")) {
	    ftm = AB_FILE_ANY;
	   }
	}
    }
    if(obj_set_file_type_mask(obj,ftm)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_maxlen_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_max_length(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_panemax_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_pane_max(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_panemin_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_pane_min(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_intvalue_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_initial_value_int(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_strvalue_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_initial_value_string(obj,(STRING)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_increment_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_increment(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_decpts_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(obj_set_decimal_points(obj,(long)res_value)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_showval_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    if(res_value) ret_val = obj_set_show_value(obj,TRUE);
    else          ret_val = obj_set_show_value(obj,FALSE);

    if(ret_val) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_scrollh_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    if(res_value) ret_val = obj_set_hscrollbar_policy(obj,AB_SCROLLBAR_ALWAYS);
    else          ret_val = obj_set_hscrollbar_policy(obj,AB_SCROLLBAR_NEVER);

    if(ret_val) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_scrollv_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    if(res_value) ret_val = obj_set_vscrollbar_policy(obj,AB_SCROLLBAR_ALWAYS);
    else          ret_val = obj_set_vscrollbar_policy(obj,AB_SCROLLBAR_NEVER);

    if(ret_val) return(FALSE);
    else return(TRUE);
}

static ABuilStrIntMap linestyle_map[] = {
	{"XmSINGLE_LINE",		AB_LINE_SINGLE_LINE},
	{"XmDOUBLE_LINE",		AB_LINE_DOUBLE_LINE},
	{"XmSINGLE_DASHED_LINE",	AB_LINE_SINGLE_DASHED_LINE},
	{"XmDOUBLE_DASHED_LINE",	AB_LINE_DOUBLE_DASHED_LINE},
	{"XmNO_LINE",			AB_LINE_NONE},
	{"XmSHADOW_ETCHED_IN",		AB_LINE_ETCHED_IN},
	{"XmSHADOW_ETCHED_OUT",		AB_LINE_ETCHED_OUT},
	{"XmSHADOW_ETCHED_IN_DASH",	AB_LINE_ETCHED_IN_DASH},
	{"XmSHADOW_ETCHED_OUT_DASH",	AB_LINE_ETCHED_OUT_DASH},
};

static BOOL 		
val_set_linestyle_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int 		i;
    AB_LINE_TYPE	linestyle = AB_LINE_NONE;

    for(i=0;i<XtNumber(linestyle_map);i++) {
	if(util_streq((STRING)res_value,linestyle_map[i].string)) {
		linestyle = (AB_LINE_TYPE) linestyle_map[i].value;
		break;
	}
    }
    /*
      fprintf(stderr,"SeparatorType %s = linestyle %d (#%d)\n",
	res_value,linestyle,i);
    */
    if(obj_set_line_style(obj,linestyle)) return(FALSE);
    else return(TRUE);
}

static ABuilStrIntMap border_map[] = {
	{"XmSHADOW_IN",			AB_LINE_SHADOW_IN},
	{"XmSHADOW_OUT",		AB_LINE_SHADOW_OUT},
	{"XmSHADOW_ETCHED_IN",		AB_LINE_ETCHED_IN},
	{"XmSHADOW_ETCHED_OUT",		AB_LINE_ETCHED_OUT},
};

static BOOL 		
val_set_border_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int 		i;
    AB_LINE_TYPE	border = AB_LINE_NONE;

    for(i=0;i<XtNumber(border_map);i++) {
	if(util_streq((STRING)res_value,border_map[i].string)) {
		border = (AB_LINE_TYPE) border_map[i].value;
		break;
	}
    }
    /* 
      fprintf(stderr,"ShadowType %s = linestyle %d (#%d)\n",res_value,border,i);
    */
    if(obj_set_border_frame(obj,border)) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_wordwrap_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    int ret_val;

    if(res_value) ret_val = obj_set_word_wrap(obj,TRUE);
    else          ret_val = obj_set_word_wrap(obj,TRUE);

    if(ret_val) return(FALSE);
    else return(TRUE);
}

static BOOL 		
val_set_packing_proc(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    if(util_streq((STRING)res_value,"XmPACK_NONE")) {
	/* obj_set_packing(obj,AB_PACK_NONE); */
	obj->info.container.packing = AB_PACK_NONE;
	return(TRUE);
    }
    if(util_streq((STRING)res_value,"XmPACK_TIGHT")) {
	/* obj_set_packing(obj,AB_PACK_TIGHT); */
	obj->info.container.packing = AB_PACK_TIGHT;
	return(TRUE);
    }
    if(util_streq((STRING)res_value,"XmPACK_COLUMN")) {
	/* obj_set_packing(obj,AB_PACK_EQUAL); */
	obj->info.container.packing = AB_PACK_EQUAL;
	return(TRUE);
    }
    return(FALSE);
}

/* This routine is used for map dispatch debugging */
static BOOL
val_proc_debug(
		    ABObj	obj,
		    STRING	res_name,
		    AB_ARG_TYPE	res_type,
		    XtPointer	res_value
)
{
    fprintf(stderr,"Mapping resource \"%s\" for object %s",res_name,
	obj_get_name(obj));
    switch(res_type)
    {
      case AB_ARG_STRING:
      case AB_ARG_LITERAL:
	  fprintf(stderr,", value = %s\n",res_value); break;
      case AB_ARG_INT:
	  fprintf(stderr,", value = %d\n",res_value); break;
      case AB_ARG_FLOAT:
	  fprintf(stderr,", value = %g\n",res_value); break;
      case AB_ARG_BOOLEAN:
	  if(res_value) fprintf(stderr,", value = TRUE\n");
	  else          fprintf(stderr,", value = FALSE\n");
	  break;
      case AB_ARG_XMSTRING:
      case AB_ARG_XMSTRING_TBL:
      case AB_ARG_WIDGET:
      case AB_ARG_PIXEL:
      case AB_ARG_PIXMAP:
      case AB_ARG_FONT:
      case AB_ARG_VOID_PTR:
      case AB_ARG_CALLBACK:
      case AB_ARG_VARIABLE:
      case AB_ARG_MNEMONIC:
      default:
	  fprintf(stderr,"\n");
    }

    return(TRUE);
}
