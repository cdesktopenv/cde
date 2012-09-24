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
/* $TOG: printOutput.c /main/6 1997/07/07 15:06:25 mgreess $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        printOption.c
**
**  Project:     HP DT dtpad, a memo maker type editor based on the
**               Dt Editor widget.
**
**  Description:  Routines which manipulate the print setup dialog
**
**************************************************************************
**********************************<+>*************************************/
/*
 *                   Common Desktop Environment
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
#include <string.h>
#include <assert.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Print.h>
#include <Xm/Text.h>
#include <Dt/Editor.h>

#include "dtpad.h"

#define READ_ACCESS     1
#define WRITE_ACCESS    2

static void	_poSetInnerPageDimensions(
				PrintOutput*,
				Dimension,
				Dimension,
				Dimension,
				Dimension
				);
static int	_poDoGetLinesPerPage(PrintOutput*);
static int	_poGetTopPosition(PrintOutput *pOutput);
static DtEditorErrorCode
		_poGetFileContents(char **contents, char *file);
static DtEditorErrorCode 
		_poStripEmbeddedNulls(char *strbuf, int *len);
extern DtEditorErrorCode
		_poEditorValidateFileAccess(char *file, int accessType);

/*
 * Used to initialize the top, right, bottom, and left margins
 * between the outside edge of the page (_form) and the inner
 * page that is written on.
 */
const char *const _poDefaultMargin = ".5in";

/************************************************************************
 * PrintOutputCreate
 *	Creates and initialiazes a new PrintOutput output including the
 *	widget hierarchy.
 ************************************************************************/
PrintOutput*
PrintOutputCreate(Widget shell)
{
    Widget		w;
    Boolean		parseError;
    PrintOutput		*pOutput;

    pOutput = (PrintOutput *) XtMalloc(sizeof(PrintOutput));
    pOutput->pShell = shell;

    pOutput->currentLine = 0;
    pOutput->linesPerPage = 0;

    w = XtVaCreateWidget("Page",
			  xmFormWidgetClass,
			  pOutput->pShell,
			  XmNresizePolicy, XmRESIZE_NONE,
			  NULL);
    pOutput->page = w;
		 
    w = XtVaCreateWidget("InnerPage",
			  xmFormWidgetClass,
			  pOutput->page,
			  XmNresizePolicy, XmRESIZE_NONE,
			  NULL);
    pOutput->innerPage = w;
		 
    w = XtVaCreateManagedWidget("HeaderLeft",
				  xmLabelWidgetClass,
				  pOutput->innerPage,
				  XmNalignment, XmALIGNMENT_BEGINNING,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL);
    pOutput->headerLeft = w;

    w = XtVaCreateManagedWidget("HeaderRight",
				  xmLabelWidgetClass,
				  pOutput->innerPage,
				  XmNalignment, XmALIGNMENT_END,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, pOutput->headerLeft,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL);
    pOutput->headerRight = w;
		 
    w = XtVaCreateManagedWidget("Editor",
#ifdef USE_DTEDITOR
				  dtEditorWidgetClass,
#else
				  xmTextWidgetClass,
#endif
				  pOutput->innerPage,
				  DtNscrollVertical, FALSE,
				  DtNscrollHorizontal, FALSE,
				  DtNshowStatusLine, FALSE,
				  DtNwordWrap, TRUE,
				  XmNeditMode, XmMULTI_LINE_EDIT,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, pOutput->headerLeft,
				  NULL);
    pOutput->editor = w;
		 
    w = XtVaCreateManagedWidget("FooterLeft",
				  xmLabelWidgetClass,
				  pOutput->innerPage,
				  XmNalignment, XmALIGNMENT_BEGINNING,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNbottomAttachment, XmATTACH_FORM,
				  NULL);
    pOutput->footerLeft = w;

    w = XtVaCreateManagedWidget("FooterRight",
				  xmLabelWidgetClass,
				  pOutput->innerPage,
				  XmNalignment, XmALIGNMENT_END,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, pOutput->footerLeft,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNbottomAttachment, XmATTACH_FORM,
				  NULL);
    pOutput->footerRight = w;

    XtVaSetValues(
		pOutput->editor,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, pOutput->footerLeft,
		NULL);

    XtManageChild(pOutput->innerPage);
    XtManageChild(pOutput->page);

    PrintOutputSetPageMargins(
		pOutput,
		_poDefaultMargin,
		_poDefaultMargin,
		_poDefaultMargin,
		_poDefaultMargin,
		&parseError );
    assert(parseError == 0);

    return pOutput;
}

