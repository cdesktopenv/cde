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
/* $XConsortium: HelpQuickD.c /main/15 1996/08/29 14:34:56 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpQuickD.c
 **
 **   Project:      Cde Help 1.0 Project
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

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>

#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Frame.h>
#include <Xm/RepType.h>

/* Copied from Xm/GeoUtilsI.h */
extern XmGeoMatrix _XmGeoMatrixAlloc( 
                        unsigned int numRows,
                        unsigned int numBoxes,
                        unsigned int extSize) ;

/* Canvas Engine incudes */
#include "CanvasP.h"  /* for the link types */

/* Help Dialog Widget Includes */
#include "Access.h"
#include "bufioI.h"

#include <Dt/Help.h>
#include "HelpP.h"
#include "DisplayAreaI.h"
#include "StringFuncsI.h"
#include "HelpQuickDP.h"
#include "HelpQuickDI.h"
#include "HelpQuickD.h"
#include "HelposI.h"
#include "HelpAccessI.h"
#include "Lock.h"

/* Display Area Includes */
#include "ActionsI.h"
#include "HelpI.h"
#include "CallbacksI.h"
#include "DestroyI.h"
#include "FormatI.h"
#include "HelpDialogI.h"
#include "HourGlassI.h"
#include "HyperTextI.h"
#include "ResizeI.h"
#include "FormatManI.h"
#include "HelpUtilI.h"
#include "MessagesP.h"
#include "SetListI.h"
#include "XUICreateI.h"
#include "FileUtilsI.h"

/* print dialogs */
#include "PrintI.h"

/* message catalog set */
#define HQSET  11

/* Quick Help Dialog Error message Defines */
#define QHDMessage1     _DtHelpMsg_0008
#define QHDMessage2     _DtHelpMsg_0007
#define QHDMessage3     _DtHelpMsg_0000
#define QHDMessage4     _DtHelpMsg_0001
#define QHDMessage5     _DtHelpMsg_0002
#define QHDMessage6     _DtHelpMsg_0003
#define QHDMessage7     _DtHelpMsg_0004
#define QHDMessage8     _DtHelpMsg_0010
#define QHDMessage9     _DtHelpMsg_0009
#define QHDMessage10    _DtHelpMsg_0005


/********    Static Function Declarations    ********/

static void NavigationTypeDefault( 
                        Widget widget,
                        int offset,
                        XrmValue *value) ;

static void ClassPartInitialize( 
                        WidgetClass wc);
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
static void CloseQuickCB (
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData);
static void HelpButtonCB (
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData);
static void PrintQuickHelpCB (
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData);
static void VariableInitialize(
                        DtHelpQuickDialogWidget nw);
static void FreeQuickHelpInfo(
                        Widget nw,
                        int cleanUpKind);
static void CatchClose(Widget widget); 
static void SetupTopic(
                       Widget nw,
                       int updateKind);
static void SetupDisplayType (
                       Widget nw,
                       int updateKind);
static void  ProcessJumpReuse(
                        Widget nw,
                        DtHelpHyperTextStruct *hyperData);
static void  ProcessBackCB(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData );
static void UpdateJumpList(
                        char *topicInfo,
                        int topicType,
                        Widget nw);
static void ResizeQuickDialogCB (
                        XtPointer clientData);
static void InitialPopupCB(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData);


/********    End Static Function Declarations    ********/



/* Static variables */
static char helpOnHelp[] = "Help4Help";


/* Supported resources for the HelpQuickDialog Widget */

