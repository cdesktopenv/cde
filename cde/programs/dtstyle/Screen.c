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
/************************************<+>*************************************/
/****************************************************************************/
/**                                                                        **/
/**   File:        Screen.c                                                **/
/**                                                                        **/
/**   Project:     DT 1.0                                                  **/
/**                                                                        **/
/**   Description: Controls the Dtstyle Screen dialog                      **/
/**                                                                        **/
/**                                                                        **/
/**                                                                        **/ 
/**                                                                        **/
/****************************************************************************/
/************************************<+>*************************************/

/*$TOG: Screen.c /main/9 1997/07/14 17:44:42 samborn $*/

#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h> 
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/VendorSEP.h>
#include <Xm/Protocols.h>

#include <stdio.h>
#include <signal.h>

#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <Dt/Action.h>
#include <Dt/UserMsg.h>
#include <Dt/SaverP.h>
#include <Dt/DialogBox.h>
#include <Dt/Icon.h>
#include <Dt/TitleBox.h>
#include <Dt/Message.h>
#include <Dt/HourGlass.h>

#include "Help.h"
#include "Main.h"
#include "SaveRestore.h"
#include "Protocol.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Screen.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#ifndef MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#define TIMEOUT_MIN         1
#define TIMEOUT_MAX         120
#define CYCLETIME_MIN       0
#define CYCLE_DEFAULT       3   /* 1-120 */
#define LOCK_DEFAULT        30  /* 1-120 */
#define SAVER_DEFAULT       10  /* 0-120 */
#define SAVER_HEIGHT        238
#define SAVER_WIDTH         298

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/

static Widget build_screenDialog( Widget shell) ;

static void formLayoutCB(Widget w,
			 XtPointer client_data,
			 XtPointer call_data) ;

static void no_svr_ext_formLayoutCB(Widget w,
				    XtPointer client_data,
				    XtPointer call_data) ;

static void _DtmapCB_screenDialog(Widget w,
				  XtPointer client_data,
				  XtPointer call_data) ;

static void no_svr_ext_DtmapCB_screenDialog(Widget w,
				  XtPointer client_data,
				  XtPointer call_data) ;

static void timeOutvalueChangedCB(Widget w,
				  XtPointer client_data,
				  XtPointer call_data) ;

static void no_svr_ext_timeOutvalueChangedCB(Widget w,
				  XtPointer client_data,
				  XtPointer call_data) ;

static void saverToggleCB(Widget w,
			  XtPointer client_data,
			  XtPointer call_data) ;

static void no_svr_ext_saverToggleCB(Widget w,
			  XtPointer client_data,
			  XtPointer call_data) ;

static void saversToggleCB(Widget w,
			  XtPointer client_data,
			  XtPointer call_data) ;

static void lockToggleCB(Widget w,
			 XtPointer client_data,
			 XtPointer call_data) ;

static void systemDefaultCB(Widget w,
			    XtPointer client_data,
			    XtPointer call_data) ;

static void no_svr_ext_systemDefaultCB(Widget w,
			    XtPointer client_data,
			    XtPointer call_data) ;

static void ButtonCB(Widget w,
		     XtPointer client_data,
		     XtPointer call_data) ;

static void no_svr_ext_ButtonCB(Widget w,
		     XtPointer client_data,
		     XtPointer call_data) ;

static XmString * MakeListStrings(char **list ) ;

static  void FreeListStrings(XmString *xmlist,
			     int count) ;

static void ListCB(Widget w,
		   XtPointer client_data,
		   XtPointer call_data) ;

static void IconHandler(Widget shell,
			XtPointer client_data,
			XEvent *event,
			Boolean *dispatch) ;

static void UnmapCB(Widget w,
		    XtPointer client_data,
		    XtPointer call_data) ;


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/
typedef struct {
    Widget      pictLabel;
    Widget      systemDefault;
    Widget	saverFrame;
    Widget      saverForm;
    Widget      saverArea;
    Widget      saverAreaFrame;
    Widget      saverLabel;
    Widget      prevLabel;
    Widget	saverOn;
    Widget      saverOff;
    Widget      saverRadioBox;
    Widget      saverList;
    Widget      timeOutLabel;
    Widget 	timeOutScale;
    Widget      saverMinutesLabel;
    Widget      cycleTimeLabel;
    Widget      cycleTimeScale; 
    Widget      cycleMinutesLabel;
    Widget	lockFrame;
    Widget      lockForm;
    Widget      lockLabel;
    Widget	lockOn;
    Widget	lockOff;
    Widget      lockRadioBox;
    Widget      lockingRadioBox;
    Widget      lockOutLabel;
    Widget      lockOutScale;
    Widget      lockMinutesLabel;
    Widget      useSaversOn;        /* used for graying saverList and saverArea in no saver extn mode */
    Widget      useSaversOff;       /* used for graying saverList and saverArea in no saver extn mode */
    int         interval;
    int         blanking;
    int         exposures;
    int         newTimeout;          /* the min value of savertimeout and locktimeout */
    int         origTimeout;         /* the original value of the min of savertimeout and locktimeout */
    int         xTimeout;            /* the value of the timeout extracted from the X server */
    Boolean     timeMismatch;        /* use to keep track of mismatch between xTimeout and newTimeout */
    int         saverTimeout;        /* the value of savertimeout */ 
    int         lockTimeout;         /* the value of locktimeout */ 
    int         cycleTimeout;        /* the value of cycletimeout */ 
    int         saverTimeout_xrdb;   /* the value of savertimeout from xrdb, in case saver is off */ 
    int         lockTimeout_xrdb;    /* the value of locktimeout from xrdb, in case lock is off */ 
    int         new_blanking;
    Boolean     systemDefaultFlag;
        
} ScreenData;                        /* using the savers toggle when the saver extn is not present */

static ScreenData  screen;
static saveRestore save = {FALSE, 0,};

typedef struct {
  char    *saversList;                  /* list of available savers */
  char    *selsaversList;               /* list of selected savers */
  char   **saverActionNames;            /* array of saver names */
  char   **saverActionDesc;             /* array of saver action descriptions */
  char   **selsaverActionNames;         /* array of originally selected saver names */
  int     *selPositions;                /* array of currently selected saver positions */
  int     *origSelPositions;            /* array of originally selected saver positions */
  int      saverCount;                  /* number of available savers */
  int      selsaverCount;               /* number of currently selected screen savers */
  int      orig_selsaverCount;          /* number of originally selected screen savers */
  int      previous_selsaverCount;      /* number of previously selected screen savers */
  char    *current_saver;               /* name of current screen saver actions */
  char    *current_saverDesc;           /* description of current screen saver actions */
  int      current_position;            /* the clicked on position in the scrolled list */
  int      shadow;                      /* shadow width of saver area */
  Pixel    bg;                          /* bg color of the style manager dialogs */
  Pixel    black;                       /* value of the black pixel */
  void    *saverstate;                  /* current running screen saver state */
} Savers, *SaversPtr;
static Savers savers; 




/*++++++++++++++++++++++++++++++++++++++++*/
/* build_saverList                        */
/* parses a space separated list of       */
/* savers to build a an array of saver    */
/* names. Returns the array and the saver */
/* count.                                 */
/*++++++++++++++++++++++++++++++++++++++++*/

char ** 
build_saverList(char * str, 
		 int * count)
{
   char tokenSep[] = " ";
   char * token;
   char ** saverList = NULL;
   register int i = 0;
   char * tmpStr, *tmpStr2;
   int len = strlen(str);
   

   *count = 0;

   tmpStr = (char *)XtCalloc(1, len + 1);
   tmpStr2 = tmpStr;
   strcpy(tmpStr, str);
   token = strtok(tmpStr, tokenSep);
   while(token != NULL)
     {
       ++(i);
       token = strtok(NULL, tokenSep);
     }

   if (i == 0)
       return (NULL);

   saverList = (char **) XtCalloc(1, i * sizeof(char *));
   if( saverList )
     {
       strcpy(tmpStr, str); 
       while (isspace(*tmpStr))
	   tmpStr++;
       token = strtok(tmpStr, tokenSep);
       *count=0;
       while(token != NULL)
	 {
	   if (DtActionExists(token))
	     {
	       saverList[*count] = (char *) XtCalloc(1, strlen( token ) + 1);
	       strcpy(saverList[*count], token);
	       ++(*count);
	     }
	   tmpStr += strlen(token);
	   if (tmpStr >= tmpStr2 + len )
	       token = NULL;
	   else 
	       {
		   do tmpStr++;
		   while (isspace(*tmpStr));
		   token = strtok(tmpStr, tokenSep);
	       }
	 }
     } 
   if (tmpStr2)
       XtFree ((char *) tmpStr2);
   return(saverList);
 }

/*+++++++++++++++++++++++++++++++++++++++*/
/* build_selsaverList                    */
/*+++++++++++++++++++++++++++++++++++++++*/

char ** 
build_selsaverList(char * envStr, 
		 int * count)
{
   char tokenSep[] = " ";
   char * token;
   char ** saverList = NULL;
   register int i = 0;
   char * tmpStr;
   int len = strlen(envStr);
   *count = 0;

   tmpStr = (char *)XtCalloc(1, len + 1);
   strcpy(tmpStr, envStr);
   token = strtok(tmpStr, tokenSep);
   while(token != NULL)
     {
       ++(i);
       token = strtok(NULL, tokenSep);
     }

   if (i == 0)
       return (NULL);

   saverList = (char **) XtCalloc(1, i * sizeof(char *));
   if( saverList )
     {
       strcpy(tmpStr, envStr); 
       token = strtok(tmpStr, tokenSep);
       *count=0;
       while(token != NULL)
	 {
	   saverList[*count] = (char *) XtCalloc(1, strlen( token ) + 1);
	   strcpy(saverList[*count], token);
	   token = strtok(NULL, tokenSep);
	   ++(*count);
	 }
     }

   XtFree ((char *) tmpStr);
   return(saverList);
 }

/*+++++++++++++++++++++++++++++++++++++++*/
/* build_saverDesc                       */
/*+++++++++++++++++++++++++++++++++++++++*/

char ** 
build_saverDesc(char ** names, 
		 int count)
{
  char          **saverDesc = NULL;
  char           *tmpstr;
  int             i;

  saverDesc = (char **) XtCalloc(1, count * sizeof(char *));

  for (i=0; i<count; i++)
    {
      tmpstr = DtActionDescription(savers.saverActionNames[i]); 
      if (tmpstr == NULL)
	{
	  saverDesc[i] = (char *) XtMalloc(strlen(savers.saverActionNames[i]) + 1);
	  strcpy(saverDesc[i], savers.saverActionNames[i]);
	}
      else
	{	  
	  saverDesc[i] = (char *) XtMalloc(strlen(tmpstr) + 1);
	  strcpy(saverDesc[i], tmpstr);
	}
      XtFree((char *) tmpstr);
    }
  return(saverDesc);  
}
  

/*+++++++++++++++++++++++++++++++++++++++*/
/* build_selectedList                    */
/* takes an array of names and a count   */
/* and returns a space separated list    */
/*+++++++++++++++++++++++++++++++++++++++*/

char *
build_selectedList(char ** saverList, 
		    int     count)
{

   char * selectedList = NULL;
   register int i = 0;
   char * tmpStr;
   int len = 0;
   int tmplen = 0;
   
   if (saverList == NULL)
     return(NULL);
   
   for (i=0; i<count; i++)
     {
       len += strlen(saverList[i]) + 1;
     }
   
   selectedList = (char *)XtMalloc(len + 1);
   selectedList[0] = '\0';   
   for (i=0; i<count; i++)
     {
       strcat(selectedList, saverList[i]);
       strcat(selectedList, " ");
     }
 
   return(selectedList);
 }


/*+++++++++++++++++++++++++++++++++++++++*/
/* free_saverList                        */
/*+++++++++++++++++++++++++++++++++++++++*/

void
free_saverList(char ** saverList, 
		    int count)

