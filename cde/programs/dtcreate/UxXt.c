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
static char sccsid[] = "@(#)96  1.4  com/config/UxXt.c, aic, aic324, 9317324f 5/12/93 15:40:01";
/*
 *  COMPONENT_NAME: AIC           AIXwindows Interface Composer
 *
 *  ORIGINS: 58
 *
 *
 *                   Copyright IBM Corporation 1991, 1993
 *
 *                         All Rights Reserved
 *
 *   Permission to use, copy, modify, and distribute this software and its
 *   documentation for any purpose and without fee is hereby granted,
 *   provided that the above copyright notice appear in all copies and that
 *   both that copyright notice and this permission notice appear in
 *   supporting documentation, and that the name of IBM not be
 *   used in advertising or publicity pertaining to distribution of the
 *   software without specific, written prior permission.
 *
 *   IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *   PURPOSE. IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 *   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 *   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 *   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 *   OR PERFORMANCE OF THIS SOFTWARE.
*/
/*---------------------------------------------------------------------
 * $XConsortium: UxXt.c /main/4 1995/11/01 16:08:57 rswiston $
 *---------------------------------------------------------------------
 *
 *
 *             Copyright (c) 1991, Visual Edge Software Ltd.
 *
 * ALL  RIGHTS  RESERVED.  Permission  to  use,  copy,  modify,  and
 * distribute  this  software  and its documentation for any purpose
 * and  without  fee  is  hereby  granted,  provided  that the above
 * copyright  notice  appear  in  all  copies  and  that  both  that
 * copyright  notice and this permission notice appear in supporting
 * documentation,  and that  the name of Visual Edge Software not be
 * used  in advertising  or publicity  pertaining to distribution of
 * the software without specific, written prior permission. The year
 * included in the notice is the year of the creation of the work.
 *-------------------------------------------------------------------*/
 /*------------------------------------------------------------------------
 *                              UxXt.c
 *-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#include <X11/Xmd.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>

#ifdef UIL_CODE
#include <Mrm/MrmPublic.h>
#endif /* UIL_CODE */

#include "UxXt.h"

static  XContext        xcontext_id = 0;

/******************************************************************************
NAME:           GetTrueToplevel( wgt )

INPUT:          Widget  wgt             - the top user-created widget

RETURN:         Widget                  - the shell widget

DESCRIPTION:    Returns the true toplevel of that interface.
                This is needed for implicit shells where the widget passed to
                one of the Ux functions is not always the shell.

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
Widget  GetTrueToplevel( Widget wgt )
{
        while (wgt && !XtIsShell(wgt))
        {
                wgt = XtParent(wgt);
        }
        return wgt;
}

/******************************************************************************
NAME:           handle_dialog_child( wgt, manage_func )

INPUT:          Widget  wgt                     - the dialogShellWidget
                void    (*manage_func)()        - either XtManageChild
                                                  or XtUnmanageChild

RETURN:         int                             - UX_NO_ERROR if successfull
                                                  UX_ERROR otherwise

DESCRIPTION:    Handles the popping up or popping down of dialog shells
                by managing or unmanaging their children.

CREATION:       Visual Edge Software            Sept 19/91
-----------------------------------------------------------------------------*/
static  int     handle_dialog_child( Widget wgt, void (*manage_func)(Widget) )
{
        XtArgVal i, num_children;
        Widget  *children;
        int     error_flag = UX_ERROR;

        XtVaGetValues( wgt,
                       XmNnumChildren, &num_children,
                       XmNchildren, &children,
                       NULL );

        /*-----------------------------------------------------
         * We manage/unmanage the first rectObj child in the list.
         * Note that the check for rectObjClass is necessary since
         * some implementations of Motif add protocol children to
         * the dialogShell. Additionally, when the LANG is set to
         * Japanese, a widget of class Core is created. This widget
         * should not be managed. We'll skip it.
         *-----------------------------------------------------*/

        for (i = 0; i < num_children; i++)
        {
                if ( XtIsSubclass( children[i], rectObjClass )  &&
                        (XtClass (children[i]) != coreWidgetClass))
                {
                        (*manage_func)(children[i]);
                        error_flag = UX_NO_ERROR;
                        break;
                }
        }

        return ( error_flag );
}

