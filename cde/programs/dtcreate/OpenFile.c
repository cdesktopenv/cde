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
/* $XConsortium: OpenFile.c /main/6 1996/10/17 14:06:40 rswiston $ */
/*******************************************************************************
        OpenFile.c

       Associated Header file: OpenFile.h
*******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include <Xm/FileSB.h>
#include <Dt/Icon.h>
#include "UxXt.h"
#include "dtcreate.h"
#include "cmnrtns.h"
#include "cmncbs.h"
#include "ca_aux.h"
#include "fileio.h"

#define CONTEXT_MACRO_ACCESS 1
#include "OpenFile.h"
#undef CONTEXT_MACRO_ACCESS

extern XtPointer
_XmStringUngenerate(XmString, XmStringTag, XmTextType, XmTextType);

/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

static  void    cancelCB_OpenFile( Widget UxWidget,
                                  XtPointer UxClientData,
                                  XtPointer UxCallbackArg )

{
        _UxCOpenFile            *UxSaveCtx, *UxContext;

        UxSaveCtx = UxOpenFileContext;
        UxOpenFileContext = UxContext =
                        (_UxCOpenFile *) UxGetContext( UxWidget );
        {

        UxPopdownInterface(UxWidget);
        }
        UxOpenFileContext = UxSaveCtx;
}

static  void    okCallback_OpenFile( Widget UxWidget,
                                    XtPointer UxClientData,
                                    XtPointer UxCallbackArg )

{
        _UxCOpenFile                     *UxSaveCtx, *UxContext;
        XmFileSelectionBoxCallbackStruct *cbs;
        /*char        *text;*/
        int         rc;
        ActionData  ADTmp;

        UxSaveCtx = UxOpenFileContext;
        UxOpenFileContext = UxContext =
                        (_UxCOpenFile *) UxGetContext( UxWidget );
        {

        /********************************************************************/
        /* Call file parsing code here!                                     */
        /********************************************************************/
        cbs = (XmFileSelectionBoxCallbackStruct *)UxCallbackArg;
        if (pszFileToEdit) {
           XtFree(pszFileToEdit);
        }
	pszFileToEdit =
	    _XmStringUngenerate(cbs->value, NULL,
				XmCHARSET_TEXT, XmCHARSET_TEXT);
        if (pszFileToEdit != (char *)NULL) {
           memset(&ADTmp, 0, sizeof(ActionData));
           rc = OpenDefinitionFile(pszFileToEdit, &ADTmp);
           /*XtFree(text);*/
           /*****************************************************************/
           /* If everything ok, then clear out data from action structure   */
           /* and store new data.                                           */
           /*****************************************************************/
           if (!rc) {
              FreeAndClearAD(&AD);
              memcpy(&AD, &ADTmp, sizeof(ActionData));
           }
        }
        UxPopdownInterface (UxWidget);

        /********************************************************************/
        /* If successfully opened and parsed file, then init the fields of  */
        /* the main window.                                                 */
        /********************************************************************/
        if (!rc) {
           clear_CreateActionAppShell_fields();
           writeCAToGUI(&AD);
        }
        }
        UxOpenFileContext = UxSaveCtx;
}

static  void    helpCallback_OpenFile( Widget UxWidget,
                                    XtPointer UxClientData,
                                    XtPointer UxCallbackArg )

{
        _UxCOpenFile            *UxSaveCtx, *UxContext;

        UxSaveCtx = UxOpenFileContext;
        UxOpenFileContext = UxContext =
                        (_UxCOpenFile *) UxGetContext( UxWidget );
        {
        DisplayHelpDialog(UxWidget, (XtPointer)HELP_OPENFILE, UxCallbackArg);
        }
        UxOpenFileContext = UxSaveCtx;
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget   _Uxbuild_OpenFile(void)
{
        Widget          _UxParent;

        /* Creation of OpenFile */
        _UxParent = XtVaCreatePopupShell( "OpenFile_shell",
                        xmDialogShellWidgetClass, UxTopLevel,
                        XmNx, 200,
                        XmNy, 290,
                        XmNwidth, 398,
                        XmNheight, 500,
                        XmNallowShellResize, TRUE,
                        XmNshellUnitType, XmPIXELS,
                        XmNtitle, GETMESSAGE(12, 25, "Create Action - Open"),
                        NULL );

        OpenFile = XtVaCreateWidget( "OpenFile",
                        xmFileSelectionBoxWidgetClass,
                        _UxParent,
                        XmNresizePolicy, XmRESIZE_GROW,
                        XmNunitType, XmPIXELS,
                        XmNwidth, 398,
                        XmNheight, 500,
                        RES_CONVERT( XmNdialogTitle, GETMESSAGE(12, 25, "Create Action - Open")),
                        RES_CONVERT( XmNdirectory, "" ),
                        RES_CONVERT( XmNtextString, "" ),
                        RES_CONVERT( XmNdirSpec, "" ),
                        XmNdialogType, XmDIALOG_FILE_SELECTION,
                        XmNtextColumns, 20,
                        XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                        XmNallowOverlap, FALSE,
                        RES_CONVERT( XmNchildPlacement, "place_below_selection" ),
                        XmNdefaultPosition, FALSE,
                        RES_CONVERT( XmNdirMask, "" ),
                        RES_CONVERT( XmNpattern, "" ),
                        NULL );
        XtAddCallback( OpenFile, XmNcancelCallback,
                (XtCallbackProc) cancelCB_OpenFile,
                (XtPointer) UxOpenFileContext );
        XtAddCallback( OpenFile, XmNokCallback,
                (XtCallbackProc) okCallback_OpenFile,
                (XtPointer) UxOpenFileContext );
        XtAddCallback( OpenFile, XmNhelpCallback,
                (XtCallbackProc) helpCallback_OpenFile,
                (XtPointer) UxOpenFileContext );
        XtVaSetValues(OpenFile, XmNuserData, OpenFile, NULL);

        UxPutContext( OpenFile, (char *) UxOpenFileContext );

        XtAddCallback( OpenFile, XmNdestroyCallback,
                (XtCallbackProc) UxDestroyContextCB,
                (XtPointer) UxOpenFileContext);

        return ( OpenFile );
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

Widget  create_OpenFile(void)
{
        Widget                  rtrn;
        _UxCOpenFile            *UxContext;

        UxOpenFileContext = UxContext =
                (_UxCOpenFile *) UxNewContext( sizeof(_UxCOpenFile), False );


        rtrn = _Uxbuild_OpenFile();

        return(rtrn);
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