/************************************************************************
 * PrintOutputCreate
 *	Class destructor.
 ************************************************************************/
void
PrintOutputDestroy(PrintOutput *pOutput)
{
    if (pOutput == NULL) return;
    XtDestroyWidget(pOutput->page);
    XtFree((char*) pOutput);
}

/************************************************************************
 * PrintOutputHideFooter
 * PrintOutputShowFooter
 * PrintOutputHideHeader
 * PrintOutputShowHeader
 *	Unmanages/Manages the footer/header widgets
 ************************************************************************/
void
PrintOutputHideFooters(PrintOutput *pOutput)
{
    XtUnmanageChild(pOutput->footerLeft);
    XtUnmanageChild(pOutput->footerRight);
    XtVaSetValues(pOutput->editor, XmNbottomAttachment, XmATTACH_FORM, NULL);
}

void
PrintOutputShowFooters(PrintOutput *pOutput)
{
    XtManageChild(pOutput->footerLeft);
    XtManageChild(pOutput->footerRight);
    XtVaSetValues(
                pOutput->editor,
                XmNbottomAttachment, XmATTACH_WIDGET,
                XmNbottomWidget, pOutput->footerLeft,
                NULL);
}

void
PrintOutputHideHeaders(PrintOutput *pOutput)
{
    XtUnmanageChild(pOutput->headerLeft);
    XtUnmanageChild(pOutput->headerRight);
    XtVaSetValues(pOutput->editor, XmNtopAttachment, XmATTACH_FORM, NULL);
}

void
PrintOutputShowHeaders(PrintOutput *pOutput)
{
    XtManageChild(pOutput->headerLeft);
    XtManageChild(pOutput->headerRight);
    XtVaSetValues(
                pOutput->editor,
                XmNtopAttachment, XmATTACH_WIDGET,
                XmNtopWidget, pOutput->headerLeft,
                NULL);
}

/************************************************************************
 * PrintOutputSetHdrFtrString
 *	Set the contents of the specifies header/footer
 ************************************************************************/
void
PrintOutputSetHdrFtrString(
			PrintOutput *pOutput,
			_DtPrintHdrFtrEnum which,
			char *contents
			)
{
    Widget	w;
    XmString	xms;

    if (contents == (char *) NULL)
      return;

    switch (which)
    {
	case DTPRINT_OPTION_HEADER_LEFT:
	  w = pOutput->headerLeft;
	  break;
	case DTPRINT_OPTION_HEADER_RIGHT:
	  w = pOutput->headerRight;
	  break;
	case DTPRINT_OPTION_FOOTER_LEFT:
	  w = pOutput->footerLeft;
	  break;
	case DTPRINT_OPTION_FOOTER_RIGHT:
	  w = pOutput->footerRight;
	  break;
	default:
	  return;
    }

    xms = XmStringCreateLocalized(contents);
    XtVaSetValues(w, XmNlabelString, xms, NULL);
    XmStringFree(xms);
}

/************************************************************************
 * PrintOutputSetHdrFtrStrings
 *	Set the contents of all the header/footers
 ************************************************************************/