{
  register int   i;

  if (saverList == NULL)
    return;
  for (i=0; i<count; i++)
    XtFree((char *) saverList[i]);
  
  XtFree ((char *) saverList);

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* popup_screenBB                        */
/*+++++++++++++++++++++++++++++++++++++++*/

void 
popup_screenBB(
        Widget shell )
{
    if (style.screenDialog == NULL) {
      _DtTurnOnHourGlass(shell);  
      build_screenDialog(shell);
      XtManageChild(style.screenDialog);
      _DtTurnOffHourGlass(shell);  
    }
    else 
    {
      XtManageChild(style.screenDialog);
      raiseWindow(XtWindow(XtParent(style.screenDialog)));
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* build_screenDialog                    */
/*+++++++++++++++++++++++++++++++++++++++*/
static Widget 
build_screenDialog(
        Widget shell )
{
    register int     i, m, n;
    Arg              args[MAX_ARGS];
    Widget           form;
    Boolean          sel_matched = False;
    int              sel_found = 1;
    int		     old_selsaverCount = 0;
    Widget           widget_list1[12]; 
    Widget           widget_list2[12]; 
    int              list1_count = 0;
    int              list2_count = 0;
    XmString         button_string[NUM_LABELS]; 
    XmString         string;
    XmString         *listStrings;
    XColor           color;
    Colormap         cmap;              
    Dimension        MaxLabelWidth = 0;
    Dimension        LabelWidth = 0;
    Atom             delete_window_atom;
    Boolean          low_res = False;

    /* initialize saver data */
    savers.shadow = 2;
    savers.saverstate = NULL;   
    savers.saverCount = 0;     
    savers.selsaverCount = 0;
    savers.previous_selsaverCount = 0;
    savers.saversList = NULL;
    savers.selsaverActionNames = NULL;
    savers.selPositions = NULL;
    savers.selsaversList = NULL;
    savers.saverActionNames = NULL;
    savers.saverActionDesc = NULL;
    savers.current_saver = NULL;
    savers.current_saverDesc = NULL;
    savers.current_position = 0;
    savers.black = BlackPixel(style.display, DefaultScreen(XtDisplay(shell)));
 
    /* load the actions data base */
    DtDbLoad();  

    if (_DtGetDisplayResolution(style.display, style.screenNum) == 1)
      low_res = True;
    
    /* Set up DialogBoxDialog button labels */
    button_string[0] = CMPSTR(_DtOkString);
    button_string[1] = CMPSTR(_DtCancelString);
    button_string[2] = CMPSTR(_DtHelpString);

    /* Create toplevel DialogBox */
    /* saveRestore - Note that save.poscnt has been initialized elsewhere. */
    /* save.posArgs may contain information from restoreScreen().*/

    XtSetArg(save.posArgs[save.poscnt], XmNbuttonCount, NUM_LABELS);  
      save.poscnt++;
    XtSetArg(save.posArgs[save.poscnt], XmNbuttonLabelStrings, button_string);  
      save.poscnt++;
    XtSetArg (save.posArgs[save.poscnt], XmNdefaultPosition, False);
      save.poscnt++;
    style.screenDialog = 
      __DtCreateDialogBoxDialog(shell, "ScreenDialog", save.posArgs, save.poscnt);

    XmStringFree(button_string[0]);
    XmStringFree(button_string[1]);
    XmStringFree(button_string[2]);

    widget_list1[0] = _DtDialogBoxGetButton(style.screenDialog,2);
    n=0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNcancelButton, widget_list1[0]); n++;
    XtSetValues (style.screenDialog, args, n);


    /* Set the title */
    n=0;
    XtSetArg(args[n], XmNtitle, ((char *)GETMESSAGE(12, 1, "Style Manager - Screen"))); n++;
    XtSetArg(args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC); n++;  
    XtSetValues (XtParent(style.screenDialog), args, n);

    /* Create the main form */
    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    form=XmCreateForm(style.screenDialog, "Form", args, n);
    
    /* Create the visual */
    n = 0;
    XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
    XtSetArg(args[n], XmNbehavior, XmICON_LABEL); n++;
    XtSetArg(args[n], XmNpixmapForeground, style.secBSCol); n++;
    XtSetArg(args[n], XmNpixmapBackground, style.secTSCol); n++;
    XtSetArg(args[n], XmNstring, NULL); n++;  
    XtSetArg(args[n], XmNshadowThickness, 0); n++;  
    XtSetArg(args[n], XmNimageName, SCREEN_ICON); n++;  
    XtSetArg(args[n], XmNtraversalOn, False); n++;  
    widget_list1[list1_count++] = 
    screen.pictLabel= _DtCreateIcon(form, "screenLabelPixmap", args, n);

    /* Create the default button */
    n = 0;
    XtSetArg(args[n], XmNmarginHeight, LB_MARGIN_HEIGHT);  n++;
    XtSetArg(args[n], XmNmarginWidth, LB_MARGIN_WIDTH);  n++;
    string = CMPSTR(((char *)GETMESSAGE(12, 3, "Default")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNnavigationType, XmTAB_GROUP); n++;  
    widget_list1[list1_count++] = 
    screen.systemDefault= XmCreatePushButtonGadget(form, "systemDefault", args, n);
    XmStringFree(string);

    /* Create the screen lock title box */
    n = 0;
    if (style.smState.smLockOnTimeoutStatus)
      string = CMPSTR(((char *)GETMESSAGE(12, 8, "Screen Lock")));
    else
      string = CMPSTR(((char *)GETMESSAGE(12, 18, "Front Panel Lock")));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widget_list1[list1_count++] = 
      screen.lockFrame= _DtCreateTitleBox(form, "lockFrame", args, n);
    XmStringFree(string);
    

    /* Create a form for the screen lock widgets  */
    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    screen.lockForm= XmCreateForm(screen.lockFrame, "lockForm", args, n);

        
    /* Create the screen saver title box   */
    n = 0;
    if (style.smState.smLockOnTimeoutStatus)
      string = CMPSTR(((char *)GETMESSAGE(12, 4, "Screen Saver")));
    else
      string = CMPSTR(((char *)GETMESSAGE(12, 15, "Auto Screen Blanking")));
    XtSetArg(args[n], XmNtitleString, string);  n++;
    widget_list1[list1_count++] = 
    screen.saverFrame= _DtCreateTitleBox(form, "saverFrame", args, n);
    XmStringFree(string);
    
    /* Create a form for the screen saver widgets */
    n = 0;
    XtSetArg(args[n], XmNallowOverlap, False); n++;
    XtSetArg(args[n], XmNchildType, XmWORK_AREA);  n++;
    screen.saverForm= XmCreateForm(screen.saverFrame, "saverForm", args, n);


    /* Create the cycle time labels and slider     */             
    n = 0;
    if (style.smState.smLockOnTimeoutStatus)
      string = CMPSTR(((char *)GETMESSAGE(12, 6, "Time Per Saver")));
    else
      string = CMPSTR(((char *)GETMESSAGE(12, 20, "Time Per Background")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    /* only if saver extensions are present */     
    if (style.smState.smLockOnTimeoutStatus)
      screen.cycleTimeLabel = XmCreateLabelGadget(screen.saverForm,"cycleTimeLabel", args, n);
    else
      screen.cycleTimeLabel = XmCreateLabelGadget(screen.lockForm,"cycleTimeLabel", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNminimum, CYCLETIME_MIN); n++; 
    XtSetArg(args[n], XmNmaximum, TIMEOUT_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    if (style.smState.smLockOnTimeoutStatus)
      screen.cycleTimeScale= XmCreateScale(screen.saverForm,"cycleTimeScale", args, n);
    else
      screen.cycleTimeScale= XmCreateScale(screen.lockForm,"cycleTimeScale", args, n);
    
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 7, "minutes")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    if (style.smState.smLockOnTimeoutStatus)
      screen.cycleMinutesLabel= XmCreateLabelGadget(screen.saverForm,"cycleMinutes", args, n);
    else
      screen.cycleMinutesLabel= XmCreateLabelGadget(screen.lockForm,"cycleMinutes", args, n);
    XmStringFree(string);

    /* Create the screen saver on/off radio buttons and label */             
    n = 0;
    if (style.smState.smLockOnTimeoutStatus)
      string = CMPSTR(((char *)GETMESSAGE(12, 12, "Screen Saver:")));
    else
      string = CMPSTR(((char *)GETMESSAGE(12, 16, "Screen Blanker:")));
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.saverLabel= XmCreateLabelGadget(screen.saverForm, "saverLabel", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    screen.saverRadioBox= XmCreateRadioBox(screen.saverForm, "saverRadioBox", args, n);
    
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 10, "On")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.saverOn= XmCreateToggleButtonGadget(screen.saverRadioBox, "saverOn", args, n);
    XmStringFree(string);
    
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 11, "Off")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.saverOff= XmCreateToggleButtonGadget(screen.saverRadioBox, "saverOff", args, n);
    XmStringFree(string);
 
    /* Create the screen saver list */             
    n = 0;
    XtSetArg (args[n], XmNautomaticSelection, True);              n++;
    XtSetArg (args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);      n++;
    XtSetArg (args[n], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE);      n++;
    if (low_res) {
      XtSetArg (args[n], XmNheight, 100); n++; }
    else {
      XtSetArg (args[n], XmNheight, SAVER_HEIGHT); n++; }
    XtSetArg (args[n], XmNhighlightColor, 0);      n++;
    /* choose the parent of saverList depending on the presence of saver extensions */ 
    if (style.smState.smLockOnTimeoutStatus)
      screen.saverList = XmCreateScrolledList (screen.saverForm, "saversList", args, n); 
    else
      screen.saverList = XmCreateScrolledList (screen.lockForm, "saversList", args, n); 
    /* use the environment variable DTSCREENSAVERLIST to get the list of */
    /* available screen saver actions, load them into the scrolled list*/
    savers.saversList = getenv ("DTSCREENSAVERLIST"); 
    /* if DTSCREENSAVER is not set, set saver count to zero */
    if (savers.saversList == NULL) {
	savers.saverCount = 0;
	savers.selsaverCount=0;
    } else {
	savers.saverActionNames = build_saverList(savers.saversList, &savers.saverCount);
	savers.saverActionDesc = build_saverDesc(savers.saverActionNames, savers.saverCount);
	/* convert to XmString */ 
	listStrings = MakeListStrings(savers.saverActionDesc); 
	XmListAddItems (screen.saverList, listStrings, savers.saverCount, 0); 
	XtAddCallback (screen.saverList, XmNmultipleSelectionCallback, 
		       ListCB, (XtPointer)NULL);
	FreeListStrings (listStrings, savers.saverCount);        
	
	/* check the state to see which item(s) should be selected, if none are */
	/* selected or if none in the selected list match the  available list then */
	/* set the selection to the last position by default. This should be blank */
	/* if the DTSCREENSAVERLIST environment variable is set to default */
	savers.selsaverActionNames = 
	  build_selsaverList(style.smSaver.saverList, &savers.selsaverCount);

	/* since savers.selsaverCount may be changed below, we save */
	/* number of originally selected savers in order to accurately free the */
	/* savers.selsaverActionNames array below. */
	old_selsaverCount = savers.selsaverCount;

	if (savers.selsaverCount) {
	  savers.selPositions = (int *) XtMalloc(sizeof(int) * savers.selsaverCount);
	  for (n=0; n<savers.selsaverCount; n++) {
	    /* determine if this selected saver matches one in valid saver list */
	    for (m=0; (m < savers.saverCount) && 
	              ((sel_found = strcmp(savers.selsaverActionNames[n],
			                   savers.saverActionNames[m])) != 0); m++) {}
	    if (sel_found == 0) {
	      XmListSelectPos (screen.saverList, m+1, False);
	      savers.selPositions[n]=m+1;
	      sel_matched = True; 
	    } else {
	      /* This selected saver isn't valid, need to get rid of it.
	         Do this by shifting each selected saver in the array
		 back one position, i.e. n = n+1, n+1 = n+2, ....  
		 This overwrites the nth element and leaves the empty
		 space at the end of the array.  */
	      for (i=n; i < (savers.selsaverCount - 1); i++)
		savers.selsaverActionNames[i] = savers.selsaverActionNames[i+1];

	      /* reduce # of selected savers */
 	      savers.selsaverCount--;

	      /* reset selected saver array position counter back one position for 
	         next loop test */
	      n--;
	    } /* if (sel_found == 0) */
	  } /* for n<savers.selsaverCount */
	} /* if (savers.selsaverCount)  */

	/* if there are still savers selected that match valid saver names */ 
	if (savers.selsaverCount) {
	  /* set current saver to first selection */
	  savers.current_saver = savers.saverActionNames[savers.selPositions[0] - 1];
	  savers.current_saverDesc = savers.saverActionDesc[savers.selPositions[0] - 1];
	  savers.current_position = savers.selPositions[0];
	} else {
	  /* highlight last saver */
	  XmListSelectPos (screen.saverList, savers.saverCount, False);   

	  /* set current saver to last one */
	  savers.current_saver = savers.saverActionNames[savers.saverCount - 1]; 
	  savers.current_saverDesc = savers.saverActionDesc[savers.saverCount - 1]; 

	  savers.selPositions = (int *) XtMalloc(sizeof(int));
	  savers.selPositions[0] = savers.saverCount;
	  savers.selsaverCount = 1;
	  savers.current_position = 1;
	}
	/* save the selected positions for later use */
	savers.origSelPositions = (int *) XtMalloc(sizeof(int) * savers.selsaverCount);
	for (i=0; i<savers.selsaverCount; i++)
	    savers.origSelPositions[i] = savers.selPositions[i];
	savers.orig_selsaverCount = savers.selsaverCount;

	savers.previous_selsaverCount = savers.selsaverCount;
	free_saverList(savers.selsaverActionNames, old_selsaverCount); 
      }
    
    /* Create frame for the saver area */
    n = 0;
    XtSetArg (args[n], XmNshadowType, XmSHADOW_IN);                     n++;
    XtSetArg (args[n], XmNshadowThickness, savers.shadow);           n++;
    XtSetArg (args[n], XmNhighlightThickness, 0);                       n++;
    XtSetArg (args[n], XmNtraversalOn, False);                          n++;  
    /* depending on the presence of saver extensions choose the parent of saverAreaFrame */ 
    if (style.smState.smLockOnTimeoutStatus)
      screen.saverAreaFrame = XmCreateFrame(screen.saverForm, "saverAreaFrame", args, n);
    else
      screen.saverAreaFrame = XmCreateFrame(screen.lockForm, "saverAreaFrame", args, n);
    
    /* Create saver area  for the screen savers */
    n = 0;
    XtSetArg (args[n], XmNtraversalOn, False); n++;  
    if (low_res) {
      XtSetArg (args[n], XmNwidth, 180); n++; }
    else {
      XtSetArg (args[n], XmNwidth, SAVER_WIDTH); n++; }
    screen.saverArea = XmCreateDrawingArea (screen.saverAreaFrame, "saverArea", args, n);
    /* get the initial secondary color pixel value for dtstyle for later use */
    XtVaGetValues(screen.saverArea, XmNbackground, &savers.bg, NULL);

    /* label for the previewed saver  */
    n = 0;
    if (savers.saverCount)
	string = XmStringCreateLocalized (savers.current_saverDesc);
    else
	string = XmStringCreateLocalized (" ");
    XtSetArg(args[n], XmNlabelString, string); n++;
    if (style.smState.smLockOnTimeoutStatus)
	screen.prevLabel = XmCreateLabelGadget(screen.saverForm,"prevLabel", args, n);
    else
	screen.prevLabel = XmCreateLabelGadget(screen.lockForm,"prevLabel", args, n);
    XmStringFree(string);

    /* Create the screen saver labels and slider */
    n = 0;
    if (style.smState.smLockOnTimeoutStatus)
      string = CMPSTR(((char *)GETMESSAGE(12, 5, "Start Saver")));
    else
      string = CMPSTR(((char *)GETMESSAGE(12, 17, "Start Blanker")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    screen.timeOutLabel= XmCreateLabelGadget(screen.saverForm,"timeOutLabel", args, n);
    XmStringFree(string);

    n = 0;
    XtSetArg(args[n], XmNminimum, TIMEOUT_MIN); n++; 
    XtSetArg(args[n], XmNmaximum, TIMEOUT_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    screen.timeOutScale= XmCreateScale(screen.saverForm,"timeOutScale", args, n);

    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 7, "minutes")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.saverMinutesLabel= XmCreateLabelGadget(screen.saverForm,"saverMinutes", args, n);
    XmStringFree(string);

    /* Create the screen lock on/off radio buttons and label  */             
    /* mapped only when saver extensions are present */
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 13, "Screen Lock:")));
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.lockLabel= XmCreateLabelGadget(screen.lockForm, "lockLabel", args, n);
    XmStringFree(string);
    
    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    screen.lockRadioBox= XmCreateRadioBox(screen.lockForm, "lockRadioBox", args, n);
    
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 10, "On")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNset, style.smState.smCoverScreen ? True : False); n++;
    screen.lockOn= XmCreateToggleButtonGadget(screen.lockRadioBox, "lockOn", args, n);
    XmStringFree(string);
    
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 11, "Off")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    XtSetArg(args[n], XmNset, style.smState.smCoverScreen ? False : True); n++;
    screen.lockOff= XmCreateToggleButtonGadget(screen.lockRadioBox, "lockOff", args, n);
    XmStringFree(string);
    
    
    /* Create the screen lock labels and slider */
    /* mapped only when saver extensions are present */
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 14, "Start Lock")));
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.lockOutLabel= XmCreateLabelGadget(screen.lockForm,"lockOutLabel", args, n);
    XmStringFree(string);
    
    n = 0;
    XtSetArg(args[n], XmNminimum, TIMEOUT_MIN); n++; 
    XtSetArg(args[n], XmNmaximum, TIMEOUT_MAX); n++; 
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNhighlightThickness, SCALE_HIGHLIGHT_THICKNESS); n++; 
    screen.lockOutScale= XmCreateScale(screen.lockForm,"lockOutScale", args, n);
    
    
    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 7, "minutes")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.lockMinutesLabel= XmCreateLabelGadget(screen.lockForm,"lockMinutes", args, n);
    XmStringFree(string);

    /* Create the radio buttons for lock savers */
    /* mapped only when saver extensions are not present */
    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNspacing, 25); n++;
    screen.lockingRadioBox= XmCreateRadioBox(screen.lockForm, "lockingRadioBox", args, n);

    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 21, "Use Backgrounds For Lock")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.useSaversOn= XmCreateToggleButtonGadget(screen.lockingRadioBox, "useSaversOn", args, n);
    XmStringFree(string);
    


    n = 0;
    string = CMPSTR(((char *)GETMESSAGE(12, 22, "Transparent Lock")));
    XtSetArg(args[n], XmNlabelString, string); n++;
    screen.useSaversOff= XmCreateToggleButtonGadget(screen.lockingRadioBox, "useSaversOff", args, n);
    XmStringFree(string);
    
    /* add callbacks */

    /* Configure dialog based on the presence of screen saver extensions */
    if (!style.smState.smLockOnTimeoutStatus)
      {
	XtAddCallback(style.screenDialog, XmNmapCallback, no_svr_ext_formLayoutCB, NULL);
	XtAddCallback(style.screenDialog, XmNmapCallback, no_svr_ext_DtmapCB_screenDialog, shell);
	XtAddCallback(screen.systemDefault, XmNactivateCallback, no_svr_ext_systemDefaultCB, NULL);
	XtAddCallback(screen.timeOutScale, XmNvalueChangedCallback, 
		      no_svr_ext_timeOutvalueChangedCB, NULL);
	XtAddCallback(screen.saverOn, XmNvalueChangedCallback, 
		      no_svr_ext_saverToggleCB, NULL);
	XtAddCallback(screen.saverOff, XmNvalueChangedCallback,
		      no_svr_ext_saverToggleCB, NULL);
	XtAddCallback(screen.useSaversOn, XmNvalueChangedCallback, 
		      saversToggleCB, NULL);
	XtAddCallback(screen.useSaversOff, XmNvalueChangedCallback,
		      saversToggleCB, NULL);

	XtAddCallback(style.screenDialog, XmNcallback, no_svr_ext_ButtonCB, NULL);
      }
    else
      {
	XtAddCallback(style.screenDialog, XmNmapCallback, formLayoutCB, NULL);
        XtAddCallback(style.screenDialog, XmNmapCallback, _DtmapCB_screenDialog, shell);
	XtAddCallback(screen.systemDefault, XmNactivateCallback, systemDefaultCB, NULL);
	XtAddCallback(screen.timeOutScale, XmNvalueChangedCallback, 
		      timeOutvalueChangedCB, NULL);
	XtAddCallback(screen.lockOutScale, XmNvalueChangedCallback, 
		      timeOutvalueChangedCB, NULL);
	XtAddCallback(screen.saverOn, XmNvalueChangedCallback, 
		      saverToggleCB, NULL);
	XtAddCallback(screen.saverOff, XmNvalueChangedCallback,
		      saverToggleCB, NULL);
	XtAddCallback(screen.lockOn, XmNvalueChangedCallback, 
		      lockToggleCB, NULL);
	XtAddCallback(screen.lockOff, XmNvalueChangedCallback,
		      lockToggleCB, NULL);
	XtAddCallback(style.screenDialog, XmNcallback, ButtonCB, NULL);
      }
    XtAddCallback(style.screenDialog, XmNunmapCallback, UnmapCB, shell);
    XtAddCallback(style.screenDialog, XmNhelpCallback,
		  (XtCallbackProc)HelpRequestCB, (XtPointer)HELP_SCREEN_DIALOG);
    
    /* calculate the max label width for labels */
    if ((LabelWidth = XtWidth(screen.saverLabel)) > MaxLabelWidth) 
      MaxLabelWidth = LabelWidth;
    if ((LabelWidth = XtWidth(screen.timeOutLabel)) > MaxLabelWidth) 
      MaxLabelWidth = LabelWidth;
    if ((LabelWidth = XtWidth(screen.cycleTimeLabel)) > MaxLabelWidth) 
      MaxLabelWidth = LabelWidth;
    if (style.smState.smLockOnTimeoutStatus)
      {
	if ((LabelWidth = XtWidth(screen.lockLabel)) > MaxLabelWidth) 
	  MaxLabelWidth = LabelWidth;
	if ((LabelWidth = XtWidth(screen.lockOutLabel)) > MaxLabelWidth) 
	  MaxLabelWidth = LabelWidth;
      }
    
    XtVaSetValues(screen.saverLabel, XmNwidth, MaxLabelWidth, NULL);
    XtVaSetValues(screen.timeOutLabel, XmNwidth, MaxLabelWidth, NULL);
    XtVaSetValues(screen.cycleTimeLabel, XmNwidth, MaxLabelWidth, NULL);
    if (style.smState.smLockOnTimeoutStatus)
      {
	XtVaSetValues(screen.lockLabel, XmNwidth, MaxLabelWidth, NULL);
	XtVaSetValues(screen.lockOutLabel, XmNwidth, MaxLabelWidth, NULL);
      }
    
    /* manage widgets */
    XtManageChildren(widget_list1,list1_count);
    XtManageChild(form);
    XtManageChild(screen.saverForm);
    XtManageChild(screen.cycleTimeLabel);
    XtManageChild(screen.cycleTimeScale);
    XtManageChild(screen.saverLabel);
    XtManageChild(screen.prevLabel);  
    XtManageChild(screen.saverRadioBox); 
    XtManageChild(screen.saverOn);
    XtManageChild(screen.saverOff);
    XtManageChild(screen.timeOutLabel);
    XtManageChild(screen.timeOutScale);   
    XtManageChild(screen.saverMinutesLabel);
    XtManageChild(screen.saverList);
    XtManageChild(screen.saverAreaFrame);  
    XtManageChild(screen.saverArea);  
    XtManageChild(screen.cycleMinutesLabel);
    XtManageChild(screen.saverList);
    XtManageChild(screen.lockForm);
    /* manage the lock label, scale and minutes label only if */
    /* saver extensions are present */
    if (style.smState.smLockOnTimeoutStatus)
     { 
       XtManageChild(screen.lockLabel);
       XtManageChild(screen.lockRadioBox);
       XtManageChild(screen.lockOn);
       XtManageChild(screen.lockOff); 
       XtManageChild(screen.lockOutLabel);
       XtManageChild(screen.lockOutScale);   
       XtManageChild(screen.lockMinutesLabel);
     }
    else
      {
	XtManageChild(screen.lockingRadioBox);
	XtManageChild(screen.useSaversOn);
	XtManageChild(screen.useSaversOff);
      }
   XtAddEventHandler(style.shell, StructureNotifyMask, False, IconHandler, NULL);

    return(style.screenDialog);
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
    register int     n;
    Arg              args[MAX_ARGS];
    Dimension        ScaleHeight;
    Dimension        LabelHeight;
    Dimension        RadioHeight;
    Dimension        RadioOffset, ScaleOffset;

    /* calculate width and height information */    
    ScaleHeight = XtHeight(screen.timeOutScale);
    LabelHeight = XtHeight(screen.timeOutLabel);
    RadioHeight = XtHeight(screen.saverRadioBox);
    RadioOffset = ((Dimension) (RadioHeight - LabelHeight) / 2) - 2;
    ScaleOffset = 0; 

    /* do form attachments */

    /* Picture Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (screen.pictLabel, args, n);

    /* system Default */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.systemDefault, args, n);

    /* lock titlebox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.lockFrame, args, n);


    /* lock label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);     n++;
    XtSetArg (args[n], XmNtopOffset,         style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (screen.lockLabel, args, n);


    /* lock Radiobox */
    n=0;
    
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          screen.lockLabel);  n++;
    XtSetArg(args[n], XmNtopOffset,          -RadioOffset);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;    
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNleftWidget,         screen.lockLabel);  n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (screen.lockRadioBox, args, n);


    /* lockOut  Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          screen.lockLabel);  n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing + ScaleHeight - LabelHeight);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (screen.lockOutLabel, args, n);


   /* lockOut Scale */ 
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.lockOutLabel);n++; 
    XtSetArg(args[n], XmNbottomOffset,       ScaleOffset); n++;  
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget,         screen.lockOutLabel);n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_WIDGET);       n++; 
    XtSetArg(args[n], XmNrightWidget,        screen.lockMinutesLabel);n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.lockOutScale, args, n);


    /* Minutes label */ 
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.lockOutLabel);n++; 
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.lockMinutesLabel, args, n);


    /* saver titlebox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,          screen.pictLabel);  n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.lockFrame);  n++;
    XtSetArg(args[n], XmNbottomOffset,       2*style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing); n++;
    XtSetValues (screen.saverFrame, args, n);

    /* cycleTime Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);  n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (screen.cycleTimeLabel, args, n);

    /* cycleTime Scale */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.cycleTimeLabel);n++;
    XtSetArg(args[n], XmNbottomOffset,       ScaleOffset);n++; 
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNleftWidget,         screen.cycleTimeLabel);n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_WIDGET);       n++; 
    XtSetArg(args[n], XmNrightWidget,        screen.cycleMinutesLabel);n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.cycleTimeScale, args, n);

    /* Minutes label */ 
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.cycleTimeLabel);n++; 
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++; 
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.cycleMinutesLabel, args, n);
 
    /* TimeOut Label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.cycleTimeLabel);n++;    
    XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing + ScaleHeight - LabelHeight);  n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
    XtSetValues (screen.timeOutLabel, args, n);

    /* Minutes label */ 
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;    
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.timeOutLabel);n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++; 
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.saverMinutesLabel, args, n);

    /* TimeOut Scale */ 
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,       screen.timeOutLabel);n++;
    XtSetArg(args[n], XmNbottomOffset,       ScaleOffset); n++; 
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,         screen.timeOutLabel);n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNrightWidget,        screen.saverMinutesLabel);n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.timeOutScale, args, n);

    /* saver label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);     n++;
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (screen.saverLabel, args, n);


    /* saver Radiobox */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,          screen.saverLabel);  n++;
    XtSetArg(args[n], XmNtopOffset,          -RadioOffset);  n++;
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNleftWidget,         screen.saverLabel);  n++;
    XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
    XtSetValues (screen.saverRadioBox, args, n);


    /* preview label */
    n=0;
    XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);       n++;
    XtSetArg(args[n], XmNtopWidget,          screen.saverLabel);  n++; 
    XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);  n++; 
    XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);     n++;
    XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
    XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
    XtSetValues (screen.prevLabel, args, n);

    /* scrolled list  */
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,      XmATTACH_WIDGET);          n++;
    XtSetArg (args[n], XmNtopWidget,          screen.prevLabel);  n++;
    XtSetArg (args[n], XmNtopOffset,          style.verticalSpacing/2);    n++;
    XtSetArg (args[n], XmNrightAttachment,    XmATTACH_NONE);        n++;
    XtSetArg (args[n], XmNleftAttachment,     XmATTACH_FORM);        n++;
    XtSetArg(args[n],  XmNleftOffset,         style.horizontalSpacing);  n++;
    XtSetArg (args[n], XmNbottomAttachment,   XmATTACH_WIDGET);       n++;
    XtSetArg(args[n],  XmNbottomWidget,       screen.timeOutLabel);       n++;
    XtSetArg(args[n],  XmNbottomOffset,       5*style.verticalSpacing);  n++;
    XtSetValues (XtParent(screen.saverList),  args, n);


    /* saverArea */
    n=0;
    XtSetArg (args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET);   n++;
    XtSetArg(args[n], XmNtopWidget,           XtParent(screen.saverList));  n++;
    XtSetArg (args[n], XmNleftAttachment,     XmATTACH_WIDGET);         n++;
    XtSetArg(args[n], XmNleftWidget,          XtParent(screen.saverList));  n++;
    XtSetArg(args[n], XmNleftOffset,          style.horizontalSpacing);  n++;
    XtSetArg (args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
    XtSetArg(args[n], XmNrightOffset,         style.horizontalSpacing);  n++;
    XtSetArg (args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET);   n++;
    XtSetArg(args[n], XmNbottomWidget,        XtParent(screen.saverList));  n++;
    XtSetValues (screen.saverAreaFrame, args, n);
 

    XtRemoveCallback(style.screenDialog, XmNmapCallback, formLayoutCB, NULL);

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* no_svr_ext_formLayoutCB               */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
no_svr_ext_formLayoutCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  register int     n;
  Arg              args[MAX_ARGS];
  Dimension        ScaleHeight;
  Dimension        LabelHeight;
  Dimension        RadioHeight;
  Dimension        RadioOffset, ScaleOffset;
  
  /* calculate width and height information */    
  ScaleHeight = XtHeight(XtParent(screen.timeOutScale));
  LabelHeight = XtHeight(screen.timeOutLabel);
  RadioHeight = XtHeight(screen.saverRadioBox);
  
  RadioOffset = ((Dimension) (RadioHeight - LabelHeight) / 2) - 2;
  ScaleOffset = 0;
  /* do form attachments */
  
  /* Picture Label */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
  XtSetValues (screen.pictLabel, args, n);
  
  /* system Default */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.systemDefault, args, n);
  
  
  /* saver titlebox */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
  XtSetArg(args[n], XmNtopWidget,          screen.pictLabel);  n++;
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing); n++;
  XtSetValues (screen.saverFrame, args, n);

  /* saver label */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);     n++;
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
  XtSetValues (screen.saverLabel, args, n);
  
  /* saver Radiobox */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget,          screen.saverLabel);  n++;
  XtSetArg(args[n], XmNtopOffset,          -RadioOffset);  n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);     n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);       n++;
  XtSetArg(args[n], XmNleftWidget,         screen.saverLabel);  n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
  XtSetValues (screen.saverRadioBox, args, n);
  
  /* TimeOut Label */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget,          screen.saverLabel);  n++;
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing + ScaleHeight - LabelHeight); n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
  XtSetValues (screen.timeOutLabel, args, n);
  
  /* TimeOut Scale */ 
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget,       screen.timeOutLabel);n++;
  XtSetArg(args[n], XmNbottomOffset,       ScaleOffset); n++; 
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
  XtSetArg(args[n], XmNleftWidget,         screen.timeOutLabel);n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_WIDGET);       n++;
  XtSetArg(args[n], XmNrightWidget,        screen.saverMinutesLabel);n++;
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.timeOutScale, args, n);

  /* Minutes label */ 
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;    
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget,       screen.timeOutLabel);n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);     n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++; 
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.saverMinutesLabel, args, n);
  
  /* lock titlebox */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
  XtSetArg(args[n], XmNtopWidget,          screen.saverFrame);  n++; 
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);    n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);    n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.lockFrame, args, n);

  /* locking radiobox */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);  n++; 
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_NONE);     n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);         n++;
  XtSetValues (screen.lockingRadioBox, args, n);

  /* preview label */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_WIDGET);     n++;
  XtSetArg(args[n], XmNtopWidget,          screen.lockingRadioBox);  n++; 
  XtSetArg(args[n], XmNtopOffset,          style.verticalSpacing);  n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.prevLabel, args, n);
  
  /* cycleTime Label */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);     n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNbottomOffset,       style.verticalSpacing);  n++;
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_FORM);       n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_NONE);       n++;
  XtSetValues (screen.cycleTimeLabel, args, n);
  
  /* cycleTime Scale */
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget,       screen.cycleTimeLabel);n++;
  XtSetArg(args[n], XmNbottomOffset,       ScaleOffset); n++; 
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_WIDGET);     n++;
  XtSetArg(args[n], XmNleftWidget,         screen.cycleTimeLabel);n++;
  XtSetArg(args[n], XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_WIDGET);       n++; 
  XtSetArg(args[n], XmNrightWidget,        screen.cycleMinutesLabel);n++;
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.cycleTimeScale, args, n);
  
  /* Minutes label */ 
  n=0;
  XtSetArg(args[n], XmNtopAttachment,      XmATTACH_NONE);       n++;
  XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget,       screen.cycleTimeLabel);n++; 
  XtSetArg(args[n], XmNleftAttachment,     XmATTACH_NONE);     n++;
  XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM);       n++; 
  XtSetArg(args[n], XmNrightOffset,        style.horizontalSpacing);  n++;
  XtSetValues (screen.cycleMinutesLabel, args, n);
  
  /* scrolled list  */
  n = 0;
  XtSetArg (args[n], XmNtopAttachment,      XmATTACH_WIDGET);          n++;
  XtSetArg (args[n], XmNtopWidget,          screen.prevLabel);  n++;
  XtSetArg (args[n], XmNtopOffset,          style.verticalSpacing);    n++;
  XtSetArg (args[n], XmNrightAttachment,    XmATTACH_NONE);        n++;
  XtSetArg (args[n], XmNleftAttachment,     XmATTACH_FORM);        n++;
  XtSetArg(args[n],  XmNleftOffset,         style.horizontalSpacing);  n++;
  XtSetArg (args[n], XmNbottomAttachment,   XmATTACH_WIDGET);       n++;
  XtSetArg(args[n],  XmNbottomWidget,       screen.cycleTimeLabel);       n++;
  XtSetArg(args[n],  XmNbottomOffset,       5*style.verticalSpacing);  n++;
  XtSetValues (XtParent(screen.saverList),  args, n);
  
  
  /* saverArea */
  n=0;
  XtSetArg (args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET);   n++;
  XtSetArg(args[n], XmNtopWidget,           XtParent(screen.saverList));  n++;
  XtSetArg (args[n], XmNleftAttachment,     XmATTACH_WIDGET);         n++;
  XtSetArg(args[n], XmNleftWidget,          XtParent(screen.saverList));  n++;
  XtSetArg(args[n], XmNleftOffset,          style.horizontalSpacing);  n++;
  XtSetArg (args[n], XmNrightAttachment,    XmATTACH_FORM);         n++;
  XtSetArg(args[n], XmNrightOffset,         style.horizontalSpacing);  n++;
  XtSetArg (args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET);   n++;
  XtSetArg(args[n], XmNbottomWidget,        XtParent(screen.saverList));  n++;
  XtSetValues (screen.saverAreaFrame, args, n);
  
  
  XtRemoveCallback(style.screenDialog, XmNmapCallback, no_svr_ext_formLayoutCB, NULL);
  }

