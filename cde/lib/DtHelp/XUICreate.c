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
/* $TOG: XUICreate.c /main/22 1997/06/18 17:33:46 samborn $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        XUICreate.c
 **
 **   Project:     Cde Help System
 **
 **   Description: Builds a GUI containing a Drawn Button with scroll
 **                bars and wraps it around a canvas.
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992,
                 1993, 1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*
 * system includes
 */
#include <stdlib.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/AtomMgr.h>
#include <Xm/DrawnB.h>
#include <Xm/Form.h>
#include <Xm/ScrollBar.h>

/*
 * Canvas Engine
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "bufioI.h"
#include "DisplayAreaP.h"
#include "CallbacksI.h"
#include "XUICreateI.h"
#include "FontI.h"
#include "FontAttrI.h"
#include "DestroyI.h"
#include "FormatUtilI.h"
#include "SetListI.h"
#include "XInterfaceI.h"
#include "Lock.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
static  Boolean    get_fontsetproperty(
                        XFontSet        fontset,
                        Atom            atom,
                        unsigned long   *value_return);
/********    End Private Function Declarations    ********/

/********    Private Variables    ********/

static	_DtCvVirtualInfo DefVirtFunctions =
  {
	_DtHelpDAGetCvsMetrics,		/* void (*_DtCvGetMetrics)()     */
	_DtHelpDARenderElem,		/* void (*_DtCvRenderElem)()     */
	_DtHelpDAGetStrWidth,		/* _DtCvUnit (*_DtCvGetElemWidth)()  */
	_DtHelpDAGetFontMetrics,	/* void (*_DtCvGetFontMetrics)() */
	_DtHelpDABuildSelection,	/* void (*_DtCvBuildSelection)() */
	NULL,				/* _DtCvStatus (*_DtCvFilterExecCmd)()*/
  };

/********    End Private Variables  ********/

/********    Semi-Private Variables    ********/
/********    End Semi-Private Variables  ********/

/******************************************************************************
 *
 * Here are the type and variable declarations for finding view subresources.
 *
 *****************************************************************************/

/*
 * These are used to parse subresources for the text display areas.
 */

typedef struct 
  {
    int    marginWidth;
    int    marginHeight;
    int    leading;
    int    moveThreshold;
    int    initialDelay;
    int    repeatDelay;

    XmFontList userFont;
    Pixel  search_color;
  }
  DAArgs, *DAArgsPtr;

static XtResource Resources[] = {
   {
     "leading", "Leading", XmRInt, sizeof(int),
     XtOffset(DAArgsPtr, leading), XmRImmediate, (XtPointer) 1
   },
    
   {
     XmNmarginWidth, XmCMarginWidth, XmRInt, sizeof(int),
     XtOffset(DAArgsPtr, marginWidth), XmRImmediate, (XtPointer) 5
   },
    
   {
     XmNmarginHeight, XmCMarginHeight, XmRInt, sizeof(int),
     XtOffset(DAArgsPtr, marginHeight), XmRImmediate, (XtPointer) 5
   },

   {
     "moveThreshold", "MoveThreshold", XmRInt, sizeof(int),
     XtOffset(DAArgsPtr, moveThreshold), XmRImmediate, (XtPointer) 10
   },
    
   {
     XmNinitialDelay, XmCInitialDelay, XmRInt, sizeof(int),
     XtOffset(DAArgsPtr, initialDelay), XmRImmediate, (XtPointer) 250
   },
    
   {
     XmNrepeatDelay, XmCRepeatDelay, XmRInt, sizeof(int),
     XtOffset(DAArgsPtr, repeatDelay), XmRImmediate, (XtPointer) 50
   },
    
   {
     "userFont", XmCFontList, XmRFontList, sizeof (XmFontList),
     XtOffset(DAArgsPtr, userFont), XmRString, "Fixed"
   },

   {
     "searchColor", "SearchColor", XtRPixel, sizeof (Pixel),
     XtOffset(DAArgsPtr, search_color), XmRString, "blue"
   },

};