void
PrintOutputSetHdrFtrStrings(
			PrintOutput *pOutput,
			char *hdrLeft,
			char *hdrRight,
			char *ftrLeft,
			char *ftrRight
			)
{
    XmString	xms;

    if (hdrLeft)
    {
        xms = XmStringCreateLocalized(hdrLeft);
        XtVaSetValues(pOutput->headerLeft, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }

    if (hdrRight)
    {
        xms = XmStringCreateLocalized(hdrRight);
        XtVaSetValues(pOutput->headerRight, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }

    if (ftrLeft)
    {
        xms = XmStringCreateLocalized(ftrLeft);
        XtVaSetValues(pOutput->footerLeft, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }

    if (ftrRight)
    {
        xms = XmStringCreateLocalized(ftrRight);
        XtVaSetValues(pOutput->footerRight, XmNlabelString, xms, NULL);
        XmStringFree(xms);
    }
}

/************************************************************************
 * PrintOutputSetPageMargin
 *	Set the contents of the specified page margin
 ************************************************************************/
void
PrintOutputSetPageMargin(
			PrintOutput *pOutput,
			_DtPrintMarginEnum which,
			const char *margin,
			Boolean *parseError
			)
{
    int		pixels = 0;
    XtEnum	xtParseError = FALSE;
    char	*marginString = XtNewString(margin);

    if (parseError)
      *parseError = FALSE;

    switch (which)
    {
	case DTPRINT_OPTION_MARGIN_TOP:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(pOutput->pShell),
					marginString,
					XmVERTICAL,
					XmPIXELS,
					&xtParseError);
	  if (!xtParseError && pixels > 0)
	    pOutput->marginTop = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_RIGHT:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(pOutput->pShell),
					marginString,
					XmHORIZONTAL,
					XmPIXELS,
					&xtParseError);
	  if (!xtParseError && pixels > 0)
	    pOutput->marginRight = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_BOTTOM:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(pOutput->pShell),
					marginString,
					XmVERTICAL,
					XmPIXELS,
					&xtParseError);
	  if (!xtParseError && pixels > 0)
	    pOutput->marginBottom = pixels;
	  break;
	case DTPRINT_OPTION_MARGIN_LEFT:
          pixels = XmConvertStringToUnits(
					XtScreenOfObject(pOutput->pShell),
					marginString,
					XmHORIZONTAL,
					XmPIXELS,
					&xtParseError);
	  if (!xtParseError && pixels > 0)
	    pOutput->marginLeft = pixels;
	  break;
	default:
	  return;
    }

    if (!xtParseError)
      _poSetInnerPageDimensions(
			pOutput,
			pOutput->marginTop,
			pOutput->marginRight,
			pOutput->marginBottom,
			pOutput->marginLeft);
    else
      *parseError = TRUE;

    XtFree(marginString);
}

/************************************************************************
 * PrintOutputSetPageMargins
 *	Set the contents of all the page margins
 ************************************************************************/
void
PrintOutputSetPageMargins(
			PrintOutput *pOutput,
			const char *top,
			const char *right,
			const char *bottom,
			const char *left,
			Boolean *parseError
			)
{
    int		pixels = 0;
    XtEnum	xtParseError = FALSE;
    char	*margin;

    *parseError = FALSE;

    margin = XtNewString(top);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(pOutput->pShell),
				margin,
				XmVERTICAL,
				XmPIXELS,
				&xtParseError);
    XtFree(margin);
    if (!xtParseError && pixels > 0)
    {
        pOutput->marginTop = pixels;
    }
    else
    {    
	*parseError = TRUE;
        return;
    }

    margin = XtNewString(right);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(pOutput->pShell),
				margin,
				XmHORIZONTAL,
				XmPIXELS,
				&xtParseError);
    XtFree(margin);
    if (!xtParseError && pixels > 0)
    {
        pOutput->marginRight = pixels;
    }
    else
    {    
	*parseError = TRUE;
        return;
    }

    margin = XtNewString(bottom);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(pOutput->pShell),
				margin,
				XmVERTICAL,
				XmPIXELS,
				&xtParseError);
    XtFree(margin);
    if (!xtParseError && pixels > 0)
    {
        pOutput->marginBottom = pixels;
    }
    else
    {    
	*parseError = TRUE;
        return;
    }

    margin = XtNewString(left);
    pixels = XmConvertStringToUnits(
				XtScreenOfObject(pOutput->pShell),
				margin,
				XmHORIZONTAL,
				XmPIXELS,
				&xtParseError);
    XtFree(margin);
    if (!xtParseError && pixels > 0)
    {
        pOutput->marginLeft = pixels;
    }
    else
    {    
	*parseError = TRUE;
        return;
    }

    _poSetInnerPageDimensions(
			pOutput,
			pOutput->marginTop,
			pOutput->marginRight,
			pOutput->marginBottom,
			pOutput->marginLeft);
}


