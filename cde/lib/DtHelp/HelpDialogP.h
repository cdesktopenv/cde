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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: HelpDialogP.h /main/6 1996/04/13 11:55:10 ageorge $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HelpDialogP.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Privite Header file for HelpDialog.c 
 **  -----------
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 *******************************************************************
 *************************************<+>*************************************/

#ifndef _DtHelpDialogP_h
#define _DtHelpDialogP_h

#include <Xm/BulletinBP.h>

#include <Dt/HelpDialog.h>

#include "FileListUtilsI.h"
#include "GlobSearchP.h"

#define XmDIALOG_SUFFIX            "_popup"
#define XmDIALOG_SUFFIX_SIZE       6


/* Defines for use in allocation geometry matrix. */
#define TB_MAX_WIDGETS_VERT     7
#define TB_MAX_NUM_WIDGETS      12


/****************************************************************
 *
 *  Topic List Info Structure Definition (History & Jump Back Lists)
 *
 ****************************************************************/
typedef struct _DtTopicListStruct {
    char                         *locationId;
    XmString                      topicTitleLbl;
    char                         *helpVolume;
    int                          topicType;
    int                          pathLevel;
    int                          scrollPosition;
    struct _DtTopicListStruct   *pNext;
    struct _DtTopicListStruct   *pPrevious;
} DtTopicListStruct;


/****************************************************************
 *
 *  Volume List Info: History Dialog only.
 *
 ****************************************************************/
typedef struct _DtHistoryListStruct {
    XmString                    itemTitle;
    int                         topicType;
    int                         totalNodes;
    struct _DtHistoryListStruct *pNext;
    struct _DtTopicListStruct   *pTopicHead;
    struct _DtTopicListStruct   *pTopicTale;
} DtHistoryListStruct;


/****************************************************************
 *
 *  Help Callback return structure
 *
 ****************************************************************/
typedef	struct	_DtHelpListStruct {
    char *			locationId;
    Widget			widget;
    struct _DtHelpCommonHelpStuff * help;
    struct _DtHelpListStruct *	pNext;
    struct _DtHelpListStruct *	pPrevious;
} DtHelpListStruct;



/****************************************************************
 *
 *  Kewword Search entry structure.
 *
 ****************************************************************/

typedef struct {
   char * keyWord;
   Boolean    matchesFindPattern;
} KeyEntry;




/* Class Part Structure Definition */

typedef struct
{
	XtPointer		extension;      /* Pointer to extension record */
} DtHelpDialogWidgetClassPart;




/* Full class record declaration */

typedef struct _DtHelpDialogWidgetClassRec
{
	CoreClassPart			core_class;
	CompositeClassPart		composite_class;
	ConstraintClassPart		constraint_class;
	XmManagerClassPart		manager_class;
	XmBulletinBoardClassPart	bulletin_board_class;
	DtHelpDialogWidgetClassPart	selection_box_class;
} DtHelpDialogWidgetClassRec;

externalref DtHelpDialogWidgetClassRec dtHelpDialogWidgetClassRec;


typedef struct _DtHelpDisplayWidgetStuff
{
        short           textColumns;		/* Columns Resource Value resource */
        short           textRows;		/* Rows Resource Value resource */
        char *          locationId;		/* Current Topic String resource */
        char *          helpVolume;		/* Current Project File resource */
        char *          manPage;		/* man page resource */
        char *          stringData;		/* string data resource */
        char *          helpFile;		/* help file resource */
        unsigned char	scrollBarPolicy;	/* Scrollbar policy resource */       
        unsigned char	executionPolicy;	/* Link/script exec policy resource */        
        XtCallbackList	hyperLinkCallback;	/* Hypertext Callback resource */
        _DtHelpVolumeHdl volumeHandle;        	/* handle of current volume */
        unsigned char	helpType;		/* Current displayed file type resource */
        char *          topicTitleStr;		/* title of topic */
	int             count;                  /* Nonexistent topic title count */        
	XmString        topicTitleLbl;		/* XmString title of topic */
        Boolean         firstTimePopupFlag;     /* False, untel we map the widget */


} _DtHelpDisplayWidgetStuff;

