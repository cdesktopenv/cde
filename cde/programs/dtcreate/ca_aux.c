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
/* $TOG: ca_aux.c /main/10 1999/09/17 15:44:00 mgreess $ */
/******************************************************************************/
/*                                                                            */
/* ca_aux - auxiliary functions for create action                             */
/*                                                                            */
/* Functions to get/set values from/to interface.                             */
/* Callback routines for CA interface.                                        */
/*                                                                            */
/******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <errno.h>
#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/RowColumnP.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>
#include <sys/stat.h>
#if defined(AIXV3)
#    include <sys/dir.h>
#else
#  if defined(__osf__) || defined(linux)
#    include <dirent.h>
#  else
#    include <sys/dirent.h>
#  endif
#endif

#include <Dt/Icon.h>
#include <Dt/IconFile.h>

#include "dtcreate.h"
#include "af_aux.h"
#include "ca_aux.h"
#include "cmncbs.h"
#include "cmnrtns.h"
#include "cmnutils.h"
#include "CreateActionAppShell.h"
#include "Confirmed.h"
#include "ErrorDialog.h"
#include "FileCharacteristics.h"

/* from main.c */
extern void RemoveTmpIconFiles( void );

/******************************************************************************/
/*                                                                            */
/*  External Variables                                                        */
/*                                                                            */
/******************************************************************************/
extern XtArgVal /* Dimension */ wintypeheight, winheight, xprtheight, newheight;

/******************************************************************************/
/*                                                                            */
/*  Variables                                                                 */
/*                                                                            */
/******************************************************************************/
struct _ActionData *ca_local_struct;

/******************************************************************************/
/* readCAFromGUI - loads Action data structure with data read from            */
/*                 the GUI interface.                                         */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void readCAFromGUI (struct _ActionData *ca_struct)
{
  getCAactionName (ca_struct);
  getCAdblClkCmd (ca_struct);
  getCAactionHelpText (ca_struct);
     /*getCAfileTypes (ca_struct);*/
  getCAactionOpensText (ca_struct);
     /*getCAwindowType (ca_struct);*/
     /*getCAactionIcons (ca_struct);*/
     /*getCAexpertOption (ca_struct);*/
  return;
}


/******************************************************************************/
/* writeCAToGUI - loads widgets with data stored in the integration           */
/*                data structure                                              */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
void writeCAToGUI (struct _ActionData *pAD)
{
  putCAactionName (pAD);
  putCAactionIcons (pAD);
  putCAdblClkCmd (pAD);
  putCAactionHelpText (pAD);
  putCAwindowType (pAD);
  putCAactionOpensText (pAD);
  putCAfileTypes (pAD);
  putCAdropFiletypes(pAD);
  return;
}


/******************************************************************************/
/* getCAactionName - retrieves the action name text string and stores         */
/*      the value in the ActionData structure                                 */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void getCAactionName (struct _ActionData *ca_struct)
{
  GetWidgetTextString(CA_ActionNameTextField, &(AD.pszName));
}


/******************************************************************************/
/* getCAdblClkCmd - retrieves the double-click command text                   */
/*      string and stores the value in the ActionData structure               */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void getCAdblClkCmd (struct _ActionData *ca_struct)
{
  GetWidgetTextString(CA_DblClkText, &(AD.pszCmd));
}


/******************************************************************************/
/* getCAactionHelpText - retrieves the help text string and stores            */
/*      the value in the ActionData structure                                 */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void getCAactionHelpText (struct _ActionData *ca_struct)
{
  GetWidgetTextString(CA_HelpText, &(AD.pszHelp));
}

/******************************************************************************/
/* getCAactionOpensText - retrieves the "action opens" text                   */
/*      string and stores the value in the ActionData structure               */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void getCAactionOpensText (struct _ActionData *ca_struct)
{
  GetWidgetTextString(CA_XprtActionOpenText, &(AD.pszPrompt));
}

/******************************************************************************/
/* putCAactionName - sets the action name text field in the GUI               */
/*       from the value in the ActionData structure                           */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAactionName (struct _ActionData *ca_struct)
{
  PutWidgetTextString(CA_ActionNameTextField, CA_ActionName(ca_struct));
  return;
}

/******************************************************************************/
/* putCAdblClkCmd - sets the double-click command text                        */
/*      string in the GUI from the value in the ActionData structure          */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAdblClkCmd (struct _ActionData *ca_struct)
{
  PutWidgetTextString(CA_DblClkText, CA_DblClkAction(ca_struct));
  return;
}

/******************************************************************************/
/* putCAactionHelpText - sets the help text string in the GUI from            */
/*      the value in the ActionData structure                                 */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAactionHelpText (struct _ActionData *ca_struct)
{
  PutWidgetTextString(CA_HelpText, CA_ActionHelpText(ca_struct));
  return;
}

/******************************************************************************/
/* putCAfileTypes - sets the list of file types in the GUI from               */
/*      the values in the ActionData structure                                */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAfileTypes (struct _ActionData *pAD)
{
 pFiletypeData  pFtD;
 int            i;

  if (pAD->cFiletypes) {
     for (i=0; i < pAD->cFiletypes; i++) {
       if (!XmListItemExists(CA_FiletypesList,
              XmStringCreateSimple((pAD->papFiletypes[i])->pszName))) {
          XmListAddItem(CA_FiletypesList,
            XmStringCreateSimple((pAD->papFiletypes[i])->pszName), i+1);
       }
     }
  }

  return;
}

