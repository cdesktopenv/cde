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
/* $XConsortium: HelpDialog.c /main/16 1996/11/22 12:24:49 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpDialog.c
 **
 **   Project:     Cde Help 1.0 Project
 **
 **   Description: 
 ** 
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>  /* R_OK */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

/* These includes work in R4 and R5 */
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/RepType.h>
#include <Xm/XmP.h>
#include <Xm/ScrolledW.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/List.h>
#include <Xm/RowColumnP.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>

/* Copied from Xm/GeoUtilsI.h */
extern XmGeoMatrix _XmGeoMatrixAlloc( 
                        unsigned int numRows,
                        unsigned int numBoxes,
                        unsigned int extSize) ;

/* Canvas Engine */
#include "CanvasP.h" /* for AccessI.h */

/* Help Dialog Widget Includes */
#include "Access.h"
#include "bufioI.h"
#include "AccessI.h"
#include "ActionsI.h"
#include "DisplayAreaI.h"

#include <Dt/Help.h>
#include "HelpI.h"
#include "HelpP.h"
#include "StringFuncsI.h"
#include "HelposI.h"
#include "HelpDialog.h"
#include "HelpDialogI.h"
#include "HelpDialogP.h"
#include "HistoryI.h"
#include "HelpUtilI.h"
#include "PathAreaI.h"
#include "HelpAccessI.h"
#include "HourGlassI.h"
#include "HyperTextI.h"
#include "FileUtilsI.h"
#include "Lock.h"

/* search dialogs */
#include "GlobSearchI.h"

/* print dialogs */
#include "PrintI.h"

/* Error Messages Includes */
#include "MessagesP.h"

/* Quick Help Widget Inlcudes */
#include "HelpQuickD.h"

/* Display Area Includes */
#include "XUICreateI.h"
#include "CallbacksI.h"
#include "DestroyI.h"
#include "FormatI.h"
#include "FormatManI.h"
#include "SetListI.h"
#include "ResizeI.h"


/*********** Global Variables **********/

/******** Messages *********/
/* Help Dialog Error message Defines */

#define HDMessage0	_DtHelpMsg_0000
#define HDMessage1	_DtHelpMsg_0001
#define HDMessage2	_DtHelpMsg_0002
#define HDMessage3	_DtHelpMsg_0003
#define HDMessage4	_DtHelpMsg_0004
#define HDMessage5	_DtHelpMsg_0005
#define HDMessage6	_DtHelpMsg_0006
#define HDMessage7	_DtHelpMsg_0007
#define HDMessage8	_DtHelpMsg_0008
#define HDMessage9	_DtHelpMsg_0009


/********    Static Function Declarations    ********/

static void NavigationTypeDefault( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;

static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void ClassInitialize( 
                        void) ;
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);

static void Destroy(
                        Widget w );
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args);
static void CloseHelpCB (
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData);
static void BuildDisplayArea(
                        Widget parent,
                        DtHelpDialogWidget nw);
static void ProcessJumpBack(
                        DtHelpDialogWidget nw);
static void VariableInitialize(
                        DtHelpDialogWidget nw);
static void CleanUpHelpDialog(
                        Widget nw,
                        int cleanUpKind);
static void CatchClose(Widget widget);
static void  DisplayTopLevelCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data );
static void  CopyTextCB(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData );
static void  DisplayPrintCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data );
static void  ProcessBMenuBackCB(
                        Widget w,
                        XtPointer clientData,
                        XButtonEvent * event,
                        Boolean *continueToDispatch);
static void  ProcessJumpReuse(
                        Widget nw,
                        DtHelpHyperTextStruct *hyperData);
static Widget SetupHelpDialogMenus(
                        Widget parent);
static Widget CreatePopupMenu(
                        Widget parent);
static void ResizeHelpDialogCB (
                        XtPointer clientData);
static void InitialPopupCB(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData);


/********    End Static Function Declarations    ********/



/* Static variables */

/* Supported resources for the HelpDialog Widget */

static XtResource resources[] = {
   
    {   DtNshowNewWindowButton,
        DtCShowNewWindowButton,
        XmRBoolean,
        sizeof(Boolean),
        XtOffset (DtHelpDialogWidget, help_dialog.menu.showDupBtn),
        XmRImmediate,
        (XtPointer) False
    },

    {   DtNshowTopLevelButton,
        DtCShowTopLevelButton,
        XmRBoolean,
        sizeof(Boolean),
        XtOffset (DtHelpDialogWidget, help_dialog.browser.showTopLevelBtn),
        XmRImmediate,
        (XtPointer) False
    },

    {
        DtNscrollBarPolicy,
        DtCScrollBarPolicy, DtRDtScrollBarPolicy, sizeof (unsigned char),
        XtOffset (DtHelpDialogWidget, help_dialog.display.scrollBarPolicy),
        XmRImmediate,  (XtPointer) DtHELP_AS_NEEDED_SCROLLBARS
    },

    {
        DtNexecutionPolicy,
        DtCExecutionPolicy, DtRDtExecutionPolicy, sizeof (unsigned char),
        XtOffset (DtHelpDialogWidget, help_dialog.display.executionPolicy),
        XmRImmediate,  (XtPointer) DtHELP_EXECUTE_QUERY_UNALIASED
    },

    {	DtNcolumns, 
        DtCColumns, XmRShort, sizeof(short), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.textColumns), 
	XmRImmediate, (XtPointer) 70
     },
    
     {	DtNrows, 
        DtCRows, XmRShort, sizeof(short), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.textRows), 
	XmRImmediate, (XtPointer) 25
     },
     
     {  DtNmarginWidth, 
        DtCMarginWidth, XmRHorizontalDimension, sizeof (Dimension),
        XtOffset (DtHelpDialogWidget, help_dialog.ghelp.marginWidth), 
        XmRImmediate, (XtPointer) 1
     },

     {  DtNmarginHeight, 
        DtCMarginHeight, XmRVerticalDimension, sizeof (Dimension),
        XtOffset (DtHelpDialogWidget, help_dialog.ghelp.marginHeight), 
        XmRImmediate, (XtPointer) 4
     },

    {	DtNvisiblePathCount, 
        DtCVisiblePathCount, XmRInt, sizeof(int), 
	XtOffset (DtHelpDialogWidget, help_dialog.browser.visiblePathCount), 
	XmRImmediate, (XtPointer) 7
     },

     {	DtNlocationId, 
	DtCLocationId, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.locationId), 
	XmRImmediate, (XtPointer) _DtHelpDefaultLocationId
      }, 

     {	DtNhelpPrint, 
	DtCHelpPrint, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.print.helpPrint), 
	XmRImmediate, (XtPointer) _DtHelpDefaultHelpPrint
      }, 

     {	DtNprinter, 
	DtCPrinter, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.print.printer), 
	XmRImmediate, (XtPointer) NULL
      }, 

     {	DtNpaperSize, 
	DtCPaperSize, DtRDtPaperSize, sizeof (unsigned char), 
	XtOffset (DtHelpDialogWidget, help_dialog.print.paperSize), 
	XmRImmediate, (XtPointer) DtHELP_PAPERSIZE_LETTER
      }, 

     {	DtNsrchHitPrefixFont, 
	DtCSrchHitPrefixFont, XmRString, sizeof (char *), 
	XtOffset (DtHelpDialogWidget, help_dialog.srch.hitPrefixFont), 
	XmRImmediate, (XtPointer) _DtHelpDefaultSrchHitPrefixFont
      }, 

     {	DtNhelpVolume, 
	DtCHelpVolume, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.helpVolume), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNmanPage, 
	DtCManPage, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.manPage), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNstringData, 
	DtCStringData, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.stringData), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNhelpFile, 
	DtCHelpFile, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.helpFile), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNtopicTitle, 
	DtCTopicTitle, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.topicTitleStr), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNhelpOnHelpVolume, 
	DtCHelpOnHelpVolume, XmRString, sizeof (char*), 
	XtOffset (DtHelpDialogWidget, help_dialog.help.helpOnHelpVolume), 
	XmRImmediate, (XtPointer) _DtHelpDefaultHelp4HelpVolume
      }, 

      {	DtNhelpType, 
        DtCHelpType, DtRDtHelpType, sizeof(unsigned char), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.helpType), 
	XmRImmediate, (XtPointer) DtHELP_TYPE_TOPIC
      },

      {	DtNhyperLinkCallback, 
	DtCHyperLinkCallback, XmRCallback, sizeof (XtCallbackList), 
	XtOffset (DtHelpDialogWidget, help_dialog.display.hyperLinkCallback), 
	XmRImmediate, (XtPointer) NULL
      }, 
    
      {	DtNcloseCallback, 
	DtCCloseCallback, XmRCallback, sizeof (XtCallbackList), 
	XtOffset (DtHelpDialogWidget, help_dialog.ghelp.closeCallback), 
	XmRImmediate, (XtPointer) NULL
      },

      { XmNnavigationType, XmCNavigationType, XmRNavigationType,
	sizeof(unsigned char),
	XtOffsetOf (XmManagerRec, manager.navigation_type),
	XmRCallProc, (XtPointer) NavigationTypeDefault
      },

};

/*
 * attach the action list to the widget. Then it does not
 * matter which Xt[App]Initialize an application does.
 */
static XtActionsRec DrawnBActions[] =
    {
        {"DeSelectAll"    , _DtHelpDeSelectAll    },
        {"SelectAll"      , _DtHelpSelectAll      },
        {"ActivateLink"   , _DtHelpActivateLink   },
        {"CopyToClipboard", _DtHelpCopyAction     },
        {"PageUpOrDown"   , _DtHelpPageUpOrDown   },
        {"PageLeftOrRight", _DtHelpPageLeftOrRight},
        {"NextLink"       , _DtHelpNextLink       }
    };



/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

externaldef( dthelpdialogwidgetclassrec) DtHelpDialogWidgetClassRec dtHelpDialogWidgetClassRec =
{
   {                                            /* core_class fields  */
      (WidgetClass) &xmBulletinBoardClassRec,   /* superclass         */
      "DtHelpDialog",                           /* class_name         */
      sizeof(DtHelpDialogWidgetRec),            /* widget_size        */
      ClassInitialize,                          /* class_initialize   */
      ClassPartInitialize,                      /* class_part_init    */
      FALSE,                                    /* class_inited       */
      Initialize,                               /* initialize         */
      NULL,                                     /* initialize_hook    */
      XtInheritRealize,                         /* realize            */
      DrawnBActions,                            /* actions            */
      XtNumber(DrawnBActions),                  /* num_actions        */
      resources,                                /* resources          */
      XtNumber(resources),                      /* num_resources      */
      NULLQUARK,                                /* xrm_class          */
      TRUE,                                     /* compress_motion    */
      XtExposeCompressMaximal,                  /* compress_exposure  */
      FALSE,                                    /* compress_enterlv   */
      FALSE,                                    /* visible_interest   */
      Destroy,                                  /* destroy            */
      XtInheritResize,                          /* resize             */
      XtInheritExpose,                          /* expose             */
      SetValues,                                /* set_values         */
      NULL,                                     /* set_values_hook    */
      XtInheritSetValuesAlmost,                 /* set_values_almost  */
      NULL,                                     /* get_values_hook    */
      XtInheritAcceptFocus,                     /* enter_focus        */
      XtVersion,                                /* version            */
      NULL,                                     /* callback_private   */
      XtInheritTranslations,                    /* tm_table           */
      XtInheritQueryGeometry,                   /* query_geometry     */
      NULL,                                     /* display_accelerator*/
      NULL,                                     /* extension          */
   },

   {                                            /* composite_class fields */
      XtInheritGeometryManager,                 /* geometry_manager   */
      XtInheritChangeManaged,                   /* change_managed     */
      XtInheritInsertChild,                     /* insert_child       */
      XtInheritDeleteChild,                     /* delete_child       */
      NULL,                                     /* extension          */
   },

   {                                            /* constraint_class fields */
      NULL,                                     /* resource list        */   
      0,                                        /* num resources        */   
      0,                                        /* constraint size      */   
      NULL,                                     /* init proc            */   
      NULL,                                     /* destroy proc         */   
      NULL,                                     /* set values proc      */   
      NULL,                                     /* extension            */
   },

   {                                            /* manager_class fields   */
      XmInheritTranslations,                    /* translations           */
      NULL,                                     /* syn_resources          */
      0,                                        /* num_syn_resources      */
      NULL,                                     /* syn_cont_resources     */
      0,                                        /* num_syn_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,                                     /* extension              */
   },

   {                                            /* bulletinBoard class  */
      TRUE,                                     /*always_install_accelerators*/
      _DtHelpDialogWidgetGeoMatrixCreate,      /* geo__matrix_create */
      XmInheritFocusMovedProc,                  /* focus_moved_proc */
      NULL                                      /* extension */
   },   

   {                                            /* messageBox class - none */
      0                                         /* mumble */
   }    
};

externaldef( dthelpdialogwidgetclass) WidgetClass dtHelpDialogWidgetClass
                                = (WidgetClass) &dtHelpDialogWidgetClassRec;

static char *HelpTypeNames[] =
{   "help_type_topic", 
    "help_type_string",
    "help_type_man_page",
    "help_type_file",
    "help_type_dynamic_string"
};

static char *ScrollBarValueNames[] =
{   "help_no_scrollbars",
    "help_static_scrollbars", 
    "help_as_needed_scrollbars"
};

static char *ExecutionValueNames[] =
{   "help_execute_none",
    "help_execute_query_all",
    "help_execute_query_unaliased",
    "help_execute_all"
};

/* the _DtHelpPaperSizeNames[] are in Print.c */

