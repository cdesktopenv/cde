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
/* $TOG: Print.c /main/10 1999/02/09 19:45:00 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Print.c
 **
 **   Project:     Cache Creek (Rivers) Project
 **
 **   Description: Builds and displays an instance of a Cache Creek Print
 **                Dialog.  
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
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>   /* for getenv() */
#include <string.h>
#include <unistd.h>


#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/DialogS.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/ToggleB.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

/*
 * private includes
 */
#include "DisplayAreaI.h"

#include "bufioI.h"
#include "Access.h"
#include "AccessI.h"
#include "MessagesP.h"
#include "FormatI.h"
#include "HelpI.h"
#include "HelposI.h"
#include "HelpUtilI.h"
#include "HelpAccessI.h"
#include "HelpDialogI.h"
#include "HelpDialogP.h"
#include "PrintI.h"


/***** helper structures ****/
typedef struct _DtHelpPrintCBRec {
   Widget			widget;
   _DtHelpDisplayWidgetStuff *	display;
   _DtHelpCommonHelpStuff *	help;
   _DtHelpPrintStuff *		print;
} _DtHelpPrintCBRec;

/***** Global Variables ****/
char _DtHelpDefaultHelpPrint[] = "/usr/dt/bin/dthelpprint";

/* The order of these names must match the constants DtHELP_PAPERSIZE_xxx */
char *_DtHelpPaperSizeNames[] =
{   "help_papersize_letter",
    "help_papersize_legal",
    "help_papersize_executive",
    "help_papersize_a4",
    "help_papersize_b5",
};
int _DtHelpPaperSizeNamesCnt = (sizeof(_DtHelpPaperSizeNames) / sizeof(char *));

/***** Constants ******/
#define EOS	'\0'    /* end of string */
#define EMPTYSTR   s_EmptyStr

#define PRSET   4       /* msg catalog set for print dialog */

#define PrintMessage001  _DtHelpMsg_0004

/***** Static Variables ******/
static char s_EmptyStr[] = "";

/****** Protos ******/
static void ClosePrintCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData);
static void StartPrintingCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData);
static void CreatePrintDialog(
   Widget 		widget,
   _DtHelpPrintStuff *	print,
   _DtHelpDisplayWidgetStuff * display,
   _DtHelpCommonHelpStuff * help);

/************************************************************************
 * Function: CreatePrintCBRec()
 *
 *	Create the data required by a print callback 
 *
 * This routine allocates memory for the callback record using XtCalloc().
 * When the record is no longer needed, free it with XtFree().
 ************************************************************************/
static
_DtHelpPrintCBRec * CreatePrintCBRec(
   Widget			widget,
   _DtHelpDisplayWidgetStuff *	display,
   _DtHelpCommonHelpStuff *	help,
   _DtHelpPrintStuff *		print)
{
   _DtHelpPrintCBRec * rec = NULL;
 
   rec = (_DtHelpPrintCBRec *) XtCalloc(1,sizeof(_DtHelpPrintCBRec));
   if (NULL == rec) return NULL;
   rec->widget = widget;
   rec->display = display;
   rec->help = help;
   rec->print = print;
   return rec;
}


/************************************************************************
 * Function: _DtHelpInitPrintStuff()
 *
 *	Init print-related data
 *
 ************************************************************************/
void _DtHelpInitPrintStuff (
    _DtHelpPrintStuff * print)
{
    /* Set our print display stuff to initial values */
    if (print->printer != NULL)
      print->printer = XtNewString(print->printer);
  
    if (print->helpPrint != _DtHelpDefaultHelpPrint)
      print->helpPrint = XtNewString(print->helpPrint);
  
    print->paperSize      = DtHELP_PAPERSIZE_LETTER;
    print->printVolume    = NULL;
  
    /* print dialog widgets */
    print->printForm      = NULL;
    print->subject        = NULL;
    print->printerField   = NULL;
    print->copiesField    = NULL;
    print->letterBtn = NULL;
    print->legalBtn = NULL;
    print->execBtn = NULL;
    print->b5Btn = NULL;
    print->a4Btn     = NULL;
    print->topicsFrame    = NULL;
    print->curTopicBtn    = NULL;
    print->subTopicsBtn   = NULL;
    print->tocBtn   = NULL;
    print->allTopicsBtn   = NULL;
    print->paperTopicsSeparator   = NULL;
    print->topicsBtnsSeparator    = NULL;
}


/************************************************************************
 * Function: _DtHelpFreePrintStuff()
 *
 *	Init print-related data
 *
 ************************************************************************/