/*+++++++++++++++++++++++++++++++++++++++*/
/* _DtmapCB_screenDialog                 */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
_DtmapCB_screenDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  static int    first_time = 1;
  register int  n;
  Arg           args[MAX_ARGS];
  XmString      string;
  Boolean       lock_on, saver_on;  

  if (first_time)
    {
      DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));
      
      if (!save.restoreFlag)
	putDialog ((Widget)client_data, w);
      
      /*  Retrieve the value for the saver slider from xrdb */
      screen.saverTimeout_xrdb = atoi((char *)style.xrdb.timeoutScale);
      if (screen.saverTimeout_xrdb < 1)
	screen.saverTimeout_xrdb = 10;

      /*  Retrieve the value for the lock slider from xrdb */
      screen.lockTimeout_xrdb = atoi((char *)style.xrdb.lockoutScale);
      if (screen.lockTimeout_xrdb < 1)
	screen.lockTimeout_xrdb = 30;
      
      /* initialize the value of savertimeout and locktimeout and cycle time to the */
      /* initial state or to the values retrieved from xrdb if the corresponding*/
      /* initial state is zero */
      if (style.smState.smSaverTimeout)
	screen.saverTimeout = style.smState.smSaverTimeout / 60;
      else
	screen.saverTimeout = screen.saverTimeout_xrdb;
      
      if (style.smState.smLockTimeout)
	screen.lockTimeout = style.smState.smLockTimeout / 60;
      else
	screen.lockTimeout = screen.lockTimeout_xrdb;
      
      if (style.smState.smCycleTimeout)
	screen.cycleTimeout = style.smState.smCycleTimeout / 60;
      else 
	screen.cycleTimeout = 0;
      

      /* set the value of the scales */
      XtVaSetValues(screen.timeOutScale, XmNvalue, screen.saverTimeout, NULL);
      XtVaSetValues(screen.lockOutScale, XmNvalue, screen.lockTimeout, NULL);
      XtVaSetValues(screen.cycleTimeScale, XmNvalue, screen.cycleTimeout , NULL);  

      /* set the value of the saver toggles */
      XmToggleButtonGadgetSetState(screen.saverOn, 
				   style.smState.smSaverTimeout  ? True : False, False);
      XmToggleButtonGadgetSetState(screen.saverOff, 
				   !style.smState.smSaverTimeout  ? True : False, False );
      XmToggleButtonGadgetSetState(screen.lockOn, 
				   style.smState.smLockTimeout  ? True : False, False);
      XmToggleButtonGadgetSetState(screen.lockOff, 
				   !style.smState.smLockTimeout  ? True : False, False);

      /* make saver widgets insensitive if screen saver is off */
      /* also blank out the preview label widget */
      if (XmToggleButtonGadgetGetState(screen.saverOff))
	{
	  XtSetSensitive(screen.saverList, False);
	  XtSetSensitive(screen.saverArea, False);
	  XtSetSensitive(screen.timeOutLabel, False);
	  XtSetSensitive(screen.timeOutScale, False);
	  XtSetSensitive(screen.saverMinutesLabel, False);
	  XtSetSensitive(screen.cycleTimeLabel, False);
	  XtSetSensitive(screen.cycleTimeScale, False);
	  XtSetSensitive(screen.cycleMinutesLabel, False); 
	  XtUnmanageChild(screen.prevLabel);  	      
	}
      /* make saver widgets sensitive if screen saver is on */
      /* also manage the preview label widget */
      if (XmToggleButtonGadgetGetState(screen.saverOn))
	{
	  XtSetSensitive(screen.saverList, True);
	  XtSetSensitive(screen.saverArea, True);
	  XtSetSensitive(screen.timeOutLabel, True);
	  XtSetSensitive(screen.timeOutScale, True);
	  XtSetSensitive(screen.saverMinutesLabel, True);
	  XtManageChild(screen.prevLabel);  	      
	  /* make cycletime scale sensitive if selected savers are more than 1 */
	  if (savers.selsaverCount > 1)
	    {
	      XtSetSensitive(screen.cycleTimeLabel, True);
	      XtSetSensitive(screen.cycleTimeScale, True);
	      XtSetSensitive(screen.cycleMinutesLabel, True); 
	    }
	  else
	    {
	      XtSetSensitive(screen.cycleTimeLabel, False);
	      XtSetSensitive(screen.cycleTimeScale, False);
	      XtSetSensitive(screen.cycleMinutesLabel, False); 
	    }
	}
      /* make lock widgets insensitive if lock screen is off */
      if (XmToggleButtonGadgetGetState(screen.lockOff))
	{
	  XtSetSensitive(screen.lockOutLabel, False);
	  XtSetSensitive(screen.lockOutScale, False);
	  XtSetSensitive(screen.lockMinutesLabel, False);
	}
      /* make lock widgets sensitive if lock screen is on */
	  if (XmToggleButtonGadgetGetState(screen.lockOn))
	    {
	      XtSetSensitive(screen.lockOutLabel, True);
	      XtSetSensitive(screen.lockOutScale, True);
	      XtSetSensitive(screen.lockMinutesLabel, True);
	    }
      

      if (!style.smState.smSaverTimeout)
	screen.origTimeout = screen.newTimeout = style.smState.smLockTimeout;
      else
	if (!style.smState.smLockTimeout)
	  screen.origTimeout = screen.newTimeout = style.smState.smSaverTimeout;
	else
	  screen.origTimeout = 
	    screen.newTimeout = 
	      MIN(style.smState.smSaverTimeout, style.smState.smLockTimeout); 
	
      first_time = 0;
    }
  /* check the saver values from X  -  need to do this everytime dialog is mapped because */
  /* the user may have changed settings thru X while style manager has been running */
  /* if the value of the timeout retrieved from X is not equal to the new timeout set in the */
  /* style manager interface, then set both lock time and saver time scales to value retrieved from */
  /* X, otherwise leave them alone */
  
  XGetScreenSaver(style.display, &screen.xTimeout, &screen.interval, 
		  &screen.blanking, &screen.exposures);
  
  screen.new_blanking = DontPreferBlanking;

  if (screen.xTimeout < 0) 
    screen.xTimeout = 0;
  else
    if ((screen.xTimeout < 60) && (screen.xTimeout > 1))
      screen.xTimeout = 60;
    else
      if (screen.xTimeout > TIMEOUT_MAX * 60) 
	screen.xTimeout = TIMEOUT_MAX * 60;
    
  if (screen.xTimeout < 0) 
    screen.xTimeout = 0;
  else
    if ((screen.xTimeout < 60) && (screen.xTimeout > 1))
      screen.xTimeout = 60;
    else
      if (screen.xTimeout > TIMEOUT_MAX * 60) 
	screen.xTimeout = TIMEOUT_MAX * 60;
    
  screen.timeMismatch = False;
  if (screen.xTimeout != screen.newTimeout)
    {
      screen.timeMismatch = True;
      screen.newTimeout = screen.xTimeout;
      if (screen.xTimeout)
	{
	  screen.saverTimeout = screen.xTimeout / 60;
	  screen.lockTimeout = screen.xTimeout / 60;
	}
      else
	{
	  screen.saverTimeout = screen.saverTimeout_xrdb;
	  screen.lockTimeout = screen.lockTimeout_xrdb;
	}
      /* set the value of the saver widgets */
      XmToggleButtonGadgetSetState(screen.saverOn, screen.xTimeout ? True : False, False);
      XmToggleButtonGadgetSetState(screen.saverOff, !screen.xTimeout ? True : False, False);
      XmToggleButtonGadgetSetState(screen.lockOn, screen.xTimeout ? True : False, False);
      XmToggleButtonGadgetSetState(screen.lockOff, !screen.xTimeout ? True : False, False);
      
      /* set the value of the scales */
      XtVaSetValues(screen.timeOutScale, XmNvalue, screen.saverTimeout, NULL);
      XtVaSetValues(screen.lockOutScale, XmNvalue, screen.lockTimeout_xrdb, NULL);  
            
      /* make saver widgets insensitive if screen saver is off */
      /* also blankout the preview label widget */
      if (XmToggleButtonGadgetGetState(screen.saverOff))
	{
	  XtSetSensitive(screen.saverList, False);
	  XtSetSensitive(screen.saverArea, False);
	  XtSetSensitive(screen.timeOutLabel, False);
	  XtSetSensitive(screen.timeOutScale, False);
	  XtSetSensitive(screen.saverMinutesLabel, False);
	  XtSetSensitive(screen.cycleTimeLabel, False);
	  XtSetSensitive(screen.cycleTimeScale, False);
	  XtSetSensitive(screen.cycleMinutesLabel, False); 
	  XtUnmanageChild(screen.prevLabel);  	      
	}
      /* make saver widgets sensitive if screen saver is on */
      /* also manage the preview label widget */
      if (XmToggleButtonGadgetGetState(screen.saverOn))
	{
	  XtSetSensitive(screen.saverList, True);
	  XtSetSensitive(screen.saverArea, True);
	  XtSetSensitive(screen.timeOutLabel, True);
	  XtSetSensitive(screen.timeOutScale, True);
	  XtSetSensitive(screen.saverMinutesLabel, True);
	  XtManageChild(screen.prevLabel);  	      
	  /* make cycletime scale sensitive if selected savers are more than 1 */
	  if (savers.selsaverCount > 1)
	    {
	      XtSetSensitive(screen.cycleTimeLabel, True);
	      XtSetSensitive(screen.cycleTimeScale, True);
	      XtSetSensitive(screen.cycleMinutesLabel, True); 
	    }
	  else
	    {
	      XtSetSensitive(screen.cycleTimeLabel, False);
	      XtSetSensitive(screen.cycleTimeScale, False);
	      XtSetSensitive(screen.cycleMinutesLabel, False); 
	    }
	}
      
      /* make lock widgets insensitive if lock screen is off */
      if (XmToggleButtonGadgetGetState(screen.lockOff))
	{
	  XtSetSensitive(screen.lockOutLabel, False);
	  XtSetSensitive(screen.lockOutScale, False);
	  XtSetSensitive(screen.lockMinutesLabel, False);
	}

      /* make lock widgets sensitive if lock screen is on */
      if (XmToggleButtonGadgetGetState(screen.lockOn))
	{
	  XtSetSensitive(screen.lockOutLabel, True);
	  XtSetSensitive(screen.lockOutScale, True);
	  XtSetSensitive(screen.lockMinutesLabel, True);
	}

    }  
  
  /* run the current screen saver only if screen saver is turned on and a valid saver exists*/
  if (XmToggleButtonGadgetGetState(screen.saverOn) && savers.saverCount && !savers.saverstate)
    {
      XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);  
      
      savers.saverstate = _DtSaverStart(style.display, &screen.saverArea, 1,
		      savers.current_saver, style.screenDialog);
      string = XmStringCreateLocalized (savers.current_saverDesc);
      XtVaSetValues(screen.prevLabel, XmNlabelString, string, NULL);
      XmStringFree(string);	    
    }
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* no_svr_ext_DtmapCB_screenDialog       */
/*+++++++++++++++++++++++++++++++++++++++*/