#define NUM_NAMES( list )        (sizeof( list) / sizeof( char *))





/*********************************************************************
 *
 * NavigationTypeDefault
 *    
 *
 *********************************************************************/
static void 
NavigationTypeDefault(
        Widget widget,
        int offset,		/* unused */
        XrmValue *value )
{
    static XmNavigationType navigation_type;
    Widget parent = XtParent(widget) ;

    value->addr = (XPointer) &navigation_type;
    if (XtIsShell(parent)) {
	navigation_type = XmSTICKY_TAB_GROUP;
    } else {
	navigation_type = XmTAB_GROUP;
    }
      
}


/*****************************************************************************
 * Function:	    static void ClassInitialize (
 *                      void)
 *
 * Parameters:      Void.
 *
 *
 * Return Value:    Void.
 *
 * Purpose:   Register our representation types here 
 *
 *****************************************************************************/
static void ClassInitialize(void)
{
  XmRepTypeId checkId;

    /* First check to see if these have already been registered */
     checkId = XmRepTypeGetId(DtRDtScrollBarPolicy);

    if (checkId == XmREP_TYPE_INVALID)
      {
        /* Register the help representation types here */
   
        XmRepTypeRegister(DtRDtHelpType, HelpTypeNames, NULL,
                                      NUM_NAMES(HelpTypeNames)) ;
        XmRepTypeRegister(DtRDtScrollBarPolicy, ScrollBarValueNames, NULL,
                                      NUM_NAMES(ScrollBarValueNames)) ;
        XmRepTypeRegister(DtRDtExecutionPolicy, ExecutionValueNames, NULL,
                                      NUM_NAMES(ExecutionValueNames)) ;
        XmRepTypeRegister(DtRDtPaperSize, _DtHelpPaperSizeNames, NULL,
                                      _DtHelpPaperSizeNamesCnt) ;
      }

    return ;
}



/*****************************************************************************
 * Function:	    static void ClassPartInitialize (
 *                      WidgetClass widgetClass)
 *
 * Parameters:      WidgetClass      
 *
 *
 * Return Value:    Void.
 *
 * Purpose: 	    
 *
 *****************************************************************************/
static void ClassPartInitialize(
    WidgetClass widgetClass)
{

   return ;
}
 


 

/*****************************************************************************
 * Function:	    static void VariableInitialize()
 *                      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    This routine initializes all global variables to valid
 *                  starting values.
 *
 *****************************************************************************/
static void VariableInitialize(
DtHelpDialogWidget nw)
{

  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;

  /* Set our current topic variables to initial values */
  _DtHelpCommonHelpInit(&hw->help_dialog.help);

  /* Make local copies of all resource strings assigned by the user */
  if (hw->help_dialog.display.locationId != NULL)
    hw->help_dialog.display.locationId = XtNewString(hw->help_dialog.display.locationId);
  if (hw->help_dialog.display.helpVolume != NULL)
     hw->help_dialog.display.helpVolume = XtNewString(hw->help_dialog.display.helpVolume);
  if (hw->help_dialog.display.manPage != NULL)
    hw->help_dialog.display.manPage = XtNewString(hw->help_dialog.display.manPage);
  if (hw->help_dialog.display.stringData != NULL)
    hw->help_dialog.display.stringData = XtNewString(hw->help_dialog.display.stringData);
  if (hw->help_dialog.display.helpFile != NULL)
    hw->help_dialog.display.helpFile = XtNewString(hw->help_dialog.display.helpFile);

  /*
   * Initialize the topic title variables.
   */
  hw->help_dialog.display.count = 1;            
  hw->help_dialog.display.topicTitleLbl = NULL;
  if (hw->help_dialog.display.topicTitleStr != NULL)
    hw->help_dialog.display.topicTitleStr =
			XtNewString(hw->help_dialog.display.topicTitleStr);
  else
    hw->help_dialog.display.topicTitleStr =
			XtNewString((char *)_DTGETMESSAGE(2, 70,
						"Nonexistent Topic Title."));
  /*
   * create the XmString version
   */
  if (hw->help_dialog.display.topicTitleStr != NULL)
    hw->help_dialog.display.topicTitleLbl = XmStringCreateLocalized(
					hw->help_dialog.display.topicTitleStr);

  if (hw->help_dialog.print.printer != NULL)
    hw->help_dialog.print.printer = XtNewString(hw->help_dialog.print.printer);

  if (hw->help_dialog.print.helpPrint != _DtHelpDefaultHelpPrint)
    hw->help_dialog.print.helpPrint = XtNewString(hw->help_dialog.print.helpPrint);


  hw->bulletin_board.auto_unmanage  = FALSE;
  hw->bulletin_board.resize_policy  = XmRESIZE_NONE;
 

  /* Setup some of our generic widget variables */
  hw->help_dialog.print.printVolume = NULL;


  /* Set our volume handle to an NULL initial value */
  hw->help_dialog.display.volumeHandle       = NULL;
  hw->help_dialog.ghelp.volumeFlag         = FALSE;

  _DtHelpGlobSrchInitVars(&hw->help_dialog.srch);


  /* Set our print display stuff to initial values */
  hw->help_dialog.print.printForm      = NULL;

  /* Set our map flag: true after we hit our popup callback, 
   * false otherwise
   */
  hw->help_dialog.display.firstTimePopupFlag = FALSE;

  
  /* Set our history display stuff to initial vlaues */
  hw->help_dialog.history.historyWidget     = NULL;
  hw->help_dialog.history.volumeList = NULL;
  hw->help_dialog.history.topicList  = NULL;
  hw->help_dialog.history.pHistoryListHead  = NULL;

  /* Set our path display stuff to initial values */
  hw->help_dialog.history.pPathListHead     = NULL; 
  hw->help_dialog.history.pPathListTale     = NULL;
  hw->help_dialog.history.totalPathNodes    = 0;

  /* Set our jump list display stuff to initial values */
  hw->help_dialog.backtr.pJumpListHead     = NULL;
  hw->help_dialog.backtr.pJumpListTale     = NULL;
  hw->help_dialog.backtr.totalJumpNodes    = 0;
  hw->help_dialog.backtr.scrollPosition    = -1;
  /* Set our help dialog widgets to NULL starting values */
  hw->help_dialog.menu.menuBar        = NULL;
  hw->help_dialog.browser.panedWindow    = NULL;
  hw->help_dialog.browser.pathArea       = NULL;
  hw->help_dialog.menu.topBtn         = NULL;
  hw->help_dialog.menu.popupTopBtn    = NULL;
  hw->help_dialog.menu.keyBtn         = NULL;
  hw->help_dialog.menu.backBtn        = NULL;
  hw->help_dialog.menu.popupBackBtn   = NULL;
  hw->help_dialog.menu.historyBtn     = NULL;
  hw->help_dialog.menu.printBtn       = NULL;
  hw->help_dialog.menu.closeBtn       = NULL;
  hw->help_dialog.menu.helpBtn        = NULL;
  hw->help_dialog.ghelp.definitionBox  = NULL;
  hw->help_dialog.menu.newWindowBtn  = NULL;


  hw->help_dialog.browser.btnBoxBackBtn    = NULL;
  hw->help_dialog.browser.btnBoxHistoryBtn = NULL;
  hw->help_dialog.browser.btnBoxIndexBtn   = NULL;
 
  /* Set our parentId to a null starting value */
  hw->help_dialog.ghelp.parentId       = NULL;
}


/*****************************************************************************
 * Function:	    static void Initialize (
 *                      WidgetClass widgetClass)
 *
 * Parameters:      WidgetClass      
 *
 *
 * Return Value:    Void.
 *
 * Purpose: 	    This is the Help Dialog widget initialize routine. This
 *                  routine is responsible for the following:
 *                      1) Validate all resources the user passed in.
 *                      2) Over ride any invalid resources.
 *                      3) Build the internal UI component for the Help Dialog.
 *                      4) Add any internal callbacks for the UI components.
 *
 *****************************************************************************/
static void Initialize(
    Widget rw,
    Widget nw,
    ArgList args_init,
    Cardinal *num_args)
{

  Arg		args[10];	/*  arg list		*/
  int      	n;		/*  arg count		*/
  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;

  /* Local variables */
  DtHelpListStruct *pHelpInfo;
 
  /* Initialize all global variables */
  VariableInitialize(hw);


  /* Validate the incomming arguments to make sure they are ok */


  /* Build the Menus bar */
  hw->help_dialog.menu.menuBar = SetupHelpDialogMenus((Widget)hw);


  /* Build the Paned Window to hold the path area and display area */
  n = 0;
  XtSetArg (args[n], XmNspacing, 10);  ++n;
  hw->help_dialog.browser.panedWindow = 
                 XmCreatePanedWindow ((Widget)hw, "panedWindow", args, n);
  XtManageChild (hw->help_dialog.browser.panedWindow);


  /* Call the setup routine to build the actual path area */
  _DtHelpBuildPathArea(hw->help_dialog.browser.panedWindow, hw);


  /* Force our keyboard traversal to start within the path area */
  /*
   * NOTE 04/25/96 : pathArea is NOT set by _DtHelpBuildPathArea().
   * The test for NULL is a work-around for defect CDExc20246; but
   * the proper widget should be given to XmProcessTraversal() someday.
   */
  if (hw->help_dialog.browser.pathArea != (Widget)NULL)
    XmProcessTraversal(hw->help_dialog.browser.pathArea, XmTRAVERSE_HOME);

  /* We build our display area last because we need the other UI 
   * components created first.
   */
  
  /* Call the setup routine to build the display area */
  BuildDisplayArea(hw->help_dialog.browser.panedWindow, hw);
 
  
  /* Add the proper help callback to the top level shell of our dialog */

  /* Add all our help callbacks for each of the created widgets */
  pHelpInfo = _DtHelpListAdd(DtHELP_dialogShell_STR,
                         (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) hw, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);


   /* Just for fun, lets make sure our sizes are correct */
   XtAddCallback (XtParent(hw), XmNpopupCallback, (XtCallbackProc)
                 InitialPopupCB, (XtPointer) hw);


  return;
}



/****************************************************************************
 * Function:	    static XtCallbackProc InitialPopupCB
 *                   
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    We do some last minute sizing of our dialog on its first
 *                  mapping, then we remove the callback.
 *
 ****************************************************************************/
static void InitialPopupCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
 
   /* set our firstTimePopupFlag to TRUE because we map it right after
     this call */
   hw->help_dialog.display.firstTimePopupFlag = TRUE;

  _DtHelpResizeDisplayArea (XtParent(hw),
			     hw->help_dialog.help.pDisplayArea, 
                             hw->help_dialog.display.textRows,
                             hw->help_dialog.display.textColumns);

  XtRemoveCallback (XtParent(hw), XmNpopupCallback, (XtCallbackProc)
                  InitialPopupCB, (XtPointer) hw);


}




/*****************************************************************************
 * Function:	    static Boolean SetValues(
 *                             Widget cw,
 *                             Widget rw,
 *                             Widget nw,
 *                             ArgList args,
 *                             Cardinal *num_args )
 *
 * Parameters:      cw       Specifies the current working widget.
 *                  rw       Specifies the replacement working widget.
 *                  nw       Specifies the new widget.
 *                  args     Specifies the arguments to be applied to the
 *                           New widget.
 *                  numArgs  Number of argument/value pars in args.
 *
 * Return Value:    
 *
 * Purpose: 	    Set the attributes of the Help Dialog widget.

 *
 *****************************************************************************/
