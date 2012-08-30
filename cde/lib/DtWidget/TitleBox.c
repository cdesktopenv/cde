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
/* $XConsortium: TitleBox.c /main/14 1996/10/14 10:48:11 pascale $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990,1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/**---------------------------------------------------------------------
***	
***	file:		TitleBox.c
***
***	project:	MotifPlus Widgets
***
***	description:	Source code for DtTitleBox class.
***	
***-------------------------------------------------------------------*/


/*-------------------------------------------------------------
**	Include Files
*/

#include <Xm/LabelG.h>
#if RiversVersion == _31
#include <Xm/ManagerP.h>
#include <Xm/GadgetP.h>
#endif /* RiversVersion == _31 */
#include <Xm/DrawP.h>
#include <Xm/XmP.h>
#include <Dt/TitleBoxP.h>
#include <Dt/MacrosP.h>
#include <Dt/DtMsgsP.h>
#include "DtWidgetI.h"

#include <Xm/XmPrivate.h>         /* _XmClearShadowType */



/********    Public Function Declarations    ********/

extern Widget _DtCreateTitleBox( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget _DtTitleBoxGetTitleArea( 
                        Widget w) ;
extern Widget _DtTitleBoxGetWorkArea( 
                        Widget w) ;

#define Max(x, y)    (((x) > (y)) ? (x) : (y))
/********    End Public Function Declarations    ********/

/********    Static Function Declarations    ********/

static void ConfigureChildren( 
                        DtTitleBoxWidget manager) ;
static void GetSize( 
                        DtTitleBoxWidget manager,
                        Dimension ta_w,
                        Dimension ta_h,
                        Dimension wa_w,
                        Dimension wa_h,
                        Dimension *w,
                        Dimension *h) ;
static void GetTitleString( 
                        DtTitleBoxWidget manager,
                        XrmQuark resource,
                        XtArgVal *value) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void Initialize( 
                        DtTitleBoxWidget request,
                        DtTitleBoxWidget new) ;
static void Destroy( 
                        DtTitleBoxWidget manager) ;
static void Resize( 
                        DtTitleBoxWidget manager) ;
static void Redisplay( 
                        DtTitleBoxWidget manager,
                        XEvent *event,
                        Region region) ;
static Boolean SetValues( 
                        DtTitleBoxWidget current,
                        DtTitleBoxWidget request,
                        DtTitleBoxWidget new) ;
static XtGeometryResult QueryGeometry( 
                        DtTitleBoxWidget manager,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static XtGeometryResult GeometryManager( 
                        Widget kid,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged( 
                        DtTitleBoxWidget manager) ;
static void InsertChild( 
                        Widget child) ;
static void ConstraintInitialize( 
                        Widget request,
                        Widget new) ;

/********    End Static Function Declarations    ********/


/*-------------------------------------------------------------
**	Forward Declarations
*/

#define UNSPECIFIED_CHAR	255
#define UNSPECIFIED_DIMENSION	9999
#define MARGIN_DEFAULT		10

#define WARN_CHILD_TYPE		_DtMsgDialogBox_0000
#define WARN_SHADOW_TYPE	_DtMsgIcon_0005
#define WARN_TITLE_POSITION	_DtMsgTitleBox_0000
#define WARN_TITLE_ALIGNMENT	_DtMsgTitleBox_0001

#define MESSAGE2 		_DtMsgTitleBox_0002
#define MESSAGE3 		_DtMsgTitleBox_0003
#define MESSAGE4 		_DtMsgTitleBox_0004

extern void _DtRegisterNewConverters( void ) ;


/*-------------------------------------------------------------
**	Translations and Actions
*/



/*-------------------------------------------------------------
**	Resource List
*/

/*	Define offset macros.
*/
#define TB_Offset(field) \
	XtOffset (DtTitleBoxWidget, title_box.field)
#define TBC_Offset(field) \
	XtOffset (DtTitleBoxConstraintPtr, title_box_constraint.field)

static XtResource resources[] = 
{
	{
		XmNshadowThickness,
		XmCShadowThickness, XmRShort, sizeof (short),
		XtOffset (DtTitleBoxWidget, manager.shadow_thickness),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNmarginWidth,
		XmCMarginWidth, XmRHorizontalDimension, sizeof (Dimension),
		TB_Offset (margin_width),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNmarginHeight,
		XmCMarginHeight, XmRVerticalDimension, sizeof (Dimension),
		TB_Offset (margin_height),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNshadowType,
		XmCShadowType, XmRShadowType, sizeof (unsigned char),
		TB_Offset (shadow_type),
		XmRImmediate, (XtPointer) UNSPECIFIED_CHAR
	},
	{
		XmNtitleSpacing,
		XmCTitleSpacing, XmRHorizontalDimension, sizeof (Dimension),
		TB_Offset (title_spacing),
		XmRImmediate, (XtPointer) UNSPECIFIED_DIMENSION
	},
	{
		XmNtitlePosition,
		XmCTitlePosition, XmRTitlePosition, sizeof (unsigned char),
		TB_Offset (title_position), XmRImmediate, (XtPointer) XmTITLE_TOP
	},
	{
		XmNtitleAlignment,
		XmCAlignment, XmRAlignment, sizeof (unsigned char),
		TB_Offset (title_alignment),
		XmRImmediate, (XtPointer) XmALIGNMENT_BEGINNING
	},
	{
		XmNtitleString,
		XmCXmString, XmRXmString, sizeof (XmString),
		TB_Offset (title_string),
		XmRImmediate, (XtPointer) XmUNSPECIFIED_STRING
	},
	{
		XmNfontList,
		XmCFontList, XmRFontList, sizeof (XmFontList),
		TB_Offset (font_list), XmRImmediate, (XtPointer) NULL
	}
};

/*	Synthetic Resources
*/
static XmSyntheticResource syn_resources[] = 
{
	{
		XmNtitleString, sizeof (XmString),
		TB_Offset (title_string),
		(XmExportProc) GetTitleString,
		(XmImportProc) NULL
	},
	{
		XmNmarginWidth, sizeof (Dimension),
		TB_Offset (margin_width),
		(XmExportProc) XmeFromHorizontalPixels,
		(XmImportProc) XmeToHorizontalPixels
	},
	{
		XmNmarginHeight, sizeof (Dimension),
		TB_Offset (margin_height),
		(XmExportProc) XmeFromVerticalPixels,
		(XmImportProc) XmeToVerticalPixels, 
	}
};

/*	Constraint Resources
*/
static XtResource constraints[] =
{
	{
		XmNchildType,
		XmCChildType, XmRChildType, sizeof (unsigned char),
		TBC_Offset (child_type), XmRImmediate, (XtPointer) XmWORK_AREA
	}
};


/*-------------------------------------------------------------
**	Class Record
*/
DtTitleBoxClassRec dtTitleBoxClassRec =
{
/*	Core Part
*/
	{	
		(WidgetClass) &xmManagerClassRec, /* superclass		*/
		"DtTitleBox",			/* class_name		*/
		sizeof (DtTitleBoxRec),	/* widget_size		*/
		ClassInitialize,		/* class_initialize	*/
		ClassPartInitialize,		/* class_part_initialize*/
		False,				/* class_inited		*/
		(XtInitProc) Initialize,	/* initialize		*/
		NULL,				/* initialize_hook	*/
		XtInheritRealize,		/* realize		*/
		NULL,				/* actions		*/
		0,				/* num_actions		*/
		resources,			/* resources		*/
		XtNumber (resources),		/* num_resources	*/
		NULLQUARK,			/* xrm_class		*/
		True,				/* compress_motion	*/
		True,				/* compress_exposure	*/
		True,				/* compress_enterleave	*/
		False,				/* visible_interest	*/	
		(XtWidgetProc) Destroy,		/* destroy		*/	
		(XtWidgetProc) Resize,		/* resize		*/
		(XtExposeProc) Redisplay,	/* expose		*/	
		(XtSetValuesFunc) SetValues,	/* set_values		*/	
		NULL,				/* set_values_hook	*/
		XtInheritSetValuesAlmost,	/* set_values_almost	*/
		NULL,				/* get_values_hook	*/
		NULL,				/* accept_focus		*/	
		XtVersion,			/* version		*/
		NULL,				/* callback private	*/
		XtInheritTranslations,		/* tm_table		*/
		XtInheritQueryGeometry,		/* query_geometry	*/
		NULL,				/* display_accelerator	*/
		NULL,				/* extension		*/
	},

/*	Composite Part
*/
	{
		(XtGeometryHandler) GeometryManager,/* geometry_manager	*/
		(XtWidgetProc) ChangeManaged,	/* change_managed	*/
		(XtWidgetProc) InsertChild,	/* insert_child		*/
		XtInheritDeleteChild,		/* delete_child		*/
		NULL,				/* extension		*/
	},

/*	Composite Part
*/
	{
		constraints,			/* constraint_resources	*/
		XtNumber (constraints),		/* num_constraint_resources */
		sizeof (DtTitleBoxConstraintRec), /* constraint_record	*/
		(XtInitProc) ConstraintInitialize, /* constraint_initialize */
		NULL,				/* constraint_destroy	*/
		NULL,				/* constraint_set_values */
		NULL,				/* extension		*/
	},

/*	XmManager Part
*/
	{
		XtInheritTranslations,		/* default_translations	*/
		syn_resources,			/* syn_resources	*/
		XtNumber (syn_resources),	/* num_syn_resources	*/
		NULL,				/* syn_cont_resources	*/
		0,				/* num_syn_cont_resources */
		XmInheritParentProcess,		/* parent_process	*/
		NULL,				/* extension		*/
	},

/*	DtTitleBox Part
*/
	{
		NULL,				/* extension		*/
	}
};

WidgetClass dtTitleBoxWidgetClass = (WidgetClass) &dtTitleBoxClassRec;



/*-------------------------------------------------------------
**	Private Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------------------
**	ConfigureChildren
**		Set positions and sizes of title and work area.
*/
static void 
ConfigureChildren(
        DtTitleBoxWidget manager )
{
	Widget			title_area = M_TitleArea (manager),
				work_area = M_WorkArea (manager);
	Position		ta_x = 0, ta_y = 0, wa_x = 0, wa_y = 0;
	Dimension		ta_w = 0, ta_h = 0, ta_bw = 0,
				wa_w = 0, wa_h = 0, wa_bw = 0,
				w = M_Width (manager),
				h = M_Height (manager),
				s_t = M_ShadowThickness (manager),
				m_w = M_MarginWidth (manager),
				m_h = M_MarginHeight (manager),
				ta_sp = M_TitleSpacing (manager);
	Boolean			title_top;
	unsigned char		align = M_TitleAlignment (manager);
	XtWidgetGeometry	ta_reply;

	title_top = (M_TitlePosition (manager) == XmTITLE_TOP) ? True : False;

/*	Set position and size of title area.
*/
	if (title_area)
	{
		XtQueryGeometry (title_area, NULL, &ta_reply);
		ta_w = ta_reply.width;
		ta_h = ta_reply.height;

		if (ta_w + 2U * ta_sp > (M_Width (manager) - 2U * ta_sp))
			ta_w = M_Width (manager) - 2 * ta_sp;
		if (ta_w == 0)
			ta_w = 10;
		if (ta_h == 0)
			ta_h = 10;

		if (align == XmALIGNMENT_BEGINNING)
			ta_x = ta_sp;
		else if (align == XmALIGNMENT_END)
			ta_x = w - ta_sp - ta_w;
		else
			ta_x = (w - ta_w) / 2U;

		ta_y = (title_top) ? 0 : h - ta_h;

		ta_bw = P_BorderWidth (title_area);
		XmeConfigureObject (title_area, ta_x, ta_y, ta_w, ta_h, ta_bw);
	}

/*	Set position and size of work area.
*/
	if (work_area)
	{
		wa_x = m_w + s_t;
		wa_y = (title_top) ? Max (s_t, ta_h) + m_h : s_t + m_h;
		wa_w = w - 2 * (m_w + s_t);
		if (wa_w == 0)
			wa_w = 10;
		wa_h = h - Max (s_t, ta_h) - s_t - 2 * m_h;
		if (wa_h == 0)
			wa_h = 10;
		wa_bw = P_BorderWidth (work_area);
		XmeConfigureObject (work_area,
					wa_x, wa_y, wa_w, wa_h, wa_bw);
	}
}



/*-------------------------------------------------------------------------
**	GetSize
**		Calculate desired size based on children.
*/
static void 
GetSize(
        DtTitleBoxWidget manager,
        Dimension ta_w,
        Dimension ta_h,
        Dimension wa_w,
        Dimension wa_h,
        Dimension *w,
        Dimension *h )
{
	Widget			title_area = M_TitleArea (manager),
				work_area = M_WorkArea (manager);
	XtWidgetGeometry	ta_reply, wa_reply;
	Dimension		s_t = M_ShadowThickness (manager),
				m_w = M_MarginWidth (manager),
				m_h = M_MarginHeight (manager),
				ta_sp = M_TitleSpacing (manager);


/*	Optimize title area change if possible.
*/
	if (work_area && (wa_w == 0) && (wa_h == 0))
	{
		if ((ta_w != 0) && (ta_h == 0))
		{
			if ((M_Width (manager) >=
				M_Width (work_area) + (2U * ta_sp)) &&
			    (M_Height (manager) >= Max (ta_h, s_t) +
				M_Height (work_area) + s_t + (2U * m_h)))
			{
				wa_w = M_Width (work_area);
				wa_h = M_Height (work_area);
			}
		}

		if ((wa_w == 0) && (wa_h == 0))
		{	
			XtQueryGeometry (work_area, NULL, &wa_reply);
			wa_w = wa_reply.width;
			wa_h = wa_reply.height;
		}
	}
	if (title_area && (ta_w == 0) && (ta_h == 0))
	{
		XtQueryGeometry (title_area, NULL, &ta_reply);
		ta_w = ta_reply.width;
		ta_h = ta_reply.height;
	}

	*w = Max (wa_w + (2U * (m_w + s_t)), ta_w + (2U * ta_sp));
	if (*w == 0)
		*w = 10;
	*h = Max (s_t, ta_h) + wa_h + (2 * m_h) + s_t;
	if (*h == 0)
		*h = 10;
}



/*-------------------------------------------------------------
**	GetTitleString
**		Get string from title label.
*/
static void 
GetTitleString(
        DtTitleBoxWidget manager,
        XrmQuark resource,
        XtArgVal *value )
{
	Arg		al[10];		/*  arg list		*/
	register int	ac;		/*  arg count		*/
	Widget		title_area = M_TitleArea (manager);
	XmString	string = NULL;

	if (title_area)
	{
		ac = 0;
		XtSetArg (al[ac], XmNlabelString, &string);  ac++;
		XtGetValues (title_area, al, ac);
	}

	*value = (XtArgVal) string;
}



/*-------------------------------------------------------------
**	Action Procs
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	Core Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------------------
**	ClassInitialize
**		Initialize widget class.
*/
static void 
ClassInitialize( void )
{
	_DtRegisterNewConverters ();
}


/*-------------------------------------------------------------------------
**	ClassPartInitialize
**		Initialize widget class data.
*/
static void 
ClassPartInitialize(
        WidgetClass wc )
{
}



/*-------------------------------------------------------------
**	Initialize
**		Initialize a new widget instance.
*/
static void 
Initialize(
        DtTitleBoxWidget request,
        DtTitleBoxWidget new )
{
	Arg		al[10];		/*  arg list		*/
	register int	ac;		/*  arg count		*/
	Widget		title = NULL;
	XmString	title_string = NULL;

/*	Check for unspecified dimensions.
*/
	if (M_MarginWidth (request) == UNSPECIFIED_DIMENSION)
		M_MarginWidth (new) = MARGIN_DEFAULT;
	if (M_MarginHeight (request) == UNSPECIFIED_DIMENSION)
		M_MarginHeight (new) = MARGIN_DEFAULT;
	if (M_TitleSpacing (request) == UNSPECIFIED_DIMENSION)
		M_TitleSpacing (new) = UNSPECIFIED_DIMENSION;
	if (M_ShadowThickness (new) == UNSPECIFIED_DIMENSION)
	{
		if (XtIsShell (XtParent (new)))
			M_ShadowThickness (new) = 1;
		else
			M_ShadowThickness (new) = 2;
	}

/*	Validate shadow type.
*/
	if (M_ShadowType (new) == UNSPECIFIED_CHAR)
	{
		if (XtIsShell (XtParent (new)))
			M_ShadowType (new) = XmSHADOW_OUT;
		else
			M_ShadowType (new) = XmSHADOW_ETCHED_IN;
	}
	else if (M_ShadowType (new) != XmSHADOW_IN &&
		 M_ShadowType (new) != XmSHADOW_OUT &&
		 M_ShadowType (new) != XmSHADOW_ETCHED_IN &&
		 M_ShadowType (new) != XmSHADOW_ETCHED_OUT)
	{
	      XmeWarning ((Widget)new, WARN_SHADOW_TYPE);
	}

/*	Validate title position.
*/
	if (M_TitlePosition (new) != XmTITLE_TOP &&
	    M_TitlePosition (new) != XmTITLE_BOTTOM)
	{
		XmeWarning ((Widget)new, WARN_TITLE_POSITION);
	}

/*	Validate title alignment.
*/
	if (M_TitleAlignment (new) != XmALIGNMENT_BEGINNING &&
	    M_TitleAlignment (new) != XmALIGNMENT_CENTER &&
	    M_TitleAlignment (new) != XmALIGNMENT_END)
	{
		XmeWarning ((Widget)new, WARN_TITLE_ALIGNMENT);
	}

/*	Check width and height.
*/
  	if (M_Width (new) == 0)
	{
		M_Width (new) = 2 * (M_ShadowThickness (new) +
					M_MarginWidth (new));
	  	if (M_Width (new) == 0)
			M_Width (new) = 10;
	}
  	if (M_Height (new) == 0)
	{
		M_Height (new) = 2 * (M_ShadowThickness (new) +
					M_MarginHeight (new));
	  	if (M_Height (new) == 0)
			M_Height (new) = 10;
	}

	M_OldWidth (new) = M_Width (new);
	M_OldHeight (new) = M_Height (new);
	M_OldShadowThickness (new) = M_ShadowThickness (new);


	M_WorkArea (new) = NULL;

/*	Create title_area unless title explicitly set to null;
*	use name if unspecified.
*/
	if (! M_TitleString (new))
	{
		M_TitleArea (new) = NULL;
		return;
	}

	if (M_TitleString (new) == XmUNSPECIFIED_STRING && M_Name (new))
	{
		M_TitleString (new) = XmStringCreateLocalized(M_Name (new));
		title_string = M_TitleString (new);
	}
	ac = 0;
	XtSetArg (al[ac], XmNchildType, XmTITLE_AREA);  ac++;
	XtSetArg (al[ac], XmNmarginWidth, 2);  ac++;
	XtSetArg (al[ac], XmNmarginHeight, 0);  ac++;
	XtSetArg (al[ac], XmNshadowThickness, 0);  ac++;
	XtSetArg (al[ac], XmNhighlightThickness, 0);  ac++;
	XtSetArg (al[ac], XmNlabelString, M_TitleString (new));  ac++;
	if (M_FontList (new))
	{
		XtSetArg (al[ac], XmNfontList, M_FontList (new));
		ac++;
	}
	title = XmCreateLabelGadget ((Widget)new, "title", al, ac);
	M_TitleArea (new) = title;
	XtManageChild (title);

	if (M_TitleSpacing (new) == UNSPECIFIED_DIMENSION)
		M_TitleSpacing (new) =
			Max (P_Height (title)/2U, M_ShadowThickness (new));

	M_TitleString (new) = XmUNSPECIFIED_STRING;
	M_FontList (new) = NULL;
	if (title_string != NULL)
		XmStringFree (title_string);
}


/*-------------------------------------------------------------
**	Destroy
**		Release resources allocated for widget instance.
*/
static void 
Destroy(
        DtTitleBoxWidget manager )
{
	/*	Superclass does all the work so far.
	*/
}



/*-------------------------------------------------------------
**	Resize
**		Update size of children.
*/
static void 
Resize(
        DtTitleBoxWidget manager )
{
	Widget		title_area = M_TitleArea (manager);
	Dimension	s_t = M_OldShadowThickness (manager),
			w = M_OldWidth (manager),
			h = M_OldHeight (manager);
	Position	y;

/*	Clear shadow and save shadow data.
*/
	if (XtIsRealized ((Widget)manager))
	{
		_XmClearShadowType ((Widget)manager, w, h, s_t, 0);

		if ((M_TitlePosition (manager) == XmTITLE_BOTTOM) &&
		    (M_Height (manager) > h))
		{
			y = h - P_Height (title_area);
			XClearArea (XtDisplay (manager), XtWindow (manager),
					0, y, M_Width (manager),
					M_Height (manager) - y, False);
		}			
	}
				
	M_OldWidth (manager) = M_Width (manager);
	M_OldHeight (manager) = M_Height (manager);
	M_OldShadowThickness (manager) = M_ShadowThickness (manager);

	ConfigureChildren (manager);

/*	Draw shadow and title.
*/
	if (XtIsRealized ((Widget)manager))
		Redisplay (manager, NULL, NULL);
}



/*-------------------------------------------------------------
**	Redisplay
**		Redisplay widget.
*/
static void 
Redisplay(
        DtTitleBoxWidget manager,
        XEvent *event,
        Region region )
{
	Widget		title_area = M_TitleArea (manager),
			work_area = M_WorkArea (manager);
	Position	title_x = M_X (title_area),
			title_y = M_Y (title_area);
	Dimension	title_width = M_Width (title_area),
			title_height = M_Height (title_area);

	Display *	d = 		XtDisplay (manager);
	Dimension	w = 		M_Width (manager),
			h =	 	M_Height (manager),
			s_t = 		M_ShadowThickness (manager);

	GC		top_gc = 	NULL,
			bottom_gc = 	NULL,
			fill_gc = 	NULL;
	Boolean		s_out = 	False,
			s_etched = 	False;
	Drawable	drawable = 	XtWindow (manager);
	Position	x = 0,
			y = title_height / 2 - s_t / 2;

/*	Redisplay work area.
*/
	if (work_area && XmIsGadget (work_area) && XtIsManaged (work_area))
	{
		XtExposeProc expose;
		_DtProcessLock();
		expose = XtCoreProc(work_area, expose);
		_DtProcessUnlock();

		(*expose) (work_area, event, region);
	}

/*	Draw shadow.
*/
	if (M_ShadowThickness (manager) > 0)
	{
		h -= y;
		if (M_TitlePosition (manager) == XmTITLE_BOTTOM)
			y = 0;
		XmeDrawShadows(d, XtWindow(manager),
			       M_TopShadowGC(manager),
			       M_BottomShadowGC(manager),
			       x, y, w, h, s_t, M_ShadowType(manager));
	}

/*	Redisplay title area.
*/	
	if (title_area && XtIsManaged (title_area))
	{		
		XClearArea (d, XtWindow (manager), title_x, title_y,
				title_width, title_height, False);
		if (XmIsGadget (title_area))
		{
			XtExposeProc expose;
			_DtProcessLock();
			expose = XtCoreProc(title_area, expose);
			_DtProcessUnlock();

			(*expose) (title_area, event, region);
		}
	}

}



/*-------------------------------------------------------------
**	SetValues
**		Handle changes in resource data.
*/


static Boolean 
SetValues(
        DtTitleBoxWidget current,
        DtTitleBoxWidget request,
        DtTitleBoxWidget new )
{
	Widget		title = M_TitleArea (new);
	Boolean		redisplay_flag = False,
			new_title = False,
			new_font = False;
	Arg		al[10];		/*  arg list		*/
	register int	ac;		/*  arg count		*/

/*	Validate title position.
*/
	if (M_TitlePosition (new) != M_TitlePosition (current) &&
	    M_TitlePosition (new) != XmTITLE_TOP &&
	    M_TitlePosition (new) != XmTITLE_BOTTOM)
	{
		XmeWarning ((Widget)new, WARN_TITLE_POSITION);
		M_TitlePosition (new) = M_TitlePosition (current);
	}

/*	Validate title alignment.
*/
	if (M_TitleAlignment (new) != M_TitleAlignment (current) &&
	    M_TitleAlignment (new) != XmALIGNMENT_BEGINNING &&
	    M_TitleAlignment (new) != XmALIGNMENT_CENTER &&
	    M_TitleAlignment (new) != XmALIGNMENT_END)
	{
		XmeWarning ((Widget)new, WARN_TITLE_ALIGNMENT);
		M_TitleAlignment (new) = M_TitleAlignment (current);
	}

/*	Validate shadow type.
*/
	if (M_ShadowType (new) != M_ShadowThickness (current) &&
	    M_ShadowType (new) != XmSHADOW_IN &&
	    M_ShadowType (new) != XmSHADOW_OUT &&
	    M_ShadowType (new) != XmSHADOW_ETCHED_IN &&
	    M_ShadowType (new) != XmSHADOW_ETCHED_OUT)
	{
		XmeWarning ((Widget)new, WARN_SHADOW_TYPE);
		M_ShadowType (new) = M_ShadowThickness (current);
	}

/*	Check for redisplay; query and update kids if no resize.
*/
	if (M_MarginWidth (new) != M_MarginWidth (current) ||
	    M_MarginHeight (new) != M_MarginHeight (current) ||
	    M_ShadowThickness (new) != M_ShadowThickness (current))
	{
		redisplay_flag = True;
		if (M_Width (new) == M_Width (current) &&
		    M_Height (new) == M_Height (current))
		{
			GetSize (new, 0, 0, 0, 0,
				&(M_Width (new)), &(M_Height (new)));
			ConfigureChildren (new);
		}
	}

	else if (M_TitleSpacing (new) != M_TitleSpacing (current) ||
		 M_TitlePosition (new) != M_TitlePosition (current) ||
		 M_TitleAlignment (new) != M_TitleAlignment (current))
	{
		redisplay_flag = True;
		ConfigureChildren (new);
	}
	else if (M_ShadowType (new) != M_ShadowThickness (current))
	{
		redisplay_flag = True;
	}

/*	Update title if string or font changed.
*/
	ac = 0;
	if (M_TitleString (current) != M_TitleString (new))
	{
		new_title = True;
		XtSetArg (al[ac], XmNlabelString, M_TitleString (new));  ac++;
	}
	if (M_FontList (current) != M_FontList (new))
	{
		new_font = True;
		XtSetArg (al[ac], XmNfontList, M_FontList (new));
		ac++;
	}
	if (ac && title)
	{
		XtSetValues (title, al, ac);
		if (new_title)
			M_TitleString (new) = XmUNSPECIFIED_STRING;
		if (new_font)
			M_FontList (new) = NULL;
		redisplay_flag = True;
	}

	return (redisplay_flag);
}



/*-------------------------------------------------------------
**	QueryGeometry
**		Handle query geometry request.
*/
static XtGeometryResult 
QueryGeometry(
        DtTitleBoxWidget manager,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
	Position	x = M_X (manager),
			y = M_Y (manager);
	Dimension	w = M_Width (manager),
			h = M_Height (manager),
			bw = M_BorderWidth (manager),
			new_w = 0,
			new_h = 0;
	Boolean		x_req = request->request_mode & CWX,
			y_req = request->request_mode & CWY,
			width_req = request->request_mode & CWWidth,
			height_req = request->request_mode & CWHeight,
			bw_req = request->request_mode & CWBorderWidth;

/*	Compute preferred size if preferred width or height requested.
*/
	if (width_req || height_req)
		GetSize (manager, 0, 0, 0, 0, &new_w, &new_h);

/*	Load reply.
*/
	reply->request_mode = request->request_mode;
	reply->x = request->x;
	reply->y = request->y;
	reply->width = (width_req) ? new_w : request->width;
	reply->height = (height_req) ? new_h : request->height;
	reply->border_width = request->border_width;

/*	If no change return no; otherwise yes.
*/
	if ((!width_req || (width_req && w == new_w)) &&
	    (!height_req || (height_req && h == new_h)))
		return (XtGeometryNo);
	else
		return (XtGeometryYes);		
}



/*-------------------------------------------------------------
**	Composite Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	GeometryManager
**		Handle geometry request from title area or work area.
*/
static XtGeometryResult 
GeometryManager(
        Widget kid,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
	DtTitleBoxWidget	manager = (DtTitleBoxWidget) XtParent (kid);
	Widget		title_area = M_TitleArea (manager),
			work_area = M_WorkArea (manager);
	Dimension	ta_w = 0, ta_h = 0, ta_bw = 0,
			wa_w = 0, wa_h = 0,
			w = M_Width (manager),
			h = M_Height (manager),
			sp = M_TitleSpacing (manager);
	Boolean		query_only = request->request_mode & XtCWQueryOnly,
			x_req = request->request_mode & CWX,
			y_req = request->request_mode & CWY,
			width_req = request->request_mode & CWWidth,
			height_req = request->request_mode & CWHeight,
			bw_req = request->request_mode & CWBorderWidth,
			almost = False;
	XtGeometryResult	result, parent_result;
	XtWidgetGeometry	parent_req, parent_reply;
	

/*	Check for width, height, and borderwidth requests.
*/
	if (!width_req && !height_req)
	{
		if (x_req || y_req)
			return (XtGeometryNo);
		else
			return (XtGeometryYes);
	}
	else
	{
		if (x_req || y_req)
			almost = True;
	}

/*	Compute desired size.
*/
	if (kid == title_area)
	{
		ta_w = (width_req) ? request->width : P_Width (kid);
		ta_h = (height_req) ? request->height : P_Height (kid);
		ta_bw = (bw_req) ? request->border_width : P_BorderWidth (kid);
	}
	else
	{
		wa_w = (width_req) ? request->width : P_Width (kid);
		wa_h = (height_req) ? request->height : P_Height (kid);
	}

	GetSize (manager, ta_w, ta_h, wa_w, wa_h, &w, &h);

/*	Request change from parent if necessary.
*/
	if (w != M_Width (manager) || h != M_Height (manager))
	{
		parent_req.request_mode = CWWidth | CWHeight;
		parent_req.width = w;
		parent_req.height = h;
		if (almost || query_only)
			parent_req.request_mode |= XtCWQueryOnly;
		parent_result =
			XtMakeGeometryRequest ((Widget)manager, &parent_req,
					&parent_reply);
		if (kid == (Widget) title_area)
			parent_result = XtGeometryYes;
	}
	else
		parent_result = XtGeometryYes;

/*	Reply to kid based on reply from parent.
*/
	switch ((int) parent_result)
	{
		case XtGeometryYes:
			if (query_only)
				result = XtGeometryYes;
			else if (!almost)
			{
				XtWidgetProc resize;
				if (kid == title_area)
				      XmeConfigureObject (title_area,
					     title_area->core.x,
					     title_area->core.y,
					     ta_w, ta_h, ta_bw);
				_DtProcessLock();
				resize = XtCoreProc(XtParent(kid), resize);
				_DtProcessUnlock();
				(*resize) (XtParent (kid));
				result = XtGeometryDone;
			}
			else
			{
				result = XtGeometryNo;
			}
			break;
		case XtGeometryAlmost:
		case XtGeometryNo:
			result = XtGeometryNo;
			break;
	}

	return (result);
}


/*-------------------------------------------------------------
**	ChangeManaged
**		Handle change in set of managed children.
*/
static void 
ChangeManaged(
        DtTitleBoxWidget manager )
{
	Dimension	w = M_Width (manager),
			h = M_Height (manager);

/*	Compute desired size.
*/
	GetSize (manager, 0, 0, 0, 0, &w, &h);

/*	Try to change size to fit children
*/
	if (w != M_Width (manager) || h != M_Height (manager))
	{
		switch (XtMakeResizeRequest ((Widget) manager, w, h, &w, &h))
		{
			case XtGeometryAlmost:
				XtMakeResizeRequest ((Widget) manager, w, h,
					NULL, NULL);
			case XtGeometryYes:
			case XtGeometryNo:
			default:
				break;
		}
	}
	
/*	Set positions and sizes of children.
*/
	ConfigureChildren (manager);
	XmeNavigChangeManaged ((Widget)manager);
}



/*-------------------------------------------------------------
**	InsertChild
**		Add a child.
*/
static void 
InsertChild(
        Widget child )
{
	DtTitleBoxConstraint	constraint = (DtTitleBoxConstraint)
					M_TitleBoxConstraint (child);
	DtTitleBoxWidget	w;
	XmManagerWidgetClass	mc = (XmManagerWidgetClass)
						xmManagerWidgetClass;
	unsigned char		child_type;
	XtWidgetProc		insert_child;

	_DtProcessLock();
	insert_child = mc->composite_class.insert_child;
	_DtProcessUnlock();
	(* insert_child) (child);

	w = (DtTitleBoxWidget) XtParent (child);
	
	if (constraint->child_type == XmWORK_AREA)
	{
		if (! M_WorkArea (w))
		{
			M_WorkArea (w) = child;
		}
	}
	else if (constraint->child_type == XmTITLE_AREA)
	{
		if (! M_TitleArea (w))
		{
			M_TitleArea (w) = child;
		}
	}
}


 
/*-------------------------------------------------------------
**	Constraint Procs
**-------------------------------------------------------------
*/
/*-------------------------------------------------------------
**	ConstraintInitialize
**		Add a child.
*/
static void 
ConstraintInitialize(
        Widget request,
        Widget new )
{
	DtTitleBoxWidget	manager = (DtTitleBoxWidget) XtParent (new);
	DtTitleBoxConstraint	constraint = M_TitleBoxConstraint (new);

/*	Validate child type.
*/
	if (constraint->child_type != XmWORK_AREA &&
	    constraint->child_type != XmTITLE_AREA)
	{
		XmeWarning (new, WARN_CHILD_TYPE);
		if (! M_WorkArea (manager))
		{
			constraint->child_type = XmWORK_AREA;
		}
		else if (! M_TitleArea (manager))
		{
			constraint->child_type = XmTITLE_AREA;
		}
	}
}


/*-------------------------------------------------------------
**	Manager Procs
**-------------------------------------------------------------
*/
/*	All inherited from superclass.
*/



/*-------------------------------------------------------------
**	Public Entry Points
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	_DtCreateTitleBox
**		Create a new DtTitleBox instance.
**-------------------------------------------------------------
*/
Widget 
_DtCreateTitleBox(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
	return (XtCreateWidget (name, dtTitleBoxWidgetClass, 
			parent, arglist, argcount));
}


/*-------------------------------------------------------------
**      _DtTitleBoxGetTitleArea
**		Return TitleBox title area;
**-------------------------------------------------------------
*/
Widget 
_DtTitleBoxGetTitleArea(
        Widget w )
{
	DtTitleBoxWidget	mgr =	(DtTitleBoxWidget) w;
	Widget			rtn_w =	M_TitleArea (mgr);

	return (rtn_w);
}


/*-------------------------------------------------------------
**      _DtTitleBoxGetWorkArea
**		Return TitleBox work area;
**-------------------------------------------------------------
*/
Widget 
_DtTitleBoxGetWorkArea(
        Widget w )
{
	DtTitleBoxWidget	mgr =	(DtTitleBoxWidget) w;
	Widget			rtn_w =	M_WorkArea (mgr);

	return (rtn_w);
}