/************************************************************************
 * PrintOutputSetWordWrap
 *	Set the wrap to fit value in the editor
 ************************************************************************/
void
PrintOutputSetWordWrap(PrintOutput *pOutput, Boolean onoff)
{
#ifdef USE_DTEDITOR
    XtVaSetValues(pOutput->editor, DtNwordWrap, onoff, NULL);
#else
    XtVaSetValues(pOutput->editor, XmNwordWrap, onoff, NULL);
#endif
}

/************************************************************************
 * PrintOutputGetLinesPerPage
 *	Returns the number of lines per page.
 ************************************************************************/
int
PrintOutputGetLinesPerPage(PrintOutput *pOutput)
{
    return ((int) pOutput->linesPerPage);
}

/************************************************************************
 * PrintOutputGetNumLines
 *	Returns the number of lines of test in the editor.
 ************************************************************************/
int
PrintOutputGetNumLines(PrintOutput *pOutput)
{
    XtArgVal p;
    int	total = 0;
    int saveTop;

#ifdef USE_DTEDITOR
    saveTop = pOutput->currentLine;
    PrintOutputFirstPage(pOutput);
    while (PrintOutputPageDown(pOutput))
      total += pOutput->linesPerPage;

    DtEditorGoToLine(pOutput->editor, saveTop);
    pOutput->currentLine = saveTop;
#else
    XtVaGetValues(pOutput->editor, XmNtotalLines, &p, NULL);
    total = (int)p;
#endif
    return(total);
}

/************************************************************************
 * PrintOutputPageUp
 ************************************************************************/
Boolean
PrintOutputPageUp(PrintOutput *pOutput)
{
    XmTextPosition	top_before, top_after;
    Boolean		retval;

#ifdef USE_DTEDITOR
    DtEditorGoToLine(
		pOutput->editor,
		pOutput->currentLine - pOutput->linesPerPage
		);
    pOutput->currentLine -= pOutput->linesPerPage;
    if (pOutput->currentLine < 1)
    {
	pOutput->currentLine = 1;
	retval = FALSE;
    }
#else
    top_before = (XmTextPosition) _poGetTopPosition(pOutput);
    XmTextScroll(pOutput->editor, -1 * pOutput->linesPerPage);
    top_after = (XmTextPosition) _poGetTopPosition(pOutput);
    retval = (top_before > top_after) ? TRUE : FALSE;
#endif

    return retval;
}

/************************************************************************
 * PrintOutputPageDown
 ************************************************************************/
Boolean
PrintOutputPageDown(PrintOutput *pOutput)
{
    XmTextPosition	top_before, top_after;

    top_before = (XmTextPosition) _poGetTopPosition(pOutput);

#ifdef USE_DTEDITOR
    DtEditorGoToLine(
		pOutput->editor,
		pOutput->currentLine + pOutput->linesPerPage
		);
    pOutput->currentLine += pOutput->linesPerPage;
#else
    XmTextScroll(pOutput->editor, pOutput->linesPerPage);
#endif

    top_after = (XmTextPosition) _poGetTopPosition(pOutput);
    return (top_before < top_after) ? TRUE : FALSE;
}