/******************************************************************************/
/* putCAactionOpensText - sets the "action opens" text string in              */
/*      the GUE from the value in the ActionData structure                    */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAactionOpensText (struct _ActionData *ca_struct)
{
  PutWidgetTextString(CA_XprtActionOpenText, CA_ActionOpensText(ca_struct));
  return;
}

/******************************************************************************/
/* putCAwindowType - sets the window type toggle status in the GUI            */
/*       from the value in the ActionData structure                           */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAwindowType (struct _ActionData *ca_struct)
{
  if (ca_struct->fsFlags & CA_WT_XWINDOWS) {
     /*
     XtVaSetValues(CA_WindowTypeArea,XmNmenuHistory,CA_WinTypeNoOutput,NULL);
     */
     XtVaSetValues(CA_WindowTypeArea, XmNmenuHistory, CA_WinTypeX, NULL);
  } else if (ca_struct->fsFlags & CA_WT_TERM) {
     XtVaSetValues(CA_WindowTypeArea,XmNmenuHistory,CA_WinTypeAutoClose,NULL);
  } else if (ca_struct->fsFlags & CA_WT_PERMTERM) {
     XtVaSetValues(CA_WindowTypeArea,XmNmenuHistory,CA_WinTypeManualClose,NULL);
  } else {
     /*XtVaSetValues(CA_WindowTypeArea,XmNmenuHistory,CA_WinTypeNoOutput,NULL);*/
     XtVaSetValues(CA_WindowTypeArea, XmNmenuHistory, CA_WinTypeX, NULL);
  }
  return;
}

/******************************************************************************/
/* putCAactionIcons -                                                         */
/*       GUI from the value in the ActionData structure                       */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAactionIcons (ActionData *pAD)
{
  char   *pszIconFileName = (char *)NULL;
  char   *ptr;
  char   *msgPtr, *fmtPtr, *errPtr;

  /**************************************************************************/
  /* Since we only have the base name, we must find the icon within the     */
  /* icon path, strip off the size and type extension, then set the icon    */
  /* data and gadget with this new name - base name and path.               */
  /**************************************************************************/
  FIND_ICONGADGET_ICON(pAD->pszIcon, pszIconFileName, DtLARGE);
  if (pszIconFileName) {
     ptr = strstr(pszIconFileName, LARGE_EXT);
     if (ptr) *ptr = '\0';
     SetIconData(CA_LRG_IconGadget, pszIconFileName, Large_Icon);
     XtFree(pszIconFileName);
  } else {
     msgPtr = GETMESSAGE(5, 95,
       "Could not find the following icon file within the icon\n\
search path:");
     fmtPtr = "%s\n   %s";
     errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			strlen(pAD->pszIcon) + 1) * sizeof(char));
     sprintf(errPtr, fmtPtr, msgPtr, pAD->pszIcon);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
  }

  FIND_ICONGADGET_ICON(pAD->pszIcon, pszIconFileName, DtMEDIUM);
  if (pszIconFileName) {
     ptr = strstr(pszIconFileName, MEDIUM_EXT);
     if (ptr) *ptr = '\0';
     SetIconData(CA_MED_IconGadget, pszIconFileName, Medium_Icon);
     XtFree(pszIconFileName);
  } else {
     msgPtr = GETMESSAGE(5, 95,
       "Could not find the following icon file within the icon\n\
search path:");
     fmtPtr = "%s\n   %s";
     errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			strlen(pAD->pszIcon) + 1) * sizeof(char));
     sprintf(errPtr, fmtPtr, msgPtr, pAD->pszIcon);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
  }

  FIND_ICONGADGET_ICON(pAD->pszIcon, pszIconFileName, DtTINY);
  if (pszIconFileName) {
     ptr = strstr(pszIconFileName, TINY_EXT);
     if (ptr) *ptr = '\0';
     SetIconData(CA_TINY_IconGadget, pszIconFileName, Tiny_Icon);
     XtFree(pszIconFileName);
  } else {
     msgPtr = GETMESSAGE(5, 95,
       "Could not find the following icon file within the icon\n\
search path:");
     fmtPtr = "%s\n   %s";
     errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			strlen(pAD->pszIcon) + 1) * sizeof(char));
     sprintf(errPtr, fmtPtr, msgPtr, pAD->pszIcon);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
  }
}

/******************************************************************************/
/* putCAdropFiletypes -                                                       */
/*                                                                            */
/* INPUT: _ActionData *ca_struct - Action data structure                      */
/* OUTPUT: none                                                               */
/******************************************************************************/
void putCAdropFiletypes (ActionData *pAD)
{
  Boolean  set = FALSE;

  if (pAD->fsFlags & CA_DF_ONLYFTFILES) {
     set = TRUE;
  }
  SET_TOGGLEBUTTON(CA_FiletypesInListToggle, set);
  SET_TOGGLEBUTTON(CA_AllFiletypesToggle, !set);

  return;
}