static Boolean SetValues(
    Widget cw,
    Widget rw,
    Widget nw,
    ArgList args,
    Cardinal *numArgs)
{
  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;
  DtHelpDialogWidget current = (DtHelpDialogWidget) cw ;
  Boolean updateRequest=FALSE;
  Boolean newTitle=FALSE;

  /* Validate the incomming arguments to make sure they are ok */



  /* Setup some initial argument values we know we need on the B-board */
  hw->bulletin_board.auto_unmanage  = FALSE;
  hw->bulletin_board.resize_policy  = XmRESIZE_NONE;


  /* Check DtNcolumns & or DtNrows  resource for change */
  if ((current->help_dialog.display.textRows != hw->help_dialog.display.textRows) ||
      (current->help_dialog.display.textColumns != hw->help_dialog.display.textColumns))
    {
      /* Perform a resize on our display area */
      _DtHelpResizeDisplayArea (XtParent(hw),
			     hw->help_dialog.help.pDisplayArea, 
                             hw->help_dialog.display.textRows,
                             hw->help_dialog.display.textColumns);
    }


  /* Check DtNhelpVolume resource for change */
  if (current->help_dialog.display.helpVolume != hw->help_dialog.display.helpVolume) 
    {
      hw->help_dialog.display.helpVolume = XtNewString(hw->help_dialog.display.helpVolume);
      XtFree(current->help_dialog.display.helpVolume);

      /* Update our volumeFlag so we know to resolve our new help volume */
      hw->help_dialog.ghelp.volumeFlag=FALSE;

      updateRequest = TRUE;
    }


  /* Check DtNlocationId resource for change */
  if (current->help_dialog.display.locationId != hw->help_dialog.display.locationId) 
    {
      hw->help_dialog.display.locationId = XtNewString(hw->help_dialog.display.locationId);
      XtFree(current->help_dialog.display.locationId);
      updateRequest = TRUE;
    }

   /* set the printing resources */
   _DtHelpPrintSetValues(&current->help_dialog.print,&hw->help_dialog.print,
             &hw->help_dialog.display,&hw->help_dialog.help);

  /* Check XmhelpOnHelpVolume resource for change */
  if (current->help_dialog.help.helpOnHelpVolume !=
                   hw->help_dialog.help.helpOnHelpVolume) 
    {
      hw->help_dialog.help.helpOnHelpVolume =
                           XtNewString(hw->help_dialog.help.helpOnHelpVolume);

      if (current->help_dialog.help.helpOnHelpVolume != _DtHelpDefaultHelp4HelpVolume)
         XtFree(current->help_dialog.help.helpOnHelpVolume);
    }    

  /* Check DtNmanPage resource for change */
  if (current->help_dialog.display.manPage != hw->help_dialog.display.manPage) 
    {
      hw->help_dialog.display.manPage = XtNewString(hw->help_dialog.display.manPage);
      XtFree(current->help_dialog.display.manPage);
      updateRequest = TRUE;
      newTitle      = TRUE;
    }


  /* Check DtNstringData resource for change */
  if (current->help_dialog.display.stringData != hw->help_dialog.display.stringData) 
    {
      hw->help_dialog.display.stringData = XtNewString(hw->help_dialog.display.stringData);
      XtFree(current->help_dialog.display.stringData);
      updateRequest = TRUE;
      newTitle      = TRUE;
    }

  /* Check DtNhelpFile resource for change */
  if (current->help_dialog.display.helpFile != hw->help_dialog.display.helpFile) 
    {
      hw->help_dialog.display.helpFile = XtNewString(hw->help_dialog.display.helpFile);
      XtFree(current->help_dialog.display.helpFile);
      updateRequest = TRUE;
      newTitle      = TRUE;
    }

  /* Check DtNtopicTitle resource for change */
  if (hw->help_dialog.display.topicTitleStr != NULL &&
	current->help_dialog.display.topicTitleStr !=
					hw->help_dialog.display.topicTitleStr) 
    {
      /*
       * copy the string and make a XmString version.
       */
      hw->help_dialog.display.topicTitleStr =
			XtNewString(hw->help_dialog.display.topicTitleStr);

      XtFree(current->help_dialog.display.topicTitleStr);
      updateRequest = TRUE;
      newTitle      = TRUE;
    }
  else if (newTitle == True)
    {
      /*
       * A new file, string or man page has been specified.
       * But XmNtopicTitle has not been set or set to null (otherwise
       * the if stmt would have been true). Therefore, create a new
       * title for this entry for history and topic labels.
       */
      int len;
      char *str;

      str = (char *)_DTGETMESSAGE(2, 70, "Nonexistent Topic Title.");
      len = strlen(str);
      hw->help_dialog.display.topicTitleStr = XtMalloc(sizeof(char)*(len+10));
      if (NULL != hw->help_dialog.display.topicTitleStr)
	{
          sprintf(hw->help_dialog.display.topicTitleStr, "%s%d", str, 
		  hw->help_dialog.display.count++);
          newTitle = TRUE;
        }
    }

  /*
   * Turn the string into an XmString and free the old
   */
  if (newTitle == TRUE)
    {
      hw->help_dialog.display.topicTitleLbl = XmStringCreateLocalized(
					hw->help_dialog.display.topicTitleStr);

      if (current->help_dialog.display.topicTitleLbl != NULL)
          XmStringFree(current->help_dialog.display.topicTitleLbl);
    }

  /* Check the help type for change */
  if (current->help_dialog.display.helpType != hw->help_dialog.display.helpType
	|| updateRequest)
    {
      /* Setup and display our new topic */
      _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_AND_JUMP);
    }


  return(FALSE);
}





/*****************************************************************************
 * Function:	    static void Destroy(Widget w );
 *
 * Parameters:      w       Specifies the widget to be  destroyed.
 *
 * Return Value:    
 *
 * Purpose: 	    Destroy any internally malloced memory.
 *
 *****************************************************************************/
static void Destroy(
    Widget w)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) w;

     /* This routine will clean up all malloc'ed stuff in our instance 
      * structure.  It does not remove any callbacks or delete any of the
      * widgets created in this instance of the help dialog.
      */

     CleanUpHelpDialog((Widget) hw, DtCLEAN_FOR_DESTROY);
 
     /* Remove any of the callbacks added to the help dialog ??? */


}


/*****************************************************************************
 * Function:	    static void MenuBarFix( 
 *                            XmGeoMatrix geoSpec,
 *                            int action,
 *                            XmGeoRowLayout layoutPtr,
 *                            XmKidGeometry rowPtr)
 *                            
 *                         
 *
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	  This routine is a fixup routine which can be used for rows 
 *                which consist of a single MenuBar RowColumn.  The effect of
 *                this routine is to have the RowColumn ignore the margin
 *                width and height.
 *
 *****************************************************************************/
static void MenuBarFix(
    XmGeoMatrix geoSpec,
    int action,
    XmGeoMajorLayout layoutPtr,
    XmKidGeometry rowPtr )
{
    register Dimension       marginW ;
    register Dimension       marginH ;
    register Dimension       twoMarginW ;


    marginW = geoSpec->margin_w ;
    twoMarginW = (marginW << 1) ;
    marginH = geoSpec->margin_h ;

    switch(    action    )
    {
        case XmGEO_PRE_SET:
        {   rowPtr->box.x -= marginW ;
            rowPtr->box.width += twoMarginW ;
            rowPtr->box.y -= marginH ;
            break ;
            }
        default:
        {   if(    rowPtr->box.width > twoMarginW    )
            {
                /* Avoid subtracting a margin from box width which would
                *   result in underflow.
                */
                rowPtr->box.x += marginW ;
                rowPtr->box.width -= twoMarginW ;
                }
            if(    action == XmGET_PREFERRED_SIZE    )
            {
                /* Set width to some small value so it does not
                *   effect total width of matrix.
                */
                rowPtr->box.width = 1 ;
                }
            break ;
            }
        }
    return ;
} 



/*****************************************************************************
 * Function:	     XmGeoMatrix _DtHelpDialogWidgeGeoMatrixCreate(
 *                           Widget wid,
 *                           Widget instigator,
 *                           XtWidgetGeometry *desired) 
 *
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	  This routine is responsible for all the positioning of the
 *                the internal Help Dialog widgets.
 *
 *****************************************************************************/
XmGeoMatrix _DtHelpDialogWidgetGeoMatrixCreate(
    Widget wid,
    Widget instigator,
    XtWidgetGeometry *desired )
{
 

             DtHelpDialogWidget hw = (DtHelpDialogWidget) wid ;
             XmGeoMatrix     geoSpec ;
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   boxPtr ;

    geoSpec = _XmGeoMatrixAlloc( TB_MAX_WIDGETS_VERT, TB_MAX_NUM_WIDGETS, 0) ;
    geoSpec->composite = (Widget) hw ;
    geoSpec->instigator = (Widget) instigator ;
    if(    desired    )
    {   geoSpec->instig_request = *desired ;
        } 
    geoSpec->margin_w = hw->help_dialog.ghelp.marginWidth
                        + hw->manager.shadow_thickness ;
    geoSpec->margin_h = hw->help_dialog.ghelp.marginHeight
                        + hw->manager.shadow_thickness ;
    geoSpec->no_geo_request = _DtHelpDialogWidgetNoGeoRequest ;

    layoutPtr = (XmGeoRowLayout) geoSpec->layouts ;
    boxPtr = geoSpec->boxes ;

    /* Menubar setup stuff */
      if( _XmGeoSetupKid( boxPtr, hw->help_dialog.menu.menuBar))
       {   
         layoutPtr->fix_up = MenuBarFix;
         boxPtr += 2;     
          ++layoutPtr; 
       } 
     
    
    /* Display area setup stuff */
    if( _XmGeoSetupKid(boxPtr, hw->help_dialog.browser.panedWindow))
    {   
        layoutPtr->space_above = 0;
        layoutPtr->stretch_height = TRUE ;
        layoutPtr->min_height = 100 ;
        boxPtr += 2 ;       /* For new row, add 2. */
        ++layoutPtr ;       /* For new row. */
        } 

 

    layoutPtr->space_above = hw->help_dialog.ghelp.marginWidth;
    layoutPtr->end = TRUE ;        /* Mark the last row. */
    return( geoSpec) ;
}




/*****************************************************************************
 * Function:	     Boolean _DtHelpDialogWidgetNoGeoRequest(geoSpec)
 *                          XmGeoMatrix geoSpec)
 *
 *
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	  
 *
 ****************************************************************************/
Boolean _DtHelpDialogWidgetNoGeoRequest(XmGeoMatrix geoSpec)
{

  if(    BB_InSetValues( geoSpec->composite)
      && (XtClass( geoSpec->composite) == dtHelpDialogWidgetClass)    )
    {   
      return( TRUE) ;
    } 
  return( FALSE) ;
}




/*****************************************************************************
 * Function:	   static void DisplayTopLevelCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   causes the top level topic to be displayed.
 *
 ****************************************************************************/
static void  DisplayTopLevelCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) client_data;
    
   /* Display the top level topic */
   if (hw->help_dialog.help.topLevelId != NULL)
     _DtHelpUpdateDisplayArea(hw->help_dialog.help.topLevelId, hw, TRUE,
                           DtHISTORY_AND_JUMP, DtHELP_TYPE_TOPIC);
   else
     _DtHelpUpdateDisplayArea(NULL, hw, TRUE, DtHISTORY_AND_JUMP,
                           DtHELP_TYPE_TOPIC); 

}

  


/*****************************************************************************
 * Function:	   extern void _DtHelpDisplayBackCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the top element in the jump
 *                 stack to be displayed.
 *
 ****************************************************************************/
void   _DtHelpDisplayBackCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data)
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) client_data;

   ProcessJumpBack(hw);

}





/*****************************************************************************
 * Function:	   extern void _DtHelpDuplicateWindowCB(
 *                             Widget w,   
 *                             XtPointer client_data,
 *                             XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine creates a new help widget by forcing the
 *                 equivelent of a Jump New hyper text call
 *
 ****************************************************************************/
void  _DtHelpDuplicateWindowCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget	hw = (DtHelpDialogWidget) clientData;
   DtHelpDialogCallbackStruct callDataStruct;
  

  /* Setup the DtHelpHyperProcStruct to pass back 
   * to the client as callData
   */
        
    _DtHelpTurnOnHourGlass(XtParent(hw));

    if (hw->help_dialog.display.hyperLinkCallback != NULL)
    {
         /* Use the old one */
         callDataStruct.helpVolume     = XtNewString
                                              (hw->help_dialog.display.helpVolume);
         callDataStruct.reason         = DtCR_HELP_LINK_ACTIVATE;
         callDataStruct.event          = NULL; /* should be hyperData->event */
         callDataStruct.locationId     = XtNewString
                                             (hw->help_dialog.display.locationId);
         callDataStruct.specification  = NULL;
         callDataStruct.hyperType      = DtHELP_LINK_TOPIC;
         callDataStruct.windowHint     = DtHELP_NEW_WINDOW;

         XtCallCallbackList((Widget)hw,hw->help_dialog.display.hyperLinkCallback,
                             &callDataStruct);
    }
    else
    {   /* The application did not register a hypertext callback so
         *  we must generate the proper warning message and continue!
         */
              XmeWarning((Widget)hw, (char*) HDMessage6); 
    }

    _DtHelpTurnOffHourGlass(XtParent(hw));       

}
 


/*****************************************************************************
 * Function:	   static void CopyTextCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the display area to copy text to 
 *                 the clip-board.
 *
 ****************************************************************************/
static void  CopyTextCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) clientData;

   _DtHelpInitiateClipboard(hw->help_dialog.help.pDisplayArea);


}




/*****************************************************************************
 * Function:	   static void ProcessBMenuBackCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the top element in the jump
 *                 stack to be displayed, if their is a top element.
 *
 ****************************************************************************/
static void  ProcessBMenuBackCB(
    Widget w,
    XtPointer clientData,
    XButtonEvent * event,
    Boolean *continueToDispatch)
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) clientData;

   XmMenuPosition(hw->help_dialog.menu.popupMenu, event);
   XtManageChild(hw->help_dialog.menu.popupMenu);

}
 


/*****************************************************************************
 * Function:	   extern void  _DtHelpDisplayHistoryCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the history dialog to be 
 *                 posted.
 *
 ****************************************************************************/
void   _DtHelpDisplayHistoryCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) client_data;
 

   XmUpdateDisplay((Widget) hw);
   _DtHelpTurnOnHourGlass(XtParent(hw));

   _DtHelpDisplayHistoryInfo((Widget)hw);

   _DtHelpTurnOffHourGlass(XtParent(hw));      
  
}






/*****************************************************************************
 * Function:	       void  _DtHelpDisplayBrowserHomeCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the top level of the browser.hv 
 *                 help volume to be displayed.
 *
 ****************************************************************************/
void   _DtHelpDisplayBrowserHomeCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) client_data;
   XmUpdateDisplay((Widget) hw);

   /* ??? Check to see if help volume is always fully expanded at this point
    * ??? if so, we will never have a match.
    */
   if (   hw->help_dialog.display.helpVolume == NULL
       || strstr (hw->help_dialog.display.helpVolume, DtBROWSER_NAME) == NULL) 
     {
       XtFree(hw->help_dialog.display.helpVolume);
       hw->help_dialog.display.helpVolume  = XtNewString(DtBROWSER_NAME);
     
       /* Set our help volume flag so we open the proper volume */
       hw->help_dialog.ghelp.volumeFlag         = FALSE;
     }

   XtFree(hw->help_dialog.display.locationId);
   hw->help_dialog.display.locationId = XtNewString(DtBROWSER_HOME);
   hw->help_dialog.display.helpType = DtHELP_TYPE_TOPIC;

   _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_AND_JUMP);   
}