typedef struct _DtHelpCommonHelpStuff
{
        XtPointer       pDisplayArea;		/* Display widget handle */

	/* for help on help */
        char *          helpOnHelpVolume;	/* help on help volume resource */
        DtHelpListStruct * pHelpListHead;	/* Help List Pointer */
        Widget          onHelpDialog;		/* help on help dialog */

        char *          sysVolumeSearchPath;	/* system search path */
        char *          userVolumeSearchPath;	/* user search path */

        char *          currentHelpFile;	/* Current help file */
        int             topicOffset;		/* Offset into help file */
        
        char *          topLevelId;		/* Toplevel topic volume displayed */
        int             topLevelTopicOffset;
} _DtHelpCommonHelpStuff;

typedef struct _DtHelpGeneralHelpStuff
{
        char *          parentId;		/* used for "Up" menu */

        Boolean         volumeFlag;		/* set when to process new vol */

        Widget          definitionBox;
        XtCallbackList	closeCallback;		/* Window Close Callback */

        Dimension       marginHeight;
        Dimension       marginWidth;
} _DtHelpGeneralHelpStuff;


typedef struct _DtHelpBrowserStuff 
{
        int             visiblePathCount;	/* Number of path lines shown */
        XtPointer	pTocArea;		/* Toc area handle */

        Widget          volumeLabel;
        Widget          panedWindow;  
	Widget		pathArea;

        Boolean         showTopLevelBtn;
        Widget          btnBoxBackBtn;
        Widget          btnBoxHistoryBtn;
        Widget          btnBoxIndexBtn;
        Widget          btnBoxTopLevelBtn;
} _DtHelpBrowserStuff;

typedef struct _DtHelpMenuStuff
{
        Boolean         showDupBtn;

        /* Help Dialog Widgets */
        Widget          menuBar;

	Widget		topBtn;
	Widget		keyBtn;
        Widget          backBtn;
	Widget		historyBtn;
        Widget          printBtn;
        Widget          closeBtn;
        Widget          helpBtn;
        Widget          newWindowBtn;
        Widget          copyBtn;
 
        Widget          popupMenu;
        Widget          popupBackBtn;
        Widget          popupTopBtn;
} _DtHelpMenuStuff;

typedef struct _DtHelpHistoryStuff
{
        /* Path Area Variables */
        DtTopicListStruct * pPathListHead;
        DtTopicListStruct * pPathListTale;
        int                 totalPathNodes;

        /* History Dialog Widgets & variables */
       	Widget              volumeList;		/* Scrolled volume list */   
        Widget              topicList;		/* Scrolled topic  list */   
        Widget              historyWidget;	/* Top Level History Shell */
        Widget              topicsListLabel;
        DtHistoryListStruct * pHistoryListHead;	/* Head pointer to history */
} _DtHelpHistoryStuff;

typedef struct _DtHelpBacktrackStuff
{
        /* Jump List variables */
        DtTopicListStruct  * pJumpListHead;
        DtTopicListStruct  * pJumpListTale; 
        int                  totalJumpNodes;
        int                  scrollPosition;
} _DtHelpBacktrackStuff;

