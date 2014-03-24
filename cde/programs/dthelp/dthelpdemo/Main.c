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
/* $TOG: Main.c /main/4 1998/08/05 11:46:03 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:         Main.c
 **
 **  Project:	   CDE dthelpdemo sample program.
 **
 **  Description:  This file contains the demo source for the helpdemo 
 **                application.
 **          
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994
 **      Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdlib.h>
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>

/* DtHelp include header file section */
#include <Dt/HelpQuickD.h>

/* Local header file include section */
#include "Main.h"
#include "HelpEntry.h"
#include "HelpCacheI.h"
#include "HourGlassI.h"

/*  Internal Static Functions  */
static Widget BuildMenus(Widget parent);
static void CreateMainWindow(Widget parent);
static void HelpMenuCB(
    Widget  widget,
    XtPointer clientData,
    XtPointer callData);
static void CloseCB (
    Widget	w,
    XtPointer	clientData,
    XtPointer  	callData);
static void ManageButtonsCB(
    Widget  parent,
    XtPointer clientData,
    XtPointer callData);
static void Control1ButtonsCB(
    Widget  parent,
    XtPointer clientData,
    XtPointer callData);
static void HelpCB(
    Widget  widget,
    XtPointer clientData,
    XtPointer callData);
static void DisplayMoreCB (
    Widget	w,
    XtPointer	clientData,
    XtPointer   callData);
static void CloseAndDestroyCB (
    Widget	w,
    XtPointer	clientData,
    XtPointer   callData);




/* Global variables for the Main module */
char   *appName;
Widget topLevel;
Widget mainShell;
Widget manWidget = NULL;
Widget quickWidget = NULL;

Widget createSampleWidgetBtn, destroySampleWidgetBtn;
Widget controlBtn1, controlBtn2, controlBtn3, controlBtn4;
Widget moreButton=NULL;


/* toggle button status flags */
Boolean    addMoreButton=TRUE;
Boolean    goBigger=TRUE;
Boolean    changeTopic=TRUE;


char HelpdemoHelpVol[] = "helpdemo";

/************************************************************************
 *
 *  helpdemo
 *	The main program for the helpdemo.
 *
 ************************************************************************/
void main (
    int argc,
    char *argv[])
{
 
   Display  *appDisplay;
   
   appName = strrchr(argv[0], '/');
   if (appName != NULL)
     appName++;
   else
     appName = argv[0];
   
   /* Initialize toolkit and open the display */
   topLevel = XtInitialize(appName, "Dthelpdemo", NULL, 0, &argc, argv);
   appDisplay = XtDisplay(topLevel);

   if (!appDisplay) 
     {
       XtWarning ("Dialogs: Can't open display, exiting...");
       exit (0);
     }
  

   
   /* Build up the Widget UI for the demo app */
   CreateMainWindow(topLevel);

   XtMainLoop(); 
}



/************************************************************************
 * Function:  CreateMainWindow
 *
 *   This function creates a main window, calls the routine to build our
 * menubar.
 *
 *************************************************************************/
