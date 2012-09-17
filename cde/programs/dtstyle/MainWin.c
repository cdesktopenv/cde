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
/*  $XConsortium: MainWin.c /main/8 1996/03/25 00:52:54 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        MainWin.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the main Dtstyle dialog
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>
#include <signal.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/CascadeB.h>
#include <Xm/ColorObjP.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>

/*
#include <Dt/Box.h>
*/
#include <Dt/Control.h>
#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/IconFile.h>

#include <Dt/HourGlass.h>
#include <Dt/UserMsg.h>
    
#include "Main.h"
#include "Help.h"
#include "SaveRestore.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "MainWin.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static void _DtMapCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void build_mainWindow( Widget shell) ;
static void activateCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ProcessComponentList(
                        Widget parent,
                        Widget mainRC) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/*  Internal variables                   */
/*+++++++++++++++++++++++++++++++++++++++*/

static int         colorUse = XmCO_BLACK_WHITE;   
static saveRestore save = {FALSE, 0, };


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* init_mainWindow                                          */
/*                                                          */
/* build if needed, otherwise just manage the mainWindow.   */
/* Assumes shell has been created                           */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void 
init_mainWindow( Widget shell )
{
    if (style.mainWindow == NULL) {
        build_mainWindow(shell);
	if (save.restoreFlag)
	    XtAddCallback (shell, XtNpopupCallback, _DtMapCB, NULL);
        XtManageChild(style.mainWindow);
        XtPopup(shell, XtGrabNone);
    }
    else   /* fix Shouldn't ever happen */
        XtManageChild(style.mainWindow);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* _DtMapCB                                                      */
/*        first time only, set correct sizing in case font    */
/*        size has changed (if so, saved size is wrong)       */
/*            remove callback after first time                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void 
_DtMapCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    XSizeHints hints;

    if (save.restoreFlag)
    {
	hints.min_width  = XtWidth(w);
	hints.min_height = XtHeight(w);
	hints.max_width  = XtWidth(w);
	hints.max_height = XtHeight(w);
	hints.flags      = PMinSize | PMaxSize;
	XSetWMNormalHints (style.display, XtWindow(w), &hints);
    }

    XtRemoveCallback(style.shell, XtNpopupCallback, _DtMapCB, NULL);
}

/*+++++++++++++++++++++++++++++++++++++++++++++*/
/* build_mainWindow                            */
/*                                             */
/* build the main window, but don't manage it. */
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static void 
build_mainWindow(
        Widget shell )
{
  int        count, n, n2;
  Arg        args[MAX_ARGS];
  Widget     WidgList[10];
  Widget     frame;
  Widget     mainMenu;
  Widget     mainRC;
  Widget     filePulldown;
  Widget     helpPulldown;
  Widget     exitBtn;
  char       *mnemonic;
  char       *tmpStr;
  Pixmap     pixmap;
  XmString   labelString;
  Pixmap     ditherPix;
  XmPixelSet pixelSet[XmCO_NUM_COLORS];
  Pixel      bg;
  short      a,i,p,s;


  /* saveRestore
   * Note that save.poscnt has been initialized elsewhere.  
   * save.posArgs may contain information from restoreAudio().*/

    XtSetArg(save.posArgs[save.poscnt], XmNnoResize, True); save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNresizePolicy, XmRESIZE_NONE); save.poscnt++;
    style.mainWindow= XmCreateMainWindow(shell,"mainWindow",  save.posArgs, save.poscnt);
    XtAddCallback(style.mainWindow, XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_MAIN_WINDOW);

    n = 0;
    tmpStr = (char *)XtMalloc(strlen(GETMESSAGE(4, 1, "Style Manager"))+1);
    sprintf(tmpStr, "%s", GETMESSAGE(4, 1, "Style Manager"));
    XtSetArg(args[n], XmNtitle, tmpStr); n++;
    XtSetArg(args[n], XmNiconName, 
        (char *)GETMESSAGE(4, 37, "Style")); n++;
    XtSetValues(XtParent(style.mainWindow), args, n);
    XtFree(tmpStr);

    n = 0;
    mainMenu= XmCreateMenuBar(style.mainWindow,"mainMenu", args, n);
    XtAddCallback(mainMenu, XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_MENUBAR);
    XtManageChild(mainMenu);

    /* create the pulldown widgets */
    n = 0;
    filePulldown= XmCreatePulldownMenu(mainMenu,"fileMenu", args, n );
    XtAddCallback(filePulldown, XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_FILE_MENU);

    n = 0;
    helpPulldown= XmCreatePulldownMenu(mainMenu, "helpMenu" , args, n );
    XtAddCallback(helpPulldown, XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_HELP_MENU);

    /* create the cascade butons */
    count = 0;

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 2, "F"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString =  CMPSTR((char *)GETMESSAGE(4, 3, "File"));
    XtSetArg(args[n], XmNlabelString, labelString);  n++;
    XtSetArg(args[n], XmNsubMenuId, filePulldown); n++;
    XtSetArg (args[n], XmNmarginWidth, LB_MARGIN_WIDTH); n++;
    WidgList[count++]= 
        XmCreateCascadeButton(mainMenu, "fileCascadeButton", args, n );
    XtAddCallback(WidgList[count-1], XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_FILE_MENU);
    XmStringFree(labelString);

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 4, "H"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString =  CMPSTR((char *)GETMESSAGE(4, 5, "Help"));
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNsubMenuId, helpPulldown); n++;
    XtSetArg (args[n], XmNmarginWidth, 6); n++;
    WidgList[count++]= 
        XmCreateCascadeButton(mainMenu, "helpCascadeButton", args, n );
    XtAddCallback(WidgList[count-1], XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_HELP_MENU);
    XmStringFree(labelString);

    n = 0;
    XtSetArg(args[n], XmNmenuHelpWidget, WidgList[count-1]);    n++;
    XtSetValues (mainMenu, args, n);

    XtManageChildren (WidgList, count);

    /* create the File menu pane's buttons */
    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 53, "x"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString =  CMPSTR((char *)GETMESSAGE(4, 7, "Exit"));
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    exitBtn= XmCreatePushButton(filePulldown, "exit", args, n );
    XtAddCallback(exitBtn, XmNactivateCallback, activateCB_exitBtn, NULL);
    XtAddCallback(exitBtn, XmNhelpCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_EXIT);
    XtManageChild(exitBtn);
    XmStringFree(labelString);

    /* create the Help menu pane's buttons */
    count =0;

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 54, "v"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString = CMPSTR((char *)GETMESSAGE(4, 55, "Overview"));
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    WidgList[count++]= 
        XmCreatePushButton(helpPulldown, "overview", args, n );
    XtAddCallback(WidgList[count-1], XmNactivateCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_INTRODUCTION);
    XmStringFree(labelString);

    n = 0;
    WidgList[count++]= 
        XmCreateSeparatorGadget(helpPulldown,"separator",args,n);
            
    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 43, "T"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString = CMPSTR((char *)GETMESSAGE(4, 56, "Tasks"));
    XtSetArg(args[n], XmNlabelString, labelString);  n++;
    WidgList[count++]= XmCreatePushButton(helpPulldown, "tasks", args, n );
    XtAddCallback(WidgList[count-1], XmNactivateCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_TASKS);
    XmStringFree(labelString);

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 45, "R"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString = CMPSTR((char *)GETMESSAGE(4, 57, "Reference"));
    XtSetArg(args[n], XmNlabelString, labelString);  n++;
    WidgList[count++]= XmCreatePushButton(helpPulldown, "reference", args, n );
    XtAddCallback(WidgList[count-1], XmNactivateCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_REFERENCE);
    XmStringFree(labelString);

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 47, "O"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString = CMPSTR((char *)GETMESSAGE(4, 48, "On Item"));
    XtSetArg(args[n], XmNlabelString, labelString);  n++;
    WidgList[count++]= XmCreatePushButton(helpPulldown, "onItem", args, n );
    XtAddCallback(WidgList[count-1], XmNactivateCallback,
        (XtCallbackProc)HelpModeCB, (XtPointer)NULL);
    XmStringFree(labelString);

    n = 0;
    WidgList[count++] = 
        XmCreateSeparatorGadget(helpPulldown,"separator",args,n);

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 49, "U"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString = CMPSTR((char *)GETMESSAGE(4, 58, "Using Help"));
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    WidgList[count++]= 
        XmCreatePushButton(helpPulldown, "usingHelp", args, n );
    XtAddCallback(WidgList[count-1], XmNactivateCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_USING);
    XmStringFree(labelString);

    n = 0;
    WidgList[count++] = 
        XmCreateSeparatorGadget(helpPulldown,"separator",args,n);

    n = 0;
    mnemonic = ((char *)GETMESSAGE(4, 59, "A"));
    XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
    labelString = CMPSTR((char *)GETMESSAGE(4, 60, "About Style Manager"));
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    WidgList[count++]= XmCreatePushButton(helpPulldown, "stylemanager", args, n );
    XtAddCallback(WidgList[count-1], XmNactivateCallback,
        (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_VERSION);
    XmStringFree(labelString);

    XtManageChildren(WidgList, count);

    /*  create frame and row column for icon gadgets */
    n = 0;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT); n++;
    XtSetArg(args[n], XmNshadowThickness, 1); n++;
    frame = XmCreateFrame(style.mainWindow, "frame", args, n);
    XtManageChild(frame);
                           
    /* Save pixels for later use                                           */
    /* primary top/bottom shadow -> fg/bg for pixmaps in primary areas     */
    /* secondary top/bottom shadow -> fg/bg for pixmaps in secondary areas */
    /* secondary select color -> backgrounds of editable text areas        */
    /* secondary foreground ->  fill color on toggle indicators            */

    /*  Get the primary colors to be used later */
    n = 0;
    XtSetArg(args[n], XmNbackground, &bg); n++;
    XtGetValues(style.mainWindow, args, n);

    XmGetColors (style.screen, style.colormap, bg,
                 NULL,
                 &style.primTSCol,
                 &style.primBSCol,
                 NULL);

    /*  Get the secondary colors to be used later */
    n = 0;
    XtSetArg(args[n], XmNbackground, &style.secBgCol); n++;
    XtGetValues(exitBtn, args, n);

    XmGetColors (style.screen, style.colormap, style.secBgCol,
                 &style.tgSelectColor,
                 &style.secTSCol,
                 &style.secBSCol,
                 &style.secSelectColor);

    if ((style.secTSCol == style.secBSCol) || /* B_W or shadowPixmaps==True */
        (!style.useMultiColorIcons))          /* multiColorIcons turned off */
    {
        /* force TS = White, BS = Black */
        style.secTSCol = style.primTSCol = WhitePixelOfScreen(style.screen);
        style.secBSCol = style.primBSCol = BlackPixelOfScreen(style.screen);
    }
    
    XmeGetPixelData(style.screenNum, &colorUse, pixelSet, &a, &i, &p, &s);

    n = 0;
    if (colorUse == XmCO_BLACK_WHITE)
    {
    ditherPix = XmGetPixmap (style.screen, "50_foreground",
                             style.primBSCol,style.primTSCol);
    XtSetArg(args[n], XmNbackgroundPixmap, ditherPix); n++;
    }
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNadjustLast, False); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
    XtSetArg(args[n], XmNspacing, style.horizontalSpacing); n++;
    mainRC = XmCreateRowColumn(frame, "mainRC", args, n);
    XtManageChild(mainRC);

    n = 0;
    XtSetArg(args[n], XmNmenuBar, mainMenu);    n++;
    XtSetArg(args[n], XmNcommandWindow, NULL);    n++;
    XtSetArg(args[n], XmNworkWindow, frame);    n++;
    XtSetValues (style.mainWindow, args, n);

    ProcessComponentList (shell, mainRC);

}

