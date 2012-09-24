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
/* $TOG: DmxPrintOutput.C /main/6 1997/07/07 13:57:21 mgreess $ */

/*
 *+SNOTICE
 *
 *	$:$
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */


#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Print.h>
#include <Xm/Text.h>
#include <Dt/Editor.h>

#include "Dmx.h"
#include "DmxPrintOutput.h"
#include "RoamMenuWindow.h"

//
// Used to initialize the top, right, bottom, and left margins
// between the outside edge of the page (_form) and the inner
// page that is written on.
//
const char *const DmxPrintOutput::_default_margin = ".5in";

DmxPrintOutput::DmxPrintOutput (
				Widget pshell
			 	) : UIComponent( "PrintOutput" )
{
    Widget		w;
    DtMailBoolean	parse_error;

    _w = NULL;
    _pshell = pshell;

    w = XtVaCreateWidget("Page",
			xmFormWidgetClass,
			_pshell,
			XmNresizePolicy, XmRESIZE_NONE,
			NULL);
    _form = w;
    _w = _form;
		 
    w = XtVaCreateWidget("InnerPage",
			xmFormWidgetClass,
			_form,
			XmNresizePolicy, XmRESIZE_NONE,
			NULL);
    _inner_form = w;
		 
    w = XtVaCreateManagedWidget("HeaderLeft",
				  xmLabelWidgetClass,
				  _inner_form,
				  XmNalignment, XmALIGNMENT_BEGINNING,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL);
    _header_left = w;

    w = XtVaCreateManagedWidget("HeaderRight",
				  xmLabelWidgetClass,
				  _inner_form,
				  XmNalignment, XmALIGNMENT_END,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, _header_left,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL);
    _header_right = w;
		 
    w = XtVaCreateManagedWidget("Editor",
#ifdef USE_DTEDITOR
				  dtEditorWidgetClass,
#else
				  xmTextWidgetClass,
#endif
				  _inner_form,
				  DtNscrollVertical, FALSE,
				  DtNscrollHorizontal, FALSE,
				  DtNshowStatusLine, FALSE,
				  DtNwordWrap, TRUE,
                                  XmNeditMode, XmMULTI_LINE_EDIT,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, _header_left,
				  NULL);
    _editor = w;
		 
    w = XtVaCreateManagedWidget("FooterLeft",
				  xmLabelWidgetClass,
				  _inner_form,
				  XmNalignment, XmALIGNMENT_BEGINNING,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNbottomAttachment, XmATTACH_FORM,
				  NULL);
    _footer_left = w;

    w = XtVaCreateManagedWidget("FooterRight",
				  xmLabelWidgetClass,
				  _inner_form,
				  XmNalignment, XmALIGNMENT_END,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, _footer_left,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNbottomAttachment, XmATTACH_FORM,
				  NULL);
    _footer_right = w;

    XtVaSetValues(
		_editor,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, _footer_left,
		NULL);

    installDestroyHandler();
    XtManageChild(_inner_form);
    XtManageChild(_form);

    setPageMargins(
		_default_margin,
		_default_margin,
		_default_margin,
		_default_margin,
		&parse_error );
    assert(!parse_error);
}

DmxPrintOutput::~DmxPrintOutput (void)
{
    //
    //  Don't destroy anything here.
    //  The BasicComponent class takes care of destroying
    //  the _form widget.
    //
}

void DmxPrintOutput::hideFooters (void)
{
    XtUnmanageChild(_footer_left);
    XtUnmanageChild(_footer_right);
    XtVaSetValues(_editor, XmNbottomAttachment, XmATTACH_FORM, NULL);
}

void DmxPrintOutput::showFooters (void)
{
    XtManageChild(_footer_left);
    XtManageChild(_footer_right);
    XtVaSetValues(
		_editor,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, _footer_left,
		NULL);
}

void DmxPrintOutput::hideHeaders (void)
{
    XtUnmanageChild(_header_left);
    XtUnmanageChild(_header_right);
    XtVaSetValues(_editor, XmNtopAttachment, XmATTACH_FORM, NULL);
}

void DmxPrintOutput::showHeaders (void)
{
    XtManageChild(_header_left);
    XtManageChild(_header_right);
    XtVaSetValues(
		_editor,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, _header_left,
		NULL);
}