static void CreateMainWindow(
    Widget parent)
{

  Widget	main_window;
  Widget	menuBar;
  Widget	frame;
  Widget        form;
  Widget        manageFrame, manageForm, manageTitle, radioBox;
  Widget        control1Frame, control1Form, control1Title, controlBox1;
  Widget        control2Frame, control2Form, control2Title, controlBox2;
  Arg		args[10];
  int      	n;
  char		name[15];
  int		len;

       
  XmString labelString;
        
  /* Create a Main Window Widget */
	
  n = 0;
  main_window = XmCreateMainWindow (parent, "mainwindow1", args, n);
  XtManageChild (main_window);

  /* Build the Menus via our BuildMenus function */
  menuBar = BuildMenus(main_window);


  /*	Create Frame in the MainWindow 	*/
  n = 0;
  XtSetArg (args[n], XmNmarginWidth, 2);  n++;
  XtSetArg (args[n], XmNmarginHeight, 2);  n++;
  XtSetArg (args[n], XmNshadowThickness, 1);  n++;
  XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);  n++;
  frame = XmCreateFrame (main_window, "frame", args, n);
  XtManageChild (frame);

        
  /*    Create the form */
  n = 0;
  form = XmCreateForm(frame, "form", args, n);
  XtManageChild(form);


   /****** Create the Management section Frame ******/
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 5);				n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginWidth, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 2);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   manageFrame = XmCreateFrame(form, "manageFrame", args, n);
   XtManageChild (manageFrame);

   manageForm = XmCreateForm (manageFrame,"manageForm", NULL, 0);
   XtManageChild (manageForm);

   XtAddCallback(manageForm, XmNhelpCallback, 
                 HelpCB, CREATE_FRAME);

   /* create the frame title */
   labelString = XmStringCreateLocalized("Help Widget Management");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   XtSetArg (args[n], XmNchildType, XmFRAME_TITLE_CHILD);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   manageTitle = XmCreateLabelGadget(manageFrame,"manageTitle",args,n);
   XtManageChild (manageTitle);
   XmStringFree (labelString);

   /* Create the Radio Box widget to hold the toggle buttons */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	      n++;
   radioBox = XmCreateRadioBox(manageForm, "radioBox", args, n);
   XtManageChild (radioBox);
  

   /* Create the Create Widget button */
   labelString = XmStringCreateLocalized("Create Sample Help Widget");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   createSampleWidgetBtn = 
           XmCreateToggleButtonGadget(radioBox,"createSampleWidgetBtn",args,n);
   XtManageChild (createSampleWidgetBtn);
   XmStringFree (labelString);

   XtAddCallback(createSampleWidgetBtn, XmNarmCallback,
                 ManageButtonsCB, (XtPointer)CREATE_SAMPLE);
  


   /* Create the Create Widget button */
   labelString = XmStringCreateLocalized("Destroy Sample Help Widget");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   destroySampleWidgetBtn = 
          XmCreateToggleButtonGadget(radioBox,"destroySampleWidgetBtn",args,n);
   XtManageChild (destroySampleWidgetBtn);
   XmStringFree (labelString);

   XtSetSensitive(destroySampleWidgetBtn, FALSE);


   XtAddCallback(destroySampleWidgetBtn, XmNarmCallback,
                 ManageButtonsCB, (XtPointer)DESTROY_SAMPLE);
  


   /****** Create the Control1 section Frame ******/
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, manageFrame);                 n++; 
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 5);				n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginWidth, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 2);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   control1Frame = XmCreateFrame(form, "control1Frame", args, n);
   XtManageChild (control1Frame);

   control1Form = XmCreateForm (control1Frame,"control1Form", NULL, 0);
   XtManageChild (control1Form);


   XtAddCallback(control1Form, XmNhelpCallback, 
                 HelpCB, CONTROL1_FRAME);

   /* create the frame title */
   labelString = XmStringCreateLocalized("Controlling the Help Widgets");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   XtSetArg (args[n], XmNchildType, XmFRAME_TITLE_CHILD);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   control1Title = XmCreateLabelGadget(control1Frame,"control1Title",args,n);
   XtManageChild (control1Title);
   XmStringFree (labelString);

   /* Create the Radio Box widget to hold the toggle buttons */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	      n++;
   controlBox1 = XmCreateRowColumn(control1Form, "controlBox1", args, n);
   XtManageChild (controlBox1);
  

   /* Create the Create Widget button */
   labelString = XmStringCreateLocalized("Change the Content");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   controlBtn1 = 
          XmCreateToggleButtonGadget(controlBox1,"controlBtn1",args,n);
   XtManageChild (controlBtn1);
   XmStringFree (labelString);
   XtSetSensitive(controlBtn1, FALSE);

   XtAddCallback(controlBtn1, XmNarmCallback,
                 Control1ButtonsCB, (XtPointer)CHANGE_CONTENT);

   /* Change Size button*/
   labelString = XmStringCreateLocalized("Change the Size");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   controlBtn2 = 
        XmCreateToggleButtonGadget(controlBox1,"controlBtn2",args,n);
   XtManageChild (controlBtn2);
   XmStringFree (labelString);
   XtSetSensitive(controlBtn2, FALSE);



   XtAddCallback(controlBtn2, XmNarmCallback,
                 Control1ButtonsCB, (XtPointer)CHANGE_SIZE);



   /* Control the GUI*/
   labelString = XmStringCreateLocalized("Control the User Interface");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   controlBtn3 = 
        XmCreateToggleButtonGadget(controlBox1,"controlBtn3",args,n);
   XtManageChild (controlBtn3);
   XmStringFree (labelString);
   XtSetSensitive(controlBtn3, FALSE);


   XtAddCallback(controlBtn3, XmNarmCallback,
                 Control1ButtonsCB, (XtPointer)CHANGE_GUI);
  


   /****** Create the Control2 section Frame ******/
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, control1Frame);                       n++; 
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 5);				n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginWidth, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 2);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   control2Frame = XmCreateFrame(form, "control2Frame", args, n);
   XtManageChild (control2Frame);

   control2Form = XmCreateForm (control2Frame,"control2Form", NULL, 0);
   XtManageChild (control2Form);

   XtAddCallback(control2Form, XmNhelpCallback, 
                 HelpCB, CONTROL2_FRAME);


   /* create the frame title */
   labelString =
     XmStringCreateLocalized ("Controlling the Client Via Help Widgets");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   XtSetArg (args[n], XmNchildType, XmFRAME_TITLE_CHILD);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   control2Title = XmCreateLabelGadget(control2Frame,"control2Title",args,n);
   XtManageChild (control2Title);
   XmStringFree (labelString);

   /* Create the Radio Box widget to hold the toggle buttons */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	      n++;
   controlBox2 = XmCreateRowColumn(control2Form, "controlBox2", args, n);
   XtManageChild (controlBox2);
  

   /* Create the Create Widget button */
   labelString = XmStringCreateLocalized("Client controled hypertext links");
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   controlBtn4 = 
          XmCreatePushButtonGadget(controlBox2,"controlBtn4",args,n);
   XtManageChild (controlBtn4);
   XmStringFree (labelString);
  
   XtAddCallback(controlBtn4, XmNarmCallback,
                 Control1ButtonsCB, (XtPointer)SHOW_APP_DEFINED_LINKS);
  




  /* Set MainWindow areas */
  XmMainWindowSetAreas (main_window, menuBar, NULL, NULL, NULL,	frame);
      

  /* Force a size to the form */