static XtResource resources[] = {

    {   DtNminimizeButtons,
        DtCMinimizeButtons,
        XmRBoolean,
        sizeof(Boolean),
        XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.minimize_buttons),
        XmRImmediate,
        (XtPointer) False
    },

    {
        DtNscrollBarPolicy,
        DtCScrollBarPolicy, DtRDtScrollBarPolicy, sizeof (unsigned char),
        XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.scrollBarPolicy),
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
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.textColumns), 
	XmRImmediate, (XtPointer) 50
     },
    
     {	DtNrows, 
        DtCRows, XmRShort, sizeof(short), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.textRows), 
	XmRImmediate, (XtPointer) 15
     },

     {	DtNlocationId, 
	DtCLocationId, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.locationId), 
	XmRImmediate, (XtPointer) _DtHelpDefaultLocationId
      }, 

     {	DtNhelpPrint, 
	DtCHelpPrint, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.print.helpPrint), 
	XmRImmediate, (XtPointer) _DtHelpDefaultHelpPrint
      }, 

     {	DtNprinter, 
	DtCPrinter, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.print.printer), 
	XmRImmediate, (XtPointer) NULL
      }, 

     {  DtNpaperSize,
        DtCPaperSize, DtRDtPaperSize, sizeof (unsigned char),
        XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.print.paperSize),
        XmRImmediate, (XtPointer) DtHELP_PAPERSIZE_LETTER
      },

     {	DtNhelpVolume, 
	DtCHelpVolume, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.helpVolume), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNmanPage, 
	DtCManPage, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.manPage), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNstringData, 
	DtCStringData, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.stringData), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNhelpFile, 
	DtCHelpFile, XmRString, sizeof (char*), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.helpFile), 
	XmRImmediate, (XtPointer) NULL
      }, 

      { DtNtopicTitle,
        DtCTopicTitle, XmRString, sizeof (char*),
        XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.topicTitleStr),
        XmRImmediate, (XtPointer) NULL
      },

      {	DtNhelpType, 
        DtCHelpType, DtRDtHelpType, sizeof(unsigned char), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.helpType), 
	XmRImmediate, (XtPointer) DtHELP_TYPE_TOPIC
      },
     
      { DtNhelpOnHelpVolume,
        DtCHelpOnHelpVolume, XmRString, sizeof (char*),
        XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.help.helpOnHelpVolume),
        XmRImmediate, (XtPointer) _DtHelpDefaultHelp4HelpVolume
      },

      {	DtNhyperLinkCallback, 
	DtCHyperLinkCallback, XmRCallback, sizeof (XtCallbackList), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.display.hyperLinkCallback), 
	XmRImmediate, (XtPointer) NULL
      }, 

      {	DtNcloseCallback, 
	DtCCloseCallback, XmRCallback, sizeof (XtCallbackList), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.closeCallback), 
	XmRImmediate, (XtPointer) NULL
      },

      {	DtNcloseLabelString,
        DtCCloseLabelString, XmRXmString, sizeof (XmString), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.closeLabelString), 
	XmRString, NULL
      }, 
 
      {	DtNmoreLabelString,
        DtCMoreLabelString, XmRXmString, sizeof (XmString), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.moreLabelString), 
	XmRString, NULL
      }, 
 
      {	DtNbackLabelString,
        DtCBackLabelString, XmRXmString, sizeof (XmString), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.backLabelString), 
	XmRString, NULL
      }, 

      {	DtNhelpLabelString, 
        DtCHelpLabelString, XmRXmString, sizeof (XmString), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.helpLabelString), 
	XmRString, NULL
	}, 
      
      {	DtNprintLabelString, 
        DtCPrintLabelString, XmRXmString, sizeof (XmString), 
	XtOffset (DtHelpQuickDialogWidget, qhelp_dialog.qhelp.printLabelString), 
	XmRString, NULL
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

externaldef( dthelpquickdialogwidgetclassrec) DtHelpQuickDialogWidgetClassRec dtHelpQuickDialogWidgetClassRec =
{
   {                                            /* core_class fields  */
      (WidgetClass) &xmBulletinBoardClassRec,   /* superclass         */
      "DtHelpQuickDialog",                      /* class_name         */
      sizeof(DtHelpQuickDialogWidgetRec),       /* widget_size        */
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
      _DtHelpQuickDialogWidgetGeoMatrixCreate,      /* geo__matrix_create */
      XmInheritFocusMovedProc,                  /* focus_moved_proc */
      NULL                                      /* extension */
   },   

   {                                            /* messageBox class - none */
      0                                         /* mumble */
   }    
};

externaldef( dthelpquickdialogwidgetclass) WidgetClass 
          dtHelpQuickDialogWidgetClass = 
                       (WidgetClass) &dtHelpQuickDialogWidgetClassRec;


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

  /* _XmFastSubclassInit (widgetClass, XmTEMPLATE_BOX_BIT); */

    return ;
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

static void ClassInitialize(
    void)
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
 * Function:	    static void VariableInitialize()
 *                      
 *
 * Return Value:    Void.
 *
 *
 * Purpose: 	    This routine initializes all global variables to valid
 *                  starting values.
 *
 *****************************************************************************/
static void VariableInitialize(
DtHelpQuickDialogWidget nw)
{

  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw ;

  /* Set our current topic variables to initial values */
  _DtHelpCommonHelpInit(&qw->qhelp_dialog.help);

  /* set inherited values */
  qw->bulletin_board.auto_unmanage  = FALSE;
  qw->bulletin_board.resize_policy  = XmRESIZE_NONE;

  /* Set display values */
  /* Make local copies of all resource strings assigned by the user */
  if (qw->qhelp_dialog.display.locationId != NULL)
    qw->qhelp_dialog.display.locationId = XtNewString(qw->qhelp_dialog.display.locationId);
  if (qw->qhelp_dialog.display.helpVolume != NULL)
     qw->qhelp_dialog.display.helpVolume =
                          XtNewString(qw->qhelp_dialog.display.helpVolume);
  if (qw->qhelp_dialog.display.manPage != NULL)
    qw->qhelp_dialog.display.manPage = XtNewString(qw->qhelp_dialog.display.manPage);
  if (qw->qhelp_dialog.display.stringData != NULL)
    qw->qhelp_dialog.display.stringData = XtNewString(qw->qhelp_dialog.display.stringData);
  if (qw->qhelp_dialog.display.helpFile != NULL)
    qw->qhelp_dialog.display.helpFile = XtNewString(qw->qhelp_dialog.display.helpFile);

  /* Initialize the topic title variables. */
  qw->qhelp_dialog.display.topicTitleLbl = NULL;
  if (qw->qhelp_dialog.display.topicTitleStr != NULL)
    qw->qhelp_dialog.display.topicTitleLbl = XmStringCreateLocalized(
					qw->qhelp_dialog.display.topicTitleStr);

  /* Set our volume handle to an NULL initial value */
  qw->qhelp_dialog.display.volumeHandle = NULL;

  /* setup print stuff */
  _DtHelpInitPrintStuff(&qw->qhelp_dialog.print);


 /* Set our map flag: true after we hit our popup callback, 
   * false otherwise
   */
  qw->qhelp_dialog.display.firstTimePopupFlag = FALSE;


  /* Set our jump list display stuff to initial values */
  qw->qhelp_dialog.backtr.pJumpListHead     = NULL;
  qw->qhelp_dialog.backtr.pJumpListTale     = NULL;
  qw->qhelp_dialog.backtr.totalJumpNodes    = 0;
  qw->qhelp_dialog.backtr.scrollPosition    = -1;
 
  /* Set our help dialog widgets to NULL starting values */
  qw->qhelp_dialog.qhelp.separator          = NULL;
  qw->qhelp_dialog.qhelp.displayAreaFrame   = NULL;
  qw->qhelp_dialog.qhelp.closeButton        = NULL;
  qw->qhelp_dialog.qhelp.helpButton         = NULL;
  qw->qhelp_dialog.qhelp.printButton        = NULL;
  qw->qhelp_dialog.qhelp.moreButton         = NULL;
  qw->qhelp_dialog.qhelp.backButton         = NULL;
  qw->qhelp_dialog.qhelp.definitionBox      = NULL;
}


/*****************************************************************************
 * Function:	    FilterExecCmdCB
 *
 *  clientData:     The quick help dialog widget
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
static int FilterExecCmdCB(
    void *   clientData,
    const char *   cmdStr,
    char * * ret_filteredCmdStr)
{
     DtHelpQuickDialogWidget qw;
     char    *hv_path;

     qw = (DtHelpQuickDialogWidget) _DtHelpDisplayAreaData(clientData);

     hv_path = _DtHelpFileLocate(DtHelpVOLUME_TYPE,
				 qw->qhelp_dialog.display.helpVolume,
				 _DtHelpFileSuffixList, False, R_OK);
     return _DtHelpFilterExecCmd((Widget) qw, cmdStr, 
                  qw->qhelp_dialog.display.executionPolicy, 
                  True, &qw->qhelp_dialog.help, ret_filteredCmdStr, hv_path);
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
    Cardinal *num_args )
{

  Arg		args[10];	/*  arg list		*/
  int      	n;		/*  arg count		*/
  XmFontList  defaultList;
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw ;
  DtHelpListStruct *pHelpInfo;

  /* Local variables */
  XmString          labelStr;

  /* Initialize all global variables */
  VariableInitialize(qw);

  /* Setup a frame to hold our display area */
   n = 0;
   XtSetArg(args[n], XmNshadowThickness, 0);		++n;
   qw->qhelp_dialog.qhelp.displayAreaFrame = 
             XmCreateFrame ((Widget)qw, "displayAreaFrame", args, n);
   XtManageChild (qw->qhelp_dialog.qhelp.displayAreaFrame);
   

  /*  Create a separator between the buttons  */
  n = 0;
  qw->qhelp_dialog.qhelp.separator = 
           XmCreateSeparatorGadget ((Widget)qw, "separator", args, n);
  XtManageChild (qw->qhelp_dialog.qhelp.separator);


  /* Setup the control buttons along the bottom */

  /* Close button */

  if (qw->qhelp_dialog.qhelp.closeLabelString != NULL)
    labelStr = XmStringCopy(qw->qhelp_dialog.qhelp.closeLabelString);
  else
    labelStr = XmStringCreateLocalized(
           (char *)_DTGETMESSAGE(HQSET,1,"Close"));

  n=0;
  XtSetArg(args[n], XmNlabelString, labelStr);		        ++n;
  qw->qhelp_dialog.qhelp.closeButton = 
             XmCreatePushButtonGadget((Widget)qw, "closeButton", args, n);
  XtManageChild (qw->qhelp_dialog.qhelp.closeButton);
  XtAddCallback(qw->qhelp_dialog.qhelp.closeButton,XmNactivateCallback, 
                 CloseQuickCB, (XtPointer) qw); 
  XmStringFree(labelStr);

  /* set the cancel button (for KCancel) */
  qw->bulletin_board.cancel_button= qw->qhelp_dialog.qhelp.closeButton;
  
  /* Set the close button as the default button */
  XtSetArg (args[0], XmNdefaultButton, qw->qhelp_dialog.qhelp.closeButton);
  XtSetValues ((Widget)qw, args, 1);


  /* More button: We do not manage this button, the user must do that */

  if (qw->qhelp_dialog.qhelp.moreLabelString != NULL)
    labelStr = XmStringCopy(qw->qhelp_dialog.qhelp.moreLabelString);
  else
    labelStr = XmStringCreateLocalized(
           (char *)_DTGETMESSAGE(HQSET,2,"More ..."));

  n=0;
  XtSetArg(args[n], XmNlabelString, labelStr);		        ++n;
  qw->qhelp_dialog.qhelp.moreButton = 
             XmCreatePushButtonGadget((Widget)qw, "moreButton", args, n);
  XmStringFree(labelStr);


  /* Back button */

  if (qw->qhelp_dialog.qhelp.backLabelString != NULL)
    labelStr = XmStringCopy(qw->qhelp_dialog.qhelp.backLabelString);
  else
    labelStr = XmStringCreateLocalized(
           (char *)_DTGETMESSAGE(HQSET,3,"Backtrack"));

  n=0;
  XtSetArg(args[n], XmNlabelString, labelStr);		        ++n;
  qw->qhelp_dialog.qhelp.backButton = 
             XmCreatePushButtonGadget((Widget)qw, "backButton", args, n);
  XtManageChild (qw->qhelp_dialog.qhelp.backButton);
  XtAddCallback(qw->qhelp_dialog.qhelp.backButton,XmNactivateCallback, 
                 ProcessBackCB, (XtPointer) qw); 
  XmStringFree(labelStr);

  XtSetSensitive(qw->qhelp_dialog.qhelp.backButton, FALSE);



  /* Print button */
  if (qw->qhelp_dialog.qhelp.printLabelString != NULL)
    labelStr = XmStringCopy(qw->qhelp_dialog.qhelp.printLabelString);
  else
    labelStr = XmStringCreateLocalized(
           (char *)_DTGETMESSAGE(HQSET,4,"Print ..."));  

  n=0;
  XtSetArg(args[n], XmNlabelString, labelStr);		        ++n;
  qw->qhelp_dialog.qhelp.printButton = 
             XmCreatePushButtonGadget((Widget)qw, "printButton", args, n);
  XtManageChild (qw->qhelp_dialog.qhelp.printButton);
  
  XtAddCallback(qw->qhelp_dialog.qhelp.printButton,XmNactivateCallback, 
                PrintQuickHelpCB, (XtPointer) qw); 
  XmStringFree(labelStr); 
  

  /* Help Button */

  if (qw->qhelp_dialog.qhelp.helpLabelString != NULL)
    labelStr = XmStringCopy(qw->qhelp_dialog.qhelp.helpLabelString);
  else
    labelStr = XmStringCreateLocalized(
           (char *)_DTGETMESSAGE(HQSET,5,"Help ..."));

  n=0;
  XtSetArg(args[n], XmNlabelString, labelStr); 		++n;
  qw->qhelp_dialog.qhelp.helpButton = 
        XmCreatePushButtonGadget((Widget)qw,"helpButton", args, n);
  XtManageChild(qw->qhelp_dialog.qhelp.helpButton);
  /* Now remove BulletinBoard Unmanage callback from apply and help buttons. */
  XtRemoveAllCallbacks( qw->qhelp_dialog.qhelp.helpButton, XmNactivateCallback) ;
  /* and add ours */

  pHelpInfo = _DtHelpListAdd(DtHELP_quickHelpBtn_STR,
                        (Widget) qw, &qw->qhelp_dialog.help,
                        &qw->qhelp_dialog.help.pHelpListHead);
  XtAddCallback( qw->qhelp_dialog.qhelp.helpButton, XmNactivateCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);
  XmStringFree(labelStr);

  /* Shell help */
  pHelpInfo = _DtHelpListAdd(DtHELP_quickHelpShell_STR,
                        (Widget) qw, &qw->qhelp_dialog.help,
                        &qw->qhelp_dialog.help.pHelpListHead);
  XtAddCallback((Widget) qw, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);

  /* Get our current fontlist value */
  n = 0;
  XtSetArg (args[n], XmNfontList, &(defaultList));  ++n;
  XtGetValues (qw->qhelp_dialog.qhelp.closeButton, args, n);


  /* Build the Display Area */
  qw->qhelp_dialog.help.pDisplayArea = _DtHelpCreateDisplayArea
                                   ((Widget)qw->qhelp_dialog.qhelp.displayAreaFrame,
                                   "DisplayArea",
                                   ((short) qw->qhelp_dialog.display.scrollBarPolicy),
                                   ((short) qw->qhelp_dialog.display.scrollBarPolicy),
				   False,
                                   ((int) qw->qhelp_dialog.display.textRows),
                                   ((int) qw->qhelp_dialog.display.textColumns),
                                   _DtHelpQuickDialogHypertextCB,
				   ResizeQuickDialogCB,
                                   FilterExecCmdCB,
                                   (XtPointer) qw,
                                   defaultList);

  
  /* Now Validate our incomming help requests topics */
  SetupDisplayType((Widget)qw ,DtJUMP_UPDATE);

  /* Just for fun, lets make sure our sizes are correct */
  XtAddCallback (XtParent(qw), XmNpopupCallback, (XtCallbackProc)
                 InitialPopupCB, (XtPointer) qw);


 
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
   DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) clientData ;
   
 
   /* set our firstTimePopupFlag to TRUE because we map it right 
      after this call */
   qw->qhelp_dialog.display.firstTimePopupFlag = TRUE;

  _DtHelpResizeDisplayArea (XtParent(qw),
			     qw->qhelp_dialog.help.pDisplayArea, 
                             qw->qhelp_dialog.display.textRows,
                             qw->qhelp_dialog.display.textColumns);

  XtRemoveCallback (XtParent(qw), XmNpopupCallback, (XtCallbackProc)
                  InitialPopupCB, (XtPointer) qw);

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
 
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw ;
  DtHelpQuickDialogWidget current = (DtHelpQuickDialogWidget) cw ;
  Boolean updateRequest=FALSE;

  /* Setup some initial argument values we know we need on the B-board */
  qw->bulletin_board.auto_unmanage  = FALSE;
  qw->bulletin_board.resize_policy  = XmRESIZE_NONE;
 
  /* Check DtNcolumns & or DtNrows  resource for change */
  if ((current->qhelp_dialog.display.textRows != qw->qhelp_dialog.display.textRows) ||
      (current->qhelp_dialog.display.textColumns != qw->qhelp_dialog.display.textColumns))
    {
      /* Perform a resize on our display area */
      _DtHelpResizeDisplayArea (XtParent(qw),
			     qw->qhelp_dialog.help.pDisplayArea, 
                             qw->qhelp_dialog.display.textRows,
                             qw->qhelp_dialog.display.textColumns);

    }

  /* Check DtNhelpVolume resource for change */
  if (current->qhelp_dialog.display.helpVolume != qw->qhelp_dialog.display.helpVolume) 
    {
      qw->qhelp_dialog.display.helpVolume = XtNewString(qw->qhelp_dialog.display.helpVolume);
      XtFree(current->qhelp_dialog.display.helpVolume);
      updateRequest = TRUE;
    }


  /* Check DtNlocationId resource for change */
  if (current->qhelp_dialog.display.locationId != qw->qhelp_dialog.display.locationId) 
    {
      qw->qhelp_dialog.display.locationId = XtNewString(qw->qhelp_dialog.display.locationId);
      XtFree(current->qhelp_dialog.display.locationId);
      updateRequest = TRUE;
    }

   /* set the printing resources */
   _DtHelpPrintSetValues(&current->qhelp_dialog.print,&qw->qhelp_dialog.print,
             &qw->qhelp_dialog.display,&qw->qhelp_dialog.help);

  /* Check DtNmanPage resource for change */
  if (current->qhelp_dialog.display.manPage != qw->qhelp_dialog.display.manPage) 
    {
      qw->qhelp_dialog.display.manPage = XtNewString(qw->qhelp_dialog.display.manPage);
      XtFree(current->qhelp_dialog.display.manPage);
      updateRequest = TRUE;
    }

  /* Check DtNstringData resource for change */
  if (current->qhelp_dialog.display.stringData != qw->qhelp_dialog.display.stringData) 
    {
      qw->qhelp_dialog.display.stringData = XtNewString(qw->qhelp_dialog.display.stringData);
      XtFree(current->qhelp_dialog.display.stringData);
      updateRequest = TRUE;
    }


  /* Check DtNhelpFile resource for change */
  if (current->qhelp_dialog.display.helpFile != qw->qhelp_dialog.display.helpFile) 
    {
      qw->qhelp_dialog.display.helpFile = XtNewString(qw->qhelp_dialog.display.helpFile);
      XtFree(current->qhelp_dialog.display.helpFile);
      updateRequest = TRUE;
    }

   
    /* Check and modify if required any of the push button labels */

    /* Check DtNcloseLabelString resource for change */
    if (current->qhelp_dialog.qhelp.closeLabelString != 
                                  qw->qhelp_dialog.qhelp.closeLabelString) 
      {
        qw->qhelp_dialog.qhelp.closeLabelString =
                    XmStringCopy(qw->qhelp_dialog.qhelp.closeLabelString);
        XmStringFree(current->qhelp_dialog.qhelp.closeLabelString);
        
        XtSetArg(args[0], XmNlabelString, qw->qhelp_dialog.qhelp.closeLabelString); 
        XtSetValues(qw->qhelp_dialog.qhelp.closeButton, args, 1);
      }

    /* Check DtNhelpLabelString resource for change */
    if (current->qhelp_dialog.qhelp.helpLabelString != 
                                  qw->qhelp_dialog.qhelp.helpLabelString) 
      {
        qw->qhelp_dialog.qhelp.helpLabelString =
                    XmStringCopy(qw->qhelp_dialog.qhelp.helpLabelString);
        XmStringFree(current->qhelp_dialog.qhelp.helpLabelString);
        
        XtSetArg(args[0], XmNlabelString, qw->qhelp_dialog.qhelp.helpLabelString); 
        XtSetValues(qw->qhelp_dialog.qhelp.helpButton, args, 1);
      }

    /* Check DtNmoreLabelString resource for change */
    if (current->qhelp_dialog.qhelp.moreLabelString != 
                                  qw->qhelp_dialog.qhelp.moreLabelString) 
      {
        qw->qhelp_dialog.qhelp.moreLabelString =
                    XmStringCopy(qw->qhelp_dialog.qhelp.moreLabelString);
        XmStringFree(current->qhelp_dialog.qhelp.moreLabelString);
        
        XtSetArg(args[0], XmNlabelString, qw->qhelp_dialog.qhelp.moreLabelString); 
        XtSetValues(qw->qhelp_dialog.qhelp.moreButton, args, 1);
      }

    /* Check DtNbackLabelString resource for change */
    if (current->qhelp_dialog.qhelp.backLabelString != 
                                  qw->qhelp_dialog.qhelp.backLabelString) 
      {
        qw->qhelp_dialog.qhelp.backLabelString =
                    XmStringCopy(qw->qhelp_dialog.qhelp.backLabelString);
        XmStringFree(current->qhelp_dialog.qhelp.backLabelString);
        
        XtSetArg(args[0], XmNlabelString, qw->qhelp_dialog.qhelp.backLabelString); 
        XtSetValues(qw->qhelp_dialog.qhelp.backButton, args, 1);
      }

    /* Check DtNprintLabelString resource for change */
    if (current->qhelp_dialog.qhelp.printLabelString != 
                                  qw->qhelp_dialog.qhelp.printLabelString) 
      {
        qw->qhelp_dialog.qhelp.printLabelString =
                    XmStringCopy(qw->qhelp_dialog.qhelp.printLabelString);
        XmStringFree(current->qhelp_dialog.qhelp.printLabelString);
        
        XtSetArg(args[0], XmNlabelString, qw->qhelp_dialog.qhelp.printLabelString); 
        XtSetValues(qw->qhelp_dialog.qhelp.printButton, args, 1);
      }


 
  /* Check the help type for change */
  if ((current->qhelp_dialog.display.helpType != qw->qhelp_dialog.display.helpType) ||
     (updateRequest))
    {
      /* Setup and display our new topic */
      SetupDisplayType((Widget)qw, DtJUMP_UPDATE);
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
   DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) w;

     /* This routine will clean up all malloc'ed stuff in our instance 
      * structure.  It does not remove any callbacks or delete any of the
      * widgets created in this instance of the help dialog.
      *
      * If destroy is being called, then the Display Area destroy has
      * already been called and the canvas has been freed. Therefore
      * pass in NULL for canvas type.
      */

     FreeQuickHelpInfo((Widget)qw, DtCLEAN_FOR_DESTROY);
 
     /* Remove any of the callbacks added to the help dialog ??? */


}