void
DmxPrintOutput::setHdrFtrString (_DtPrintHdrFtrEnum which, char *label)
{
    Widget	w;
    XmString	xms;

    if (label == (char *) NULL)
      return;

    switch (which)
    {
	case DTPRINT_OPTION_HEADER_LEFT:
	  w = _header_left;
	  break;
	case DTPRINT_OPTION_HEADER_RIGHT:
	  w = _header_right;
	  break;
	case DTPRINT_OPTION_FOOTER_LEFT:
	  w = _footer_left;
	  break;
	case DTPRINT_OPTION_FOOTER_RIGHT:
	  w = _footer_right;
	  break;
	default:
	  // TBD:  Need to log an error.
	  return;
    }

    xms = XmStringCreateLocalized(label);
    XtVaSetValues(w, XmNlabelString, xms, NULL);
    XmStringFree(xms);
}

void
DmxPrintOutput::setHdrFtrStrings (
				char *hdr_left,
				char *hdr_right,
				char *ftr_left,
				char *ftr_right
				   )
{
    XmString	xms;

    if (hdr_left)
    {
        xms = XmStringCreateLocalized(hdr_left);
        XtVaSetValues(_header_left, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }

    if (hdr_right)
    {
        xms = XmStringCreateLocalized(hdr_right);
        XtVaSetValues(_header_right, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }

    if (ftr_left)
    {
        xms = XmStringCreateLocalized(ftr_left);
        XtVaSetValues(_footer_left, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }

    if (ftr_right)
    {
        xms = XmStringCreateLocalized(ftr_right);
        XtVaSetValues(_footer_right, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }
}

void
DmxPrintOutput::setPageMargin (
				_DtPrintMarginEnum which,
				const char *margin,
				DtMailBoolean *parse_error
				)
{
    int		pixels = 0;
    XtEnum	xt_parse_error = FALSE;
    char	*marginstring = strdup(margin);

    if (parse_error)
      *parse_error = DTM_FALSE;

    switch (which)
    {
	case DTPRINT_OPTION_MARGIN_TOP:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(_pshell),
					marginstring,
					XmVERTICAL,
					XmPIXELS,
					&xt_parse_error);
	  if (!xt_parse_error && pixels > 0)
	    _margin_top = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_RIGHT:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(_pshell),
					marginstring,
					XmHORIZONTAL,
					XmPIXELS,
					&xt_parse_error);
	  if (!xt_parse_error && pixels > 0)
	    _margin_right = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_BOTTOM:
          _margin_bottom = pixels;
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(_pshell),
					marginstring,
					XmVERTICAL,
					XmPIXELS,
					&xt_parse_error);
	  if (!xt_parse_error && pixels > 0)
	    _margin_bottom = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_LEFT:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(_pshell),
					marginstring,
					XmHORIZONTAL,
					XmPIXELS,
					&xt_parse_error);
	  if (!xt_parse_error && pixels > 0)
	    _margin_left = pixels;
	  break;
	default:
	  // TBD:  Need to log an error.
	  return;
    }

    if (!xt_parse_error)
      setInnerPageDimensions(
			_margin_top,
			_margin_right,
			_margin_bottom,
			_margin_left);
    else
      *parse_error = DTM_TRUE;

    free(marginstring);
}

void
DmxPrintOutput::setPageMargin (_DtPrintMarginEnum which, int margin)
{
    Dimension	pixels = (Dimension) margin;

    switch (which)
    {
	case DTPRINT_OPTION_MARGIN_TOP:
          _margin_top = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_RIGHT:
          _margin_right = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_BOTTOM:
          _margin_bottom = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_LEFT:
          _margin_left = pixels;
	  break;
	default:
	  // TBD:  Need to log an error.
	  return;
    }

    setInnerPageDimensions(
			_margin_top,
			_margin_right,
			_margin_bottom,
			_margin_left);
}

