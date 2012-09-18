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
 **   File:        Dtwm.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle Window Manager customization dialog
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990, 1993.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $XConsortium: Dtwm.c /main/4 1995/10/30 13:09:13 rswiston $ */
/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/VendorSEP.h>
#include <Xm/MessageB.h>

#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>

#include <Dt/Message.h>
#include <Dt/HourGlass.h>
#include <Dt/Wsm.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"
#include "Protocol.h"

#include <ctype.h>

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Dtwm.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define POINTER 0    
#define EXPLICIT 1    

#define POINTER_STR         "pointer"
#define EXPLICIT_STR        "explicit"

#define NUM_PLACEMENT_ICONS 8
#define TOP_LEFT            0
#define TOP_RIGHT           1
#define BOTTOM_LEFT         2
#define BOTTOM_RIGHT        3
#define LEFT_TOP            4
#define LEFT_BOTTOM         5
#define RIGHT_TOP           6
#define RIGHT_BOTTOM        7
#define WARN_MSG  ((char *)GETMESSAGE(18, 12, "The workspace manager must be restarted in\n\
order for your changes to take effect.\n\
Press OK to restart the workspace manager\n\
or Cancel to reset the changes."))

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static Widget build_dtwmDlg ( Widget shell ) ;
static void formLayoutCB ( Widget, XtPointer, XtPointer ) ;
static void _DtmapCB_dtwmDlg ( Widget, XtPointer, XtPointer ) ;
static void systemDefaultCB ( Widget, XtPointer, XtPointer ) ;
static void ButtonCB ( Widget, XtPointer, XtPointer ) ;
static void getDtwmValues ( ) ;
static void _DtWmParseToLower (unsigned char  *string) ;
static void okWarnCB ( Widget, XtPointer, XtPointer ) ;
static void cancelWarnCB ( Widget, XtPointer, XtPointer ) ;

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/

typedef struct {
    Widget      pictLabel;
    Widget      systemDefault;
    Widget      windowFocusTB;
    Widget      focusPolicyRC;
    Widget      focusSeparator;
    Widget      autoRaiseTG;
    Widget      secStackTG;
    Widget      moveOpaqueTG;
    Widget      iconPlacementTB;
    Widget      useIconBoxRC;
    Widget      pointerTG;
    Widget      explicitTG;
    Widget      iconBoxTG;
    Widget	desktopTG;
    Boolean     systemDefaultFlag;
    int         origKeyboardFocusPolicy;
    int         origFocusAutoRaise;
    int         origSecStack;
    int         origMoveOpaque;
    int         origUseIconBox;
    Widget      warnDialog;
} Dtwm, *DtwmPtr;

static Dtwm dtwm;
static saveRestore save = {FALSE, 0, };

static char dtwmRes[150]="";

static char *icon[] = {
    "iconTL",
    "iconTR",
    "iconBL",
    "iconBR",
    "iconLT",
    "iconLB",
    "iconRT",
    "iconRB"
};

static char *placementStr[] = {
    "top left",
    "top right",
    "bottom left",
    "bottom right",
    "left top",
    "left bottom",
    "right top",
    "right bottom"
};