#ifdef	NOTDONE
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
#if XmREVISION >= 2
    XmGeoMajorLayout layoutPtr,
#else
    XmGeoRowLayout layoutPtr,
#endif
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
#endif

/*****************************************************************************
 * Function:	    static void SeparatorFix( 
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
 *                which consist of a single separator widget.  The effect of
 *                this routine is to have the separator ignore the margin
 *                width.
 *
 *****************************************************************************/
/*ARGSUSED*/
static void 
SeparatorFix(
        XmGeoMatrix geoSpec,
        int action,
        XmGeoMajorLayout layoutPtr, /* unused */
        XmKidGeometry rowPtr )
{
    register Dimension       marginW ;
    register Dimension       twoMarginW ;

    marginW = geoSpec->margin_w ;
    twoMarginW = (marginW << 1) ;

    switch(    action    )
    {   
        case XmGEO_PRE_SET:
        {   rowPtr->box.x -= marginW ;
            rowPtr->box.width += twoMarginW ;
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
 * Function:	     XmGeoMatrix _DtHelpQuickDialogWidgeGeoMatrixCreate(
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
XmGeoMatrix _DtHelpQuickDialogWidgetGeoMatrixCreate(
    Widget wid,
    Widget instigator,
    XtWidgetGeometry *desired )
{
 

             DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) wid ;
             XmGeoMatrix     geoSpec ;
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   boxPtr ;
             XmKidGeometry   firstButtonBox ;
#ifdef	NOTDONE
             int fix_menubar = False ;
#endif

    /* Replace the value "10" for the marginWidth resource when avail */

    geoSpec = _XmGeoMatrixAlloc( TB_MAX_WIDGETS_VERT, TB_MAX_NUM_WIDGETS, 0) ;
    geoSpec->composite = (Widget) qw ;
    geoSpec->instigator = (Widget) instigator ;
    if(    desired    )
    {   geoSpec->instig_request = *desired ;
        } 
    geoSpec->margin_w = 10 + qw->manager.shadow_thickness ;
    geoSpec->margin_h = 10 + qw->manager.shadow_thickness ;
    geoSpec->no_geo_request = _DtHelpQuickDialogWidgetNoGeoRequest ;

    layoutPtr = (XmGeoRowLayout)geoSpec->layouts ;
    boxPtr = geoSpec->boxes ;

     
    /* Display area setup stuff */
    if( _XmGeoSetupKid(boxPtr, qw->qhelp_dialog.qhelp.displayAreaFrame))
    {   
        layoutPtr->space_above = 5;
        layoutPtr->stretch_height = TRUE ;
        layoutPtr->min_height = 100 ;
        boxPtr += 2 ;       /* For new row, add 2. */
        ++layoutPtr ;       /* For new row. */
        } 


    /* Separator area setup stuff */
    if(    _XmGeoSetupKid(boxPtr, qw->qhelp_dialog.qhelp.separator))
    {   layoutPtr->fix_up = SeparatorFix ;
        layoutPtr->space_above = 10;
        boxPtr += 2 ;       /* For new row, add 2. */
        ++layoutPtr ;       /* For new row. */
        } 
    firstButtonBox = boxPtr ;
    if(    _XmGeoSetupKid( boxPtr, qw->qhelp_dialog.qhelp.closeButton)    )
    {   ++boxPtr ;
        } 
   if(    _XmGeoSetupKid( boxPtr, qw->qhelp_dialog.qhelp.moreButton)    )
    {   ++boxPtr ;
        } 
   if(    _XmGeoSetupKid( boxPtr, qw->qhelp_dialog.qhelp.backButton)    )
    {   ++boxPtr ;
        } 
    if(    _XmGeoSetupKid( boxPtr, qw->qhelp_dialog.qhelp.printButton)    )
    {   ++boxPtr ;
        } 
    if(    _XmGeoSetupKid( boxPtr, qw->qhelp_dialog.qhelp.helpButton)    )
    {   ++boxPtr ;
        } 
   
    if(    boxPtr != firstButtonBox    )
    {   /* Had at least one button.
        */
        layoutPtr->fill_mode = XmGEO_CENTER ;
        layoutPtr->fit_mode = XmGEO_WRAP ;
        layoutPtr->space_above = 10;
        if(    !(qw->qhelp_dialog.qhelp.minimize_buttons)    )
        {   layoutPtr->even_width = 1 ;
            } 
        layoutPtr->even_height = 1 ;
	++layoutPtr ;
        } 
    layoutPtr->space_above = 10 ;
    layoutPtr->end = TRUE ;        /* Mark the last row. */
    return( geoSpec) ;
}




/*****************************************************************************
 * Function:	     Boolean _DtHelpQuickDialogWidgetNoGeoRequest(geoSpec)
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
Boolean _DtHelpQuickDialogWidgetNoGeoRequest(XmGeoMatrix geoSpec)
{

  if(    BB_InSetValues( geoSpec->composite)
      && (XtClass( geoSpec->composite) == dtHelpQuickDialogWidgetClass)    )
    {   
      return( TRUE) ;
    } 
  return( FALSE) ;
}




/*****************************************************************************
 * Function:	    Widget DtCreateHelpQuickDialog(Widget parent,
 *                                             String name,
 *                                             ArgList arglist,
 *                                             Cardinal argcount);
 *
 * Parameters:      parent      Specifies the parent widget ID.
 *                  name        Specifies the name of the created BB widget.
 *                  arglis      Specifies the argument list.
 *                  argcount    Specifies the number of attribute/value pairs
 *                              in the argument list (arglist).
 *
 * Return Value:    Returns a Bulletin Board widget ID, that correlates to
 *                  the top level child in the help dialog.
 *
 * Purpose: 	    Create an instance of a Help Dialog.
 *
 *****************************************************************************/
Widget DtCreateHelpQuickDialog(
    Widget parent,
    char *name,
    ArgList al,
    Cardinal ac)
{
    Widget w;
    _DtHelpWidgetToAppContext(parent);
    
    _DtHelpAppLock(app);
    w = XmeCreateClassDialog (dtHelpQuickDialogWidgetClass, parent, 
			      name, al, ac);

    /* Add the CatchClose here so we catch the window manager close requests */
    CatchClose(w);

    _DtHelpAppUnlock(app);
    return w;
}




/*****************************************************************************
 * Function:	    Widget DtHelpQuickDialogGetChild(Widget parent,
 *                                                unsigned char child);
 *
 * Parameters:      parent      Specifies the parent widget ID.
 *                  child       Specifies the child widget to return.
 *
 * Return Value:    Returns the requested widet id.
 *
 * Purpose: 	    Gives developers access to the quick help dialogs
 *                  children.
 *
 *****************************************************************************/
Widget DtHelpQuickDialogGetChild(
        Widget widget,
        unsigned char child )
{
    DtHelpQuickDialogWidget  w = (DtHelpQuickDialogWidget)widget;
    Widget result = NULL;

    _DtHelpWidgetToAppContext(widget);

    _DtHelpAppLock(app);
    switch (child) 
      {
      case DtHELP_QUICK_PRINT_BUTTON:
          result = (w->qhelp_dialog.qhelp.printButton);
	  break;
          
        case DtHELP_QUICK_MORE_BUTTON: 
          result = (w->qhelp_dialog.qhelp.moreButton);
	  break;
   
        case DtHELP_QUICK_BACK_BUTTON: 
	  result = (w->qhelp_dialog.qhelp.backButton);
	  break;
  
        case DtHELP_QUICK_CLOSE_BUTTON: 
          result = (w->qhelp_dialog.qhelp.closeButton);
	  break;
           
        case DtHELP_QUICK_HELP_BUTTON:
          result = (w->qhelp_dialog.qhelp.helpButton);
	  break;
         
        case DtHELP_QUICK_SEPARATOR:  
          result = (w->qhelp_dialog.qhelp.separator);
	  break;

        default: 
          XmeWarning( (Widget) w, (char*)QHDMessage1); 
          break;
       }  /* End of switch statement */

    _DtHelpAppUnlock(app);
    return result;
}




/*****************************************************************************
 * Function:	    void _DtHelpQuickDialogHypertextCB(
 *                              DtHelpDispAreaStruct *pDisplayAreaStruct,
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
void _DtHelpQuickDialogHypertextCB (
    XtPointer pDisplayAreaStruct,
    XtPointer clientData,
    DtHelpHyperTextStruct *hyperData)
{
  DtHelpDialogCallbackStruct callData;
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) clientData ;
  char         *pTempAccessPath;
  char         *pTempLocationId;  
  char         *tmpErrorMsg;
  

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
               ProcessJumpReuse((Widget)qw, hyperData);
             break;

             case _DtCvWindowHint_NewWindow:
        
               _DtHelpTurnOnHourGlass(XtParent(qw));
  
               if (qw->qhelp_dialog.display.hyperLinkCallback != NULL)
                 {
                    pTempAccessPath = 
                      _DtHelpParseAccessFile(hyperData->specification);
         
                    if (pTempAccessPath != NULL)       /* Use the New one */
                      callData.helpVolume = pTempAccessPath;
                    else                               /* Use the old one */
                      callData.helpVolume =
                                XtNewString(qw->qhelp_dialog.display.helpVolume);


                    callData.reason        = DtCR_HELP_LINK_ACTIVATE;
                    callData.event         = hyperData->event;
                    callData.locationId    = _DtHelpParseIdString
                                                (hyperData->specification);
                    callData.specification = NULL;
                    callData.hyperType     = DtHELP_LINK_TOPIC;
                    callData.windowHint    = DtHELP_NEW_WINDOW;

                    XtCallCallbackList(
                     (Widget)qw,qw->qhelp_dialog.display.hyperLinkCallback,&callData);
	         }
               else
                 {  /* The application did not register a hypertext callback so
                     *  we must generate the proper warning message and 
                     *  continue!
                     */
                    XmeWarning((Widget)qw, (char*) QHDMessage1); 

                    /* Call jump-reuse procedure for default */
                    ProcessJumpReuse((Widget)qw, hyperData);
                  }
	      
               _DtHelpTurnOffHourGlass(XtParent(qw));       
 
             break;

             case _DtCvWindowHint_PopupWindow:

               _DtHelpTurnOnHourGlass(XtParent(qw));

               pTempAccessPath = 
                         _DtHelpParseAccessFile(hyperData->specification);
         
               if (pTempAccessPath == NULL)  /* Use the old one */  
                 pTempAccessPath = XtNewString(qw->qhelp_dialog.display.helpVolume);

               pTempLocationId = _DtHelpParseIdString(hyperData->specification);

               _DtHelpDisplayDefinitionBox((Widget)qw,
                                 (Widget **)&(qw->qhelp_dialog.qhelp.definitionBox),
                                 pTempAccessPath, pTempLocationId);

               XtFree(pTempLocationId);
               XtFree(pTempAccessPath);

               _DtHelpTurnOffHourGlass(XtParent(qw));       

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
          _DtHelpExecFilteredCmd((Widget) qw,
                    hyperData->specification, DtHELP_ExecutionPolicy_STR,
                    &qw->qhelp_dialog.display, &qw->qhelp_dialog.help);
        break;


      case _DtCvLinkType_ManPage:
      case _DtCvLinkType_AppDefine:	
      case _DtCvLinkType_TextFile:
     	       
        /* Process Application Man page link or App Defined link types */
        _DtHelpTurnOnHourGlass(XtParent(qw));

        /* If the application registered a hypertext callback use it! */
        if (qw->qhelp_dialog.display.hyperLinkCallback != NULL)
          {
  
            /* Setup the DtHyperProcStructer to pass back to the 
             * client as callData. 
             */
             callData.reason         = DtCR_HELP_LINK_ACTIVATE;
             callData.event          = hyperData->event;
             callData.locationId     = NULL;
             callData.helpVolume     = NULL;
	     if (_DtCvLinkType_AppDefine == hyperData->hyper_type)
	       {
		 callData.locationId =
				_DtHelpParseIdString(hyperData->specification);
		 callData.helpVolume =
			       XtNewString(qw->qhelp_dialog.display.helpVolume);
	       }
             callData.specification  = hyperData->specification;
             callData.hyperType      = hyperData->hyper_type;
             callData.windowHint     = hyperData->window_hint;

             XtCallCallbackList ((Widget) qw,
                             qw->qhelp_dialog.display.hyperLinkCallback, &callData);
	  }
        else
          { 
             /* The application did not register a hypertext 
              * callback so we must generate the proper error 
              * message and continue!
              */
              XmeWarning((Widget)qw, (char*) QHDMessage1); 

              if (hyperData->hyper_type == DtHELP_LINK_APP_DEFINE)
                {
                  tmpErrorMsg = XtNewString((char *)_DTGETMESSAGE(2, 56,
                      "The selected Hypertext link is not supported within this application."));
                  _DtHelpErrorDialog(XtParent(qw),tmpErrorMsg);
	        }
              else
                {
                  tmpErrorMsg = XtNewString((char *)_DTGETMESSAGE(2, 57,
                      "Links to Man Pages are not supported by this application."));
                  _DtHelpErrorDialog(XtParent(qw),tmpErrorMsg);

		}
              XtFree(tmpErrorMsg);
             
           }

        _DtHelpTurnOffHourGlass(XtParent(qw));  
        break;

        default:  /* This catches bogus link types */

            /* Non valid link type so we are dropping it and are generating
             * the  proper error message.
             */
             
             /* ERROR-MESSAGE */
             XmeWarning((Widget)qw, (char*) QHDMessage2);
             break;


    }  /* End Switch Statement */


}  /* End _DtHelpQuickDialogHypertextCB */



