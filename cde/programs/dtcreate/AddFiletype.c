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
/* $TOG: AddFiletype.c /main/8 1999/09/17 15:43:02 mgreess $ */
/***************************************************************************/
/*                                                                         */
/*  AddFiletype.c                                                          */
/*                                                                         */
/*    Associated Header file:  AddFiletype.h                               */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include <sys/stat.h>

#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ScrolledW.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/Form.h>

#include <Dt/Icon.h>

#include "UxXt.h"
#include "dtcreate.h"
#include "af_aux.h"
#include "CreateActionAppShell.h"
#include "FileCharacteristics.h"
#include "cmncbs.h"

#define CONTEXT_MACRO_ACCESS 1
#include "AddFiletype.h"
#undef CONTEXT_MACRO_ACCESS

/*******************************************************************************
       Global declarations.
*******************************************************************************/
/*
enum mode {NONE, NEW, SAVE, QUIT};
Widget activateField;
Widget Licon2;
extern int do_which;
enum icon_size_range filetype_icon_size = None_Selected;
*/
enum icon_size_range filetype_icon_size;


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

#if 0
/******************************************************************************/
/*                                                                            */
/* LayoutCB_AddFiletype                                                       */
/*                                                                            */
/* INPUT:  Widget    wid       - window id                                    */
/*         XtPointer cdata     - client data                                  */
/*         XtPointer *cbstruct - callback data                                */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void LayoutCB_AddFiletype(Widget wid, XtPointer cdata, XtPointer cbs)
{
  printf("In layout callback of AddFiletype\n");
}
#endif

/******************************************************************************/
/* activateCB_edit_id_characteristics - bring up file characteristics dialog  */
/* INPUT:  Widget filesel - edit button id                                    */
/*         XtPointer cdata - client data                                      */
/*         XtPointer *cbstruct - callback data                                */
/* OUTPUT: none                                                               */
/******************************************************************************/
void activateCB_edit_id_characteristics (Widget filesel, XtPointer cdata,
                   XmFileSelectionBoxCallbackStruct *cbstruct)
{
  FiletypeData  *pFtD;

  XtVaGetValues (AddFiletype,
                 XmNuserData, &pFtD,
                 NULL);

/*FileCharacteristics = create_FileCharacteristics();*/

  XtVaSetValues (FileCharacteristics,
                 XmNuserData, pFtD,
                 NULL);
  clear_FileCharacteristics_dialog_fields();
  init_FileCharacteristics_dialog_fields(pFtD);
  UxPopupInterface (FILE_CHARACTERISTICS, no_grab);
  return;
}

static  void    activateCB_AF_OkButton( Widget  UxWidget,
                                       XtPointer UxClientData,
                                       XtPointer UxCallbackArg)
{
  FiletypeData  *pFtD;
  FiletypeData  **papArray;
  int           i;

  /**************************************************************************/
  /* Check if fields are valid.                                             */
  /**************************************************************************/
  if (!AddFiletypeCheckFields()) {

     /***********************************************************************/
     /* Get FiletypeData structure that was allocated when Add button on    */
     /* the CreateActionAppShell interface was activated.                   */
     /***********************************************************************/
     XtVaGetValues(AddFiletype, XmNuserData, &pFtD, NULL);

     /***********************************************************************/
     /* Get all values from gui                                             */
     /***********************************************************************/
     readAFFromGUI(pFtD);

     /***********************************************************************/
     /* If in edit mode, then don't do add type things.                     */
     /***********************************************************************/
     if (!(pFtD->fsInternal & CA_FT_EDIT)) {

        /********************************************************************/
        /* Update the Filetype listbox with this new filetype               */
        /********************************************************************/
        AddFiletypeToList();

        /********************************************************************/
        /* Update FiletypeData array within the ActionData structure        */
        /********************************************************************/
        UpdateFiletypeDataArray(pFtD);

     }

     /***********************************************************************/
     /* Unset previous set values.                                          */
     /***********************************************************************/
     pFtD->fsInternal &= (~(CA_FT_APPLY));
     pFtD->fsInternal &= (~(CA_FT_EDIT));

     /***********************************************************************/
     /* Pop down the AddFiletype interface                                  */
     /***********************************************************************/
     UxPopdownInterface(AddFiletype);
  }
}