/*
  n = 0;
  XtSetArg(args[n], XmNwidth, 400); n++;
  XtSetArg(args[n], XmNheight, 200); n++;      
  XtSetValues(form, args, n);
*/
  XtRealizeWidget (parent);


} /* End CreateMainWindow */



/*********************************************************************
 * Function:  BuildMenus
 *
 *   This procedure builds the menubar pulldows for our demo program
 *
  *********************************************************************/
static Widget BuildMenus(
    Widget parent)

{
   Widget       menuBar;
   Widget       c[10];
   Widget	cascadeB2;
   Widget       menupane1, menupane2; 
   Widget       fileBtn1;
   Widget       helpBtn1, helpBtn2, helpBtn3;
   Arg		args[10];	
   int          n;
   XmString     accText;
   XmString     labelStr;
   char       * mnemonic;

   menuBar = XmCreateMenuBar (parent, "menuBar", args, 0); 
   XtManageChild (menuBar);


  /*******************************************************
   * Menupane:  Exit, and related buttons 
   *******************************************************/
   
  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane1 = XmCreatePulldownMenu(menuBar, "menupane1", args, n);
  

  /* Exit button */
  labelStr = XmStringCreateLocalized("Exit");
  accText = XmStringCreateLocalized("<Ctrl>X");
  mnemonic = "x";

  n = 0;
  XtSetArg(args[n], XmNaccelerator,"Ctrl <Key>X"); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNacceleratorText ,accText); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] = fileBtn1 =
	   XmCreatePushButtonGadget(menupane1,"fileBtn1", args, n);
   
  XtAddCallback(fileBtn1, XmNactivateCallback, CloseCB,(XtPointer) NULL);
   

  XmStringFree(accText);
  XmStringFree(labelStr); 

  XtManageChildren(c, 1);
 

  /*******************************************************
   * Menupane:  Help, and related buttons 
   *******************************************************/
  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 0);		++n;
  XtSetArg(args[n], XmNmarginHeight, 0);		++n;
  menupane2 = XmCreatePulldownMenu(menuBar, "menupane2", args, n);

  
  /* Item help button */
  labelStr = XmStringCreateLocalized("On Item");
  accText = XmStringCreateLocalized("<Ctrl>I");
  mnemonic = "I";
   
  n = 0;
  XtSetArg(args[n], XmNaccelerator,"Ctrl <Key>I"); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNacceleratorText ,accText); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[0] = helpBtn1 = XmCreatePushButtonGadget(menupane2,"helpBtn1", args, n);
  XtAddCallback(helpBtn1, XmNactivateCallback,
                HelpMenuCB, (XtPointer)HELP_ON_ITEM);
    
  XmStringFree(accText);
  XmStringFree(labelStr); 
   


  /* On application button */
  labelStr = XmStringCreateLocalized("On Application");
  accText = XmStringCreateLocalized("<Ctrl>A");
  mnemonic = "A";

  n = 0; 
  XtSetArg(args[n], XmNaccelerator,"Ctrl <Key>A"); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNacceleratorText ,accText); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[1] = helpBtn2 = XmCreatePushButtonGadget(menupane2,
                                            "helpBtn2", args, n);
  XmStringFree(accText);
  XmStringFree(labelStr); 
 
  XtAddCallback(helpBtn2, XmNactivateCallback,
                HelpMenuCB, (XtPointer)HELP_ON_TOPIC);


  /* On Version button */
  labelStr = XmStringCreateLocalized("On Version");
  accText = XmStringCreateLocalized("<Ctrl>V");
  mnemonic = "V";

  n = 0;
  XtSetArg(args[n], XmNaccelerator,"Ctrl <Key>V"); n++;
  XtSetArg(args[n], XmNlabelString, labelStr); n++;
  XtSetArg(args[n], XmNacceleratorText ,accText); n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
  c[2] = helpBtn3 = XmCreatePushButtonGadget(menupane2,
				          "helpBtn3",args, n);
  XtAddCallback(helpBtn3, XmNactivateCallback, 
                HelpMenuCB, (XtPointer)HELP_ON_VERSION);
  
   XmStringFree(accText);
   XmStringFree(labelStr); 
 
   XtManageChildren(c, 3);
 

   /* Create Cascade buttons for menubar */
   labelStr = XmStringCreateLocalized("File");
   mnemonic = "F";
   n = 0;
   XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
   XtSetArg(args[n], XmNsubMenuId, menupane1); n++;
   XtSetArg(args[n], XmNlabelString, labelStr); n++;
   c[0] = XmCreateCascadeButton(menuBar,"cascadeB1",args, n); 
   XmStringFree(labelStr); 


   labelStr = XmStringCreateLocalized("Help");
   mnemonic = "H";
   n = 0;
   XtSetArg(args[n], XmNmnemonic, mnemonic[0]); n++;
   XtSetArg(args[n], XmNsubMenuId, menupane2); n++;
   XtSetArg(args[n], XmNlabelString, labelStr); n++;
   c[1] = cascadeB2 = XmCreateCascadeButton(menuBar, "cascadeB2",
					    args, n); 
   XmStringFree(labelStr); 

   XtManageChildren(c, 2);

  
   /* Attaches the Help menupane to the right side */
   n = 0;
   XtSetArg (args[n], XmNmenuHelpWidget, cascadeB2);  n++;
   XtSetValues (menuBar, args, n);

   return(menuBar);


}  /* End BuildMenus */