/*****************************************************************************
 * Function:	    void ResizeQuickDialogCB()
 *
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Adjust the widget instance values for rows and columns.
 *
 ****************************************************************************/
static void ResizeQuickDialogCB (
    XtPointer clientData)
{
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) clientData ;
    


  /* Re-Set our rows and colums values */
  if ((qw->qhelp_dialog.help.pDisplayArea != NULL) &&
       qw->qhelp_dialog.display.firstTimePopupFlag == TRUE)
    _DtHelpDisplayAreaDimensionsReturn (qw->qhelp_dialog.help.pDisplayArea,
                                  &(qw->qhelp_dialog.display.textRows),
                                  &(qw->qhelp_dialog.display.textColumns));
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

  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw;
  char         *pTempAccessPath;
  char         *pTempLocationId;  

  
  /* Parse our specification into the proper fields in our instance 
   * record.  
   */

   pTempAccessPath = _DtHelpParseAccessFile(hyperData->specification);
         
   if (pTempAccessPath != NULL) 
     {
       /* Free the old one and assign the new path */
       XtFree(qw->qhelp_dialog.display.helpVolume);
       qw->qhelp_dialog.display.helpVolume = pTempAccessPath;
       
     }
 
    /* Free old copy fisrt ??? */
    pTempLocationId = _DtHelpParseIdString(hyperData->specification);
    XtFree(qw->qhelp_dialog.display.locationId);
    qw->qhelp_dialog.display.locationId = pTempLocationId;
   
    SetupDisplayType((Widget)qw, DtJUMP_UPDATE);
            
}






