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
/* $TOG: CreateActionAppShell.c /main/11 1998/10/26 17:17:50 mgreess $ */
/*******************************************************************************
        CreateActionAppShell.c

       Associated Header file: CreateActionAppShell.h
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/Protocols.h>
#include <Xm/AtomMgr.h>

#include <X11/Shell.h>
#include <X11/cursorfont.h>

#include <Dt/Icon.h>
#include <Dt/Action.h>

#include "dtcreate.h"
#include "UxXt.h"
#include "ca_aux.h"
#include "cmncbs.h"
#include "cmnrtns.h"
#include "Confirmed.h"

#define CONTEXT_MACRO_ACCESS 1
#include "CreateActionAppShell.h"
#undef CONTEXT_MACRO_ACCESS

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

Dimension winheight, xprtheight, newheight, wintypeheight;
Widget    FileTypes;

/*******************************************************************************
       The following are callback functions.
*******************************************************************************/
static char * GetDefinitionFileName( char *pszName);
static void  CreateFiles(void);
static void  questionCB_ReplaceFile(Widget UxWidget,
                             XtPointer UxClientData,
                             XtPointer UxCallbackArg);
static void  questionCB_CancelSave(Widget UxWidget,
                            XtPointer UxClientData,
                            XtPointer UxCallbackArg);

static char * GetDefinitionFileName( char *pszName)
{
  char     *pszEnvVar;
  char     *defname = (char *)NULL;
  int      bytesneeded = 0;
  char     *msgPtr, *errPtr;
  char     *newName;

  /****************************************************************/
  /* Create definition file name.                                 */
  /****************************************************************/
  newName = ReplaceSpaces(pszName);
  pszEnvVar = getenv("HOME");
  if ( pszEnvVar && strlen(pszEnvVar) ) {
     bytesneeded = strlen("/.dt/types/.dt") + strlen(pszEnvVar) + strlen(newName) + 1;
     defname = (char *)XtMalloc(bytesneeded);
     sprintf(defname, "%s/.dt/types/%s.dt", pszEnvVar, newName);
  } else {
     msgPtr = GETMESSAGE(5, 170,
	 "The HOME environment variable is not set.\n\
The HOME environment variable must be set in order\n\
for this action to be saved.");
     errPtr = XtNewString(msgPtr);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
  }
  XtFree(newName);
  return(defname);
}