/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_dtwmBB                         */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
popup_dtwmBB(
        Widget shell )
{
    if (style.dtwmDialog == NULL) {
      _DtTurnOnHourGlass(shell);  
      build_dtwmDlg(shell);
      getDtwmValues();
      XtManageChild(style.dtwmDialog);
      _DtTurnOffHourGlass(shell);  
    }
    else 
    {
        XtManageChild(style.dtwmDialog);
        raiseWindow(XtWindow(XtParent(style.dtwmDialog)));
    }
    dtwm.systemDefaultFlag = False;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* getDtwmValues                        */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
getDtwmValues( )
{
    char *str_type_return;
    XrmValue value_return;
    XrmValue    cvt_value;
    XrmDatabase db;
    Boolean status;
    char *string;

    db = XtDatabase(style.display);

    /* Get KeyboardFocusPolicy value */
    if (status = XrmGetResource (db, "dtwm.keyboardFocusPolicy",
                                 "Dtwm.KeyboardFocusPolicy",
                                 &str_type_return, &value_return))
    {
        /* make local copy of string */
        string = (char *) XtMalloc( value_return.size );
        strcpy (string, value_return.addr);

        /* convert to lower case */
        _DtWmParseToLower((unsigned char *)string);

        if (strcmp(string, "pointer") == 0)
        {
            XmToggleButtonGadgetSetState (dtwm.pointerTG, True, True); 
            dtwm.origKeyboardFocusPolicy = POINTER;
        }
        if (strcmp(string, "explicit") == 0)
        {
            XmToggleButtonGadgetSetState (dtwm.explicitTG, True, True); 
            dtwm.origKeyboardFocusPolicy = EXPLICIT;
        }
        XtFree (string);
    }
    else /* KeyboardFocusPolicy not specified */
    {
        /* set the Dtwm default value - explicit */
        XmToggleButtonGadgetSetState (dtwm.explicitTG, True, True); 
        dtwm.origKeyboardFocusPolicy = EXPLICIT;
    } 

    /* Get FocusAutoRaise value */
    if (status = XrmGetResource (db, "dtwm.focusAutoRaise",
                                     "Dtwm.FocusAutoRaise",
                                     &str_type_return, &value_return))
    {
        /* make local copy of string */
        string = (char *) XtMalloc( value_return.size );
        strcpy (string, value_return.addr);

        /* convert to lower case */
        _DtWmParseToLower((unsigned char *)string);

        dtwm.origFocusAutoRaise =                
               (strcmp(string, "true") ? False : True); 
        XmToggleButtonGadgetSetState (dtwm.autoRaiseTG, 
               dtwm.origFocusAutoRaise, True);

        XtFree (string);
    }
    else /* FocusAutoRaise not specified */
    {
        /* set the Dtwm default value: 
         *  True when keyboardFocusPolicy is explicit 
         *  False when keyboardFocusPolicy is pointer
         */
        dtwm.origFocusAutoRaise =   
                ((dtwm.origKeyboardFocusPolicy == EXPLICIT) ? True : False); 
        XmToggleButtonGadgetSetState (dtwm.autoRaiseTG, 
                dtwm.origFocusAutoRaise, True);
    }

    /* Get SecStack value from secondaries OnTop resource*/
    if (status = XrmGetResource (db, "dtwm.secondariesOnTop",
                                     "Dtwm.secondariesOnTop",
                                     &str_type_return, &value_return))
    {
        /* make local copy of string */
        string = (char *) XtMalloc( value_return.size );
        strcpy (string, value_return.addr);

        /* convert to lower case */
        _DtWmParseToLower((unsigned char *)string);

        dtwm.origSecStack =                
               (strcmp(string, "true") ? False : True); 
        XmToggleButtonGadgetSetState (dtwm.secStackTG, 
               !dtwm.origSecStack, True);

        XtFree (string);
    }
    else /* SecStack resource not specified */
      {
#ifdef sun
	dtwm.origSecStack = False;
	XmToggleButtonGadgetSetState (dtwm.secStackTG, 
				      !dtwm.origSecStack, False);
#else
	dtwm.origSecStack = True;
	XmToggleButtonGadgetSetState (dtwm.secStackTG, 
				      !dtwm.origSecStack, True);
#endif
      }
    /* Get UseIconBox value */
    if (status = XrmGetResource (db, "dtwm.useIconBox",
                                 "Dtwm.UseIconBox",
                                 &str_type_return, &value_return))
    {
        /* make local copy of string */
        string = (char *) XtMalloc( value_return.size );
        strcpy (string, value_return.addr);

        /* convert to lower case */
        _DtWmParseToLower((unsigned char *)string);

        dtwm.origUseIconBox =                
              (strcmp(string, "true") ? False : True); 
        if (dtwm.origUseIconBox)
            XmToggleButtonGadgetSetState (dtwm.iconBoxTG, True, True); 
        else
            XmToggleButtonGadgetSetState (dtwm.desktopTG, True, True); 

        XtFree (string);
    }                                 
    else /* UseIconBox not specified */
    {
        /* set the Dtwm default value - False, desktop - True*/
        dtwm.origUseIconBox =  False;
        XmToggleButtonGadgetSetState (dtwm.desktopTG, True, True); 
    }

    /* Get MoveOpaque value */
    if (status = XrmGetResource (db, "dtwm.moveOpaque",
                                     "Dtwm.MoveOpaque",
                                     &str_type_return, &value_return))
    {
        /* make local copy of string */
        string = (char *) XtMalloc( value_return.size );
        strcpy (string, value_return.addr);

        /* convert to lower case */
        _DtWmParseToLower((unsigned char *)string);

        dtwm.origMoveOpaque =               
               (strcmp(string, "true") ? False : True); 
        XmToggleButtonGadgetSetState (dtwm.moveOpaqueTG, 
               dtwm.origMoveOpaque, True);

        XtFree (string);
    }
    else /* MoveOpaque not specified */
    {
        /* set the Dtwm default value: False */

        dtwm.origMoveOpaque =  False; 
        XmToggleButtonGadgetSetState (dtwm.moveOpaqueTG, 
                dtwm.origMoveOpaque, True);
    }

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* build__dtwmDlg                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static Widget 
build_dtwmDlg(
        Widget shell )
{
    register int     i, n;
    Arg              args[MAX_ARGS];
    XmString         button_string[NUM_LABELS]; 
    XmString         string; 
    Widget           form;
    Widget           windowFocusForm;
    Widget           iconPlacementForm;
    int              count = 0;
    Widget           widgetList1[6];

    /* get dtwm resource values  */

    /* Set up DialogBoxDialog button labels */
    button_string[0] = CMPSTR(_DtOkString);
    button_string[1] = CMPSTR(_DtCancelString);
    button_string[2] = CMPSTR(_DtHelpString);

    /* Create toplevel DialogBox */
    /* saveRestore
     * Note that save.poscnt has been initialized elsewhere.  
     * save.posArgs may contain information from restoreBeep().*/

    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);  save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNdefaultPosition, False);
    save.poscnt++;
    style.dtwmDialog = 
        __DtCreateDialogBoxDialog(shell, "dtwmDialog", save.posArgs, save.poscnt);
    XtAddCallback(style.dtwmDialog, XmNhelpCallback,
            (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_DTWM_DIALOG);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widgetList1[0] = _DtDialogBoxGetButton(style.dtwmDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widgetList1[0]); n++;
    XtSetValues (style.dtwmDialog, args, n);

    n=0;
    XtSetArg(args[n], XmNtitle, ((char *)GETMESSAGE(18, 1, "Style Manager - Window"))); n++;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;
    XtSetValues (XtParent(style.dtwmDialog), args, n);

    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    form = XmCreateForm(style.dtwmDialog, "dtwmForm", args, n);

    n = 0;
    XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
    XtSetArg(args[n], XmNbehavior, XmICON_LABEL); n++;
    XtSetArg(args[n], XmNpixmapForeground, style.secBSCol); n++;
    XtSetArg(args[n], XmNpixmapBackground, style.secTSCol); n++;
    XtSetArg(args[n], XmNstring, NULL); n++;  
    XtSetArg(args[n], XmNshadowThickness, 0); n++;  
    XtSetArg(args[n], XmNimageName, DTWM_ICON); n++;  
    XtSetArg(args[n], XmNtraversalOn, False); n++;  
    widgetList1[count++] = 
    dtwm.pictLabel = _DtCreateIcon(form, "dtwmpictLabel", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
    string = CMPSTR(((char *)GETMESSAGE(18, 2, "Default")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    widgetList1[count++] = dtwm.systemDefault = 
        XmCreatePushButtonGadget(form, "systemDefault", args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 3, "Window Behavior"));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widgetList1[count++] = dtwm.windowFocusTB
        = _DtCreateTitleBox(form, "windowFocusTB", args, n);
    XmStringFree(string);
    
    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    windowFocusForm = 
        XmCreateForm(dtwm.windowFocusTB, "windowFocusForm", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 4, "Window Icons"));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widgetList1[count++] = dtwm.iconPlacementTB
        = _DtCreateTitleBox(form, "iconPlacementTB", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    iconPlacementForm = 
        XmCreateForm(dtwm.iconPlacementTB, "iconPlacementForm", args, n);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    dtwm.focusPolicyRC = 
        XmCreateRadioBox(windowFocusForm, "focusPolicyRC", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 13, "Point In Window To Make Active"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    dtwm.pointerTG = 
        XmCreateToggleButtonGadget(dtwm.focusPolicyRC, "pointerTG", args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 14, "Click In Window To Make Active"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    dtwm.explicitTG = 
        XmCreateToggleButtonGadget(dtwm.focusPolicyRC, "explicitTG", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNmargin, 0); n++;
    dtwm.focusSeparator = 
        XmCreateSeparatorGadget(windowFocusForm, "focusSeparator", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 15, "Raise Window When Made Active"));
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNlabelString, string);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    dtwm.autoRaiseTG = 
        XmCreateToggleButtonGadget(windowFocusForm, "autoRaiseTG", args, n);
    XmStringFree(string);


    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 11, "Allow Primary Windows On Top"));
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNlabelString, string);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    dtwm.secStackTG = 
        XmCreateToggleButtonGadget(windowFocusForm, "secStackTG", args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 16, "Show Contents During Move"));
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    XtSetArg(args[n], XmNlabelString, string);  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
    dtwm.moveOpaqueTG = 
        XmCreateToggleButtonGadget(windowFocusForm, "moveOpaqueTG", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 0); n++;
    XtSetArg(args[n], XmNmarginHeight, 0); n++;
    dtwm.useIconBoxRC = 
        XmCreateRadioBox(iconPlacementForm, "useIconBoxRC", args, n);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 9, "Use Icon Box"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    dtwm.iconBoxTG = 
        XmCreateToggleButtonGadget(dtwm.useIconBoxRC, "iconBoxTG", args, n);
    XmStringFree(string);

    n = 0;
    string = CMPSTR((char *)GETMESSAGE(18, 10, "Place On Workspace"));
    XtSetArg(args[n], XmNlabelString, string); n++;
    dtwm.desktopTG = 
        XmCreateToggleButtonGadget(dtwm.useIconBoxRC, "desktopTG", args, n);
    XmStringFree(string);

    XtAddCallback(style.dtwmDialog, XmNmapCallback, formLayoutCB, NULL);
    XtAddCallback(style.dtwmDialog, XmNmapCallback, _DtmapCB_dtwmDlg, shell);
    XtAddCallback(style.dtwmDialog, XmNcallback, ButtonCB, NULL);
    XtAddCallback(dtwm.systemDefault, XmNactivateCallback, systemDefaultCB, NULL);

    XtManageChild(form);
    XtManageChildren(widgetList1,count); 

    XtManageChild(windowFocusForm);
    XtManageChild(iconPlacementForm);

    XtManageChild(dtwm.focusPolicyRC);
    XtManageChild(dtwm.pointerTG);
    XtManageChild(dtwm.explicitTG);

    XtManageChild(dtwm.focusSeparator);
    XtManageChild(dtwm.autoRaiseTG);
    XtManageChild(dtwm.secStackTG);
    XtManageChild(dtwm.moveOpaqueTG);

    XtManageChild(dtwm.useIconBoxRC);
    XtManageChild(dtwm.iconBoxTG);
    XtManageChild(dtwm.desktopTG);

    return(style.dtwmDialog);
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* formLayoutCB                          */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
formLayoutCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int              n;
    Arg              args[MAX_ARGS];

    /* Picture Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (dtwm.pictLabel, args, n);

    /* system Default */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (dtwm.systemDefault, args, n);

    /* Window Focus TitleBox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          dtwm.pictLabel);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (dtwm.windowFocusTB, args, n);

    /* Icon Placement TitleBox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          dtwm.windowFocusTB); n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (dtwm.iconPlacementTB, args, n);

    /* FocusPolicy RC */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         0);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        0);  n++;
    XtSetValues (dtwm.focusPolicyRC, args, n);

    /* FocusSeparator */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          dtwm.focusPolicyRC);   n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         0);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        0);  n++;
    XtSetValues (dtwm.focusSeparator, args, n);

    /* AutoRaise TG */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          dtwm.focusSeparator);  n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         0);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        0);  n++;
    XtSetValues (dtwm.autoRaiseTG, args, n);

    /* secStack TG */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          dtwm.autoRaiseTG);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing-3); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);         n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing); n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         0);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        0);  n++;
    XtSetValues (dtwm.secStackTG, args, n);


    /* MoveOpaque TG */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          dtwm.secStackTG);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing-3); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing); n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNleftOffset,         0);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        0);  n++;
    XtSetValues (dtwm.moveOpaqueTG, args, n);

    /* Use Icon Box RC */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       0);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         0);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        0);  n++;
    XtSetValues (dtwm.useIconBoxRC, args, n);

    XtRemoveCallback(style.dtwmDialog, XmNmapCallback, formLayoutCB, NULL);
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtmapCB_dtwmDlg                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
_DtmapCB_dtwmDlg(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    static int  first_time = 1;
    int         n;
    Arg         args[MAX_ARGS];


    DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));

    if (!save.restoreFlag)
        putDialog ((Widget)client_data, w);
   
    XtRemoveCallback(style.dtwmDialog, XmNmapCallback, _DtmapCB_dtwmDlg, NULL);
  
}


