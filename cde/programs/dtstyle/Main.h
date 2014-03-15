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
/* $XConsortium: Main.h /main/8 1996/10/15 08:02:10 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Main.h
 **
 **   Project:     SUI
 **
 **   Description: defines and typedefs for DtStyle 
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

#ifndef _main_h
#define _main_h
 
/* 
 *  #include statements 
 */

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include "Font.h"
#include <Dt/DtP.h>
#include <Dt/SessionM.h>
#include <Dt/SessionP.h>

/*
 * Copied from Xm/BaseClassI.h
 */
extern XmWidgetExtData _XmGetWidgetExtData( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */

/* 
 *  #define statements 
 */

#define MAX_ARGS         20
#define CMPSTR(str)      XmStringCreateLocalized (str)
#define XMCLASS          "Dtstyle"
#define DIALOG_MWM_FUNC  MWM_FUNC_MOVE | MWM_FUNC_CLOSE 

#define COLOR    "Color"
#define FONT     "Font"
#define BACKDROP "Backdrop"
#define KEYBOARD "Keyboard"
#define MOUSE    "Mouse"
#define BEEP     "Beep"
#define SCREEN   "Screen"
#define DTWM     "Dtwm"
#define STARTUP  "Startup"
#define I18N     "I18N"

#define COLOR_ICON    "DtColor"
#define FONT_ICON     "DtFont"
#define BACKDROP_ICON "DtBkdrp"
#define KEYBOARD_ICON "DtKeybd"
#define MOUSE_ICON    "DtMouse"
#define BEEP_ICON     "DtBeep"
#define SCREEN_ICON   "DtScrn"
#define DTWM_ICON     "DtDtwm"
#define STARTUP_ICON  "DtStart"
#define I18N_ICON     "DtI18N"


/* help files */
#define KB_TRAV_HELP           "vs_keys"
#define BACKDROP_HELP          "vs_back"
#define AUDIO_HELP             "vs_beep"
#define PALETTE_HELP           "vs_colo"
#define COLOR_EDIT_HELP        "vs_dtd"
#define ADD_PALETTE_HELP       "vs_coad"
#define DELETE_PALETTE_HELP    "vs_code"
#define FONT_HELP              "vs_font"
#define HOST_HELP              "vs_host"
#define KEYBOARD_HELP          "vs_keyb"
#define MOUSE_HELP             "vs_mous"
#define PASSWORD_HELP          "vs_pass"
#define SCREEN_HELP            "vs_scre"
#define STARTUP_HELP           "vs_star"
#define I18N_HELP              "vs_i18n"
#define STYLE_HELP             "vs_wind"

/* DialogBoxDialog label #defines */
#define OK_BUTTON             1
#define CANCEL_BUTTON         2
#define HELP_BUTTON           3
#define NUM_LABELS            3

/* geometry */
#define LB_MARGIN_HEIGHT      2
#define LB_MARGIN_WIDTH       12
#define SCALE_HIGHLIGHT_THICKNESS  (Dimension) 2

/* 
 * typedef statements 
 */

typedef struct {
    int        numFonts;
    XmFontList userFont,
               systemFont;
    String     userFontStr,
               systemFontStr;
    Fontset    fontChoice[10];
    String     session,
               backdropDir,
               paletteDir,
               timeoutScale, lockoutScale;
    Boolean    writeXrdbImmediate;
    Boolean    writeXrdbColors;
    String     componentList;
    XmStringTable   imServerHosts;
    XmStringTable   preeditType;
    int        pipeTimeOut;
} ApplicationData, *ApplicationDataPtr;

typedef struct {
    Boolean     restoreFlag;
    int         poscnt;
    Arg         posArgs[MAX_ARGS];
} saveRestore;

typedef struct {
    Display         *display;
    Screen          *screen;
    int              screenNum;
    Window           root;
    Colormap         colormap;
    Boolean          useMultiColorIcons;
    Pixel            tgSelectColor,
                     secSelectColor,              /** for editable text bg **/
		     primTSCol, primBSCol,        /** TS and BS colors are **/
                     secTSCol, secBSCol,          /** for icon fg and bg   **/
                     secBgCol;
    ApplicationData  xrdb;
    char            *home;
    char            *execName;
    SmStateInfo      smState;
    SmSaverInfo      smSaver; 
    Boolean          longfilename;
    Widget           shell,
                     mainWindow,
                     colorDialog,
                     backdropDialog,
                     fontDialog,
                     kbdDialog,
                     mouseDialog,
                     audioDialog,
                     screenDialog,
                     startupDialog,
                     dtwmDialog,
                     i18nDialog,
                     buttonsForm;
    XmString         tmpXmStr;
    Widget           errDialog, errParent;
    char             tmpBigStr[1024];
    int              fontSize, timeoutscale;
    short            count;
    Boolean          colorSrv;
    Boolean          dynamicColor;
    Boolean          workProcs;
    int              horizontalSpacing, verticalSpacing;
	int              visualClass;
} Style;

/*
 *  External variables  
 */

extern Style style;
extern char  *progName;
extern XrmDatabase sys_pl_DB;
extern XrmDatabase adm_pl_DB;
extern XrmDatabase hm_pl_DB;
extern XrmDatabase pl_DB;

/*  
 *  External Interface  
 */

extern int local_xerror( Display *dpy, XErrorEvent *rep) ; /* defined in hostBB.c for 
								historical reasons*/


extern char *_DtGetMessage( 
		        char *filename,
                        int set, 
                        int n, 
                        char *s );
extern void raiseWindow(
                        Window dialogWin) ;
extern void CenterMsgCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern void ErrDialog( 
                        char *errString,
                        Widget visualParent) ;
extern void InfoDialog( 
                        char *infoString,
                        Widget parent,
                        Boolean unmapParent) ;
extern void putDialog( 
                        Widget parent,
                        Widget dialog) ;
extern void smStateCB( 
                        DtString fields[],
                        XtPointer client_data,
                        int num_fields) ;
extern void HandleStopMsg( 
                        DtString fields[],
                        XtPointer client_data,
                        int num_words) ;
extern void LoadDatabase(void);

/*
 * macro to get message catalog strings
 */
#ifndef NO_MESSAGE_CATALOG
# ifdef DT_LITE
#  define GETMESSAGE(set, number, string)\
    _DtGetMessage(set, number, "")
# else /*  DT_LITE*/
#  define CLIENT_MESSAGE_CAT_NAME       "dtstyle"
#  define GETMESSAGE(set, number, string)\
    _DtGetMessage(CLIENT_MESSAGE_CAT_NAME, set, number, string)
# endif /* DT_LITE */
#else
# define GETMESSAGE(set, number, string)\
    string
#endif

#endif /* _main_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
  