/*****************************************************************************
 * Function:	       void _DtHelpDisplayIndexCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the index dialog to be 
 *                 posted.
 *
 ****************************************************************************/
void  _DtHelpDisplayIndexCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

   DtHelpDialogWidget	hw = (DtHelpDialogWidget) client_data;
    
   
   XmUpdateDisplay((Widget) hw);
   _DtHelpTurnOnHourGlass(XtParent(hw));

   /* display the index search dialog */
   _DtHelpGlobSrchDisplayDialog((Widget)hw,NULL,hw->help_dialog.display.helpVolume);
   
   _DtHelpTurnOffHourGlass(XtParent(hw));       
      
}




/*****************************************************************************
 * Function:	   static void DisplayPrintCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine will cause the history dialog to be 
 *                 posted.
 *
 ****************************************************************************/
static void  DisplayPrintCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

  DtHelpDialogWidget	hw = (DtHelpDialogWidget) client_data;

  XmUpdateDisplay((Widget) hw);
  _DtHelpTurnOnHourGlass(XtParent(hw));
  
  /* display the print dialog */
  _DtHelpDisplayPrintDialog((Widget) hw,&hw->help_dialog.print,
		&hw->help_dialog.display,&hw->help_dialog.help);

  _DtHelpTurnOffHourGlass(XtParent(hw));      
}




/*****************************************************************************
 * Function:	    Widget DtCreateHelpDialog(Widget parent,
 *                                             String name,
 *                                             ArgList arglist,
 *                                             Cardinal argcount);
 *
 * Parameters:      parent      Specifies the parent widget ID.
 *                  name        Specifies the name of the created dialog
 *                              shell widget.
 *                  arglis      Specifies the argument list.
 *                  argcount    Specifies the number of attribute/value pairs
 *                              in the argument list (arglist).
 *
 * Return Value:    Returns a Help Dialog shell widget ID.
 *
 * Purpose: 	    Create an instance of a Help Dialog.
 *
 *****************************************************************************/
Widget DtCreateHelpDialog(
    Widget parent,
    char *name,
    ArgList al,
    Cardinal ac)
{
    Widget w;
    _DtHelpWidgetToAppContext(parent);
    
    _DtHelpAppLock(app);
    w = XmeCreateClassDialog (dtHelpDialogWidgetClass, parent, name, al, ac);

    /* Add the CatchClose here so we catch the window manager close requests */
    CatchClose(w);

    _DtHelpAppUnlock(app);
    return w;
}



/*****************************************************************************
 * Function:	    _DtHelpFilterExecCmdCB
 *
 *  clientData:     The general help dialog widget
 *  cmdStr:         cmd string to filter
 *  ret_filteredCmdStr:  string after filtering.  NULL if exec denied
 *
 * Return Value:    0: ok, < 0: error
 *
 * Purpose: 	    filter an execution command using executionPolicy rsrc
 *
 * Memory:
 *   The caller must free memory allocated for the ret_filteredCmdStr
 *****************************************************************************/
int _DtHelpFilterExecCmdCB(
    void *   clientData,
    const char *   cmdStr,
    char * * ret_filteredCmdStr)
{
     DtHelpDialogWidget    hw;
     char    *hv_path;

     hw = (DtHelpDialogWidget) _DtHelpDisplayAreaData(clientData);
     hv_path = _DtHelpFileLocate(DtHelpVOLUME_TYPE,
				 hw->help_dialog.display.helpVolume,
				 _DtHelpFileSuffixList, False, R_OK);

     return _DtHelpFilterExecCmd((Widget) hw, cmdStr, 
                 hw->help_dialog.display.executionPolicy, 
                 True, &hw->help_dialog.help, ret_filteredCmdStr, hv_path);
}


/*****************************************************************************
 * Function:	    void BuildDisplayArea(Widget parent,
 *                                 DtHelpDialogWidget nw);  
 *
 *
 * Parameters:      parent      Specifies the widget ID of the help dialog you
 *                              want to set the topic in.
 *                  nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Creates the Display area in the help dialog.
 *
 *****************************************************************************/
static void BuildDisplayArea(
    Widget parent,
    DtHelpDialogWidget nw)
{
  Arg	args[2];	
  int   n;
  XmFontList  defaultList;
  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;


  /* Get our current fontlist value */
  n = 0;
  XtSetArg (args[n], XmNfontList, &(defaultList));  ++n;
  XtGetValues (hw->help_dialog.menu.topBtn, args, n);


  /* Build the Display Area */
  hw->help_dialog.help.pDisplayArea = _DtHelpCreateDisplayArea(parent,
                                   "DisplayArea",
                                    ((short) hw->help_dialog.display.scrollBarPolicy),
                                    ((short) hw->help_dialog.display.scrollBarPolicy),
				    False,
                                    ((int) hw->help_dialog.display.textRows),
                                    ((int) hw->help_dialog.display.textColumns),
                                    _DtHelpDialogHypertextCB,
                                    ResizeHelpDialogCB,
                                    _DtHelpFilterExecCmdCB,
                                    (XtPointer) hw,
                                    defaultList);

  /* Build the popup menu item for inside the display area */
  hw->help_dialog.menu.popupMenu =  CreatePopupMenu((Widget)hw);


  /* Determin what type of info we are going to display */
  _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_AND_JUMP);
  
}






/*****************************************************************************
 * Function:	    void _DtHelpSetupDisplayType(DtHelpDialogWidget nw,
 *                                            int updateType);  
 *
 *
 * Parameters:      nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins the type of topic the user want's to display
 *                  in the current help dialog and sets it up for display.
 *
 *****************************************************************************/
void _DtHelpSetupDisplayType(
     DtHelpDialogWidget nw,
     Boolean vol_changed,
     int updateType)
{
   char                *templocationId;
   Boolean validAccessPath=FALSE;
   XtPointer           topicHandle;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;
   int                 status=VALID_STATUS;
   char                *tmpMessage;
   char                *tmpError;
   char                *locTitle;
   char                *volumeTitle;
   DtTopicListStruct  *tmpPtr;
   Boolean             adjustTopicTitle=TRUE;
                      /* This is a local flag that we use to determine 
                       * if we need to re-adjust the topic volume title 
                       * at the end of this 
                       *  Function: True == Modify title, False == leave alone.
                       */
   Boolean didWeHaveAnError=FALSE;
   XmString      labelString;
   Arg		 args[10];
   int      	 n=0;


   /* Here we need to store away our current scroll position as the currentlly
    * displayed item is about replaced w/a new item.
    * We already have placed this item in the jump stack, and now are just adding
    * the proper value for our scrollPosition.
    */

   if (hw->help_dialog.backtr.pJumpListHead != NULL)
     {
       tmpPtr = hw->help_dialog.backtr.pJumpListHead;
       tmpPtr->scrollPosition = 
                     _DtHelpGetScrollbarValue(hw->help_dialog.help.pDisplayArea);
     }


   /* Here we set our UP, and Home Topic buttons to FALSE, they should be
    * true then the following code will set them correctly.
    */
   XtSetSensitive(hw->help_dialog.menu.topBtn, FALSE);
   XtSetSensitive(hw->help_dialog.menu.popupTopBtn, FALSE);
	

   switch (hw->help_dialog.display.helpType)
    {
      case DtHELP_TYPE_TOPIC:

        /* Expand the helpVolume resource if needed */
        if ((hw->help_dialog.ghelp.volumeFlag) &&
           (hw->help_dialog.display.volumeHandle != NULL))
           validAccessPath = TRUE;
        else
          if (hw->help_dialog.display.helpVolume != NULL)
             validAccessPath = _DtHelpExpandHelpVolume((Widget)hw,
					&hw->help_dialog.display,
					&hw->help_dialog.help,
					&hw->help_dialog.print);
  
        /* If the developer gave us a valid Access Path then we can continue
         * else, we will do nothing
         */
        if (validAccessPath)
          {
#if 0
             /* Update our keyword search dialog if needed */
             if ( FALSE == hw->help_dialog.ghelp.volumeFlag)
                 _DtHelpGlobSrchUpdateCurVol((Widget) hw);

       
             /* We now set this to true after we update our volumeTitle
              * value in _DtHelpUpdateDisplayArea.
              */
          
             /* hw->help_dialog.ghelp.volumeFlag         = TRUE;*/
#endif         

             /* Setup all the proper lists and display area with the 
              * topic string 
              */
             templocationId = XtNewString(hw->help_dialog.display.locationId);

             /* Set the initial locationId into the display area */
             _DtHelpUpdateDisplayArea(templocationId, hw, vol_changed, updateType,
                                   DtHELP_TYPE_TOPIC);
             XtFree(templocationId);
             adjustTopicTitle = FALSE;

          }
        else
          {
             if (hw->help_dialog.display.helpVolume == NULL)
               {
                  didWeHaveAnError=TRUE;
                  tmpError = XtNewString((char *)_DTGETMESSAGE(2, 60,
                            "No help volume specified."));
                  _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, 
                            (Widget)hw, tmpError, NULL);

                  _DtHelpUpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING,
                                     (Widget) hw);
                  hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
                  hw->help_dialog.display.stringData = XtNewString(tmpError);
                  XtFree(tmpError);
               }
           else
	       {
                 /* We cannot find our HelpVolume so display the proper error
                  * message in the help dialog and continue.
                  */
                 didWeHaveAnError=TRUE;
                 tmpMessage = XtNewString((char *)_DTGETMESSAGE(2, 58,
                                "The requested online help is either not installed "
				"or not in the proper help search path.  For "
				"information on installing online help, consult "
				"the documentation for the product.\n"));
                 locTitle = XtNewString((char *)_DTGETMESSAGE(2, 62,
                                        "Location ID:"));
                 volumeTitle = XtNewString((char *)_DTGETMESSAGE(2, 61,
                                        "Help Volume:"));
                 tmpError = XtMalloc(strlen(tmpMessage) +
                                     strlen(locTitle) +
                                     strlen(volumeTitle) + 
                                     strlen(hw->help_dialog.display.locationId) +
                                     strlen(hw->help_dialog.display.helpVolume) + 4);
                 (void) strcpy(tmpError, tmpMessage);
                 (void) strcat(tmpError, volumeTitle);
                 (void) strcat(tmpError, " ");
                 (void) strcat(tmpError, hw->help_dialog.display.helpVolume);
                 (void) strcat(tmpError,"\n");
                 (void) strcat(tmpError, locTitle);
                 (void) strcat(tmpError, " ");
                 (void) strcat(tmpError, hw->help_dialog.display.locationId);

                 /* Set current path area to null by giving it a null id */
                 _DtHelpUpdatePathArea(NULL, hw);

                 _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, 
                                        (Widget)hw, tmpError, NULL);

                 _DtHelpUpdateJumpList(tmpError,DtHELP_TYPE_DYNAMIC_STRING,
                                    (Widget) hw);
                 hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
                 hw->help_dialog.display.stringData = XtNewString(tmpError);

                 XtFree(tmpError);
                 XtFree(tmpMessage);
                 XtFree(locTitle);
                 XtFree(volumeTitle);
  
	       }
          } 
      
        break;

      case DtHELP_TYPE_STRING:


         _DtHelpUpdatePathArea(NULL, hw); 

        /* Set the string to the current help dialog */
        status = _DtHelpFormatAsciiString(hw->help_dialog.help.pDisplayArea,
					hw->help_dialog.display.stringData,
                                       &topicHandle);
        if (status >= VALID_STATUS)
          {
     
            /* Update the History and Jump Lists */
            if (updateType == DtHISTORY_AND_JUMP)
              _DtHelpUpdateJumpList(hw->help_dialog.display.stringData,
                                 DtHELP_TYPE_STRING, (Widget) hw);
        
  
            _DtHelpUpdateHistoryList(hw->help_dialog.display.stringData,
                              DtHELP_TYPE_STRING, TRUE, (Widget)hw);
            
            _DtHelpDisplayAreaSetList (hw->help_dialog.help.pDisplayArea,
                                    topicHandle, FALSE, 
                                    hw->help_dialog.backtr.scrollPosition);
          }
        else
          {
            didWeHaveAnError=TRUE;
            tmpError = XtNewString((char *)_DTGETMESSAGE(2, 50,
                       "String data could not be formatted."));

            _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, (Widget)hw,
                                   tmpError, (char*)HDMessage0);
          
            _DtHelpUpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                               (Widget) hw);
            hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
            hw->help_dialog.display.stringData = XtNewString(tmpError);
            XtFree(tmpError);
          }

        break;

      case DtHELP_TYPE_DYNAMIC_STRING:

        _DtHelpUpdatePathArea(NULL, hw);

        /* Set the string to the current help dialog */
        status = _DtHelpFormatAsciiStringDynamic(
				    hw->help_dialog.help.pDisplayArea,
				    hw->help_dialog.display.stringData,
                                    &topicHandle);
            
        if (status >= VALID_STATUS)
          {
     
            /* Update the History and Jump Lists */
            if (updateType == DtHISTORY_AND_JUMP)
              _DtHelpUpdateJumpList(hw->help_dialog.display.stringData,
                                 DtHELP_TYPE_DYNAMIC_STRING, (Widget) hw);
        
            _DtHelpUpdateHistoryList(hw->help_dialog.display.stringData,
                              DtHELP_TYPE_DYNAMIC_STRING, TRUE, (Widget)hw);
            _DtHelpDisplayAreaSetList (hw->help_dialog.help.pDisplayArea,
                                    topicHandle, FALSE,
                                    hw->help_dialog.backtr.scrollPosition);
          }
        else
          {
            didWeHaveAnError=TRUE;
            tmpError = XtNewString((char *)_DTGETMESSAGE(2, 51,
                  "Dynamic string data could not be formatted."));

            _DtHelpUpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                               (Widget) hw);
        
            hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
            hw->help_dialog.display.stringData = XtNewString(tmpError);

            _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea,(Widget)hw,
                                    tmpError, (char*) HDMessage1);
            XtFree(tmpError);
	   }
         break;

      case DtHELP_TYPE_MAN_PAGE:

        _DtHelpUpdatePathArea(NULL, hw);

        /* Set the string to the current help dialog */
        status = _DtHelpFormatManPage(
				hw->help_dialog.help.pDisplayArea,
				hw->help_dialog.display.manPage, &topicHandle);
      
        if (status >= VALID_STATUS)
         {
            /* Update the History and Jump Lists */

            if (updateType == DtHISTORY_AND_JUMP)
              _DtHelpUpdateJumpList(hw->help_dialog.display.manPage,
                                 DtHELP_TYPE_MAN_PAGE, (Widget)hw);
    
              _DtHelpUpdateHistoryList(hw->help_dialog.display.manPage,
                               DtHELP_TYPE_MAN_PAGE, TRUE, (Widget)hw);
              _DtHelpDisplayAreaSetList (hw->help_dialog.help.pDisplayArea,
                                      topicHandle, FALSE, 
                                      hw->help_dialog.backtr.scrollPosition);
          }
        else
          {
             didWeHaveAnError=TRUE;
             tmpError = XtNewString((char *)_DTGETMESSAGE(2, 52,
                                    "Man Page could not be formatted. The requested Man Page is either not present, or corrupt."));

             _DtHelpUpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING,
                                (Widget) hw);
             hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
             hw->help_dialog.display.stringData = XtNewString(tmpError);

             _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, (Widget)hw,
                tmpError, (char*) HDMessage2);
             XtFree(tmpError);

	  }
	 break;

      case DtHELP_TYPE_FILE:

        _DtHelpUpdatePathArea(NULL, hw);
      
        /* Set the string to the current help dialog */
        status = _DtHelpFormatAsciiFile(hw->help_dialog.help.pDisplayArea,
				hw->help_dialog.display.helpFile, &topicHandle);
           
        if (status >= VALID_STATUS)
          {
            /* Update the History and Jump Lists */

            if (updateType == DtHISTORY_AND_JUMP)
              _DtHelpUpdateJumpList(hw->help_dialog.display.helpFile,
                                 DtHELP_TYPE_FILE,(Widget)hw);
    
            _DtHelpUpdateHistoryList(hw->help_dialog.display.helpFile,
                                  DtHELP_TYPE_FILE, TRUE, (Widget)hw);
            _DtHelpDisplayAreaSetList (hw->help_dialog.help.pDisplayArea,
                                    topicHandle, FALSE, 
                                    hw->help_dialog.backtr.scrollPosition);
          }
        else
          {
             didWeHaveAnError=TRUE;
             tmpError = XtNewString((char *)_DTGETMESSAGE(2, 53,
                                   "Text file data could not be formatted. The requested text file is either not present, or corrupt."));

             _DtHelpUpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                                (Widget) hw);

             hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
             hw->help_dialog.display.stringData = XtNewString(tmpError);

             _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, (Widget)hw,
                tmpError, (char*) HDMessage3);

             XtFree(tmpError);

	   }
        break;


      default:  

        /* ERROR-MESSAGE */
        /* This means the user used the wrong help type */
        didWeHaveAnError=TRUE;
        tmpError = XtNewString((char *)_DTGETMESSAGE(2, 80,
                              "The specified help type is invalid."));

        _DtHelpUpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                           (Widget) hw);

        hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
        hw->help_dialog.display.stringData = XtNewString(tmpError);

        _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, (Widget)hw,
           tmpError, (char*) HDMessage4);

        XtFree(tmpError);

        break;

    }  /* End Switch Statement */


   if ((adjustTopicTitle == TRUE) && (didWeHaveAnError == FALSE))
     {
       /* We have a valid HelpType other than a volume and 
       * no error condition, so lets update the Topics title.
       */
       XtSetArg(args[0], XmNlabelString, hw->help_dialog.display.topicTitleLbl);
       XtSetValues (hw->help_dialog.browser.volumeLabel, args, 1);
     }
   else if (didWeHaveAnError)
     {
       /* error messages displayed, clear out the topics title */
       n=0;
       labelString = XmStringCreateLocalized(" ");
       XtSetArg (args[n], XmNlabelString, labelString);	            n++;
       XtSetValues (hw->help_dialog.browser.volumeLabel, args, n);
       XmStringFree(labelString); 
       if (hw->help_dialog.history.topicList != NULL)
         XmListDeselectAllItems(hw->help_dialog.history.topicList);
       if (hw->help_dialog.history.volumeList != NULL)
         XmListDeselectAllItems(hw->help_dialog.history.volumeList);

       /* Set current path area to null by giving it a null id */
       _DtHelpUpdatePathArea(NULL, hw);
     }

     /* Update our keyword search dialog */
     _DtHelpGlobSrchUpdateCurVol((Widget) hw);

     /* Update the print dialog */
     _DtHelpUpdatePrintDialog(&hw->help_dialog.print,
		&hw->help_dialog.display,&hw->help_dialog.help,False);

     /* covers error states and non-help topics; case of help topic but
        in/not in the top-level volume is covered elsewhere */
     if (    hw->help_dialog.browser.showTopLevelBtn == TRUE
         && hw->help_dialog.display.helpType != DtHELP_TYPE_TOPIC)
        XtSetSensitive(hw->help_dialog.browser.btnBoxTopLevelBtn, TRUE);
}