/*+++++++++++++++++++++++++++++++++++++++*/
/* systemDefaultCB                       */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
systemDefaultCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{

    /* KeyboardFocusPolicy:  explicit */
    XmToggleButtonGadgetSetState (dtwm.explicitTG, True, True); 

    /* FocusAutoRaise:  True */
    XmToggleButtonGadgetSetState (dtwm.autoRaiseTG, True, True); 
   
#ifdef sun
    /* Sun default is secStack: False */
    XmToggleButtonGadgetSetState (dtwm.secStackTG, True, True);
#else
    /* secStack:  True */
    XmToggleButtonGadgetSetState (dtwm.secStackTG, False, True); 
#endif

    /* OpaqueMove:  False */
    XmToggleButtonGadgetSetState (dtwm.moveOpaqueTG, False, True); 

    /* PlaceOnDesktop:  True */
    XmToggleButtonGadgetSetState (dtwm.desktopTG, True, True); 

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* ButtonCB                              */
/* callback for PushButtons in DialogBox */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
ButtonCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  int         n;
  Arg         args[MAX_ARGS];
  char        *resPtr;
  Boolean     state;
  Boolean     changeFlag = 0;
  DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;
  
  switch (cb->button_position)
    {
    case OK_BUTTON:
      
      XtUnmanageChild (w);
      
      /* create the Dtwm resource specs for xrdb */
      
      state = XmToggleButtonGadgetGetState (dtwm.explicitTG);
      if ( state != dtwm.origKeyboardFocusPolicy)
	{
	  sprintf(dtwmRes, "Dtwm.keyboardFocusPolicy: %s\n",
		  XmToggleButtonGadgetGetState (dtwm.pointerTG)
		  ? POINTER_STR : EXPLICIT_STR);
	  changeFlag = 1;
	}
      
      /* write out FocusAutoRaise if toggle has changed or if
       * KeyboardFocusPolicy has changed.  FAR default value is 
       * based off of KFP so must set explicitly or visible toggle 
       * will be inacurate */
      state = XmToggleButtonGadgetGetState (dtwm.autoRaiseTG);
      if (state !=  dtwm.origFocusAutoRaise || changeFlag)
	{
	  sprintf(dtwmRes+strlen(dtwmRes), "Dtwm*focusAutoRaise: %s\n",
		  XmToggleButtonGadgetGetState (dtwm.autoRaiseTG)
		  ? "True" : "False");
	  changeFlag = 1;
	}
      
      state = !XmToggleButtonGadgetGetState (dtwm.secStackTG);
      if ( state != dtwm.origSecStack)
	{
	  sprintf(dtwmRes+strlen(dtwmRes), "Dtwm*secondariesOnTop: %s\n",
		  XmToggleButtonGadgetGetState (dtwm.secStackTG)
		  ? "False" : "True");
	  changeFlag = 1;
	}
      
      state = XmToggleButtonGadgetGetState (dtwm.moveOpaqueTG);
      if ( state != dtwm.origMoveOpaque)
	{
	  sprintf(dtwmRes+strlen(dtwmRes), "Dtwm*moveOpaque: %s\n",
		  XmToggleButtonGadgetGetState (dtwm.moveOpaqueTG)
		  ? "True" : "False");
	  changeFlag = 1;
	}
      
      state = XmToggleButtonGadgetGetState (dtwm.iconBoxTG);
      if (state != dtwm.origUseIconBox)
	{
	  sprintf(dtwmRes+strlen(dtwmRes), "Dtwm*useIconBox: %s\n",
		  XmToggleButtonGadgetGetState (dtwm.iconBoxTG)
		  ? "True" : "False");
	  changeFlag = 1;
	}
      
      if (changeFlag)
	{
	  if (dtwm.warnDialog == NULL)
	    {
	      n = 0;
	      XtSetArg(args[n], XmNokLabelString, CMPSTR(_DtOkString)); n++;
	      XtSetArg(args[n], XmNcancelLabelString, CMPSTR(_DtCancelString)); n++;
	      XtSetArg(args[n], XmNborderWidth, 3); n++;
	      XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC ); n++;
	      XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++; 
	      XtSetArg(args[n], XmNdialogTitle, CMPSTR((char *)GETMESSAGE(2,2, "Notice"))); n++;
	      XtSetArg(args[n], XmNmessageString, CMPSTR(WARN_MSG)); n++;
              dtwm.warnDialog = XmCreateInformationDialog(style.shell, "warnDialog", args, n);
	      
	      XtUnmanageChild (XmMessageBoxGetChild(dtwm.warnDialog, XmDIALOG_HELP_BUTTON)); 
	      
	      XtAddCallback(dtwm.warnDialog, XmNokCallback, okWarnCB, 
			    client_data); 
	      XtAddCallback(dtwm.warnDialog, XmNcancelCallback, cancelWarnCB, 
			    client_data); 
	    }
	  XtManageChild(dtwm.warnDialog);  
	   
	}
      break;
      
      
    case CANCEL_BUTTON:
    
      /* reset to dtwm values from when dialog is mapped */ 
      XtUnmanageChild(w);
      
      XmToggleButtonGadgetSetState (dtwm.pointerTG, 
				    dtwm.origKeyboardFocusPolicy ? POINTER : EXPLICIT , True); 
      
      XmToggleButtonGadgetSetState (dtwm.explicitTG, 
				    dtwm.origKeyboardFocusPolicy ? EXPLICIT : POINTER , True); 
      
      XmToggleButtonGadgetSetState (dtwm.autoRaiseTG, 
				    dtwm.origFocusAutoRaise ? True : False , True); 
      
      XmToggleButtonGadgetSetState (dtwm.secStackTG, 
				    dtwm.origSecStack ? False : True , True); 
      
      XmToggleButtonGadgetSetState (dtwm.moveOpaqueTG, 
				    dtwm.origMoveOpaque ? True : False , True); 
      
      XmToggleButtonGadgetSetState (dtwm.iconBoxTG, 
				    dtwm.origUseIconBox ? True : False , True); 
      
      XmToggleButtonGadgetSetState (dtwm.desktopTG, 
				    dtwm.origUseIconBox ? False : True , True); 
      
      break;
    
  case HELP_BUTTON:
    XtCallCallbacks(style.dtwmDialog, XmNhelpCallback, (XtPointer)NULL);
    break;
    
  default:
    break;
  }
}