void
DmxPrintOutput::setPageMargins (
				const char *top,
				const char *right,
				const char *bottom,
				const char *left,
				DtMailBoolean *parse_error
				 )
{
    int		pixels = 0;
    XtEnum	xt_parse_error = FALSE;
    char	*margin;

    *parse_error = DTM_FALSE;

    margin = strdup(top);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(_pshell),
				margin,
				XmVERTICAL,
				XmPIXELS,
				&xt_parse_error);
    free(margin);
    if (!xt_parse_error && pixels > 0)
    {
        _margin_top = pixels;
    }
    else
    {    
	*parse_error = DTM_TRUE;
        return;
    }

    margin = strdup(right);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(_pshell),
				margin,
				XmHORIZONTAL,
				XmPIXELS,
				&xt_parse_error);
    free(margin);
    if (!xt_parse_error && pixels > 0)
    {
        _margin_right = pixels;
    }
    else
    {    
	*parse_error = DTM_TRUE;
        return;
    }

    margin = strdup(bottom);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(_pshell),
				margin,
				XmVERTICAL,
				XmPIXELS,
				&xt_parse_error);
    free(margin);
    if (!xt_parse_error && pixels > 0)
    {
        _margin_bottom = pixels;
    }
    else
    {    
	*parse_error = DTM_TRUE;
        return;
    }

    margin = strdup(left);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(_pshell),
				margin,
				XmHORIZONTAL,
				XmPIXELS,
				&xt_parse_error);
    free(margin);
    if (!xt_parse_error && pixels > 0)
    {
        _margin_left = pixels;
    }
    else
    {    
	*parse_error = DTM_TRUE;
        return;
    }

    setInnerPageDimensions(
			_margin_top,
			_margin_right,
			_margin_bottom,
			_margin_left);
}

void
DmxPrintOutput::setPageMargins (
				int top,
				int right,
				int bottom,
				int left
				 )
{
    _margin_top = (top > 0) ? (Dimension) top : _margin_top;
    _margin_right = (right > 0) ? (Dimension) right : _margin_right;
    _margin_bottom = (bottom > 0) ? (Dimension) bottom : _margin_bottom;
    _margin_left = (left > 0) ? (Dimension) left : _margin_left;

    setInnerPageDimensions(
			_margin_top,
			_margin_right,
			_margin_bottom,
			_margin_left);
}

void
DmxPrintOutput::setWrapToFit (DtMailBoolean onoff)
{
#ifdef USE_DTEDITOR
    XtVaSetValues(_editor, DtNwordWrap, onoff, NULL);
#else
    XtVaSetValues(_editor, XmNwordWrap, onoff, NULL);
#endif
}



int DmxPrintOutput::getCharactersPerLine ()
{
    XtArgVal	columns = 0;

#ifdef USE_DTEDITOR
    XtVaGetValues(_editor, DtNcolumns, &columns, NULL);
#else
    XtVaGetValues(_editor, XmNcolumns, &columns, NULL);
#endif
    return((int) columns);
}



int DmxPrintOutput::getNumLines ()
{
    XtArgVal	total = 0;

#ifdef USE_DTEDITOR
#else
    XtVaGetValues(_editor, XmNtotalLines, &total, NULL);
    //
    // Correct for off by one error.
    //
    total -= 1;
#endif
    return((int)total);
}


int DmxPrintOutput::getLastPosition ()
{
    XmTextPosition last;

#ifdef USE_DTEDITOR
    last = DtEditorGetLastPosition(_editor);
#else
    last = XmTextGetLastPosition(_editor);
#endif
    return((int) last);
}

int DmxPrintOutput::getTopPosition ()
{
    XmTextPosition top;

#ifdef USE_DTEDITOR
    XtVaGetValues(_editor, DtNtopCharacter, &top, NULL);
#else
    top = XmTextGetTopCharacter(_editor);
#endif
    return((int) top);
}

DtMailBoolean DmxPrintOutput::pageUp ()
{
    XmTextPosition top_before, top_after;

#ifdef USE_DTEDITOR
    return DTM_FALSE;
#else
    top_before = XmTextGetTopCharacter(_editor);
    XmTextScroll(_editor, -1 * _lines_per_page);
    top_after = XmTextGetTopCharacter(_editor);

    return (top_before > top_after) ? DTM_TRUE : DTM_FALSE;
#endif
}

DtMailBoolean DmxPrintOutput::pageDown ()
{
    XmTextPosition top_before, top_after;

#ifdef USE_DTEDITOR
    return DTM_FALSE;
#else
    top_before = XmTextGetTopCharacter(_editor);
    XmTextScroll(_editor, _lines_per_page);
    top_after = XmTextGetTopCharacter(_editor);

    return (top_before < top_after) ? DTM_TRUE : DTM_FALSE;
#endif
}