/*****************************************************************************
 * Function:	    void _DtHelpUpdateDisplayArea(char *locationId,
 *                             DtHelpDialogWidget   nw,  
 *                             int                   listUpdateType, 
 *                             int                   topicUpdateType);
 *
 *
 * Parameters:      locationId Specifies the ID string for the new topic we
 *                              are going to display in the HelpDialog widget.
 *
 *                  nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the Display area with a new Cache Creek Topic.
 *                  
 *****************************************************************************/
void _DtHelpUpdateDisplayArea(
    char *locationId,
    DtHelpDialogWidget nw,
    Boolean vol_changed,
    int listUpdateType,
    int topicUpdateType)
{
   XtPointer	 topicHandle;
   Boolean       validlocationId=FALSE;
   int           status=NON_VALID_STATUS;
   char          *userErrorStr=NULL;
   char          *sysErrorStr=NULL;
   char          *tmpMsg=NULL;
   char	         *retTitle;
   XmString      labelString;
   Arg		 args[10];
   int      	 n=0;
	
   DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;
  
   _DtHelpTurnOnHourGlass(XtParent(hw));
   
    validlocationId = False;  /* default */

    /* If we have a null idString, assign the top level topic to it */
    if (locationId == NULL
        && hw->help_dialog.help.topLevelId != NULL)
    {
         locationId = XtNewString(hw->help_dialog.help.topLevelId);
    }

    if (locationId != NULL)
    {
       XmUpdateDisplay((Widget)hw);
        
       /* format the initial locationId info to display */
       status = _DtHelpFormatTopic (
        	    hw->help_dialog.help.pDisplayArea,
        	    hw->help_dialog.display.volumeHandle, 
        	    locationId,
        	    True,
        	    &topicHandle);
        
       /* unless locationId not found, the topic is valid */
       if (status != -2) 
           validlocationId = True;
    }  /* if existing loc id, try to format it */
                
    if (status == 0)  /* success */
    {
        if (hw->help_dialog.ghelp.volumeFlag == FALSE)
        {
	   XmFontList  fontList;
	   Boolean     mod = False;

	   XtSetArg(args[0], XmNfontList, &fontList);
	   XtGetValues(hw->help_dialog.browser.volumeLabel, args, 1);

           /* We have a new volume, so update the volume title */
        
	   _DtHelpFormatVolumeTitle(hw->help_dialog.help.pDisplayArea,
					hw->help_dialog.display.volumeHandle,
					&labelString, &fontList, &mod);
        
           if(labelString == NULL && NULL != hw->help_dialog.display.helpVolume)
        	labelString = XmStringCreateLocalized(
					hw->help_dialog.display.helpVolume);
        
	   if (NULL != labelString)
	     {
	       n = 0;
               XtSetArg (args[n], XmNlabelString, labelString);	n++;
	       if (mod == True)
	         { XtSetArg (args[n], XmNfontList, fontList);	n++; }
               XtSetValues (hw->help_dialog.browser.volumeLabel, args, n);
               XmStringFree(labelString); 
	     }

	   if (True == mod)
	       XmFontListFree(fontList);

           /* Update our volumeFlag because we know that its we have just,
            * opened a new volume. It will get set to false when we need 
            * to process a different help volume, and remain true while
            * processing the current help volume.
           */
           hw->help_dialog.ghelp.volumeFlag = TRUE;
       }  /* if volumeFlag == False */
        
       /* Update our history and jump lists */
       if (listUpdateType == DtHISTORY_AND_JUMP)
       {
           _DtHelpUpdateHistoryList(locationId, topicUpdateType,
						vol_changed, (Widget)hw);
           _DtHelpUpdateJumpList(locationId,  topicUpdateType,(Widget) hw);
       }
       else if (listUpdateType == DtHISTORY_ONLY)
           _DtHelpUpdateHistoryList(locationId, topicUpdateType,
	 					 vol_changed, (Widget)hw);
       else if (listUpdateType == DtJUMP_ONLY)
           _DtHelpUpdateJumpList(locationId,  topicUpdateType,(Widget)hw);
        
       /* Adjust our "Top Level" browser home buttons */
       if (hw->help_dialog.browser.showTopLevelBtn == TRUE)
       {
           if (strstr (hw->help_dialog.display.helpVolume, DtBROWSER_NAME) != NULL) 
           {
		/* FIX: get top topic of the current volume & compare with locationId */
               if (strcmp (hw->help_dialog.display.locationId, DtBROWSER_HOME)== 0)
        	    XtSetSensitive(hw->help_dialog.browser.btnBoxTopLevelBtn, FALSE);
               else
        	    XtSetSensitive(hw->help_dialog.browser.btnBoxTopLevelBtn, TRUE);
            }
            else
        	XtSetSensitive(hw->help_dialog.browser.btnBoxTopLevelBtn, TRUE);
       }  /* if showTopLevelBtn */
        
       /* Check to see if we are going to display the top level is so */
       if (_DtHelpCeIsTopTopic(hw->help_dialog.display.volumeHandle,
                               hw->help_dialog.display.locationId) == 0)
       {
           XtSetSensitive(hw->help_dialog.menu.topBtn, FALSE);
           XtSetSensitive(hw->help_dialog.menu.popupTopBtn, FALSE);
       }
       else
       {
           XtSetSensitive(hw->help_dialog.menu.topBtn, TRUE);
           XtSetSensitive(hw->help_dialog.menu.popupTopBtn, TRUE);
       }
        
       /* Change the widget instance record locationId field to 
        *  represent the new locationId being desplayed.
        */
       XtFree(hw->help_dialog.display.locationId);
       hw->help_dialog.display.locationId = XtNewString(locationId);
        
    
       /* Update the current path area for the given topic */
       _DtHelpUpdatePathArea(hw->help_dialog.display.locationId, hw);
     
       _DtHelpDisplayAreaSetList (hw->help_dialog.help.pDisplayArea,
        	 topicHandle, FALSE, hw->help_dialog.backtr.scrollPosition);

    }  /* if status == 0 */
        
    if (status < 0) 
    {
        /* Invalid idString */

        /* Clear the current volume title label */
        /* ??? Is this the best way to null out a label gadget ??? */
        labelString = XmStringCreateLocalized(" ");
        XtSetArg (args[n], XmNlabelString, labelString);	
        XtSetValues (hw->help_dialog.browser.volumeLabel, args, 1);
        XmStringFree(labelString); 

        /* Clear our history dialog selected items */
        if (hw->help_dialog.history.topicList != NULL)
            XmListDeselectAllItems(hw->help_dialog.history.topicList);
        if (hw->help_dialog.history.volumeList != NULL)
            XmListDeselectAllItems(hw->help_dialog.history.volumeList);

        /* Set current path area to null by giving it a null idStirng */
        _DtHelpUpdatePathArea(NULL, hw);

        /* De-sensatize our buttons, they do not work 
         * with out valid topics.
         */
      
        XtSetSensitive(hw->help_dialog.menu.topBtn, FALSE);
        XtSetSensitive(hw->help_dialog.menu.popupTopBtn, FALSE);

        if (hw->help_dialog.history.pHistoryListHead == NULL)
        {    
           XtSetSensitive(hw->help_dialog.menu.historyBtn, FALSE);
           XtSetSensitive(hw->help_dialog.browser.btnBoxHistoryBtn, FALSE);
        }
        else
        {
           /* in case they were desensitized for any reason */
           XtSetSensitive(hw->help_dialog.menu.historyBtn, TRUE);
           XtSetSensitive(hw->help_dialog.browser.btnBoxHistoryBtn, TRUE);
        }

        if (hw->help_dialog.backtr.pJumpListHead != NULL)    
        {
           XtSetSensitive(hw->help_dialog.menu.backBtn, TRUE);
           XtSetSensitive(hw->help_dialog.menu.popupBackBtn, TRUE);
           XtSetSensitive(hw->help_dialog.browser.btnBoxBackBtn, TRUE);
        } 

        
        /* Determin the proper error message to give */
        if (!validlocationId)
        {
      
            if (locationId == NULL)
            {
                 tmpMsg = (char *)_DTGETMESSAGE(2, 59,
                                 "No location ID specified.");
                 userErrorStr = XtNewString(tmpMsg);
            }
            else
            {
                 tmpMsg = (char *)_DTGETMESSAGE(2, 55,
                                 "Nonexistent location ID:");
                 userErrorStr = XtMalloc(strlen(tmpMsg) + 
                                  strlen(hw->help_dialog.display.locationId)+ 2);
                 (void) strcpy(userErrorStr, tmpMsg);
                 (void) strcat(userErrorStr, " ");
                 (void) strcat(userErrorStr, hw->help_dialog.display.locationId);
                 sysErrorStr = XtNewString(HDMessage9);
	    }
        }
        else
        {
            tmpMsg = (char*)_DTGETMESSAGE(2, 54,
                     "Help topic could not be formatted.");
            userErrorStr = XtNewString(tmpMsg);
            sysErrorStr = XtNewString(HDMessage5);
        }


        _DtHelpUpdateJumpList(userErrorStr, DtHELP_TYPE_DYNAMIC_STRING,
                           (Widget) hw);

        hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
        hw->help_dialog.display.stringData = XtNewString(userErrorStr);

        _DtHelpDisplayFormatError(hw->help_dialog.help.pDisplayArea, (Widget) hw,
                               userErrorStr, sysErrorStr);
        XtFree(userErrorStr);
        XtFree(sysErrorStr);
    }   /* if (status < 0) */

    _DtHelpTurnOffHourGlass(XtParent(hw));          
}




