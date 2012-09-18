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
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Font.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle Font dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $XConsortium: Font.c /main/7 1996/10/30 11:14:15 drk $ */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/VendorSEP.h>

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>

#include <Dt/Message.h>
#include <Dt/SessionM.h>
#include <Dt/HourGlass.h>

#include <string.h>
#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/

#define FONT_MSG   ((char *)GETMESSAGE(5, 23, "Style Manager - Font"))
#define PREVIEW    ((char *)GETMESSAGE(5, 17, "Preview"))
#define IMMEDIATE  ((char *)GETMESSAGE(5, 18, "The changes to fonts will show up in some\napplications the next time they are started.\nOther applications, such as file manager and\napplication manager, will not show the font\nchanges until you Exit the desktop and then log\nback in.")) 
#define LATER      ((char *)GETMESSAGE(5, 19, "The selected font will be used when\n you restart this session."))
#define INFO_MSG   ((char *)GETMESSAGE(5, 24, "The font that is currently used for your desktop is not\navailable in the Size list. If a new font is selected and\napplied, you will not be able to return to the current font\nusing the Style Manager - Font dialog."))
#define SYSTEM_MSG ((char *)GETMESSAGE(5, 20, "AaBbCcDdEeFfGg0123456789"))
#define USER_MSG   ((char *)GETMESSAGE(5, 21, "AaBbCcDdEeFfGg0123456789"))
#define BLANK_MSG  "                          "
#define SIZE       ((char *)GETMESSAGE(5, 22, "Size"))


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget fontWkarea;
    Widget fontpictLabel;
    Widget previewTB;
    Widget previewForm;
    Widget systemLabel;
    Widget userText;
    Widget sizeTB;
    Widget sizeList;
    int    originalFontIndex;
    int    selectedFontIndex;
    String selectedFontStr;
    Boolean userTextChanged;
} FontData;
static FontData font;

static saveRestore save = {FALSE, 0, };

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/


static void CreateFontDlg( 
                        Widget parent) ;