/******************************************************************************
NAME:           popup_dialog( wgt, grab_flag )

INPUT:          Widget          wgt             - dialogShell to pop up
                XtGrabKind      grab_flag       - the grab flag

RETURN:         void

DESCRIPTION:    Pops up a dialogShell.

CREATION:       Visual Edge Software            Sept 19/91
-----------------------------------------------------------------------------*/
static  void    popup_dialog( Widget wgt, XtGrabKind grab_flag )
{
        if ( handle_dialog_child( wgt, XtManageChild ) == UX_ERROR )
                XtPopup( wgt, grab_flag );
}

/******************************************************************************
NAME:           UxPopupInterface( wgt, grab_flag )

INPUT:          Widget          wgt             - Widget to popup
                XtGrabKind      grab_flag       - grab flag

RETURN:         int                     UX_ERROR or UX_NO_ERROR

DESCRIPTION:    Popups up an interface. The widget should be a toplevel widget.
                Note that special handling is required for dialogShells since
                those are popped up by managing their children if they have
                some.
                The grab_flag could be any of:
                                no_grab (XtGrabNone)
                                nonexclusive_grab (XtGrabNonexclusive)
                                exclusive_grab (XtGrabExclusive)

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
int     UxPopupInterface( Widget wgt, XtGrabKind grab_flag )
{
        if (!(wgt = GetTrueToplevel(wgt)))
                return ( UX_ERROR );

        if ( XtIsSubclass( wgt, xmDialogShellWidgetClass ) )
        {
                popup_dialog( wgt, grab_flag );
        }
        else
        {
                XtPopup( wgt, grab_flag );
        }

        return ( UX_NO_ERROR );
}

/******************************************************************************
NAME:           popdown_dialog( wgt )

INPUT:          Widget  wgt             - dialogShell to popdown

RETURN:         void

DESCRIPTION:    Pops down a dialogShell.

CREATION:       Visual Edge Software            Sept 19/91
-----------------------------------------------------------------------------*/
static  void    popdown_dialog( Widget wgt )
{
        if ( handle_dialog_child( wgt, XtUnmanageChild ) == UX_ERROR )
                XtPopdown( wgt );
}