/*****************************************************************************
 * Function:	    void _DtHelpUpdateJumpList(char *topicInfo,
 *                                 int topicType,
 *                                 DtHelpDialogWidget nw);  
 *
 *
 * Parameters:      topicInfo   Specifies the 
 *                  int         Specifies the topicInfo type.
 *
 *                  nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the Jump List with the new topic.
 *
 *****************************************************************************/
void _DtHelpUpdateJumpList(
    char *topicInfo,
    int topicType,
    Widget nw)
{
  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;
 
  /* Add the new topic to the top of the jump list */
  /* We add a -1 for the scrollbar position value, and will replace it with the
   * actual value just prior to changing the window to the new topic.
   */
  _DtHelpTopicListAddToHead(topicInfo, hw->help_dialog.display.topicTitleLbl,
                        topicType, DtJUMP_LIST_MAX,
                        hw->help_dialog.display.helpVolume,
                        &hw->help_dialog.backtr.pJumpListHead,
                        &hw->help_dialog.backtr.pJumpListTale,
                        &hw->help_dialog.backtr.totalJumpNodes,
                        -1);

  if (hw->help_dialog.backtr.totalJumpNodes <= 1) 
    {
      XtSetSensitive(hw->help_dialog.menu.backBtn, FALSE);
      XtSetSensitive(hw->help_dialog.menu.popupBackBtn, FALSE);
      XtSetSensitive(hw->help_dialog.browser.btnBoxBackBtn, FALSE);
    }
  else
    {
      XtSetSensitive(hw->help_dialog.menu.backBtn, TRUE);
      XtSetSensitive(hw->help_dialog.menu.popupBackBtn, TRUE);
      XtSetSensitive(hw->help_dialog.browser.btnBoxBackBtn, TRUE);
    }
}




/*****************************************************************************
 * Function:	    void ProcessJumpBack(DtHelpDialogWidget nw);  
 *
 *
 * Parameters:      helpDialogWidget  Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Pops the top element in the jump list off the stack
 *                  and jumps
 *
 *                  back to that topic.
 *
 *****************************************************************************/
static void ProcessJumpBack(
    DtHelpDialogWidget nw)
{
 
  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;
  DtTopicListStruct *pTemp= NULL;


  /* Pop the top element off our jump list and display the new top element */
  _DtHelpTopicListDeleteHead(&hw->help_dialog.backtr.pJumpListHead,
                          &hw->help_dialog.backtr.pJumpListTale,
                          &hw->help_dialog.backtr.totalJumpNodes);
                          
                          

  /* Assign pTemp to the current head pointer for or jump list */
  pTemp = hw->help_dialog.backtr.pJumpListHead;

  if (hw->help_dialog.backtr.totalJumpNodes <= 1)
    {
      XtSetSensitive(hw->help_dialog.menu.backBtn, FALSE);
      XtSetSensitive(hw->help_dialog.menu.popupBackBtn, FALSE);
      XtSetSensitive(hw->help_dialog.browser.btnBoxBackBtn, FALSE);
    }
  else
    {
      XtSetSensitive(hw->help_dialog.menu.backBtn, TRUE);
      XtSetSensitive(hw->help_dialog.menu.popupBackBtn, TRUE);
       XtSetSensitive(hw->help_dialog.browser.btnBoxBackBtn, TRUE);
    }


  /* Assign the jump values to or instance structure variables */
  if (NULL != pTemp->helpVolume &&
	strcmp (hw->help_dialog.display.helpVolume, pTemp->helpVolume) != 0)
    {
      XtFree(hw->help_dialog.display.helpVolume);
      hw->help_dialog.display.helpVolume  = XtNewString(pTemp->helpVolume);
      
      /* Set our help volume flag so we open the proper volume */
      hw->help_dialog.ghelp.volumeFlag         = FALSE;

   }

   /* grab the current backtracs's topic title value */
   XmStringFree(hw->help_dialog.display.topicTitleLbl);
   hw->help_dialog.display.topicTitleLbl = XmStringCopy(pTemp->topicTitleLbl); 


  /* Assign our scrollbar value to our instance structure  so we jump to the
   * proper location 
   */
 
  hw->help_dialog.backtr.scrollPosition = pTemp->scrollPosition;


  hw->help_dialog.display.helpType = pTemp->topicType;

  switch (pTemp->topicType)
    {
      case DtHELP_TYPE_TOPIC:

        /* Update our help dialog with top jump element */
        XtFree(hw->help_dialog.display.locationId);
        hw->help_dialog.display.locationId = XtNewString(pTemp->locationId);
        
        _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_ONLY);
      
	break;

      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
        XtFree(hw->help_dialog.display.stringData);
        hw->help_dialog.display.stringData = XtNewString(pTemp->locationId);
        
        _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_ONLY);
     
        break;

      case DtHELP_TYPE_MAN_PAGE:
        XtFree(hw->help_dialog.display.manPage);
        hw->help_dialog.display.manPage = XtNewString(pTemp->locationId);
        
        _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_ONLY);
     
	break;

      case DtHELP_TYPE_FILE:
        XtFree(hw->help_dialog.display.helpFile);
        hw->help_dialog.display.helpFile = XtNewString(pTemp->locationId);
        
        _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_ONLY);
     
        break;


      default:  

        /* ERROR-MESSAGE */
        /* This means the user used the worng help type */
        XmeWarning((Widget)hw, (char*) HDMessage4);

        break;

    }  /* End Switch Statement */
    
    /* reset our scrollPosition back to its default here so in any other case,
     * we jump to the top of the topic 
     */
   hw->help_dialog.backtr.scrollPosition = -1;

}


/*****************************************************************************
 * Function:	    void _DtHelpDialogHypertextCB(
 *                              XtPointer pDisplayAreaStruct,
 *                              XtPointer clientData, 
 *                      	DtHelpHyperTextStruct *hyperData.) 
 *
 * Parameters:      pDisplayAreaStruct  Specifies the curretn display are info.
 *
 *                  clientData          Specifies the client data passed into
 *                                      the hypertext callback. 
 *
 *                  hyperData           Specifies the current hypertext info
 *                                      structure.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process all hypertext requests in a given Help Dialogs
 *                  display area.
 *
 ****************************************************************************/
void _DtHelpDialogHypertextCB (
    XtPointer pDisplayAreaStruct,
    XtPointer clientData,
    DtHelpHyperTextStruct *hyperData)
{
  char         *pTempAccessPath;
  char         *pTempLocationId;  
  DtHelpDialogCallbackStruct callData;
  char         *tmpErrorMsg;

  DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData ;
   
  /* We allow users to force a new window and override the jump-reuse
   * hyper type and force a new window to be used
   */
   if (   (   ButtonRelease == hyperData->event->type 
           && hyperData->event->xbutton.state & (ControlMask|ShiftMask))
       || (   KeyPress == hyperData->event->type 
           && hyperData->event->xkey.state & (ControlMask|ShiftMask)))
     hyperData->window_hint = _DtCvWindowHint_NewWindow;


 
  switch (hyperData->hyper_type)
    {

      case _DtCvLinkType_SameVolume:
      case _DtCvLinkType_CrossLink:


         switch (hyperData->window_hint)
           {

	     case _DtCvWindowHint_CurrentWindow:
               ProcessJumpReuse((Widget)hw, hyperData);
             break;

             case _DtCvWindowHint_NewWindow:
        
               _DtHelpTurnOnHourGlass(XtParent(hw));
  
               if (hw->help_dialog.display.hyperLinkCallback != NULL)
                 {
                    pTempAccessPath = 
                      _DtHelpParseAccessFile(hyperData->specification);
         
                    if (pTempAccessPath != NULL)       /* Use the New one */
                      callData.helpVolume = pTempAccessPath;
                    else                               /* Use the old one */
                      callData.helpVolume =
                                XtNewString(hw->help_dialog.display.helpVolume);


                    callData.reason        = DtCR_HELP_LINK_ACTIVATE;
                    callData.event         = hyperData->event;
                    callData.locationId    =  _DtHelpParseIdString
                                                  (hyperData->specification);
                    callData.specification = NULL;
                    callData.hyperType     = DtHELP_LINK_TOPIC;
                    callData.windowHint    = DtHELP_NEW_WINDOW;
                   
                    /* Now, envoke the apps hyper callback */
                    XtCallCallbackList(
                     (Widget)hw,hw->help_dialog.display.hyperLinkCallback,&callData);
	         }
               else
                 {  /* The application did not register a hypertext callback so
                     *  we must generate the proper warning message and 
                     *  continue!
                     */
                    XmeWarning((Widget)hw, (char*) HDMessage6); 

                    /* Call jump-reuse procedure for default */
                    ProcessJumpReuse((Widget)hw, hyperData);
                  }
	      
               _DtHelpTurnOffHourGlass(XtParent(hw));       
 
             break;

             case _DtCvWindowHint_PopupWindow:

               _DtHelpTurnOnHourGlass(XtParent(hw));

               pTempAccessPath = 
                         _DtHelpParseAccessFile(hyperData->specification);
         
               if (pTempAccessPath == NULL)  /* Use the old one */  
                 pTempAccessPath = XtNewString(hw->help_dialog.display.helpVolume);

               pTempLocationId = _DtHelpParseIdString(hyperData->specification);

               _DtHelpDisplayDefinitionBox((Widget)hw,
                                 (Widget **)&(hw->help_dialog.ghelp.definitionBox),
                                 pTempAccessPath, pTempLocationId);

               XtFree(pTempLocationId);
               XtFree(pTempAccessPath);

               _DtHelpTurnOffHourGlass(XtParent(hw));       

             break;

             default:  

               /* Non valid window hint generage the proper
                *  error message.
                */
             
                /* ERROR-MESSAGE */
              break;


            }  /* End Window_hint Switch Statement */

        break;



       case _DtCvLinkType_Execute:
          _DtHelpExecFilteredCmd((Widget) hw, 
                    hyperData->specification, DtHELP_ExecutionPolicy_STR, 
                    &hw->help_dialog.display, &hw->help_dialog.help);
         break;


      case _DtCvLinkType_ManPage:
      case _DtCvLinkType_AppDefine:	
      case _DtCvLinkType_TextFile:
       
        /* Process Application Man page link or App Defined link types */
        _DtHelpTurnOnHourGlass(XtParent(hw));

        /* If the application registered a hypertext callback use it! */
        if (hw->help_dialog.display.hyperLinkCallback != NULL)
          {
  
            /* Setup the DtHyperProcStructer to pass back to the 
             * client as callData. 
             */
             callData.reason        = DtCR_HELP_LINK_ACTIVATE;
             callData.event         = hyperData->event;
             callData.locationId    = NULL;
             callData.helpVolume    = NULL;
	     if (_DtCvLinkType_AppDefine == hyperData->hyper_type)
	       {
		 callData.locationId =
				_DtHelpParseIdString(hyperData->specification);
		 callData.helpVolume =
                                XtNewString(hw->help_dialog.display.helpVolume);
	       }
             callData.specification = hyperData->specification;
             callData.hyperType     = hyperData->hyper_type;
             callData.windowHint    = hyperData->window_hint;
          
             XtCallCallbackList ((Widget) hw,
                             hw->help_dialog.display.hyperLinkCallback, &callData);
	  }
        else
          { 
             /* The application did not register a hypertext 
              * callback so we must generate the proper error 
              * message and continue!
              */
              XmeWarning((Widget)hw, (char*) HDMessage8); 

              if (hyperData->hyper_type == DtHELP_LINK_APP_DEFINE)
                {
                  tmpErrorMsg = XtNewString((char *)_DTGETMESSAGE(2, 56,
                        "The selected Hypertext link is not supported within this application."));
                  _DtHelpErrorDialog(XtParent(hw),tmpErrorMsg);
	        }
              else
                {
                  tmpErrorMsg = XtNewString((char *)_DTGETMESSAGE(2, 57,
                        "Links to Man Pages are not supported by this application."));
                  _DtHelpErrorDialog(XtParent(hw),tmpErrorMsg);

		}
              XtFree(tmpErrorMsg);
             
           }

        _DtHelpTurnOffHourGlass(XtParent(hw));  
        break;

        default:  /* This catches bogus link types */

            /* Non valid link type so we are dropping it and are generating
             * the  proper error message.
             */
             
             /* ERROR-MESSAGE */
             XmeWarning((Widget)hw, (char*) HDMessage7);
             break;


    }  /* End Switch Statement */


}  /* End _DtHelpDialogHypertextCB */



 