/*+++++++++++++++++++++++++++++++++++++++
 * ProcessComponentList();
 * 
 *+++++++++++++++++++++++++++++++++++++++*/

static void 
ProcessComponentList(
        Widget parent,
        Widget mainRC )
{
   int       count=0;
   int       j=0;
   int       n, n2;
   Arg       args[MAX_ARGS];
   Widget    componentWidget;
   int       s;
   char      *string;
   char      *resString, *resString2;
   XmString  cmp_string;
   int       helpTag;
   char      *dtIcon;
   int       len;

   style.workProcs = False;
   if (style.xrdb.componentList == NULL) return;

   n = 0;
   XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
   XtSetArg(args[n], XmNpixmapPosition, XmPIXMAP_TOP); n++;
   XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT); n++;
   n2 = 3;    /* number of preset args for creating Icons */

   resString = strdup (style.xrdb.componentList);
   resString2 = resString;
   len = strlen(resString);
   string = strtok(resString, " ");
   s = 0;
   while (string != NULL)
   {
      /* check for valid name, duplicate name *** */

      /* create Icon in Dtstyle main window */

      n = n2;
      XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
      if (colorUse == XmCO_BLACK_WHITE)
      {
          XtSetArg(args[n], XmNuseEmbossedText, False); n++;
      }
      XtSetArg(args[n], XmNpixmapForeground, style.primBSCol); n++;
      XtSetArg(args[n], XmNpixmapBackground, style.primTSCol); n++;
      if (strcmp(string, COLOR) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 27, "Color"));
          helpTag = HELP_COLOR_BUTTON;
	  dtIcon = _DtGetIconFileName(style.screen, COLOR_ICON, NULL, NULL, 0);
          style.workProcs = True;          
      }
      else if (strcmp(string, FONT) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 28, "Font"));
	  dtIcon = _DtGetIconFileName(style.screen, FONT_ICON, NULL, NULL, 0);
          helpTag = HELP_FONT_BUTTON;
      }

      else if (strcmp(string, BACKDROP) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 29, "Backdrop"));
	  dtIcon = _DtGetIconFileName(style.screen, BACKDROP_ICON, NULL, NULL, 0);
          helpTag = HELP_BACKDROP_BUTTON;
      }

      else if (strcmp(string, KEYBOARD) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 30, "Keyboard"));
	  dtIcon = _DtGetIconFileName(style.screen, KEYBOARD_ICON, NULL, NULL, 0);	  
          helpTag = HELP_KEYBOARD_BUTTON;        
      }

      else if (strcmp(string, MOUSE) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 31, "Mouse"));
	  dtIcon = _DtGetIconFileName(style.screen, MOUSE_ICON, NULL, NULL, 0);	  
          helpTag = HELP_MOUSE_BUTTON;
      }

      else if (strcmp(string, BEEP) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 32, "Beep"));
	  dtIcon = _DtGetIconFileName(style.screen, BEEP_ICON, NULL, NULL, 0);	  
          helpTag = HELP_AUDIO_BUTTON;
      }

      else if (strcmp(string, SCREEN) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 33, "Screen"));
	  dtIcon = _DtGetIconFileName(style.screen, SCREEN_ICON, NULL, NULL, 0);	  
          helpTag = HELP_SCREEN_BUTTON;
      }

      else if (strcmp(string, DTWM) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 38, "Window"));
	  dtIcon = _DtGetIconFileName(style.screen, DTWM_ICON, NULL, NULL, 0);	  
          helpTag = HELP_DTWM_BUTTON;
      }

      else if (strcmp(string, STARTUP) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 35, "Startup"));
	  dtIcon = _DtGetIconFileName(style.screen, STARTUP_ICON, NULL, NULL, 0);	  
          helpTag = HELP_STARTUP_BUTTON;
      }
      else if (strcmp(string, I18N) == 0)
      {
          cmp_string = CMPSTR(GETMESSAGE(4, 34, "Intl'"));
	  dtIcon = _DtGetIconFileName(style.screen, I18N_ICON, NULL, NULL, 0);	  
          helpTag = HELP_I18N_BUTTON;        
      }
      else
      {
          s += strlen(string) +1;
	  if (resString+s >= resString2 + len)
	      string = NULL;
	  else
	      string = strtok(resString+s, " ");
          continue;
      }
      XtSetArg(args[n], XmNimageName, dtIcon); n++;  
      XtSetArg(args[n], XmNstring, cmp_string); n++;
      componentWidget = _DtCreateControl(mainRC, string, args, n); 
      XtManageChild (componentWidget);
      XtAddCallback(componentWidget, XmNcallback, activateCB, string);
      XtAddCallback(componentWidget, XmNhelpCallback,
             (XtCallbackProc)HelpRequestCB, (XtPointer)helpTag);
      XmStringFree(cmp_string);
      XtFree(dtIcon);
      
      s += strlen(string) +1;
      if (resString+s >= resString2 + len)
	  string = NULL;
      else
	  string = strtok(resString+s, " ");

   } /* while */

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* cancel all the dialogs (for exit)     */
/*+++++++++++++++++++++++++++++++++++++++*/
void 
callCancels( void )
{
 DtDialogBoxCallbackStruct CancelBut;
 char dialogStates[60];
 static Boolean calledAlready = False;

  if (!calledAlready) {
     dialogStates[0] = '\0';

   if(style.colorDialog != NULL && XtIsManaged(style.colorDialog)) {
       CancelBut.button_position = CANCEL_BUTTON;
       XtCallCallbacks(style.colorDialog, XmNcallback, &CancelBut);
   }

     if(style.fontDialog != NULL && XtIsManaged(style.fontDialog)) {
	 CancelBut.button_position = CANCEL_BUTTON;
	 XtCallCallbacks(style.fontDialog, XmNcallback, &CancelBut);
     }

     if(style.audioDialog != NULL && XtIsManaged(style.audioDialog)) {
	 CancelBut.button_position = CANCEL_BUTTON;
	 XtCallCallbacks(style.audioDialog, XmNcallback, &CancelBut);
     }

     if(style.kbdDialog != NULL && XtIsManaged(style.kbdDialog)) {
	 CancelBut.button_position = CANCEL_BUTTON;
	 XtCallCallbacks(style.kbdDialog, XmNcallback, &CancelBut);
     }

     if(style.mouseDialog != NULL && XtIsManaged(style.mouseDialog)) {
	 CancelBut.button_position = CANCEL_BUTTON;
	 XtCallCallbacks(style.mouseDialog, XmNcallback, &CancelBut);
     }

     if(style.screenDialog != NULL &&  XtIsManaged(style.screenDialog)) {
	 CancelBut.button_position = CANCEL_BUTTON;
	 XtCallCallbacks(style.screenDialog, XmNcallback, &CancelBut);
     }

     if(style.startupDialog != NULL && XtIsManaged(style.startupDialog)) {
	 CancelBut.button_position = CANCEL_BUTTON;
	 XtCallCallbacks(style.startupDialog, XmNcallback, &CancelBut);
     }

     if (dialogStates) 
       _DtAddToResource(style.display, dialogStates);

     calledAlready = True;
  }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* Menu CallBacks                        */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
activateCB_exitBtn(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
/*callCancels() remembers whether its been called so it won't get called*/
/*twice on logout.*/
   callCancels();

   XSync(style.display, 0);
   exit(0);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* PushButton CallBacks                  */
/*+++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++*/
/* activateCB                            */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
activateCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  
  DtControlCallbackStruct *cbs = 
        (DtControlCallbackStruct *) call_data;
  char *cd;
  cd = (char *) client_data;
   
  /* if any other event besides an activate
     event invokes this callback, don't invoke 
     any dialogs.  Just ignore the event.
  */
  if (cbs->reason != XmCR_ACTIVATE) return;

  if (strcmp(cd, COLOR) == 0)
    Customize(style.shell);
  
  else if (strcmp(cd, FONT) == 0)
    popup_fontBB(style.shell);
  
  else if (strcmp(cd, BACKDROP) == 0)
    BackdropDialog (style.shell);
  
  else if (strcmp(cd, KEYBOARD) == 0)
    popup_keyboardBB(style.shell);
  
  else if (strcmp(cd, MOUSE) == 0)
    popup_mouseBB(style.shell);
  
  else if (strcmp(cd, BEEP) == 0)
    popup_audioBB(style.shell);
            
  else if (strcmp(cd, SCREEN) == 0)
    popup_screenBB(style.shell);
  
  else if (strcmp(cd, DTWM) == 0)
    popup_dtwmBB(style.shell);
  
  else if (strcmp(cd, STARTUP) == 0)
    popup_startupBB(style.shell);

  else if (strcmp(cd, I18N) == 0)
    popup_i18nBB(style.shell);

}


/************************************************************************
 * restoreMain()
 *
 * restore any state information saved with saveMain.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreMain(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("mainWindow");
    xrm_name [1] = XrmStringToQuark ("ismapped");
    xrm_name [2] = 0;

    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
      /* Are we supposed to be mapped? */
      if (strcmp(value.addr, "True") == 0) {
        save.poscnt = 0;

        /* get x position */
        xrm_name [1] = XrmStringToQuark ("x");
        if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
          XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr));
	  save.poscnt++;
        }

        /* get y position */
        xrm_name [1] = XrmStringToQuark ("y");
        if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) {
          XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr));
	  save.poscnt++;
        }

	save.restoreFlag = True;
        init_mainWindow(shell);
      }
    }
    else
      _DtSimpleError (progName, DtWarning, NULL, ((char *)GETMESSAGE(4,36,
	"No Dtstyle restore information available for current session")), NULL);
}

/************************************************************************
 * saveMain()
 *
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with the
 * file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreMain.  The suggested minimum is whether you are mapped, and your
 * location.
 ************************************************************************/
void 
saveMain(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */

    if (style.mainWindow != NULL) {
        if (XtIsRealized(style.mainWindow))
            sprintf(bufr, "*mainWindow.ismapped: True\n");
  
        /* Get and write out the geometry info for our Window */
        x = XtX(XtParent(style.mainWindow));
        y = XtY(XtParent(style.mainWindow));
        sprintf(bufr, "%s*mainWindow.x: %d\n", bufr, x);
        sprintf(bufr, "%s*mainWindow.y: %d\n", bufr, y);

        write (fd, bufr, strlen(bufr));
    }
}