static void _DtmapCB_fontBB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ButtonCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void changeSampleFontCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void valueChangedCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_fontBB                          */
/*+++++++++++++++++++++++++++++++++++++++*/
void 
popup_fontBB(
        Widget shell )
{
  if (style.fontDialog == NULL) {
    _DtTurnOnHourGlass(shell);  
    CreateFontDlg(shell); 
    XtManageChild(style.fontDialog);
    _DtTurnOffHourGlass(shell);  
  } else { 
    XtManageChild(style.fontDialog);
    raiseWindow(XtWindow(XtParent(style.fontDialog)));
  }

  /* If no font is found to match current Desktop
     font, pop up informative message */
  if (font.selectedFontIndex < 0)
     InfoDialog(INFO_MSG, style.shell, False);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* CreateFontDlg                         */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
CreateFontDlg(
        Widget parent )
{

    register int     n;
    int              i;
    Arg              args[MAX_ARGS];
    Widget           appTBox;
    Widget           sizeMenuPlDn;
    XmString         button_string[NUM_LABELS];
    XmString         string;
    int              count = 0;
    Widget           widget_list[6];
    XmString         *sizeItems;
    XmStringTable    selectedSize;
    char             sizeStr[111];
    Dimension        fontheight;


    font.selectedFontStr = style.xrdb.systemFontStr;

    /* Assume nothing is selected */
    font.selectedFontIndex = -1;

    /* 
     * The following flag is used to determine if the user has 
     * entered anything into the sample user font field.  If 
     * he does, than when the font selection is changed, the 
     * default message "aAbBcC..." won't be displayed overwriting
     * the user's text, only the fontlist will be changed. 
     * This flag will be set in the valueChanged callback for the
     * font.sizeList widget.
     */
    font.userTextChanged = FALSE;

    /* 
     * Look for the selectedFont in the fontChoice array and set 
     * selectedFontIndex to that entry
     */
    for (i=0; i<style.xrdb.numFonts; i++)
        if (strcmp (font.selectedFontStr, 
                    style.xrdb.fontChoice[i].sysStr) == 0)
        {
            font.selectedFontIndex = i;        
            if (!style.xrdb.fontChoice[i].userFont)
                GetUserFontResource(i);
            if (!style.xrdb.fontChoice[i].sysFont)
                GetSysFontResource(i);
            break;    
        }            

    /* 
     * Save the index of the originally selected font.  If no
     * font is selected, this value will remain -1.
     */
    font.originalFontIndex = font.selectedFontIndex;

    /* Set up button labels. */
    button_string[0] = CMPSTR(_DtOkString);
    button_string[1] = CMPSTR(_DtCancelString);
    button_string[2] = CMPSTR(_DtHelpString);

    /* Create toplevel DialogBox */
    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.  
     * save.posArgs may contain information from restoreFont().
     */

    XtSetArg(save.posArgs[save.poscnt], XmNallowOverlap, False); save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNdefaultPosition, False); 
    save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  
    save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string); 
    save.poscnt++;
    style.fontDialog = 
        __DtCreateDialogBoxDialog(parent, "Fonts", save.posArgs, save.poscnt);
    XtAddCallback(style.fontDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(style.fontDialog, XmNmapCallback, _DtmapCB_fontBB, 
                            (XtPointer)parent);
    XtAddCallback(style.fontDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_FONT_DIALOG);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widget_list[0] = _DtDialogBoxGetButton(style.fontDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widget_list[0]); n++;
    XtSetValues (style.fontDialog, args, n);

    n=0;
    XtSetArg(args[n], XmNtitle, FONT_MSG); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetValues (XtParent(style.fontDialog), args, n);

    n = 0;
    XtSetArg (args[n], XmNchildType, XmWORK_AREA);  n++;
    XtSetArg(args[n], XmNhorizontalSpacing, style.horizontalSpacing); n++;
    XtSetArg(args[n], XmNverticalSpacing, style.verticalSpacing); n++;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    font.fontWkarea = XmCreateForm(style.fontDialog, "fontWorkArea", args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNtopOffset, style.verticalSpacing);  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;

    XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
    XtSetArg(args[n], XmNbehavior, XmICON_LABEL); n++;
    XtSetArg(args[n], XmNpixmapForeground, style.secBSCol); n++;
    XtSetArg(args[n], XmNpixmapBackground, style.secTSCol); n++;
    XtSetArg(args[n], XmNstring, NULL); n++;  
    XtSetArg(args[n], XmNshadowThickness, 0); n++;  
    XtSetArg(args[n], XmNimageName, FONT_ICON); n++;  
    XtSetArg(args[n], XmNtraversalOn, False); n++;  
    widget_list[count++] = font.fontpictLabel = 
        _DtCreateIcon(font.fontWkarea, "fontpictLabel", args, n);

    /* Create a TitleBox and Scale/List to choose the font size */

    n = 0;
    string = CMPSTR(SIZE);
    XtSetArg(args[n], XmNtitleString, string);  n++;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          font.fontpictLabel);  n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing+5);  n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    widget_list[count++] = font.sizeTB =
        _DtCreateTitleBox(font.fontWkarea, "sizeTB", args, n); 
    XmStringFree(string);

    /* calculate size for each of the fonts based on system font size */

    sizeItems = (XmString *) XtMalloc(sizeof(XmString) * style.xrdb.numFonts);
    for (n=0; n<style.xrdb.numFonts; n++)
      {
	sprintf(sizeStr, "%d", (int)(n+1));
	sizeItems[n] = CMPSTR(sizeStr);
	style.xrdb.fontChoice[n].pointSize = CMPSTR(sizeStr); 
      }
  

    n=0;
    XtSetArg (args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
    XtSetArg (args[n], XmNautomaticSelection, True); n++;
    XtSetArg (args[n], XmNvisibleItemCount, 7); n++;
    XtSetArg (args[n], XmNitemCount, style.xrdb.numFonts); n++;
    XtSetArg (args[n], XmNitems, sizeItems); n++;

    /* 
     * If matching font was found for current selection,
     * display it as selected.  Otherwise, don't select
     * anything.
     */
    if (font.selectedFontIndex >=0) {
      selectedSize = &(style.xrdb.fontChoice[font.selectedFontIndex].pointSize);
      XtSetArg (args[n], XmNselectedItems, selectedSize); n++; 
      XtSetArg (args[n], XmNselectedItemCount, 1); n++;
    }
    font.sizeList = XmCreateScrolledList(font.sizeTB,"sizeList",args,n);
    XtAddCallback(font.sizeList, XmNbrowseSelectionCallback,
                                         changeSampleFontCB, NULL);

    /* If a font match was found and selected, then set it's point size. */
    if (font.selectedFontIndex >=0)
      XmListSetItem(font.sizeList,
        style.xrdb.fontChoice[font.selectedFontIndex].pointSize);
    XtFree((char *)sizeItems);

    /* preview TitleBox */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          font.fontpictLabel);  n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing+5);  n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++; 
    XtSetArg(args[n], XmNleftWidget,         font.sizeTB);         n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    string = CMPSTR(PREVIEW); 
    XtSetArg(args[n], XmNtitleString, string); n++;
    widget_list[count++] = font.previewTB =
        _DtCreateTitleBox(font.fontWkarea, "systemSample", args, n);
    XmStringFree(string);
  
    /*form to contain preview font area*/
    n = 0;
    font.previewForm = 
        XmCreateForm(font.previewTB, "previewForm", args, n);

    /* sample system font */
    n = 0;

    /* 
     * If a font match was found and selected, then set the fontlist
     * and the sample string.  Otherwise, output a blank message.
     */
    if (font.selectedFontIndex >=0) {
      XtSetArg (args[n], XmNfontList, 
        style.xrdb.fontChoice[font.selectedFontIndex].sysFont); n++; 
      string = CMPSTR(SYSTEM_MSG); 
    } else {
      string = CMPSTR(BLANK_MSG);
    }
    XtSetArg (args[n], XmNlabelString, string);  n++;
    XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNtopOffset, 2 * style.verticalSpacing);  n++;    
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE);  n++;
    font.systemLabel = 
        XmCreateLabelGadget(font.previewForm, "systemSample", args, n);
    XmStringFree(string);

    /* sample user font */
    n = 0;
    /* 
     * If a font match was found and selected, then set the fontlist
     * and the sample string.   Otherwise output a blank message.
     */
    if (font.selectedFontIndex >=0) {
      XtSetArg (args[n], XmNfontList, 
	      style.xrdb.fontChoice[font.selectedFontIndex].userFont); n++;
      XtSetArg (args[n], XmNvalue, USER_MSG);  n++;
    } else {
      XtSetArg (args[n], XmNvalue, NULL);  n++;
    }

    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, font.systemLabel);  n++;
    XtSetArg (args[n], XmNtopOffset, 2 * style.verticalSpacing);  n++;    
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, font.systemLabel);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, font.systemLabel);  n++;
    font.userText = 
        XmCreateText(font.previewForm, "userText", args, n);
    /* Add callback to determine if user changes text in sample field */
    XtAddCallback(font.userText, XmNvalueChangedCallback, valueChangedCB, NULL);

    XtManageChild(font.systemLabel);
    XtManageChild(font.userText);
    XtManageChild(font.previewForm);
    XtManageChild(font.sizeList);
    XtManageChildren(widget_list,count);
    XtManageChild(font.fontWkarea);

}