/******************************************************************************
NAME:           UxPopdownInterface( wgt )

INPUT:          Widget  wgt             - Widget to popdown

RETURN:         int                     UX_ERROR / UX_NO_ERROR

DESCRIPTION:    Pops down an interface. The widget should be a toplevel widget.
                Note that special handling is required for dialogShells since
                those are popped down by unmanaging their children if they have
                some.

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
int     UxPopdownInterface( Widget wgt )
{
        if (!(wgt = GetTrueToplevel(wgt)))
                return ( UX_ERROR );

        if ( XtIsSubclass( wgt, xmDialogShellWidgetClass ) )
        {
                popdown_dialog( wgt );
        }
        else
        {
                XtPopdown( wgt );
        }

        return ( UX_NO_ERROR );
}

/******************************************************************************
NAME:           UxDeleteContextCB( wgt, client_data, call_data )

INPUT:          Widget          wgt             - widget causing the callback
                XtPointer       client_data     - not used
                XtPointer       call_data       - not used

RETURN:         void

DESCRIPTION:    Deletes the X context entry.

EXT REFERENCES: UxTopLevel, xcontext_id

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
void    UxDeleteContextCB( Widget wgt, XtPointer client_data,
                                                XtPointer _call_data )
{
        XtPointer       call_data = _call_data;

        (void) XDeleteContext( XtDisplay( UxTopLevel ),
                               (Window) wgt,
                               (XContext)(XtArgVal) client_data );
}

/******************************************************************************
NAME:           UxPutContext( wgt, context )

INPUT:          Widget  wgt             - Widget
                XtPointer context       - context pointer

RETURN:         int                     UX_ERROR / UX_NO_ERROR

DESCRIPTION:    Uses the X Context manager to store the given context pointer
                in a memory location that is indexed by the given widget id.
                Also adds a destroyCallback to delete that context when the
                widget is destroyed.

EXT REFERENCES: UxTopLevel, xcontext_id
EXT EFFECTS:    xcontext_id

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
int     UxPutContext( Widget wgt, caddr_t context )
{
        int             status;

        if ( xcontext_id == 0 )
                xcontext_id = XUniqueContext();

        if ( wgt == NULL )
                return ( UX_ERROR );

        status = XSaveContext( XtDisplay( UxTopLevel ),
                               (Window) wgt,
                               xcontext_id,
                               (char *) context );
        if ( status != 0 )
                return ( UX_ERROR );

        XtAddCallback (wgt, XmNdestroyCallback,
                        UxDeleteContextCB, (XtPointer)(XtArgVal) xcontext_id);

        return ( UX_NO_ERROR );
}

/******************************************************************************
NAME:           UxGetContext( wgt )

INPUT:          Widget  wgt             - widget

RETURN:         caddr_t                 - the context pointer

DESCRIPTION:    Uses the X Context manager to find the context pointer
                stored in a memory location indexed by the given widget id.

EXT REFERENCES: UxTopLevel, xcontext_id

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
caddr_t         UxGetContext( Widget wgt )
{
        int             status;
        caddr_t         context;

        if ( wgt == NULL )
                return ( (caddr_t) NULL );

        status = XFindContext( XtDisplay( UxTopLevel ),
                               (Window) wgt,
                               xcontext_id,
                               &context );

        if ( status != 0 )
                return  ( (caddr_t) NULL );

        return ( context );
}

/******************************************************************************
NAME:           DelayedFreeData ( client_data, id )

INPUT:          XtPointer       client_data     - pointer to be freed
                XtIntervalId    *id;

RETURN:         void

DESCRIPTION:    This XtTimerCallbackProc function simply frees the client data.

CREATION:       Visual Edge Software            April 30 1993
-----------------------------------------------------------------------------*/
static  void    DelayedFreeData( XtPointer client_data, XtIntervalId *_id)
{
        XtIntervalId    *id = _id;

        if (client_data != NULL) {
                XtFree((char *) client_data);
        }
}
/******************************************************************************
NAME:           UxDestroyContextCB ( wgt, client_data, call_data )

INPUT:          Widget          wgt             - widget
                XtPointer       client_data     - pointer to be freed
                XtPointer       call_data       - not used

RETURN:         void

DESCRIPTION:    This callback function registers a timeout to free the
                context structure. This mechanism is used to ensure that
                user's destroyCallbacks are executed before we free the
                context structure.

CREATION:       Visual Edge Software            April 6 1991
-----------------------------------------------------------------------------*/
void    UxDestroyContextCB( Widget _wgt, XtPointer client_data,
                                XtPointer _call_data )
{
        Widget          wgt = _wgt;
        XtPointer       call_data = _call_data;

        if (client_data != NULL) {
                XtAppAddTimeOut(UxAppContext, 0,
                                DelayedFreeData, client_data);
        }
}