/******************************************************************************/
/* set_ca_dialog_height - adjusts height of create action dialog to exclude   */
/*                        height used by expert option section.               */
/* INPUT: none                                                                */
/* OUTPUT: none                                                               */
/******************************************************************************/
void set_ca_dialog_height (void)
{
  Boolean showExpert = sessiondata.useSession && sessiondata.showExpert;

  /* get height of window */
  XtVaGetValues (CreateActionAppShell, XmNheight, &winheight, NULL);
  /* get height of expert option form */
  XtVaGetValues (XprtOptionForm, XmNheight, &xprtheight, NULL);
  /* calculate height of window without expert option form */
  newheight = winheight - xprtheight;

  if (!showExpert)
  {
    /* remove attachment to expert option form */
    XtVaSetValues (CA_Expand, XmNbottomAttachment, XmATTACH_NONE, NULL);
    XtUnmanageChild (XprtOptionForm);

    if (!sessiondata.useSession)
    {
      /* give window new height */
      XtVaSetValues (CreateActionAppShell, XmNheight, newheight, NULL);
    }

    XtVaSetValues (CA_Expand, XmNbottomAttachment, XmATTACH_FORM, NULL);
  }
  else
  {
    XtVaSetValues (CA_Expand, RES_CONVERT(XmNlabelString,
					  GETMESSAGE(3, 52, "Basic")),
		   NULL);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/*         CALLBACK FUNCTIONS                                                 */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*      Callbacks for Create Action 'File' menu                               */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*  initAD                                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
void
initAD( ActionData *pAD )
{
  memset(pAD, 0, sizeof(ActionData));
  pAD->fsFlags |= CA_WT_XWINDOWS;
  pAD->pszIcon = (char *)XtMalloc(strlen(ca_icon_default) + 1);
  strcpy(pAD->pszIcon, ca_icon_default);
}

/******************************************************************************/
/* FreeAndClearAD - free all fields in AD, frees filetypes and array,         */
/*                  and zero's out all of the fields of the structure.        */
/* INPUT:  none                                                               */
/* OUTPUT: none                                                               */
/******************************************************************************/
void FreeAndClearAD (ActionData *pAD)
{
  FiletypeData  *pFtD;
  int           i;

  if (pAD->pszName) XtFree(pAD->pszName);
  if (pAD->pszIcon) XtFree(pAD->pszIcon);
  if (pAD->pszCmd) XtFree(pAD->pszCmd);
  if (pAD->pszHelp) XtFree(pAD->pszHelp);
  if (pAD->pszPrompt) XtFree(pAD->pszPrompt);

  if (pAD->papFiletypes) {
     for (i=0; i < pAD->cFiletypes; i++) {
        free_Filetypedata(pAD->papFiletypes[i]);
     }
     XtFree((char *)pAD->papFiletypes);
  }
  memset(pAD, 0, sizeof(ActionData));
  return;

}

/******************************************************************************/
/* clear_CreateActionAppShell_fields                                          */
/*                                                                            */
/* INPUT:  none                                                               */
/* OUTPUT: none                                                               */
/******************************************************************************/
void clear_CreateActionAppShell_fields(void)
{
  clear_text_field (ACTION_NAME);
  clear_text_field (DBL_CLK_CMND);

  XtVaSetValues(CA_WindowTypeArea, XmNmenuHistory, CA_WinTypeX, NULL);

  clear_text_field (ACTION_OPENS_TEXT);
  clear_text (ACTION_HELP_TXT);
  delete_all_list_items (FILETYPES_FOR_ACTION);

  createCB_IconGadget(CA_LRG_IconGadget, True, Large_Icon);
  createCB_IconGadget(CA_MED_IconGadget, True, Medium_Icon);
  createCB_IconGadget(CA_TINY_IconGadget, True, Tiny_Icon);

  SET_TOGGLEBUTTON(CA_AllFiletypesToggle, TRUE);
  SET_TOGGLEBUTTON(CA_FiletypesInListToggle, FALSE);

  return;
}

/******************************************************************************/
/* activateCB_FileNew - clears the text and list widgets on the               */
/*                                create action main panel                    */
/* INPUT:  Widget wid - widget id                                             */
/*         XtPointer cdata - client data                                      */
/*         XtPointer cbs - callback structur                                  */
/* OUTPUT: none                                                               */
/******************************************************************************/
void activateCB_FileNew (Widget wid, XtPointer cdata,
                         XtPointer cbstruct)
{
  char          pszFile[MAXBUFSIZE];

  /**************************************************************************/
  /* Clear the fields in the gui and set fields to default.                 */
  /**************************************************************************/
  clear_CreateActionAppShell_fields();

  /**************************************************************************/
  /* Clear the pszFileToEdit global variable.                               */
  /**************************************************************************/
  pszFileToEdit = (char *)NULL;

  /**************************************************************************/
  /* Clear the fields within the ActionData structure and free any          */
  /* FiletypeData structures.                                               */
  /**************************************************************************/
  FreeAndClearAD(&AD);

  /**************************************************************************/
  /* Initialize fields within the ActionData structure.                     */
  /**************************************************************************/
  initAD(&AD);

  /**************************************************************************/
  /* Get a new copy of AD.                                                  */
  /**************************************************************************/
  FreeAndClearAD(pMyCopyAD);
  XtFree((char *)pMyCopyAD);
  pMyCopyAD = copyAD(&AD);

  return;
}


/******************************************************************************/
/* activateCB_FileOpen  -  Callback for the Open menu bar option that is      */
/*                         located within the File submenu.  This callback    */
/*                         pops up the File Selection dialog box.             */
/*                                                                            */
/* INPUT:  Widget wid - widget id                                             */
/*         XtPointer cdata - client data                                      */
/*         XtPointer cbs - callback structure                                 */
/*                                                                            */
/* OUTPUT: none                                                               */
/******************************************************************************/
void  activateCB_FileOpen(Widget wid, XtPointer cdata,
                          XtPointer cbstruct)
{
  Widget filter;

  XtVaSetValues (OpenFile, RES_CONVERT(XmNdirMask, "~/.dt/types/*.dt"),
                 RES_CONVERT(XmNdirSpec, "~/.dt/types/"),
                 RES_CONVERT(XmNdirectory, "~/.dt/types/"),
                 NULL);

  filter = XmFileSelectionBoxGetChild (OpenFile, XmDIALOG_FILTER_TEXT);
  /*XtVaSetValues (filter, XmNeditable, False, NULL);*/
  UxPopupInterface (OpenFile, no_grab);
  return;
}

/******************************************************************************/
/*                                                                            */
/* FreeResources                                                              */
/*                                                                            */
/* INPUT:  none                                                               */
/* OUTPUT: none                                                               */
/******************************************************************************/
void FreeResources ( void )
{
  /**************************************************************************/
  /* Clear the fields within the ActionData structure and free any          */
  /* FiletypeData structures.                                               */
  /**************************************************************************/
  FreeAndClearAD(&AD);

  /**************************************************************************/
  /* Free my copy of AD.                                                    */
  /**************************************************************************/
  FreeAndClearAD(pMyCopyAD);
  XtFree((char *)pMyCopyAD);

  /********************************************************************/
  /* Free temporary icon files                                        */
  /********************************************************************/
  RemoveTmpIconFiles();

  return;
}

/******************************************************************************/
/*  activateCB_FileQuit                                                       */
/* INPUT:  Widget wid - widget id                                             */
/*         XtPointer cdata - client data                                      */
/*         XtPointer cbs - callback structur                                  */
/*  OUTPUT: none                                                              */
/******************************************************************************/
void activateCB_FileQuit (Widget wid, XtPointer cdata,
                          XtPointer cbstruct)
{
  char    *msgPtr;
  char    *savePtr;
  char    *noSavePtr;
  char    *cancelPtr;

  /**************************************************************************/
  /* Get latest changes from main window.                                   */
  /**************************************************************************/
  readCAFromGUI(&AD);

  /**************************************************************************/
  /* Check if anything has changed.  If so ask if they really want to quit. */
  /**************************************************************************/
  if (compareAD(&AD, pMyCopyAD)) {
     msgPtr = GETMESSAGE(5, 150, "Your current work has not been saved.");
     msgPtr = XtNewString(msgPtr);

     /* CDExc19439 - Save GETMESSAGE results to buffers since it can */
     /* return an internal buffer which can be overwritten on each call. */
     savePtr = XtNewString(GETMESSAGE(2, 25, "Save"));
     noSavePtr = XtNewString(GETMESSAGE(5, 152, "Don't Save"));
     cancelPtr = XtNewString(GETMESSAGE(5, 154, "Cancel Close"));

     display_question_message(CreateActionAppShell, msgPtr,
			     savePtr, (XtCallbackProc)saveCB_QuestionDialog,
                             noSavePtr, nosaveCB_QuestionDialog,
                             cancelPtr, cancelCB_QuestionDialog);

     XtFree(cancelPtr);
     XtFree(noSavePtr);
     XtFree(savePtr);
     XtFree(msgPtr);
     return;
  }

  FreeResources();

  exit(0);
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*      Callbacks for Create Action 'Options' menu                            */
/*                                                                            */
/*                                                                            */
/* void activateCB_ExpertOption ();                                           */
/*                                                                            */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/* activateCB_ExpertOption - turn expert option on/off                        */
/* INPUT: Widget wid - widget id                                              */
/*        XtPointer client_data - client data                                 */
/*        XtPointer *cbs - callback data                                      */
/* OUTPUT: none                                                               */
/* Global Variables Accessed: (other than widgets)                            */
/* wintypeheight                                                              */
/******************************************************************************/
void activateCB_ExpertOption (Widget wid, XtPointer client_data,
                              XtPointer *cbs)
{
  XtArgVal /* Dimension */  ca_width;

  if (!XtIsManaged(XprtOptionForm)) {
     /**********************************************************************/
     /* Expand the window to show optional data.                           */
     /**********************************************************************/
     XtVaGetValues (CA_WindowTypeArea, XmNwidth, &ca_width, NULL);
     XtVaSetValues (CA_Expand,
                    XmNbottomAttachment, XmATTACH_NONE,
                    NULL);
     XtVaSetValues (XprtOptionForm,
                    XmNbottomAttachment, XmATTACH_FORM,
                    XmNleftAttachment, XmATTACH_FORM,
                    XmNrightAttachment, XmATTACH_FORM,
                    XmNwidth, ca_width,
                    NULL);
     XtManageChild (XprtOptionForm);
     XtVaSetValues (CA_Expand,
                    XmNbottomAttachment, XmATTACH_WIDGET,
                    XmNbottomWidget, XprtOptionForm,
                    XmNbottomOffset, 10,
                  /*XmNlabelString, GETMESSAGE(3, 52, "Basic"),*/
                    RES_CONVERT( XmNlabelString, GETMESSAGE(3, 52, "Basic")),
                    NULL);
     XtVaSetValues (CA_HelpTextWindow, XmNheight, wintypeheight, NULL);
     /*XtVaSetValues (CA_WindowTypeArea, XmNheight, wintypeheight, NULL);*/
  } else {
     /**********************************************************************/
     /* Shrink the window to hide optional data.                           */
     /**********************************************************************/
     XtVaSetValues (CA_Expand, XmNbottomAttachment, XmATTACH_NONE, NULL);
     XtUnmanageChild (XprtOptionForm);
     XtVaSetValues (CA_Expand,
                    XmNbottomAttachment, XmATTACH_FORM,
                    XmNbottomOffset, 15,
                  /*XmNlabelString, GETMESSAGE(3, 50, "Advanced"),*/
                    RES_CONVERT( XmNlabelString, GETMESSAGE(3, 50, "Advanced")),
                    NULL);
     XtVaSetValues (CA_HelpTextWindow, XmNheight, wintypeheight, NULL);
     XtVaSetValues (CA_WindowTypeArea, XmNheight, wintypeheight, NULL);
  }
  return;
}

/******************************************************************************/
/*                                                                            */
/* valueChangedCB_ColorMonoOption - turn monochrome option on/off             */
/*                                                                            */
/* INPUT: Widget wid - widget id                                              */
/*        XtPointer client_data - client data                                 */
/*        XmToggleButtonCallbackStruct *tcb - callback data                   */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void valueChangedCB_ColorMonoOption (Widget wid, XtPointer client_data,
                              XmToggleButtonCallbackStruct *tcb)
{
  int        toggle_set= tcb->set;
  IconData   *pIconData;
  Widget     widIcon;
  Widget     widIcons[3];
  int        i;

  /***************************************************************************/
  /* If set then change icons from pixmaps to bitmaps.                       */
  /***************************************************************************/
  if (toggle_set) {
     bShowPixmaps = FALSE;
  } else {
     bShowPixmaps = TRUE;
  }

  /***************************************************************************/
  /* Change icons for CA large, med, and tiny icons                          */
  /***************************************************************************/
  widIcons[0] = CA_LRG_IconGadget;
  widIcons[1] = CA_MED_IconGadget;
  widIcons[2] = CA_TINY_IconGadget;
  for (i = 0; i < 3; i++) {
     pIconData = GetIconDataFromWid(widIcons[i]);
     if (pIconData) {
        if (bShowPixmaps) {
           SET_ICONGADGET_ICON(widIcons[i], pIconData->pmFileName);
        } else {
           SET_ICONGADGET_ICON(widIcons[i], pIconData->bmFileName);
        }
     }
  }

}

/******************************************************************************/
/*                                                                            */
/* createCB_ColorMonoOption                                                   */
/*                                                                            */
/* INPUT: Widget wid - widget id                                              */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void createCB_ColorMonoOption (Widget wid)
{
  bShowPixmaps = TRUE;
#ifdef DEBUG
  printf("bShowPixmaps = %d\n", (int)bShowPixmaps);
#endif
  return;
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*      Callbacks for Create Action 'Help' menu                               */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*      Callbacks for Create Action Panel Buttons                             */
/*                                                                            */
/*                                                                            */
/* void activateCB_action_icon ();                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* activateCB_action_icon -                                                  */
/*                                                                            */
/* INPUT: Widget wid - id of icon gadget                                      */
/*        XtPointer client_data - not used                                    */
/*        XmPushButtonCallbackStruct *cbs - not used                          */
/* OUTPUT: none                                                               */
/*                                                                            */
/******************************************************************************/
void activateCB_action_icon (Widget wid, XtPointer client_data,
                             DtIconCallbackStruct *cbs)
{
  Time    lts, mct = 0;
  static  Time prev_lts = 0;

  if (cbs->reason == XmCR_ACTIVATE) {

#ifdef DEBUG
    printf("In activate_action_icon callback.\n");
#endif
    if (last_action_pushed && (last_action_pushed != wid)) {
       XtVaSetValues(XtParent(last_action_pushed), XmNborderWidth, 0, NULL);
    }
    XtVaSetValues(XtParent(wid), XmNborderWidth, ICON_BORDER_WIDTH, NULL);

    last_action_pushed = wid;

    /********************************************************************/
    /* This is the support needed to provide double-click functionality */
    /* to the icon gadgets.  When double-clicked, the icon editor will  */
    /* be launched.                                                     */
    /********************************************************************/
    XtVaSetValues(wid, XmNpushButtonClickTime, 0, NULL);

    lts = XtLastTimestampProcessed(XtDisplay(wid));
    mct = XtGetMultiClickTime(XtDisplay(wid));

    if ((prev_lts + mct) > lts) {
       prev_lts = XtLastTimestampProcessed(XtDisplay(wid));
    } else {
       prev_lts = XtLastTimestampProcessed(XtDisplay(wid));
       return;
    }

#ifdef DEBUG
    printf("DblClick icon callback.\n");
#endif

    activateCB_edit_icon(wid, (XtPointer)CA_ACTION_ICONS,
		(XmPushButtonCallbackStruct *)cbs /* unused anyway */);

  }
  return;
}

/******************************************************************************/
/* activateCB_add_filetype - bring up the Add Filetype dialog with correct    */
/*              dialog title.                                                 */
/* INPUT: Widget wid - not used                                               */
/*        XtPointer client_data - not used                                    */
/*        XmPushButtonCallbackStruct *cbs - not used                          */
/* OUTPUT: none                                                               */
/******************************************************************************/
void activateCB_add_filetype (Widget wid, XtPointer client_data,
                              XmPushButtonCallbackStruct *cbs)
{
  FiletypeData *pFtD;
  char         *ptr;
  char          tmpbuf[50];
  char	       *pre, *suf, *title;
  size_t       len;

  if (!CreateActionAppShellCheckFields()) {

#ifdef DEBUG
    printf("Allocating FiletypeData structure\n"); /* debug */
#endif

    /******************************************************************/
    /* Allocate FiletypeData structure                                */
    /******************************************************************/
    pFtD = (FiletypeData *)XtMalloc(sizeof(FiletypeData));
    memset(pFtD, 0, sizeof(FiletypeData));

    /******************************************************************/
    /* Clear dialog fields                                            */
    /******************************************************************/
    clear_AddFiletype_dialog_fields();
    clear_FileCharacteristics_dialog_fields();

    /******************************************************************/
    /* Create name for filetype                                       */
    /******************************************************************/
    memset(tmpbuf, 0, 50);
    sprintf(tmpbuf, "%d", (AD.cFiletypes + 1));
    ptr = (char *)XmTextFieldGetString(CA_ActionNameTextField);
    pFtD->pszName = (char *)XtMalloc(strlen(ptr) + strlen("_FILE_") + strlen(tmpbuf) + 1);
    sprintf(pFtD->pszName, "%s_FILE_%d", ptr, (AD.cFiletypes + 1));
    XtFree(ptr);
    XmTextFieldSetString(AF_FileTypeNameTextField, pFtD->pszName);

    pFtD->pszIcon = (char *)XtMalloc(strlen(af_icon_default) + 1);
    strcpy(pFtD->pszIcon, af_icon_default);

    /******************************************************************/
    /* Set Open command string                                        */
    /******************************************************************/
    /*
    pFtD->pszOpenCmd = (char *)XmTextGetString(CA_DblClkText);
    */
    GetWidgetTextString(CA_DblClkText, &(pFtD->pszOpenCmd));
    XmTextSetString(AF_OpenCmdText, pFtD->pszOpenCmd);

    pre = GETMESSAGE(3, 10, "Create Action");
    suf = GETMESSAGE(7, 10, "Add Datatype");
    len = strlen(pre) + strlen(suf) + 4;
    title = XtMalloc(len);
    snprintf(title,len - 1,"%s - %s", pre, suf);

    XtVaSetValues (AddFiletype,
                   RES_CONVERT (XmNdialogTitle, title ),
                   XmNuserData, pFtD,
                   NULL);

    XtFree(title);
    UxPopupInterface (AddFiletype, no_grab);
  }
  return;
}


/******************************************************************************/
/* activateCB_edit_filetype - bring up the Add Filetype dialog with correct   */
/*              dialog title.                                                 */
/* INPUT: Widget wid - not used                                               */
/*        XtPointer client_data - not used                                    */
/*        XmPushButtonCallbackStruct *cbs - not used                          */
/* OUTPUT: none                                                               */
/******************************************************************************/
void activateCB_edit_filetype (Widget wid, XtPointer client_data,
                              XmPushButtonCallbackStruct *cbs)
{
  FiletypeData *pFtD;
  int          *poslist, poscnt;
  int          selecteditem;
  char         *msgPtr1, *msgPtr2, *fmtPtr, *errPtr;
  char	       *pre, *suf, *title;
  size_t len;

  pre = GETMESSAGE(3, 10, "Create Action");
  suf = GETMESSAGE(7, 11, "Edit Datatype");
  len = strlen(pre) + strlen(suf) + 4;
  title = XtMalloc(len);
  snprintf(title,len - 1, "%s - %s", pre, suf);

  /**************************************************************************/
  /* Determine the selected list item.                                      */
  /**************************************************************************/
  if (XmListGetSelectedPos(CA_FiletypesList, &poslist, &poscnt)) {
     selecteditem = *poslist;
     selecteditem--;
     if (selecteditem >= 0) {
        pFtD = AD.papFiletypes[selecteditem];
        pFtD->fsInternal |= CA_FT_EDIT;
        GetWidgetTextString(CA_DblClkText, &(pFtD->pszOpenCmd));
        clear_AddFiletype_dialog_fields();
        init_AddFiletype_dialog_fields(pFtD);

        /********************************************************************/
        /* Pop up the AddFiletype dialog.                                   */
        /********************************************************************/
        XtVaSetValues (AddFiletype,
                       RES_CONVERT (XmNdialogTitle, title),
                       XmNuserData, pFtD,
                       NULL);
        UxPopupInterface (AddFiletype, no_grab);
     }
  } else {
     msgPtr1 = XtNewString(GETMESSAGE(5, 120,
		  "You have not selected a Datatype from the\n\
'Datatypes That Use This Action' list."));
     msgPtr2 = XtNewString(GETMESSAGE(5, 130,
		  "Please select the Datatype you would like to Edit."));
     fmtPtr = "%s\n%s";
     len = (strlen(msgPtr1) + strlen(msgPtr2) +
             strlen(fmtPtr) + 3);
     errPtr = XtMalloc(len);
     snprintf(errPtr,len - 1, fmtPtr, msgPtr1, msgPtr2);
     XtFree(msgPtr2);
     XtFree(msgPtr1);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
  }
  XtFree(title);
  return;
}

/******************************************************************************/
/*                                                                            */
/* get_selected_action_icon - determines which action icon is currently       */
/*                            selected.                                       */
/*                                                                            */
/* INPUT: none                                                                */
/* OUTPUT: Widget wid - id of selected icon gadget                            */
/*                                                                            */
/******************************************************************************/
Widget get_selected_action_icon (void)
{
#if 0
  Widget wid;

  if (_DtIconGetState(CA_LRG_IconGadget)) wid = CA_LRG_IconGadget;
  else if (_DtIconGetState(CA_MED_IconGadget)) wid = CA_MED_IconGadget;
  else if (_DtIconGetState(CA_TINY_IconGadget)) wid = CA_TINY_IconGadget;
  else wid = CA_LRG_IconGadget;
#endif

  if (!last_action_pushed) {
    last_action_pushed = CA_LRG_IconGadget;
  }
  return (last_action_pushed);
}

/******************************************************************************/
/*                                                                            */
/* CreateActionAppShellCheckFields                                            */
/*                                                                            */
/* INPUT:  none                                                               */
/* OUTPUT: FALSE  - no errors found                                           */
/*         TRUE   - found errors                                              */
/*                                                                            */
/******************************************************************************/
Boolean CreateActionAppShellCheckFields(void)
{

  char    *ptr = (char *)NULL;
  char    *ptrPrompt = (char *)NULL;
  Boolean bError = FALSE;
  char    *msgPtr, *errPtr;

  /**************************************************************************/
  /* Check if action name is present.                                       */
  /**************************************************************************/
  if (!bError) {
     ptr = (char *)NULL;
     GetWidgetTextString(CA_ActionNameTextField, &ptr);
#ifdef DEBUG
     printf("Action Name = '%s'\n", ptr);
#endif
     if (!ptr) {
	msgPtr = GETMESSAGE(5, 10, "The Action Name is missing.\n\
Please enter a name in the 'Action Name' field.");
	errPtr = XtNewString(msgPtr);
        display_error_message(CreateActionAppShell, errPtr);
	XtFree(errPtr);
        XmProcessTraversal(CA_ActionNameTextField, XmTRAVERSE_CURRENT);
        bError = TRUE;
     } else {
        XtFree(ptr);
     }
  }

  /**************************************************************************/
  /* Check if execute command is present.                                   */
  /**************************************************************************/
  if (!bError) {
     ptr = (char *)NULL;
     GetWidgetTextString(CA_DblClkText, &ptr);
#ifdef DEBUG
     printf("Exec String = '%s'\n", ptr);
#endif
     if (!ptr) {
	msgPtr = GETMESSAGE(5, 15, "The execute command is missing.\n\
Please enter a command in the\n\
'Command To Execute When Double-clicked' field.");
	errPtr = XtNewString(msgPtr);
        display_error_message(CreateActionAppShell, errPtr);
	XtFree(errPtr);
        XmProcessTraversal(CA_DblClkText, XmTRAVERSE_CURRENT);
        bError = TRUE;
     } else {
        /********************************************************************/
        /* Now check to see if there is a prompt string.  If so, make sure  */
        /* there is an argument variable within the exec string.            */
        /********************************************************************/
        GetWidgetTextString(CA_XprtActionOpenText, &ptrPrompt);
        if (ptrPrompt) {
           if (!strchr(ptr, '$')) {
              msgPtr = GETMESSAGE(5, 30, "A variable name is missing in the\n\
'Command To Execute When Double-clicked' field.\n\
Add a variable name for the label you have entered\n\
in the 'When Action Opens, Ask Users for' field.\n\
Valid variable names are $*, $1, $2, ..., $9.");
	      errPtr = XtNewString(msgPtr);
              display_error_message(CreateActionAppShell, errPtr);
	      XtFree(errPtr);
              XmProcessTraversal(CA_DblClkText, XmTRAVERSE_CURRENT);
              bError = TRUE;
           }
           XtFree(ptrPrompt);
        }
        XtFree(ptr);
     }
  }

  /**************************************************************************/
  /* Check if droppable filetypes buttons are set correctly.                */
  /**************************************************************************/
  if (!bError) {
     if ((AD.fsFlags & CA_DF_ONLYFTFILES) && !(AD.papFiletypes)) {
	msgPtr = GETMESSAGE(5, 20,
		"The 'Datatypes That Use This Action' list is empty.\n\
You have selected 'Only Above List' in the 'Dropable\n\
Datatypes' field.\n\
Change the Dropable Datatype choice to 'All Datatypes'\n\
or add at least one datatype to the list.");
	errPtr = XtNewString(msgPtr);
        display_error_message(CreateActionAppShell, errPtr);
	XtFree(errPtr);
        XmProcessTraversal(CA_AllFiletypesToggle, XmTRAVERSE_CURRENT);
        bError = TRUE;
     }
  }

  return(bError);
}

/******************************************************************************/
/*                                                                            */
/* copyString                                                                 */
/*                                                                            */
/* INPUT:                                                                     */
/* OUTPUT:                                                                    */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
char * copyString(char *string)
{
  char *newstring;

  if (string) {
     newstring = (char *)XtMalloc(strlen(string) + 1);
     strcpy(newstring, string);
  } else {
     newstring = (char *)NULL;
  }
  return(newstring);
}

/******************************************************************************/
/*                                                                            */
/* copyFtD                                                                    */
/*                                                                            */
/* INPUT:                                                                     */
/* OUTPUT:                                                                    */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
FiletypeData * copyFtD(FiletypeData *pFtD)
{
  FiletypeData *pcopyFtD;

  pcopyFtD = (FiletypeData *)XtMalloc(sizeof(FiletypeData));
  memset(pcopyFtD, 0, sizeof(FiletypeData));

  if (pFtD->pszName) {
     pcopyFtD->pszName = copyString(pFtD->pszName);
  }
  if (pFtD->pszIcon) {
     pcopyFtD->pszIcon = copyString(pFtD->pszIcon);
  }
  if (pFtD->pszHelp) {
     pcopyFtD->pszHelp = copyString(pFtD->pszHelp);
  }
  if (pFtD->pszOpenCmd) {
     pcopyFtD->pszOpenCmd = copyString(pFtD->pszOpenCmd);
  }
  if (pFtD->pszPrintCmd) {
     pcopyFtD->pszPrintCmd = copyString(pFtD->pszPrintCmd);
  }
  if (pFtD->pszPattern) {
     pcopyFtD->pszPattern = copyString(pFtD->pszPattern);
  }
  if (pFtD->pszPermissions) {
     pcopyFtD->pszPermissions = copyString(pFtD->pszPermissions);
  }
  if (pFtD->pszContents) {
     pcopyFtD->pszContents = copyString(pFtD->pszContents);
  }

  pcopyFtD->sStart = pFtD->sStart;
  pcopyFtD->sEnd = pFtD->sEnd;
  pcopyFtD->fsFlags = pFtD->fsFlags;
  pcopyFtD->fsInternal = pFtD->fsInternal;

  return(pcopyFtD);
}

/******************************************************************************/
/*                                                                            */
/* copyAD                                                                     */
/*                                                                            */
/* INPUT:                                                                     */
/* OUTPUT:                                                                    */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
ActionData * copyAD(ActionData *pAD)
{
  ActionData *pcopyAD;
  int i;

  pcopyAD = (ActionData *)XtMalloc(sizeof(ActionData));
  if (!pcopyAD) {
     return((ActionData *)NULL);
  }
  memset(pcopyAD, 0, sizeof(ActionData));

  if (pAD->pszName) {
     pcopyAD->pszName = copyString(pAD->pszName);
  }
  if (pAD->pszIcon) {
     pcopyAD->pszIcon = copyString(pAD->pszIcon);
  }
  if (pAD->pszCmd) {
     pcopyAD->pszCmd = copyString(pAD->pszCmd);
  }
  if (pAD->pszHelp) {
     pcopyAD->pszHelp = copyString(pAD->pszHelp);
  }
  if (pAD->pszPrompt) {
     pcopyAD->pszPrompt = copyString(pAD->pszPrompt);
  }
  if (pAD->papFiletypes) {
     pcopyAD->papFiletypes = (FiletypeData **) XtMalloc(sizeof(FiletypeData *) * (pAD->cFiletypes + 1));
     for (i=0; i < pAD->cFiletypes; i++) {
        pcopyAD->papFiletypes[i] = copyFtD(pAD->papFiletypes[i]);
     }
  }
  pcopyAD->cFiletypes = pAD->cFiletypes;
  pcopyAD->fsFlags = pAD->fsFlags;
  return(pcopyAD);
}
/******************************************************************************/
/*                                                                            */
/* compareStrings                                                             */
/*                                                                            */
/* INPUT:  two pointers to char *'s                                           */
/* OUTPUT: 0  they are the same                                               */
/*         1  they are different                                              */
/*                                                                            */
/******************************************************************************/
Boolean compareStrings(char *pszStr1, char *pszStr2)
{
  if (pszStr1 || pszStr2) {
     if (pszStr1) {
        if (pszStr2) {
           if (!strcmp(pszStr1, pszStr2)) {
              return(EQUAL);
           } else {
              return(NOT_EQUAL);
           }
        } else {
           return(NOT_EQUAL);
        }
     } else {
        return(NOT_EQUAL);
     }
  } else {
     return(EQUAL);
  }
}

/******************************************************************************/
/*                                                                            */
/* compareFtD                                                                 */
/*                                                                            */
/* INPUT:  two pointers to FtD structures                                     */
/* OUTPUT: 0  they are the same                                               */
/*         1  they are different                                              */
/*                                                                            */
/******************************************************************************/
Boolean compareFtD(FiletypeData *pFtD1, FiletypeData *pFtD2)
{
  if (compareStrings(pFtD1->pszName, pFtD2->pszName)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszIcon, pFtD2->pszIcon)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszHelp, pFtD2->pszHelp)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszOpenCmd, pFtD2->pszOpenCmd)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszPrintCmd, pFtD2->pszPrintCmd)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszPattern, pFtD2->pszPattern)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszPermissions, pFtD2->pszPermissions)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pFtD1->pszContents, pFtD2->pszContents)) {
     return(NOT_EQUAL);
  }
  if (pFtD1->fsFlags != pFtD2->fsFlags) {
     return(NOT_EQUAL);
  }
  if (pFtD1->sStart != pFtD2->sStart) {
     return(NOT_EQUAL);
  }
  if (pFtD1->sEnd != pFtD2->sEnd) {
     return(NOT_EQUAL);
  }

  return(EQUAL);
}