/*****************************************************************************
 * Function:	    void SetupDisplayType(Widget nw, int updateKind);
 *
 *
 * Parameters:      nw          Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins the type of topic the user want's to display
 *                  in the current Quick help dialog and sets it up for display.
 *
 *****************************************************************************/
static void SetupDisplayType(
     Widget nw,
     int updateKind)
{
   XtPointer          topicHandle;
   DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw ;
   int                status=VALID_STATUS;
   char               *tmpError;
   DtTopicListStruct  *tmpPtr;


   /* Here we need to store away our current scroll position as the currentlly
    * displayed item is about replaced w/a new item.
    * We already have placed this item in the jump stack, and now are just adding
    * the proper value for our scrollPosition.
    */

   if (qw->qhelp_dialog.backtr.pJumpListHead != NULL)
     {
       tmpPtr = qw->qhelp_dialog.backtr.pJumpListHead;
       tmpPtr->scrollPosition = 
                     _DtHelpGetScrollbarValue(qw->qhelp_dialog.help.pDisplayArea);
     }


   switch (qw->qhelp_dialog.display.helpType)
    {
      case DtHELP_TYPE_TOPIC:

        SetupTopic((Widget)qw, updateKind);
        break;

      case DtHELP_TYPE_STRING:
     
        /* Set the string to the current help dialog */
        status = _DtHelpFormatAsciiString(qw->qhelp_dialog.help.pDisplayArea,
					qw->qhelp_dialog.display.stringData, 
                                       &topicHandle);
        if (status >= VALID_STATUS)
          {
            _DtHelpDisplayAreaSetList (qw->qhelp_dialog.help.pDisplayArea,
                                    topicHandle, FALSE,
                                    qw->qhelp_dialog.backtr.scrollPosition);

            if (updateKind == DtJUMP_UPDATE)
              UpdateJumpList(qw->qhelp_dialog.display.stringData,
                             DtHELP_TYPE_STRING, (Widget) qw);

          }
        else
          {
            tmpError = XtNewString((char *)_DTGETMESSAGE(2, 50,
              "String data could not be formatted."));

            _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea, (Widget)qw,
              tmpError, (char*)QHDMessage3);
         
             qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
             qw->qhelp_dialog.display.stringData = XtNewString(tmpError);

            if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                              (Widget) qw);
            XtFree(tmpError);
          }

        break;

      case DtHELP_TYPE_DYNAMIC_STRING:
     
        /* Set the string to the current help dialog */
        status = _DtHelpFormatAsciiStringDynamic(
					qw->qhelp_dialog.help.pDisplayArea,
					qw->qhelp_dialog.display.stringData,
                                              &topicHandle);
        if (status >= VALID_STATUS)
          {
            _DtHelpDisplayAreaSetList (qw->qhelp_dialog.help.pDisplayArea,
                                    topicHandle, FALSE, 
                                    qw->qhelp_dialog.backtr.scrollPosition);
            if (updateKind == DtJUMP_UPDATE) 
              UpdateJumpList(qw->qhelp_dialog.display.stringData,
                             DtHELP_TYPE_DYNAMIC_STRING, (Widget) qw);
        
          }
        else
          {
            tmpError = XtNewString((char *)_DTGETMESSAGE(2, 51,
                             "Dynamic string data could not be formatted."));
            _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea, (Widget)qw,
                                   tmpError, (char*)QHDMessage4);

            qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
            qw->qhelp_dialog.display.stringData = XtNewString(tmpError);

            if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                              (Widget) qw);
             XtFree(tmpError);

 	  }

        break;


      case DtHELP_TYPE_MAN_PAGE:
         status = _DtHelpFormatManPage(qw->qhelp_dialog.help.pDisplayArea,
				qw->qhelp_dialog.display.manPage, &topicHandle);
         if (status >= VALID_STATUS)
           {
             _DtHelpDisplayAreaSetList (qw->qhelp_dialog.help.pDisplayArea,
                                    topicHandle, FALSE, 
                                    qw->qhelp_dialog.backtr.scrollPosition);  
            if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(qw->qhelp_dialog.display.manPage,
                              DtHELP_TYPE_MAN_PAGE, (Widget) qw);

           }
         else
           {
             tmpError = XtNewString((char *)_DTGETMESSAGE(2, 52,
                                    "Man Page could not be formatted. The requested Man Page is either not present, or corrupt."));

             _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea,(Widget)qw,
                tmpError, (char*)QHDMessage5);
 
             qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
             qw->qhelp_dialog.display.stringData = XtNewString(tmpError);

             if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                              (Widget) qw);
             XtFree(tmpError);

	   }
	 break;

      case DtHELP_TYPE_FILE:
      
        /* Set the string to the current help dialog */
        status = _DtHelpFormatAsciiFile(qw->qhelp_dialog.help.pDisplayArea,
					qw->qhelp_dialog.display.helpFile,
                                     &topicHandle);
        if (status >= VALID_STATUS)
           {
             _DtHelpDisplayAreaSetList (qw->qhelp_dialog.help.pDisplayArea,
                                    topicHandle, FALSE, 
                                    qw->qhelp_dialog.backtr.scrollPosition);
  
             if (updateKind == DtJUMP_UPDATE)
                UpdateJumpList(qw->qhelp_dialog.display.helpFile,
                               DtHELP_TYPE_FILE, (Widget) qw);
 
           }
         else
           {
             tmpError = XtNewString((char *)_DTGETMESSAGE(2, 53,
                "Text file data could not be formatted. The requested text file is either not present, or corrupt."));

             _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea,(Widget)qw,
                tmpError, (char*)QHDMessage6);
 
             qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
             qw->qhelp_dialog.display.stringData = XtNewString(tmpError);

             if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                              (Widget) qw);
             XtFree(tmpError);

           }

         break;


      default:  

        /* ERROR-MESSAGE */
        /* This means the user used the wrong help type */
        tmpError = XtNewString((char *)_DTGETMESSAGE(2, 80,
                           "The specified help type is invalid."));

        _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea,(Widget)qw,
           tmpError, (char*)QHDMessage7);
 
        qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
        qw->qhelp_dialog.display.stringData = XtNewString(tmpError);

        if (updateKind == DtJUMP_UPDATE)
          UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING, 
                         (Widget) qw);
        XtFree(tmpError);

        break;

    }  /* End Switch Statement */

    /* Update the print dialog */
    _DtHelpUpdatePrintDialog(&qw->qhelp_dialog.print,
                &qw->qhelp_dialog.display,&qw->qhelp_dialog.help,False);

}