static void  CreateFiles(void)
{
  Boolean  bHaveError = False;
  ushort   usRc = 0;
  char     *pszDefName;
  char     *msgPtr1, *msgPtr2, *fmtPtr, *bufPtr;
#ifdef UNDO_CMVC_3571
  char     *msgPtr3;
#endif

  /******************************************************************/
  /* Create Definition file name.                                   */
  /******************************************************************/
  pszDefName = GetDefinitionFileName(AD.pszName);
  if (!pszDefName) {
     bHaveError = True;
  }

  /******************************************************************/
  /* Create Definition file.                                        */
  /******************************************************************/
  if ( !bHaveError && ((usRc = WriteDefinitionFile(pszDefName, &AD)) != 0) ) {
     bHaveError = TRUE;
  }

  /******************************************************************/
  /* Create Action file.                                            */
  /******************************************************************/
  if ( !bHaveError && ((usRc = WriteActionFile(&AD)) != 0) ) {
     bHaveError = TRUE;
  }

  /******************************************************************/
  /* Copy icon files to home directory   ~/.dt/icons                */
  /******************************************************************/
  if ( !bHaveError && ((usRc = CopyIconFiles(&AD)) != 0) ) {
     bHaveError = TRUE;
  }

  /*******************************************************************/
  /* Reload action database and pop up confirmation message if       */
  /* operation was successful.                                       */
  /*******************************************************************/
  if (!bHaveError) {
     _DtTurnOnHourGlass(CreateActionAppShell);
     if (!DbInitDone) {
        DtInitialize(UxDisplay, UxTopLevel, pszExecName, pszExecName);
        DtDbLoad();
        DbInitDone = TRUE;
     }
     DtActionInvoke(UxTopLevel, "ReloadActions",
                      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

     _DtTurnOffHourGlass(CreateActionAppShell);

     msgPtr1 = XtNewString(GETMESSAGE(5, 70, "The new action"));
     msgPtr2 = XtNewString(GETMESSAGE(5, 71,
	      "has been placed in your home folder."));
#ifdef UNDO_CMVC_3571
     msgPtr3 = XtNewString(GETMESSAGE(5, 73,
	      "The action definition file that was created is called:"));
     fmtPtr = "%s\n\n   %s\n\n%s\n\n%s\n   %s";
#else
     fmtPtr = "%s\n\n   %s\n\n%s";
#endif

     bufPtr = XtMalloc((strlen(msgPtr1) + strlen(msgPtr2) +
			strlen(AD.pszName) +
#ifdef UNDO_CMVC_3571
			strlen(msgPtr3) + strlen(pszFile) +
#endif
			strlen(fmtPtr) + 1) * sizeof(char));

     sprintf(bufPtr, fmtPtr, msgPtr1, AD.pszName, msgPtr2
#ifdef UNDO_CMVC_3571
	     , msgPtr3, pszFile
#endif
	     );

     display_confirmed_message(CreateActionAppShell, bufPtr);

     XtFree(bufPtr);
#ifdef UNDO_CMVC_3571
     XtFree(msgPtr3);
#endif
     XtFree(msgPtr2);
     XtFree(msgPtr1);

  }

  /*******************************************************************/
  /* Update the copy of the AD structure.  This will be used to      */
  /* determine if the user has saved the most recent data before     */
  /* exiting.                                                        */
  /*******************************************************************/
  FreeAndClearAD(pMyCopyAD);
  XtFree((char *) pMyCopyAD);
  pMyCopyAD = copyAD(&AD);

}


static void  questionCB_ReplaceFile(Widget UxWidget,
                             XtPointer UxClientData,
                             XtPointer UxCallbackArg)
{
  CreateFiles();
  XtDestroyWidget(XtParent(UxWidget));
}

static void  questionCB_CancelSave(Widget UxWidget,
                            XtPointer UxClientData,
                            XtPointer UxCallbackArg)
{
  XtDestroyWidget(XtParent(UxWidget));
}


void  activateCB_CA_MB_FileSave(Widget UxWidget,
                                XtPointer UxClientData,
                                XtPointer UxCallbackArg)

{
  ushort   usRc = 0;
  char     *pszActionFile;
  char     *msgPtr, *bufPtr;
  Boolean  bHaveError = FALSE;
  struct   stat statbuf;
  char     *newName;
  char     *pszEnvVar;

  /*********************************************************************/
  /*  Check that required fields are filled in for main window.        */
  /*********************************************************************/
  if (!CreateActionAppShellCheckFields()) {

    /*******************************************************************/
    /*  Query fields from main window and save into structure.         */
    /*******************************************************************/
    readCAFromGUI(&AD);

    /*******************************************************************/
    /*  Write the files.                                               */
    /*******************************************************************/
#ifdef UNDO_CMVC_4662
    if (pszFileToEdit) {
       strcpy(pszFile, pszFileToEdit);
    } else {
#endif

       /****************************************************************/
       /* Create Action file name.                                     */
       /****************************************************************/
       newName = ReplaceSpaces(AD.pszName);
       pszActionFile = NULL;
       pszEnvVar = getenv("HOME");
       if ( pszEnvVar && strlen(pszEnvVar) ) {
	  pszActionFile = malloc(strlen(pszEnvVar)+strlen(newName)+10);
	  if (pszActionFile)
            sprintf(pszActionFile, "%s/%s", pszEnvVar, newName);
       } else {
	  msgPtr = GETMESSAGE(5, 170,
	      "The HOME environment variable is not set.\n\
The HOME environment variable must be set in order\n\
for this action to be saved.");
	  bufPtr = XtNewString(msgPtr);
          display_error_message(CreateActionAppShell, bufPtr);
	  XtFree(bufPtr);
          bHaveError = TRUE;
       }

       /****************************************************************/
       /* Check if action file exists and flag a warning if it does.   */
       /****************************************************************/
       if ( (!bHaveError) &&
	    (pszActionFile) &&
	    (!stat(pszActionFile, &statbuf)) ) {

	  /* INTERNATIONALIZE */
	  msgPtr = "A file named\n\n   %s\n\n\
already exists in your home folder.\n\
The existing file may be an application,\n\
data file, or an action file.\n\n\
What do you want to do?";
	  bufPtr = XtMalloc((strlen(msgPtr) + strlen(newName) + 1) *
			    sizeof(char));
          sprintf(bufPtr, msgPtr, newName);

	  /* INTERNATIONALIZE */
          display_question_message(CreateActionAppShell, bufPtr,
                              "Replace", questionCB_ReplaceFile,
                              "Cancel Save", questionCB_CancelSave,
                              (char *)NULL, (XtCallbackProc)NULL);
	  XtFree(bufPtr);
	  XtFree(pszActionFile);
          bHaveError = True;
       }

       /****************************************************************/
       /* Now we are done with the newName variable and can free.      */
       /****************************************************************/
       XtFree(newName);

#ifdef UNDO_CMVC_4662
    }
#endif

    if (!bHaveError) {
       CreateFiles();
    }

  }
}

static  void    activateCB_CA_FiletypesDelete( Widget UxWidget,
                                              XtPointer UxClientData,
                                              XtPointer UxCallbackArg)
{
  int           *poslist, poscnt;
  int           *p, n;
  XmString      *items;
  FiletypeData  **papArray;
  int           i, k;
  int           cnt;
  Boolean       bFound;
  char          *msgPtr1, *msgPtr2, *fmtPtr, *errPtr;
  size_t        len;

  /**************************************************************************/
  /* Determine the filetypes to delete and delete them.                     */
  /**************************************************************************/
  XtVaGetValues(FileTypes, XmNitems, &items, NULL);
  if (XmListGetSelectedPos(FileTypes, &poslist, &poscnt)) {
     n = poscnt;
     p = poslist + (n-1);
     while (n--) {
       /*********************************************************************/
       /* Delete the filetype name from the listbox.                        */
       /*********************************************************************/
       XmListDeleteItem(FileTypes, items[*p -1]);
       p--;
     }

     /***********************************************************************/
     /* Remove the filetypes from the filetype array in the ActionData      */
     /* structure.                                                          */
     /***********************************************************************/
     if ((AD.cFiletypes - poscnt) > 0) {
        if ( papArray = (FiletypeData **) XtMalloc(sizeof(FiletypeData *) * (AD.cFiletypes - poscnt)) ) {
           cnt = 0;
           for (i = 0; i < AD.cFiletypes; i++) {
              bFound = FALSE;
              for (k = 0; k < poscnt; k++) {
                if ((i + 1) == poslist[k]) {
                   bFound = TRUE;
                   break;
                }
              }
              /**************************************************************/
              /* If did not find this element in the delete list, then add  */
              /* it back to our good list.                                  */
              /* Else do not include in the good list and free its memory.  */
              /**************************************************************/
              if (!bFound) {
                 papArray[cnt] = AD.papFiletypes[i];
                 cnt++;
              } else {
                 free_Filetypedata(AD.papFiletypes[i]);
              }
           }
           if (AD.papFiletypes) {
             XtFree((XtPointer)AD.papFiletypes);
           }
           AD.papFiletypes = papArray;
           AD.cFiletypes = cnt;
        }
     } else {
        /********************************************************************/
        /* Delete the entire array.                                         */
        /********************************************************************/
        for (i = 0; i < AD.cFiletypes; i++) {
          free_Filetypedata(AD.papFiletypes[i]);
        }
        AD.papFiletypes = (FiletypeData **)NULL;
        AD.cFiletypes = 0;
     }
  } else {
     msgPtr1 = XtNewString(GETMESSAGE(5, 120,
		  "You have not selected a Datatype from the\n\
'Datatypes That Use This Action' list."));
     msgPtr2 = XtNewString(GETMESSAGE(5, 125,
		  "Please select the Datatype you would like to Delete."));
     fmtPtr = "%s\n%s";
     len = (strlen(msgPtr1) + strlen(msgPtr2) + strlen(fmtPtr) + 2);
     errPtr = XtMalloc(len);
     snprintf(errPtr,len - 1, fmtPtr, msgPtr1, msgPtr2);
     XtFree(msgPtr2);
     XtFree(msgPtr1);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
  }
}

#if 0
/*****************************************************************************/
/*                                                                           */
/* valueChangedCB_CA_WinType                                                 */
/*                                                                           */
/*****************************************************************************/
static  void    valueChangedCB_CA_WinType( Widget UxWidget,
                                           XtPointer UxClientData,
                                           XtPointer UxCallbackArg)
{
  Boolean bSetToggle;

  bSetToggle = ((XmToggleButtonCallbackStruct *)UxCallbackArg)->set;
#ifdef DEBUG
  printf("Wintype change before %X\n", AD.fsFlags);
#endif
  switch ((int)UxClientData) {
    case CA_WT_XWINDOWS :
         if (bSetToggle) AD.fsFlags |= CA_WT_XWINDOWS;
         else AD.fsFlags &= (~(CA_WT_XWINDOWS));
         break;
    case CA_WT_TERM :
         if (bSetToggle) AD.fsFlags |= CA_WT_TERM;
         else AD.fsFlags &= (~(CA_WT_TERM));
         break;
    case CA_WT_PERMTERM :
         if (bSetToggle) AD.fsFlags |= CA_WT_PERMTERM;
         else AD.fsFlags &= (~(CA_WT_PERMTERM));
         break;
  }
#ifdef DEBUG
  printf("Wintype change after %X\n", AD.fsFlags);
#endif
}
#endif

/*****************************************************************************/
/*                                                                           */
/* valueChangedCB_DroppableFiletypes                                         */
/*                                                                           */
/*****************************************************************************/
static  void    valueChangedCB_DroppableFiletypes( Widget UxWidget,
                                           XtPointer UxClientData,
                                           XtPointer UxCallbackArg)
{
  if (((XmToggleButtonCallbackStruct *)UxCallbackArg)->set) {
    AD.fsFlags |= CA_DF_ONLYFTFILES;
  } else {
    AD.fsFlags &= (~(CA_DF_ONLYFTFILES));
  }
}

/*****************************************************************************/
/*                                                                           */
/* simpleCB_WindowTypeOptionMenu                                             */
/*                                                                           */
/*****************************************************************************/
static  void    activateCB_WindowTypeOptionMenu( Widget UxWidget,
                                           XtPointer UxClientData,
                                           XtPointer UxCallbackArg)
{
  AD.fsFlags &= (~(CA_WT_XWINDOWS | CA_WT_TERM | CA_WT_PERMTERM));
  switch ((int)UxClientData) {
    case CA_WT_XWINDOWS :
         AD.fsFlags |= CA_WT_XWINDOWS;
         break;
    case CA_WT_TERM :
         AD.fsFlags |= CA_WT_TERM;
         break;
    case CA_WT_PERMTERM :
         AD.fsFlags |= CA_WT_PERMTERM;
         break;
  }
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget   _Uxbuild_CreateActionAppShell(void)
{
        Widget               _UxParent;
        Widget               CA_MB_FileOption_shell;
        Widget               CA_MB_HelpOption_shell;
        Widget               CA_MB_Help_Sep1;
        Widget               CA_MB_Help_Sep2;
        Widget               CA_MB_OptionsOption_shell;
        char                 *UxTmp0;

        Widget               CA_WindowType_OptionMenuShell;
        Widget               CA_WindowType_Pane;
        Widget               CA_WindowType_OptionMenu;
        char                 mnemonic[2];
        Dimension            dim1, dim2;
        Widget               CA_MED_IconForm, CA_TINY_IconForm, CA_LRG_IconForm;
        IconData             *pIconData;

	XmString	     emptyString;
	char		     titleBuf[MAXBUFSIZE];

#define ICON_MIN_HEIGHT      83
#define FINDEDIT_SPACING     6
#define BORDER_OFFSET        3

        /* Creation of CreateActionAppShell */
        _UxParent = UxParent;
        if ( _UxParent == NULL )
        {
            _UxParent = UxTopLevel;
        }

	strcpy(titleBuf, GETMESSAGE(3, 10, "Create Action"));
        CreateActionAppShell = XtVaCreatePopupShell( "CreateActionAppShell",
                        applicationShellWidgetClass,
                        _UxParent,
                        /*
                        XmNx, 35,
                        XmNy, 8,
                        XmNminWidth, -1,
                        XmNminHeight, -1,
                        */
                        XmNiconName, titleBuf,
                        XmNtitle, titleBuf,
                        XmNallowShellResize, TRUE,
                        XmNheight, 550,
			XmNdeleteResponse, XmDO_NOTHING, /* CDExc19440 */
                        NULL );

	/* CDExc19440 - make sure user doesn't accidentally lose work. */
	XmAddWMProtocolCallback(CreateActionAppShell,
				XmInternAtom(UxDisplay,
					     "WM_DELETE_WINDOW",
					     False),
				activateCB_FileQuit,
				(XtPointer)NULL);

        if (bLowRes) {
           /* Creation of CreateActionMainWindow */
           CreateActionMainWindow = XtVaCreateManagedWidget( "CreateActionMainWindow",
                        xmMainWindowWidgetClass,
                        CreateActionAppShell,
                        XmNunitType, XmPIXELS,
                        XmNx, 0,
                        XmNy, 0,
                        XmNscrollingPolicy, XmAUTOMATIC,
                        NULL );
        } else {
           /* Creation of CreateActionMainWindow */
           CreateActionMainWindow = XtVaCreateManagedWidget( "CreateActionMainWindow",
                        xmMainWindowWidgetClass,
                        CreateActionAppShell,
                        XmNunitType, XmPIXELS,
                        XmNx, 0,
                        XmNy, 0,
                        NULL );
        }
        XtAddCallback( CreateActionMainWindow, XmNhelpCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_ONITEM );

        /*********************************************************************/
        /*                                                                   */
        /* MenuBar                                                           */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_MenuBar */
        CA_MenuBar = XtVaCreateManagedWidget( "CA_MenuBar",
                        xmRowColumnWidgetClass,
                        CreateActionMainWindow,
                        XmNrowColumnType, XmMENU_BAR,
                        XmNmenuAccelerator, "<KeyUp>F10",
                        XmNpacking, XmPACK_TIGHT,
                        XmNentryAlignment, XmALIGNMENT_BEGINNING,
                        XmNorientation, XmHORIZONTAL,
                        NULL );

        /* Creation of CA_MB_FileOption */
        CA_MB_FileOption_shell = XtVaCreatePopupShell ("CA_MB_FileOption_shell",
                        xmMenuShellWidgetClass, CA_MenuBar,
                        XmNwidth, 1,
                        XmNheight, 1,
                        XmNallowShellResize, TRUE,
                        XmNoverrideRedirect, TRUE,
                        NULL );

        CA_MB_FileOption = XtVaCreateWidget( "CA_MB_FileOption",
                        xmRowColumnWidgetClass,
                        CA_MB_FileOption_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL );

        /* Creation of CA_MB_FileNew */
        /*
        mnemonic = GETMESSAGE(2, 21, "N");
        */
        strcpy(mnemonic, GETMESSAGE(2, 21, "N"));
        CA_MB_FileNew = XtVaCreateManagedWidget( "CA_MB_FileNew",
                        xmPushButtonWidgetClass,
                        CA_MB_FileOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 20, "New")),
                        XmNmnemonic, mnemonic[0],
                        NULL );
        XtAddCallback( CA_MB_FileNew, XmNactivateCallback,
                (XtCallbackProc) activateCB_FileNew,
                (XtPointer) NULL );

        /* Creation of CA_MB_FileOpen */
        /*
        mnemonic = GETMESSAGE(2, 16, "O");
        */
        strcpy(mnemonic, GETMESSAGE(2, 16, "O"));
        CA_MB_FileOpen = XtVaCreateManagedWidget( "CA_MB_FileOpen",
                        xmPushButtonWidgetClass,
                        CA_MB_FileOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 15, "Open...")),
                        XmNmnemonic, mnemonic[0],
                        NULL );
        XtAddCallback( CA_MB_FileOpen, XmNactivateCallback,
                (XtCallbackProc) activateCB_FileOpen,
                (XtPointer) NULL );

        /* Creation of CA_MB_FileSave */
        /*
        mnemonic = GETMESSAGE(2, 26, "S");
        */
        strcpy(mnemonic, GETMESSAGE(2, 26, "S"));
        CA_MB_FileSave = XtVaCreateManagedWidget( "CA_MB_FileSave",
                        xmPushButtonWidgetClass,
                        CA_MB_FileOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 25, "Save")),
                        XmNmnemonic, mnemonic[0],
                        NULL );
        XtAddCallback( CA_MB_FileSave, XmNactivateCallback,
                (XtCallbackProc) activateCB_CA_MB_FileSave,
                (XtPointer) NULL );

        /* Creation of CA_MB_Sep */
        CA_MB_Sep = XtVaCreateManagedWidget( "CA_MB_Sep",
                        xmSeparatorWidgetClass,
                        CA_MB_FileOption,
                        NULL );

        /* Creation of CA_MB_FileExit */
        /*
        mnemonic = GETMESSAGE(2, 31, "x");
        */
        strcpy(mnemonic, GETMESSAGE(2, 31, "x"));
        CA_MB_FileExit = XtVaCreateManagedWidget( "CA_MB_FileExit",
                        xmPushButtonWidgetClass,
                        CA_MB_FileOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 30, "Exit")),
                        XmNmnemonic, mnemonic[0],
                        NULL );
        XtAddCallback( CA_MB_FileExit, XmNactivateCallback,
                (XtCallbackProc) activateCB_FileQuit,
                (XtPointer) NULL );

        /* Creation of CA_MB_FileTop */
        /*
        mnemonic = GETMESSAGE(2, 11, "F");
        */
        strcpy(mnemonic, GETMESSAGE(2, 11, "F"));
        CA_MB_FileTop = XtVaCreateManagedWidget( "CA_MB_FileTop",
                        xmCascadeButtonWidgetClass,
                        CA_MenuBar,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 10, "File")),
                        XmNmnemonic, mnemonic[0],
                        XmNsubMenuId, CA_MB_FileOption,
                        NULL );

        /* Creation of CA_MB_HelpOption */
        CA_MB_HelpOption_shell = XtVaCreatePopupShell ("CA_MB_HelpOption_shell",
                        xmMenuShellWidgetClass, CA_MenuBar,
                        XmNwidth, 1,
                        XmNheight, 1,
                        XmNallowShellResize, TRUE,
                        XmNoverrideRedirect, TRUE,
                        NULL );

        CA_MB_HelpOption = XtVaCreateWidget( "CA_MB_HelpOption",
                        xmRowColumnWidgetClass,
                        CA_MB_HelpOption_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL );

        /*
         * Creation of CA_MB_HelpOverview
        */
        strcpy(mnemonic, GETMESSAGE(2, 61, "v"));
        CA_MB_HelpOverview = XtVaCreateManagedWidget( "CA_MB_HelpOverview",
                        xmPushButtonWidgetClass,
                        CA_MB_HelpOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 60, "Overview")),
                        XmNmnemonic, mnemonic[0],
                        XmNuserData, CreateActionAppShell,
                        NULL );
        XtAddCallback( CA_MB_HelpOverview, XmNactivateCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_OVERVIEW );

        /*
         * Creation of CA_MB_HelpTasks
        */
        strcpy(mnemonic, GETMESSAGE(2, 71, "T"));
        CA_MB_HelpTasks = XtVaCreateManagedWidget( "CA_MB_HelpTasks",
                        xmPushButtonWidgetClass,
                        CA_MB_HelpOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 70, "Tasks")),
                        XmNmnemonic, mnemonic[0],
                        XmNuserData, CreateActionAppShell,
                        NULL );
        XtAddCallback( CA_MB_HelpTasks, XmNactivateCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_TASKS );