/******************************************************************************/
/*                                                                            */
/* compareAD                                                                  */
/*                                                                            */
/* INPUT:  two pointers to AD structures                                      */
/* OUTPUT: 0  they are the same                                               */
/*         1  they are different                                              */
/*                                                                            */
/******************************************************************************/
Boolean compareAD(ActionData *pAD1, ActionData *pAD2)
{
  int i;

  if (compareStrings(pAD1->pszName, pAD2->pszName)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pAD1->pszIcon, pAD2->pszIcon)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pAD1->pszCmd, pAD2->pszCmd)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pAD1->pszHelp, pAD2->pszHelp)) {
     return(NOT_EQUAL);
  }
  if (compareStrings(pAD1->pszPrompt, pAD2->pszPrompt)) {
     return(NOT_EQUAL);
  }
  if (pAD1->fsFlags != pAD2->fsFlags) {
     return(NOT_EQUAL);
  }
  if (pAD1->cFiletypes != pAD2->cFiletypes) {
     return(NOT_EQUAL);
  }

  if (pAD1->papFiletypes) {
     for (i=0; i < pAD1->cFiletypes; i++) {
        if (compareFtD(pAD1->papFiletypes[i], pAD2->papFiletypes[i])) {
           return(NOT_EQUAL);
        }
     }
  }

  return(EQUAL);
}