/*****************************************************************************
 * Function:	    void SetupTopic(Widget nw, int updateKind)
 *
 * Parameters:      nw      Specifies the widget ID of the quick help dialog 
 *                           you to setup the topic in.
 *                  updateKind   Specifies weather to update the jump list or
 *                               not if an error message occures.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Display a new help topic in an existing quick help dialog.
 *
 *****************************************************************************/
static void SetupTopic(
    Widget nw,
    int updateKind)
{
  Boolean validTopic = FALSE;
  Boolean validPath  = FALSE;
  XtPointer	topicHandle;
  int           status=NON_VALID_STATUS;
  char          *userErrorStr=NULL;
  char          *sysErrorStr=NULL;
  char          *tmpMsg=NULL;
  char          *tmpError;
  char          *locTitle;
  char          *volumeTitle;



  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw;

  _DtHelpTurnOnHourGlass(XtParent(qw));
    
  /* Locate our HelpVolume file */
   if (qw->qhelp_dialog.display.helpVolume != NULL)
     {
       validPath = _DtHelpExpandHelpVolume((Widget)qw,
                                &qw->qhelp_dialog.display,
                                &qw->qhelp_dialog.help,
                                &qw->qhelp_dialog.print);
     }
     
  if (validPath)
    {
      validTopic = False;  /* default */
      if (qw->qhelp_dialog.display.locationId != NULL)
        {
           XmUpdateDisplay((Widget)qw);

           /* format the initial locationId info to display */
           status = _DtHelpFormatTopic (
		     qw->qhelp_dialog.help.pDisplayArea,
                     qw->qhelp_dialog.display.volumeHandle, 
                     qw->qhelp_dialog.display.locationId,
                     True,
                     &topicHandle);
            
            /* unless locationId not found, the topic is valid */
            if (status != -2) 
               validTopic = True;
            
            if (status == 0)  /* success */
            {
                _DtHelpDisplayAreaSetList (qw->qhelp_dialog.help.pDisplayArea, 
                                        topicHandle, FALSE, 
                                        qw->qhelp_dialog.backtr.scrollPosition);

                if (updateKind == DtJUMP_UPDATE)
                   UpdateJumpList(qw->qhelp_dialog.display.locationId,
                                     DtHELP_TYPE_TOPIC, (Widget) qw);
            }
        }
    }


  /* Setup and display the proper error message if we have any problems
   * with displaying the proper topic.
   */
   if (!validPath)
     {
       if (qw->qhelp_dialog.display.helpVolume == NULL)
         {

           tmpError = XtNewString((char *)_DTGETMESSAGE(2, 60,
                                  "No help volume specified."));

           _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea, (Widget)qw,
                                  tmpError, NULL);
           
           if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING,
                              (Widget) qw);

           qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
           qw->qhelp_dialog.display.stringData = XtNewString(tmpError);

           XtFree(tmpError);


	 }
       else
         {
         
            /* We cannot find our HelpVolume so display the proper error
             * message in the help dialog and continue.
             */
            tmpMsg = XtNewString((char *)_DTGETMESSAGE(2, 58,
                             "The requested online help is either not installed or not in the proper help search path.  For information on installing online help, consult the documentation for the product.\n"));
            locTitle = XtNewString((char *)_DTGETMESSAGE(2, 62,
                                  "Location ID:"));
            volumeTitle = XtNewString((char *)_DTGETMESSAGE(2, 61,
                                       "Help Volume:"));
            tmpError = XtMalloc(strlen(tmpMsg) +
                                strlen(locTitle) +
                                strlen(volumeTitle) + 
                                strlen(qw->qhelp_dialog.display.locationId) +
                                strlen(qw->qhelp_dialog.display.helpVolume) + 4);

            (void) strcpy(tmpError, tmpMsg);
            (void) strcat(tmpError, volumeTitle);
            (void) strcat(tmpError, " ");
            (void) strcat(tmpError, qw->qhelp_dialog.display.helpVolume);
            (void) strcat(tmpError,"\n");
            (void) strcat(tmpError, locTitle);
            (void) strcat(tmpError, " ");
            (void) strcat(tmpError, qw->qhelp_dialog.display.locationId);


           _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea, (Widget)qw,
                    tmpError, NULL);
 
          
           UpdateJumpList((char *)tmpError,
                               DtHELP_TYPE_DYNAMIC_STRING, (Widget) qw);
           qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
           qw->qhelp_dialog.display.stringData = XtNewString(tmpError);


           XtFree(tmpError);
           XtFree(tmpMsg);
           XtFree(locTitle);
           XtFree(volumeTitle);

         }

     }
   else if  (!validTopic)
     {
       if (qw->qhelp_dialog.display.locationId == NULL)
         {
            tmpMsg =_DTGETMESSAGE(2, 59,"No location ID specified.");
            userErrorStr = XtNewString(tmpMsg);
	 }
       else
        {
           tmpMsg = _DTGETMESSAGE(2, 55,
                                 "Nonexistent location ID:");
           userErrorStr = XtMalloc(strlen(tmpMsg) + 
                                   strlen(qw->qhelp_dialog.display.locationId)+ 2);
           (void) strcpy(userErrorStr, tmpMsg);
           (void) strcat(userErrorStr, " ");
           (void) strcat(userErrorStr, qw->qhelp_dialog.display.locationId);
           sysErrorStr = (char*)QHDMessage9;
	}
        _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea, (Widget)qw,
                    userErrorStr, sysErrorStr);

        qw->qhelp_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;
        qw->qhelp_dialog.display.stringData = XtNewString(userErrorStr);


        if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList((char *)userErrorStr,
                             DtHELP_TYPE_DYNAMIC_STRING, (Widget) qw);

     
        XtFree(userErrorStr);

     }
   else if (status <= NON_VALID_STATUS) 
     {
       tmpError = XtNewString((char *)_DTGETMESSAGE(2, 54,
                    "Help topic could not be formatted."));

       _DtHelpDisplayFormatError(qw->qhelp_dialog.help.pDisplayArea,(Widget)qw,
                    tmpError, (char*)QHDMessage10);
      
       if (updateKind == DtJUMP_UPDATE)
               UpdateJumpList(tmpError, DtHELP_TYPE_DYNAMIC_STRING,
                              (Widget) qw);

       XtFree(tmpError);

     }   

    _DtHelpTurnOffHourGlass(XtParent(qw));          

}