void _DtHelpFreePrintStuff (
    _DtHelpPrintStuff * print,
    int			cleanUpKind)
{
     XtFree(print->printVolume);
     print->printVolume = NULL;

     if (print->printForm != NULL)
     {
          XtUnmanageChild(print->printForm);
     }

    if (cleanUpKind == DtCLEAN_FOR_DESTROY)
    {
       if (print->helpPrint != _DtHelpDefaultHelpPrint)
           XtFree(print->helpPrint);
       print->helpPrint = NULL;

       XtFree(print->printer);
       print->printer = NULL;
    }
}


/************************************************************************
 * Function: _DtHelpPrintSetValues()
 *
 *	set print-related data
 *
 ************************************************************************/
void _DtHelpPrintSetValues (
    _DtHelpPrintStuff * currentPrint,
    _DtHelpPrintStuff * newPrint,
    _DtHelpDisplayWidgetStuff * newDisplay,
    _DtHelpCommonHelpStuff * newCommonHelp)
{
  /* Check DtNhelpPrint resource for change */
  if (currentPrint->helpPrint != newPrint->helpPrint)
    {
      newPrint->helpPrint = XtNewString(newPrint->helpPrint);

      if (currentPrint->helpPrint != _DtHelpDefaultHelpPrint)
        XtFree(currentPrint->helpPrint);
    }

  /* Check DtNprinter resource for change */
  if (currentPrint->printer != newPrint->printer)
    {
      newPrint->printer = XtNewString(newPrint->printer);
      XtFree(currentPrint->printer);
    }


  /* Do NOT check DtNpaperSize resource for change */
  /* It is a C-only resource */
}



/************************************************************************
 * Function: ClosePrintCB()
 *
 *	Close the Print dialog.
 *
 ************************************************************************/
static void ClosePrintCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
  _DtHelpPrintCBRec * printrec = (_DtHelpPrintCBRec *) clientData;
 
  /* We unmap the print dialog */
  XtUnmanageChild(printrec->print->printForm);
}


/************************************************************************
 * Function: DestroyDialogCB()
 *
 *	Free allocated memory
 *
 ************************************************************************/
static void DestroyDialogCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   _DtHelpPrintCBRec * printrec = (_DtHelpPrintCBRec *) clientData;
   XtFree((char *)printrec);
}

/************************************************************************
 * Function: CheckCopiesCB()
 *
 *	Check whether the copies in the copies field is valid
 *
 ************************************************************************/
static void CheckCopiesCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
/*   _DtHelpPrintCBRec * printrec = (_DtHelpPrintCBRec *) clientData;  */
}


/************************************************************************
 * Function: CheckPrinterCB()
 *
 *	Check whether the printer in the printer field is valid
 *      and store a valid printer string in printPrinter.
 *
 ************************************************************************/
static void CheckPrinterCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
/*   _DtHelpPrintCBRec * printrec = (_DtHelpPrintCBRec *) clientData;  */
}


/************************************************************************
 * Function: StartPrintingCB()
 *
 *	Start printing and close the PRINT Dialog
 *
 ************************************************************************/