/*****************************************************************************
 * Function:	    void ResizeHelpDialogCB()
 *
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Adjust the widget instance values for rows and columns.
 *
 ****************************************************************************/
static void ResizeHelpDialogCB (
    XtPointer clientData)
{
  DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData ;
    

  /* Re-Set our rows and colums values */
  if ((hw->help_dialog.help.pDisplayArea != NULL) &&
       hw->help_dialog.display.firstTimePopupFlag == TRUE)
     _DtHelpDisplayAreaDimensionsReturn (hw->help_dialog.help.pDisplayArea,
                                   &(hw->help_dialog.display.textRows),
                                   &(hw->help_dialog.display.textColumns));
}



/*****************************************************************************
 * Function:	    static void  ProcessJumpReuse(nw, hyperData)
 *                                  Widget nw;
 *                                  DtHelpHyperTextStruct *hyperData;
 *
 * Parameters:      nw    Specifies the widget ID of the current help dialog
 *                        widget.
 * 
 *                  hyperData Specifies the hypertext data callback struct.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process the jump-reuse hypertext link data.
 *
 *****************************************************************************/
static void  ProcessJumpReuse(
    Widget nw,
    DtHelpHyperTextStruct *hyperData)
{

  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;
  char         *pTempAccessPath;
  char         *pTempLocationId;  

 
  /* Parse our specification into the proper fields in our instance 
   * record.  
   */

   pTempAccessPath = _DtHelpParseAccessFile(hyperData->specification);
         
   if (pTempAccessPath != NULL) 
     {
       /* Free the old one and assign the new path */
       XtFree(hw->help_dialog.display.helpVolume);
       hw->help_dialog.display.helpVolume = pTempAccessPath;
       hw->help_dialog.ghelp.volumeFlag = FALSE;
     }
 
    /* Free old copy fisrt ??? */
    pTempLocationId = _DtHelpParseIdString(hyperData->specification);
    XtFree(hw->help_dialog.display.locationId);
    hw->help_dialog.display.locationId = pTempLocationId;
    hw->help_dialog.display.helpType = DtHELP_TYPE_TOPIC;
   
    _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_AND_JUMP);
 
}



/*****************************************************************************
 * Function:	    void _DtHelpTopicListFree (DtTopicListStruct *pHead);  
 *
 *
 * Parameters:      pHead   Specifies the head pointer to the topic list.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Frees all elements in a Topic List.
 *
 *****************************************************************************/
void _DtHelpTopicListFree(
    DtTopicListStruct *pHead)
{
  DtTopicListStruct *pTemp=NULL;


   while (pHead != NULL)
     {
       /* Free up each element in the current node */
       pTemp            = pHead;
       pHead            = pHead->pNext;
       pTemp->pNext     = NULL;
       pTemp->pPrevious = NULL;

       /* Free the id String and AccessPath elements */
       XtFree(pTemp->locationId);
       XtFree(pTemp->helpVolume);
       XmStringFree(pTemp->topicTitleLbl);

       /* Now, free the whole node */
       XtFree((char *)pTemp);

    }



}



/*****************************************************************************
 * Function:	    void CleanUpHelpDialog(DtHelpDialogWidget nw,
 *                                         int cleanUpKind);  
 *
 *
 * Parameters:      nw  Specifies the current help dialog widget.
 *                  cleanUpKind Specifies the type of clean up we are doing, 
 *                  celan up and reuse or clean up and destroy.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    This function will re-initializes a Help Dialog Widget to 
 *                  known good starting values or clean up in prepretion for 
 *                  an impending destroy.
 *
 *****************************************************************************/
static void CleanUpHelpDialog(
    Widget nw,
    int cleanUpKind)
{
  Arg	     args[10];

  DtHistoryListStruct *pTempVolume=NULL;
  DtHistoryListStruct *pTempVolumeCurrent=NULL;

  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;


  XtFree(hw->help_dialog.print.printVolume);
  hw->help_dialog.print.printVolume    = NULL;

  /* must clean volume list and topic list from history */
  pTempVolume = hw->help_dialog.history.pHistoryListHead;
  while (pTempVolume != NULL)
  {
      _DtHelpTopicListFree(pTempVolume->pTopicHead);
      pTempVolumeCurrent = pTempVolume;   
      pTempVolume = pTempVolume->pNext;
     
      XmStringFree(pTempVolumeCurrent->itemTitle); 
      XtFree((char *)pTempVolumeCurrent);
  }
  hw->help_dialog.history.pHistoryListHead  = NULL;
 
  _DtHelpTopicListFree(hw->help_dialog.history.pPathListHead);
  hw->help_dialog.history.pPathListHead     = NULL; 
  hw->help_dialog.history.pPathListTale     = NULL;
  hw->help_dialog.history.totalPathNodes    = 0;


  /* Free our jump list display stuff */
  _DtHelpTopicListFree(hw->help_dialog.backtr.pJumpListHead);
  hw->help_dialog.backtr.pJumpListHead     = NULL;
  hw->help_dialog.backtr.pJumpListTale     = NULL;
  hw->help_dialog.backtr.totalJumpNodes    = 0;


  /* do nothing for index search dialog on a Clean for Reuse */

  /* Close our current help volume */
  if (hw->help_dialog.display.volumeHandle != NULL)
    {
      _DtHelpCloseVolume(hw->help_dialog.display.volumeHandle);
      hw->help_dialog.display.volumeHandle = NULL;
    }

  /* Depending on weither we are just closing this help dialog or destroying 
   * it we will do the following!
   */
  
  if (cleanUpKind == DtCLEAN_FOR_DESTROY)
    {

      /* Free any remaining char * values we malloc'ed in our help dialog */
      XtFree(hw->help_dialog.display.locationId);
      XtFree(hw->help_dialog.display.helpVolume); 
     
      XtFree(hw->help_dialog.display.manPage);
      XtFree(hw->help_dialog.display.stringData);
      XtFree(hw->help_dialog.display.helpFile);
      XtFree(hw->help_dialog.display.topicTitleStr);
      if (hw->help_dialog.display.topicTitleLbl != NULL)
          XmStringFree(hw->help_dialog.display.topicTitleLbl);

      XtFree(hw->help_dialog.ghelp.parentId);

      XtFree(hw->help_dialog.print.printer);
      if (hw->help_dialog.print.helpPrint != _DtHelpDefaultHelpPrint)
        XtFree(hw->help_dialog.print.helpPrint);

      /* Clean and close and destroy (True) our index search dialog */
      _DtHelpGlobSrchCleanAndClose(&hw->help_dialog.srch,True);

      /* Set our current topic variables to initial values */
      _DtHelpCommonHelpClean(&hw->help_dialog.help,True);
    }
  else
    {

      /* Reset our buttons to the proper state */
      XtSetSensitive(hw->help_dialog.menu.backBtn, FALSE);
      XtSetSensitive(hw->help_dialog.menu.popupBackBtn, FALSE);
      XtSetSensitive(hw->help_dialog.browser.btnBoxBackBtn, FALSE);

      /* Clean and close our index search dialog */
      _DtHelpGlobSrchCleanAndClose(&hw->help_dialog.srch,False);
    
      if (hw->help_dialog.history.historyWidget != NULL)
        {
          XtUnmanageChild(hw->help_dialog.history.historyWidget);
        }

     if (hw->help_dialog.print.printForm != NULL)
        {
          XtUnmanageChild(hw->help_dialog.print.printForm);
        }

      /* Free our history display stuff */
      if (hw->help_dialog.history.volumeList != NULL)
        {
          XtSetArg(args[0], XmNitems, NULL);
          XtSetArg(args[1], XmNitemCount, 0);
          XtSetValues(hw->help_dialog.history.volumeList, args, 2);
        }
      if (hw->help_dialog.history.topicList != NULL)
        {
          XtSetArg(args[0], XmNitems, NULL);
          XtSetArg(args[1], XmNitemCount, 0);
          XtSetValues(hw->help_dialog.history.topicList, args, 2);
        }
             
      /* Set our toc areay area to a null starting vlaues */
      _DtHelpDisplayAreaClean(hw->help_dialog.browser.pTocArea);
  
      /* Set our current topic variables to initial values */
      _DtHelpCommonHelpClean(&hw->help_dialog.help,False);
    }

}




/****************************************************************************
 * Function:         CatchClose( Widget w);
 *                          
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Catches the window manager close requests and assigns our
 *                  CloseHelpCB to handel them.
 *
 ***************************************************************************/
static void CatchClose (
    Widget widget)
{

  Atom      wm_delete_window;
  Arg       args[2];

  /* Grab the window mgr close */
  wm_delete_window = XmInternAtom(XtDisplay(XtParent(widget)),
                                            "WM_DELETE_WINDOW", FALSE);
  XtSetArg(args[0], XmNdeleteResponse, XmDO_NOTHING);

  /* Current Help Dialog Window */
  XmAddWMProtocolCallback(XtParent(widget),wm_delete_window,
                          (XtCallbackProc)CloseHelpCB, (XtPointer) widget);
  XtSetValues(XtParent(widget), args, 1);
  
} 




/************************************************************************
 * Function: CloseHelpCB()
 *
 *	Close the Help Dialog Window
 *
 ************************************************************************/
static void CloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

  DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
  DtHelpDialogCallbackStruct callDataInfo;
  XmPushButtonCallbackStruct *callbackStruct = 
                               (XmPushButtonCallbackStruct*) callData; 

  /*
   * Check to see if a selection is in process.
   * If so, cancel the selection and don't close the dialog
   */
  if (_DtHelpCancelSelection(hw->help_dialog.help.pDisplayArea) == True)
	return;

  /* No selection in progress. Close the dialog.
   *
   * ??? By definition, when a user closes a Help Dialog if it is 
   * used again by the application the state will be new. So we should 
   * flush out any info currently in the help, history, and search dialogs.
   */
 
  CleanUpHelpDialog((Widget)hw, DtCLEAN_FOR_REUSE);

  /* Look to see if they registered their own close callback */
  if (hw->help_dialog.ghelp.closeCallback != NULL)
    {
       callDataInfo.reason = DtCR_HELP_CLOSE;
       callDataInfo.event = callbackStruct->event;
       callDataInfo.locationId = NULL;
       callDataInfo.helpVolume = NULL;
       callDataInfo.specification = NULL;
       callDataInfo.hyperType = 0;
   
       
       /* All we do is envoke the applications close callback */
       XtCallCallbackList((Widget)hw,hw->help_dialog.ghelp.closeCallback,
                         (XtPointer) &callDataInfo);
    }
  else
   XtUnmanageChild((Widget)hw);

 	
}







 

/*********************************************************************
 * Function:  SetupHelpDialogMenus
 *
 *   This procedure build the menubar pulldows buttons for the Index
 * Browser and the Topic Viewer.
 *
 * Called by: MakeIndexWindow, and MakeTopicWindow.
 *********************************************************************/