/*********************************************************************
 * Function: ManageButtonsCallback
 *
 * Description: 
 *
 *********************************************************************/
static void ManageButtonsCB(
    Widget  parent,
    XtPointer clientData,
    XtPointer callData)
{

  int      n;
  Arg      args[10];	
  Widget   helpWidget, printWidget;

  TurnOnHourGlass(topLevel);

  switch ((long)clientData)
    {
      case CREATE_SAMPLE:
   
      XtSetSensitive(destroySampleWidgetBtn, TRUE);
      XtSetSensitive(createSampleWidgetBtn, FALSE);
      XtSetSensitive(controlBtn1, TRUE);
      XtSetSensitive(controlBtn2, TRUE);
      XtSetSensitive(controlBtn3, TRUE);

      /* Set the state of our toggle buttons */
      XmToggleButtonSetState(destroySampleWidgetBtn, FALSE, FALSE);

     if (quickWidget==NULL)
        {
            Widget dp = parent;
            while (!XtIsSubclass(dp, applicationShellWidgetClass))
                    dp = XtParent(dp);
  
            n =0;
            XtSetArg (args[n], XmNtitle, "Quick Dialog Sample");  n++;
            XtSetArg (args[n], DtNhelpVolume,HelpdemoHelpVol);   n++;
            XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC);  n++;
            XtSetArg (args[n], DtNlocationId, APP_MENU_ID);  n++;
            quickWidget = DtCreateHelpQuickDialog(dp,"quickWidget",args, n);
            

            XtAddCallback(quickWidget, DtNcloseCallback,
                          CloseAndDestroyCB, (XtPointer)NULL);

            /* Add the popup position callback to our main help dialog */
            XtAddCallback (XtParent(quickWidget), XmNpopupCallback,
                          (XtCallbackProc)HelpMapCB, 
                          (XtPointer)dp);
 
            /* Add our hypertext callback here */
            XtAddCallback(quickWidget, DtNhyperLinkCallback,
                       ProcessLinkCB, NULL);



            /* We do not want a print button for now so we unmap it */     
            printWidget = DtHelpQuickDialogGetChild (quickWidget, 
                                         DtHELP_QUICK_PRINT_BUTTON);
            XtUnmanageChild (printWidget);
  

            /* We do not want a help button for now so we unmap it */     
            helpWidget = DtHelpQuickDialogGetChild (quickWidget, 
                                        DtHELP_QUICK_HELP_BUTTON);
            XtUnmanageChild (helpWidget);

            XtManageChild(quickWidget);

          }
        break;

      case DESTROY_SAMPLE:

        XtSetSensitive(destroySampleWidgetBtn, FALSE);
        XtSetSensitive(createSampleWidgetBtn, TRUE);
        XtSetSensitive(controlBtn1, FALSE);
        XtSetSensitive(controlBtn2, FALSE);
        XtSetSensitive(controlBtn3, FALSE);


        /* Set the state of our toggle buttons */
        XmToggleButtonSetState(createSampleWidgetBtn, FALSE, FALSE);
        XmToggleButtonSetState(controlBtn1, FALSE, FALSE);
        XmToggleButtonSetState(controlBtn2, FALSE, FALSE);
        XmToggleButtonSetState(controlBtn3, FALSE, FALSE);


        /* Reinitialize our toggle button state flags */
        addMoreButton=TRUE;
        goBigger=TRUE;
        changeTopic=TRUE;


        if (quickWidget != NULL)
        XtDestroyWidget(quickWidget);

        quickWidget = NULL;
        moreButton = NULL;

        break;
     
      default:

        break;
    } /* End of switch statement */

  TurnOffHourGlass(topLevel);


}