/************************************************************************
 * PrintOutputFirstPage
 ************************************************************************/
void
PrintOutputFirstPage(PrintOutput *pOutput)
{
#ifdef USE_DTEDITOR
    XtVaSetValues(pOutput->editor, DtNtopCharacter, (XmTextPosition) 0, NULL);
    pOutput->currentLine = 1;
#else
    XmTextSetTopCharacter(pOutput->editor, (XmTextPosition) 0);
#endif
}

/************************************************************************
 * PrintOutputLoadFile
 ************************************************************************/
DtEditorErrorCode
PrintOutputLoadFile( PrintOutput *pOutput, char *file)
{
    char		*contents = NULL;
    DtEditorErrorCode	errorCode;

#ifdef USE_DTEDITOR
    errorCode = DtEditorSetContentsFromFile(pOutput->editor, file);
#else
    errorCode = _poGetFileContents(&contents, file);
    if (contents != NULL)
    {
        XmTextSetString(pOutput->editor, contents);
        free(contents);
    }
#endif

    return errorCode;
}

/************************************************************************
 ************************************************************************
 *
 * Private class methods
 *
 *     _poDoGetLinesPerPage	- Returns number of rows in the editor.
 *     _poSetInnerPageDimensions- Sets the XmN x,y,width,height of InnerPage.
 *     _poGetTopPosition	- Returns the position of the 1st visible char.
 *     _poGetFileContents 	- Returns the contents of the specified file.
 *     _poStripEmbeddedNulls	- Removes any embedded NULLs (\0) in a
 *				  string of length 'length'.  The removal
 *				  occurs in place, with 'length' set to the
 *				  new, stripped length.  The resulting string
 *				  is terminated with a trailing NULL.  
 * 
 ************************************************************************
 ************************************************************************/

static int
_poDoGetLinesPerPage(PrintOutput *pOutput)
{
    XtArgVal   lpp;

    XtVaGetValues(pOutput->editor, XmNrows, &lpp, NULL);
    return ((int) lpp);
}


static void
_poSetInnerPageDimensions(
			PrintOutput *pOutput,
			Dimension top,
			Dimension right,
			Dimension bottom,
			Dimension left
			)
{
    XtArgVal	outer_height0, outer_width0, footer_height0, header_height0;
    Dimension	inner_height, inner_width, inner_x, inner_y,
		outer_height, outer_width,
		editor_height, footer_height, header_height;

    XtVaGetValues(pOutput->page,
		  XmNheight, &outer_height0,
		  XmNwidth, &outer_width0,
		  NULL);

    XtVaGetValues(pOutput->headerLeft,
		  XmNheight, &header_height0,
		  NULL);

    XtVaGetValues(pOutput->footerLeft,
		  XmNheight, &footer_height0,
		  NULL);

    outer_height = (Dimension)outer_height0;
    outer_width = (Dimension)outer_width0;
    header_height = (Dimension)header_height0;
    footer_height = (Dimension)footer_height0;

    inner_x = left;
    inner_y = top;
    inner_height = (outer_height > (Dimension) (top + bottom)) ?
			(outer_height - (top +  bottom)) :
			outer_height;
    inner_width = (outer_width > (Dimension) (left + right)) ?
			(outer_width - (left + right)) :
			outer_width;
    editor_height =
		(inner_height > (Dimension) (header_height + footer_height)) ?
			(inner_height - (header_height + footer_height)) :
			inner_height;
    
    XtVaSetValues(pOutput->editor, XmNheight, editor_height, NULL);

    XtVaSetValues(pOutput->innerPage,
		XmNleftAttachment, XmATTACH_NONE,
		XmNtopAttachment, XmATTACH_NONE,
		XmNx, inner_x,
		XmNy, inner_y,
		XmNheight, inner_height,
		XmNwidth, inner_width,
		NULL);

    pOutput->linesPerPage = _poDoGetLinesPerPage(pOutput);
}