static Widget SetupHelpDialogMenus(
    Widget parent)
{
   Widget       menuBar;
   Widget       c[10];
   Widget	cascadeB5;
   Widget       menupane1, menupane2, menupane3, menupane4, menupane5;
   Widget       helpBtn1, helpBtn2, helpBtn3, helpBtn4, helpBtn5, helpBtn6;
   Arg		args[10];	
   int          n;
   XmString     labelStr;
   char        *mnemonic;
   DtHelpListStruct *pHelpInfo;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) parent;


   /* Build the menubar */

   n = 0;
   menuBar = XmCreateMenuBar (parent, "menuBar", args, n); 
   XtManageChild (menuBar);


  /*******************************************************
   * Menupane:  Close, and related buttons 
   *******************************************************/
   
  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane1 = XmCreatePulldownMenu(menuBar, "fileMenu", args, n);
  
  /* Add callback for File Menu */
  pHelpInfo = _DtHelpListAdd(DtHELP_FileMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) menupane1, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);


  /* Print button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 1,"Print...")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 2,"P"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] = hw->help_dialog.menu.printBtn = XmCreatePushButtonGadget(menupane1,
						  "print", args, n);
  XtAddCallback(hw->help_dialog.menu.printBtn,XmNactivateCallback, 
                 DisplayPrintCB, (XtPointer) hw);
   
  XmStringFree(labelStr);
  

  if (hw->help_dialog.menu.showDupBtn == True)
    {
      /* Duplicate button */
      labelStr = XmStringCreateLocalized(((char *)
                 _DTGETMESSAGE(2, 5,"New Window...")));
      mnemonic = ((char *)_DTGETMESSAGE(2, 6,"W"));
      n = 0;
      XtSetArg(args[n], XmNlabelString, labelStr); n++;
      XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
      c[1] = hw->help_dialog.menu.newWindowBtn = 
                  XmCreatePushButtonGadget(menupane1,"newWindowBtn", args, n);
      XtAddCallback(hw->help_dialog.menu.newWindowBtn,XmNactivateCallback, 
                    _DtHelpDuplicateWindowCB, (XtPointer) hw); 
      XmStringFree(labelStr);

      /* Exit button */
      labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 3,"Close")));
      mnemonic = ((char *)_DTGETMESSAGE(2, 4,"C"));
      n = 0;
      XtSetArg(args[n], XmNlabelString, labelStr); n++;
      XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
      c[2] = hw->help_dialog.menu.closeBtn =
	       XmCreatePushButtonGadget(menupane1,"close", args, n);
  
      XtAddCallback(hw->help_dialog.menu.closeBtn,XmNactivateCallback, 
                    CloseHelpCB, (XtPointer) hw); 
      XmStringFree(labelStr); 

      /* set the cancel button (for KCancel) */
      hw->bulletin_board.cancel_button= hw->help_dialog.menu.closeBtn;

 
      /* Manage the children for the File menu pane */ 
      XtManageChildren(c, 3);
       
    }
  else
    {
      /* Exit button */
      labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 3,"Close")));
      mnemonic = ((char *)_DTGETMESSAGE(2, 4,"C"));
      n = 0;
      XtSetArg(args[n], XmNlabelString, labelStr); n++;
      XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
      c[1] = hw->help_dialog.menu.closeBtn =
	     XmCreatePushButtonGadget(menupane1,"close", args, n);
  
      XtAddCallback(hw->help_dialog.menu.closeBtn,XmNactivateCallback, 
                 CloseHelpCB, (XtPointer) hw); 
      XmStringFree(labelStr); 
 
      /* set the cancel button (for KCancel) */
      hw->bulletin_board.cancel_button= hw->help_dialog.menu.closeBtn;
  
      /* Manage the children for the File menu pane */ 
      XtManageChildren(c, 2);
    }


  /*******************************************************
   * Menupane:  Edit, (Copy)
   *******************************************************/

  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane2 = XmCreatePulldownMenu(menuBar, "editMenu", args, n);

  /* Add callback for Edit Menu */
  pHelpInfo = _DtHelpListAdd(DtHELP_EditMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) menupane2, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);

  /* Edit button */
  labelStr = XmStringCreateLocalized((
                    (char *)_DTGETMESSAGE(2,7,"Copy")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 8,"C"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] = hw->help_dialog.menu.copyBtn = XmCreatePushButtonGadget
                                        (menupane2, "copy", args, n);
  XtAddCallback(hw->help_dialog.menu.copyBtn,XmNactivateCallback, 
                 CopyTextCB, (XtPointer) hw); 
 
  XmStringFree(labelStr); 
   
  XtManageChildren(c, 1);


  /*******************************************************
   * Menupane:  Search, (Topic Index)
   *******************************************************/

  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane3 = XmCreatePulldownMenu(menuBar, "searchMenu", args, n);

  /* Add callback for Search Menu */
  pHelpInfo = _DtHelpListAdd(DtHELP_SearchMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) menupane3, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);

  /* Search button */
  labelStr = XmStringCreateLocalized((
                    (char *)_DTGETMESSAGE(2, 9,"Index...")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 10,"I"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] = hw->help_dialog.menu.keyBtn = XmCreatePushButtonGadget
                                        (menupane3, "keyword", args, n);
  XtAddCallback(hw->help_dialog.menu.keyBtn,XmNactivateCallback, 
                 _DtHelpDisplayIndexCB, (XtPointer) hw); 
 
  XmStringFree(labelStr); 
   
  XtManageChildren(c, 1);



  /*******************************************************
   * Menupane:  Navigate, and related buttons 
   *******************************************************/

  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane4 = XmCreatePulldownMenu(menuBar, "navigateMenu", args, n);

  /* Add callback for Navigate Menu */
  pHelpInfo = _DtHelpListAdd(DtHELP_NavigateMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) menupane4 , XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);

 
  /* Back Button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 13,"Backtrack")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 14,"B"));
  n = 0; 
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] =  hw->help_dialog.menu.backBtn  = XmCreatePushButtonGadget(menupane4,
                                            "backTrack", args, n);
  XtAddCallback(hw->help_dialog.menu.backBtn,XmNactivateCallback, 
                  _DtHelpDisplayBackCB, (XtPointer) hw); 
  XtSetSensitive(hw->help_dialog.menu.backBtn, FALSE);

  XmStringFree(labelStr); 


  /* Top Button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 11,"Home Topic")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 12,"T"));
  n = 0; 
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[1] =  hw->help_dialog.menu.topBtn  = XmCreatePushButtonGadget(menupane4,
                                            "homeTopic", args, n);
  XtAddCallback(hw->help_dialog.menu.topBtn,XmNactivateCallback, 
                 DisplayTopLevelCB, (XtPointer) hw); 

  XmStringFree(labelStr); 


  /* Place a menu break here  */
  n = 0;
  c[2] = XmCreateSeparatorGadget(menupane4, "seporator",args, n);

  /* History Button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 15,"History...")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 16,"H"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[3] = hw->help_dialog.menu.historyBtn = XmCreatePushButtonGadget(menupane4,
				          "history",args, n);

  XtAddCallback(hw->help_dialog.menu.historyBtn,XmNactivateCallback, 
                 _DtHelpDisplayHistoryCB, (XtPointer) hw); 

  XmStringFree(labelStr); 
 

  /* Manage all the children of this manu pane */ 
  XtManageChildren(c, 4);




  /*******************************************************
   * Menupane:  Help, and related buttons 
   *******************************************************/
  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane5 = XmCreatePulldownMenu(menuBar, "helpMenu", args, n);

  /* Add callback for Help Menu */
  pHelpInfo = _DtHelpListAdd(DtHELP_HelpMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) menupane5, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);

  /* On HELP button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 17,"Overview - Using Help")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 18,"v"));
  n = 0; 
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] = helpBtn1 = XmCreatePushButtonGadget(menupane5,
                                            "usingHelp", args, n);
  pHelpInfo = _DtHelpListAdd(DtHELP_onHelpMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback(helpBtn1, XmNactivateCallback,
                _DtHelpCB, (XtPointer) pHelpInfo);
  
  XmStringFree(labelStr); 


  /* Place a menu break here  */
  n = 0;
  c[1] = XmCreateSeparatorGadget(menupane5, "seporator",args, n);


  /* On Table Of Contents Help button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2,19,"Table Of Contents")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 20, "C"));
  n = 0; 
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[2] = helpBtn2 = XmCreatePushButtonGadget(menupane5,
                                            "tableOfContents", args, n);
  pHelpInfo = _DtHelpListAdd(DtHELP_onTableOfContenseMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback(helpBtn2, XmNactivateCallback,
                _DtHelpCB, (XtPointer) pHelpInfo);
  
  XmStringFree(labelStr); 


  /* On Tasks Help button */
  labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 21,"Tasks")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 22, "T"));
  n = 0; 
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[3] = helpBtn3 = XmCreatePushButtonGadget(menupane5,
                                            "tasks", args, n);
  pHelpInfo = _DtHelpListAdd(DtHELP_onTasksMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback(helpBtn3, XmNactivateCallback,
                _DtHelpCB, (XtPointer) pHelpInfo);
  XmStringFree(labelStr); 


  /* On Reference button */
  labelStr = XmStringCreateLocalized(((char *)
             _DTGETMESSAGE(2, 23,"Reference")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 24,"R"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[4] = helpBtn4 = XmCreatePushButtonGadget(menupane5,
				          "reference",args, n);
  pHelpInfo = _DtHelpListAdd(DtHELP_onReferenceMenu_STR,
                        (Widget) hw,  &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback(helpBtn4, XmNactivateCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);
  XmStringFree(labelStr); 


  /* On Mouse & Keyboard button */
  labelStr = XmStringCreateLocalized(((char *)
             _DTGETMESSAGE(2, 25,"Mouse and Keyboard")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 26,"M"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[5] = helpBtn5 = XmCreatePushButtonGadget(menupane5,
				          "reference",args, n);
  pHelpInfo = _DtHelpListAdd(DtHELP_onMouseAndKeysMenu_STR,
                        (Widget) hw,  &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback(helpBtn5, XmNactivateCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);
  XmStringFree(labelStr); 


  /* Place a menu break here  */
  n = 0;
  c[6] = XmCreateSeparatorGadget(menupane5, "seporator",args, n);

  /* On Version */
  labelStr = XmStringCreateLocalized(((char *)
             _DTGETMESSAGE(2, 27,"About Help")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 28,"A"));
  n = 0;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[7] = helpBtn6 = XmCreatePushButtonGadget(menupane5,
				          "reference",args, n);
  pHelpInfo = _DtHelpListAdd(DtHELP_onVersionMenu_STR,
                        (Widget) hw,  &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback(helpBtn6, XmNactivateCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);
  XmStringFree(labelStr); 


  /* Manage the children for the Help menu pane */
  XtManageChildren(c, 8);
 


  /* Create Cascade buttons for menubar */

  /* File Menu */
  labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(2, 29,"File")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 30,"F"));
  n = 0;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  XtSetArg(args[n], XmNsubMenuId, menupane1); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  c[0] = XmCreateCascadeButton(menuBar,"file",args, n); 
  XmStringFree(labelStr); 

  /* Edit Menu */
  labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(2, 37,"Edit")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 38,"E"));
  n = 0;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  XtSetArg(args[n], XmNsubMenuId, menupane2); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  c[1] = XmCreateCascadeButton(menuBar, "edit", args, n); 
  XmStringFree(labelStr); 

  /* Search Menu */
  labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(2, 31,"Search")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 32,"S"));
  n = 0;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  XtSetArg(args[n], XmNsubMenuId, menupane3); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  c[2] = XmCreateCascadeButton(menuBar, "search", args, n); 
  XmStringFree(labelStr); 

  /* Navigate Menu */
  labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(2, 33,"Navigate")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 34,"N"));
  n = 0;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  XtSetArg(args[n], XmNsubMenuId, menupane4); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  c[3] = XmCreateCascadeButton(menuBar, "navigate", args, n); 
  XmStringFree(labelStr); 

  /* Help Menu */
  labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(2, 35,"Help")));
  mnemonic = ((char *)_DTGETMESSAGE(2, 36,"H"));
  n = 0;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  XtSetArg(args[n], XmNsubMenuId, menupane5); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  c[4] = cascadeB5 = XmCreateCascadeButton(menuBar, "help", args, n); 
  XmStringFree(labelStr); 

  XtManageChildren(c, 5);

   
  /* Attaches the Help menupane to the right side */
  n = 0;
  XtSetArg (args[n], XmNmenuHelpWidget, cascadeB5);  n++;
  XtSetValues (menuBar, args, n);


  return(menuBar);


}  /* End SetupHelpDialogMenus */




/*********************************************************************
 * Function:  CreatePopupMenu
 *
 *   This procedure builds the popup menu for the display area
 *
 **********************************************************************/
static Widget CreatePopupMenu(
    Widget nw)
{
   Widget       popup;
   Widget       c[10];
   Arg		args[10];	
   int          n;
   XmString     labelStr;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;
   DtHelpListStruct *pHelpInfo;
   char        *mnemonic;
   int numButtons = XGetPointerMapping(XtDisplay(nw),(unsigned char *)NULL, 0);

   /* Build the popup */
   /* If no Btn3 then use Btn2 to post the menu */
   n = 0;
   if (numButtons < 3) {
      XtSetArg(args[n], XmNwhichButton, Button2); n++;
   }
   popup = XmCreatePopupMenu (
		_DtHelpDisplayAreaWidget(hw->help_dialog.help.pDisplayArea),
                             "popup", args, n); 

   /* Add our button three event handler to our newly created display area */
   XtAddEventHandler(
		_DtHelpDisplayAreaWidget(hw->help_dialog.help.pDisplayArea), 
                    ButtonPressMask, FALSE, 
                    (XtEventHandler)ProcessBMenuBackCB, hw);

  /* Add Help callback for Popup Menu */
  pHelpInfo = _DtHelpListAdd(DtHELP_PopupMenu_STR,
                       (Widget) hw, &hw->help_dialog.help,
			&hw->help_dialog.help.pHelpListHead);
  XtAddCallback((Widget) popup, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);

   /* Build the popup menu items */

  /* Popup Label*/
   labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(3, 4,"Help")));
   n = 0; 
   XtSetArg(args[n], XmNlabelString, labelStr); n++;
   c[0] = XmCreateLabelGadget(popup, "popupLabel", args, n);
   XmStringFree(labelStr); 

   /* Place a menu break here  */
   n = 0;
   c[1] = XmCreateSeparatorGadget(popup, "seporator",args, n);

 
   /* Back Button */
   labelStr = XmStringCreateLocalized(((char *) _DTGETMESSAGE(2, 13,"Backtrack")));
   mnemonic = ((char *)_DTGETMESSAGE(2, 14,"B"));
   n = 0; 
   XtSetArg(args[n], XmNlabelString, labelStr); n++;
   XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
   c[2] =  hw->help_dialog.menu.popupBackBtn  = XmCreatePushButtonGadget(popup,
                                            "backTrack", args, n);
   XmStringFree(labelStr); 
 
   XtAddCallback(hw->help_dialog.menu.popupBackBtn,XmNactivateCallback, 
                   _DtHelpDisplayBackCB, (XtPointer) hw); 
   XtSetSensitive(hw->help_dialog.menu.popupBackBtn, FALSE);


   /* Top Button */
   labelStr = XmStringCreateLocalized(((char *)_DTGETMESSAGE(2, 11,"Home Topic")));
   mnemonic = ((char *)_DTGETMESSAGE(2, 12,"T"));
   n = 0; 
   XtSetArg(args[n], XmNlabelString, labelStr); n++;
   XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
   c[3] = hw->help_dialog.menu.popupTopBtn  = XmCreatePushButtonGadget(popup,
                                                     "homeTopic", args, n);
   XmStringFree(labelStr); 
   XtAddCallback(hw->help_dialog.menu.popupTopBtn,XmNactivateCallback, 
                  DisplayTopLevelCB, (XtPointer) hw); 
   
   XtManageChildren(c, 4);

  return(popup);


}