static void StartPrintingCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   _DtHelpPrintCBRec * printrec = (_DtHelpPrintCBRec *) clientData;
   int printType=0;
   char *printTopic=NULL;
   char *topicTitle=NULL;
   Boolean printAll=False;
   Boolean printSub=False;
   Boolean printTocIndex=False;
   char * printer;
   XmString titleLbl = NULL;
   int    paperSize;
   int    copyCnt;
   char * copies;
   Widget dfltSize = NULL;
   Arg args[5];

   /* Determine the current selected print type */

   if (XmToggleButtonGetState(printrec->print->allTopicsBtn))
     {
       printType = _DtPRINT_ALL;
       printTopic = printrec->help->topLevelId;
       printAll = True;
     }
   else
   if (XmToggleButtonGetState(printrec->print->tocBtn))
     {
       printType = _DtPRINT_TOC;
       printTopic = NULL;
       printTocIndex = True;
     }
   else
   if (XmToggleButtonGetState(printrec->print->subTopicsBtn))
     {
       printType = _DtPRINT_SUB;
       printTopic = printrec->display->locationId;
       printSub = True;
     }
   else
   if (XmToggleButtonGetState(printrec->print->curTopicBtn))
     {
       printType = _DtPRINT_CURRENT;
       switch (printrec->display->helpType)
         {
            case DtHELP_TYPE_TOPIC:
              printTopic = printrec->display->locationId;
            break;

            case DtHELP_TYPE_MAN_PAGE:
              printTopic = printrec->display->manPage;
            break;

            case DtHELP_TYPE_FILE:
               printTopic = printrec->display->helpFile;
               titleLbl   = printrec->display->topicTitleLbl;
            break;

            case DtHELP_TYPE_STRING:
            case DtHELP_TYPE_DYNAMIC_STRING:
              /* ??? Full support for dynamic string types */
              printTopic = printrec->display->stringData;
              titleLbl   = printrec->display->topicTitleLbl;
            break;
   
          }  /* End Switch Statement */
     }

   /*
    * change the XmString into a char *.
    */
  if (titleLbl != NULL)
    {
      int                len;
      int                newLen = 0;
      XmStringContext    theContext;
      XmStringCharSet    theSet;
      XmStringDirection  theDir;
      char              *newTxt;
      Boolean            theSep;

      if (XmStringInitContext(&theContext, titleLbl) == True)
	{
	  while (XmStringGetNextSegment(theContext,
					&newTxt,
					&theSet,
					&theDir,
					&theSep) == True)
	    {
	      len = strlen(newTxt);
	      if (len > 0)
		{
	          topicTitle = XtRealloc(topicTitle, len + 1);
	          if (topicTitle != NULL)
		    {
	              strcpy(&topicTitle[newLen], newTxt);
		      newLen += len;
		    }
		}
	    }
	  XmStringFreeContext(theContext);
	}
    }

   /* get printer */
   printer = XmTextFieldGetString(printrec->print->printerField);
   if (printer && printer[0] == EOS) 
     { XtFree(printer); printer = NULL; }

   /* get copies */
   copies = XmTextFieldGetString(printrec->print->copiesField);
   copyCnt = 1;
   if (   NULL == copies
       || (copies && copies[0] == EOS) 
       || (copies && sscanf(copies,"%d",&copyCnt) != 1 )
       || (copyCnt < 0 || copyCnt > 500) )
     { XtFree(copies);  copies = XtNewString("1"); }

   /* get paper size */
    /* Get the chosen size */
    XtSetArg (args[0], XmNmenuHistory, &dfltSize); 
    XtGetValues (printrec->print->paperSizeOptMenu, args, 1);
    if (dfltSize == printrec->print->legalBtn)
       paperSize = DtHELP_PAPERSIZE_LEGAL;
    else if (dfltSize == printrec->print->execBtn)
       paperSize = DtHELP_PAPERSIZE_EXECUTIVE;
    else if (dfltSize == printrec->print->a4Btn)
       paperSize = DtHELP_PAPERSIZE_A4;
    else if (dfltSize == printrec->print->b5Btn)
       paperSize = DtHELP_PAPERSIZE_B5;
    else
       paperSize = DtHELP_PAPERSIZE_LETTER;

   if (printType != 0)
   {
       /* Call the print setup routine to start the print job */
       _DtHelpPrintJob(
                    printrec->widget,
                    printrec->print->helpPrint,
                    printer,
                    paperSize,
                    copies,
                    printrec->print->printVolume,
                    printrec->display->helpType,
                    printTopic,
                    printAll,
                    printSub,
                    printTocIndex,
                    topicTitle);
   }

   XtFree(printer);
   XtFree(copies);
   XtFree(topicTitle);

   /* We unmap the print dialog */
   XtUnmanageChild(printrec->print->printForm);
}



/*****************************************************************************
 * Function:      void _DtHelpUpdatePrintDialog();
 *
 *
 * Parameters:      new      Specifies the help widget.
 *
 * Return Value:
 *
 * Purpose:       Updates the print dialog to reflect current topic
 *
 *****************************************************************************/
void _DtHelpUpdatePrintDialog(
    _DtHelpPrintStuff *       print,
    _DtHelpDisplayWidgetStuff * display,
    _DtHelpCommonHelpStuff *  help,
    Boolean                   setDefaults)
{
   int n;
   Arg args[5];
   char * titleStr;
   XmString labelString=NULL;
   XmString volumeString=NULL;
   XmFontList fontList = NULL;
   Boolean mod = False;
   char buf[400];

   /* only work on print dialog if we need to */
   if (print->printForm == NULL) return;

   /* Set the proper default toggle button value */
   if ( setDefaults )
   {
      if(print->curTopicBtn)
         XmToggleButtonSetState(print->curTopicBtn, True, False);
      if(print->subTopicsBtn)
         XmToggleButtonSetState(print->subTopicsBtn,False,False);
      if(print->tocBtn)
         XmToggleButtonSetState(print->tocBtn,False,False);
      if(print->allTopicsBtn)
         XmToggleButtonSetState(print->allTopicsBtn,False,False);
   }

   /* adj where the TopicsBtn separator is attached */
   /* if attached to the PaperTopics separator, the topics are hidden */
   n = 0;
   if ( display->helpType == DtHELP_TYPE_TOPIC )
   {
#if 1
      XtSetSensitive(print->topicsFrame,True);
#else
      XtManageChild(print->topicsFrame);
      XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);          n++;
      XtSetArg (args[n], XmNtopWidget, print->topicsFrame);           n++;
#endif
   }
   else
   {
#if 1
      XtSetSensitive(print->topicsFrame,False);
#else
      XtUnmanageChild(print->topicsFrame);
      XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);          n++;
      XtSetArg (args[n], XmNtopWidget, print->paperTopicsSeparator);  n++;