static void 
no_svr_ext_DtmapCB_screenDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  static int    first_time = 1;
  register int  n;
  Arg           args[MAX_ARGS];
  XmString      string;
  int           mintime;
    
  if (first_time)
    {
      DtWsmRemoveWorkspaceFunctions(style.display, XtWindow(XtParent(w)));
      
      if (!save.restoreFlag)
	putDialog ((Widget)client_data, w);
      
      /*  Retrieve the value for the saver slider from xrdb */
      screen.saverTimeout_xrdb = atoi((char *)style.xrdb.timeoutScale);
      if (screen.saverTimeout_xrdb < 1)
	screen.saverTimeout_xrdb = 10;

      if (style.smState.smCycleTimeout)
	screen.cycleTimeout = style.smState.smCycleTimeout / 60;
      else 
	screen.cycleTimeout = 0;
      
      /* set the value of the cycle time scale */
      XtVaSetValues(screen.cycleTimeScale, XmNvalue, screen.cycleTimeout , NULL);  
      
      /* set the initial condition for the locking RadioBoxes */ 
      
      XmToggleButtonGadgetSetState(screen.useSaversOn, 
				   style.smState.smSaverTimeout  ? True : False, False);
      XmToggleButtonGadgetSetState(screen.useSaversOff, 
				   style.smState.smSaverTimeout  ? False : True, False);

      if (style.smState.smSaverTimeout)
	{
	  XtSetSensitive(screen.saverList, True);
	  XtSetSensitive(screen.saverArea, True);
	  if (savers.selsaverCount > 1)
	    {
	      XtSetSensitive(screen.cycleTimeLabel, True);
	      XtSetSensitive(screen.cycleTimeScale, True);
	      XtSetSensitive(screen.cycleMinutesLabel, True); 
	    }
	  else
	    {
	      XtSetSensitive(screen.cycleTimeLabel, False);
	      XtSetSensitive(screen.cycleTimeScale, False);
	      XtSetSensitive(screen.cycleMinutesLabel, False); 
	    }
	  XtManageChild(screen.prevLabel);  	
	  SmNewSaverTime(60);
	}  
      else
	{
	  XtSetSensitive(screen.saverList, False);
	  XtSetSensitive(screen.saverArea, False);
	  XtSetSensitive(screen.cycleTimeLabel, False);
	  XtSetSensitive(screen.cycleTimeScale, False);
	  XtSetSensitive(screen.cycleMinutesLabel, False); 
	  XtUnmanageChild(screen.prevLabel);  
	  SmNewSaverTime(0);
	} 
      screen.newTimeout = screen.origTimeout = 0;

      first_time = 0;
    }
  
  /* check the saver values from X  -  need to do this everytime dialog is mapped because */
  /* the user may have changed settings thru X while style manager has been running */
  /* if the value of the timeout retrieved from X is not equal to the new timeout set in the */
  /* style manager interface, then set both lock time and saver time scales to value retrieved from */
  /* X, otherwise leave them alone */
  
  XGetScreenSaver(style.display, &screen.xTimeout, &screen.interval, 
		  &screen.blanking, &screen.exposures);
  
  screen.new_blanking = PreferBlanking;

  if (screen.xTimeout < 0) 
    screen.xTimeout = 0;
  else
    if ((screen.xTimeout < 60) && (screen.xTimeout > 1))
      screen.xTimeout = 60;
    else
      if (screen.xTimeout > TIMEOUT_MAX * 60) 
	screen.xTimeout = TIMEOUT_MAX * 60;
    
  if (screen.xTimeout != screen.newTimeout)
    {
      screen.origTimeout = screen.newTimeout = screen.xTimeout;
      screen.saverTimeout = screen.xTimeout / 60;
    }  
  
  /* set the value of the saver widgets */
  XmToggleButtonGadgetSetState(screen.saverOn, screen.newTimeout ? True : False, False);
  XmToggleButtonGadgetSetState(screen.saverOff, !screen.newTimeout ? True : False, False);
  
  if (!screen.saverTimeout)
    screen.saverTimeout = screen.saverTimeout_xrdb;
  
  XtVaSetValues(screen.timeOutScale, XmNvalue, screen.saverTimeout, NULL);  
  
  /* make saver widgets insensitive if screen saver is off */
  if (XmToggleButtonGadgetGetState(screen.saverOff))
    {
      XtSetSensitive(screen.timeOutLabel, False);
      XtSetSensitive(screen.timeOutScale, False);
      XtSetSensitive(screen.saverMinutesLabel, False);
    }
  /* make saver widgets sensitive if screen saver is on */
  if (XmToggleButtonGadgetGetState(screen.saverOn))
    {
      XtSetSensitive(screen.timeOutLabel, True);
      XtSetSensitive(screen.timeOutScale, True);
      XtSetSensitive(screen.saverMinutesLabel, True);
    }
  
  
  /* run the current screen saver only if the useSaversOn toggle is enabled and valid saver exist*/
  if (XmToggleButtonGadgetGetState(screen.useSaversOn) && savers.saverCount && !savers.saverstate)
    {
      XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);  
      savers.saverstate = _DtSaverStart(style.display, &screen.saverArea, 1,
				   savers.current_saver, style.screenDialog);
      string = XmStringCreateLocalized (savers.current_saverDesc);
      XtVaSetValues(screen.prevLabel, XmNlabelString, string, NULL);
      XmStringFree(string);	    
    }
    
}