static String	    DrawnBTransTable = "\
~s    ~m ~a <Key>Return:      ActivateLink()\n\
~s  c ~m ~a <Key>backslash:   DeSelectAll()\n\
~s  c ~m ~a <Key>slash:       SelectAll()\n\
~s    ~m ~a <Key>space:       ActivateLink()\n\
            <Key>osfActivate: ActivateLink()\n\
            <Key>osfCopy:     CopyToClipboard()\n\
   ~c       <Key>osfDown:     NextLink(1)\n\
    c       <Key>osfDown:     PageUpOrDown(1)\n\
~s  c ~m ~a <Key>osfInsert:   CopyToClipboard()\n\
   ~c       <Key>osfLeft:      NextLink(0)\n\
    c       <Key>osfLeft:      PageLeftOrRight(0)\n\
   ~c       <Key>osfPageDown:  PageUpOrDown(1)\n\
    c       <Key>osfPageDown:  PageLeftOrRight(1)\n\
            <Key>osfPageLeft:  PageLeftOrRight(0)\n\
            <Key>osfPageRight: PageLeftOrRight(1)\n\
   ~c       <Key>osfPageUp:    PageUpOrDown(0)\n\
    c       <Key>osfPageUp:    PageLeftOrRight(0)\n\
   ~c       <Key>osfRight:     NextLink(1)\n\
    c       <Key>osfRight:     PageLeftOrRight(1)\n\
~s ~c       <Key>osfSelect:    ActivateLink()\n\
   ~c       <Key>osfUp:        NextLink(0)\n\
    c       <Key>osfUp:        PageUpOrDown(0)\n\
    c       <Key>osfBeginLine: NextLink(2)\n\
    c       <Key>osfEndLine:   NextLink(3)\
";

static	XtTranslations	DrawnBTrans = NULL;

/*********************************************************************
 *		Private Functions
 *********************************************************************/
/* This function returns the max value of XGetFontProperty calls for each
 * font in the fontset.
 */
static Boolean
get_fontsetproperty(
        XFontSet fontset,
        Atom atom,
        unsigned long *value_return)
{
    int numfont;
    XFontStruct **font_list;
    char **name_list;
    int i;
    Bool ret = FALSE;
    unsigned long value;

    numfont=XFontsOfFontSet(fontset,&font_list,&name_list);
    for(i = 0; i < numfont; i++) {
        if(XGetFontProperty(font_list[i], atom, &value) == TRUE) {
            if(ret == FALSE) {
                *value_return = value;
                ret = TRUE;
            }
            else {
                if(value > *value_return)
                    *value_return = value;
            }
        }
    }
    return(ret);
}

/*
 * This function returns the underline distance and size for the base
 * font.
 */
static	void
GetUnderLineInfo (
	Display			*dpy,
	DtHelpDispAreaStruct	*pDAS,
	int			*ret_underThick )
{
    int   idx;
    XtPointer myFont;
    XFontSetExtents *extents;

    /*
     * cast the parameter as a pointer to a long though the value
     * returned is really a int.
     */
    idx = __DtHelpDefaultFontIndexGet(pDAS);
    if (idx < 0)
      {
        myFont = (XtPointer)__DtHelpFontSetGet(pDAS->font_info, idx);
        if (get_fontsetproperty((XFontSet)myFont, XA_UNDERLINE_POSITION,
                ((unsigned long *) &(pDAS->underLine))) == FALSE)
	  {
            extents = XExtentsOfFontSet((XFontSet)myFont);
            pDAS->underLine = ((int)(extents->max_ink_extent.height +
                               extents->max_ink_extent.y)) / 2;
          }
      }
    else
      {
        myFont = (XtPointer)__DtHelpFontStructGet(pDAS->font_info, idx);
        if (XGetFontProperty((XFontStruct *)myFont, XA_UNDERLINE_POSITION, 
			     ((unsigned long *) &(pDAS->underLine))) == FALSE)
            pDAS->underLine = ((XFontStruct *)myFont)->max_bounds.descent / 2;
      }

    /*
     * if we do not find an underline thickness for the font
     * use 15% of the font height.
     *
     * cast the parameter to a pointer to a long though the value
     * returned is really a unsigned int.
     */
    if (idx < 0)
      {
        if (get_fontsetproperty((XFontSet)myFont, XA_UNDERLINE_THICKNESS,
                ((unsigned long *) ret_underThick)) == FALSE)
            *ret_underThick = pDAS->lineHeight * 15 / 100;
      }
    else
      {
        if (XGetFontProperty((XFontStruct *)myFont, XA_UNDERLINE_THICKNESS, 
			     ((unsigned long *) ret_underThick)) == FALSE)
            *ret_underThick = pDAS->lineHeight * 15 / 100;
      }


    /*
     * now adjust the underline depth so that the when the underline is
     * drawn the top of the line is UNDERLINE_POSITION pixels below
     * the base line.
     */
/*
 * SYSTEM - ifdef this for other architectures.
 * an example of Hewlett-Packard's implementation is given for reference.
 */
		/* On Hewlett-Packard machines lines are */
		/* drawn like this:                      */
		/*					 */
		/*  line width 1:  y-> ****************  */
		/*					 */
		/*		       ****************  */
		/*  line width 2:  y-> ****************  */
		/*					 */
		/*		       ****************  */
		/*  line width 3:  y-> ****************  */
		/*		       ****************  */
		/*					 */
		/*		       ****************  */
		/*		       ****************  */
		/*  line width 4:  y-> ****************  */
		/*		       ****************  */
		/*					 */
		/*		       ****************  */
		/*		       ****************  */
		/*  line width 5:  y-> ****************  */
		/*		       ****************  */
		/*		       ****************  */
		/* etc......				 */
		/*					 */
    pDAS->underLine = pDAS->underLine + (*ret_underThick) / 2;

}