#endif
   }
   XtSetValues (print->topicsBtnsSeparator, args, n);

   /* update the dialog's print subject label string */
   switch ( display->helpType )
   {
      /* coming out of the case, volumeString & titleStr should be set */
      case DtHELP_TYPE_TOPIC:
         titleStr = _DTGETMESSAGE(PRSET, 50,"Help Volume: ");

         /* get volume title and allow for localized fonts */

         /* get the font list of the label */
         XtSetArg (args[0], XmNfontList, &fontList);
         XtGetValues (print->subject, args, 1);

#if defined(DONT_USE_CDExc22774)
	 /* Don't need to copy, _DtHelpFormatVolumeTitle copies 
	  * before modifying.
	  */

         /* copy the list before passing it in for modification */
         /* we must free this now */
         fontList = XmFontListCopy (fontList);
#endif

         /* get formated volume title; volumeString is owned by caller */
         _DtHelpFormatVolumeTitle(help->pDisplayArea,display->volumeHandle,
                                    &volumeString,&fontList,&mod);
 
         /* if volumeString caused a font list change, add it back */
         if (mod)
         { /* Add the title's font to the label */
            XtSetArg (args[0], XmNfontList, fontList);
            XtSetValues (print->subject, args, 1);
            if (fontList) XmFontListFree(fontList);
         }

         break;
      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
         titleStr = _DTGETMESSAGE(PRSET, 51,"Help Message");
         break;
      case DtHELP_TYPE_FILE:
         titleStr = _DTGETMESSAGE(PRSET, 52,"Help File");
         break;
      case DtHELP_TYPE_MAN_PAGE:
         /* assumption: buf won't overflow */
         sprintf(buf,"%s%s", _DTGETMESSAGE(PRSET, 53,"Manual Page: "),
                 display->manPage);
         titleStr = buf;
         break;
      default:
         titleStr = _DTGETMESSAGE(PRSET, 54,"Error message");
         break;
   }

   /* at this point, titleStr and volumeString are set */

   /* set the dialog label strings only if needed */
   labelString = XmStringCreateLocalized(titleStr);

   /* if there is a valid volumeString, concatenate it */
   if (volumeString)
   {
     XmString fullTitle;
     fullTitle = XmStringConcat(labelString,volumeString);
     XmStringFree(labelString);
     XmStringFree(volumeString);
     labelString = fullTitle;
   }

      /* set the dialog label string */
      XtSetArg (args[0], XmNlabelString, labelString);
      XtSetValues (print->subject, args, 1);
   XmStringFree (labelString);

   XmUpdateDisplay(print->printForm);
}



/*****************************************************************************
 * Function:	    void _DtHelpDisplayPrintDialog();
 *                             
 * 
 * Parameters:	
 *	new     	Specifies the help widget.
 *	print		print stuff
 *	display 	display widget stuff
 *	help		common help stuff
 *
 * Return Value:
 *
 * Purpose: 	    Creates and displays an instance of the print dialog.
 *
 *****************************************************************************/
void _DtHelpDisplayPrintDialog(
   Widget 		widget,
   _DtHelpPrintStuff *	print,
   _DtHelpDisplayWidgetStuff * display,
   _DtHelpCommonHelpStuff * help)
{
   Arg args[5];
   Widget dfltSize = NULL;

   /* Build a print dialog if we need to */
   if (print->printForm == NULL)
   {
      CreatePrintDialog(widget,print,display,help);

      /*** Only init the values if creating dialog for first time ***/

      switch(print->paperSize)
      {
         case DtHELP_PAPERSIZE_LEGAL:
             dfltSize = print->legalBtn;
             break;
         case DtHELP_PAPERSIZE_EXECUTIVE:
             dfltSize = print->execBtn;
             break;
         case DtHELP_PAPERSIZE_A4:
             dfltSize = print->a4Btn;
             break;
         case DtHELP_PAPERSIZE_B5:
             dfltSize = print->b5Btn;
             break;
         case DtHELP_PAPERSIZE_LETTER:
         default:
             dfltSize = print->letterBtn;
             break;
      }
      /* Set the default size */
      XtSetArg (args[0], XmNmenuHistory, dfltSize); 
      XtSetValues (print->paperSizeOptMenu, args, 1);

      /* update the variable contents of the dialog; do set defaults */
      _DtHelpUpdatePrintDialog(print,display,help,True);
   }
   else
   {
      /* update the variable contents of the dialog; dont set defaults */
      _DtHelpUpdatePrintDialog(print,display,help,False);
   }


   if ( XtIsManaged(print->printForm) == False )
   {
      /* Make sure the Print Dialog is managed */
      /* recall that the printForm is the form inside the dlg shell */
      XtManageChild(print->printForm);
      XtMapWidget(XtParent((Widget)print->printForm));
   }
   else
   {
      /* raise the window to top of the stack */
      Widget parent = XtParent(print->printForm);
      XRaiseWindow ( XtDisplay(parent), XtWindow(parent) );
   }
}