/*****************************************/
/* timeOutvalueChangedCB                 */
/* set the timeout to be the minimum of  */
/* the lock scale and timeout scale      */
/*****************************************/
static void 
timeOutvalueChangedCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n;
    Arg              args[MAX_ARGS];

    n=0;
    XtSetArg(args[n], XmNvalue, &screen.saverTimeout); n++;
    XtGetValues(screen.timeOutScale, args, n);

    n=0;
    XtSetArg(args[n], XmNvalue, &screen.lockTimeout); n++;
    XtGetValues(screen.lockOutScale, args, n);

    /* set newTimeout to the min value of lock scale and saver scale */
    /* unless either screen saver or lock is off */
    if (!XmToggleButtonGadgetGetState(screen.lockOn))
	  screen.newTimeout = screen.saverTimeout * 60;
    else  
      if (!XmToggleButtonGadgetGetState(screen.saverOn))
	screen.newTimeout = screen.lockTimeout * 60;
      else
	screen.newTimeout = MIN(screen.saverTimeout, screen.lockTimeout) * 60;
	
    /* register new timeout with X */
     XSetScreenSaver(style.display, screen.newTimeout, 
         screen.interval, screen.new_blanking, screen.exposures);

    screen.systemDefaultFlag = False;
}
/*****************************************/
/* no_svr_ext_timeOutvalueChangedCB      */
/* set the timeout to be the minimum of  */
/* the lock scale and timeout scale      */
/*****************************************/
static void 
no_svr_ext_timeOutvalueChangedCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n;
    Arg              args[MAX_ARGS];

    n=0;
    XtSetArg(args[n], XmNvalue, &screen.saverTimeout); n++;
    XtGetValues(screen.timeOutScale, args, n);

    /* set newTimeout to the value of saver scale */
    screen.newTimeout = screen.saverTimeout * 60;
	
    /* register new timeout with X */
     XSetScreenSaver(style.display, screen.newTimeout, 
         screen.interval, screen.new_blanking, screen.exposures);
    screen.systemDefaultFlag = False;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* saverToggleCB                         */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
saverToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  register int     n;
  Arg              args[MAX_ARGS];
  Boolean          set;
  XmString string;
  XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *)call_data;
  
  set = (Boolean) cb->set;
  
  
  if (w == screen.saverOff && !set )
    {
      return;
    }
  
  if (w == screen.saverOff && set)
    {
      /* Terminate the screen saver that is currently running */		
      if (savers.saverstate)
	{
	  _DtSaverStop(style.display, savers.saverstate);
	  savers.saverstate = NULL;
	  XSync(style.display, False);
	  XClearWindow(style.display, XtWindow(screen.saverArea));
	} 
      XtSetSensitive(screen.saverList, False);
      XtSetSensitive(screen.saverArea, False);
      XtSetSensitive(screen.timeOutLabel, False);
      XtSetSensitive(screen.timeOutScale, False);
      XtSetSensitive(screen.saverMinutesLabel, False);
      XtSetSensitive(screen.cycleTimeLabel, False);
      XtSetSensitive(screen.cycleTimeScale, False);
      XtSetSensitive(screen.cycleMinutesLabel, False);
      /* do not display preview string */
      XtUnmanageChild(screen.prevLabel);  	
      /* set the background color of the saver window to the */
      /* secondary color ID */ 
      XSync(style.display, False);
      XtVaSetValues(screen.saverArea, XmNbackground, savers.bg, NULL);  
      XClearWindow(style.display, XtWindow(screen.saverArea));
      /* set the new timeout for the X server - note that since */
      /* screen saver is disabled, the new timeout is now the value of the */
      /* lock slider or zero if lock is disabled  */
      if (XmToggleButtonGadgetGetState(screen.lockOn))
	screen.newTimeout = screen.lockTimeout * 60;
      else
	screen.newTimeout = 0;
      /* let the session manger know not to run any savers */
      SmNewSaverTime(0); 
    }
  
  
  if (w == screen.saverOn && !set)
    {
      return;
    }
  
  if (w == screen.saverOn && set)
    {
      XtSetSensitive(screen.saverList, True);
      XtSetSensitive(screen.saverArea, True);
      XtSetSensitive(screen.timeOutLabel, True);
      XtSetSensitive(screen.timeOutScale, True);
      XtSetSensitive(screen.saverMinutesLabel , True);
      if (savers.selsaverCount > 1)
	{
	  XtSetSensitive(screen.cycleTimeLabel, True);
	  XtSetSensitive(screen.cycleTimeScale, True);
	  XtSetSensitive(screen.cycleMinutesLabel, True );
	}
      else
	{
	  XtSetSensitive(screen.cycleTimeLabel, False);
	  XtSetSensitive(screen.cycleTimeScale, False);
	  XtSetSensitive(screen.cycleMinutesLabel, False); 
	}
      
      if (savers.saverCount)
	{
	  /* display the preview label */
	  XtManageChild(screen.prevLabel);  
	  /* Restart the screen saver that is is currently selected */	
	  if (!savers.saverstate)
	    {
	      XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);    
	      savers.saverstate = _DtSaverStart(style.display, 
                      &screen.saverArea, 1, savers.current_saver,
                      style.screenDialog);
	    }
	  /* set the new timeout for the X server - note that since */
	  /* screen saver is enabled, the new timeout is now the value of the */
	  /* saver slider or the minimum of the timeout sliders */
	}
      if (!XmToggleButtonGadgetGetState(screen.lockOn))
	screen.newTimeout = screen.saverTimeout * 60;
      else
	screen.newTimeout = MIN(screen.saverTimeout, screen.lockTimeout) * 60;
      /* let the session manger know to run savers */
      SmNewSaverTime(screen.newTimeout); 
    }
  
  XSetScreenSaver(style.display, screen.newTimeout, 
		  screen.interval, screen.new_blanking, screen.exposures);
  
  screen.systemDefaultFlag = False;
}
/*+++++++++++++++++++++++++++++++++++++++*/
/* no_svr_ext_saverToggleCB              */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
no_svr_ext_saverToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n;
    Arg              args[MAX_ARGS];
    Boolean          set;
    XmString string;
    XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *)call_data;

    set = (Boolean) cb->set;

 
    if (w == screen.saverOff && !set )
      {
      return;
      }

    if (w == screen.saverOff && set)
      {
	XtSetSensitive(screen.timeOutLabel, False);
	XtSetSensitive(screen.timeOutScale, False);
	XtSetSensitive(screen.saverMinutesLabel, False);
	/* set the new timeout for the X server - note that since */
	/* screen saver is disabled, the new timeout is now zero */
	screen.newTimeout = 0;
      }
          
    if (w == screen.saverOn && !set)
      {
	return;
      }
    
    if (w == screen.saverOn && set)
      {
	XtSetSensitive(screen.timeOutLabel, True);
	XtSetSensitive(screen.timeOutScale, True);
	XtSetSensitive(screen.saverMinutesLabel , True);
	/* register the new timeout with the X server - note that since */
	/* screen saver is enabled, the new timeout is now the value of the */
	/* saver slider */
	screen.newTimeout = screen.saverTimeout * 60;
      }
      
    XSetScreenSaver(style.display, screen.newTimeout, 
			screen.interval, screen.new_blanking, screen.exposures);

    screen.systemDefaultFlag = False;
  }

/*+++++++++++++++++++++++++++++++++++++++*/
/* lock ToggleCB                         */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
lockToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    register int     n;
    Arg              args[MAX_ARGS];
    Boolean          set;
    XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *)call_data;
    
    set = (Boolean) cb->set;

 
    if (w == screen.lockOff && !set )
      {
      return;
      }

    if (w == screen.lockOff && set)
      {
      	XtSetSensitive(screen.lockOutLabel, False);
	XtSetSensitive(screen.lockOutScale, False);
	XtSetSensitive(screen.lockMinutesLabel, False);

	/* register the new timeout with the X server - note that since */
	/* lock is disabled, the new timeout is now the value of the */
	/* saver slider or zero if saver is disabled */
	if (XmToggleButtonGadgetGetState(screen.saverOn))
	  screen.newTimeout = screen.saverTimeout * 60;
	else
	  screen.newTimeout = 0;

	XSetScreenSaver(style.display, screen.newTimeout, 
			screen.interval, screen.new_blanking, screen.exposures);
      }

    if (w == screen.lockOn && !set)
      {
      return;
      }

    if (w == screen.lockOn && set)
      {
	XtSetSensitive(screen.lockOutLabel, True);
	XtSetSensitive(screen.lockOutScale, True);
	XtSetSensitive(screen.lockMinutesLabel, True);

	/* register the new timeout with the X server - note that since */
	/* lock is disabled, the new timeout is now the value of the */
	/* lock slider or the minimum of the two timeout sliders */
	if (!XmToggleButtonGadgetGetState(screen.saverOn))
	  screen.newTimeout = screen.lockTimeout * 60;
	else
	  screen.newTimeout = MIN(screen.saverTimeout, screen.lockTimeout) * 60;

	XSetScreenSaver(style.display, screen.newTimeout, 
			screen.interval, screen.new_blanking, screen.exposures);
      }
    screen.systemDefaultFlag = False;

}

/*+++++++++++++++++++++++++++++++++++++++*/
/* saversToggleCB                        */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
saversToggleCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  register int     n;
  Arg              args[MAX_ARGS];
  Boolean          set;
  XmString string;
  XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *)call_data;
  
  set = (Boolean) cb->set;

  if (w == screen.useSaversOff && !set )
      {
      return;
      }

    if (w == screen.useSaversOff && set)
      {
	/* Terminate the screen saver that is currently running */		
	if (savers.saverstate)
	  {
	    _DtSaverStop(style.display, savers.saverstate);
	    savers.saverstate = NULL;
	    XSync(style.display, False);
	    XClearWindow(style.display, XtWindow(screen.saverArea));
	  } 
	
	XtSetSensitive(screen.saverList, False);
	XtSetSensitive(screen.saverArea, False);
	XtSetSensitive(screen.cycleTimeLabel, False);
	XtSetSensitive(screen.cycleTimeScale, False);
	XtSetSensitive(screen.cycleMinutesLabel, False);
	/* do not display preview string */
	XtUnmanageChild(screen.prevLabel);  	
	/* set the background color of the saver window to the */
	/* secondary color ID */
	XSync(style.display, False);
	XtVaSetValues(screen.saverArea, XmNbackground, savers.bg, NULL);  
	XClearWindow(style.display, XtWindow(screen.saverArea));
	/* let the session manger know not to run any savers */
	SmNewSaverTime(0);
      }

    if (w == screen.useSaversOn && !set)
      {
      return;
      }

    if (w == screen.useSaversOn && set)
      {
	XtSetSensitive(screen.saverList, True);
	XtSetSensitive(screen.saverArea, True);
	if (savers.selsaverCount > 1)
	  {
	    XtSetSensitive(screen.cycleTimeLabel, True);
	    XtSetSensitive(screen.cycleTimeScale, True);
	    XtSetSensitive(screen.cycleMinutesLabel, True); 
	  }
	if (savers.saverCount)
	  {
	    /* display the preview label */
	    XtManageChild(screen.prevLabel);  
	    /* Restart the screen saver that is is currently selected */	
	    if (!savers.saverstate)
	      {
		XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);    
		savers.saverstate = _DtSaverStart(style.display, 
						  &screen.saverArea, 1, savers.current_saver,                                     style.screenDialog);
	      }
	  }
	/* let the session manger know to run the savers */
	SmNewSaverTime(screen.saverTimeout*60);
	
      }
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
  register int  n;
  Arg           args[MAX_ARGS];
  
  screen.saverTimeout = SAVER_DEFAULT;
  n=0;
  XtSetArg(args[n], XmNvalue, screen.saverTimeout); n++;
  XtSetValues(screen.timeOutScale, args, n);
  
  screen.lockTimeout = LOCK_DEFAULT;
  n=0;
  XtSetArg(args[n], XmNvalue, screen.lockTimeout); n++;
  XtSetValues(screen.lockOutScale, args, n);
  XmToggleButtonGadgetSetState(screen.lockOff, True, True);
  
  screen.cycleTimeout = CYCLE_DEFAULT;
  n=0;
  XtSetArg(args[n], XmNvalue, screen.cycleTimeout); n++;
  XtSetValues(screen.cycleTimeScale, args, n);
  XmToggleButtonGadgetSetState(screen.saverOn, True, True);
  
  screen.newTimeout = MIN(SAVER_DEFAULT, LOCK_DEFAULT) * 60; 
  /* register new timeout with X */
  XSetScreenSaver(style.display, screen.newTimeout, 
		  screen.interval, screen.new_blanking, screen.exposures);
  screen.systemDefaultFlag = True;
}