/*
 * _poGetTopPosition
 */
static int
_poGetTopPosition(PrintOutput *pOutput)
{
    XmTextPosition top = (XmTextPosition) 0;

#ifdef USE_DTEDITOR
    XtVaGetValues(pOutput->editor, DtNtopCharacter, &top, NULL);
#else
    top = XmTextGetTopCharacter(pOutput->editor);
#endif
    return (int) top;
}

static DtEditorErrorCode
_poGetFileContents(char **contents, char *file)
{
    DtEditorErrorCode	error = DtEDITOR_NO_ERRORS;
    struct stat		statbuf;
    char		*buf;
    int			nbytes;
    FILE		*fp = NULL;

    if (file == NULL || *file == '\0') return

    /*
     * Open the file for reading.
     */
    error = _poEditorValidateFileAccess(file, READ_ACCESS);
    if (error != DtEDITOR_NO_ERRORS) return error;

    if( (fp = fopen(file, "r")) == NULL )
      return DtEDITOR_UNREADABLE_FILE;

    stat(file, &statbuf);
    nbytes = statbuf.st_size;

    /* 
     * Read the file contents (with room for null) & convert to a
     * string.  We want to use a string because the 
     * DtEditorSetContents/Append/Insert/... functions create another 
     * copy of the data before actually putting it into the widget.
     */
    buf = (char *) malloc(nbytes + 1);
    if (buf == NULL)
      return DtEDITOR_INSUFFICIENT_MEMORY;

    nbytes = fread(buf, sizeof(char), nbytes, fp);
    buf[nbytes] = '\0';

    /* 
     * Strip out any embedded NULLs because the text widget will only 
     * accept data up to the first NULL.
     */
    error = _poStripEmbeddedNulls(buf, &nbytes);

    /* Close the file */
    fclose(fp);

    *contents = buf;
    return error;
} /* end LoadFile */

static DtEditorErrorCode 
_poStripEmbeddedNulls(char *strbuf, int *len)
{
    DtEditorErrorCode retval = DtEDITOR_NO_ERRORS;

    if (strlen(strbuf) != *len)
    {
       int firstNull;

       retval = DtEDITOR_NULLS_REMOVED;

       /*
        * The file contains NULL characters, so we strip them out and
        * report that we have done so.
        */
       while((firstNull = strlen(strbuf)) != *len)
       {
          int lastNull = firstNull;

          while((lastNull+1) < *len && strbuf[lastNull+1] == (char)'\0')
            lastNull++;

          memcpy(&strbuf[firstNull], &strbuf[lastNull+1], *len-lastNull);
          *len -= 1 + lastNull - firstNull;
       }
    }

    return retval;
}

extern DtEditorErrorCode
_poEditorValidateFileAccess( char *file, int accessType )
{
   struct stat statbuf;
   unsigned short tmpMode;

   if (file && *file ) return DtEDITOR_INVALID_FILENAME;

   if (access(file, F_OK) != 0) return DtEDITOR_NONEXISTENT_FILE; 

   if( stat(file, &statbuf) != 0 ) return DtEDITOR_NO_FILE_ACCESS;

   if( (statbuf.st_mode & S_IFMT) == S_IFDIR ) return DtEDITOR_DIRECTORY;

   if( (statbuf.st_mode & S_IFMT) == S_IFCHR ) return DtEDITOR_CHAR_SPECIAL_FILE;

   if((statbuf.st_mode & S_IFMT) == S_IFBLK) return DtEDITOR_BLOCK_MODE_FILE;

   switch( accessType )
   {
      case READ_ACCESS:
	if (access(file, R_OK) != 0) return DtEDITOR_UNREADABLE_FILE;
	break;
      case WRITE_ACCESS:
	if (access(file, W_OK) != 0) return DtEDITOR_UNWRITABLE_FILE;
	break;
      default:
	break;
   } /* end stat suceeded */

   return DtEDITOR_NO_ERRORS;
}