/*****************************************************************************
 * Function:	    void FreeQuickHelpInfo();  
 *
 *
 * Parameters:      nw  Specifies the current help dialog widget.
 *                  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    This function will re-initializes a Help Dialog Widget to 
 *                  known good starting values or clean up in prepretion for 
 *                  an impending destroy.
 *
 *****************************************************************************/
static void FreeQuickHelpInfo(
    Widget nw,
    int cleanUpKind)
{
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw;

  /* Free our jump list display stuff */
  _DtHelpTopicListFree(qw->qhelp_dialog.backtr.pJumpListHead);
  qw->qhelp_dialog.backtr.pJumpListHead     = NULL;
  qw->qhelp_dialog.backtr.pJumpListTale     = NULL;
  qw->qhelp_dialog.backtr.totalJumpNodes    = 0;

  /* Close our current help volume */
  if (qw->qhelp_dialog.display.volumeHandle != NULL)
    {
      _DtHelpCloseVolume(qw->qhelp_dialog.display.volumeHandle);
      qw->qhelp_dialog.display.volumeHandle = NULL;
    }


  if (cleanUpKind == DtCLEAN_FOR_DESTROY)
    {

      /* Free any remaining char * values we malloc'ed in our help dialog */
      XtFree(qw->qhelp_dialog.help.currentHelpFile);  
      XtFree(qw->qhelp_dialog.display.locationId);
      XtFree(qw->qhelp_dialog.display.helpVolume); 
      XtFree(qw->qhelp_dialog.display.manPage);
      XtFree(qw->qhelp_dialog.display.stringData);
      XtFree(qw->qhelp_dialog.display.helpFile);
      if (qw->qhelp_dialog.display.topicTitleLbl != NULL)
          XmStringFree(qw->qhelp_dialog.display.topicTitleLbl);
     
      /* close print dialog, free all memory */
      _DtHelpFreePrintStuff(&qw->qhelp_dialog.print,DtCLEAN_FOR_DESTROY);

      /* Free all the info we saved for our help callbacks */
      _DtHelpListFree(&qw->qhelp_dialog.help.pHelpListHead);

    }
  else
    {
      /* Set our display area to a null starting vlaues */
      _DtHelpDisplayAreaClean(qw->qhelp_dialog.help.pDisplayArea);

      /* Set the back button to false */
      XtSetSensitive(qw->qhelp_dialog.qhelp.backButton, FALSE);

      /* close print dialog, free unused memory */
      _DtHelpFreePrintStuff(&qw->qhelp_dialog.print,DtCLEAN_FOR_REUSE);

    }


}