/*****************************************************************************
 * Function:	    Widget CreatePrintDialog(Widget nw);
 *                             
 * 
 * Parameters:      
 *
 * Return Value:
 *
 * Purpose: 	    Creates  an instance of the print dialog.
 *
 *****************************************************************************/
static void CreatePrintDialog(
   Widget 		widget,
   _DtHelpPrintStuff *	print,
   _DtHelpDisplayWidgetStuff * display,
   _DtHelpCommonHelpStuff * help)
{
   Widget printShell;
   Widget printForm;
   Widget topicsForm;
   Widget menupane;
   Widget frameTitle;
   Widget printerLabel;
   Widget copiesLabel;
   Widget radioBox;
   Widget separator;
   Widget okBtn, cancelBtn, helpBtn;
   XmString labelString;
   char * chrStr;
   DtHelpListStruct *pHelpInfo;
   _DtHelpPrintCBRec * printCBRec = NULL;
   int n;
   Arg args[20];

   /*  Create the shell and form used for the dialog.  */
   chrStr = XtNewString(((char *)_DTGETMESSAGE(PRSET, 1, "Help - Print")));
   n = 0;
   XtSetArg (args[n], XmNtitle, chrStr);	 			n++;
   printShell = XmCreateDialogShell(widget, "printShell", args, n);
   XtFree(chrStr);

   /* Set the useAsyncGeo on the shell */
   n = 0;
   XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
/*   XtSetValues (XtParent(printShell), args, n);*/
   XtSetValues (printShell, args, n);

   /* Allocate the printRec and arrange to free it when dialog destroyed */
   printCBRec = CreatePrintCBRec(widget,display,help,print);
   XtAddCallback (printShell, XmNdestroyCallback,
                  DestroyDialogCB, (XtPointer) printCBRec);

   /* create the form in the dialog to hold the contents */
   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);				n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNshadowThickness, 1);				n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);			n++;
   XtSetArg (args[n], XmNautoUnmanage, False);				n++;
   printForm = XmCreateForm (printShell, "printForm", args, n);

   /**** create the printing-related widgets ****/

   /* create print subject label */
   labelString = XmStringCreateLocalized(EMPTYSTR);
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);                 n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNleftOffset, 15);                               n++;
#if 0
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNrightOffset, 10);                               n++;
#endif
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);             n++;
   print->subject = 
                    XmCreateLabelGadget (printForm, "printSubject", args, n);
   XtManageChild (print->subject);
   XmStringFree (labelString);

   /* create printer label */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (PRSET, 10,"Printer:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, print->subject);	n++;
   XtSetArg (args[n], XmNtopOffset, 15);                                n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNleftOffset, 15);                               n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);             n++;
   printerLabel = XmCreateLabelGadget (printForm, "printerLabel", args, n);
   XtManageChild (printerLabel);
   XmStringFree (labelString);

   /* create the printer name text field */
   n = 0;
   /* get the printer value from printer resource, LPDEST, or leave blank */
   if ( print->printer != NULL && print->printer[0] != EOS )
      chrStr = print->printer;
   else if ( (chrStr = getenv("LPDEST")) == NULL || chrStr[0] == EOS)
      chrStr = (char *)_DTGETMESSAGE(PRSET, 60,"");   /* dflt printer value */
   XtSetArg (args[n], XmNvalue, chrStr );                               n++;
   XtSetArg (args[n], XmNcursorPosition, strlen(chrStr) );              n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, print->subject);	n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftWidget, printerLabel);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 60);				n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);                        n++;
   print->printerField = 
                        XmCreateTextField (printForm,"printerField",args, n);
   XtManageChild (print->printerField);
   XtAddCallback (print->printerField, XmNactivateCallback,
                  CheckPrinterCB, (XtPointer) printCBRec);
   XtAddCallback (print->printerField, XmNvalueChangedCallback,
                  CheckPrinterCB, (XtPointer) printCBRec);

   /* create copies label */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (PRSET, 11,"Copies:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget,print->subject);	n++;
   XtSetArg (args[n], XmNtopOffset, 15);                                n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNleftWidget, print->printerField);  n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);             n++;
   copiesLabel = XmCreateLabelGadget (printForm, "copiesLabel", args, n);
   XtManageChild (copiesLabel);
   XmStringFree (labelString);

   /* create the copies text field */
   n = 0;
   XtSetArg (args[n], XmNvalue,(char *)_DTGETMESSAGE(PRSET, 61,"1"));   n++;/*dflt copy cnt*/
   XtSetArg (args[n], XmNcursorPosition, 1 ); /* 1=strlen("1") */       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget,print->subject);	n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftWidget, copiesLabel);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);                        n++;
   print->copiesField = 
                        XmCreateTextField (printForm,"copiesField",args, n);
   XtManageChild (print->copiesField);
   XtAddCallback (print->copiesField, XmNactivateCallback,
                  CheckCopiesCB, (XtPointer) printCBRec);
   XtAddCallback (print->copiesField, XmNvalueChangedCallback,
                  CheckCopiesCB, (XtPointer) printCBRec);

   /* papersize option menu */
  /*******************************************************
   * Menupane:  Papersize buttons
   * No callback on each button is needed because we compare widget ptrs 
   *******************************************************/
 
   n = 0;
   XtSetArg(args[n], XmNmarginWidth, 0);         ++n;
   XtSetArg(args[n], XmNmarginHeight, 0);        ++n;
   menupane = XmCreatePulldownMenu(printForm, "paperSizeMenu", args, n);
 
   /* Letter button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE(PRSET, 21,"Letter 8.5 x 11 in")));
   n = 0;
   XtSetArg(args[n], XmNlabelString, labelString); n++;
   print->letterBtn = XmCreatePushButtonGadget(menupane, "letter", args, n);
   XtManageChild(print->letterBtn);
   XmStringFree(labelString);
 
   /* Legal button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE(PRSET, 22,"Legal 8.5 x 14 in")));
   n = 0;
   XtSetArg(args[n], XmNlabelString, labelString); n++;
   print->legalBtn = XmCreatePushButtonGadget(menupane, "legal", args, n);
   XtManageChild(print->legalBtn);
   XmStringFree(labelString);
 
   /* Executive button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE(PRSET, 23,"Executive 7.25 x 10.5 in")));
   n = 0;
   XtSetArg(args[n], XmNlabelString, labelString); n++;
   print->execBtn = XmCreatePushButtonGadget(menupane, "executive", args, n);
   XtManageChild(print->execBtn);
   XmStringFree(labelString);
 
   /* A4 button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE(PRSET, 24,"A4 210 x 297 mm")));
   n = 0;
   XtSetArg(args[n], XmNlabelString, labelString); n++;
   print->a4Btn = XmCreatePushButtonGadget(menupane, "A4", args, n);
   XtManageChild(print->a4Btn);
   XmStringFree(labelString);
 
   /* B5 button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE(PRSET, 25,"B5 182 x 257 mm")));
   n = 0;
   XtSetArg(args[n], XmNlabelString, labelString); n++;
   print->b5Btn = XmCreatePushButtonGadget(menupane, "B5", args, n);
   XtManageChild(print->b5Btn);
   XmStringFree(labelString);

   /* Option Menu */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE(PRSET, 20,"Paper Size:")));
   n = 0;
   XtSetArg(args[n], XmNlabelString, labelString);			n++;
   XtSetArg(args[n], XmNsubMenuId, menupane);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget,printerLabel);			n++;
   XtSetArg (args[n], XmNtopOffset, 15);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNrightOffset, 5);                               n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNleftOffset, 10);                               n++;
   XtSetArg (args[n], XmNmarginWidth, 5);                               n++;
   XtSetArg (args[n], XmNmarginHeight, 2);                              n++;
   print->paperSizeOptMenu = XmCreateOptionMenu(printForm,
                                        "paperSizeOptionMenu", args, n);
   XtManageChild(print->paperSizeOptMenu);
   XmStringFree(labelString);

   /* the DtNpaperSize resource is used to set the default item
      in _DtHelpDisplayPrintDialog() */

   /*  Create a separator between the paper size and topic range selection/buttons  */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, print->paperSizeOptMenu);          	n++;
   XtSetArg (args[n], XmNtopOffset, 0);					n++;
   XtSetArg (args[n], XmNseparatorType, XmNO_LINE);			n++;
   print->paperTopicsSeparator =  
              XmCreateSeparatorGadget (printForm, "separator", args, n);
   XtManageChild (print->paperTopicsSeparator);

   /* if currently showing a topic, display the topic selection box */
   /****** Create the topics selection frame ******/
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, print->paperTopicsSeparator); n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 5);				n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginWidth, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 2);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;/*EXP*/
   print->topicsFrame = 
                          XmCreateFrame(printForm, "topicsFrame", args, n);
   XtManageChild (print->topicsFrame);

   /* FIX: consider eliminating the form and just putting the RowColumn
           RadioBox inside the frame */

   /* unused message catalog entries: 8: Executive, 9: Legal */

   /* put form inside frame */
   topicsForm = XmCreateForm (
                   print->topicsFrame,"topicsForm", NULL, 0);
   XtManageChild (topicsForm);

   /* create the frame title */
   labelString = XmStringCreateLocalized ((_DTGETMESSAGE(
			PRSET,30,"Topics To Print")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   XtSetArg (args[n], XmNchildType, XmFRAME_TITLE_CHILD);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   frameTitle = XmCreateLabelGadget(
                   print->topicsFrame,"topicsFrameTitle",args,n);
   XtManageChild (frameTitle);
   XmStringFree (labelString);

   /* Create the Radio Box widget to hold the toggle buttons */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	      n++;
   radioBox = XmCreateRadioBox(topicsForm, "radioBox", args, n);
   XtManageChild (radioBox);
  
   /* Create the print current topic button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                     (PRSET, 31,"Current Topic")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   print->curTopicBtn = 
           XmCreateToggleButtonGadget(radioBox,"printCurTopicBtn",args,n);
   XtManageChild (print->curTopicBtn);
   XmStringFree (labelString);

   /* Create the print current chapter button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                     (PRSET, 32,"Current and Subtopics")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);		      n++;
   print->subTopicsBtn = 
           XmCreateToggleButtonGadget(radioBox,"print.subTopicsBtn",args,n);
   XtManageChild (print->subTopicsBtn);
   XmStringFree (labelString);

   /* Create the print TOC buttion */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                     (PRSET, 33,"Table of Contents and Index")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);	              n++;
   print->tocBtn = XmCreateToggleButtonGadget 
                       (radioBox, "printTocBtn", args, n);
   XtManageChild (print->tocBtn);
   XmStringFree (labelString);

   /* Create the print all topics buttion */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                     (PRSET, 34,"Entire Volume")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);	              n++;
   print->allTopicsBtn = XmCreateToggleButtonGadget 
                       (radioBox, "printAllTopicsBtn", args, n);
   XtManageChild (print->allTopicsBtn);
   XmStringFree (labelString);

   /* create a separator */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopWidget, print->topicsFrame); n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   print->topicsBtnsSeparator = 
                    XmCreateSeparatorGadget (printForm, "separator", args, n);
   XtManageChild (print->topicsBtnsSeparator);

   /* for easy ref */
   separator = print->topicsBtnsSeparator;

   /*  Create the action buttons along the bottom */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (PRSET, 40,"Print")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 3);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 32);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