/************************************************************************
 * restoreDtwm()
 * /u1/shakib/ncode
 * restore any state information saved with savedtwm.
 * This is called from restoreSession with the application
 * shell and the special xrm database retrieved for restore.
 ************************************************************************/
void 
restoreDtwm(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("dtwmDlg");
    xrm_name [2] = 0;

    /* get x position */
    xrm_name [1] = XrmStringToQuark ("x");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNx, atoi((char *)value.addr)); save.poscnt++;
      save.restoreFlag = True;
    }

    /* get y position */
    xrm_name [1] = XrmStringToQuark ("y");
    if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)){
      XtSetArg (save.posArgs[save.poscnt], XmNy, atoi((char *)value.addr)); save.poscnt++;
    }

    xrm_name [1] = XrmStringToQuark ("ismapped");
    XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value);
    /* Are we supposed to be mapped? */
    if (strcmp(value.addr, "True") == 0) 
      popup_dtwmBB(shell);
}

/************************************************************************
 * saveDtwm()
 *
 * This routine will write out to the passed file descriptor any state
 * information this dialog needs.  It is called from saveSessionCB with the
 * file already opened.
 * All information is saved in xrm format.  There is no restriction
 * on what can be saved.  It doesn't have to be defined or be part of any
 * widget or Xt definition.  Just name and save it here and recover it in
 * restoreBackdrop.  The suggested minimum is whether you are mapped, and your
 * location.
 ************************************************************************/