/*****************************************************************************
 * Function:	    void UpdateJumpList(char *topicInfo,
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
static void UpdateJumpList(
    char *topicInfo,
    int topicType,
    Widget nw)
{
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) nw ;
  
  /* Add the new topic to the top of the jump list */
  /* We add a -1 for the scrollbar position value, and will replace it with the
   * actual value just prior to changing the window to the new topic.
   */
  _DtHelpTopicListAddToHead(topicInfo, NULL, topicType, 999,
                        qw->qhelp_dialog.display.helpVolume,
                        &qw->qhelp_dialog.backtr.pJumpListHead,
                        &qw->qhelp_dialog.backtr.pJumpListTale,
                        &qw->qhelp_dialog.backtr.totalJumpNodes,
                         -1);

  if (qw->qhelp_dialog.backtr.totalJumpNodes <= 1) 
    XtSetSensitive(qw->qhelp_dialog.qhelp.backButton, FALSE);
  else
    XtSetSensitive(qw->qhelp_dialog.qhelp.backButton, TRUE);
 
}



/*****************************************************************************
 * Function:	   static void ProcessBackCB(
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
static void  ProcessBackCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData )
{


 
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget)  clientData;
  DtTopicListStruct *pTemp= NULL;

  _DtHelpTurnOnHourGlass(XtParent(qw));


  /* Pop the top element off our jump list and display the new top element */
  _DtHelpTopicListDeleteHead(&qw->qhelp_dialog.backtr.pJumpListHead,
                          &qw->qhelp_dialog.backtr.pJumpListTale,
                          &qw->qhelp_dialog.backtr.totalJumpNodes);


  /* Assign pTemp to the current head pointer for or jump list */
  pTemp = qw->qhelp_dialog.backtr.pJumpListHead;

  if (qw->qhelp_dialog.backtr.totalJumpNodes <= 1)
    {
      XtSetSensitive(qw->qhelp_dialog.qhelp.backButton, FALSE);
 
      /* Also make sure the default button is set properly */
      XmProcessTraversal(qw->qhelp_dialog.qhelp.closeButton, XmTRAVERSE_HOME);
    }
  else
    XtSetSensitive(qw->qhelp_dialog.qhelp.backButton, TRUE);


  /* Assign the jump values to or instance structure variables */
  XtFree(qw->qhelp_dialog.display.helpVolume);
  qw->qhelp_dialog.display.helpVolume  = XtNewString(pTemp->helpVolume);
  qw->qhelp_dialog.display.helpType = pTemp->topicType;

  /* Set our help volume flag so we open the proper volume */
  /*  qw->help_dialog.ghelp.volumeFlag         = FALSE; */
 

  /* Assign our scrollbar value to our instance structure  so we jump to the
   * proper location 
   */
 
  qw->qhelp_dialog.backtr.scrollPosition = pTemp->scrollPosition;


  switch (pTemp->topicType)
    {
      case DtHELP_TYPE_TOPIC:

        /* Update our help dialog with top jump element */
        XtFree(qw->qhelp_dialog.display.locationId);
        qw->qhelp_dialog.display.locationId = XtNewString(pTemp->locationId);
        
        SetupDisplayType((Widget)qw, DtNO_JUMP_UPDATE);
      
	break;

      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
        XtFree(qw->qhelp_dialog.display.stringData);
        qw->qhelp_dialog.display.stringData = XtNewString(pTemp->locationId);
        
        SetupDisplayType((Widget)qw, DtNO_JUMP_UPDATE);
     
        break;

      case DtHELP_TYPE_MAN_PAGE:
        XtFree(qw->qhelp_dialog.display.manPage);
        qw->qhelp_dialog.display.manPage = XtNewString(pTemp->locationId);
        
        SetupDisplayType((Widget)qw, DtNO_JUMP_UPDATE);
     
	break;

      case DtHELP_TYPE_FILE:
        XtFree(qw->qhelp_dialog.display.helpFile);
        qw->qhelp_dialog.display.helpFile = XtNewString(pTemp->locationId);
        
        SetupDisplayType((Widget)qw, DtNO_JUMP_UPDATE);
     
        break;


      default:  

        /* ERROR-MESSAGE */
        /* This means the user used the worng help type */
        XmeWarning((Widget)qw, (char*) QHDMessage7);

        break;

    }  /* End Switch Statement */


    /* reset our scrollPosition back to its default here so in any other case,
     * we jump to the top of the topic 
     */
    qw->qhelp_dialog.backtr.scrollPosition = -1;


   _DtHelpTurnOffHourGlass(XtParent(qw)); 
    
}



/****************************************************************************
 * Function:         CatchClose( Widget w);
 *                          
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Catches the window manager close requests and assigns our
 *                  CloseQuickCB to handel them.
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
                          CloseQuickCB, (XtPointer)widget);
  XtSetValues(XtParent(widget), args, 1);
  
} 




/************************************************************************
 * Function: CloseQuickCB()
 *
 *	Close the Help Dialog Window
 *
 ************************************************************************/
static void CloseQuickCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
  DtHelpDialogCallbackStruct callDataInfo;
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) clientData;
  XmPushButtonCallbackStruct *callbackStruct = 
                               (XmPushButtonCallbackStruct*) callData; 

    /*
     * Check to see if a selection is in process.
     * If so, cancel the selection and don't close the dialog
     */
    if (_DtHelpCancelSelection(qw->qhelp_dialog.help.pDisplayArea) == True)
	return;

  /* No selection in progress. Close the dialog.
   *
   * ??? By definition, when a user closes a Help Dialog if it is 
   * used again by the application the state will be new. So we should 
   * flush out any info currently in the help, history, and search dialogs.
   *
   * We are just cleaning the canvas and the destroy routine for the
   * Display Area *has not been called*. Therefore canvas is still valid.
   */
 
  FreeQuickHelpInfo((Widget)qw, DtCLEAN_FOR_REUSE);
 
  /* Look to see if they registered their own close callback */
  if (qw->qhelp_dialog.qhelp.closeCallback != NULL)
    {
       callDataInfo.reason = DtCR_HELP_CLOSE;
       callDataInfo.event = callbackStruct->event;
       callDataInfo.locationId = NULL;
       callDataInfo.helpVolume = NULL;
       callDataInfo.specification = NULL;
       callDataInfo.hyperType = 0;
          
       /* All we do is envoke the applications close callback */
       XtCallCallbackList((Widget)qw,qw->qhelp_dialog.qhelp.closeCallback,
                          &callDataInfo);
    }
  else
    XtUnmanageChild((Widget)qw);
}



#if 0  /* no longer needed, now that the Help button is hardwired
          to the _DtHelpCB function. */

/************************************************************************
 * Function: HelpButtonCB()
 *
 *	Arrange to call the help callback on the widget 
 *
 ************************************************************************/
static void HelpButtonCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) clientData;
  XmAnyCallbackStruct	*callback = (XmAnyCallbackStruct *) callData;



  /* Invoke the help system. */
  _XmManagerHelp((Widget)qw, callback->event, NULL, NULL);


}
#endif




/************************************************************************
 * Function: PrintQuickHelpCB()
 *
 *	Print the current help topic
 *
 ************************************************************************/
static void PrintQuickHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
  DtHelpQuickDialogWidget qw = (DtHelpQuickDialogWidget) clientData;

  XmUpdateDisplay((Widget) qw);
  _DtHelpTurnOnHourGlass(XtParent(qw));

  /* display the print dialog */
  _DtHelpDisplayPrintDialog((Widget) qw,&qw->qhelp_dialog.print,
		&qw->qhelp_dialog.display,&qw->qhelp_dialog.help);

  _DtHelpTurnOffHourGlass(XtParent(qw));
}