/*********************************************************************
 *		Public Functions
 *********************************************************************/
/*********************************************************************
 * Function: CreateDA
 *
 *    CreateDA creates a Text Graphic area with the appropriate scroll bars.
 *
 *********************************************************************/
static XtPointer
HelpCreateDA(
    Widget	 parent,
    char	*name,
    short	 vert_flag,
    short	 horz_flag,
    Boolean	 traversal_flag,
    _DtCvValue   honor_size,
    _DtCvRenderType render_type,
    int          rows,
    int          columns,
    unsigned short media_resolution,
    void	(*hyperTextCB)(),
    void	(*resizeCB)(),
    int		(*exec_ok_routine)(),
    XtPointer	 client_data,
    XmFontList	 default_list )
{
    DtHelpDispAreaStruct *pDAS;
    DAArgs DA_args;
    Arg args[20];
    int n;
    int maxFontAscent;
    int maxFontDescent;
    int maxFontCharWidth;
  
    short margin_width;
    short margin_height;
    short shadowThick, highThick;
    Dimension hBarHeight = 0;
    Dimension vBarWidth  = 0;
    Dimension width;
    Boolean   value;

    Widget form;
    Display *dpy = XtDisplay(parent);
    Screen      *retScr = XtScreen(parent);
    int          screen = XScreenNumberOfScreen(retScr);

    Colormap colormap;

    Pixmap		 tile;
    XGCValues		 gcValues;
    unsigned long	 gcMask;

    unsigned long char_width;

    Atom xa_ave_width;

    XFontStruct *tmpFont = NULL;
    XtPointer	 default_font = NULL;
    XRectangle   rectangle[1];

    XmFontContext	fontContext;
    XmFontListEntry	fontEntry;
    XmFontType		fontType;

    /* Allocate the Display Area. */
    pDAS = (DtHelpDispAreaStruct *) XtMalloc(sizeof(DtHelpDispAreaStruct));

   /*
     * get the resources
     */
    XtGetSubresources(parent, &DA_args, name, "XmDrawnButton",
		    Resources, XtNumber(Resources), NULL, 0);

    if (rows <= 0)
	rows = 1;
    if (columns <= 0)
	columns = 1;

    /*
     * initialize the structure variables.
     */
    pDAS->text_selected = False;
    pDAS->primary       = False;
    pDAS->select_state  = _DtHelpNothingDoing;
    pDAS->toc_y         = 0;
    pDAS->toc_base      = 0;
    pDAS->toc_flag      = 0;
    if (traversal_flag)
        pDAS->toc_flag  = _DT_HELP_SHADOW_TRAVERSAL | _DT_HELP_NOT_INITIALIZED;

    pDAS->max_spc       = 0;
    pDAS->cur_spc       = 0;
    pDAS->maxX          = 0;
    pDAS->virtualX      = 0;
    pDAS->firstVisible  = 0;
    pDAS->visibleCount  = rows;
    pDAS->maxYpos       = 0;
    pDAS->neededFlags   = 0;
    pDAS->vert_init_scr = DA_args.initialDelay;
    pDAS->vert_rep_scr  = DA_args.repeatDelay;
    pDAS->horz_init_scr = DA_args.initialDelay;
    pDAS->horz_rep_scr  = DA_args.repeatDelay;
    pDAS->moveThreshold = DA_args.moveThreshold;
    pDAS->marginWidth   = DA_args.marginWidth;
    pDAS->marginHeight  = DA_args.marginHeight;
    pDAS->searchColor   = DA_args.search_color;
    pDAS->depth		= 0;
    pDAS->spc_chars     = NULL;
    pDAS->scr_timer_id  = 0;
    pDAS->def_pix       = 0;
    pDAS->context       = NULL;
    pDAS->vertIsMapped  = False;
    pDAS->horzIsMapped  = False;
    pDAS->lst_topic     = NULL;
    pDAS->nextNonVisible = 0;
    pDAS->media_resolution = media_resolution;
    pDAS->honor_size = honor_size;
    pDAS->render_type = render_type;
    pDAS->dtinfo = 0;
    pDAS->stipple = None;

    /*
     * locale dependant information
     */
    pDAS->nl_to_space      = 1;
    pDAS->cant_begin_chars = NULL;
    pDAS->cant_end_chars   = NULL;
    if (1 < MB_CUR_MAX)
	_DtHelpLoadMultiInfo (&(pDAS->cant_begin_chars),
				&(pDAS->cant_end_chars), &(pDAS->nl_to_space));

    /*
     * initialize the hypertext callback pointer
     */
    pDAS->exec_filter = exec_ok_routine;
    pDAS->hyperCall   = hyperTextCB;
    pDAS->resizeCall  = resizeCB;
    pDAS->clientData  = client_data;

    /*
     * zero out other callback fields
     */
    pDAS->vScrollNotify = NULL ;
    pDAS->armCallback = NULL ;

    /*
     * create the atoms needed
     */
    xa_ave_width = XmInternAtom(dpy, "AVERAGE_WIDTH"     , False);

    /*
     * Malloc for the default font.
     */
    (void) XmeRenderTableGetDefaultFont(DA_args.userFont, &tmpFont);
    if (default_list != NULL &&
		XmFontListInitFontContext (&fontContext, default_list))
      {
	fontEntry = XmFontListNextEntry (fontContext);
	if (fontEntry != NULL)
	    default_font = XmFontListEntryGetFont (fontEntry, &fontType);

	XmFontListFreeFontContext (fontContext);
      }

    /*
     * fake out the next call by using the parent as the display widget
     */
    pDAS->dispWid = parent;
    __DtHelpFontDatabaseInit (pDAS, default_font, fontType, tmpFont);

    /*
     * Get the base font meterics.
     */
    __DtHelpFontMetrics (pDAS->font_info, __DtHelpDefaultFontIndexGet(pDAS),
			&maxFontAscent, &maxFontDescent, &maxFontCharWidth,
			NULL, NULL);

    pDAS->leading    = DA_args.leading;
    pDAS->fontAscent = maxFontAscent;
    pDAS->lineHeight = maxFontAscent + maxFontDescent + pDAS->leading + 1;

    n = __DtHelpDefaultFontIndexGet(pDAS);
    if (n < 0)
        value = get_fontsetproperty(__DtHelpFontSetGet(pDAS->font_info, n),
			xa_ave_width, ((unsigned long *) &(pDAS->charWidth)));
    else
	value = XGetFontProperty(__DtHelpFontStructGet(pDAS->font_info, n),
			xa_ave_width, ((unsigned long *) &(pDAS->charWidth)));

    if (False == value || 0 == pDAS->charWidth)
      {
	int len = maxFontCharWidth;

	if (n < 0)
	    len += XmbTextEscapement(
				__DtHelpFontSetGet(pDAS->font_info,n),"1",1);
	else
	    len += XTextWidth(__DtHelpFontStructGet(pDAS->font_info, n),"1",1);

        pDAS->charWidth = 10 * len / 2;
      }

    /*
     * Create the form to manage the window and scroll bars.
     */
    n = 0;
    XtSetArg(args[n], XmNresizePolicy      , XmRESIZE_ANY);		++n;
    XtSetArg(args[n], XmNshadowType	, XmSHADOW_OUT);		++n;
    form = XmCreateForm(parent, "DisplayAreaForm", args, n);
    XtManageChild(form);

    /*
     * force the shadowThickness to zero. The XmManager will try to set
     * this to one.
     */
    n = 0;
    XtSetArg(args[n], XmNshadowThickness   , 0);			++n;
    XtSetArg(args[n], XmNhighlightThickness, 0);			++n;
    XtSetValues (form, args, n);

    /*
     * get the colors and margin widths and heights
     */
    n = 0;
    XtSetArg (args[n], XmNmarginWidth , &margin_width);		++n;
    XtSetArg (args[n], XmNmarginHeight, &margin_height);	++n;
    XtSetArg (args[n], XmNcolormap    , &colormap);		++n;
    XtGetValues(form, args, n);

    /* Create the vertical scrollbar. */
    pDAS->vertScrollWid = NULL;
    if (vert_flag != _DtHelpNONE)
      {
	if (vert_flag == _DtHelpSTATIC)
	    pDAS->vertIsMapped = True;

	n = 0;
	XtSetArg(args[n], XmNtopAttachment     , XmATTACH_FORM);	++n;
	XtSetArg(args[n], XmNtopOffset         , 0);			++n;
	XtSetArg(args[n], XmNbottomAttachment  , XmATTACH_FORM);	++n;
	XtSetArg(args[n], XmNbottomOffset      , margin_height);	++n;
	XtSetArg(args[n], XmNrightAttachment   , XmATTACH_FORM);	++n;
	XtSetArg(args[n], XmNrightOffset       , 0);			++n;
	XtSetArg(args[n], XmNorientation       , XmVERTICAL);		++n;
	XtSetArg(args[n], XmNtraversalOn       , True);			++n;
	XtSetArg(args[n], XmNhighlightThickness, 1);			++n;
	XtSetArg(args[n], XmNshadowType        , XmSHADOW_IN);		++n;
	XtSetArg(args[n], XmNvalue             , 0);			++n;
	XtSetArg(args[n], XmNminimum           , 0);			++n;
	/* fake out the scrollbar manager, who will init dims to 100 */
	XtSetArg(args[n], XmNheight            , 1);			++n;
	XtSetArg(args[n], XmNmaximum           , 1);			++n;
	XtSetArg(args[n], XmNincrement         , 1);			++n;
	XtSetArg(args[n], XmNpageIncrement     , 1);			++n;
	XtSetArg(args[n], XmNsliderSize        , 1);			++n;
	XtSetArg(args[n], XtNmappedWhenManaged , pDAS->vertIsMapped);	++n;
	pDAS->vertScrollWid = XmCreateScrollBar(form,
					"DisplayDtHelpVertScrollBar", args, n);

	XtManageChild(pDAS->vertScrollWid);
	if (vert_flag != _DtHelpSTATIC)
	    pDAS->neededFlags = _DtHelpSET_AS_NEEDED (pDAS->neededFlags,
						_DtHelpVERTICAL_SCROLLBAR);
  
	XtAddCallback(pDAS->vertScrollWid, XmNdragCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNincrementCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNdecrementCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNpageIncrementCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNpageDecrementCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNtoBottomCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNtoTopCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->vertScrollWid, XmNvalueChangedCallback,
					_DtHelpVertScrollCB, (XtPointer) pDAS);

	XtAddEventHandler (pDAS->vertScrollWid, ButtonPressMask, True,
			(XtEventHandler)_DtHelpMoveBtnFocusCB,(XtPointer) pDAS);

      }

    /* Create the horizontal scrollbar. */
    pDAS->horzScrollWid = NULL;
    if (horz_flag != _DtHelpNONE)
      {
	if (horz_flag == _DtHelpSTATIC)
	    pDAS->horzIsMapped = True;

	n = 0;
	XtSetArg(args[n], XmNbottomAttachment  , XmATTACH_FORM);	++n;
	XtSetArg(args[n], XmNbottomOffset      , 0);			++n;
	XtSetArg(args[n], XmNrightAttachment   , XmATTACH_FORM);	++n;
	XtSetArg(args[n], XmNrightOffset       , margin_width);		++n;
	XtSetArg(args[n], XmNleftAttachment    , XmATTACH_FORM);	++n;
	XtSetArg(args[n], XmNleftOffset        , 0);			++n;
	XtSetArg(args[n], XmNorientation       , XmHORIZONTAL);		++n;
	XtSetArg(args[n], XmNtraversalOn       , True);			++n;
	XtSetArg(args[n], XmNhighlightThickness, 1);			++n;
	XtSetArg(args[n], XmNshadowType        , XmSHADOW_IN);		++n;
	XtSetArg(args[n], XmNvalue             , 0);			++n;
	XtSetArg(args[n], XmNminimum           , 0);			++n;
	XtSetArg(args[n], XmNmaximum           , (pDAS->charWidth/10));	++n;
	/* fake out the scrollbar manager, who will init dims to 100 */
	XtSetArg(args[n], XmNwidth             , 1);			++n;
	XtSetArg(args[n], XmNincrement         , (pDAS->charWidth/10));	++n;
	XtSetArg(args[n], XmNpageIncrement     , (pDAS->charWidth/10));	++n;
	XtSetArg(args[n], XmNsliderSize        , (pDAS->charWidth/10));	++n;
	XtSetArg(args[n], XtNmappedWhenManaged , pDAS->horzIsMapped);	++n;
	pDAS->horzScrollWid = XmCreateScrollBar(form,
					"DisplayHorzScrollBar", args, n);
	XtManageChild(pDAS->horzScrollWid);
	if (horz_flag != _DtHelpSTATIC)
	    pDAS->neededFlags = _DtHelpSET_AS_NEEDED (pDAS->neededFlags,
						_DtHelpHORIZONTAL_SCROLLBAR);
  
	XtAddCallback(pDAS->horzScrollWid, XmNdragCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNincrementCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNdecrementCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNpageIncrementCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNpageDecrementCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNtoBottomCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNtoTopCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
	XtAddCallback(pDAS->horzScrollWid, XmNvalueChangedCallback,
					_DtHelpHorzScrollCB, (XtPointer) pDAS);
      }

    /*
     * check for the horizontal attachments
     */
    n = 0;
    if (horz_flag == _DtHelpSTATIC)
      {
	XtSetArg(args[n], XmNbottomAttachment  , XmATTACH_WIDGET);	++n;
	XtSetArg(args[n], XmNbottomWidget      , pDAS->horzScrollWid);	++n;
      }
    else
      {
	XtSetArg(args[n], XmNbottomAttachment  , XmATTACH_FORM);	++n;
      }

    /*
     * check for the vertical attachments
     */
    if (vert_flag == _DtHelpSTATIC)
      {
	XtSetArg(args[n], XmNrightAttachment   , XmATTACH_WIDGET);	++n;
	XtSetArg(args[n], XmNrightWidget       , pDAS->vertScrollWid);	++n;
      }
    else
      {
	XtSetArg(args[n], XmNrightAttachment  , XmATTACH_FORM);		++n;
      }

    /*
     * do the rest of the arguments
     */
    XtSetArg(args[n], XmNbottomOffset      , margin_height);		++n;
    XtSetArg(args[n], XmNrightOffset       , margin_width);		++n;
    XtSetArg(args[n], XmNtopAttachment     , XmATTACH_FORM);		++n;
    XtSetArg(args[n], XmNtopOffset	   , 0);			++n;
    XtSetArg(args[n], XmNleftAttachment    , XmATTACH_FORM);		++n;
    XtSetArg(args[n], XmNleftOffset	   , 0);			++n;
    XtSetArg(args[n], XmNrecomputeSize     , False);			++n;
    XtSetArg(args[n], XmNpushButtonEnabled , False);			++n;
    XtSetArg(args[n], XmNtraversalOn       , True);			++n;
    XtSetArg(args[n], XmNhighlightThickness, 1);			++n;
    XtSetArg(args[n], XmNshadowType        , XmSHADOW_IN);		++n;
    XtSetArg(args[n], XmNmultiClick	   , XmMULTICLICK_DISCARD);	++n;
    XtSetArg(args[n], XmNuserData  	   , pDAS);			++n;
    pDAS->dispWid = XmCreateDrawnButton(form, name, args, n);
    XtManageChild(pDAS->dispWid);
  
    XtAddCallback(pDAS->dispWid, XmNexposeCallback, _DtHelpExposeCB,
						(XtPointer) pDAS);
    XtAddCallback(pDAS->dispWid, XmNresizeCallback, _DtHelpResizeCB,
						(XtPointer) pDAS);
    XtAddCallback(pDAS->dispWid, XmNarmCallback, _DtHelpClickOrSelectCB,
						(XtPointer) pDAS);
    XtAddCallback(pDAS->dispWid, XmNdisarmCallback, _DtHelpEndSelectionCB,
						(XtPointer) pDAS);

    XtAddEventHandler (pDAS->dispWid, Button1MotionMask, True,
			(XtEventHandler)_DtHelpMouseMoveCB, (XtPointer) pDAS);

    /*
     * add my actions
     * parse the translations.
     */
    _DtHelpProcessLock();
    if (DrawnBTrans == NULL)
        DrawnBTrans = XtParseTranslationTable(DrawnBTransTable);

     /*
     * override the translations
    XtSetArg(args[n], XmNtranslations	   , DrawnBTrans);		++n;
     */
    if (DrawnBTrans != NULL)
	XtOverrideTranslations(pDAS->dispWid, DrawnBTrans);
    _DtHelpProcessUnlock();
  
    if (_XmGetFocusPolicy(parent) == XmPOINTER)
      {
	XtAddEventHandler (pDAS->dispWid, EnterWindowMask, True,
			(XtEventHandler)_DtHelpEnterLeaveCB, (XtPointer) pDAS);
	XtAddEventHandler (pDAS->dispWid, LeaveWindowMask, True,
			(XtEventHandler)_DtHelpEnterLeaveCB, (XtPointer) pDAS);
      }
    else
	XtAddEventHandler (pDAS->dispWid, FocusChangeMask, True,
			(XtEventHandler)_DtHelpFocusCB, (XtPointer) pDAS);

    XtAddEventHandler (pDAS->dispWid, VisibilityChangeMask, True,
			(XtEventHandler)_DtHelpVisibilityCB, (XtPointer) pDAS);
  

    /* Add a destroy callback so that the display area can clean up prior to
     * the help widget getting destroyed (e.g. display area's parent) 
     */
    XtAddCallback(pDAS->dispWid, XmNdestroyCallback,
				_DtHelpDisplayAreaDestroyCB, (XtPointer) pDAS);
  
    n = 0;
    XtSetArg(args[n], XmNshadowThickness   , &shadowThick);	++n;
    XtSetArg(args[n], XmNhighlightThickness, &highThick);	++n;
    XtSetArg(args[n], XmNforeground    , &(pDAS->foregroundColor)); ++n;
    XtSetArg(args[n], XmNbackground    , &(pDAS->backgroundColor)); ++n;
    XtSetArg(args[n], XmNhighlightColor, &(pDAS->traversalColor) ); ++n;
    XtSetArg(args[n], XmNdepth         , &(pDAS->depth)          ); ++n;
    XtGetValues(pDAS->dispWid, args, n);
    pDAS->decorThickness = shadowThick + highThick;

    /*
     * Get the underline information
     */
    GetUnderLineInfo (dpy, pDAS, &(pDAS->lineThickness));

    /*
     * get the tiling pattern.
     */
    tile = XmGetPixmap (XtScreen(pDAS->dispWid), "50_foreground",
				pDAS->foregroundColor, pDAS->backgroundColor);

    /*
     * Get the data for the graphics contexts and create the GC's.
     */
    gcMask = (GCFunction   | GCPlaneMask   | GCForeground  |
		GCBackground  | GCLineWidth   | GCLineStyle   |
		GCClipXOrigin | GCClipYOrigin | GCClipMask);

    gcValues.function      = GXcopy;
    gcValues.plane_mask    = AllPlanes;
    gcValues.foreground    = pDAS->foregroundColor;
    gcValues.background    = pDAS->backgroundColor;
    gcValues.line_style    = LineSolid;
    gcValues.line_width    = pDAS->lineThickness;
    gcValues.clip_x_origin = 0;
    gcValues.clip_y_origin = 0;
    gcValues.clip_mask     = None;
    if (tile)
      {
	gcMask |= GCTile;
	gcValues.tile = tile;
      }
 
    pDAS->normalGC = XCreateGC(dpy,
		RootWindowOfScreen(XtScreen(pDAS->dispWid)), gcMask, &gcValues);

    gcMask &= (~GCTile);
    pDAS->pixmapGC = XCreateGC(dpy,
		RootWindowOfScreen(XtScreen(pDAS->dispWid)), gcMask, &gcValues);
  
    gcValues.foreground = pDAS->backgroundColor;
    gcValues.background = pDAS->foregroundColor;
    pDAS->invertGC = XCreateGC(dpy,
		RootWindowOfScreen(XtScreen(pDAS->dispWid)), gcMask, &gcValues);
  
    /*
     * Set the size of the text view area to the requested
     * number of columns and lines.
     */
    char_width          = pDAS->charWidth * columns;

    /*
     * Make sure the margins include enough room for a traversal line
     */
    if (((int) pDAS->marginWidth) < pDAS->lineThickness)
	pDAS->marginWidth = pDAS->lineThickness;
    if (((int) pDAS->marginHeight) < pDAS->lineThickness)
	pDAS->marginHeight = pDAS->lineThickness;
    if (pDAS->leading < pDAS->lineThickness)
	pDAS->leading = pDAS->lineThickness;

    /*
     * get the scrollbar widths.
     */
    if (NULL != pDAS->horzScrollWid)
      {
	n = 0;
	XtSetArg(args[n], XmNheight      , &hBarHeight); n++;
	XtSetArg(args[n], XmNinitialDelay, &(pDAS->horz_init_scr)); n++;
	XtSetArg(args[n], XmNrepeatDelay , &(pDAS->horz_rep_scr));  n++;
	XtGetValues(pDAS->horzScrollWid, args, n);
      }

    if (NULL != pDAS->vertScrollWid)
      {
	n = 0;
	XtSetArg(args[n], XmNwidth       , &vBarWidth);             n++;
	XtSetArg(args[n], XmNinitialDelay, &(pDAS->vert_init_scr)); n++;
	XtSetArg(args[n], XmNrepeatDelay , &(pDAS->vert_rep_scr));  n++;
	XtGetValues(pDAS->vertScrollWid, args, n);
      }

    /*
     * calculate the display area height/width
     */
    pDAS->dispUseWidth  = ((int) (char_width / 10 + (char_width % 10 ? 1 : 0)))
					      + 2 * pDAS->marginWidth;
    if (vert_flag != _DtHelpSTATIC && pDAS->dispUseWidth < vBarWidth)
	pDAS->dispUseWidth = vBarWidth;
    pDAS->dispWidth     = pDAS->dispUseWidth  + 2 * pDAS->decorThickness;

    pDAS->dispUseHeight = pDAS->lineHeight * rows;
    if (horz_flag != _DtHelpSTATIC && pDAS->dispUseHeight < hBarHeight)
	pDAS->dispUseHeight = hBarHeight;
    pDAS->dispHeight    = pDAS->dispUseHeight + 2 * pDAS->decorThickness;

    /*
     * Truncate the width and height to the size of the display.
     * This will prevent an X protocal error when it is asked for
     * a too large size. Besides, any decent window manager will
     * force the overall size to the height and width of the display.
     * This simply refines the size down to a closer (but not perfect)
     * fit.
     */
    if (((int) pDAS->dispWidth) > XDisplayWidth (dpy, screen)) {
	pDAS->dispWidth = XDisplayWidth (dpy, screen);
    }
    if (((int) pDAS->dispHeight) > XDisplayHeight (dpy, screen)) {
	pDAS->dispHeight = XDisplayHeight (dpy, screen);
    }

    n = 0;
    pDAS->formWidth  = 0;
    pDAS->formHeight = 0;
    XtSetArg(args[n], XmNwidth, pDAS->dispWidth);		++n;
    XtSetArg(args[n], XmNheight, pDAS->dispHeight);		++n;
    XtSetValues(pDAS->dispWid, args, n);

    /*
     * set the scroll bar values
     */
    if (pDAS->vertScrollWid != NULL)
      {
        n = 0;
        XtSetArg(args[n], XmNmaximum           , pDAS->dispUseHeight);	++n;
        XtSetArg(args[n], XmNincrement         , pDAS->lineHeight);	++n;
        XtSetArg(args[n], XmNpageIncrement     , pDAS->lineHeight);	++n;
        XtSetArg(args[n], XmNsliderSize        , pDAS->dispUseHeight);	++n;
        XtSetValues(pDAS->vertScrollWid, args, n);
      }
  
    if (pDAS->resizeCall)
	(*(pDAS->resizeCall)) (pDAS->clientData);

    /*
     * calculate the offset for the right edge of the
     * horizontal scrollbar.
     */
    if (vert_flag == _DtHelpSTATIC && pDAS->horzScrollWid)
      {
	width = vBarWidth + margin_width;

	XtSetArg(args[0], XmNrightOffset , width);
	XtSetValues(pDAS->horzScrollWid, args, 1);
      }

    /*
     * calculate the offset for the bottom end of the
     * vertical scrollbar.
     */
    if (horz_flag == _DtHelpSTATIC && pDAS->vertScrollWid)
      {
	width = hBarHeight + margin_height;

	XtSetArg(args[0], XmNbottomOffset , width);
	XtSetValues(pDAS->vertScrollWid, args, 1);
      }

    rectangle[0].x      = pDAS->decorThickness;
    rectangle[0].y      = pDAS->decorThickness;
    rectangle[0].width  = pDAS->dispUseWidth;
    rectangle[0].height = pDAS->dispUseHeight;
    XSetClipRectangles(XtDisplay(pDAS->dispWid), pDAS->normalGC, 0, 0,
						rectangle, 1, Unsorted);
    XSetClipRectangles(XtDisplay(pDAS->dispWid), pDAS->invertGC, 0, 0,
						rectangle, 1, Unsorted);

    /*
     * get the colormap and the visual
     */
    if (!XtIsShell(parent) && XtParent(parent) != NULL)
	parent = XtParent(parent);

    pDAS->visual = NULL;

    n = 0;
    XtSetArg (args[n], XmNcolormap, &(pDAS->colormap)); n++;
    XtSetArg (args[n], XmNvisual  , &(pDAS->visual  )); n++;
    XtGetValues (parent, args, n);

    if (pDAS->visual == NULL)
	pDAS->visual = XDefaultVisualOfScreen(XtScreen(pDAS->dispWid));

    /*
     * set up the canvas
     */
    _DtHelpProcessLock();
    DefVirtFunctions.exec_cmd_filter = exec_ok_routine;

    pDAS->canvas = _DtCanvasCreate (DefVirtFunctions, (_DtCvPointer) pDAS);
    _DtHelpProcessUnlock();

    return (XtPointer) pDAS;

}  /* End _DtHelpCreateDA */


/*********************************************************************
 * Function: CreateDisplayArea
 *
 *    Creates a Text Graphic area with the appropriate scroll bars.
 *    specifying the size with rows and columns
 *
 *********************************************************************/
XtPointer
_DtHelpCreateDisplayArea(
    Widget	 parent,
    char	*name,
    short	 vert_flag,
    short	 horz_flag,
    Boolean	 traversal_flag,
    int		 rows,
    int		 columns,
    void	(*hyperTextCB)(),
    void	(*resizeCB)(),
    int		(*exec_ok_routine)(),
    XtPointer	 client_data,
    XmFontList	 default_list )
{
    return HelpCreateDA(parent, name,
			vert_flag, horz_flag,
			traversal_flag,
			_DtCvIGNORE_BOUNDARY, _DtCvRENDER_PARTIAL,
			rows, columns, 100,
			hyperTextCB, resizeCB, exec_ok_routine,
			client_data, default_list);
}

/*********************************************************************
 * Function: CreateOutputArea
 *
 *    Creates a Text Graphic area with the appropriate scroll bars.
 *    specifying the size with width and height
 *
 *********************************************************************/
XtPointer
_DtHelpCreateOutputArea(
    Widget	 parent,
    char	*name,
    short	 vert_flag,
    short	 horz_flag,
    Boolean	 traversal_flag,
    _DtCvValue   honor_size,
    _DtCvRenderType render_type,
    Dimension	 width,
    Dimension	 height,
    unsigned short media_resolution,
    void	(*hyperTextCB)(),
    void	(*resizeCB)(),
    int		(*exec_ok_routine)(),
    XtPointer	 client_data,
    XmFontList	 default_list )
{
    DtHelpDispAreaStruct *pDAS;

    pDAS = HelpCreateDA(parent, name,
			vert_flag, horz_flag,
			traversal_flag, honor_size, render_type,
			1, 1, media_resolution,
			hyperTextCB, resizeCB, exec_ok_routine,
			client_data, default_list);
    /* if area created successfully, then resize it to the given size */
    if (pDAS)
	_DtHelpSetScrollBars(pDAS, width, height);

    return pDAS;
}