void DmxPrintOutput::setTopPosition ( int pos )
{
    XmTextPosition top = (XmTextPosition) pos;

#ifdef USE_DTEDITOR
    XtVaSetValues(_editor, DtNtopCharacter, top, NULL);
#else
    XmTextSetTopCharacter(_editor, top);
#endif
}

void DmxPrintOutput::appendContents (void* stream, char *contents)
{
    DmxPrintOutput	*thisOutput = (DmxPrintOutput *) stream;

#ifdef USE_DTEDITOR
    DtEditorContentRec	rec;

    rec.type = DtEDITOR_TEXT;
    rec.value.string = contents;
    DtEditorInsert(thisOutput->_editor, &rec);
#else
    XmTextPosition pos;

    pos = XmTextGetLastPosition(thisOutput->_editor);
    XmTextInsert(thisOutput->_editor, pos, contents);
#endif
}

void DmxPrintOutput::appendNewLine ()
{
#ifdef PRINT_TO_VIDEO 
    char	*contents = "=========== NewLine ==========\n";
#else
    char	*contents = "\n";
#endif
    appendContents((XtPointer) this, contents);
}

void DmxPrintOutput::appendPageBreak ()
{
    char *	buf;
    char	*contents = "\n";
    int		nlines, missing;

    nlines = getNumLines();
    missing = ((nlines % _lines_per_page) > 0) ?
		(_lines_per_page - (nlines % _lines_per_page)) :
		0;

    if (! missing)
      return;
    
#ifdef PRINT_TO_VIDEO 
    for (int i=0; i<missing; i++)
    {
	char	buffer[128];

	sprintf(buffer, "Page Break Line:  %d\n", i);
        appendContents((XtPointer) this, buffer);
    }
    fprintf(
	stdout,
	"Total Lines:  %d;  Lines Per Page:  %d;  Missing Lines:  %d\n",
	nlines,
	_lines_per_page,
	missing);
#else
    buf = (char *) malloc((missing * 2) + 1);
    for (int i=0; i<missing; i++)
      strcat(buf, contents);

    appendContents(this, contents);

    free(buf);
#endif
}

void DmxPrintOutput::clearContents (void)
{
#ifdef USE_DTEDITOR
    DtEditorContentRec	content;

    content.type = DtEDITOR_TEXT;
    rec.value.string = NULL;
    status = DtEditorSetContents(my_text, &content);
#else
    XmTextSetString(_editor, "");
#endif
}


/*
 * Private class methods
 */
int
DmxPrintOutput::doGetLinesPerPage ()
{
    XtArgVal /* Dimension */	lpp;

    XtVaGetValues(_editor, XmNrows, &lpp, NULL);
    return ((int) lpp);
}


void
DmxPrintOutput::setInnerPageDimensions (
					Dimension top,
					Dimension right,
					Dimension bottom,
					Dimension left
					   )
{
    XtArgVal height0, width0;
    Dimension	inner_height, inner_width, inner_x, inner_y,
		outer_height, outer_width,
		editor_height, footer_height, header_height;

    XtVaGetValues(_form,
		  XmNheight, &height0,
		  XmNwidth, &width0,
		  NULL);
    outer_height = (Dimension)height0;
    outer_width = (Dimension)width0;

    XtVaGetValues(_header_left,
		  XmNheight, &height0,
		  NULL);
    header_height = (Dimension)height0;

    XtVaGetValues(_footer_left,
		  XmNheight, &height0,
		  NULL);
    footer_height = (Dimension)height0;

    inner_x = left;
    inner_y = top;
    inner_height = ((int) outer_height > (top + bottom)) ?
			(outer_height - (top +  bottom)) :
			outer_height;
    inner_width = ((int) outer_width > (left + right)) ?
			(outer_width - (left + right)) :
			outer_width;
    editor_height = ((int) inner_height > (header_height + footer_height)) ?
			(inner_height - (header_height + footer_height)) :
			inner_height;
    
    XtVaSetValues(_editor, XmNheight, editor_height, NULL);

    XtVaSetValues(_inner_form,
		XmNleftAttachment, XmATTACH_NONE,
		XmNtopAttachment, XmATTACH_NONE,
		XmNx, inner_x,
		XmNy, inner_y,
		XmNheight, inner_height,
		XmNwidth, inner_width,
		NULL);

    _lines_per_page = doGetLinesPerPage();
}