#if 0
        /* Creation of CA_MB_HelpTOC */
        /*
        mnemonic = GETMESSAGE(2, 66, "T");
        */
        strcpy(mnemonic, GETMESSAGE(2, 66, "T"));
        CA_MB_HelpTOC = XtVaCreateManagedWidget( "CA_MB_HelpTOC",
                        xmPushButtonWidgetClass,
                        CA_MB_HelpOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 65, "Table Of Contents...")),
                        XmNmnemonic, mnemonic[0],
                        XmNuserData, CreateActionAppShell,
                        NULL );
#endif
        /*
        * Creation of CA_MB_HelpReference
        */
        strcpy(mnemonic, GETMESSAGE(2, 76, "R"));
        CA_MB_HelpReference = XtVaCreateManagedWidget( "CA_MB_HelpReference",
                        xmPushButtonWidgetClass,
                        CA_MB_HelpOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 75, "Reference")),
                        XmNmnemonic, mnemonic[0],
                        XmNuserData, CreateActionAppShell,
                        NULL );
        XtAddCallback( CA_MB_HelpReference, XmNactivateCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_REFERENCE );

        /*
         * Creation of CA_MB_Help_Sep1
         */
        CA_MB_Help_Sep1 = XtVaCreateManagedWidget( "CA_MB_Help_Sep1",
                        xmSeparatorWidgetClass,
                        CA_MB_HelpOption,
                        NULL );

        /*
         * Creation of CA_MB_HelpUsing
         */
        strcpy(mnemonic, GETMESSAGE(2, 81, "U"));
        CA_MB_HelpUsing = XtVaCreateManagedWidget( "CA_MB_HelpUsing",
                        xmPushButtonWidgetClass,
                        CA_MB_HelpOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 80, "Using Help")),
                        XmNmnemonic, mnemonic[0],
                        XmNuserData, CreateActionAppShell,
                        NULL );
        XtAddCallback( CA_MB_HelpUsing, XmNactivateCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_USING );

        /*
         * Creation of CA_MB_Help_Sep2
         */
        CA_MB_Help_Sep2 = XtVaCreateManagedWidget( "CA_MB_Help_Sep2",
                        xmSeparatorWidgetClass,
                        CA_MB_HelpOption,
                        NULL );

        /*
         * Creation of CA_MB_HelpAbout
         */
        strcpy(mnemonic, GETMESSAGE(2, 86, "A"));
        CA_MB_HelpAbout = XtVaCreateManagedWidget( "CA_MB_HelpAbout",
                        xmPushButtonWidgetClass,
                        CA_MB_HelpOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 85, "About Create Action")),
                        XmNmnemonic, mnemonic[0],
                        XmNuserData, CreateActionAppShell,
                        NULL );
        XtAddCallback( CA_MB_HelpAbout, XmNactivateCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_ABOUT );

        /*
         * Creation of CA_MB_HelpTop
         */
        strcpy(mnemonic, GETMESSAGE(2, 54, "H"));
        CA_MB_HelpTop = XtVaCreateManagedWidget( "CA_MB_HelpTop",
                        xmCascadeButtonWidgetClass,
                        CA_MenuBar,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 53, "Help")),
                        XmNmnemonic, mnemonic[0],
                        XmNsubMenuId, CA_MB_HelpOption,
                        XmNuserData, CreateActionAppShell,
                        NULL );

        /* Creation of CA_MB_OptionsOption */
        CA_MB_OptionsOption_shell = XtVaCreatePopupShell ("CA_MB_OptionsOption_shell",
                        xmMenuShellWidgetClass, CA_MenuBar,
                        XmNwidth, 1,
                        XmNheight, 1,
                        XmNallowShellResize, TRUE,
                        XmNoverrideRedirect, TRUE,
                        NULL );

        CA_MB_OptionsOption = XtVaCreateWidget( "CA_MB_OptionsOption",
                        xmRowColumnWidgetClass,
                        CA_MB_OptionsOption_shell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL );

        /* Creation of ColorMonoOption */
        strcpy(mnemonic, GETMESSAGE(2, 50, "M"));
        ColorMonoOption = XtVaCreateManagedWidget( "ColorMonoOption",
                        xmToggleButtonWidgetClass,
                        CA_MB_OptionsOption,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 49, "Show Monochrome Icons")),
                        XmNmnemonic, mnemonic[0],
                        XmNindicatorSize, 20,
                        XmNhighlightThickness, 0,
                        NULL );
        XtAddCallback( ColorMonoOption, XmNvalueChangedCallback,
                (XtCallbackProc) valueChangedCB_ColorMonoOption,
                (XtPointer) NULL );
        createCB_ColorMonoOption(ColorMonoOption);


        /* Creation of CA_MB_OptionTop */
        strcpy(mnemonic, GETMESSAGE(2, 36, "O"));
        CA_MB_OptionTop = XtVaCreateManagedWidget( "CA_MB_OptionTop",
                        xmCascadeButtonWidgetClass,
                        CA_MenuBar,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(2, 35, "Options")),
                        XmNmnemonic, mnemonic[0],
                        XmNsubMenuId, CA_MB_OptionsOption,
                        NULL );

        /* Creation of CA_Form */
        CA_Form = XtVaCreateManagedWidget( "CA_Form",
                        xmFormWidgetClass,
                        CreateActionMainWindow,
                        XmNnoResize, FALSE,
                        XmNresizePolicy, XmRESIZE_ANY,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Action Name                                                       */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_ActionNameLabel */
        CA_ActionNameLabel = XtVaCreateManagedWidget( "CA_ActionNameLabel",
                        xmLabelWidgetClass,
                        CA_Form,
                        XmNx, 10,
                        XmNy, 30,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 12, "Action Name (Icon Label):")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 20,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of CA_ActionNameTextField */
        CA_ActionNameTextField = XtVaCreateManagedWidget( "CA_ActionNameTextField",
                        xmTextFieldWidgetClass,
                        CA_Form,
                        XmNx, 10,
                        XmNy, 50,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_ActionNameLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );
#if 0
        XtAddCallback( CA_ActionNameTextField, XmNvalueChangedCallback,
                (XtCallbackProc) valueChangedCB_CA_ActionNameTextField,
                (XtPointer) NULL );
#endif

        /*********************************************************************/
        /*                                                                   */
        /* Icons                                                             */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_ActionIconLabel */
        CA_ActionIconLabel = XtVaCreateManagedWidget( "CA_ActionIconLabel",
                        xmLabelWidgetClass,
                        CA_Form,
                        XmNx, 10,
                        XmNy, 95,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 13, "Action Icons:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 15,
                        XmNtopWidget, CA_ActionNameTextField,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of CA_ActionIconForm */
        CA_ActionIconForm = XtVaCreateManagedWidget( "CA_ActionIconForm",
                        xmFormWidgetClass,
                        CA_Form,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNx, 10,
                        XmNy, 121,
                        XmNborderWidth, 0,
                        XmNnavigationType, XmTAB_GROUP,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_ActionIconLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNshadowThickness, 3,
                        XmNshadowType, XmSHADOW_ETCHED_IN,
                        XmNautoUnmanage, FALSE,
                        XmNnoResize, FALSE,
                        XmNallowOverlap, TRUE,
                        XmNmarginHeight, 0,
                        XmNverticalSpacing, 5,
                        NULL );

        CA_MED_IconForm = XtVaCreateManagedWidget( "CA_MED_IconForm",
                        xmFormWidgetClass,
                        CA_ActionIconForm,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNborderWidth, ICON_BORDER_WIDTH,
                        XmNleftPosition, 32,
                        XmNleftOffset, 0,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 88,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_POSITION,
                        NULL );

        /* Creation of CA_MED_IconGadget */
	emptyString = XmStringCreateLocalized("");
        pIconData = (IconData *)XtMalloc(sizeof(IconData));
        memset(pIconData, 0, sizeof(IconData));
        IconDataList[CA_MED] = pIconData;
        CA_MED_IconGadget = XtVaCreateManagedWidget( "CA_MED_IconGadget",
                        dtIconGadgetClass,
                        CA_MED_IconForm,
                        XmNalignment, XmALIGNMENT_CENTER,
                        XmNuserData, pIconData,
                        XmNstring, emptyString,
                        XmNrecomputeSize, TRUE,
                        XmNleftOffset, BORDER_OFFSET,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, BORDER_OFFSET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, BORDER_OFFSET,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNbottomOffset, BORDER_OFFSET,
                        XmNbottomAttachment, XmATTACH_FORM,
                        /*
                        XmNleftPosition, 32,
                        XmNleftOffset, 0,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 88,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_POSITION,
                        */
                        NULL );
        pIconData->wid = CA_MED_IconGadget;
        pIconData->size = Medium_Icon;
        XtAddCallback( CA_MED_IconGadget, XmNcallback,
                (XtCallbackProc) activateCB_action_icon,
                (XtPointer) NULL );
        createCB_IconGadget( CA_MED_IconGadget, TRUE, Medium_Icon );
        last_action_pushed = CA_MED_IconGadget;

        CA_TINY_IconForm = XtVaCreateManagedWidget( "CA_TINY_IconForm",
                        xmFormWidgetClass,
                        CA_ActionIconForm,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNborderWidth, 0,
                        XmNleftPosition, 52,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 88,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_POSITION,
                        NULL );

        /* Creation of CA_TINY_IconGadget */
        pIconData = (IconData *)XtMalloc(sizeof(IconData));
        memset(pIconData, 0, sizeof(IconData));
        IconDataList[CA_TINY] = pIconData;
        CA_TINY_IconGadget = XtVaCreateManagedWidget( "CA_TINY_IconGadget",
                        dtIconGadgetClass,
                        CA_TINY_IconForm,
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
        pIconData->wid = CA_TINY_IconGadget;
        pIconData->size = Tiny_Icon;
        XtAddCallback( CA_TINY_IconGadget, XmNcallback,
                (XtCallbackProc) activateCB_action_icon,
                (XtPointer) NULL );
        createCB_IconGadget( CA_TINY_IconGadget, TRUE, Tiny_Icon );

        CA_LRG_IconForm = XtVaCreateManagedWidget( "CA_LRG_IconForm",
                        xmFormWidgetClass,
                        CA_ActionIconForm,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNborderWidth, 0,
                        XmNleftPosition, 11,
                        XmNleftOffset, -1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 88,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_POSITION,
                        NULL );

        /* Creation of CA_LRG_IconGadget */
        pIconData = (IconData *)XtMalloc(sizeof(IconData));
        memset(pIconData, 0, sizeof(IconData));
        IconDataList[CA_LRG] = pIconData;
        CA_LRG_IconGadget = XtVaCreateManagedWidget( "CA_LRG_IconGadget",
                        dtIconGadgetClass,
                        CA_LRG_IconForm,
                        XmNalignment, XmALIGNMENT_CENTER,
                        XmNuserData, pIconData,
                        XmNstring, emptyString,
                        XmNrecomputeSize, TRUE,
                        XmNleftOffset, BORDER_OFFSET,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, BORDER_OFFSET,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, BORDER_OFFSET,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNbottomOffset, BORDER_OFFSET,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );
        pIconData->wid = CA_LRG_IconGadget;
        pIconData->size = Large_Icon;
        XtAddCallback( CA_LRG_IconGadget, XmNcallback,
                (XtCallbackProc) activateCB_action_icon,
                (XtPointer) NULL );
        createCB_IconGadget( CA_LRG_IconGadget, TRUE, Large_Icon );

	XmStringFree(emptyString);

        /* Creation of CA_EditIcon_button */
        CA_EditIcon_button = XtVaCreateManagedWidget( "CA_EditIcon_button",
                        xmPushButtonGadgetClass,
                        CA_ActionIconForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 20, "Edit Icon...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftPosition, 68,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNtopPosition, 50,
                        XmNtopOffset, (int)(FINDEDIT_SPACING / 2),
                        XmNtopAttachment, XmATTACH_POSITION,
                        NULL );
        XtAddCallback( CA_EditIcon_button, XmNactivateCallback,
                (XtCallbackProc) activateCB_edit_icon,
                (XtPointer) CA_ACTION_ICONS );

        /* Creation of CA_FindSet_button */
        CA_FindSet_button = XtVaCreateManagedWidget( "CA_FindSet_button",
                        xmPushButtonGadgetClass,
                        CA_ActionIconForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(6, 19, "Find Set...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNmultiClick, XmMULTICLICK_DISCARD,
                        XmNleftPosition, 68,
                        XmNleftOffset, 0,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomPosition, 50,
                        XmNbottomOffset, (int)(FINDEDIT_SPACING / 2),
                        XmNbottomAttachment, XmATTACH_POSITION,
                        NULL );
        XtAddCallback( CA_FindSet_button, XmNactivateCallback,
                (XtCallbackProc) activateCB_open_FindSet,
                (XtPointer) CA_ACTION_ICONS );

        /******************************************************************/
        /* Determine which button is wider, and attach the short on to it */
        /******************************************************************/
        XtVaGetValues(CA_EditIcon_button, XmNwidth, &dim1, NULL);
        XtVaGetValues(CA_FindSet_button, XmNwidth, &dim2, NULL);
        if (dim1 > dim2) {
           XtVaSetValues(CA_FindSet_button,
                         XmNwidth, dim1,
                         NULL);
        } else {
           XtVaSetValues(CA_EditIcon_button,
                         XmNwidth, dim2,
                         NULL);
        }
        /******************************************************************/
        /* Determine vertical positioning for findset and edit buttons    */
        /******************************************************************/
        XtVaGetValues(CA_EditIcon_button, XmNheight, &dim1, NULL);
        XtVaGetValues(CA_FindSet_button, XmNheight, &dim2, NULL);
        if ((int)(dim1 + dim2) + (3*(FINDEDIT_SPACING)) > ICON_MIN_HEIGHT) {
           XtVaSetValues(CA_EditIcon_button,
                         XmNbottomOffset, FINDEDIT_SPACING,
                         XmNbottomAttachment, XmATTACH_FORM,
                         NULL);
           XtVaSetValues(CA_FindSet_button,
                         XmNbottomOffset, FINDEDIT_SPACING,
                         XmNbottomWidget, CA_EditIcon_button,
                         XmNbottomAttachment, XmATTACH_WIDGET,
                         XmNtopOffset, FINDEDIT_SPACING,
                         XmNtopAttachment, XmATTACH_FORM,
                         NULL);
        }


        /******************************************************************/
        /* Bogus separator to keep ActionIconForm a minimum height        */
        /******************************************************************/
        separator1 = XtVaCreateManagedWidget( "separator1",
                        xmSeparatorWidgetClass,
                        CA_ActionIconForm,
                        XmNx, 10,
                        XmNy, 10,
                        XmNwidth, 20,
                        XmNheight, ICON_MIN_HEIGHT,
                        XmNorientation, XmVERTICAL,
                        XmNseparatorType, XmNO_LINE,
                        XmNtopOffset, 5,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 5,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Open Command                                                      */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_DblClkLabel */
        CA_DblClkLabel = XtVaCreateManagedWidget( "CA_DblClkLabel",
                        xmLabelWidgetClass,
                        CA_Form,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 22, "Command When Action Is Opened (Double-clicked):")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 15,
                        XmNtopWidget, CA_ActionIconForm,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of CA_DblClkText */
        CA_DblClkText = XtVaCreateManagedWidget( "CA_DblClkText",
                        xmTextFieldWidgetClass,
                        CA_Form,
                        XmNnavigationType, XmTAB_GROUP,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_DblClkLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNrows, 1,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Help Text Label                                                   */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_HlpTxtLabel */
        CA_HlpTxtLabel = XtVaCreateManagedWidget( "CA_HlpTxtLabel",
                        xmLabelWidgetClass,
                        CA_Form,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 30, "Help Text For Action Icon:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 15,
                        XmNtopWidget, CA_DblClkText,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Advanced Features Area                                            */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of XprtOptionForm */
        XprtOptionForm = XtVaCreateManagedWidget( "XprtOptionForm",
                        xmFormWidgetClass,
                        CA_Form,
                        XmNresizePolicy, XmRESIZE_ANY,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 1,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Optional Separator                                                */
        /*                                                                   */
        /*********************************************************************/

        /* Creation of CA_XprtSep1 */
        CA_XprtSep1 = XtVaCreateManagedWidget( "CA_XprtSep1",
                        xmSeparatorWidgetClass,
                        XprtOptionForm,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 1,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, 0,
                        XmNtopAttachment, XmATTACH_FORM,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Prompt Field                                                      */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_ActionOpenLabel */
        CA_ActionOpenLabel = XtVaCreateManagedWidget( "CA_ActionOpenLabel",
                        xmLabelWidgetClass,
                        XprtOptionForm,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 11, "When Action Opens, Ask Users for:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, 15,
                        XmNtopWidget, CA_XprtSep1,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        NULL );

        /* Creation of CA_XprtActionOpenText */
        CA_XprtActionOpenText = XtVaCreateManagedWidget( "CA_XprtActionOpenText",
                        xmTextWidgetClass,
                        XprtOptionForm,
                        XmNvalue, "",
                        XmNnavigationType, XmTAB_GROUP,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_ActionOpenLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                      /*XmNeditMode, XmMULTI_LINE_EDIT,*/
                      /*XmNwordWrap, TRUE,*/
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Filetypes that use this Action area                               */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_FiletypesLabel */
        CA_FiletypesLabel = XtVaCreateManagedWidget( "CA_FiletypesLabel",
                        xmLabelWidgetClass,
                        XprtOptionForm,
                        XmNx, 10,
                        XmNy, 122,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 12, "Datatypes That Use This Action:")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, 15,
                        XmNtopWidget, CA_XprtActionOpenText,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Add, Delete, Edit buttons for Filetypes                           */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of rowColumn1 */
        rowColumn1 = XtVaCreateManagedWidget( "rowColumn1",
                        xmRowColumnWidgetClass,
                        XprtOptionForm,
                        XmNnavigationType, XmTAB_GROUP,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_FiletypesLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        /* Creation of CA_FiletypesAdd */
        CA_FiletypesAdd = XtVaCreateManagedWidget( "CA_FiletypesAdd",
                        xmPushButtonGadgetClass,
                        rowColumn1,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 13, "Add...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNmarginLeft, 5,
                        XmNmarginRight, 5,
                        NULL );
        XtAddCallback( CA_FiletypesAdd, XmNactivateCallback,
                (XtCallbackProc) activateCB_add_filetype,
                (XtPointer) NULL );

        /* Creation of CA_FiletypesDelete */
        CA_FiletypesDelete = XtVaCreateManagedWidget( "CA_FiletypesDelete",
                        xmPushButtonGadgetClass,
                        rowColumn1,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 14, "Delete")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNmarginLeft, 5,
                        XmNmarginRight, 5,
                        NULL );
        XtAddCallback( CA_FiletypesDelete, XmNactivateCallback,
                (XtCallbackProc) activateCB_CA_FiletypesDelete,
                (XtPointer) NULL );

        /* Creation of CA_FiletypesEdit */
        CA_FiletypesEdit = XtVaCreateManagedWidget( "CA_FiletypesEdit",
                        xmPushButtonGadgetClass,
                        rowColumn1,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 15, "Edit...")),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNmarginLeft, 5,
                        XmNmarginRight, 5,
                        NULL );
        XtAddCallback( CA_FiletypesEdit, XmNactivateCallback,
                (XtCallbackProc) activateCB_edit_filetype,
                (XtPointer) NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Filetypes Listbox Area                                            */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_FiletypesWindow */
        CA_FiletypesWindow = XtVaCreateManagedWidget( "CA_FiletypesWindow",
                        xmScrolledWindowWidgetClass,
                        XprtOptionForm,
                        XmNx, 10,
                        XmNy, 140,
                        XmNshadowThickness, 2,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_FiletypesLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNbottomOffset, 0,
                        XmNbottomWidget, rowColumn1,
                        XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightWidget, rowColumn1,
                        XmNrightAttachment, XmATTACH_WIDGET,
                        NULL );

        /* Creation of CA_FiletypesList */
        CA_FiletypesList = XtVaCreateManagedWidget( "CA_FiletypesList",
                        xmListWidgetClass,
                        CA_FiletypesWindow,
                        XmNselectionPolicy, XmSINGLE_SELECT,
                        XmNshadowThickness, 0,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Droppable Filetypes Toggle Area                                   */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_DroppableFiletypesLabel */
        CA_DroppableFiletypesLabel = XtVaCreateManagedWidget( "CA_DroppableFiletypesLabel",
                        xmLabelWidgetClass,
                        XprtOptionForm,
                        XmNx, 10,
                        XmNy, 204,
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 16, "Dropable Datatypes:")),
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNtopOffset, 15,
                        XmNtopWidget, rowColumn1,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        NULL );

        if (bLowRes) {
           /* Creation of CA_DroppableFiletypesRowColumn */
           CA_DroppableFiletypesRowColumn = XtVaCreateManagedWidget( "CA_DroppableFiletypesRowColumn",
                           xmRowColumnWidgetClass,
                           XprtOptionForm,
                           XmNx, 200,
                           XmNy, 204,
                           RES_CONVERT( XmNlabelString, "" ),
                           XmNorientation, XmHORIZONTAL,
                           XmNpacking, XmPACK_TIGHT,
                           XmNradioBehavior, TRUE,
                           XmNwhichButton, 1,
                           XmNnavigationType, XmTAB_GROUP,
                           XmNtopOffset, 0,
                           XmNtopWidget, CA_DroppableFiletypesLabel,
                           XmNtopAttachment, XmATTACH_WIDGET,
                           XmNbottomOffset, 0,
                           XmNbottomAttachment, XmATTACH_FORM,
                           XmNleftOffset, 10,
                           XmNleftAttachment, XmATTACH_FORM,
                           XmNrightOffset, 10,
                           XmNrightAttachment, XmATTACH_FORM,
                           NULL );
        } else {
           /* Creation of CA_DroppableFiletypesRowColumn */
           CA_DroppableFiletypesRowColumn = XtVaCreateManagedWidget( "CA_DroppableFiletypesRowColumn",
                           xmRowColumnWidgetClass,
                           XprtOptionForm,
                           RES_CONVERT( XmNlabelString, "" ),
                           XmNorientation, XmHORIZONTAL,
                           XmNpacking, XmPACK_TIGHT,
                           XmNradioBehavior, TRUE,
                           XmNwhichButton, 1,
                           XmNnavigationType, XmTAB_GROUP,
                           XmNtopOffset, -7,
                           XmNtopWidget, CA_DroppableFiletypesLabel,
                           XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
                           XmNbottomOffset, 0,
                           XmNbottomAttachment, XmATTACH_FORM,
                           XmNleftOffset, 0,
                           XmNleftWidget, CA_DroppableFiletypesLabel,
                           XmNleftAttachment, XmATTACH_WIDGET,
                           XmNrightOffset, 10,
                           XmNrightAttachment, XmATTACH_FORM,
                           NULL );
        }

        /* Creation of CA_AllFiletypesToggle */
        CA_AllFiletypesToggle = XtVaCreateManagedWidget( "CA_AllFiletypesToggle",
                        xmToggleButtonWidgetClass,
                        CA_DroppableFiletypesRowColumn,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 17, "All Datatypes")),
                        XmNset, TRUE,
                        NULL );

        /* Creation of CA_FiletypesInListToggle */
        CA_FiletypesInListToggle = XtVaCreateManagedWidget( "CA_FiletypesInListToggle",
                        xmToggleButtonWidgetClass,
                        CA_DroppableFiletypesRowColumn,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(4, 18, "Only Above List")),
                        NULL );
        XtAddCallback( CA_FiletypesInListToggle, XmNvalueChangedCallback,
                (XtCallbackProc) valueChangedCB_DroppableFiletypes,
                (XtPointer) CA_DF_ONLYFTFILES );


        /*********************************************************************/
        /*                                                                   */
        /* Expand button                                                     */
        /*                                                                   */
        /*********************************************************************/
        CA_Expand = XtVaCreateManagedWidget( "CA_Expand",
                        xmPushButtonWidgetClass,
                        CA_Form,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 50, "Advanced")),
                        XmNmultiClick, XmMULTICLICK_DISCARD,
                        XmNnavigationType, XmTAB_GROUP,
                        XmNmarginLeft, 5,
                        XmNmarginRight, 5,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 20,
                        XmNbottomWidget, XprtOptionForm,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        NULL );
        XtAddCallback( CA_Expand, XmNactivateCallback,
                (XtCallbackProc) activateCB_ExpertOption,
                (XtPointer) NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Start of WindowType Option Menu                                   */
        /*                                                                   */
        /*********************************************************************/
        CA_WindowType_OptionMenuShell = XtVaCreatePopupShell ("CA_WindowType_OptionMenuShell",
                        xmMenuShellWidgetClass,
                        CA_Form,
                        XmNwidth, 1,
                        XmNheight, 1,
                        XmNallowShellResize, TRUE,
                        XmNoverrideRedirect, TRUE,
                        NULL );

        CA_WindowType_Pane = XtVaCreateWidget( "CA_WindowType_Pane",
                        xmRowColumnWidgetClass,
                      /*CA_WindowType,*/
                        CA_WindowType_OptionMenuShell,
                        XmNrowColumnType, XmMENU_PULLDOWN,
                        NULL );

        CA_WinTypeX = XtVaCreateManagedWidget( "CA_WinTypeX",
                        xmPushButtonWidgetClass,
                        CA_WindowType_Pane,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 34, "Graphical (X-Window)")),
                        NULL );
        XtAddCallback( CA_WinTypeX, XmNactivateCallback,
                (XtCallbackProc) activateCB_WindowTypeOptionMenu,
                (XtPointer) CA_WT_XWINDOWS );

        CA_WinTypeAutoClose = XtVaCreateManagedWidget( "CA_WinTypeAutoClose",
                        xmPushButtonWidgetClass,
                        CA_WindowType_Pane,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 36, "Terminal (Auto-Close)")),
                        NULL );
        XtAddCallback( CA_WinTypeAutoClose, XmNactivateCallback,
                (XtCallbackProc) activateCB_WindowTypeOptionMenu,
                (XtPointer) CA_WT_TERM );

        CA_WinTypeManualClose = XtVaCreateManagedWidget( "CA_WinTypeManualClose",
                        xmPushButtonWidgetClass,
                        CA_WindowType_Pane,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 38, "Terminal (Manual Close)")),
                        NULL );
        XtAddCallback( CA_WinTypeManualClose, XmNactivateCallback,
                (XtCallbackProc) activateCB_WindowTypeOptionMenu,
                (XtPointer) CA_WT_PERMTERM );

        CA_WinTypeNoOutput = XtVaCreateManagedWidget( "CA_WinTypeNoOutput",
                        xmPushButtonWidgetClass,
                        CA_WindowType_Pane,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 40, "No Output")),
                        NULL );
        XtAddCallback( CA_WinTypeNoOutput, XmNactivateCallback,
                (XtCallbackProc) activateCB_WindowTypeOptionMenu,
                (XtPointer) CA_WT_XWINDOWS );

        CA_WindowType_OptionMenu = XtVaCreateManagedWidget( "CA_WindowType_OptionMenu",
                        xmRowColumnWidgetClass,
                        CA_Form,
                        XmNrowColumnType, XmMENU_OPTION,
                        XmNsubMenuId, CA_WindowType_Pane,
                        RES_CONVERT( XmNlabelString, GETMESSAGE(3, 32, "Window Type: ")),
                        XmNnavigationType, XmTAB_GROUP,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 15,
                        XmNbottomWidget, CA_Expand,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        NULL );
        CA_WindowTypeArea = CA_WindowType_OptionMenu;

        /*********************************************************************/
        /*                                                                   */
        /* Help Text Window                                                  */
        /*                                                                   */
        /*********************************************************************/
        /* Creation of CA_HelpTextWindow */
        CA_HelpTextWindow = XtVaCreateManagedWidget( "CA_HelpTextWindow",
                        xmScrolledWindowWidgetClass,
                        CA_Form,
                        XmNx, 10,
                        XmNy, 336,
                        XmNshadowThickness, 0,
                        XmNtopOffset, 0,
                        XmNtopWidget, CA_HlpTxtLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 15,
                        XmNbottomWidget, CA_WindowTypeArea,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        NULL );

        /* Creation of CA_HelpText */
        CA_HelpText = XtVaCreateManagedWidget( "CA_HelpText",
                        xmTextWidgetClass,
                        CA_HelpTextWindow,
                        XmNeditMode, XmMULTI_LINE_EDIT ,
                        XmNscrollVertical, TRUE,
                        XmNscrollHorizontal, FALSE,
                        XmNwordWrap, TRUE,
                        XmNx, 12,
                        XmNy, 0,
                        XmNrows, 3,
                        NULL );

        /*********************************************************************/
        /*                                                                   */
        /* Misc.                                                             */
        /*                                                                   */
        /*********************************************************************/
        XtVaSetValues(CA_MenuBar,
                        XmNmenuHelpWidget, CA_MB_HelpTop,
                        NULL );

        XtAddCallback( CreateActionAppShell, XmNdestroyCallback,
                (XtCallbackProc) UxDestroyContextCB,
                (XtPointer) NULL );

        XmMainWindowSetAreas( CreateActionMainWindow, CA_MenuBar, (Widget) NULL,
                        (Widget) NULL, (Widget) NULL, CA_Form );

        return ( CreateActionAppShell );
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