/*********************************************************************
 * Function: ManageButtonsCallback
 *
 * Description: 
 *
 *********************************************************************/
static void Control1ButtonsCB(
    Widget  parent,
    XtPointer clientData,
    XtPointer callData)
{

  int               n;
  Arg               args[10];	
  static Dimension  columns=0;
  static Dimension  rows=0;
  

  TurnOnHourGlass(topLevel);

  switch ((long)clientData)
    {
      case CHANGE_CONTENT:
         
        /* Show a man page */
        if (changeTopic)
          {
            n =0;
            XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC);  n++;
            XtSetArg (args[n], DtNlocationId, CREATE_FRAME);  n++;
            XtSetValues(quickWidget, args, n);
            changeTopic = FALSE;
          }
        else
	  {
            /* Show Topic */
            n = 0;
            XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC);  n++;
            XtSetArg (args[n], DtNlocationId, APP_MENU_ID);      n++;
            XtSetValues(quickWidget, args, n);
            changeTopic = TRUE;
           }

        break;

      case CHANGE_GUI:
    
        if (moreButton == NULL)
          {
            /* Get the more buttion child */
            moreButton = DtHelpQuickDialogGetChild(quickWidget, 
                                                   DtHELP_QUICK_MORE_BUTTON);

            XtAddCallback(moreButton, XmNactivateCallback,
                 DisplayMoreCB, (XtPointer)NULL);
  
 
          }
        
        if (addMoreButton)
          {

              /* manage the child */
              XtManageChild(moreButton);
              addMoreButton = FALSE;
          }
        else
	  {
            /* unmanage more button */
            XtUnmanageChild(moreButton);
            addMoreButton = TRUE;

          }
 
        break;

      case CHANGE_SIZE:

        if (rows == 0)
          {
            /* Get the current dialog size */
            n =0;
            XtSetArg (args[n], DtNcolumns, &columns);  n++;
            XtSetArg (args[n], DtNrows, &rows);   n++;
            XtGetValues(quickWidget, args, n);
          }

        if (goBigger)
          {
            n =0;
            XtSetArg (args[n], DtNcolumns, columns+5);  n++;
            XtSetArg (args[n], DtNrows, rows+5);   n++;
            XtSetValues(quickWidget, args, n);
            goBigger = FALSE;
          }
        else
	  {
            /* Go smaller */
            n =0;
            XtSetArg (args[n], DtNcolumns, columns);  n++;
            XtSetArg (args[n], DtNrows, rows);   n++;
            XtSetValues(quickWidget, args, n);
            goBigger = TRUE;
          }

      
        break;
      
      case SHOW_APP_DEFINED_LINKS:
         
         /* display the general dialog w/more help in it */
         DisplayMain (parent, HelpdemoHelpVol, CONTROL2_FRAME);

 
        break;

      default:

        break;
    } /* End of switch statement */


  TurnOffHourGlass(topLevel);


}