static  void    activateCB_AF_ApplyButton( Widget UxWidget,
                                          XtPointer UxClientData,
                                          XtPointer UxCallbackArg)
{
  FiletypeData  *pFtD;

  /**************************************************************************/
  /* Check if fields are valid.                                             */
  /**************************************************************************/
  if (!AddFiletypeCheckFields()) {

     /***********************************************************************/
     /* Get FiletypeData structure that was allocated when Add button on    */
     /* the CreateActionAppShell interface was activated.                   */
     /***********************************************************************/
     XtVaGetValues(AddFiletype, XmNuserData, &pFtD, NULL);

     /***********************************************************************/
     /* Get all values from gui                                             */
     /***********************************************************************/
     readAFFromGUI(pFtD);

     /***********************************************************************/
     /* Set the variable that indicates that Apply button was used.         */
     /***********************************************************************/
     pFtD->fsInternal |= CA_FT_APPLY;

     /***********************************************************************/
     /* If in edit mode, then don't do apply type things.                   */
     /***********************************************************************/
     if (!(pFtD->fsInternal & CA_FT_EDIT)) {

        /********************************************************************/
        /* Update the Filetype listbox with this new filetype               */
        /********************************************************************/
        if (XmListItemExists(CA_FiletypesList, XmStringCreateSimple("NONE"))) {
          XmListDeleteItem(CA_FiletypesList, XmStringCreateSimple("NONE"));
        }
        AddFiletypeToList();

        /********************************************************************/
        /* Update FiletypeData array within the ActionData structure        */
        /********************************************************************/
        UpdateFiletypeDataArray(pFtD);

     }
  }
}