Widget  create_applicationShell1(swidget _UxUxParent)
{
        Widget                   rtrn;
        _UxCCreateActionAppShell *UxContext;
        Dimension                dimHeight;
        Dimension                dimWidth;
        Dimension                dimTmp;
        Widget                   widTmp;

        UxCreateActionAppShellContext = UxContext =
                (_UxCCreateActionAppShell *) UxNewContext( sizeof(_UxCCreateActionAppShell), False );

        UxParent = _UxUxParent;

        rtrn = _Uxbuild_CreateActionAppShell();

        /******************************************************************/
        /* Need to realize app shell widget now, because we need to       */
        /* determine initial geometry before the advanced area is         */
        /* unmanaged in   set_ca_dialog_height  routine.                  */
        /******************************************************************/
	XtSetMappedWhenManaged(rtrn, False);
        XtRealizeWidget(rtrn);
        XmProcessTraversal(CA_ActionNameTextField, XmTRAVERSE_CURRENT);

        FileTypes = (Widget)CA_FiletypesList;

        set_ca_dialog_height ();

        if (sessiondata.useSession) {
           XtVaSetValues (CreateActionAppShell,
			  XmNx, sessiondata.x,
			  XmNy, sessiondata.y,
			  XmNwidth, sessiondata.width,
			  XmNheight, sessiondata.height,
                          XmNinitialState, sessiondata.iconicState,
                          NULL);

	   /* display in workspace */

	   if (sessiondata.workspaces[0])
	   {
	       char *workspaces = sessiondata.workspaces;
	       char *ptr;
	       Atom * workspace_atoms = NULL;
	       int num_workspaces = 0;
	       do
	       {
		   ptr = strchr (workspaces, '*');
		   if (ptr != NULL) *ptr = NULL;

		   workspace_atoms = (Atom *) XtRealloc (
				(char *)workspace_atoms, 
				sizeof (Atom) * (num_workspaces + 1));
		   workspace_atoms[num_workspaces] = XmInternAtom(
					XtDisplay(CreateActionAppShell),
					workspaces, True);

		   num_workspaces++;				
		   if (ptr != NULL)
		   {
		       *ptr = '*';
		       workspaces = ptr + 1;
		   }
	       } while (ptr != NULL);
	       DtWsmSetWorkspacesOccupied( XtDisplay(CreateActionAppShell),
					   XtWindow(CreateActionAppShell),
					   workspace_atoms,
					   num_workspaces);
	       XtFree ((char *) workspace_atoms);
	       workspace_atoms = NULL;
	   }
	}
	else if (bLowRes) {
           XtVaGetValues (CA_Form,
                          XmNheight, &dimHeight,
                          XmNwidth, &dimWidth,
                          NULL);
           XtVaGetValues (CreateActionMainWindow,
                          XmNverticalScrollBar, &widTmp,
                          NULL);
           XtVaGetValues (widTmp,
                          XmNwidth, &dimTmp,
                          NULL);
           dimHeight += dimTmp;
           dimWidth += dimTmp;

           XtVaGetValues (CA_MenuBar,
                          XmNheight, &dimTmp,
                          NULL);
           dimHeight += dimTmp;

           XtVaSetValues (CreateActionAppShell,
                          XmNheight, dimHeight,
                          XmNwidth, dimWidth,
                          NULL);
        }

	XtSetMappedWhenManaged(rtrn, True);
	XtManageChild(rtrn);

        return(rtrn);
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