/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtmapCB_fontBB                          */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
_DtmapCB_fontBB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   
    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

    if (!save.restoreFlag)
        putDialog ((Widget)client_data, w);
      
    XmTextShowPosition(font.userText, 0);    

    XtRemoveCallback(style.fontDialog, XmNmapCallback, _DtmapCB_fontBB, NULL);

}

    
/*+++++++++++++++++++++++++++++++++++++++++++++++++*/
/* ButtonCB                                        */
/* Process callback from PushButtons in DialogBox. */
/*+++++++++++++++++++++++++++++++++++++++++++++++++*/
static void 
ButtonCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  DtDialogBoxCallbackStruct *cb     
           = (DtDialogBoxCallbackStruct *) call_data;
  int      n, len, items;
  char	   *str, *fntstr, *fntsetstr;
  Arg      args[MAX_ARGS];
  char     fontres[8192];

  switch (cb->button_position)
    {
        /* Set the xrdb or pass to dtsession and close the window */
      case OK_BUTTON:
         /* 
	  * Need to test for the case where the Font dialog maps without 
	  * any fonts selected, and the user presses OK.  Do this by 
	  * checking for number of items selected before changing 
	  * anything.
	  */
         XtVaGetValues (font.sizeList, XmNselectedItemCount, &items, NULL);

         /*   Post an info dialog explaining when the new fonts will appear */
         if ((font.selectedFontIndex != font.originalFontIndex) && (items > 0))
         {
            XtUnmanageChild(style.fontDialog);  

            if(style.xrdb.writeXrdbImmediate) 
            { 
              InfoDialog(IMMEDIATE, style.shell, False); 
            }
            else 
            {
              InfoDialog(LATER, style.shell, False); 
            }

	    /* 
	       for *FontSet resource: find first font entry delimited by a ":" 
	       or an "=". 
	    */ 
            len =strcspn(style.xrdb.fontChoice[font.selectedFontIndex].userStr,
			 ":=");
            fntsetstr = (char *) XtCalloc(1, len + 1);
            memcpy(fntsetstr, 
		   style.xrdb.fontChoice[font.selectedFontIndex].userStr,
		   len);

            /* 
	       Since the *Font and *FontSet resources may be used by old
	       X applications, a fontlist of multiple fonts must be converted 
               to Xt font set format (';'s converted to ','s since many old X 
               apps don't understand ';' syntax.)
	    */
            str = strstr(fntsetstr,";");
            while (str) {
   		*str = ',';
 		str = strstr(str,";");
	    }

	    /* 
	       for *Font resource: find first font entry delimited by a comma, 
               a colon or an = 
	    */
            len = strcspn(fntsetstr,",:=");
            fntstr = (char *) XtCalloc(1, len + 1);
            memcpy(fntstr, 
		   style.xrdb.fontChoice[font.selectedFontIndex].userStr,
		   len);

	    /*
	      for *FontSet resource: if we got a font (instead of a font set)
	      from the first entry, then wildcard its charset fields
	     */
	    len = strlen(fntsetstr);
	    if (style.xrdb.fontChoice[font.selectedFontIndex].userStr[len] 
		!= ':') {
		str = strchr(fntsetstr, '-');
		for (n = 1; n < 13 && str; n++)
		    str = strchr(str + 1, '-');
		if (str)
		    strcpy(str + 1, "*-*");
	    }

           /* create the font resource specs with the selected font for xrdb */
            sprintf(fontres,
		 "*systemFont: %s\n*userFont: %s\n*FontList: %s\n*buttonFontList: %s\n*labelFontList: %s\n*textFontList: %s\n*XmText*FontList: %s\n*XmTextField*FontList: %s\n*DtEditor*textFontList: %s\n*Font: %s\n*FontSet: %s\n",
                 style.xrdb.fontChoice[font.selectedFontIndex].sysStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].userStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].sysStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].sysStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].sysStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].userStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].userStr,
                 style.xrdb.fontChoice[font.selectedFontIndex].userStr,
	         style.xrdb.fontChoice[font.selectedFontIndex].userStr,
                 fntstr, fntsetstr);

	    XtFree(fntstr);
	    XtFree(fntsetstr);

            /* if writeXrdbImmediate true write to Xrdb else send to session mgr */
    	    if(style.xrdb.writeXrdbImmediate)
	        _DtAddToResource(style.display,fontres);

            SmNewFontSettings(fontres);

            font.originalFontIndex = font.selectedFontIndex;
            style.xrdb.systemFontStr = font.selectedFontStr;
         }

         else 
           	 XtUnmanageChild(style.fontDialog);  
                 
         break;

    case CANCEL_BUTTON:

      /* reset preview area fonts to original and close the window*/

      XtUnmanageChild(style.fontDialog);

      if (font.originalFontIndex >= 0)
        XmListSelectPos(font.sizeList, font.originalFontIndex+1, True);
      else { 
	/* 
	 * if no font was originally selected, need to undo any results
	 * from selections that were made by user before pressing Cancel.
	 */
	XtVaSetValues (font.sizeList, XmNselectedItemCount, 0, NULL);
        XtVaSetValues (font.userText, 
		       XmNvalue, BLANK_MSG, 
	    	       XmNfontList, style.xrdb.userFont,
		       NULL);
        XtVaSetValues (font.systemLabel, 
		       XmNlabelString, CMPSTR(BLANK_MSG), 
	    	       XmNfontList, style.xrdb.systemFont,
		       NULL);
	font.userTextChanged = FALSE;
	font.selectedFontIndex = -1;
      }
      break;

    case HELP_BUTTON:
      XtCallCallbacks(style.fontDialog, XmNhelpCallback, (XtPointer)NULL);
      break;

    default:
      break;
    }
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* changSampleFontCB                     */
/*  Change the font in the sample areas  */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
changeSampleFontCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int       n;
    int       pos;
    int       hourGlassOn;
    Arg       args[MAX_ARGS];
    XmListCallbackStruct *cb = (XmListCallbackStruct *) call_data;

    pos = cb->item_position-1;

    font.selectedFontIndex = pos;
    font.selectedFontStr = style.xrdb.fontChoice[pos].sysStr;

    hourGlassOn = !style.xrdb.fontChoice[pos].userFont ||
                !style.xrdb.fontChoice[pos].sysFont;

    if (hourGlassOn)
      _DtTurnOnHourGlass(style.fontDialog);

    if (!style.xrdb.fontChoice[pos].userFont)
      GetUserFontResource(pos);
    if (!style.xrdb.fontChoice[pos].sysFont)
      GetSysFontResource(pos);

    if (hourGlassOn)
      _DtTurnOffHourGlass(style.fontDialog);

    /* Set the sample System Font string to different Font */
    n = 0;
    XtSetArg(args[n], XmNfontList, style.xrdb.fontChoice[pos].sysFont); n++;
    /* string_val = CMPSTR(SYSTEM_MSG);*/
    XtSetArg (args[n], XmNlabelString, CMPSTR(SYSTEM_MSG));  n++;
    XtSetValues (font.systemLabel, args, n); 

    /* 
     * If the user didn't change the text field, output standard user 
     * text message.
     */
    n = 0;
    if (!font.userTextChanged) 
      XtSetArg (args[n], XmNvalue, USER_MSG);  n++;
    XtSetArg(args[n], XmNfontList, style.xrdb.fontChoice[pos].userFont); n++;
    XtSetValues (font.userText, args, n);
    XmTextShowPosition(font.userText, 0);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* valueChangedCB                        */