void 
saveDtwm(
        int fd )
{
    Position x,y;
    Dimension width, height;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.dtwmDialog != NULL) 
    {
        if (XtIsManaged(style.dtwmDialog))
            sprintf(bufr, "*dtwmDlg.ismapped: True\n");
        else
            sprintf(bufr, "*dtwmDlg.ismapped: False\n");

	/* Get and write out the geometry info for our Window */
	x = XtX(XtParent(style.dtwmDialog));
	y = XtY(XtParent(style.dtwmDialog));

        /* Modify x & y to take into account window mgr frames
         * This is pretty bogus, but I don't know a better way to do it.
         */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	width = XtWidth(style.dtwmDialog);
	height = XtHeight(style.dtwmDialog);

	sprintf(bufr, "%s*dtwmDlg.x: %d\n", bufr, x);
	sprintf(bufr, "%s*dtwmDlg.y: %d\n", bufr, y);
	sprintf(bufr, "%s*dtwmDlg.width: %d\n", bufr, width);
	sprintf(bufr, "%s*dtwmDlg.height: %d\n", bufr, height);
	write (fd, bufr, strlen(bufr));
    }
}


/*************************************<->*************************************
 *
 *  _DtWmParseToLower (string)
 *
 *
 *  Description:
 *  -----------
 *  Lower all characters in a string.
 *
 *
 *  Inputs:
 *  ------
 *  string = NULL-terminated character string or NULL
 *
 * 
 *  Outputs:
 *  -------
 *  string = NULL-terminated lower case character string or NULL
 *
 *
 *  Comments:
 *  --------
 *  Can handle multi-byte characters
 * 
 *************************************<->***********************************/