/******************************************************************************
NAME:           UxConvertFontList( fontlist_str )

INPUT:          char    *fontlist_str           - string form of fontlist

RETURN:         XmFontList                      - fontlist

DESCRIPTION:    Converts a fontlist from the resource-file string format
                to the XmFontList type expected in C code.
                The conversion is done by a call to the built-in Motif convertor
                and the return value points into Xt's resource cache
                and so the return value should NOT be XmFontListFree'd.

CREATION:       Visual Edge Software            June 17 1992
-----------------------------------------------------------------------------*/
XmFontList    UxConvertFontList( char *fontlist_str )
{
        XrmValue        from, to;
        XmFontList      fontlist = NULL;
        Boolean         status;

        from.size = strlen( fontlist_str ) + 1;
        from.addr = fontlist_str;

        to.size = sizeof(XmFontList);
        to.addr = (caddr_t) &fontlist;

        status = XtConvertAndStore( UxTopLevel,
                                    XmRString, &from,
                                    XmRFontList, &to );

        return ( fontlist );
}

/******************************************************************************
NAME:           static ConvertPixmap( file_name, depth )

INPUT:          char    *file_name              - Pixmap file name
                int     depth                   - 0 for no depth specified

RETURN:         Pixmap

DESCRIPTION:    If the depth argument is 0 , then call XmGetPixmap (no depth
                argument). Otherwise, call XmGetPixmapByDepth (with depth as
                specified by the argument).
                Since there is no way of knowing how long the Pixmaps will be
                used, they are never destroyed. This is not a serious problem
                since XmGetPixmap does not regenerate existing Pixmaps.
                It simply keeps a reference count for the number of times a
                Pixmap is returned.

CREATION:       Visual Edge Software            March 31, 1993
-----------------------------------------------------------------------------*/
static Pixmap    ConvertPixmap( char *file_name, int depth )
{
        XrmValue fg_from, fg_to, bg_from, bg_to;
        Pixel fg, bg;

        fg_from.size = strlen(XtDefaultForeground);
        fg_from.addr = XtDefaultForeground;
        fg_to.addr = (XPointer)&fg;
        bg_from.size = strlen(XtDefaultBackground);
        bg_from.addr = XtDefaultBackground;
        bg_to.addr = (XPointer)&bg;
        if (!XtConvertAndStore
             (UxTopLevel, XtRString, &bg_from, XtRPixel, &bg_to)
            || !XtConvertAndStore
                (UxTopLevel, XtRString, &fg_from, XtRPixel, &fg_to)
            || (fg == bg)) {
                fg = WhitePixelOfScreen(XtScreen(UxTopLevel));
                bg = BlackPixelOfScreen(XtScreen(UxTopLevel));
                }
        if (depth)
                return (XmGetPixmapByDepth
                        (XtScreen(UxTopLevel), file_name, fg, bg, depth));

        else
                return (XmGetPixmap(XtScreen(UxTopLevel), file_name, fg, bg));
}

/******************************************************************************
NAME:           UxConvertPixmap( file_name )

INPUT:          char    *file_name              - pixmap file name

RETURN:         Pixmap                          - Pixmap

DESCRIPTION:    Call ConvertPixmap with depth 0 (Pixmap)

CREATION:       Visual Edge Software            March 31, 1993
-----------------------------------------------------------------------------*/
Pixmap    UxConvertPixmap( char *file_name )
{
        return (ConvertPixmap(file_name, 0));
}

/*------------------------------------------------------------------------
 * NAME:        UxNewContext
 *      <Allocate a context structure>
 * INPUT:       size    -- of the desired structure
 *              isSubclass      -- nonzero if the requester is a subclass,
 *                                 requiring that the result also be
 *                                 returned for the base.
 * RETURNS:     Pointer to a data area of (at least) the desired size.
 *------------------------------------------------------------------------*/
void*   UxNewContext (size_t size, int isSubclass)
{
        static void* LastSubclassResult = 0;
        static int   LastResultSize = 0;

        void * result;

        if (LastSubclassResult) {
                result = LastSubclassResult;
        } else {
                result = XtMalloc(size);
        }

        if (isSubclass) {
                LastSubclassResult = result;
                if (LastResultSize  < size) {
                        LastResultSize   = size;
                }
        } else {
                LastSubclassResult = 0;
                LastResultSize     = 0;
        }

        return (result);
}
/**  end of file ***/