static  void    activateCB_AF_CancelButton( Widget UxWidget,
                                           XtPointer UxClientData,
                                           XtPointer UxCallbackArg)
{
   FiletypeData *pFtD;

   UxPopdownInterface(AddFiletype);
   XtVaGetValues(AddFiletype, XmNuserData, &pFtD, NULL);
   if (!((pFtD->fsInternal & CA_FT_APPLY) || (pFtD->fsInternal & CA_FT_EDIT))) {
      free_Filetypedata(pFtD);
   } else {
      pFtD->fsInternal &= (~(CA_FT_APPLY));
      pFtD->fsInternal &= (~(CA_FT_EDIT));
   }
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget   _Uxbuild_AddFiletype(void)
{
        Widget                _UxParent;
        char                  *UxTmp0;
        Widget                AF_CharacteristicsWindow;
        Widget                ActionAreaForm;
        int                   ntotalbuttons = 4;
        int                   nbutton = 0;
        Widget                AF_ScrolledWindow;
        Widget                AF_BigForm;
        XtArgVal /* Dimension */             dim1, dim2, dim3;
        Widget                AF_MED_IconForm, AF_TINY_IconForm;
        IconData             *pIconData;
	char		     *pre, *suf, *title;

	XmString	      emptyString;
    size_t len;

#define TIGHTNESS             20
#define ICON_MIN_HEIGHT       83
#define FINDEDIT_SPACING       6
#define BORDER_OFFSET          3

        /* Creation of AddFiletype */
        _UxParent = UxParent;
        if ( _UxParent == NULL )
        {
             _UxParent = UxTopLevel;
        }

        pre = GETMESSAGE(3, 10, "Create Action");
        suf = GETMESSAGE(7, 10, "Add Datatype");
        len = strlen(pre) + strlen(suf) + 4;
        title = XtMalloc(len);
        snprintf(title,len - 1, "%s - %s", pre, suf);

        _UxParent = XtVaCreatePopupShell( "AddFiletype_shell",
                        xmDialogShellWidgetClass, _UxParent,
                        XmNx, 422,
                        XmNy, 148,
                        XmNshellUnitType, XmPIXELS,
                        XmNtitle, title,
                        NULL );
        /*
        XtAddCallback(_UxParent, XmNpopupCallback, LayoutCB_AddFiletype, NULL);
        */

        if (bLowRes) {
           AF_BigForm = XtVaCreateWidget( "AF_BigForm",
                           xmFormWidgetClass,
                           _UxParent,
                           XmNunitType, XmPIXELS,
                           XmNdialogStyle, XmDIALOG_APPLICATION_MODAL,
                           RES_CONVERT( XmNdialogTitle, title ),
                           XmNautoUnmanage, FALSE,
                           NULL );
           AF_ScrolledWindow = XtVaCreateManagedWidget( "AF_ScrolledWindow",
                           xmScrolledWindowWidgetClass,
                           AF_BigForm,
                           XmNscrollingPolicy, XmAUTOMATIC,
                           XmNleftAttachment, XmATTACH_FORM,
                           XmNrightAttachment, XmATTACH_FORM,
                           XmNbottomAttachment, XmATTACH_FORM,
                           XmNtopAttachment, XmATTACH_FORM,
                           NULL );
           XtVaSetValues(AF_BigForm, XmNuserData, AF_ScrolledWindow, NULL);

           AddFiletype = XtVaCreateManagedWidget( "AddFiletype",
                           xmFormWidgetClass,
                           AF_ScrolledWindow,
                           NULL );
        } else {
           AddFiletype = XtVaCreateWidget( "AddFiletype",
                           xmFormWidgetClass,
                           _UxParent,
                           XmNunitType, XmPIXELS,
                           XmNdialogStyle, XmDIALOG_APPLICATION_MODAL,
                           RES_CONVERT( XmNdialogTitle, title ),
                           XmNautoUnmanage, FALSE,
                           NULL );
        }

	XtFree(title);

        XtAddCallback( AddFiletype, XmNhelpCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_ADDFILETYPE );

        /* Creation of AF_FiletypeFamilyNameLabel */
        AF_FiletypeFamilyNameLabel = XtVaCreateManagedWidget( "AF_FiletypeFamilyNameLabel",
                        xmLabelWidgetClass,
                        AddFiletype,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 14, "Name of Datatype Family:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNtopOffset, 15,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_FileTypeNameTextField */
        AF_FileTypeNameTextField = XtVaCreateManagedWidget( "AF_FileTypeNameTextField",
                        xmTextFieldWidgetClass,
                        AddFiletype,
                        XmNtopOffset, 0,
                        XmNtopWidget, AF_FiletypeFamilyNameLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        ActionAreaForm = XtVaCreateManagedWidget( "ActionAreaForm",
                        xmFormWidgetClass,
                        AddFiletype,
                        XmNnavigationType, XmTAB_GROUP,
                        XmNskipAdjust, TRUE,
                        XmNfractionBase, ((TIGHTNESS * ntotalbuttons) - 1),
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 20,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_OkButton */
        AF_OkButton = XtVaCreateManagedWidget( "AF_OkButton",
                        xmPushButtonGadgetClass,
                        ActionAreaForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 10, "OK")),
                        XmNmultiClick, XmMULTICLICK_DISCARD,
                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),
                        NULL );
        nbutton++;
        XtAddCallback( AF_OkButton, XmNactivateCallback,
                (XtCallbackProc) activateCB_AF_OkButton,
                (XtPointer) NULL );

        /* Creation of AF_ApplyButton */
        AF_ApplyButton = XtVaCreateManagedWidget( "AF_ApplyButton",
                        xmPushButtonGadgetClass,
                        ActionAreaForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 11, "Apply")),
                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),
                        NULL );
        nbutton++;
        XtAddCallback( AF_ApplyButton, XmNactivateCallback,
                (XtCallbackProc) activateCB_AF_ApplyButton,
                (XtPointer) NULL );

        /* Creation of AF_CancelButton */
        AF_CancelButton = XtVaCreateManagedWidget( "AF_CancelButton",
                        xmPushButtonGadgetClass,
                        ActionAreaForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 12, "Cancel")),
                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),
                        NULL );
        nbutton++;
        XtAddCallback( AF_CancelButton, XmNactivateCallback,
                (XtCallbackProc) activateCB_AF_CancelButton,
                (XtPointer) NULL );

        /* Creation of AF_HelpButton */
        AF_HelpButton = XtVaCreateManagedWidget( "AF_HelpButton",
                        xmPushButtonGadgetClass,
                        ActionAreaForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 13, "Help")),
                        XmNuserData, AddFiletype,
                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),
                        NULL );
        nbutton++;
        XtAddCallback( AF_HelpButton, XmNactivateCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_ADDFILETYPE );

        /* Creation of AF_IdCharacteristicsLabel */
        AF_IdCharacteristicsLabel = XtVaCreateManagedWidget( "AF_IdCharacteristicsLabel",
                        xmLabelWidgetClass,
                        AddFiletype,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 15, "Identifying Characteristics:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNtopOffset, 10,
                        XmNtopWidget, AF_FileTypeNameTextField,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_IdCharacteristicsText */
        AF_IdCharacteristicsText = XtVaCreateManagedWidget( "AF_IdCharacteristicsText",
                        xmTextWidgetClass,
                        AddFiletype,
                        XmNheight, 90,
                        XmNeditMode, XmMULTI_LINE_EDIT ,
                        XmNeditable, FALSE,
                        XmNrows, 3,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 0,
                        XmNtopWidget, AF_IdCharacteristicsLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        NULL );
#ifdef DEBUG
        printf("AF_IdCharacteristicsText = %p\n", AF_IdCharacteristicsText);
#endif

        /* Creation of AF_IdCharacteristicsEdit */
        AF_IdCharacteristicsEdit = XtVaCreateManagedWidget( "AF_IdCharacteristicsEdit",
                        xmPushButtonGadgetClass,
                        AddFiletype,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 16, "Edit...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNmultiClick, XmMULTICLICK_DISCARD,
                        XmNnavigationType, XmTAB_GROUP,
                        XmNtraversalOn, TRUE,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomWidget, AF_IdCharacteristicsText,
                        XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                        NULL );
        XtAddCallback( AF_IdCharacteristicsEdit, XmNactivateCallback,
                (XtCallbackProc) activateCB_edit_id_characteristics,
                (XtPointer) NULL );
        XtRealizeWidget(AF_IdCharacteristicsEdit);
        XtVaGetValues(AF_IdCharacteristicsEdit, XmNwidth, &dim1, NULL);

        XtVaSetValues(AF_IdCharacteristicsText,
                      XmNrightOffset, dim1 + 25,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);

        /* Creation of AF_FiletypeIconHelpLabel */
        AF_FiletypeIconHelpLabel = XtVaCreateManagedWidget( "AF_FiletypeIconHelpLabel",
                      xmLabelWidgetClass,
                      AddFiletype,
                      RES_CONVERT( XmNlabelString, GETMESSAGE(7, 17, "Help Text for this Datatype Icon:")),
                      XmNalignment, XmALIGNMENT_BEGINNING,
                      XmNleftOffset, 10,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightOffset, 10,
                      XmNrightAttachment, XmATTACH_FORM,
                      XmNtopOffset, 10,
                      XmNtopWidget, AF_IdCharacteristicsText,
                      XmNtopAttachment, XmATTACH_WIDGET,
                      NULL );

        /*
        XtVaSetValues(AF_IdCharacteristicsText,
                      XmNrightWidget, AF_IdCharacteristicsEdit,
                      XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
                      NULL);
        */

        /* Creation of AF_FiletypePrintCmdTextField */
        AF_FiletypePrintCmdTextField = XtVaCreateManagedWidget( "AF_FiletypePrintCmdTextField",
                        xmTextFieldWidgetClass,
                        AddFiletype,
                        XmNbottomOffset, 15,
                        XmNbottomWidget, AF_OkButton,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_FiletypePrintCmdLabel */
        AF_FiletypePrintCmdLabel = XtVaCreateManagedWidget( "AF_FiletypePrintCmdLabel",
                        xmLabelWidgetClass,
                        AddFiletype,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 23, "Command to Print this Datatype:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNbottomOffset, 0,
                        XmNbottomWidget, AF_FiletypePrintCmdTextField,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_OpenCmdText */
        AF_OpenCmdText = XtVaCreateManagedWidget( "AF_OpenCmdText",
                        xmTextWidgetClass,
                        AddFiletype,
                        XmNeditable, FALSE,
                        XmNbottomOffset, 15,
                        XmNbottomWidget, AF_FiletypePrintCmdLabel,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNrows, 1,
                        XmNeditMode, XmMULTI_LINE_EDIT ,
                        NULL );

        /* Creation of AF_OpenFiletypeCmdLabel */
        AF_OpenFiletypeCmdLabel = XtVaCreateManagedWidget( "AF_OpenFiletypeCmdLabel",
                        xmLabelWidgetClass,
                        AddFiletype,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 22, "Command to Open this Datatype:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNbottomOffset, 0,
                        XmNbottomWidget, AF_OpenCmdText,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_CommandsLabel */
        AF_CommandsLabel = XtVaCreateManagedWidget( "AF_CommandsLabel",
                        xmLabelWidgetClass,
                        AddFiletype,
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 21, "Commands")),
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 15,
                        XmNbottomWidget, AF_OpenFiletypeCmdLabel,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        NULL );

        /* Creation of AF_FiletypeIconForm */
        AF_FiletypeIconForm = XtVaCreateManagedWidget( "AF_FiletypeIconForm",
                        xmFormWidgetClass,
                        AddFiletype,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNborderWidth, 0,
                        XmNbottomOffset, 20,
                        XmNbottomWidget, AF_CommandsLabel,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNshadowType, XmSHADOW_ETCHED_IN,
                        XmNshadowThickness, 3,
                        XmNheight, 90,
                        NULL );

        AF_MED_IconForm = XtVaCreateManagedWidget( "AF_MED_IconForm",
                        xmFormWidgetClass,
                        AF_FiletypeIconForm,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNborderWidth, ICON_BORDER_WIDTH,
                        XmNleftPosition, 16,
                        XmNleftOffset, 0,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 88,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_POSITION,
                        NULL );

        /* Creation of AF_MED_IconGadget */
	emptyString = XmStringCreateLocalized("");

        pIconData = (IconData *)XtMalloc(sizeof(IconData));
        memset(pIconData, 0, sizeof(IconData));
        IconDataList[AF_MED] = pIconData;
        AF_MED_IconGadget = XtVaCreateManagedWidget( "AF_MED_IconGadget",
                        dtIconGadgetClass,
                        AF_MED_IconForm,
                        XmNalignment, XmALIGNMENT_CENTER,
                        XmNuserData, pIconData,
                        XmNrecomputeSize, TRUE,
                        XmNstring, emptyString,
                        XmNleftOffset, BORDER_OFFSET,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, BORDER_OFFSET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, BORDER_OFFSET,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNbottomOffset, BORDER_OFFSET,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );
        pIconData->wid = AF_MED_IconGadget;
        pIconData->size = Medium_Icon;
        XtAddCallback( AF_MED_IconGadget, XmNcallback,
                (XtCallbackProc) activateCB_filetype_icon,
                (XtPointer) NULL );
        createCB_IconGadget(AF_MED_IconGadget, FALSE, Medium_Icon);
        last_filetype_pushed = AF_MED_IconGadget;

        AF_TINY_IconForm = XtVaCreateManagedWidget( "AF_TINY_IconForm",
                        xmFormWidgetClass,
                        AF_FiletypeIconForm,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNleftPosition, 41,
                        XmNleftOffset, -1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 86,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_POSITION,
                        NULL );

        /* Creation of AF_TINY_IconGadget */
        pIconData = (IconData *)XtMalloc(sizeof(IconData));
        memset(pIconData, 0, sizeof(IconData));
        IconDataList[AF_TINY] = pIconData;
        AF_TINY_IconGadget = XtVaCreateManagedWidget( "AF_TINY_IconGadget",
                        dtIconGadgetClass,
                        AF_TINY_IconForm,
                        XmNalignment, XmALIGNMENT_CENTER,
                        XmNuserData, pIconData,
                        XmNrecomputeSize, TRUE,
                        XmNstring, emptyString,
                        XmNleftOffset, BORDER_OFFSET,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, BORDER_OFFSET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, BORDER_OFFSET,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNbottomOffset, BORDER_OFFSET,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );
        pIconData->wid = AF_TINY_IconGadget;
        pIconData->size = Tiny_Icon;
        XtAddCallback( AF_TINY_IconGadget, XmNcallback,
                (XtCallbackProc) activateCB_filetype_icon,
                (XtPointer) NULL );
        createCB_IconGadget(AF_TINY_IconGadget, FALSE, Tiny_Icon);

	XmStringFree(emptyString);

        /* Creation of AF_FiletypeIconsEditButton */
        AF_FiletypeIconsEditButton = XtVaCreateManagedWidget( "AF_FiletypeIconsEditButton",
                        xmPushButtonGadgetClass,
                        AF_FiletypeIconForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 20, "Edit Icon...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNmultiClick, XmMULTICLICK_DISCARD,
                        /*XmNnavigationType, XmTAB_GROUP,*/
                        XmNleftPosition, 64,
                        XmNleftOffset, -1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        NULL );
        XtAddCallback( AF_FiletypeIconsEditButton, XmNactivateCallback,
                (XtCallbackProc) activateCB_edit_icon,
                (XtPointer) CA_FILETYPE_ICONS );

        /* Creation of AF_FiletypeIconFindSetButton */
        AF_FiletypeIconFindSetButton = XtVaCreateManagedWidget( "AF_FiletypeIconFindSetButton",
                        xmPushButtonGadgetClass,
                        AF_FiletypeIconForm,
                        XmNx, 290,
                        XmNy, 20,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 19, "Find Set...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        /*XmNnavigationType, XmTAB_GROUP,*/
                        XmNmultiClick, XmMULTICLICK_DISCARD,
                        XmNleftPosition, 64,
                        XmNleftOffset, -1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        NULL );
        XtAddCallback( AF_FiletypeIconFindSetButton, XmNactivateCallback,
                (XtCallbackProc) activateCB_open_FindSet,
                (XtPointer) CA_FILETYPE_ICONS );

        /******************************************************************/
        /* Determine which button is wider, attach the short one onto it  */
        /******************************************************************/
        XtVaGetValues(AF_FiletypeIconsEditButton, XmNwidth, &dim1, NULL);
        XtVaGetValues(AF_FiletypeIconFindSetButton, XmNwidth, &dim2, NULL);
        if (dim1 > dim2) {
           XtVaSetValues(AF_FiletypeIconFindSetButton,
                         XmNwidth, dim1,
                         NULL);
        } else {
           XtVaSetValues(AF_FiletypeIconsEditButton,
                         XmNwidth, dim2,
                         NULL);
        }
        /******************************************************************/
        /* Determine vertical positioning for findset and edit buttons    */
        /******************************************************************/
        /*XtRealizeWidget(AF_FiletypeIconForm);*/
        XtVaGetValues(AF_FiletypeIconForm, XmNheight, &dim3, NULL);
        XtVaGetValues(AF_FiletypeIconsEditButton, XmNheight, &dim1, NULL);
        XtVaGetValues(AF_FiletypeIconFindSetButton, XmNheight, &dim2, NULL);
#ifdef DEBUG
        printf("find set size = %d\n", dim2);
        printf("edit size = %d\n", dim1);
        printf("icon area form size = %d\n", dim3);
#endif
        if ((int)(dim1 + dim2) + 3*(FINDEDIT_SPACING) > (int)dim3) {
           XtVaSetValues(AF_FiletypeIconsEditButton,
                         XmNbottomOffset, FINDEDIT_SPACING,
                         XmNbottomAttachment, XmATTACH_FORM,
                         NULL);
           XtVaSetValues(AF_FiletypeIconFindSetButton,
                         XmNbottomOffset, FINDEDIT_SPACING,
                         XmNbottomWidget, AF_FiletypeIconsEditButton,
                         XmNbottomAttachment, XmATTACH_WIDGET,
                         XmNtopOffset, FINDEDIT_SPACING,
                         XmNtopAttachment, XmATTACH_FORM,
                         NULL);
        } else {
           XtVaSetValues(AF_FiletypeIconsEditButton,
                         XmNtopPosition, 50,
                         XmNtopOffset, (int)(FINDEDIT_SPACING / 2),
                         XmNtopAttachment, XmATTACH_POSITION,
                         NULL);
           XtVaSetValues(AF_FiletypeIconFindSetButton,
                         XmNbottomPosition, 50,
                         XmNbottomOffset, (int)(FINDEDIT_SPACING / 2),
                         XmNbottomAttachment, XmATTACH_POSITION,
                         NULL);
        }

        /******************************************************************/
        /* Creation of AF_FiletypeIconsBoxLabel                           */
        /******************************************************************/
        AF_FiletypeIconsBoxLabel = XtVaCreateManagedWidget( "AF_FiletypeIconsBoxLabel",
                        xmLabelWidgetClass,
                        AddFiletype,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(7, 18, "Datatype Icons:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNbottomOffset, 0,
                        XmNbottomWidget, AF_FiletypeIconForm,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of AF_FiletypeHelpScrolledWindow */
        AF_FiletypeHelpScrolledWindow = XtVaCreateManagedWidget( "AF_FiletypeHelpScrolledWindow",
                        xmScrolledWindowWidgetClass,
                        AddFiletype,
                        XmNscrollingPolicy, XmAPPLICATION_DEFINED,
                        XmNx, 10,
                        XmNy, 226,
                        XmNvisualPolicy, XmVARIABLE,
                        XmNscrollBarDisplayPolicy, XmSTATIC,
                        XmNshadowThickness, 0,
                        XmNtopOffset, 0,
                        XmNtopWidget, AF_FiletypeIconHelpLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNbottomOffset, 10,
                        XmNbottomWidget, AF_FiletypeIconsBoxLabel,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNheight, 94,
                        NULL );

        /* Creation of AF_FiletypeHelpText */
        AF_FiletypeHelpText = XtVaCreateManagedWidget( "AF_FiletypeHelpText",
                        xmTextWidgetClass,
                        AF_FiletypeHelpScrolledWindow,
                        XmNscrollHorizontal, FALSE,
                        XmNscrollVertical, TRUE,
                        XmNeditMode, XmMULTI_LINE_EDIT ,
                        XmNwordWrap, TRUE,
                        XmNx, 19,
                        XmNy, 0,
                        XmNheight, 80,
                        NULL );

        /* Creation of AF_separator2 */
        AF_separator2 = XtVaCreateManagedWidget( "AF_separator2",
                        xmSeparatorWidgetClass,
                        AddFiletype,
                        XmNleftOffset, 0,
                        XmNleftWidget, AF_CommandsLabel,
                        XmNleftAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 10,
                        XmNbottomWidget, AF_CommandsLabel,
                        XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                        NULL );

#if 0
        /* Creation of AF_separator1 */
        AF_separator1 = XtVaCreateManagedWidget( "AF_separator1",
                        xmSeparatorWidgetClass,
                        AddFiletype,
                        XmNbottomOffset, 0,
                        XmNbottomWidget, AF_separator2,
                        XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 0,
                        XmNrightWidget, AF_CommandsLabel,
                        XmNrightAttachment, XmATTACH_WIDGET,
                        NULL );
#endif

        XtVaSetValues(AddFiletype,
                        XmNcancelButton, AF_CancelButton,
                        XmNdefaultButton, AF_OkButton,
                        XmNinitialFocus, AF_FileTypeNameTextField,
                        NULL );

        XtAddCallback( AddFiletype, XmNdestroyCallback,
                (XtCallbackProc) UxDestroyContextCB,
                (XtPointer) UxAddFiletypeContext);

        if (bLowRes) {
           return ( AF_BigForm );
        } else {
           return ( AddFiletype );
        }
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

Widget  create_AddFiletype(swidget _UxUxParent)
{
        Widget                  rtrn;
        _UxCAddFiletype         *UxContext;
        XtArgVal /* Dimension */               dimHeight;
        XtArgVal /* Dimension */               dimWidth;
        XtArgVal /* Dimension */               dimTmp;
        Widget                  widScrolledWindow;
        Widget                  widTmp;

        UxAddFiletypeContext = UxContext =
                (_UxCAddFiletype *) UxNewContext( sizeof(_UxCAddFiletype), False );

        UxParent = _UxUxParent;

        rtrn = _Uxbuild_AddFiletype();
        if (bLowRes) {
           XtRealizeWidget(rtrn);
           XtVaGetValues (AddFiletype,
                          XmNheight, &dimHeight,
                          XmNwidth, &dimWidth,
                          NULL);
           XtVaGetValues (rtrn,
                          XmNuserData, &widScrolledWindow,
                          NULL);
           XtVaGetValues (widScrolledWindow,
                          XmNverticalScrollBar, &widTmp,
                          NULL);
           XtVaGetValues (widTmp,
                          XmNwidth, &dimTmp,
                          NULL);
           dimHeight += dimTmp;
           dimWidth += dimTmp;

           XtVaSetValues (rtrn,
                          XmNheight, dimHeight,
                          XmNwidth, dimWidth,
                          NULL);
        }
        return(rtrn);
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