static
void _DtWmParseToLower (unsigned char  *string)
{
    unsigned char *pch = string;
#ifdef MULTIBYTE
    int            chlen;

    while ((chlen = mblen ((char *)pch, MB_CUR_MAX)) > 0)
    {
        if ((chlen == 1) && (isupper (*pch)))
	{
	    *pch = tolower(*pch);
	}
	pch += chlen;
    }
#else
    while (*pch != NULL)
    {
        if (isupper (*pch))
	{
	    *pch = tolower(*pch);
	}
	pch++;
    }
#endif

} /* END OF FUNCTION _DtWmParseToLower */




/*+++++++++++++++++++++++++++++++++++++++*/
/* cancelWarnCB - callback for the      */
/* cancel button of the warnDialog      */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
cancelWarnCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XmToggleButtonGadgetSetState (dtwm.pointerTG, 
				dtwm.origKeyboardFocusPolicy ? POINTER : EXPLICIT , True); 
  
  XmToggleButtonGadgetSetState (dtwm.explicitTG, 
            dtwm.origKeyboardFocusPolicy ? EXPLICIT : POINTER , True); 
  
  XmToggleButtonGadgetSetState (dtwm.autoRaiseTG, 
				dtwm.origFocusAutoRaise ? True : False , True); 
  
  XmToggleButtonGadgetSetState (dtwm.secStackTG, 
            dtwm.origSecStack ? False : True , True); 
  
  XmToggleButtonGadgetSetState (dtwm.moveOpaqueTG, 
				dtwm.origMoveOpaque ? True : False , True); 
  
  XmToggleButtonGadgetSetState (dtwm.iconBoxTG, 
            dtwm.origUseIconBox ? True : False , True); 
  
  XmToggleButtonGadgetSetState (dtwm.desktopTG, 
				dtwm.origUseIconBox ? False : True , True); 
}




/*+++++++++++++++++++++++++++++++++++++++*/
/* okWarnCB - callback for the           */
/* OK button of the warnDialog           */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
okWarnCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  dtwm.origKeyboardFocusPolicy = XmToggleButtonGadgetGetState (dtwm.explicitTG);
  dtwm.origFocusAutoRaise = XmToggleButtonGadgetGetState (dtwm.autoRaiseTG);
  dtwm.origSecStack = !XmToggleButtonGadgetGetState (dtwm.secStackTG);
  dtwm.origMoveOpaque = XmToggleButtonGadgetGetState (dtwm.moveOpaqueTG);
  dtwm.origUseIconBox = XmToggleButtonGadgetGetState (dtwm.iconBoxTG);
  

  /* write out resources to xrdb */
  _DtAddToResource(style.display, dtwmRes);
  
  /* force the workspace manager to restart */
  _DtWmRestartNoConfirm(style.display, style.root);
}