/*+++++++++++++++++++++++++++++++++++++++*/
/* no_svr_ext_systemDefaultCB            */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
no_svr_ext_systemDefaultCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  register int  n;
  Arg           args[MAX_ARGS];

  screen.saverTimeout = SAVER_DEFAULT;
  n=0;
  XtSetArg(args[n], XmNvalue, screen.saverTimeout); n++;
  XtSetValues(screen.timeOutScale, args, n);
  
  screen.cycleTimeout = CYCLE_DEFAULT;
  n=0;
  XtSetArg(args[n], XmNvalue, screen.cycleTimeout); n++;
  XtSetValues(screen.cycleTimeScale, args, n);

  XmToggleButtonGadgetSetState(screen.saverOn, True, True);
  XmToggleButtonGadgetSetState(screen.useSaversOn, True, True);

  screen.newTimeout = SAVER_DEFAULT * 60;
  /* register new timeout with X */
  XSetScreenSaver(style.display, screen.newTimeout, 
		  screen.interval, screen.new_blanking, screen.exposures);
  
  screen.systemDefaultFlag = True;
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
  register int        i, n, m;
  Arg                 args[MAX_ARGS];
  Boolean             sel_matched = False;
  Boolean             lockset;
  Boolean             saverset;
  Boolean             flag;
  static char         screenres[48];
  int                 mintime;
  
  DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;
  
  switch (cb->button_position)
    {
    case OK_BUTTON:
      
      XtUnmanageChild (w);
      /* terminate the saver that is currently running */
      if (savers.saverstate)
	{
	  _DtSaverStop(style.display, savers.saverstate);
	  savers.saverstate = NULL;
	}
      
      /* write saver timeout resource to xrdb to save scale value */
      /* when screen saver is turned off */
      saverset = XmToggleButtonGadgetGetState(screen.saverOn);
      if (!saverset)
	{
	  sprintf (screenres, "Dtstyle*timeoutScale: %d\n", screen.saverTimeout);
	  _DtAddToResource (style.display, screenres);
	}
      
      /* write lock timeout resource to xrdb to save scale value */
      /* when lock is turned off */
      lockset = XmToggleButtonGadgetGetState(screen.lockOn);
      if (!lockset)
	{
	  sprintf (screenres, "Dtstyle*lockoutScale: %d\n", screen.lockTimeout);
	  _DtAddToResource (style.display, screenres);
	}

      /* if sendSettings is true send message to Session Manager */
      if(style.smState.smSendSettings)
	{
	  if(screen.systemDefaultFlag)
	    {
	      SmDefaultScreenSettings();
	    }
	  else
	    {
	      
	      SmNewScreenSettings(saverset||lockset ? screen.newTimeout : 0, 
				  screen.new_blanking,    
				  screen.interval,        
				  screen.exposures);
	    }
	}
      
      

      if (savers.saverCount)
	{
	  savers.selsaverActionNames = (char **) XtMalloc(sizeof(char *) * 
							  savers.selsaverCount); 
	  
	  for (i=0; i<savers.selsaverCount; i++)
	    {
	      char *tmp = savers.saverActionNames[savers.selPositions[i] - 1];
	      savers.selsaverActionNames[i] = (char *) XtMalloc(strlen(tmp) + 1);
	      strcpy(savers.selsaverActionNames[i], tmp);
	    }			
            
	  if (savers.selsaversList != NULL)
	    XtFree((char*) savers.selsaversList);	
	  
	  savers.selsaversList = build_selectedList(savers.selsaverActionNames,
						savers.selsaverCount);
	}
        
      else 
	savers.selsaversList = NULL;

      /* save the selected positions for later use */
      savers.orig_selsaverCount = savers.selsaverCount;
      if (savers.origSelPositions != NULL)
	XtFree((char *) savers.origSelPositions);      
      savers.origSelPositions = (int *) XtMalloc(sizeof(int) * savers.orig_selsaverCount);
      for (i=0; i<savers.orig_selsaverCount; i++)
	savers.origSelPositions[i] = savers.selPositions[i];

      /* free the allocated selected savers */
      free_saverList(savers.selsaverActionNames, savers.selsaverCount); 
      savers.selsaverActionNames = NULL;

      XtVaGetValues(screen.cycleTimeScale, XmNvalue, &screen.cycleTimeout, NULL);
      
      /* send new saver time, lock time and time per saver to SM if they have changed, */
      /* flag value indicates which value has changed */
            
      SmNewSaverSettings(saverset ? screen.saverTimeout*60 : 0, 
			 lockset ? screen.lockTimeout*60 : 0, 
			 screen.cycleTimeout*60,
			 savers.selsaversList);
      
      style.smState.smSaverTimeout = saverset ? screen.saverTimeout*60 : 0;
      style.smState.smLockTimeout = lockset ? screen.lockTimeout*60 : 0;
      style.smState.smCycleTimeout = screen.cycleTimeout*60;
      style.smSaver.saverList = savers.selsaversList;
      
      screen.origTimeout = screen.newTimeout;
      XSetScreenSaver(style.display, style.smState.smSaverTimeout,
		      style.smState.smCycleTimeout,
		      screen.new_blanking,
		      screen.exposures); 
     
      break;
      
    case CANCEL_BUTTON:
      XtUnmanageChild(w);
            
      /* initialize the value of savertimeout and locktimeout and cycle time to the */
      /* current state or to the values retrieved from xrdb if the corresponding */
      /* initial state is zero */
      if (style.smState.smSaverTimeout)
	screen.saverTimeout = style.smState.smSaverTimeout / 60;
      else
	screen.saverTimeout = screen.saverTimeout_xrdb;
      
      if (style.smState.smLockTimeout)
	screen.lockTimeout = style.smState.smLockTimeout / 60;
      else
	screen.lockTimeout = screen.lockTimeout_xrdb;
      
      if (style.smState.smCycleTimeout)
	screen.cycleTimeout = style.smState.smCycleTimeout / 60;
      else 
	screen.cycleTimeout = 0;
      
      
      /* reset the value of the scales */
      XtVaSetValues(screen.timeOutScale, XmNvalue, screen.saverTimeout, NULL);
      XtVaSetValues(screen.lockOutScale, XmNvalue, screen.lockTimeout, NULL);
      XtVaSetValues(screen.cycleTimeScale, XmNvalue, screen.cycleTimeout , NULL);  
      
      /* reset the value of the saver toggles */
      XmToggleButtonGadgetSetState(screen.saverOn, style.smState.smSaverTimeout  ? True : False, True);
      XmToggleButtonGadgetSetState(screen.saverOff, !style.smState.smSaverTimeout  ? True : False, True);
      XmToggleButtonGadgetSetState(screen.lockOn, style.smState.smLockTimeout  ? True : False, True);
      XmToggleButtonGadgetSetState(screen.lockOff, !style.smState.smLockTimeout  ? True : False, True);
      
      /* reset the X timeout */
      screen.newTimeout = screen.origTimeout;
      XSetScreenSaver(style.display, screen.origTimeout, screen.interval, 
		      screen.blanking, screen.exposures);
               
      /* if timeMismatch is true, set the timeout to xTimeout so that the next time */
      /* the interface is mapped the correct timeouts show up */
      if (screen.timeMismatch)
	XSetScreenSaver(style.display, screen.xTimeout, screen.interval, 
			screen.blanking,screen.exposures);
      
      
      if (savers.saverCount)
	  {
	    /* reset the list selection back to the current state */
	    /* first deselect all items */
	    XmListDeselectAllItems(screen.saverList);
	    for (i=0; i<savers.orig_selsaverCount; i++)
	      XmListSelectPos(screen.saverList, savers.origSelPositions[i], False);
	    /* set the current saver to be the first saver in the selected list */
	    savers.current_saver = savers.saverActionNames[savers.origSelPositions[0] - 1];
	    savers.current_saverDesc = savers.saverActionDesc[savers.origSelPositions[0] - 1];
	    savers.current_position = savers.origSelPositions[0];
	  }

      /* Need to make sure the savers.selPositions array reflects the accurate
         current selections.  If the user invokes the Screen dialog, then cancels,
         then reinvokes and OKs without making any changes to the selections, the
         savers.selPositions array is assumed to contain the accurate selections.
         Unless we reset it here to the origSelPositions, it may be inaccurate.
      */

      XtFree((char *) savers.selPositions);
      savers.selPositions = (int *) XtMalloc(sizeof(int) * savers.orig_selsaverCount);

      for (i=0; i<savers.orig_selsaverCount; i++)
        savers.selPositions[i] = savers.origSelPositions[i];

      savers.selsaverCount = savers.orig_selsaverCount;

      break;


      break;
      
      
    case HELP_BUTTON:
      XtCallCallbacks(style.screenDialog, XmNhelpCallback, (XtPointer)NULL);
      break;
      
    default:
      break;
    }
}  

/*+++++++++++++++++++++++++++++++++++++++*/
/* no_svr_ext_ButtonCB                   */
/* callback for PushButtons in DialogBox */
/*+++++++++++++++++++++++++++++++++++++++*/
static void 
no_svr_ext_ButtonCB(
		     Widget w,
		     XtPointer client_data,
		     XtPointer call_data )
{
  register int        i, n, m;
  Arg                 args[MAX_ARGS];
  Boolean             sel_matched = False;
  Boolean             saverset;
  Boolean             flag;
  static char         screenres[48];
  int                 mintime;
  
  DtDialogBoxCallbackStruct *cb = (DtDialogBoxCallbackStruct *) call_data;
  
  switch (cb->button_position)
    {
    case OK_BUTTON:
      
      XtUnmanageChild (w);
      /* terminate the saver that is currently running */
      if (savers.saverstate)
	{
	  _DtSaverStop(style.display, savers.saverstate);
	  savers.saverstate = NULL;
	}
      
      /* write saver timeout resource to xrdb to save scale value */
      /* when screen saver is turned off */
      saverset = XmToggleButtonGadgetGetState(screen.saverOn);
      if (!saverset)
	{
	  sprintf (screenres, "Dtstyle*timeoutScale: %d\n", screen.saverTimeout);
	  _DtAddToResource (style.display, screenres);
	}
      
      /* if sendSettings is true send message to Session Manager */
      if(style.smState.smSendSettings)
	{
	  if(screen.systemDefaultFlag)
	    {
	      SmDefaultScreenSettings();
	    }
	  else
	    {
	      SmNewScreenSettings(saverset ? screen.newTimeout : 0, 
				  screen.new_blanking,    
				  screen.interval,        
				  screen.exposures);
	    }
	}
      


      if (savers.saverCount)
	{
	  savers.selsaverActionNames = (char **) XtMalloc(sizeof(char *) * 
							  savers.selsaverCount); 
	  
	  for (i=0; i<savers.selsaverCount; i++)
	    {
	      char *tmp = savers.saverActionNames[savers.selPositions[i] - 1];
	      savers.selsaverActionNames[i] = (char *) XtMalloc(strlen(tmp) + 1);
	      strcpy(savers.selsaverActionNames[i], tmp);
	    }			
            
	  if (savers.selsaversList != NULL)
	    XtFree((char*) savers.selsaversList);	
	  
	  savers.selsaversList = build_selectedList(savers.selsaverActionNames,
						savers.selsaverCount);
	}
        
      else 
	savers.selsaversList = NULL;

      /* save the selected positions for later use */
      savers.orig_selsaverCount = savers.selsaverCount;
      if (savers.origSelPositions != NULL)
	XtFree((char *) savers.origSelPositions);      
      savers.origSelPositions = (int *) XtMalloc(sizeof(int) * savers.orig_selsaverCount);
      for (i=0; i<savers.orig_selsaverCount; i++)
	savers.origSelPositions[i] = savers.selPositions[i];

      /* free the allocated selected savers */
      free_saverList(savers.selsaverActionNames, savers.selsaverCount); 
      savers.selsaverActionNames = NULL;

      XtVaGetValues(screen.cycleTimeScale, XmNvalue, &screen.cycleTimeout, NULL);
      
      /* send new saver time, lock time and time per saver to SM if they have changed, */
      /* flag value indicates which value has changed */
      /* send zero to the session manger if no savers selected */
      
      if(!XmToggleButtonGadgetGetState(screen.useSaversOn))
	saverset = False;
      else
	saverset = True;
      SmNewSaverSettings(saverset ? screen.saverTimeout*60 : 0, 
			 style.smState.smLockTimeout, 
			 screen.cycleTimeout*60,
			 savers.selsaversList);
      
      style.smState.smSaverTimeout = saverset ? screen.saverTimeout*60 : 0;
      style.smState.smCycleTimeout = screen.cycleTimeout*60;
      style.smSaver.saverList = savers.selsaversList;
  
      screen.origTimeout = screen.newTimeout;
      XSetScreenSaver(style.display, style.smState.smSaverTimeout,
		      style.smState.smCycleTimeout,
		      screen.new_blanking,
		      screen.exposures); 
      
      break;
      
    case CANCEL_BUTTON:
      XtUnmanageChild(w);
      
      /* initialize the value of savertimeout and cycle time to the */
      /* original value */
      
      if (screen.origTimeout)
        screen.saverTimeout = screen.origTimeout / 60;
      else
      screen.saverTimeout = screen.saverTimeout_xrdb;
      
      
      if (style.smState.smCycleTimeout)
	screen.cycleTimeout = style.smState.smCycleTimeout / 60;
      else 
	screen.cycleTimeout = 0;
      
      /* reset the value of the scales */
      XtVaSetValues(screen.timeOutScale, XmNvalue, screen.saverTimeout, NULL);
      XtVaSetValues(screen.cycleTimeScale, XmNvalue, screen.cycleTimeout , NULL);  
      
      /* reset the value of the saver toggles */
      XmToggleButtonGadgetSetState(screen.saverOn, screen.origTimeout  ? True : False, True);
      XmToggleButtonGadgetSetState(screen.saverOff, !screen.origTimeout  ? True : False, True);

      /* reset the value of the locking radio box toggles */
      if (style.smState.smSaverTimeout)
	{
	  XmToggleButtonGadgetSetState(screen.useSaversOn, True, False);
	  XmToggleButtonGadgetSetState(screen.useSaversOff, False, False);
	  XtSetSensitive(screen.saverList, True);
	  XtSetSensitive(screen.saverArea, True);
	  if (savers.selsaverCount > 1)
	    {
	      XtSetSensitive(screen.cycleTimeLabel, True);
	      XtSetSensitive(screen.cycleTimeScale, True);
	      XtSetSensitive(screen.cycleMinutesLabel, True); 
	    }
	  if (savers.saverCount)
	      /* display the preview label */
	      XtManageChild(screen.prevLabel);  

	  /* let the session manger know to run the savers */
	  SmNewSaverTime(screen.saverTimeout*60);
	}
      else
	{
	  XmToggleButtonGadgetSetState(screen.useSaversOff, True, False);
	  XmToggleButtonGadgetSetState(screen.useSaversOn, False, False);
	  XtSetSensitive(screen.saverList, False);
	  XtSetSensitive(screen.saverArea, False);
	  XtSetSensitive(screen.cycleTimeLabel, False);
	  XtSetSensitive(screen.cycleTimeScale, False);
	  XtSetSensitive(screen.cycleMinutesLabel, False);
	  /* do not display preview string */
	  XtUnmanageChild(screen.prevLabel);  	
	  /* set the background color of the saver window to the */
	  /* secondary color ID */
	  XSync(style.display, False);
	  XtVaSetValues(screen.saverArea, XmNbackground, savers.bg, NULL);  
	  XClearWindow(style.display, XtWindow(screen.saverArea));
	  /* let the session manger know not to run any savers */
	  SmNewSaverTime(0);
	}
	  
      /* reset the X timeout */
      screen.newTimeout = screen.origTimeout;
      XSetScreenSaver(style.display, screen.origTimeout, screen.interval, 
		      screen.blanking,screen.exposures);
      

      if (savers.saverCount)
	{
	/* reset the list selection back to the current state */
	  /* first deselect all items */
	  XmListDeselectAllItems(screen.saverList);
	  for (i=0; i<savers.orig_selsaverCount; i++)
	    XmListSelectPos(screen.saverList, savers.origSelPositions[i], False);
	  /* set the current saver to be the first saver in the selected list */
	  savers.current_saver = savers.saverActionNames[savers.origSelPositions[0] - 1];
	  savers.current_saverDesc = savers.saverActionDesc[savers.origSelPositions[0] - 1];
	  savers.current_position = savers.origSelPositions[0];
	}

      /* Need to make sure the savers.selPositions array reflects the accurate 
	 current selections.  If the user invokes the Screen dialog, then cancels,
	 then reinvokes and OKs without making any changes to the selections, the
	 savers.selPositions array is assumed to contain the accurate selections.
	 Unless we reset it here to the origSelPositions, it may be inaccurate.
      */

      XtFree((char *) savers.selPositions);
      if (savers.orig_selsaverCount > 0) {
	savers.selPositions =
	  (int *) XtMalloc(sizeof(int) * savers.orig_selsaverCount);

	for (i=0; i<savers.orig_selsaverCount; i++)
	  savers.selPositions[i] = savers.origSelPositions[i];
      }
	
      savers.selsaverCount = savers.orig_selsaverCount;

      break;
      
      
    case HELP_BUTTON:
      XtCallCallbacks(style.screenDialog, XmNhelpCallback, (XtPointer)NULL);
      break;
      
    default:
      break;
    }
}