/*********************************************************************
 * Function: HelpMenuCB
 *
 * Description: This is the callback that is called when the user selects
 *              any of the help menu items.
 *
 *********************************************************************/
static void HelpMenuCB(
    Widget  widget,
    XtPointer clientData,
    XtPointer callData)

{
  Widget selWidget=NULL;
  String name;
  Arg  argList[10];
  Cardinal argCount;
  int      	n;
  int  status=DtHELP_SELECT_ERROR;



  TurnOnHourGlass(topLevel);


  /* Determine which help button was activated and display the
   * appropriate help information */

   switch ((long)clientData)
	   {
	     case HELP_ON_ITEM:
               while (!XtIsSubclass(widget, applicationShellWidgetClass))
                   widget = XtParent(widget);


               status = DtHelpReturnSelectedWidgetId(widget, 0, &selWidget);

                switch ((int)status)
	          {
		    case DtHELP_SELECT_ERROR:
                      printf("Selection Error, cannot continue\n");
                    break;
		    case DtHELP_SELECT_VALID:
                      while (selWidget != NULL)
                        {
                          if ((XtHasCallbacks(selWidget, XmNhelpCallback) 
                                            == XtCallbackHasSome))
                            {
                               XtCallCallbacks((Widget)selWidget, 
                                               XmNhelpCallback,NULL);
                               break;
                            }
                          else
                           selWidget = XtParent(selWidget);
                        }
                    break;
		    case DtHELP_SELECT_ABORT:
                      printf("Selection Aborted by user.\n");
                    break;
		    case DtHELP_SELECT_INVALID:
                      printf("You must select a component within your app.\n");
                    break;

                  }

               break;

	     case HELP_ON_TOPIC:
               DisplayMain (widget, HelpdemoHelpVol, APP_MENU_ID);

               break;
 
             case HELP_ON_VERSION:
               DisplayVersion (widget, HelpdemoHelpVol, VER_MENU_ID);
               break;

           
             default:

             break;
           } /* End of switch statement */

  TurnOffHourGlass(topLevel);


}