#if 0
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
#endif
   XtSetArg (args[n], XmNbottomOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   okBtn = XmCreatePushButtonGadget (printForm, "okBtn", args, n);
   XtAddCallback(okBtn, XmNactivateCallback, StartPrintingCB,
  	     (XtPointer) printCBRec);
   XtManageChild (okBtn);
   XmStringFree (labelString);

   /* Build the Cancel Button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (PRSET, 41,"Cancel")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 35);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 64);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   cancelBtn = XmCreatePushButtonGadget (printForm, "cancelBtn", args, n);
   XtAddCallback(cancelBtn, XmNactivateCallback, ClosePrintCB,
  	     (XtPointer) printCBRec);
   XtManageChild (cancelBtn);
   XmStringFree (labelString);

   /* Build the Help button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (PRSET, 42,"Help")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 68);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 97);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   helpBtn = XmCreatePushButtonGadget (printForm, "helpBtn", args, n);
   XtManageChild (helpBtn);
   pHelpInfo = _DtHelpListAdd(DtHELP_printHelpBtn_STR,
                        widget, help, &help->pHelpListHead);
   XtAddCallback(helpBtn, XmNactivateCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);
   XmStringFree (labelString);

   /*** now do some setup ***/
   /* make OK the default btn btn */
   /* make Cancel the cancel (KCancel) btn */
   /* set focus on the printer field */
   n = 0;
   XtSetArg (args[n], XmNdefaultButton, okBtn);				n++;
   XtSetArg (args[n], XmNcancelButton, cancelBtn);			n++;
   XtSetArg (args[n], XmNinitialFocus, print->printerField);		n++;
   XtSetValues (printForm,args,n);

   /*  Adjust the decorations for the dialog shell of the dialog  */
   n = 0;
   XtSetArg(args[n], XmNmwmFunctions,  MWM_FUNC_MOVE);                  n++;
   XtSetArg (args[n], XmNmwmDecorations, 
             MWM_DECOR_BORDER | MWM_DECOR_TITLE);			n++;
   XtSetValues (printShell, args, n);

   /* Add the popup position callback to our print  dialog */
   XtAddCallback (printShell, XmNpopupCallback, (XtCallbackProc) _DtHelpMapCB,
                  (XtPointer) XtParent(widget));

   /* Add the proper help callback to the print dialog shell "F1" support */
   pHelpInfo = _DtHelpListAdd(DtHELP_printShell_STR,
                        widget, help, &help->pHelpListHead);
   XtAddCallback(printForm, XmNhelpCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);

   /* Assign our new print dialog to our widget instance */
   print->printForm = printForm;

   /* Set the tab navigation order */
   XtSetArg (args[0], XmNnavigationType, XmSTICKY_TAB_GROUP);
   XtSetValues (print->printerField,args,1);
   XtSetValues (print->copiesField,args,1);
   XtSetValues (print->paperSizeOptMenu,args,1);
   XtSetValues (print->topicsFrame,args,1);
   XtSetValues (okBtn,args,1);
   XtSetValues (cancelBtn,args,1);
   XtSetValues (helpBtn,args,1);
}




