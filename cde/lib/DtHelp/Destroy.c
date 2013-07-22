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
/* $XConsortium: Destroy.c /main/12 1996/03/18 14:08:16 cde-hal $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Destroy.c
 **
 **   Project:     Cde Help System
 **
 **   Description: This code frees the information associated with
 **                a Display Area.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <stdlib.h>
#include <X11/Xatom.h>

/*
 * Canvas Engine
 */
#include "CanvasP.h"

/*
 * private includes
 */
#include "DisplayAreaP.h"
#include "CallbacksI.h"
#include "DestroyI.h"
#include "FontI.h"
#include "GraphicsI.h"
#include "HyperTextI.h"
#include "StringFuncsI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Public Function Declarations    ********/

/******************************************************************************
 *
 * These are the public routines available.
 *
 *****************************************************************************/
/*********************************************************************
 * Function: _DtHelpDisplayAreaClean
 *
 *    _DtHelpDisplayAreaClean deallocates all paragraph, font, and
 *    graphic information associated with it.
 *
 *********************************************************************/
void
_DtHelpDisplayAreaClean (XtPointer  client_data)
{
    int   n;
    Arg   args[5];
    Atom  xa_primary = XA_PRIMARY;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * remove the selection if necessary
     */
    _DtHelpLoseSelectionCB (pDAS->dispWid, &xa_primary);

    /*
     * clean the canvas
     */
    if (NULL != pDAS->canvas)
    	_DtCanvasClean (pDAS->canvas);

    /*
     * clean the slate
     */
    if (XtIsRealized (pDAS->dispWid))
        XClearArea(XtDisplay(pDAS->dispWid), XtWindow(pDAS->dispWid),
			pDAS->decorThickness, pDAS->decorThickness,
			pDAS->dispUseWidth, pDAS->dispUseHeight, False);

    pDAS->toc_flag &= ~(_DT_HELP_TOC_ON);

    /*
     * reset any scrollbars that are managed.
     */
    n = 0;
    XtSetArg (args[n], XmNvalue        , 0);	++n;
    XtSetArg (args[n], XmNsliderSize   , 1);	++n;
    XtSetArg (args[n], XmNmaximum      , 1);	++n;
    XtSetArg (args[n], XmNpageIncrement, 1);	++n;

    if (pDAS->vertScrollWid && XtIsManaged (pDAS->vertScrollWid))
	XtSetValues (pDAS->vertScrollWid, args, n);

    if (pDAS->horzScrollWid && XtIsManaged (pDAS->horzScrollWid))
	XtSetValues (pDAS->horzScrollWid, args, n);

}  /* End _DtHelpDisplayAreaClean */

/*********************************************************************
 * Function: _DtHelpDisplayAreaDestroyCB
 *
 *    _DtHelpDisplayAreaDestroyCB deallocates all information
 *    associated with it.
 *
 *********************************************************************/
void
_DtHelpDisplayAreaDestroyCB (
    Widget w,
    XtPointer client_data,
    XtPointer call_data)
{
    int      i;
    long     strIdx;
    XrmName		  xrmList[_DtHelpFontQuarkNumber];
    Display		 *dpy;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    Boolean  usedDef = False;

    /*
     * do the gross cleaning
     */
    _DtHelpDisplayAreaClean(client_data);

    if (NULL != pDAS->canvas)
	_DtCanvasDestroy (pDAS->canvas);

    dpy = XtDisplay(pDAS->dispWid);
    /*
     * Free the gc's
     */
    XFreeGC (dpy, pDAS->normalGC);
    XFreeGC (dpy, pDAS->pixmapGC);
    XFreeGC (dpy, pDAS->invertGC);
    if (pDAS->def_pix != 0 &&
	XmDestroyPixmap(XDefaultScreenOfDisplay(dpy), pDAS->def_pix) == False)
	    XFreePixmap(dpy, pDAS->def_pix);
    if (pDAS->context != NULL)
    {
        _DtGrDestroyContext(pDAS->context);
        XtFree((char *) pDAS->context);
    }

    /*
     * destroy the widgets.
     */
    XtDestroyWidget(pDAS->dispWid);

    if (pDAS->horzScrollWid != NULL)
        XtDestroyWidget(pDAS->horzScrollWid);
    if (pDAS->vertScrollWid != NULL)
        XtDestroyWidget(pDAS->vertScrollWid);

    /*
     * free the special characters
     */
    if (pDAS->spc_chars != NULL)
        free ((char *) pDAS->spc_chars);

    /*
     * free the font info.
     */
    _DtHelpGetStringQuarks(xrmList);
    if (__DtHelpFontIndexGet(pDAS, xrmList, &strIdx) != 0)
	usedDef = True;

    if (pDAS->font_info.def_font_db != NULL)
        XrmDestroyDatabase(pDAS->font_info.def_font_db);
    if (pDAS->font_info.font_idx_db != NULL)
        XrmDestroyDatabase(pDAS->font_info.font_idx_db);
    if (pDAS->font_info.exact_fonts != NULL)
      {
	_DtHelpCeFreeStringArray(pDAS->font_info.exact_fonts);
	free(pDAS->font_info.exact_idx);
      }
    if (pDAS->font_info.font_structs != NULL)
      {
	/*
	 * if the string index is a positive number, this
	 * indicates that the string font is a font not
	 * a font set and it wouldn't be freed in this
	 * routine.
	 */
	for (i = 0; i < pDAS->font_info.struct_cnt; i++)
	  {
	    if (usedDef == True || i != strIdx)
	        XFreeFont(dpy, pDAS->font_info.font_structs[i]);
	  }
        free(pDAS->font_info.font_structs);
      }
    if (pDAS->font_info.font_sets != NULL)
      {
	/*
	 * if the string index is a negative number, this turns
	 * it to positive and the tests will succeed.
	 *
	 * if the string index was a font not a fontset, then
	 * this will turn the value negative and the test
	 * will never succeed.
	 */
	strIdx = (-strIdx) - 1;
	for (i = 0; i < pDAS->font_info.set_cnt; i++)
	  {
	    if (usedDef == True || i != strIdx)
	        XFreeFontSet(dpy, pDAS->font_info.font_sets[i]);
	  }
        free(pDAS->font_info.font_sets);
        free(pDAS->font_info.fs_metrics);
      }

    /*
     * free the locale dependant information
     */
    if (NULL != pDAS->cant_begin_chars)
        free (pDAS->cant_begin_chars);
    if (NULL != pDAS->cant_end_chars)
        free (pDAS->cant_end_chars);

    /*
     * destroy the display area pointer
     */
    XtFree ((char *) pDAS);

}  /* End DtHelpDisplayAreaDestroy */