/*********************************************************************
 * Function: HelpCB
 *
 * Description: This is the callback that is called when the user selects
 *              item help or F1 key help
 *
 *********************************************************************/
static void HelpCB(
    Widget  widget,
    XtPointer clientData,
    XtPointer callData)
{

  char * locationId = (char *) clientData;



  /* Just display the proper help based on the id string passed in */
  /* We pass in a NULL for our helpVolume because and let the value
   * defined in the app-defaults file be used.
   */

  DisplayMain (widget, HelpdemoHelpVol, locationId);


}




/*********************************************************************
 * Function: CloseCB
 *
 * Description: This is the callback that is called when the user selects
 *              the Exit menu button.
 *
 *********************************************************************/
static void CloseCB (
    Widget	w,
    XtPointer	clientData,
    XtPointer  	callData)

{
  /* Now we go away */
  exit(0);

}




/*********************************************************************
 * Function: DisplayMoreCB
 *
 * Description: Handles the selection of the more button added to the 
 *              sample quick help dialog widget.  It causes a 
 *              general help dialog to be posted w/more informaion in it.
 *
 *********************************************************************/
static void DisplayMoreCB (
    Widget	w,
    XtPointer	clientData,
    XtPointer   callData)

{

  /* display the general dialog w/more help in it */
  DisplayMain (w, HelpdemoHelpVol, MORE_BTN_ID);





}


/*********************************************************************
 * Function: CloseAndDestroyCB
 *
 * Description: Closes and destroys our sample quick help dialog widget.
 *
 *********************************************************************/
static void CloseAndDestroyCB (
    Widget	w,
    XtPointer	clientData,
    XtPointer   callData)

{

  /* Close and destroy the our quick dialog test widget */
  XtUnmanageChild (quickWidget);
  XtDestroyWidget(quickWidget);


  /* reinitialize our global sample widget, and associated moreButton */
  quickWidget = NULL;
  moreButton=NULL; 
  addMoreButton=TRUE;


  XtSetSensitive(destroySampleWidgetBtn, FALSE);
  XtSetSensitive(createSampleWidgetBtn, TRUE);
  XtSetSensitive(controlBtn1, FALSE);
  XtSetSensitive(controlBtn2, FALSE);
  XtSetSensitive(controlBtn3, FALSE);
  
  /* Set the state of our toggle buttons */
  XmToggleButtonSetState(createSampleWidgetBtn, FALSE, FALSE);

  XmToggleButtonSetState(controlBtn1, FALSE, FALSE);
  XmToggleButtonSetState(controlBtn2, FALSE, FALSE);
  XmToggleButtonSetState(controlBtn3, FALSE, FALSE);


  /* Reinitialize our toggle button state flags */
  addMoreButton=TRUE;
  goBigger=TRUE;
  changeTopic=TRUE;


  
}

