/*****************************************************************************
 * Function:	    void _DtHelpPrintJob(
 *                   
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Sets up and forks off a print job to helpprint
 *
 *****************************************************************************/
void _DtHelpPrintJob(
    Widget	widget,
    char *	printExec,
    char *	printer,
    int		paperSize,
    char *	copies,
    char *	helpVolume,
    int 	helpType,
    char *	helpData,
    Boolean	printAll,
    Boolean	printSub,
    Boolean	printTocIndex,
    char *	topicTitle)
{
  char * argv[20];
  char * quotes="''";
  int	 i;
  int	 pid;
  char   tmpHelpType[4];

   /* Setup the helpprint command and let it go */
   i = 0;
   argv[i++] = printExec;

   if (printer != NULL && printer[0] != EOS)
   {
        argv[i++] = "-printer";
        argv[i++] = printer;
   }
 
   if (copies != NULL && copies[0] != EOS)
   {
        argv[i++] = "-copies";
        argv[i++] = copies;
   }
 
   if (NULL != topicTitle)
   {
      argv[i++] = "-topicTitle";
      argv[i++] = topicTitle;
   }

   if (paperSize >= _DtHelpPaperSizeNamesCnt) paperSize = 0;
   argv[i++] = "-paperSize";
   argv[i++] = _DtHelpPaperSizeNames[paperSize];
 
   argv[i++] = "-display";
   argv[i++] = (char*)XDisplayString(XtDisplay(widget));
 
   argv[i++] = "-helpType";
   sprintf(tmpHelpType, "%d", helpType);
   argv[i++] = tmpHelpType;
 
   if (printAll)
      argv[i++] = "-allTopics";
  
   if (printSub)
      argv[i++] = "-subTopics";
  
   if (printTocIndex)
   {
      argv[i++] = "-toc";
      argv[i++] = "-index";
      argv[i++] = "-frontMatter";
   }
  
   if (!printSub && !printAll && !printTocIndex)
      argv[i++] = "-oneTopic";

   if (NULL == helpData) helpData = quotes;
   switch (helpType)
   {
     case DtHELP_TYPE_TOPIC:
        argv[i++] = "-locationId";
        argv[i++] = helpData;
        argv[i++] = "-helpVolume";
        argv[i++] = helpVolume;
        break;
 
     case DtHELP_TYPE_MAN_PAGE:
        argv[i++] = "-manPage";
        argv[i++] = helpData;
        break;
 
     case DtHELP_TYPE_FILE:
         argv[i++] = "-helpFile";
         argv[i++] = helpData;
         break;
 
      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
         argv[i++] = "-stringData";
         argv[i++] = helpData;
         break;
    
      default:  
         /* ERROR-MESSAGE */
         /* We should never get here, but just in case... */
         XmeWarning(widget, (char*)PrintMessage001);
         break;
   }  /* End Switch Statement */
 
   argv[i++] = NULL;
 	
#if 0	/* DBG */
   { /* for debugging, print out the command line */
     char * * tmpargv;
     for ( tmpargv = argv; *tmpargv; tmpargv++ ) fprintf(stderr,"%s ", *tmpargv);
     fprintf(stderr,"\n");
   }
#endif

#ifdef __hpux
   pid = vfork();
#else
   pid = fork();
#endif /* __hpux */
 
   if (pid == 0)
   {
      (void) execvp (argv[0], argv);
      _exit (1);
   }
 
  /* Return an error if bad pid? */
}