typedef struct _DtHelpGlobSearchStuff
{
        /* search dialog resource (undocumented) */
        char *            hitPrefixFont;	/* hit prefix mono-space font resource */

        /* host application locale settings */
        char *            iconv3Codeset;	/* iconv3-ok codeset of app */
        _DtHelpCeIconvContext iconv3Context;     /* from vol to app locale */

        /* File Selection Dialog font list */
        XmFontList        volTitlesFontList;	/*font list req'd by font titles*/

        /* Search Dialog Widgets & variables */
        Widget            srchForm;
        Widget            actionBtn;
        Widget            curVolRadBtn;
        Widget            allVolRadBtn;
        Widget            selVolRadBtn;
        Widget            selectBtn;
        Widget            fullIndexRadBtn;
        Widget            containsRadBtn;
        Widget            wordField;
        Widget            statusLabel;
        Widget            resultList;
        Widget            gotoBtn;
        Widget            closeBtn;
        Widget            helpBtn;
        Widget            selectionDlg;		/* selectionBox (child of dlg) */
        char *            curVolPath;		/* cur vol shown in btn label */
        char *            rawWordStr;		/* word as taken from srchWord */
        char *            normWordStr;		/* normalized word str */
        char *            localeWordStr;	/* after iconv() of NormWordStr */
        wchar_t           wordFieldFirstChar;
        short             wordFieldLen;
        short             statusLineUsage;
        _DtHelpGlobSrchSources srchSources;	/* current state of radio but */
        _DtHelpFileEntry  curSrchVol;		/* vol currently being searched */
        unsigned int      hitsFontLoaded:1;	/* is font loaded? */
        unsigned int      volScanDone:1;	/* is the volume list complete? */
        unsigned int      fullIndex:1;		/* state of search */
        unsigned int      readyToStart:1;	/* state of search */
        unsigned int      hitsFound:1;		/* state of search */
        unsigned int      searchInProgress:1;	/* state of search */
        unsigned int      curVolRadBtnSens:1;	/* state of curVol sens */
        short             volLeftCnt;		/* updated during search */
        _DtHelpFileList   volListHead;		/* info on search topics found */
        XtWorkProcId      workProcId;		/* ID of search proc */
} _DtHelpGlobSearchStuff;

typedef struct _DtHelpPrintStuff
{
        /* printing resources */
        char *          helpPrint;		/* help print command resource */
        unsigned char	paperSize;		/* paper size resource */        
        char *          printer;		/* printer destination resource */
        char *          printVolume;		/* volume to print resource */

        /* Print Dialog widgets & variables */
        Widget          printForm;		/* Form inside top level print shell */
        Widget          subject;		/* label for subject matter to print */
        Widget          printerField;		/* text field for printer */
        Widget          copiesField;		/* text field for copies */
        Widget          letterBtn;		/* menu item for letter paper */
        Widget          legalBtn;		/* menu item for legal paper */
        Widget          execBtn;		/* menu item for executive paper */
        Widget          a4Btn;			/* menu item for A4 paper */
        Widget          b5Btn;			/* menu item for B5 paper */
        Widget          paperSizeOptMenu;	/* menu item for A4 paper */
        Widget          topicsFrame;		/* frame holding print topic selection */
        Widget          curTopicBtn;		/* radio btn for cur topic */
        Widget          subTopicsBtn;		/* radio btn for cur & sub topics */
        Widget          tocBtn;			/* radio btn for table of contents */
        Widget          allTopicsBtn;		/* radio btn for all topics */
        Widget          paperTopicsSeparator;	/* hidden sep btwn paper size & topic sel */
        Widget          topicsBtnsSeparator;	/* sep between topic sel & btns */
} _DtHelpPrintStuff;


/* fields for the GeneralHelp widget record */
typedef struct
{
      _DtHelpDisplayWidgetStuff	display;
      _DtHelpCommonHelpStuff	help;
      _DtHelpGeneralHelpStuff	ghelp;
      _DtHelpBrowserStuff	browser;
      _DtHelpMenuStuff		menu;
      _DtHelpHistoryStuff	history;
      _DtHelpBacktrackStuff	backtr;
      _DtHelpGlobSearchStuff	srch;
      _DtHelpPrintStuff		print;
} DtHelpDialogWidgetPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _DtHelpDialogWidgetRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	XmBulletinBoardPart	bulletin_board;
	DtHelpDialogWidgetPart	help_dialog;
} DtHelpDialogWidgetRec;



/********    Private Function Declarations    ********/

extern XmGeoMatrix _DtHelpDialogWidgetGeoMatrixCreate( 
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *desired) ;
extern Boolean _DtHelpDialogWidgetNoGeoRequest( 
                        XmGeoMatrix geoSpec) ;

/********    End Private Function Declarations    ********/



#endif /* _XmTemplateBP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