/*  Set flag indicating that the user    */
/*  text field has been modified.        */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
valueChangedCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )

{ 
  font.userTextChanged = TRUE; 
}

/************************************************************************
 * restoreFonts()
 *
 * restore any state information saved with saveFonts.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreFonts(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("Fonts");
    xrm_name [2] = 0;

    /* get x position */
    xrm_name [1] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr)); 
      save.poscnt++;
      save.restoreFlag = True;
    }

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr)); 
      save.poscnt++;
    }

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0) 
      popup_fontBB(shell);
}

/************************************************************************
 * saveFonts()
 *
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with the
 * file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreFonts.  The suggested minimum is whether you are mapped, and your
 * location.
 ************************************************************************/
void 
saveFonts(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.fontDialog != NULL) {
        if (XtIsManaged(style.fontDialog))
          sprintf(bufr, "*Fonts.ismapped: True\n");
        else
          sprintf(bufr, "*Fonts.ismapped: False\n");

        /* Get and write out the geometry info for our Window */

        x = XtX(XtParent(style.fontDialog));
        y = XtY(XtParent(style.fontDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

        sprintf(bufr, "%s*Fonts.x: %d\n", bufr, x);
        sprintf(bufr, "%s*Fonts.y: %d\n", bufr, y);

        write (fd, bufr, strlen(bufr));
    }
}