/************************************************************************/
/* restoreScreen()                                                      */
/* restore any state information saved with saveScreen.                 */ 
/* This is called from restoreSession with the application              */ 
/* shell and the special xrm database retrieved for restore.            */
/************************************************************************/
void 
restoreScreen(
        Widget shell,
        XrmDatabase db )
{
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    xrm_name [0] = XrmStringToQuark ("Screen");
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
      popup_screenBB(shell);
}

/*******************************************************************************/
/* saveScreen()                                                                */
/* This routine will write out to the passed file descriptor any state         */ 
/* information this dialog needs.  It is called from saveSessionCB with the    */
/* file already opened.                                                        */
/* All information is saved in xrm format.  There is no restriction            */
/* on what can be saved.  It doesn't have to be defined or be part of any      */
/* widget or Xt definition.  Just name and save it here and recover it in      */
/* restoreBackdrop.  The suggested minimum is whether you are mapped, and your */
/* location.                                                                   */
/*******************************************************************************/
void 
saveScreen(
        int fd )
{
    Position x,y;
    char *bufr = style.tmpBigStr;     /* size=[1024], make bigger if needed */
    XmVendorShellExtObject  vendorExt;
    XmWidgetExtData         extData;

    if (style.screenDialog != NULL) 
    {
        if (XtIsManaged(style.screenDialog))
            sprintf(bufr, "*Screen.ismapped: True\n");
        else
            sprintf(bufr, "*Screen.ismapped: False\n");

	/* Get and write out the geometry info for our Window */
	x = XtX(XtParent(style.screenDialog));
	y = XtY(XtParent(style.screenDialog));

        /* Modify x & y to take into account window mgr frames */
        /* This is pretty bogus, but I don't know a better way to do it. */
        extData = _XmGetWidgetExtData(style.shell, XmSHELL_EXTENSION);
        vendorExt = (XmVendorShellExtObject)extData->widget;
        x -= vendorExt->vendor.xOffset;
        y -= vendorExt->vendor.yOffset;

	sprintf(bufr, "%s*Screen.x: %d\n", bufr, x);
	sprintf(bufr, "%s*Screen.y: %d\n", bufr, y);
       
	write (fd, bufr, strlen(bufr));
    }
}


/************************************************************************/
/*   MakeListStrings()                                                  */
/*           Make XmStrings from the saver names, to pass into list.    */ 
/************************************************************************/
static XmString * 
MakeListStrings( char ** list )
{
    register int   i;
    XmString      *xmList;
    
    xmList = (XmString *) XtMalloc(savers.saverCount * sizeof(XmString));

    for (i = 0; i < savers.saverCount; i++)
    {
        xmList[i] = XmStringCreateLocalized (list[i]);
    }

    return (xmList);
}


/************************************************************************/
/*   FreeListStrings()                                                  */
/************************************************************************/

static void 
FreeListStrings(XmString *xmlist,
		int count)
{
  int         i;
  
  for (i = 0; i < count; i++)
    {
      if (xmlist[i]) 
	XmStringFree(xmlist[i]);
    }
  XtFree ((char *)xmlist);
}


/************************************************************************/
/* ListCB()                                                             */
/* Get the lastly selected Screen saver client running in the preview   */
/* area. For a multiple selection by dragging, the last item in the     */
/* selection is the client that is previewed.                           */
/************************************************************************/
static void 
ListCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XmListCallbackStruct  *cb = (XmListCallbackStruct *)call_data;
  register int           i;
  Boolean                runsaver = False;
  XmString               string;


  /* If no items are left in the selection (i.e. the last selected item */
  /* was deselected) then  force the previously selected item to remain */
  /* selected */
    
  if (cb->selected_item_count == 0)
    {
      XmListSelectPos(screen.saverList, cb->item_position, False);
      savers.selsaverCount = 1;
      if (savers.selPositions != NULL)
	XtFree ((char *) savers.selPositions);
      savers.selPositions = (int *) XtMalloc(sizeof(int));
      savers.selPositions[0] = cb->item_position;
      savers.current_saver = savers.saverActionNames[savers.selPositions[0] - 1];
      savers.current_saverDesc = savers.saverActionDesc[savers.selPositions[0] - 1];
      return;
    }
 
  /*  make a copy of the selected item positions for later use */
  
  if (savers.selPositions != NULL)
    XtFree ((char *) savers.selPositions);
  
  savers.selPositions = (int *) XtMalloc(sizeof(int) * cb->selected_item_count);
  for (i=0; i< cb->selected_item_count; i++)
    {
      savers.selPositions[i] = cb->selected_item_positions[i];
    }
  
  savers.selsaverCount = cb->selected_item_count;
  if (savers.selsaverCount < 2) 
    {
      XtSetSensitive(screen.cycleTimeScale, False);
      XtSetSensitive(screen.cycleTimeLabel, False);
      XtSetSensitive(screen.cycleMinutesLabel, False);
    }
  else 
    if (savers.previous_selsaverCount < 2)
      {
	XtSetSensitive(screen.cycleTimeScale, True);
	XtSetSensitive(screen.cycleTimeLabel, True);
	XtSetSensitive(screen.cycleMinutesLabel, True);
      }

  savers.previous_selsaverCount = savers.selsaverCount;
      
  /* Find out if the item is selected or deselected.  When an item */
  /* is deselected item_position is set to the deselected item which has */
  /* the keyboard focus. However, the deselected screen saver must not be  */
  /* previewed. Therfore before a screen saver is started, its position must */
  /* exist in the selected_item_positions array. If not then just return without */
  /* altering the current (previewed) saver unless the deselected saver was the */
  /* current saver. In that case kill the current saver and preview the last */
  /* item from the selected list. */
  for (i=0; i<cb->selected_item_count; i++)
    {
      if(cb->selected_item_positions[i] == cb->item_position)
	runsaver=True;
    }
  
  if (!runsaver)
    {
      i = cb->selected_item_positions[(cb->selected_item_count-1)];
      if (savers.current_position == cb->item_position)
	{
	    if (savers.saverstate)
	      {
		/* Terminate the screen saver that is currently running */
		_DtSaverStop(style.display, savers.saverstate);
		XSync(style.display, False);
		savers.saverstate = NULL;
	      }
	    savers.current_saver = savers.saverActionNames[i-1];
	    savers.current_saverDesc = savers.saverActionDesc[i-1];
	    savers.current_position = i;
	    XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);   
	    savers.saverstate = _DtSaverStart(style.display, &screen.saverArea,
                                  1, savers.current_saver, style.screenDialog);
	    string = XmStringCreateLocalized (savers.current_saverDesc);
	    XtVaSetValues(screen.prevLabel, XmNlabelString, string, NULL);
	    XmStringFree(string);	    
 	  }
      return;
    }
  
  /* The item position returned is the selcted item */ 
  /* Start screen saver. _DtSaverStop() must be called to terminate the */
  /* screen saver. */
  if (savers.saverstate)
    {
      /* Terminate the screen saver that is currently running */
      _DtSaverStop(style.display, savers.saverstate);
      XSync(style.display, False);
      savers.saverstate = NULL;
    }
  
  XSync(style.display, False);                
  XClearWindow(style.display, XtWindow(screen.saverArea));
  XSync(style.display, False);
  XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);   
  
  savers.current_position = cb->item_position;  
  savers.current_saver = savers.saverActionNames[savers.current_position - 1];
  savers.current_saverDesc = savers.saverActionDesc[savers.current_position - 1];
  savers.saverstate = _DtSaverStart(style.display, &screen.saverArea, 1,
				   savers.current_saver, style.screenDialog);
  /* update the preview label with the current running saver */  
  XtVaSetValues(screen.prevLabel, XmNlabelString, cb->item, NULL);
}



/************************************************************************/
/* IconHandler()                                                        */
/* Kills the currently previewed saver when the user iconifies the      */
/* style manager.  An XtEventHandler.                                   */
/************************************************************************/
static void 
IconHandler(
        Widget shell,
        XtPointer client_data,
        XEvent *event,
        Boolean *dispatch)
{
  
  XmString string;

  if (event->type == UnmapNotify)
    {
      /* terminate the saver that is currently running */
      if (savers.saverstate)
	{
	  _DtSaverStop(style.display, savers.saverstate);
	  savers.saverstate = NULL;
	}
    }
  /* run the current screen saver only if the useSaversOn/saverOn toggle is enabled 
     and valid saver exist and one is not already running*/
  else if (savers.saverCount && event->type == 19 && !savers.saverstate)
    if (style.smState.smLockOnTimeoutStatus)
      {
	if (XmToggleButtonGadgetGetState(screen.saverOn) && savers.saverCount)
	  {
	    XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);  
	    savers.saverstate = _DtSaverStart(style.display, &screen.saverArea, 1,
					      savers.current_saver, style.screenDialog);
	    string = XmStringCreateLocalized (savers.current_saverDesc);
	    XtVaSetValues(screen.prevLabel, XmNlabelString, string, NULL);
	    XmStringFree(string);	    
	  }
      }
    else
      {
	if (XmToggleButtonGadgetGetState(screen.useSaversOn) && savers.saverCount)
	  {
	    XtVaSetValues(screen.saverArea, XmNbackground, savers.black, NULL);  
	    savers.saverstate = _DtSaverStart(style.display, &screen.saverArea, 1,
					      savers.current_saver, style.screenDialog);
	    string = XmStringCreateLocalized (savers.current_saverDesc);
	    XtVaSetValues(screen.prevLabel, XmNlabelString, string, NULL);
	    XmStringFree(string);	    
	  }
      }
}


/************************************************************************/
/* UnmapCB()                                                            */
/* Kills the currently previewed saver when the user iconifies the      */
/* style manager or unmaps the screen dialog.                           */
/************************************************************************/

static void 
UnmapCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  /* terminate the saver that is currently running */
  if (savers.saverstate)
    {
      _DtSaverStop(style.display, savers.saverstate);
      savers.saverstate = NULL;
    }
  
}









