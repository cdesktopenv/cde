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
/* $TOG: GlobSearch.c /main/21 1999/11/11 10:59:15 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        GlobSearch.c
 **
 **   Project:     DtHelp Project
 **
 **   Description: Builds and displays an instance of a DtHelp GlobSearch
 **                Dialog.
 ** 
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#if defined(sun)
#include <sys/utsname.h>
#endif
#include <limits.h>
#include <stdlib.h>  /* for MB_CUR_MAX */
#include <unistd.h>  /* R_OK */
#include <locale.h>  /* getlocale(), LOCALE_STATUS */
#ifndef NO_REGEX
# ifdef NO_REGCOMP
#  if defined(SVR4)
#   include <libgen.h>		/* for regcmp, regex */
#  endif
# else
#  include <regex.h>		/* for regcomp, regexec */
# endif
#endif

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Frame.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/ScrolledW.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
#include <Xm/ToggleBG.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <Dt/Help.h>
#include <Dt/HelpDialog.h>

/*
 * private includes
 */
#include "Access.h"
#include "StringFuncsI.h"
#include "DisplayAreaI.h"
#include "HelpDialogP.h"
#include "HelpDialogI.h"
#include "HelpAccessI.h"
#include "HelpUtilI.h"
#include "HelposI.h"
#include "GlobSearchI.h"
#include "FileListUtilsI.h"
#include "FileUtilsI.h"
#include "FormatI.h"
#include "HelpXlate.h"
#include "VolSelectI.h"
#include "Lock.h"

/******** TYPES ***********/
typedef enum {
    SEARCH_RESULTS_STATUS = 1,
    WORKING_STATUS = 2,
    SCANNING_STATUS = 3,
    BLANK_STATUS = 4,
    NO_VOL_STATUS = 5,
    FIRST_PROMPT_STATUS = 6
} ResultsStatus;

static Boolean VolumeHasIndexP (
    _DtHelpGlobSrchSources srchSource,
    int          helpType,
    char * baseName);
static void DeleteListContents(
        _DtHelpGlobSearchStuff * srch);
static void StatusLabelUpdate(
    DtHelpDialogWidget hw,
    ResultsStatus      status,
    Boolean            forceUpdate,
    int                intArg);
static void ResultsListUpdate(
    DtHelpDialogWidget hw,
    _DtHelpFileEntry         newFile);
static void StopSearchCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData);
static void  VolNameDisplay(
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file,
          Boolean           insertVol);
static int VolHitsDisplay (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file);
static void UpdateSearchVolumesCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData);
static void UpdateSearchStartStatusCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData);

/********** CONSTANTS *************/
#define DIR_SLASH '/'
#define EOS       '\0'

/* message catalog set number for GlobSearch.c */
#define GSSET    5

#define DONT_SET    (-1)              /* used as third value of a Boolean */

#define HIT_FONT_RES_NAME   "*helpSearchMonoFont"
#define HIT_FONT_RES_CLASS  "*HelpSearchMonoFont"
#define PREFIX_FONT_SPEC  "-dt-interface user-bold-r-normal-m*-*-*-*-*-m-*-iso8859-1"
#define PREFIX_FONT_SPEC1 "-dt-application-bold-r-normal-*-*-140-*-*-m-*-iso8859-1"
#define PREFIX_FONT_SPEC2 "-dt-interface user-bold-r-normal-m*-*-*-*-*-m-*-iso8859-1"
#define PREFIX_FONT_SPEC3 "courb14"      /* non CDE platforms */
#define PREFIX_FONT_TAG   "prefixFontTag"

#define START_SEARCH_CAT s_GlobSrchDlgBtnCatNum[0]
#define START_SEARCH_STR s_GlobSrchDlgBtnStrs[0]
#define START_SEARCH_MNEM "S"
#define CONT_SEARCH_CAT s_GlobSrchDlgBtnCatNum[1]
#define CONT_SEARCH_STR s_GlobSrchDlgBtnStrs[1]
#define CONT_SEARCH_MNEM "S"
#define STOP_SEARCH_CAT s_GlobSrchDlgBtnCatNum[2]
#define STOP_SEARCH_STR s_GlobSrchDlgBtnStrs[2]
#define STOP_SEARCH_MNEM "S"
#define CLOSE_BTN_CAT s_GlobSrchDlgBtnCatNum[3]
#define CLOSE_BTN_STR  s_GlobSrchDlgBtnStrs[3]
#define CLOSE_BTN_MNEM  "C"
#define HELP_BTN_CAT  s_GlobSrchDlgBtnCatNum[4]
#define HELP_BTN_STR  s_GlobSrchDlgBtnStrs[4]
#define HELP_BTN_MNEM  "H"


/********** MACROS ************/
#define max(a,b)  ((a) > (b) ? (a) : (b))

/********** Global VARIABLES ************/
char _DtHelpDefaultSrchHitPrefixFont[] = PREFIX_FONT_SPEC;

/********** VARIABLES ************/
static const char *DirSlash = "/";
static char * s_GlobSrchDlgBtnStrs[] = {
            "Start Search", 
            "Continue Search", 
            "Stop Search",
            "Close",
            "Help",
            NULL };
/* catalog index numbers for the strings */
static unsigned char s_GlobSrchDlgBtnCatNum[] = {
             12, 13, 14, 15, 16 };

static char *     s_PrefixFontListTag = NULL;

/* Setup for the Retrun Translation set for the text field */
static char defaultBtnTranslations[] = "<Key>Return: Activate()";
/* static char defaultListTranslations[] = "<Key>Return: Activate()"; */
/* static char defaultMgrTranslations[] = "<Key>Return: ManagerParentActivate()"; */

#if DOC
/************************************************************************
The startPosition and nextVolPosition values of the DtHelpGlobSrchVol
structure are used as follows:

startPosition:  the position of the volume in the results list
                if hitCnt == 0, startPosition is the same value as the
                most recent volume with hits
nextVolPosition:  the position of the next volume in the results list
                if hitCnt == 0, nextVolPosition is the same value as the
                most recent volume with hits
                The value must be adjusted to include the positions
                occupied by hits and topics that are listed for the volume

These variables are maintained using the AdjustPositionValues()
routine, which increment/decrement the position values of all
files after the start.  So, when doing a search and display,
as results are added to the list, the position values of volumes
later in the list are always up to date.
************************************************************************/
#endif

/*======================================================================*/
/*======================================================================*/
/*======================================================================*/
/*======================================================================*/
 

/*****************************************************************************
 * Function:	    void MergeFontListIntoWidgetFonts()
 * 
 * Parameters:      widget        a widget with a XmNfontList resource
 *                  newFontsList  font list with fonts to merge into widget
 *
 * Return Value:    void
 *
 * Purpose: 	    Merges the fonts from the newFontsList into the fontlist
 *                  of the widget
 *
 *****************************************************************************/
static void MergeFontListIntoWidgetFonts(
     Widget     widget,
     XmFontList newFontsList)
{
    XmFontList fontList = NULL;
    XmFontContext context;
    XmFontListEntry entry;
    Arg args[2];

    /* get current resultList fontlist */
    XtSetArg(args[0], XmNfontList, &fontList);
    XtGetValues(widget,args,1);

    if (NULL == fontList) return;           /* RETURN on error */

    /* work with copy, because FontListAppendEntry() destroys input FL */
    fontList = XmFontListCopy(fontList);

    /* walk through the volTitlesFontList entries and add them in */
    XmFontListInitFontContext(&context,newFontsList);
    for ( entry = XmFontListNextEntry(context);
          NULL != entry;
          entry = XmFontListNextEntry(context) )
    {
       fontList = XmFontListAppendEntry(fontList,entry);
    }
    if (context) XmFontListFreeFontContext(context);
    
    /* install the changed list */
    XtSetArg(args[0], XmNfontList, fontList);
    XtSetValues(widget,args,1);

    if (fontList) XmFontListFree(fontList);
}


/*****************************************************************************
 * Function:	    void UpdateCurVolBtnSens()
 * 
 * Parameters:      new          the help widget
 *
 * Return Value:    True if cur vol has an index
 *                  False if not
 *
 * Purpose: 	    Checks on current volume for an index
                    and sets buttons appropriately
 *
 *****************************************************************************/
static Boolean UpdateCurVolBtnSens(
          DtHelpDialogWidget hw,
          Boolean            selectVolBtn)
{
     XmToggleButtonCallbackStruct status;        /* the call data */
     Boolean curState;
     Widget sourceBtn;

     /* set the cur vol btn sensitivity */
     /* set the volumes/show selection btns by generating a false event */
     status.reason = XmCR_VALUE_CHANGED;
     status.set = True;
     status.event = (XEvent *) 1; /* thwart == NULL test in Update...() */

#if 0
     if ( VolumeHasIndexP (
              hw->help_dialog.srch.srchSources,
              hw->help_dialog.display.helpType,
              hw->help_dialog.display.helpVolume) )
     {
         curState = True;
         sourceBtn = hw->help_dialog.srch.curVolRadBtn;
     }
     else
     {
         curState = False;
         sourceBtn = hw->help_dialog.srch.allVolRadBtn;
     }
     /* ???  XtSetSensitive(hw->help_dialog.srch.curVolRadBtn,curState); */
     hw->help_dialog.srch.curVolRadBtnSens = curState;
     if (selectVolBtn) UpdateSearchVolumesCB(sourceBtn,hw,&status);
#else

     curState = True;
     sourceBtn = hw->help_dialog.srch.curVolRadBtn;
 
     hw->help_dialog.srch.curVolRadBtnSens = curState;
     if (selectVolBtn)
         UpdateSearchVolumesCB(sourceBtn,(XtPointer)hw,(XtPointer)&status);
#endif


     return curState;
}
 

/************************************************************************
 * Function: LoadPrefixFont()
 *
 * Loads the prefix font, if that hasn't yet occurred
 *
 ************************************************************************/
static void LoadPrefixFont(
          DtHelpDialogWidget hw)
{
   XmFontListEntry entry;
   XmFontList      curFontList = NULL;
   XmFontList      newFontList = NULL;
   char *          fontSpec = NULL;
   Arg             args[3];

   if (hw->help_dialog.srch.hitsFontLoaded) return;          /* RETURN */

   /* this code is for when the resource is part of the widget */
   fontSpec = hw->help_dialog.srch.hitPrefixFont;

   /* get current font list */
   XtSetArg(args[0], XmNfontList, &curFontList);
   XtGetValues(hw->help_dialog.srch.resultList,args,1);

   /* work with copy, because FontListAppendEntry() destroys input FL */
   newFontList = XmFontListCopy(curFontList);

   /* load and merge fonts */
   s_PrefixFontListTag = PREFIX_FONT_TAG;
   entry = XmFontListEntryLoad(XtDisplay(hw->help_dialog.srch.srchForm),
             fontSpec,XmFONT_IS_FONT, s_PrefixFontListTag);
   newFontList = XmFontListAppendEntry(newFontList,entry);
   XmFontListEntryFree(&entry);

   /* install font */
   if (NULL == newFontList)
   {  /* error case */
      s_PrefixFontListTag = XmFONTLIST_DEFAULT_TAG;
   }
   else
   {
      /* set new font list */
      XtSetArg(args[0], XmNfontList, newFontList);
      XtSetValues(hw->help_dialog.srch.resultList,args,1);
      if (newFontList) XmFontListFree(newFontList);
   }
   hw->help_dialog.srch.hitsFontLoaded = True;
}

 

/************************************************************************
 * Function: VolumeHasIndexP()
 *
 * Looks for the specified volume in the file system and 
 * tests whether the volume has an index associated with it.
 *
 ************************************************************************/
static Boolean VolumeHasIndexP (
    _DtHelpGlobSrchSources srchSource,
    int          helpType,
    char * 	baseName)
{
   char * path = NULL;
   _DtHelpVolumeHdl vol;
   int  keyWordCount = 0;
   char **currentKeyList = NULL;

   /* if help content isn't a volume, it has no index */
   if (    _DtHelpGlobSrchCurVolume == srchSource
        && helpType != DtHELP_TYPE_TOPIC ) 
       return False;                                     /* RETURN */

   /* try to locate file and its entry, if present */
   path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, baseName,
                                  _DtHelpFileSuffixList,False,R_OK);
   if (_DtHelpOpenVolume(path, &vol) == 0 )
   {
      /* Get the keyword list values */
      keyWordCount = _DtHelpCeGetKeywordList(vol,&(currentKeyList));

      /* no need to free currentKeyList because these are mgd by CE */
      _DtHelpCloseVolume(vol);
   }

   XtFree(path);
   return (keyWordCount > 0);
}



/************************************************************************
 * Function: GetVolumeInfoCB()
 *
 * Get the info on the volume and return it
 * Any of the 'out' parameters may be NULL.
 *
 ************************************************************************/
static Boolean GetVolumeInfoCB (
    XtPointer  pDisplayArea,  /* in: display area in use */
    char *     volumePath,    /* in: full path to file */
    char **    ret_title,     /* out: mallocd doc title text string */
    XmString * ret_titleXmStr,/* out: mallocd doc title string */
    char **    ret_docId,     /* out: mallocd doc Id string */
    char **    ret_timeStamp, /* out: mallocd doc time string */
    int *      ret_nameKey,   /* out: hash value for fast discimination */
    XmFontList * io_fontList, /* io: fontList for title */
    Boolean *  ret_mod)       /* out: has font list been changed */
{
   char empty = EOS;
   _DtHelpVolumeHdl vol;
   char * baseName = volumePath;     /* no path component */

   /* calc baseName */
   if(_DtHelpCeStrrchr(volumePath, DirSlash, MB_CUR_MAX, &baseName)==0)
	baseName++;

   /* open volume */
   if ( _DtHelpOpenVolume(volumePath, &vol) != 0 )
   {  /* if can't open, do best possible */
      if (ret_titleXmStr) 
         *ret_titleXmStr = XmStringCreateLocalized(baseName); 
      if (ret_title) *ret_title= XtNewString(baseName);
      if (ret_nameKey) *ret_nameKey = _DtHelpCeStrHashToKey(baseName);
      return False;
   }

   /** volume is open, now get the data **/

   if (ret_title)
   {
      char *   locTitle = NULL;

      /* locTitle is owned by caller */
      _DtHelpGetAsciiVolumeTitle(pDisplayArea, vol, &locTitle);
      if (locTitle == NULL)
         locTitle = XtNewString(baseName);
      *ret_title = locTitle;
   }

   if (ret_titleXmStr)
   {
      XmString   locTitleXmStr = NULL;

      /* locTitle is owned by caller */
      _DtHelpFormatVolumeTitle(pDisplayArea,vol,
                                   &locTitleXmStr,io_fontList,ret_mod);
      if (locTitleXmStr == NULL)
         locTitleXmStr = XmStringCreateLocalized(baseName); 
      *ret_titleXmStr = locTitleXmStr;
   }

   if (ret_docId || ret_timeStamp)
   {
      char * locDocId = NULL;
      char * locTimeStamp = NULL;

      /* locDocId & locTimeStamp will point to private memory; do not modify */
      _DtHelpCeGetDocStamp(vol,&locDocId, &locTimeStamp);

      if (ret_docId) 
         *ret_docId = locDocId ? locDocId : XtNewString(&empty);
      if (ret_timeStamp) 
         *ret_timeStamp = locTimeStamp?locTimeStamp:XtNewString(&empty);
   }

   if (ret_nameKey)
   {
      *ret_nameKey = _DtHelpCeStrHashToKey(baseName);
   }

   _DtHelpCloseVolume(vol);

   return True;
}


/************************************************************************
 * Function: AdjustPositionValues()

 * Adjust the position values of all volumes in the list by the amount given
 *
 ************************************************************************/
static void AdjustPositionValues(
   _DtHelpFileList   fileList,
   int            adjStartAmount,
   int            adjNextAmount,
   int            adjustThisFile)
{
   _DtHelpGlobSrchVol * curVol;

   if (   NULL == fileList 
       || NULL == (curVol = (_DtHelpGlobSrchVol *) fileList->clientData) )
       return;                                       /* RETURN */

   /* do we need to find first next file that has hits and is in list? */
   if(False == adjustThisFile)
   {
      curVol->nextVolPosition += adjNextAmount;
      for ( fileList = _DtHelpFileListGetNext(NULL,fileList); /* begin with next */
            NULL != fileList;
            fileList = _DtHelpFileListGetNext(NULL,fileList) )
      {
         register _DtHelpGlobSrchVol * vol=(_DtHelpGlobSrchVol *)fileList->clientData;
         if (vol->showVolInList && (vol->hitCnt > 0 || vol->zeroHitsOk) )
            break;                                   /* BREAK */
         vol->nextVolPosition += adjNextAmount;
      }
      /* all further files require same adj amts */
      adjStartAmount = adjNextAmount;
   }


   /* add in the adjust amount to all files */
   for ( /* do nothing */; 
         NULL != fileList;
         fileList = _DtHelpFileListGetNext(NULL,fileList) )
   {
      register _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) fileList->clientData;
      if (vol)
      {
         vol->startPosition += adjStartAmount;
         vol->nextVolPosition += adjNextAmount;
      }
      /* overwrite, once we've done the first file, if haven't already */
      adjStartAmount = adjNextAmount;
   }
}


/************************************************************************
 * Function: HitLoadTopics()
 *
 *    Loads the topics referred to by a hit
 *    Returns: 0 if loaded ok, -1 otherwise
 ************************************************************************/
static int HitLoadTopics (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry         file,
          _DtHelpGlobSrchHit *     hit)
{
    _DtHelpVolumeHdl volHandle;
    char * *   origTopicIdList = NULL;
    int        topicCnt;
    int        i;
    XmString * titlesList = NULL;
    char * *   idList = NULL;
    char * *   fileList = NULL;
    XmFontList fontList = NULL;
    Boolean    mod = False;
    Boolean    allMods = False;
    Arg        args[5];

    if (hit->topicsLoaded) return 0;                    /* RETURN: ok */

    if (_DtHelpOpenVolume(file->fullFilePath, &volHandle) != 0 ) 
        return -1;                                      /* RETURN: error */

    topicCnt = _DtHelpCeFindKeyword(volHandle,hit->indexEntry,&origTopicIdList);
    if (topicCnt <= 0) return -1;                       /* RETURN: error */
    
    /* get results font list */
    XtSetArg(args[0], XmNfontList, &fontList);
    XtGetValues(hw->help_dialog.srch.resultList,args,1);
 
#if defined(DONT_USE_CDExc22774)
    /* Don't need to copy, _DtHelpFormatVolumeTitle copies 
     * before modifying.
     */

    /* FIX: check whether this is a memory leak.  It isn't if the
       GetValues of XmFontList returns its own list, not a copy */
    /* work with copy, because FontListAppendEntry() destroys input FL */
    fontList = XmFontListCopy(fontList);
#endif

    /* get the titles of all topics */
    for (i=0; i<topicCnt; i++)
    {
        XmString titleStr = NULL;
        Boolean  valid = False;
	XmFontList lastFontList = NULL;

        /*
	 * mod==True indicates _DtHelpFormatTopicTitle copied fontList
	 * once already.  Save a pointer to it so we can free the font list
	 * if _DtHelpFormatTopicTitle copies it again.
	 */
	lastFontList = fontList;

        valid = _DtHelpFormatTopicTitle(hw->help_dialog.help.pDisplayArea,
                    volHandle,origTopicIdList[i], &titleStr, &fontList, &mod);

	if (mod && NULL != lastFontList) XmFontListFree(lastFontList);
	lastFontList = NULL;
        allMods |= mod;  /* track for all iterations */

        if(valid!=0 || NULL==titleStr) 
        {
           titleStr=XmStringCreateLocalized(origTopicIdList[i]);
        }  /* if couldn't get title */

        /* note that titleStr is an XmString, not an (Xt) String */
        titlesList = (XmString *)_DtHelpCeAddPtrToArray (
                          (void **)titlesList, (void *) titleStr);
        idList = (char **)_DtHelpCeAddPtrToArray (
                          (void **)idList,
			  (void *) XtNewString(origTopicIdList[i]));
        fileList = (char **)_DtHelpCeAddPtrToArray (
                          (void **)fileList,
			  (void *) XtNewString(file->fullFilePath));
    }  /* for all topics of this index entry */

    /* install font list, if changed */
    if (allMods)
    {
       XtSetArg(args[0], XmNfontList, fontList);
       XtSetValues(hw->help_dialog.srch.resultList,args,1);
       if (fontList) XmFontListFree(fontList);
    }

    /* put results into hit */
    hit->topicTitles = titlesList;
    hit->topicIdList = idList;
    hit->topicFileList = fileList;
    hit->topicCnt = topicCnt;
    hit->topicsLoaded = True;

    _DtHelpCloseVolume(volHandle);

    return 0;
} 



/************************************************************************
 * Function: HitNameDisplay()
 *
 *    Updates the name of the hit, not delete or insert
 ************************************************************************/
static int HitNameDisplay (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file,
          _DtHelpGlobSrchHit * hit,
          int               hitPos,
          Boolean           insertHit)
{
     char *   expandPrefix;
     char *   contractPrefix;
     char *   gotoPrefix;
     XmString prefixString;
     XmString labelString;
     char *   tmpStr;
     char     buf[30];

     /* FIX: this method of getting/using the prefixes 
             is a performance nightmare; plus subject to font variability */
     expandPrefix = (char *)_DTGETMESSAGE (GSSET, 28," +");
     contractPrefix = (char *)_DTGETMESSAGE (GSSET, 29," -");
     gotoPrefix = (char *)_DTGETMESSAGE (GSSET, 30,"       ");
  
     /* if more than one topic for this index, signal it */
     tmpStr = gotoPrefix;  /* index entry has one topic */
     if ( hit->topicCnt > 1 ) 
     {
        if (hit->topicsDisplayed)    /* topics currently displayed */
           tmpStr = contractPrefix;
        else                         /* topics not displayed */
           tmpStr = expandPrefix;
        sprintf(buf, "%s%3d  ", tmpStr, hit->topicCnt); /* 4d too spacy */
        tmpStr = buf;
     }
     LoadPrefixFont(hw);
     prefixString = XmStringGenerate(tmpStr, s_PrefixFontListTag,
				     XmCHARSET_TEXT, NULL);
     labelString = XmStringConcat(prefixString,hit->indexTitle);
        /* recall indexTitle is an XmString in the volume's indexXmStrsList */

     /* install/insert the item */
     if (insertHit)
        XmListAddItemUnselected(hw->help_dialog.srch.resultList,
                                 labelString,hitPos);
     else
        XmListReplaceItemsPosUnselected(hw->help_dialog.srch.resultList,
                                 &labelString,1,hitPos);

     XmStringFree(prefixString);
     XmStringFree(labelString);
     return (insertHit ? 1 : 0);
}



/************************************************************************
 * Function: HitTopicsDisplay()
 *
 *    Loads the topics referred to by a hit and displays them
 *    Returns the number of topics displayed
 ************************************************************************/
static int HitTopicsDisplay (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry         file,
          _DtHelpGlobSrchHit *     hit,
          int                   firstTopicListPosition)
{
    XmString * pTopicString;
    XmString prefixString;
    XmString labelString;
    XmString *items;
    char *   tmpStr;
    int      i;

    if ( False == hit->topicsLoaded && HitLoadTopics(hw,file,hit) < 0 ) 
       return 0;                                     /* RETURN: error */
    if ( hit->topicsDisplayed ) return 0;            /* RETURN: ok */

    LoadPrefixFont(hw);
  /*tmpStr = (char *)_DTGETMESSAGE (GSSET, 32,"  *      ");*/
    tmpStr = (char *)_DTGETMESSAGE (GSSET, 31,"         ");
    prefixString = XmStringGenerate(tmpStr, s_PrefixFontListTag,
				    XmCHARSET_TEXT, NULL);

    if (hit->topicCnt > 0)
    {
	items = (XmString *) XtMalloc (sizeof (XmString) * hit->topicCnt);

        /* put XmString-formatted topic titles into list */
        for (i = 0, pTopicString = hit->topicTitles; NULL != *pTopicString; 
						pTopicString++, i++ )
	   items[i] = XmStringConcat(prefixString,*pTopicString);

       XmListAddItemsUnselected(hw->help_dialog.srch.resultList, items,
				     hit->topicCnt, firstTopicListPosition);
       for (i = 0; i < hit->topicCnt; i++)
	   XmStringFree(items[i]);
       XtFree((char *) items);
    }

    XmStringFree(prefixString);

    /* set state */
    hit->topicsDisplayed = True;
    hit->showTopicsWithHit = True;
    return hit->topicCnt;
}




/************************************************************************
 * Function: HitTopicsUndisplay()
 *
 *   Releases the topics referred to by a hit and undisplays them
 *   Returns number of topics removed from display and hit.
 ************************************************************************/
static int HitTopicsUndisplay (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file,
          _DtHelpGlobSrchHit * hit,
          int               firstTopicListPosition)
{
   int      topPos;
   Arg args[5];

   if ( False == hit->topicsDisplayed ) return 0;           /* RETURN */

   XtSetArg(args[0], XmNtopItemPosition, &topPos);
   XtGetValues(hw->help_dialog.srch.resultList,args,1);

   /* count the topics -- they are also being collapsed */
   if (hit->topicCnt > 0) 
     XmListDeleteItemsPos(hw->help_dialog.srch.resultList,
					hit->topicCnt, firstTopicListPosition);

   hit->topicsDisplayed = False;
   /* NOTE: don't reset hit->showTopicsWithHit here; require that
      to be explicitly reset in ProcessResultSelection() */

   XmListSetPos(hw->help_dialog.srch.resultList, topPos);

   return hit->topicCnt;
}




/************************************************************************
 * Function: HitFree()
 *
 *    Frees the memory associated with a hit and returns the next hit
 *    member of the structure
 ************************************************************************/
static _DtHelpGlobSrchHit * HitFree (
          _DtHelpGlobSrchHit * hit,
          Boolean           freeHitItself)
{
   XmString *           nextStr;
   _DtHelpGlobSrchHit * nextHit;

   if (NULL == hit) return NULL;

   nextHit = hit->next;

   /* Free the mem of the topics id list */
   _DtHelpCeFreeStringArray(hit->topicIdList);
   hit->topicIdList = NULL;        /* mem not owned by me */

   /* Free the mem of the topics files list */
   _DtHelpCeFreeStringArray(hit->topicFileList);
   hit->topicFileList = NULL;

   /* topicTitles are XmStrings and we can't use FreeStringArray() */
   for ( nextStr = hit->topicTitles; 
         NULL != nextStr && NULL != *nextStr; 
         nextStr++)
       XmStringFree (*nextStr);
   XtFree((char *) hit->topicTitles); /* its an XmString * */
   hit->topicTitles = NULL;

   /* set flags */
   hit->topicsLoaded = False;
   hit->topicsDisplayed = False;

   if (freeHitItself)
     {
       /* Free the index entry */
       XtFree(hit->indexEntry);
       XtFree((String)hit);
     }

   return nextHit;
}


/************************************************************************
 * Function: HitListFree()
 *
 *    Walks along a hit list and frees its contents
 ************************************************************************/
static void HitListFree(
        _DtHelpGlobSrchVol * vol,
        Boolean           freeHitsThemselves)
{
   register _DtHelpGlobSrchHit * hit;

   if (NULL == vol) return;                          /* RETURN */

   for ( hit = vol->hitListHead; 
         NULL != hit; 
         hit = HitFree(hit,freeHitsThemselves) )
      { /* nothing */ }

    /* reset search flags */
    vol->nothingDone = True;
    vol->topicSearchDone = False;
    vol->indexSearchDone = False;
    vol->searchCompleted = False;
    vol->searchedCnt = 0;
    vol->gatheredFullIndex = False;

    /* reset hit flags */
    vol->hitsDisplayed = False;
    vol->showHitsWithVol = False;
    vol->zeroHitsOk = False;
    vol->hitCnt = 0;

    /* reset list display flags */
    vol->startPosition = 1;
    vol->nextVolPosition = 1;

    /* dont free indexXmStrsList here because they
       are reused for every search on this volume */
    vol->curIndexXmStr = vol->indexXmStrsList;

    /* indexEntriesList,volhandle inited elsewhere */

    if (freeHitsThemselves) 
    {
        vol->hitListHead = NULL;
        vol->hitListTail = NULL;
    }
}


/************************************************************************
 * Function: HitListFreeAllVolHits()
 *
 *    Walks along all the volumes and frees their hits
 ************************************************************************/
static void
HitListFreeAllVolHits (
        DtHelpDialogWidget hw,
        Boolean            freeFullIndex)
{
   _DtHelpGlobSrchVol * vol;
   _DtHelpFileEntry     curFile;
   
   /* walk the files, freeing all hits and their data */
   for ( curFile = hw->help_dialog.srch.volListHead;
         NULL != curFile;
         curFile = _DtHelpFileListGetNext(NULL,curFile) )
   {
       vol = (_DtHelpGlobSrchVol *) curFile->clientData;

       if (NULL == vol) continue;                   /* CONTINUE */
       if (vol->gatheredFullIndex && freeFullIndex == False) continue;
       HitListFree(vol,True);         /* True: free hits themselves */
   }
   hw->help_dialog.srch.hitsFound = False;
}


/************************************************************************
 * Function: HitListGetNth()
 *
 *	Retrieves the Nth entry from the hits contained in the VolList
 *     
 *     This code follows the same counting paradigm used when adding
 *     the items to the srchResultList widget, so that we can just use the
 *     position value returned by it to retrieve the selected item.
 *
 *     This paradigm also counts the presence of a volume as an item
 *     if it contains any hits, and doesn't count it if it contains
 *     no hits.  It also counts the topicTitles that are associated
 *     with a hit and displayed indented beneath it.
 *
 *     The position count is 1 based, which corresponds with the 
 *     XmList approach.
 *
 * Return params:
 *     ret_hit: retuns NULL if an error or a pointer to the hit structure.
 *     ret_locationId:  returns NULL is position is for the hit
 *              structure itself, returns a pointer to private
 *              memory containing the location id if position
 *              is a topic belonging to the hit.
 *
 * Returns: 0 if ok, -1 if error
 ************************************************************************/
static int HitListGetNth (
    _DtHelpFileList   volListHead,
    int            position,
    _DtHelpFileEntry *     ret_file,
    _DtHelpGlobSrchHit * * ret_hit,
    char * *            ret_locationId,
    char * *            ret_helpFile)
{
   _DtHelpGlobSrchHit * hit = NULL;
   _DtHelpGlobSrchVol * curVol;
   _DtHelpFileEntry     curFile = NULL;
   _DtHelpFileEntry     posFile = NULL;
   char * *          locationIdList = NULL;
   char * *          fileNameList = NULL;
   int               curPos;
   
   if (NULL == volListHead) goto done;               /* GOTO on error */

   /* walk along the volumes to find the one containing the position
      Because we need the most recent file with hits before the
      nextFile that has too high a position, only set posFile when
      we aren't ready to stop yet and we're on a file with hits. */
   for ( curFile = volListHead;
         NULL != curFile;
         curFile = _DtHelpFileListGetNext(curFile,curFile) )
   {
     /* it is impt to use > test so that multiple volumes with the same
        start position are properly handled.  This occurs when they
        have no hits in them. */
     curVol = (_DtHelpGlobSrchVol *) curFile->clientData;
     if (NULL == curVol) continue;
#if 0
     if (curVol->startPosition > position) break;    /* BREAK */
     if (curVol->hitCnt > 0) posFile = curFile;      /* recent with hits */
#else
     if (curVol->showVolInList && curVol->nextVolPosition > position) 
     {
        posFile = curFile;
        break;  /* BREAK */
     }
#endif
   }
   if(NULL == posFile) goto done;                    /* GOTO on error */

   /*** walk along the hits to find the one matching the position ***/
   curVol = (_DtHelpGlobSrchVol *) posFile->clientData;
   curPos = curVol->startPosition;
   hit = NULL;
   locationIdList = NULL;
   fileNameList = NULL;

   /* (position == curPos) ==> volume name itself was selected */
   if ( curPos < position )
   {   /* a hit inside the volume */
      if ( curVol->hitsDisplayed )
      {
         for ( curPos++, hit = curVol->hitListHead;
               curPos < position && NULL != hit;
               curPos++, hit = hit->next )
         {
            if (False == hit->topicsDisplayed) continue;      /* CONTINUE */
   
            /* walk all displayed topics to see if the position is here */
            for ( locationIdList=hit->topicIdList,fileNameList=hit->topicFileList;
                  NULL != locationIdList[0];
                  locationIdList++, fileNameList++ )
            {
               /* use GOTO to escape with all pointers correct and
                  without introducing an additional flag variable */
               if (++curPos == position) goto done;      /* GOTO */
            }  /* for all locations of a hit before the position */
         }  /* for all members of the hit list before the position */
      }  /* if hits are currently shown */
      else posFile = NULL;            /* an error occurred */
   }  /* if position is in the hit list */

done:
   if (ret_hit) *ret_hit = hit;
   if (ret_file) *ret_file = posFile;
   if (ret_locationId)
   {
     if ( locationIdList ) *ret_locationId = locationIdList[0];
     else *ret_locationId = NULL;
   }
   if (ret_helpFile)
   {
     if ( fileNameList ) *ret_helpFile = fileNameList[0];
     else *ret_helpFile = NULL;
   }
   /* WARNING: depends on pointers and integers the same size */
#ifdef __osf__
   return (NULL == ((ulong_t)hit|(ulong_t)posFile|(ulong_t)locationIdList)) ? -1 : 0;
#else
   return (0 == ((int)hit|(int)posFile|(int)locationIdList)) ? -1 : 0;
#endif
}



/************************************************************************
 * Function: HitListAddFound()
 *
 *     Adds a hit to the hit list of the specified volume
 *     The hits are added either to the end of the list, 
 *     so that the srchResultList presents the items in the order found,
 *     or in a sorted order.
 *     If a hit on that topic already exists, just the existing
 *     hit structure is returned.
 *
 *  Return Parameters:
 *   Ret_hit points to the hit data.  This is not a copy--do not
 *     free the pointer.
 *     
 *  Return value:
 *     Returns 0 if no error, -1 if an error occurred.
 ************************************************************************/
static int HitListAddFound (
    _DtHelpFileEntry     curFile,
    XmString             indexTitle,
    char *               indexEntry,
    Boolean              insertSorted,
    _DtHelpGlobSrchHit **ret_hit)
{
   _DtHelpGlobSrchHit * prev;
   _DtHelpGlobSrchHit * next;
   _DtHelpGlobSrchHit * srcHit;
   _DtHelpGlobSrchVol * vol;
   int               newKey;
   extern _CEStrcollProc _DtHelpCeGetStrcollProc();
   _CEStrcollProc strcollfn = _DtHelpCeGetStrcollProc();
   
   if (NULL == curFile) return -1;                 /* RETURN */
   vol = (_DtHelpGlobSrchVol *) curFile->clientData;
   if (NULL == vol) return -1;                     /* RETURN */

   /* walk along the hits, looking for one matching the new hit */
   /* recall that position is 1-based */
   newKey = _DtHelpCeStrHashToKey(indexEntry);
   prev = next = NULL;
   if ( insertSorted )     /* find position and check for duplicates */
   {
      /* walk along list */
      for( next = vol->hitListHead;
           NULL != next; 
           prev = next, next = next->next )
      {
        register int ret;

        /* do a NLS case insensitive compare using NLS collating */
        if ( (ret = (*strcollfn)(next->indexEntry,indexEntry)) >= 0 )
        {
          if (0 == ret)
          {
             if(ret_hit) *ret_hit = next;
             return 0;                                  /* RETURN */
          }
          /* prev and next are set correctly */
          break;                                        /* BREAK */
        }
      }
   }
   else     /* check for duplicates */
   {
      /* walk along list */
      for( next = vol->hitListHead;
           NULL != next; 
           prev = next, next = next->next )
      {
        if (    newKey == next->indexKey                    /* quick compare */
             && strcmp(indexEntry,next->indexEntry) == 0 )  /* long compare */
        {
          if(ret_hit) *ret_hit = next;
          return 0;                                    /* RETURN */
        }
      }
      /* prev and next are set correctly (at end of list) */
   }

   /* handle a new hit */
   srcHit = (_DtHelpGlobSrchHit *)XtCalloc(1,sizeof(_DtHelpGlobSrchHit));
   if (NULL == srcHit) return -1;                         /* RETURN */

   /* init hit values */
   /* leave srcHit->hitCnt == 0 here */
   if (NULL == indexTitle)
      srcHit->indexTitle = XmStringCreateLocalized(indexEntry);
   else
      srcHit->indexTitle = indexTitle;
   srcHit->indexEntry = XtNewString(indexEntry);
   srcHit->indexKey = newKey;
   srcHit->volume = curFile;

   /* integrate hit into the list */
   srcHit->next = next;
   if (prev) prev->next = srcHit;
   else vol->hitListHead = srcHit;
   if (!next) vol->hitListTail = srcHit;

   /* add in the volume contribution */
   vol->hitCnt++;
  
   /* return stuff */
   if(ret_hit) *ret_hit = srcHit;
   return 0;
}



/************************************************************************
 * Function: CountSelectedVolumes()
 *
 *	Counts the number volumes with the searchThisVolume flag set
 *      for which the search has yet to complete
 *
 ************************************************************************/
static int CountSelectedVolumes (
    _DtHelpFileList  volListHead,
    Boolean countSearchCompletedVolumes)
{
  int               count = 0;
 
  /* walk all volumes */
  for ( /* nothing */;
        NULL != volListHead;
        volListHead = _DtHelpFileListGetNext(NULL, volListHead) )
  {
    register _DtHelpGlobSrchVol * vol;

    /* get the volume info */
    vol = (_DtHelpGlobSrchVol *) volListHead->clientData;
/* if (NULL != vol && vol->searchThisVolume && False == vol->searchCompleted)*/
    if (vol && vol->searchThisVolume)
    {
       if (   (False == vol->searchCompleted)
           || (countSearchCompletedVolumes && vol->searchCompleted) )
          count++;
    }
  } /* walk all volumes */
  return count;
}



/************************************************************************
 * Function: GetNextSearchFileAndDisplayCompleted()
 *
 *	Scans list for next file ready for searching
 *      If it encounters a file that has completed it's search
 *      and for which the results should be displayed, they are displayed
 *
 ************************************************************************/
static _DtHelpFileEntry GetNextSearchFileAndDisplayCompleted(
                 DtHelpDialogWidget hw,
                 _DtHelpFileList          listHead,
                 _DtHelpFileEntry         curFile)
{
    /* get first file needing work */
    for ( curFile = _DtHelpFileListGetNext(listHead, curFile);
          NULL != curFile;
          curFile = _DtHelpFileListGetNext(NULL, curFile) )
    {
       register _DtHelpGlobSrchVol * vol;

       vol = (_DtHelpGlobSrchVol *) curFile->clientData;
       if (NULL == vol) continue;                  /* CONTINUE */

       /* if file already searched and should be displayed, then do so */
       if (   vol->searchThisVolume && vol->searchCompleted 
           && vol->showVolInList && (vol->hitCnt > 0 || vol->zeroHitsOk) )
       {
          /* record that a hit found */
          if (   vol->hitCnt > 0
              || (    vol->zeroHitsOk 
                  && _DtHelpGlobSrchSelectedVolumes == hw->help_dialog.srch.srchSources))
             hw->help_dialog.srch.hitsFound = True;

          /* True: adjust count beginning with this file */
          AdjustPositionValues(curFile,0,1,True);
          ResultsListUpdate(hw,curFile);
   
          /* display the hits as well? */
          if (   vol->showHitsWithVol
              || hw->help_dialog.srch.srchSources == _DtHelpGlobSrchCurVolume)
          {
             VolHitsDisplay(hw,curFile);

             /* update the volume label to show state; False--dont insert */
             VolNameDisplay(hw,curFile,False);
          }
       }
   
       /* if want to search, and need to, then return it */
       if (    vol->searchThisVolume && False == vol->searchCompleted 
            && vol->showVolInList ) /* don' search unless results will be shown */
          break;
    }

    XmUpdateDisplay((Widget) hw->help_dialog.srch.resultList);
    return curFile;
}



/************************************************************************
 * Function: AddVolInfoToFile
 *
 *	Creates a volume info entry for each member of the list
 *      and initializes its values
 *      If searchStatus is False, all volumes are set.  If its True,
 *      all are set if selectedFilesOnly is false, otherwise only
 *      selected files are set to true.
 *
 ************************************************************************/
static void AddVolInfoToFile(
        _DtHelpFileEntry file,
        Boolean      initialSearchStatus,
        Boolean      displayStatus,
        Boolean      selectedFilesOnly)
{
   _DtHelpGlobSrchVol * vol;

   if (NULL == file) return;                     /* RETURN */

   vol = (_DtHelpGlobSrchVol *) file->clientData;
   if (NULL == vol)
   {
      vol = (_DtHelpGlobSrchVol *) XtCalloc(1,sizeof(_DtHelpGlobSrchVol));
      file->clientData = (XtPointer) vol;
      if (NULL == vol) return;       /* RETURN:  memory alloc error */

      /* calloc() inited almost everything inside to 0 */
      /* set first time creation values */
      vol->nothingDone = True;
   }

   /* now set search and display flags */
   vol->searchThisVolume = initialSearchStatus;
   vol->showVolInList = displayStatus;
   if ( False == file->fileSelected && True == selectedFilesOnly )
   {
      vol->searchThisVolume = False;
      vol->showVolInList = False;
   }
}


/************************************************************************
 * Function: AddVolInfoToList
 *
 *	Creates a volume info entry for each member of the list
 *      and initializes its values
 *      If searchStatus is False, all volumes are set.  If its True,
 *      all are set if selectedFilesOnly is false, otherwise only
 *      selected files are set to true.
 *
 ************************************************************************/
static void AddVolInfoToList(
        _DtHelpFileList listHead,
        Boolean      initialSearchStatus,
        Boolean      displayStatus,
        Boolean      selectedFilesOnly)
{
   /* walk the files and allocate the vol info as needed */
   for ( /*nothing*/;
         NULL != listHead;
         listHead = _DtHelpFileListGetNext(listHead,listHead) )
   {
       AddVolInfoToFile(listHead,
                 initialSearchStatus, displayStatus, selectedFilesOnly);
   }
}



/************************************************************************
 * Function: SetVolStatus()
 *
 *	Sets all volumes in the list to the specified search and/or display
 *      status values.
 *
 *      If searchThisVolume is False, all volumes are set.  If its True,
 *      all are set if selectedFilesOnly is false, otherwise only
 *      selected files are set to true.  If searchThisVolume is False,
 *      then the settings remain unchanged in all volumes.
 *
 ************************************************************************/
static void SetVolStatus(
        _DtHelpFileList listHead,
        Boolean      searchThisVolume,
        Boolean      showVolInList,
        Boolean      zeroHitsOk,
        Boolean      selectedFilesOnly)
{
   /* walk the files and set the status */
   for ( /*nothing*/;
         NULL != listHead;
         listHead = _DtHelpFileListGetNext(listHead,listHead) )
   {
     _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) listHead->clientData;

     if (NULL == vol) continue;

     if (    (False == selectedFilesOnly)
          || (True == listHead->fileSelected && True == selectedFilesOnly) )
     {
        /* just set the inclusion and/or display flags; 
           don't reset the search progress flags */
        vol->searchThisVolume = searchThisVolume;
        vol->showVolInList = showVolInList;
        vol->zeroHitsOk = zeroHitsOk;
     }
   }
}


/************************************************************************
 * Function: VolNameDisplay()
 *
 *    Updates the display of the volume name
 ************************************************************************/
static void  VolNameDisplay(
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file,
          Boolean           insertVol)
{
   _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) file->clientData;
   XmString prefixString;
   XmString labelString;
   char *   tmpStr;
   char     buf[40];

   if ( NULL == vol || (False == vol->zeroHitsOk && 0 == vol->hitCnt) )
       return;                                      /* RETURN */

   /* put volume name into list */
   if (vol->hitCnt <= 0) tmpStr = (char *)_DTGETMESSAGE (GSSET, 25," ");
   else if (vol->hitsDisplayed) tmpStr = (char *)_DTGETMESSAGE (GSSET, 27,"-");
   else tmpStr = (char *)_DTGETMESSAGE (GSSET, 26,"+");   /* >0 hits */
   sprintf(buf, "%s%3d ", tmpStr, vol->hitCnt);  /* 4d too spacy */

   LoadPrefixFont(hw);
   prefixString = XmStringGenerate(buf, s_PrefixFontListTag,
				   XmCHARSET_TEXT, NULL);
   labelString = XmStringConcat(prefixString,file->fileTitleXmStr);

   /* replace or insert to reflect changed contents */
   if (insertVol)
      XmListAddItemUnselected(hw->help_dialog.srch.resultList,
                                 labelString, vol->startPosition);
   else
      XmListReplaceItemsPosUnselected(hw->help_dialog.srch.resultList,
                                 &labelString, 1, vol->startPosition);

   XmStringFree(prefixString);
   XmStringFree(labelString);
}


/************************************************************************
 * Function: VolHitsDisplay()
 *
 *    displays the hits associated with a volume
 *    Returns the number of new items added to the list
 ************************************************************************/
static int VolHitsDisplay (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file)
{
     _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) file->clientData;
     _DtHelpGlobSrchHit * hit;
     int               listPos = vol->startPosition; /* vol item position */
     int               itemCnt;

     if (True == vol->hitsDisplayed ) return 0;        /* RETURN */

     _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

     /*** walk through the hits and count and display any open hits ***/
     for ( hit = vol->hitListHead, itemCnt = 0; 
           NULL != hit;
           hit = hit->next)
     {
        itemCnt++;           /* True: insert Hit into list */
        HitNameDisplay (hw,file,hit,listPos + itemCnt,True);

        /* display topics too? */
        if (hit->showTopicsWithHit)
        {
           int newItems;
           newItems = HitTopicsDisplay(hw,file,hit, listPos + itemCnt + 1);
           HitNameDisplay (hw,file,hit,listPos + itemCnt,False);
           itemCnt += newItems;
        }
     }

     /* new state */
     vol->hitsDisplayed = True;
     vol->showHitsWithVol = True;
     
     /* adjust count beginning with next file with hits */
     AdjustPositionValues(file, 0, itemCnt, True);

     _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

     return itemCnt;
}



/************************************************************************
 * Function: VolHitsUndisplay()
 *
 *    undisplays the hits associated with a volume
 ************************************************************************/
static int VolHitsUndisplay (
          DtHelpDialogWidget hw,
          _DtHelpFileEntry     file)
{
   _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) file->clientData;
   _DtHelpGlobSrchHit * hit;
   Boolean           nonVisibleItems;
   int               listPos = vol->startPosition; /* vol item position */
   int               itemCnt;
   int               curDelPos;
   int               undisItemCnt;
   int               visItemCnt;
   int               topNonVisPos;
   int               topPos;
   Arg               args[5];

   if (False == vol->hitsDisplayed) return 0;         /* RETURN */

   _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));
   if (hw->help_dialog.srch.selectionDlg)
      _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

   XtSetArg(args[0], XmNtopItemPosition, &topPos);
   XtSetArg(args[1], XmNvisibleItemCount, &visItemCnt);
   XtGetValues(hw->help_dialog.srch.resultList,args,2);

   /* these are the num visible items below the volume name */
   topNonVisPos = topPos + visItemCnt;   /* 1st non vis pos */

   /* num of items that will be undisplayed */
   undisItemCnt = vol->nextVolPosition - (vol->startPosition + 1);

   /* find out if any items aren't visible */
   nonVisibleItems = False;
   if ( vol->nextVolPosition > topNonVisPos )
      nonVisibleItems = True;

   /*** walk through the hits and count and delete any open topics ***/
   /* Make two passes:  the first pass deletes all non-visible
      items.  The second pass deletes the remaining visible items.
      The objective is to make collapse as fast and smooth as expand. */
   if (nonVisibleItems)
   {  /* delete all items with a position > topNonVisPos */
      curDelPos = listPos + 1;   /* starting position */
      for ( hit = vol->hitListHead, itemCnt = 0; 
            NULL != hit;
            hit = hit->next )
      {
         /* are we still in the visible region? */
         if (curDelPos < topNonVisPos)
         {
            /* move down the list to next hit */
            curDelPos++;         /* this hit's item */
            /* plus any open topics */
            if (hit->topicsDisplayed) curDelPos += hit->topicCnt;
            continue;                           /* CONTINUE */
         }
         /* we're in non-visible region--undisplay hit */
         XmListDeletePos(hw->help_dialog.srch.resultList, curDelPos);
         /* undisplay any topics */
         if ( hit->topicsDisplayed )
            itemCnt += HitTopicsUndisplay(hw,file,hit,curDelPos);
      }
   } /* if non visible items */

   /* now delete the remaining visible items */
   /* Note that curDelPos is a virtual cursor that helps us
      figure out when to stop undisplaying.  Virtually, we''re
      moving the cursor down the list with every delete.  Actually,
      the rest of the list bumps up one--that''s why we delete
      at listPos+1. */
 
   curDelPos = listPos + 1;   /* starting position */
   for ( hit = vol->hitListHead, itemCnt = 0; 
         (NULL != hit) && (curDelPos < topNonVisPos);
         hit = hit->next )
   {
      curDelPos++;      /* undisplay hit item */
      XmListDeletePos(hw->help_dialog.srch.resultList, listPos + 1);
      /* undipslay any open topics */
      if ( hit->topicsDisplayed )
      {
         curDelPos += HitTopicsUndisplay(hw,file,hit,listPos + 1);
         XmListSetPos(hw->help_dialog.srch.resultList, topPos);
      }
   }

   /* new state */
   vol->hitsDisplayed = False;
   /* NOTE: don't reset vol->showHitsWithVol here; require that
      to be explicitly reset in ProcessResultSelection() */
   
   /* adjust count beginning with next file with hits */
   AdjustPositionValues(file, 0, -undisItemCnt, True);

   XmListSetPos(hw->help_dialog.srch.resultList, topPos);

   _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));
   if (hw->help_dialog.srch.selectionDlg)
      _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

   return undisItemCnt;
}



/************************************************************************
 * Function: VolListFree()
 *
 *	Releases all memory used by the volume list
 *
 ************************************************************************/
static void VolListFree (
    _DtHelpGlobSearchStuff * srch)
{
  _DtHelpFileList      nextFile;
 
  /* walk all volumes */
  nextFile = srch->volListHead;
  while ( NULL != nextFile )
  {
  _DtHelpGlobSrchVol * vol;
  _DtHelpFileList      tmpFile;
  XmString *           nextStr;

    /* get the volume info */
    vol = (_DtHelpGlobSrchVol *) nextFile->clientData;
    
    /** free vol entry contents and container **/
    XtFree(vol->stdLocale);
    XtFree(vol->iconv3Codeset);
    HitListFree(vol,True);           /* True: free hits themselves */

    /* no need to free indexEntriesList because these aren't owned by vol */
    vol->indexEntriesList = NULL;

    /* free indexXmStrsList */
    /* indexXmStrs are XmStrings and we can't use FreeStringArray() */
    for ( nextStr = vol->indexXmStrsList;
          NULL != nextStr && NULL != *nextStr; 
          nextStr++)
       XmStringFree (*nextStr);
    XtFree((char *) vol->indexXmStrsList); /* its a XmString * */
    vol->indexXmStrsList = NULL;

    tmpFile = nextFile;
    nextFile = _DtHelpFileListGetNext(NULL, nextFile);
    _DtHelpFileFreeEntry(tmpFile);     /* also frees the clientData (vol) */
  } /* walk all volumes */

  /* reset related values */
  srch->volListHead = NULL;
  srch->curSrchVol = NULL;
  srch->volLeftCnt = 0;

  /* delete the results */
  DeleteListContents(srch);
}



/************************************************************************
 * Function: ScanStatusCB
 *
 *     Updates the status display with the number of volumes found
 *     as the scan progresses
 *
 ************************************************************************/
static void ScanStatusCB(
          int           count,
          XtPointer     clientData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;

   StatusLabelUpdate(hw,SCANNING_STATUS,True,count);
}


/************************************************************************
 * Function: VolListBuild
 *
 *     Frees the old list and builds a new one by scanning the
 *     help directories for volumes and adding the associated
 *     volume information used for searching.
 *
 ************************************************************************/
static void VolListBuild(
          DtHelpDialogWidget hw,
          Boolean               searchStatus,
          Boolean               displayStatus,
          Boolean               selectedVolumesOnly)
{
    Boolean mod;
    XmFontList origFontList;

    /* turn on hour glass */
    _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

    StatusLabelUpdate(hw,SCANNING_STATUS,True,0);
     
    origFontList = hw->help_dialog.srch.volTitlesFontList;

    /* and rescan the volumes for the new list */
   _DtHelpFileListScanPaths( &hw->help_dialog.srch.volListHead,
                     &hw->help_dialog.srch.volTitlesFontList,&mod,
                     DtHelpVOLUME_TYPE, _DtHelpFileSuffixList, False, 
                     GetVolumeInfoCB,hw->help_dialog.help.pDisplayArea,
/* sysPathCompare */ _DtHELP_FILE_NAME, 
/*otherPathCompare*/ _DtHELP_FILE_NAME|_DtHELP_FILE_TIME| _DtHELP_FILE_IDSTR, 
/* sortBy */         _DtHELP_FILE_TITLE, ScanStatusCB, (XtPointer)hw);
  
    /* if new fonts added to list, add them to the resultList */
    if (mod)
    {
        MergeFontListIntoWidgetFonts( 
              hw->help_dialog.srch.resultList,
              hw->help_dialog.srch.volTitlesFontList);

        if (origFontList) XmFontListFree(origFontList);
    }

    /* Add on the Vol info and (T,F) allow search on all volumes */
    AddVolInfoToList(hw->help_dialog.srch.volListHead, 
                           searchStatus, displayStatus, selectedVolumesOnly);

    hw->help_dialog.srch.volScanDone = True;

    StatusLabelUpdate(hw,BLANK_STATUS,True,0);

    /* turn off hour glass */
    _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));
}



#if 0 || defined(DEBUG)	/* for debug, set to 1 */
/************************************************************************
 * Function: OutputVolInfo()
 *
 *	Output info on all volumes in the list
 *
 ************************************************************************/
static void OutputVolInfo(
        _DtHelpFileList listHead)
{
   for ( /*nothing*/;
         NULL != listHead;
         listHead = _DtHelpFileListGetNext(listHead,listHead) )
   {
     _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) listHead->clientData;
     if (NULL == vol) continue;

     /* output the info */
     printf("==================\n");
     printf("fileName: %s, %s\npathName: %s\n",
         listHead->fileName, listHead->fileTitle, listHead->fullFilePath);
     printf("searchThisVolume = %d, hitCnt = %d, startPos = %d\n",
         (int) listHead->fileSelected,
         (int) vol->hitCnt,
         (int) vol->startPosition);
   }
}
#endif


/************************************************************************
 * Function: SearchContinuePossible()
 *
 *	Test whether the search can be continued from the
 *      current state.
 *
 ************************************************************************/
static Boolean SearchContinuePossible(
   DtHelpDialogWidget hw,
   char *             srchWord)
{
    char *  normSrchWord;
    Boolean possible = False;

    normSrchWord = XtNewString(srchWord);
    _DtHelpCeCompressSpace(normSrchWord);
    _DtHelpCeUpperCase(normSrchWord);

   /* are we searching on the same word as previous search ? */
   /* comparison with srchNormWordStr is correct, given calls to this fn */
/* FIX: use CheckSearchWord()? */
   if (   (   hw->help_dialog.srch.fullIndex
           || (   NULL != hw->help_dialog.srch.normWordStr
               && NULL != normSrchWord
               && strcmp(normSrchWord, hw->help_dialog.srch.normWordStr) == 0) )
        && NULL != hw->help_dialog.srch.volListHead 
        && hw->help_dialog.srch.volLeftCnt > 0 )
       possible = True;

    XtFree(normSrchWord);
    return possible;
}


#ifdef	not_used
/************************************************************************
 * Function: SearchOnSameCriteria()
 *
 *	Test whether the search criteria have changed
 *
 ************************************************************************/
static Boolean SearchOnSameCriteria(
   DtHelpDialogWidget hw,
   char *             srchWord)
{
    char *  normSrchWord;
    Boolean sameCrit = False;

    normSrchWord = XtNewString(srchWord);
    _DtHelpCeCompressSpace(normSrchWord);
    _DtHelpCeUpperCase(normSrchWord);

   /* are we searching on the same word as previous search ? */
   /* comparison with srchNormWordStr is correct, given calls to this fn */
   if (   hw->help_dialog.srch.fullIndex
       || (   NULL != hw->help_dialog.srch.normWordStr
           && NULL != normSrchWord
           && strcmp(normSrchWord, hw->help_dialog.srch.normWordStr) == 0) )
       sameCrit = True;

    XtFree(normSrchWord);
    return sameCrit;
}
#endif /* not_used */




/************************************************************************
 * Function: UpdateActionButtonLabel()
 *
 *	Sets the start button label properly
 *
 ************************************************************************/
static void UpdateActionButtonLabel(
   DtHelpDialogWidget hw,
   char * srchWord,
   Boolean startContAction)
{
   Arg        args[5];
   XmString   labelString;
   XmString   curLabelString;
   String     textString;
   /* char *     mnemonic; */

   /* if action label is to start or continue */
   _DtHelpProcessLock();
   if (startContAction)
   {
      if ( SearchContinuePossible(hw,srchWord) )
      {  /* then continue search */
         textString=(char *)_DTGETMESSAGE (GSSET, 
                                 CONT_SEARCH_CAT,CONT_SEARCH_STR);
         labelString = XmStringCreateLocalized(textString);
         /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 22,CONT_SEARCH_MNEM));*/
      }
      else
      {  /* otherwise start it */
         textString=(char *)_DTGETMESSAGE (GSSET,
                                 START_SEARCH_CAT,START_SEARCH_STR);
         labelString = XmStringCreateLocalized(textString);
         /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 20,START_SEARCH_MNEM));*/
      }
   }
   else    /* action label is to stop */
   {
      textString = (char *)_DTGETMESSAGE (GSSET, 
                                 STOP_SEARCH_CAT,STOP_SEARCH_STR);
      labelString = XmStringCreateLocalized(textString);
      /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, ??,STOP_SEARCH_MNEM));*/
   }
   _DtHelpProcessUnlock();

   /* get the current state of the button */
   XtSetArg(args[0],XmNlabelString,&curLabelString);
   XtGetValues (hw->help_dialog.srch.actionBtn, args, 1);

   /* update label if it is different; avoid flashing this way */
   if ( XmStringCompare(labelString,curLabelString) == False )
   {
      XtSetArg(args[0],XmNlabelString,labelString);
      XtSetArg(args[1], XmNalignment, XmALIGNMENT_CENTER);
/*    XtSetArg(args[2], XmNmnemonic, mnemonic[0]);*/
      XtSetValues (hw->help_dialog.srch.actionBtn, args, 2);
      XmUpdateDisplay(hw->help_dialog.srch.actionBtn);
   }

   /* release the memory */
   XmStringFree(labelString);
   XmStringFree(curLabelString);
}


/*****************************************************************************
 * Function:	    CheckSearchWord()
 *
 * Parameters:  
 *
 * Return Value:    True: word has changed
 *                  False: word is same
 *
 * Purpose: 	    Checks whether the search word is a new
 *                  one or the same as before.  If the same, it frees
 *                  the word and returns False.  If new, it moves the
 *                  previous word to another variable and installs
 *                  the new word in its place, using the srchWord mem.
 *                  ...but only if updateWidgetFields is True
 *
 *****************************************************************************/
static Boolean CheckSearchWord(
         DtHelpDialogWidget   hw,
         char *                srchWord,
         Boolean               updateWidgetFields)
{
     wchar_t firstChar;
     char *  rawSrchWord;

     /* avoid a core dump */
     if (   NULL == srchWord )
     {
        if (    NULL == hw->help_dialog.srch.normWordStr
             || hw->help_dialog.srch.normWordStr[0] == 0) 
            return False; /* word hasn't changed */
        return True;                           /* RETURN: word has changed */
     }

     _DtHelpCeCompressSpace(srchWord);
     rawSrchWord = XtNewString(srchWord);
     _DtHelpCeUpperCase(srchWord);
     /* FIX: to support regcomp(3), do that here */

     /* are we searching on the same word? */
     if (    NULL != hw->help_dialog.srch.normWordStr
          && strcmp(srchWord, hw->help_dialog.srch.normWordStr) == 0 )
     {
         XtFree(srchWord);
         XtFree(rawSrchWord);
#if 0
         /* output a message */
         if (    NULL != hw->help_dialog.srch.normWordStr 
              && EOS != hw->help_dialog.srch.normWordStr[0])
            StatusLabelUpdate(hw,FIRST_PROMPT_STATUS,False,0);
#endif

         return False;                           /* RETURN: no change */
     }
     
     if (False == updateWidgetFields) return True; /* RETURN */

     /* update search word fields */
     XtFree(hw->help_dialog.srch.normWordStr);
     XtFree(hw->help_dialog.srch.rawWordStr);
     hw->help_dialog.srch.normWordStr = srchWord;
     hw->help_dialog.srch.rawWordStr = rawSrchWord;
     hw->help_dialog.srch.wordFieldLen = strlen(srchWord); /* note: not nl_strlen 
                                  because used on strncmp not nl_strncmp */

     /* convert first char to a value for easy access for _DtHelpStrchr() */
     if ( mbtowc(&firstChar,srchWord,1) <= 0 ) firstChar = 0;
     hw->help_dialog.srch.wordFieldFirstChar = firstChar;

     /* output a message */
     if (    NULL != hw->help_dialog.srch.normWordStr 
          && EOS != hw->help_dialog.srch.normWordStr[0])
        StatusLabelUpdate(hw,FIRST_PROMPT_STATUS,False,0);

     return True;  /* word changed */
}



/*****************************************************************************
 * Function:	    PrepSearchSourceData()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Based on the current settings of the volume
 *                  selection buttons construct or utilitize an
 *                  existing volume list for processing.
 *                  Determine whether to continue search or
 *                  restart by comparing search words and states.
 *
 *****************************************************************************/
static void PrepSearchSourceData(
    DtHelpDialogWidget hw,
    char * srchWord)
{
     /* try to install the new word; if returns False, word is same 
        as before. True = also update widget fields. */
     /* This test determines whether we might need to continue. */
     if (    hw->help_dialog.srch.fullIndex
          || CheckSearchWord(hw,srchWord,True) == False )
     {
	/* check to see if CheckSearchWord() was called. If it wasn't
	   then srchWord has not been freed or saved. We will lose the
	   memory unless we free it here. */
	if (hw->help_dialog.srch.fullIndex)
	    XtFree(srchWord);

        /* are we continuing an interrupted search? */
        if (    hw->help_dialog.srch.volLeftCnt > 0
             && NULL != hw->help_dialog.srch.volListHead )
            return;                       /* RETURN: continue with search */

        /* if searching full index, False=dont free vol with full index */
        if (hw->help_dialog.srch.fullIndex)
            HitListFreeAllVolHits(hw,False);
        /* else we're searching on the same word, but completed 
           previous search.  So reset the volumes to search/display 
           according to the current settings of the Volumes radio buttons,
           but leave the results intact.
        */
        /* fall thru to do prep */
     }
     else /* searching for a different word */
     {
        /* free all hit-related data and reset flags */
        HitListFreeAllVolHits(hw,True);  /*True=free everything*/
     }

     /*** It's not just a continue.  So, we may be beginning a new search 
          or continuing a search based on new volume selection params ***/

     /* update display area */
     DeleteListContents(&hw->help_dialog.srch);
     StatusLabelUpdate(hw,WORKING_STATUS,False,0);

     /*** update the volumes list according to sources selected ***/
     if ( _DtHelpGlobSrchSelectedVolumes == hw->help_dialog.srch.srchSources )
     { 
         /* volume list already built when opening selection dialog */
         /* (F,F,F,F): disable search,disable display,no 0 hits,for all vols */
         SetVolStatus(hw->help_dialog.srch.volListHead,False,False,False,False);
         /* (T,T,T,T): enable search, enable display, 
                zero hits ok, only for selected volumes */
         SetVolStatus(hw->help_dialog.srch.volListHead,True,True,True,True);
     }   /* if selected volumes button */
     else if ( _DtHelpGlobSrchAllVolumes == hw->help_dialog.srch.srchSources )
     {
        /* Scan directories for volumes if necessary */
        if (    False == hw->help_dialog.srch.volScanDone
             || NULL == hw->help_dialog.srch.volListHead)
        {
           /* Add on the Vol info; (T,T,F) search, display, all */
           VolListBuild(hw,True,True,False);
        }

        /* (T,T,F,F): enable search,enable display,no 0 hits,for all vols */
        SetVolStatus(hw->help_dialog.srch.volListHead,True,True,False,False);
     } /* if all volumes button */
     else    /*** current volume button ***/
     {
       char * path;
       _DtHelpFileEntry cur;

       /* (F,F,F,F): disable search,disable display,no 0 hits,for all vols */
       SetVolStatus(hw->help_dialog.srch.volListHead,False,False,False,False);

       /* try to locate file and its entry, if present */
       path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, hw->help_dialog.display.helpVolume,
                                  _DtHelpFileSuffixList,False,R_OK);
       cur = _DtHelpFileListGetMatch(hw->help_dialog.srch.volListHead,path,
                      GetVolumeInfoCB, _DtHELP_FILE_NAME,
                      hw->help_dialog.help.pDisplayArea);

       if (NULL == cur && NULL != path)  /*file was located but isn't in list*/
       {  /* add it to the list */
          Boolean mod;
          XmFontList origFontList;

          origFontList = hw->help_dialog.srch.volTitlesFontList;

          _DtHelpFileListAddFile(&hw->help_dialog.srch.volListHead,
                       &hw->help_dialog.srch.volTitlesFontList,&mod,path,NULL,
                       GetVolumeInfoCB, _DtHELP_FILE_NAME, _DtHELP_FILE_TITLE,
                       hw->help_dialog.help.pDisplayArea);
          if (mod)
          {
             MergeFontListIntoWidgetFonts( 
                       hw->help_dialog.srch.resultList,
                       hw->help_dialog.srch.volTitlesFontList);

             if (origFontList) XmFontListFree(origFontList);
          }

          /* (F,F,F): disable search, disable display, for all volumes */
          AddVolInfoToList(hw->help_dialog.srch.volListHead,False,False,False);
          cur = _DtHelpFileListGetMatch(hw->help_dialog.srch.volListHead,path,
                      GetVolumeInfoCB, _DtHELP_FILE_NAME,
                      hw->help_dialog.help.pDisplayArea);
       }

       if (cur) /* file present in list */
       {
          /* enable search and display on this volume */
          _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) cur->clientData;
          
          vol->searchThisVolume = True;
          vol->showVolInList = True;
          vol->zeroHitsOk = True;    /* show zero hits for search of cur vol */
       }
       else      /* file not found */
       {
          /* FIX: put up error dialog and disable the button */
          /*printf("Current volume %s not found\n", 
                               hw->help_dialog.display.helpVolume);*/
       } /* if file found */

       XtFree(path);                  /* tests for NULL */
     }  /* if selected/all/cur volumes sources */

#if 0 || defined(DEBUG)	/* for debug, set to 1 */
     OutputVolInfo(hw->help_dialog.srch.volListHead);
#endif

     /* get num volumes to process */
     hw->help_dialog.srch.volLeftCnt = 
             CountSelectedVolumes(hw->help_dialog.srch.volListHead,True);

     /* empty the results list */
     XmUpdateDisplay(hw->help_dialog.srch.resultList);
}


/************************************************************************
 * Function: CloseSearchCB()
 *
 *	Close the Global Search Dialog
 *
 ************************************************************************/
static void CloseSearchCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   _DtHelpGlobSearchStuff * srch = (_DtHelpGlobSearchStuff *) clientData;

   /* stop any work proc */
   if(srch->workProcId)
   {
      XtRemoveWorkProc(srch->workProcId);
      srch->workProcId = 0;
   }

   /* unmanage the srchForm dialog but dont destroy it */
   if (NULL != srch->srchForm)
     {
       XtUnmanageChild(srch->srchForm);
       XtUnmapWidget(srch->srchForm);
     }
}


/*****************************************************************************
 * Function:	    void DeleteListContents()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    empties list and resets position values
 *
 *****************************************************************************/
static void DeleteListContents(
        _DtHelpGlobSearchStuff * srch)
{
   _DtHelpFileEntry  curFile;

   if (NULL != srch->resultList)
       XmListDeleteAllItems(srch->resultList);

   for ( curFile = srch->volListHead;
         NULL != curFile;
         curFile = _DtHelpFileListGetNext(NULL,curFile) )
   {
      register _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) curFile->clientData;
      if (NULL == vol) continue;                      /* CONTINUE */

      /* if any hits are displayed and any have open topics, 
         walk through and close them */
      if (    vol->hitsDisplayed 
           && ((vol->nextVolPosition - vol->startPosition) != vol->hitCnt) )
      {
         register _DtHelpGlobSrchHit * hit;
         for ( hit = vol->hitListHead;
               NULL != hit;
               hit = hit->next)
         {
            hit->topicsDisplayed = False;
         }
     }

      vol->startPosition = 1;
      vol->nextVolPosition = 1;
      vol->hitsDisplayed = False;
   }
}


/*****************************************************************************
 * Function:	    void StatusLabelUpdate()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Installs proper label over results list
 *
 *****************************************************************************/
static void StatusLabelUpdate(
        DtHelpDialogWidget hw,
        ResultsStatus      status,
        Boolean            forceUpdate,
        int                intArg)
{
   char *        labelMsg;
   XmString      labelString;
   XmString      curLabelString;
   Arg           args[5];
   char          buf[100];

#if 0
   /* only update if necessary */
   if (status == hw->help_dialog.srch.statusLineUsage && !forceUpdate)
      return;
#endif

   /* order up the right message */
   hw->help_dialog.srch.statusLineUsage = status;
   switch (status)
   {
   case SCANNING_STATUS:
       labelMsg = (char *)_DTGETMESSAGE(GSSET, 17,"Scanning for volumes... Found %d");
       sprintf(buf,labelMsg,intArg);
       labelMsg = buf;
       break;                                              /* BREAK */
   case WORKING_STATUS:
       labelMsg = (char *)_DTGETMESSAGE(GSSET, 18,
                                  "Searching... Volumes remaining: %d");
       /* we need +1 because of when this message is used */
       sprintf(buf,labelMsg,hw->help_dialog.srch.volLeftCnt + 1);
       labelMsg = buf;
       break;                                              /* BREAK */
   case SEARCH_RESULTS_STATUS:
       /* if no hits found */
       if (hw->help_dialog.srch.hitsFound == False) /*dont use == True*/
       {
          if (hw->help_dialog.srch.fullIndex)
          {
             if (hw->help_dialog.srch.srchSources == _DtHelpGlobSrchAllVolumes)
                labelMsg = (char *)_DTGETMESSAGE (GSSET,19,
                        "No index entries found.");
             else
                labelMsg = (char *)_DTGETMESSAGE (GSSET,50,
                        "No index entries found.  Try All Volumes.");
          }
          else  /* searched on a pattern */
          {
              labelMsg = (char *)_DTGETMESSAGE (GSSET,51,
                        "No index entries found.  Try another word.");
          }
          /* change focus to the search word.  Traversal is ignored 
             if srchWord is desensitized (Contains btn not selected) */

          if (XtIsSensitive(hw->help_dialog.srch.wordField))
	  {
	    /*
	     * make sure the text field is editable before sending
	     * the traversal there.
	     */
	    XtSetArg(args[0],XmNeditable,True);
	    XtSetValues(hw->help_dialog.srch.wordField, args, 1);
            XmProcessTraversal
               (hw->help_dialog.srch.wordField,XmTRAVERSE_CURRENT);
	  }
          else
            XmProcessTraversal
               (hw->help_dialog.srch.allVolRadBtn,XmTRAVERSE_CURRENT);

       }
       else /* if hits found */
       {
          /* select the appropriate message */
          if (hw->help_dialog.srch.fullIndex)
          {
             labelMsg = (char *)_DTGETMESSAGE(GSSET, 20,"Complete Index");

           }
          else
          {
             labelMsg = (char *)_DTGETMESSAGE(GSSET, 21,"Entries with \"%s\"");
             sprintf(buf,labelMsg,hw->help_dialog.srch.rawWordStr);
             labelMsg = buf;

          }
             /* change focus to the results list */
             XmProcessTraversal(
                      hw->help_dialog.srch.resultList,XmTRAVERSE_CURRENT);
 

      } /* if no hits found else */
        
       break;                                              /* BREAK */
   case NO_VOL_STATUS:
       labelMsg = (char *)_DTGETMESSAGE (GSSET, 22,"No volumes selected");
       break;                                              /* BREAK */
   case FIRST_PROMPT_STATUS:
       labelMsg = (char *)_DTGETMESSAGE(GSSET,23,"Select search options above");
       break;                                              /* BREAK */
   case BLANK_STATUS:
   default:
      /* DBG: when an empty string, form constraints are wrong; make a space */
       labelMsg = " ";
       break;                                              /* BREAK */
   }

   /* make a label string */
   labelString = XmStringCreateLocalized(labelMsg);

   /* get the current string of the label */
   curLabelString = NULL;
   XtSetArg(args[0],XmNlabelString,&curLabelString);
   XtGetValues (hw->help_dialog.srch.statusLabel, args, 1);

   /* update label if it is different; avoid flashing this way */
   if (    NULL != curLabelString && NULL != labelString
        && XmStringCompare(labelString,curLabelString) == False )
   {
      /* update msg over results list */
      XtSetArg(args[0],XmNlabelString,labelString);
      XtSetValues(hw->help_dialog.srch.statusLabel,args,1);
   }

   XmStringFree(labelString);
   XmStringFree(curLabelString);

   XmUpdateDisplay(hw->help_dialog.srch.statusLabel);
}




/*****************************************************************************
 * Function:	    void ResultsListUpdate()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    empty list and display results status message
 *
 *****************************************************************************/
static void ResultsListUpdate(
        DtHelpDialogWidget   hw,
        _DtHelpFileEntry     newFile)
{
   _DtHelpFileEntry testFile;

   /* check that in fact, there are no results that will be overwritten */
   for ( testFile = hw->help_dialog.srch.volListHead;
         NULL != testFile;
         testFile = _DtHelpFileListGetNext(NULL,testFile) )
   {
      register _DtHelpGlobSrchVol * vol;
      vol = (_DtHelpGlobSrchVol *)testFile->clientData;
      if (vol && vol->showVolInList && (vol->hitCnt > 0  || vol->zeroHitsOk))
         break;                                            /* BREAK */
   }

   /* clean the list out if this is the first new result */
   /* this presumes that files are processed in order in the list */
   if (testFile == newFile)
   {
      DeleteListContents(&hw->help_dialog.srch);
      AdjustPositionValues(newFile,0,1,True); 
   }
   /* True: insert new item */
   VolNameDisplay(hw,newFile,True);

   /* Update status label */
   StatusLabelUpdate(hw,SEARCH_RESULTS_STATUS,True,0); /* FIX: False */
}



/*****************************************************************************
 * Function:	    void SearchForPattern()
 *
 * Parameters:  
 *
 * Return Value:    True if pattern found, False if not found
 *
 * Purpose: 	    Find a pattern in a string
 *
 * WARNING:         may convert string to uppercase in place
 *                  depending on which code is compiled.
 *****************************************************************************/
static Boolean SearchForPattern(
          char * string,
          char * pattern,
          int     patternLen)
{
#if defined(NO_REGEX)  /* don't support regex in string searches */
       char * hit;

       _DtHelpCeUpperCase(string);
       /* FIX: make this code much faster by using an improved
          pattern search algorithm, such as Rabin-Karp. */
       /* hit is set to NULL if no matching char is found */
       while (_DtHelpCeStrchr(string, pattern, MB_CUR_MAX, &hit) == 0)
       {
          if ( strncmp(hit,pattern,patternLen) == 0 )
             break;                             /* BREAK */
          else
             string = hit + mblen(hit,1);
       }
       return (NULL != hit);                    /* RETURN True or False */
#else /* if NO_REGEX */
# ifndef NO_REGCOMP
        regex_t    re;
        int        ret = -1;

        /* use regexec to pattern match */
        /* a 0 return value indicates success */
        if ( regcomp(&re,pattern,REG_NOSUB|REG_ICASE|REG_EXTENDED) == 0 )
        {
           /* a 0 return value indicates success */
           ret = regexec(&re,string,0,NULL,0);
           regfree(&re);
        }
        return (ret == 0);                      /* RETURN True or False */
# else  /* NO_REGCOMP */
        char *compiledRE;
        char *ret = NULL;

        /* a non NULL return value indicates success */
        compiledRE = (char *)regcmp(pattern, (char *) NULL);
        if (compiledRE)
        {
           /* a non NULL return value indicates success */
	   ret = regex(compiledRE, string);
	   free(compiledRE);
        }
        return (ret != NULL);	                /* RETURN True or False */
# endif /* NO_REGCOMP */
#endif /* if NO_REGEX */
}


/*****************************************************************************
 * Function:	    void OpenVolForSearch()
 *
 * Parameters:  
 *
 * Return Value:    False if an error
 *                  True if no error
 *
 * Purpose: 	    Opens a volume for searching, 
 *                  gets the volume locale, if 
 *                  different from system locale,
 *                  saves system locale in vol,
 *                  installs volume locale
 *
 *                  Note that the system locale
 *****************************************************************************/
static Boolean OpenVolForSearch(
           DtHelpDialogWidget  hw,
           char *               fullFilePath,
           _DtHelpGlobSrchVol * curVol)
{
    Boolean openedVolume = False;

    /* open the volume if not open */
    if ( NULL == curVol->volHandle )
    {
       _DtHelpOpenVolume(fullFilePath, &curVol->volHandle);
       if (NULL == curVol->volHandle) return False;  /* RETURN */

       if (curVol->gotLocale == False)
       {
          curVol->stdLocale = _DtHelpGetVolumeLocale(curVol->volHandle);
          /* get the op-specific locales; the strings are default values */
          _DtHelpCeXlateStdToOpLocale(DtLCX_OPER_ICONV3,curVol->stdLocale,
          		"iso88591",&curVol->iconv3Codeset);
          curVol->gotLocale = True;
       }
       openedVolume = True;

       /* get the codeset of the application's locale, if haven't gotten it */
       if ( NULL == hw->help_dialog.srch.iconv3Codeset )
       {
          char * locale = NULL;
          _DtHelpCeXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
                     setlocale(LC_CTYPE,NULL),&locale,NULL,NULL);
          /* get the op-specific locales; the strings are default values */
          /* impt: XlateStdToOpLocale() call requires a locale, not a codeset */
          _DtHelpCeXlateStdToOpLocale(DtLCX_OPER_ICONV3,locale,
       		"iso88591",&hw->help_dialog.srch.iconv3Codeset);
          XtFree(locale);
       }
   
       /** only saves or changes the locale if necessary **/
       /* create an iconv3 context to convert codeset of */
       /* the volume to the codeset of the application */
       if ( _DtHelpCeIconvContextSuitable(hw->help_dialog.srch.iconv3Context,
               curVol->iconv3Codeset,hw->help_dialog.srch.iconv3Codeset) == False )
       {
          /* close old, open new */
          _DtHelpCeIconvClose(&hw->help_dialog.srch.iconv3Context);  
          _DtHelpCeIconvOpen(&hw->help_dialog.srch.iconv3Context,
                 curVol->iconv3Codeset,hw->help_dialog.srch.iconv3Codeset,' ',' ');
       }
    }
    /* else volume already open */

    /* if just opened volume */
    if (openedVolume)
    {
       /* we just share pointers; when freeing localeWordStr, we need to
          test whether they are different before freeing */
       hw->help_dialog.srch.localeWordStr = hw->help_dialog.srch.normWordStr;
    }

    /* get the str coll function, iff needed */
    if (NULL == curVol->strcollProc)
    {
       /* get strcollProc for vol locale */
       curVol->strcollProc = _DtHelpCeGetStrcollProc();
    }

    return True;
}


/*****************************************************************************
 * Function:	    void CloseVolAfterSearch()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Closes a volume opened for searching, 
 *                  installs system locale, if diff from volume.
 *
 *****************************************************************************/
static void CloseVolAfterSearch(
           DtHelpDialogWidget  hw,
           _DtHelpGlobSrchVol *    curVol)
{
    /* close the volume if open */
    if ( NULL != curVol->volHandle )
    {
       _DtHelpCloseVolume(curVol->volHandle);
       curVol->volHandle = NULL;
       /* free the locale word string, if allocated */
       if(hw->help_dialog.srch.localeWordStr!=hw->help_dialog.srch.normWordStr)
       {
          XtFree(hw->help_dialog.srch.localeWordStr);
          hw->help_dialog.srch.localeWordStr = NULL;
       }
    }
   
    /* don't free the iconv context...may apply to the next volume */
}



/*****************************************************************************
 * Function:	    void SearchTopic()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Get a single topic and search for the search word in it
 *
 *****************************************************************************/
static void SearchTopic(
   DtHelpDialogWidget hw,
   _DtHelpFileEntry     curFile,
   _DtHelpGlobSrchVol * curVol,
   char *            srchWord,
   int               srchWordLen,
   int               numSearches)
{
#if 0
    int i;

    /* if the volume is not open; don't continue */
    if ( NULL == curVol->volHandle )
       return;                                  /* RETURN */

    if ( NULL == curVol->indexEntriesList )
       _CEVolumeTopicList(curVol->volHandle,&curVol->indexEntriesList);
    if ( NULL == curVol->volHandle || NULL == curVol->indexEntriesList )
    {
       curVol->topicSearchInProgress = False;
       curVol->topicSearchDone = True;
       curVol->indexSearchInProgress = True;
       if ( NULL == curVol->volHandle ) curVol->searchCompleted = True;
       return;                                 /* RETURN */
    }

    curVol->topicSearchInProgress = True;


    /* search the topics */
    /* note that indexEntriesList[0] is correct; ptr is incremented */
    /* we can increment the pointer because we don't own this memory;
       the open volume does. */
    for (i=0; 
         i < numSearches && NULL != curVol->indexEntriesList[0]; 
         i++, curVol->indexEntriesList++ )
    {
       char * topicTitle = NULL;
       Boolean validTopic;

       validTopic = _DtHelpGetTopicTitle( curVol->volHandle,
                         curVol->indexEntriesList[0],&topicTitle);
       if(validTopic)
       {
          /* look for a hit and process if there was one */
          if (SearchForPattern(topicTitle,srchWord,srchWordLen) == True)
          {
             HitListAddFound(curFile, curVol->indexEntriesList[0], True, NULL);
          }
       }

       /* clean up and advance to next topic */
       XtFree(topicTitle);
       curVol->searchedCnt++;
    }  /* for all topics to search */
#endif

    if (    NULL == curVol->indexEntriesList      /* no topics queried */
         || NULL == curVol->indexEntriesList[0] ) /* no more topics available */
    {
       /* hand off to next stage */
       curVol->indexEntriesList = NULL;
       curVol->topicSearchInProgress = False;
       curVol->topicSearchDone = True;
       curVol->indexSearchInProgress = True;
    }
}




/*****************************************************************************
 * Function:	    void SearchIndex()
 *
 * Parameters:  
 *
 * Return Value:    void
 *
 * Purpose: 	    Get a single Index and search for the search word in it
 *
 * Commentary:      There are two strings for each index entry.
 *                  One string is the straight text in
 *                  the codeset of the volume.  The
 *                  other string is a XmString that
 *                  has a font tag that will allow
 *                  it to be displayed in the
 *                  correct codeset.
 *
 * Preconditions:   This code requires that the volume to be search
 *                  has already been opened [OpenVolForSearch()],
 *                  that the locale of the volume has been installed,
 *                  and that the search word is in the same code
 *                  set as the volume and has been converted to
 *                  upper case.
 *
 *****************************************************************************/
static void SearchIndex(
   DtHelpDialogWidget hw,
   _DtHelpFileEntry     curFile,
   _DtHelpGlobSrchVol * curVol,
   _DtHelpCeIconvContext iconvContext,
   char *            srchWord,
   int               srchWordLen,
   int               numSearches)
{
    int i;
    char * indexEntry = NULL;
    size_t entryLen = 100;      /* starting size */
    Arg    args[5];

    /* if the volume is not open; don't continue */
    if ( NULL == curVol->volHandle )
       return;                             /* RETURN */

    /* get the XmStrings of the entries */
    if ( NULL == curVol->indexXmStrsList )
    {
       XmFontList   fontList = NULL;
       Boolean      mod = False;
       int          numEntries = 0;

       /* get results font list */
       XtSetArg(args[0], XmNfontList, &fontList);
       XtGetValues(hw->help_dialog.srch.resultList,args,1);

#if 1
       /* Don't need to copy, _DtHelpFormatVolumeTitle copies 
        * before modifying.
        */

       /* work with copy, because FontListAppendEntry() destroys input FL */
       fontList = XmFontListCopy(fontList);
#endif

       /* get the entries */
       _DtHelpFormatIndexEntries(hw->help_dialog.help.pDisplayArea,
                  curVol->volHandle,&numEntries, &curVol->indexXmStrsList,
                  &fontList, &mod);

       /* install the changed list */
       if (mod)
       {
          XtSetArg(args[0], XmNfontList, fontList);
          XtSetValues(hw->help_dialog.srch.resultList,args,1);
          if (fontList) XmFontListFree(fontList);
       }
    }

    /* get the pure strings for searching */
    if ( NULL == curVol->indexEntriesList )
    {
       /* recall that the array and strings pointed to by
          the indexEntriesList is owned by the open volume */
       _DtHelpCeGetKeywordList(curVol->volHandle,&curVol->indexEntriesList);

       /* and re-init curIndexXmStr */
       curVol->curIndexXmStr = curVol->indexXmStrsList;
    }

    if (   NULL == curVol->volHandle 
        || NULL == curVol->indexEntriesList
        || NULL == curVol->indexXmStrsList )
    {
       curVol->indexSearchInProgress = False;
       curVol->indexSearchDone = True;
       curVol->searchCompleted = True;
       if ( NULL == curVol->volHandle ) curVol->searchCompleted = True;
       return;                                 /* RETURN */
    }

    curVol->indexSearchInProgress = True;

    /* alloc memory for index processing */
    indexEntry = XtMalloc(sizeof(char)*(entryLen+1));
    if (NULL == indexEntry) return;            /* RETURN */

    /* get a index entry */
    for (i=0; 
         i < numSearches && NULL != curVol->indexEntriesList[0]; 
         i++, curVol->indexEntriesList++, curVol->curIndexXmStr++ )
    {
       /* prevent overflow of buffer */
       if (strlen(curVol->indexEntriesList[0]) > entryLen)
       {
          entryLen = strlen(curVol->indexEntriesList[0]) + 50;
          indexEntry = XtRealloc(indexEntry,entryLen+1);
          if (NULL == indexEntry) break;          /* BREAK */
       }
       indexEntry[0] = EOS;  /* init to empty string */

       /* look for a hit and process if there was one */
       /* if srchWord is NULL, all entries are hits */
       /* copy, and possible iconv, the index string into the */
       /* the codeset of srchWord (the application) */
       if (    NULL == srchWord
            || (   _DtHelpCeIconvStr(iconvContext,curVol->indexEntriesList[0],
                        &indexEntry, &entryLen,indexEntry,entryLen) == 0
                && SearchForPattern(indexEntry, srchWord, srchWordLen) == True ) )
       {
          _DtHelpGlobSrchHit * hit;
          char * *          topicIdList;

          HitListAddFound ( curFile, curVol->curIndexXmStr[0],
                             curVol->indexEntriesList[0], True, &hit);

          /* topicIdList is set but not allocated & need not be freed */
          hit->topicCnt = _DtHelpCeFindKeyword(curVol->volHandle,
                                   hit->indexEntry,&topicIdList);
       }

       /* clean up and advance to next topic */
       curVol->searchedCnt++;
    }

    if (    NULL == curVol->indexEntriesList     /*no topics queried*/
         || NULL == curVol->indexEntriesList[0] )/*no more index entries avail*/
    {
       /* hand off to next stage */
       curVol->indexEntriesList = NULL;                 /* init */
       curVol->curIndexXmStr = curVol->indexXmStrsList; /* re-init */
       curVol->indexSearchInProgress = False;
       curVol->indexSearchDone = True;
       curVol->searchCompleted = True;
    }

    XtFree(indexEntry);
}


/*****************************************************************************
 * Function:	    void SearchVolume()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    do the search operation
 *                  This routine does incremental search--it searchs
 *                  a little, then returns to allow further processing.
 *                  It is called whenever Xt detects no other events.
 *
 *****************************************************************************/
static void SearchVolume(
         DtHelpDialogWidget hw,
        _DtHelpFileEntry        curFile,
        _DtHelpGlobSrchVol *    curVol,
        int                  numSearches)
{
  XmString labelString;
  Arg args[10];
  char buf[50];

  _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));
  if (hw->help_dialog.srch.selectionDlg)
     _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

  /* Process according to state */
  if ( curVol->nothingDone )
  {
     curVol->nothingDone = False;
     curVol->topicSearchInProgress = True;

     /* Update the search status string */
     sprintf(buf,(char *)_DTGETMESSAGE(GSSET,18,
                                "Searching... Volumes remaining: %d"),
                 hw->help_dialog.srch.volLeftCnt);
     labelString = XmStringCreateLocalized(buf);
     XtSetArg(args[0],XmNlabelString,labelString);
     XtSetValues (hw->help_dialog.srch.statusLabel, args, 1);
     XmStringFree(labelString);

     XmUpdateDisplay((Widget) hw->help_dialog.srch.statusLabel);
  }

  OpenVolForSearch(hw,curFile->fullFilePath,curVol);

  if ( curVol->topicSearchInProgress )
  {
     SearchTopic(hw, curFile,curVol,hw->help_dialog.srch.localeWordStr,
              hw->help_dialog.srch.wordFieldLen, numSearches);
  }

  /* NOTE: because this isn't an "else if", we could potentially
     search for 2*numSearches; but I know that I've disabled the
     topic search.  Furthermore, my code depends upon the possibility
     of searching an entire volume in one call to the routine, if
     desired, and having an else if would break that.
  */
  if ( curVol->indexSearchInProgress )
  {
      char * srchWord;
      srchWord = hw->help_dialog.srch.localeWordStr;
      /* FullIndex search indicated by a null search word string */
      if (hw->help_dialog.srch.fullIndex) srchWord = NULL;
      SearchIndex(hw, curFile, curVol, hw->help_dialog.srch.iconv3Context, 
              srchWord, hw->help_dialog.srch.wordFieldLen, numSearches);
  }

  /* If search was completed, update dependent stuff */
  if (curVol->searchCompleted)
  {
     /* handle completion */
     CloseVolAfterSearch(hw,curVol);
     curVol->indexEntriesList = NULL;       /* mem not owned by GlobSrch */
     if (hw->help_dialog.srch.fullIndex)
        curVol->gatheredFullIndex = True;
     hw->help_dialog.srch.volLeftCnt--;
     if (hw->help_dialog.srch.volLeftCnt < 0)
        hw->help_dialog.srch.volLeftCnt = 0;
  }

  _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));
  if (hw->help_dialog.srch.selectionDlg)
     _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

}


/*****************************************************************************
 * Function:	    void SearchFile()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    do the search operation on an entire file
 *                  It searches the entire file at once
 *                  and displays the results in the list.
 *
 *****************************************************************************/
static void SearchFile(
   DtHelpDialogWidget      hw,
   _DtHelpFileEntry        file)
{
   register _DtHelpGlobSrchVol * vol;

   /* get volume info, adding if necessary */
   vol = (_DtHelpGlobSrchVol *) file->clientData;
   /* (T,T,F): enable search, enable display, any volume */
   if (NULL == vol) AddVolInfoToFile(file, True, True, False);
   vol = (_DtHelpGlobSrchVol *) file->clientData;
   if (NULL == vol) return;

   /* conduct search of entire volume in one call */
   hw->help_dialog.srch.volLeftCnt = 1;
   vol->searchThisVolume = True;
   vol->showVolInList = True;
   vol->zeroHitsOk = True;
   SearchVolume(hw,file,vol,INT_MAX);

   /* update status */
   StatusLabelUpdate(hw,SEARCH_RESULTS_STATUS,True,0);/*FIX: False*/

   /* update the list to show the volume */
   AdjustPositionValues(file,0,1,True);
   /* True: do insert new item */
   VolNameDisplay(hw,file,True);

   if (vol->showHitsWithVol)
   {
      VolHitsDisplay(hw,file);
      /* False: don't insert new item */
      VolNameDisplay(hw,file,False);
   } 
}




/*****************************************************************************
 * Function:	    void DoBackgroundSearchWP(
 *                                  XtPointer clientData)
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    do the search operation in the background
 *                  This routine does incremental search--it searchs
 *                  a little, then returns to allow further processing.
 *                  It is called whenever Xt detects no other events.
 *
 *****************************************************************************/
static Boolean DoBackgroundSearchWP(
    XtPointer clientData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
  _DtHelpFileEntry curFile;
  _DtHelpGlobSrchVol * curVol = NULL;
  
  if ( 0 == hw->help_dialog.srch.volLeftCnt ) 
     goto searchComplete;                                  /* GOTO: done! */

  /* get file to work on */
  curFile = hw->help_dialog.srch.curSrchVol;
  if ( NULL == curFile )
  {
    curFile = GetNextSearchFileAndDisplayCompleted(
                             hw, hw->help_dialog.srch.volListHead, NULL);
    if ( NULL == curFile ) goto searchComplete;            /* GOTO: done */
    hw->help_dialog.srch.curSrchVol = curFile;
  }  /* if no current file */

  /* get volume info */
  curVol = (_DtHelpGlobSrchVol *) curFile->clientData;

#define NUM_SEARCHES_PER_CALL  30
  SearchVolume(hw,curFile,curVol,NUM_SEARCHES_PER_CALL);

  /* If search was completed, update dependent stuff */
  if (curVol->searchCompleted)
  {
    /* update the search results list */
    /* True: adjust count beginning with this file */
    if (curVol->hitCnt > 0 || curVol->zeroHitsOk) 
    {
       /* record that a hit found */
       if (   curVol->hitCnt > 0
           || (    curVol->zeroHitsOk 
               && _DtHelpGlobSrchSelectedVolumes == hw->help_dialog.srch.srchSources))
          hw->help_dialog.srch.hitsFound = True;

       /* update the list to show the volume */
       AdjustPositionValues(curFile,0,1,True);
       ResultsListUpdate(hw,curFile);

       /* update the list to show the hits? */
       if (hw->help_dialog.srch.srchSources == _DtHelpGlobSrchCurVolume)
       {
          VolHitsDisplay(hw,curFile);
          /* False: don't insert new item */
          VolNameDisplay(hw,curFile,False);
       }
       XmUpdateDisplay((Widget) hw->help_dialog.srch.resultList);
    }

    /* prepare for next search */
    hw->help_dialog.srch.curSrchVol = 
                     GetNextSearchFileAndDisplayCompleted(hw,NULL,curFile);

  }  /* if search completed */

  /* return */
  if (   hw->help_dialog.srch.volLeftCnt > 0
      || False == curVol->searchCompleted ) 
     return False;              /* RETURN: False ==> continue processing */

  /* Do this when no more files are left to search */
searchComplete:
  {
  XmPushButtonCallbackStruct status;

  /* zero search data before stopping */
  hw->help_dialog.srch.volLeftCnt = 0;                    /* just in case */
  hw->help_dialog.srch.curSrchVol = NULL;                 /* just in case */

  /* generate a fake event to reset the dialog */
  status.reason = XmCR_ACTIVATE;
  status.event = NULL;
  status.click_count = 1;
  StopSearchCB(hw->help_dialog.srch.actionBtn,
	       (XtPointer)hw,(XtPointer)&status);

  return True;              /* RETURN: True ==> stop processing */
  }
}



/*****************************************************************************
 * Function:	    void StartSearchCB(Widget w,
 *                                  XtPointer clientData,
 *                                  XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Create a new user bookmark
 *
 * Warning:  This CB is also used by the volume selection buttons
 *           to activate a search when they are selected.  The routine
 *           cannot rely upon the w or callData params.  The clientData
 *           will always be the help widget.
 *****************************************************************************/
static void StartSearchCB(
    Widget w,                     /* dont use */
    XtPointer clientData,
    XtPointer callData)           /* dont use */
{
   char       *srchWord = NULL;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   XtAppContext appCntx;
   int        n;
   Arg        args[5];

   /* set state of start button correctly */
   UpdateSearchStartStatusCB(NULL, (XtPointer) hw, NULL);

   /* if help content isn't a volume, it has no index */
   if (    _DtHelpGlobSrchCurVolume == hw->help_dialog.srch.srchSources
        && hw->help_dialog.display.helpType != DtHELP_TYPE_TOPIC )
      return;                         /* RETURN */

   /* retrieve the name */
   srchWord = XmTextFieldGetString(hw->help_dialog.srch.wordField);

   /* test for empty (also emtpy spaces) and only process if not emtpy */
   _DtHelpCeCompressSpace(srchWord);
   if (    (strlen(srchWord) > (size_t) 0 || hw->help_dialog.srch.fullIndex)
        && hw->help_dialog.srch.workProcId == 0) 
   /* test on workProcId prevents multiple activations due to 
      race condition on desensitizing the start button */
   {
     _DtHelpTurnOnHourGlass(XtParent(hw));
     _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

     /*XtSetSensitive (hw->help_dialog.srch.actionBtn,False);*/
     XtSetSensitive (hw->help_dialog.srch.curVolRadBtn,False);
     XtSetSensitive (hw->help_dialog.srch.allVolRadBtn,False);
     XtSetSensitive (hw->help_dialog.srch.selVolRadBtn,False);
     XtSetSensitive (hw->help_dialog.srch.selectBtn,False);

/* 9/26/94: these two XtSetArgs (and following SetValues) cause a core dump
   for the SunOS 5.3 optimized executable.  It isn't a problem for the
   non-optimized or debuggable versions or any versions on AIX 3.2 or 
   HP-UX 9.05. */
#if !defined(sun) && !defined(__osf__)
     n = 0;
     XtSetArg(args[n],XmNeditable,False);			n++;
     XtSetArg(args[n],XmNcursorPositionVisible,False);		n++;
     XtSetValues (hw->help_dialog.srch.wordField, args, n);
#endif
     XmUpdateDisplay(hw->help_dialog.srch.srchForm);

     /* process the source selection and retain in widget srchWord memory */
     PrepSearchSourceData(hw, srchWord);

     /* activate search thread; do it even with a 0 count, because the
        process reactivates the dialog after search is completed */
     appCntx = XtWidgetToApplicationContext((Widget) hw);
     hw->help_dialog.srch.workProcId = 
             XtAppAddWorkProc(appCntx,DoBackgroundSearchWP, (XtPointer) hw);

     hw->help_dialog.srch.searchInProgress = True;

     /* dont free srchWord */

     /* turn on "Stop" label */
     UpdateActionButtonLabel(hw,NULL,False);

     _DtHelpTurnOffHourGlass(XtParent(hw));
     _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));
   }
   else 
   {
      if ( strlen(srchWord) == 0 )
         StatusLabelUpdate(hw,FIRST_PROMPT_STATUS,False,0);
      XtFree(srchWord);
   }
}




/*****************************************************************************
 * Function:	    void StartSelectedVolumeSearchCB(Widget w,
 *                                  XtPointer clientData,
 *                                  XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Begin a search on the selected volume
 *
 *****************************************************************************/
static void StartSelectedVolumeSearchCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   _DtHelpFileEntry file;
   String        srchWord;
   Boolean       noSelectedFiles = True;

    /* if not full index search */
    if (hw->help_dialog.srch.fullIndex == False)
    {
       /* get the current contents of the search word field */
       srchWord = XmTextFieldGetString(hw->help_dialog.srch.wordField);
   
       /* test the word ; False=dontUpdate--do this in PrepSearchSourceData() */
       if ( CheckSearchWord(hw,srchWord,False) == True )
       {   /* word has changed since last search; do a whole new search */
           StartSearchCB(NULL,(XtPointer) hw, NULL);
           return;                                   /* RETURN */
       }
   
       /* if the text field is empty, do nothing */
       if ( strlen(hw->help_dialog.srch.normWordStr) == 0 )
           return;                                   /* RETURN */
    }

    /* set this to true to counteract False if no hits */
    hw->help_dialog.srch.hitsFound = True;

    /* Word is the same, just the selection has changed. */
    /* Determine which one was selected or deselected */
    for ( file = hw->help_dialog.srch.volListHead;
          NULL != file;
          file = _DtHelpFileListGetNext(NULL,file) )
    {
       register _DtHelpGlobSrchVol * vol;
       vol = (_DtHelpGlobSrchVol *) file->clientData;
       if (NULL == vol) continue;

       /* update flag */
       if (file->fileSelected) noSelectedFiles = False;

       /* test for contradictory flags; fileSelected reflects dialog list */
       if (file->fileSelected != vol->showVolInList)
       {
          /* file has been deselected */
          if (False == file->fileSelected)
          {
              if (vol->showVolInList)
              {
                 /* update list: delete hits, topics, and volume title */
                 if (vol->hitsDisplayed) VolHitsUndisplay(hw,file);

                 /* delete hits first, then volume */
                 XmListDeletePos(
                          hw->help_dialog.srch.resultList,vol->startPosition);
                 AdjustPositionValues(file, 0, -1, True);
                 vol->showVolInList = False;
                 vol->searchThisVolume = False;
              }
          }
          else /* file has been selected */
          {
             SearchFile(hw,file);
          }  /* if file de/selected */
       }  /* if contradictory flags */
    }  /* for all volumes */

    /* update state */
    if (noSelectedFiles) StatusLabelUpdate(hw,NO_VOL_STATUS,True,0);
}



/*****************************************************************************
 * Function:	    void UpdateSearchStartStatusCB(Widget w,
 *                                  XtPointer clientData,
 *                                  XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    update the status of the search status button
 *        Routine is called whenever text is changed in the search
 *        word field.
 *
 *****************************************************************************/
static void UpdateSearchStartStatusCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   char       *srchWord = NULL;
   /*XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *) callData;*/
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   Boolean    newState;

   /* retrieve the name */
   srchWord = XmTextFieldGetString(hw->help_dialog.srch.wordField);

   /* test for empty (also emtpy spaces) */
   _DtHelpCeCompressSpace(srchWord);

   /* FIX: change the locale from the volume locale back to the system 
      locale temporarily (if not already in that locale) */
   _DtHelpCeUpperCase(srchWord);

   /* only sensitize if working on a help volume and word is not emtpy 
      or not needed */
   newState = True;
   if (    (   _DtHelpGlobSrchCurVolume == hw->help_dialog.srch.srchSources
            && hw->help_dialog.display.helpType != DtHELP_TYPE_TOPIC)
        || (   strlen(srchWord) == (size_t) 0 
            && hw->help_dialog.srch.fullIndex == False))
       newState = False;
   XtSetSensitive (hw->help_dialog.srch.actionBtn,newState);

   /* if word has changed from word of previous search, change to Start */
   UpdateActionButtonLabel(hw,srchWord,True);

   XtFree(srchWord);
}


/*****************************************************************************
 * Function:	    void StopSearchCB(Widget w,
 *                                    XtPointer clientData,
 *                                    XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Stop the search
 *
 *****************************************************************************/
static void StopSearchCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
  int     n;
  Arg     args[10];
  Boolean cur;

  /* stop the search thread */
  if(hw->help_dialog.srch.workProcId != 0)
     XtRemoveWorkProc(hw->help_dialog.srch.workProcId);
  hw->help_dialog.srch.workProcId = 0;

  hw->help_dialog.srch.searchInProgress = False;

  /* update statuses */
#ifdef OLD_WAY
   XtSetSensitive (hw->help_dialog.srch.curVolRadBtn,
                              hw->help_dialog.srch.curVolRadBtnSens);
#else
   XtSetSensitive (hw->help_dialog.srch.curVolRadBtn, TRUE);
#endif
  
  XtSetSensitive (hw->help_dialog.srch.allVolRadBtn,True);
  XtSetSensitive (hw->help_dialog.srch.selVolRadBtn,True);
  n = 0;
  XtSetArg(args[n],XmNeditable,True);				n++;
  XtSetArg(args[n],XmNcursorPositionVisible,True);		n++;
  XtSetValues (hw->help_dialog.srch.wordField, args, n);

  /* update select btn status; only when selected and dialog closed */
  XtSetArg(args[0],XmNset,&cur);
  XtGetValues(hw->help_dialog.srch.selVolRadBtn,args,1);
  XtSetSensitive(hw->help_dialog.srch.selectBtn,
           cur && (   NULL == hw->help_dialog.srch.selectionDlg
                   || !XtIsManaged(hw->help_dialog.srch.selectionDlg) ) );

  /* set the action button label appropriately */
  UpdateActionButtonLabel(hw,hw->help_dialog.srch.normWordStr,True);

  /* post message of results */
  StatusLabelUpdate(hw,SEARCH_RESULTS_STATUS,True,0);
}




/*****************************************************************************
 * Function:	    void ActionButtonCB(Widget w,
 *                                    XtPointer clientData,
 *                                    XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Act upon the action
 *
 *****************************************************************************/
static void ActionButtonCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
  DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;

  if ( hw->help_dialog.srch.searchInProgress == False )
     StartSearchCB(w,clientData,callData);
  else
     StopSearchCB(w,clientData,callData);
}



#ifdef	not_used
/*****************************************************************************
 * Function:	    void GotoResultCB(Widget w,
 *                                              XtPointer clientData,
 *                                              XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Goto a result topic
 *
 *****************************************************************************/
static void GotoResultCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   char *          helpFile = NULL;
   int *           topicPosList=NULL;  
   int             topicPosition;  
   Boolean         status;
   int             i;

   /* Determine the item selected and find that item in our list */
   status = XmListGetSelectedPos(hw->help_dialog.srch.resultList, &topicPosList,&i);

   /* if true, an item selected */
   if (status && topicPosList)
   {
     _DtHelpGlobSrchHit * entry = NULL;
     _DtHelpFileEntry  file = NULL;
     char * locationId = NULL;
     topicPosition = topicPosList[0] - 1;	/* convert to 0 based */
     XtFree((String)topicPosList);
  
     /* Get the entry */
     HitListGetNth(hw->help_dialog.srch.volListHead,
                         topicPosition,&file,&entry,&locationId,&helpFile);

     /* jump to selected location */
     XtFree(hw->help_dialog.display.locationId);
     hw->help_dialog.display.locationId = XtNewString(locationId);
     XtFree(hw->help_dialog.display.helpVolume);
     hw->help_dialog.display.helpVolume = XtNewString(helpFile);
     hw->help_dialog.ghelp.volumeFlag = False;  /* force search for volume */
     hw->help_dialog.display.helpType = DtHELP_TYPE_TOPIC;

     _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_AND_JUMP);
  }
  else
  {
     if (topicPosList) XtFree((String)topicPosList);

     /* FIX: Put up a message to suggest selecting an item first */
     /* Don't close the dialog */
  }
}
#endif /* not_used */

#if 0  /* save--used in code commented out elsewhere in this file */

/*****************************************************************************
 * Function:	    void CloseHelpCB()
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Close and destroy the help widget
 *
 *****************************************************************************/
static void CloseHelpCB(
     Widget          w,
     XtPointer       clientData,
     XtPointer       callData)
{
    XtUnmanageChild(w);
    XtDestroyWidget(w);
}
#endif  /* save--used in code commented out elsewhere in this file */


/*****************************************************************************
 * Function:	    void GotoLocation(hw,helpVolume,location)
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Goto a topic by opening a new help dialog window
 *
 *****************************************************************************/
static void  GotoLocation(
    DtHelpDialogWidget hw,
    char * helpVolume,
    char * locationId)
{
#if 0   /* don't delete...useful as reference */
  {
   Widget helpDlg;
   Arg    args[15];
   int    n;

   /* This opens a new help window on the topic */
   n = 0;
   XtSetArg( args[n], XmNtitle, "Help" );                        n++;
   XtSetArg( args[n], DtNhelpType, DtHELP_TYPE_TOPIC );         n++;
   XtSetArg( args[n], DtNlocationId, locationId );               n++;
   XtSetArg( args[n], DtNhelpVolume, helpVolume );               n++;
   helpDlg = DtCreateHelpDialog(XtParent(hw),"mainHelpDialog",args,n);
   XtAddCallback(helpDlg,DtNcloseCallback,CloseHelpCB,helpDlg);
   XtManageChild(helpDlg);
  }
#else
   {
     _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));

     /* jump to selected location in same window */
     XtFree(hw->help_dialog.display.locationId);
     hw->help_dialog.display.locationId = XtNewString(locationId);
     XtFree(hw->help_dialog.display.helpVolume);
     hw->help_dialog.display.helpVolume = XtNewString(helpVolume);
     hw->help_dialog.ghelp.volumeFlag = False;  /* force search for volume */
     hw->help_dialog.display.helpType = DtHELP_TYPE_TOPIC;

     _DtHelpSetupDisplayType(hw, TRUE, DtHISTORY_AND_JUMP);

     _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));
     if (hw->help_dialog.srch.selectionDlg)
        _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.selectionDlg));
   }
#endif
}



/*****************************************************************************
 * Function:	    void ProcessResultSelectionCB(Widget w,
 *                                              XtPointer clientData,
 *                                              XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process user selection of an item in the result List.
 *
 *****************************************************************************/
static void ProcessResultSelectionCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   int *     topicPosList = NULL;  
   int       topicPosition;  
   Boolean   status;
   int       n;

   /* Determine the item selected and find that item in our list */
   status = XmListGetSelectedPos(
              hw->help_dialog.srch.resultList, &topicPosList,&n);

   /* if true, an item selected */
   if (status && topicPosList)
   {
     _DtHelpFileEntry     file = NULL;
     _DtHelpGlobSrchHit * hit = NULL;
     char *            locationId = NULL;
     char *            helpFile = NULL;

     topicPosition = topicPosList[0];	/* 1 based */
     XtFree((String)topicPosList);
  
     /* Get the entry */
     if ( HitListGetNth(hw->help_dialog.srch.volListHead,
                       topicPosition,&file,&hit,&locationId,&helpFile) == 0 )
     {
        _DtHelpTurnOnHourGlass(XtParent(hw->help_dialog.srch.srchForm));

        /* deactivate it */
        XmListDeselectPos(hw->help_dialog.srch.resultList,topicPosition);

        /* was a volume item selected? */
        if ( NULL == hit )
        {
           _DtHelpGlobSrchVol * vol = (_DtHelpGlobSrchVol *) file->clientData;
           if (vol->hitCnt > 0) 
           {
              if (False == vol->hitsDisplayed) 
              {
                 VolHitsDisplay(hw,file);     /* toggle state */
              }
              else 
              {
                 VolHitsUndisplay(hw,file);
                 vol->showHitsWithVol = False;
              }
              /* update the volume label to show the state */
              /* False: dont insert hw item */
              VolNameDisplay(hw,file,False);
           }
           /* else don't otherwise handle a 0-hits volume */
        }
        /* was an expandable index selected */
        else if (NULL == locationId && hit->topicCnt > 1)
        {
           int topicCnt = 0;
       
           /* if we're turning them on, build a list */
           if (False == hit->topicsDisplayed)
           {
              topicCnt = HitTopicsDisplay(hw,file,hit,topicPosition+1);
           }
           else
           {
              topicCnt = - HitTopicsUndisplay(hw,file,hit,topicPosition+1);
              hit->showTopicsWithHit = False;
           }
           /* update the hit label to show the state; False--dont insert */
           HitNameDisplay(hw,file,hit,topicPosition,False);
       
           /* add to all the following */
           if (topicCnt != 0)
           {
              /* adjust count beginning with next file with hits */
              AdjustPositionValues(file, 0, topicCnt, True);
           }
        }
        /* a location or index with one topic was selected */
        else 
        {
           /* if a single click, goto the location */
           if (hit->topicCnt == 1)
           {
              if ( HitLoadTopics(hw,file,hit) == 0 )
              {
                 GotoLocation(hw, file->fullFilePath, hit->topicIdList[0]);
                 HitFree(hit,False);     /*release what was loaded 3 lines up*/
              }
              else
              {  /* volume couldn't be opened or some other error */
                /* FIX: error message */
              }
           } /* if an index with a single topic */
           else
           { /* a location from an open multi-topic index entry */
              GotoLocation(hw, file->fullFilePath, locationId);
           }
        } /* else index with one topic or topic was selected */

        /* finish up processing a selection */
        _DtHelpTurnOffHourGlass(XtParent(hw->help_dialog.srch.srchForm));

     } /* got the nth item */
  }
  else  /* could not find the nth hit */
  {
     if (topicPosList) XtFree((String)topicPosList);
  }
}




/*****************************************************************************
 * Function:	    void CloseVolSelDialogCB(Widget w,
 *                                          XtPointer clientData,
 *                                          XtPointer callData);
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Resets the selection dialog variable to NULL
 *
 *****************************************************************************/
static void CloseVolSelDialogCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;

   /* unmanage & destroy the dialog zero the variable */
   if (    hw->help_dialog.srch.selectionDlg
        && XtIsManaged(hw->help_dialog.srch.selectionDlg) )
      XtUnmanageChild(hw->help_dialog.srch.selectionDlg);

   /* start a search to be sure we got all changes; FIX: need this??? */
   /*StartSearchCB(NULL,hw,NULL);*/
   XtSetSensitive(hw->help_dialog.srch.selectBtn,True);

   /* change focus */
   if (hw->help_dialog.srch.hitsFound == True)
     XmProcessTraversal(hw->help_dialog.srch.resultList,XmTRAVERSE_CURRENT);
   else
     XmProcessTraversal(hw->help_dialog.srch.allVolRadBtn,XmTRAVERSE_CURRENT);
}



/*****************************************************************************
 * Function:	    void CreateVolSelDialog(
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Creates selection dialog to chose volumes
 *
 *****************************************************************************/
static void CreateVolSelDialog(
            DtHelpDialogWidget hw)
{
    Widget    selDlg;
    _DtHelpFileDlgChildren dlgChld;
    Atom      wm_delete_window;
    Arg       args[5];

    /* desensitize the button */
    XtSetSensitive(hw->help_dialog.srch.selectBtn,False);
    
    /* only create one */
    if ( NULL != hw->help_dialog.srch.selectionDlg ) 
    {
       XtManageChild(hw->help_dialog.srch.selectionDlg);
       return;
    }

    /* open the modal dialog and let user select the volumes */
    selDlg = _DtHelpFileListCreateSelectionDialog(
                   hw, hw->help_dialog.srch.srchForm, False,
                   (char *)_DTGETMESSAGE (GSSET, 40,
                             "Help - Search Volume Selection"),
                   &hw->help_dialog.srch.volTitlesFontList,
                   hw->help_dialog.srch.volListHead, &dlgChld);
    hw->help_dialog.srch.selectionDlg = selDlg;

    XtAddCallback(dlgChld.closeBtn, XmNactivateCallback, 
                             CloseVolSelDialogCB,(XtPointer)hw);
    XtAddCallback(dlgChld.form, XmNunmapCallback, 
                             CloseVolSelDialogCB,(XtPointer)hw);

    /* set the default response to a WM_DELETE_WINDOW message
      to no response instead of UNMAP. */
    XtSetArg(args[0], XmNdeleteResponse, XmDO_NOTHING);
    XtSetValues(dlgChld.shell, args, 1);
    wm_delete_window = XmInternAtom(XtDisplay(dlgChld.shell),
                                            "WM_DELETE_WINDOW", FALSE);
    XmAddWMProtocolCallback(dlgChld.shell,wm_delete_window,
             (XtCallbackProc)CloseVolSelDialogCB, (XtPointer)hw);

    XtAddCallback(dlgChld.list, XmNsingleSelectionCallback, 
                           StartSelectedVolumeSearchCB, (XtPointer) hw);
    XtAddCallback(dlgChld.list, XmNmultipleSelectionCallback, 
                           StartSelectedVolumeSearchCB, (XtPointer) hw);
    XtAddCallback(dlgChld.list, XmNextendedSelectionCallback, 
                           StartSelectedVolumeSearchCB, (XtPointer) hw);
    XtAddCallback(dlgChld.list, XmNdefaultActionCallback, 
                           StartSelectedVolumeSearchCB, (XtPointer) hw);
}



/*****************************************************************************
 * Function:	    void OpenSelectDialogCB(Widget w,
 *                                          XtPointer clientData,
 *                                          XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process user selection of the select button
 *
 *****************************************************************************/
static void OpenSelectDialogCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;

   /* only build the list of files if it hasn't been done */
   if (   False == hw->help_dialog.srch.volScanDone
       || NULL == hw->help_dialog.srch.volListHead)
   {
      /* Add on the Vol info; (T,T,T) search, display, selected only */
      VolListBuild(hw,True,True,True);
   }

   /* FIX: the modal behaviour doesn't seem to be set; is it desired/needed? */
   /* don't need to worry about having open twice, since
      the dialog is always opened in modal mode from the search dialog */
   /* open the dialog and let user select the volumes */
   CreateVolSelDialog(hw);
}


/*****************************************************************************
 * Function:	    void UpdateSearchVolumesCB(Widget w,
 *                                             XtPointer clientData,
 *                                             XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process user selection of a button in the Volumes Radio box
 *
 *****************************************************************************/
static void UpdateSearchVolumesCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
   XmToggleButtonCallbackStruct *status = (XmToggleButtonCallbackStruct *) callData;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   Arg args[5];


   /* change focus */
   
   if (hw->help_dialog.srch.hitsFound == True) 
     XmProcessTraversal(hw->help_dialog.srch.resultList,XmTRAVERSE_CURRENT);
   else
     XmProcessTraversal(hw->help_dialog.srch.allVolRadBtn,XmTRAVERSE_CURRENT);



   /* if turning off the button, just reinforce the current settings.
      This message occurs when an on button is hit again. */
   if ( False == status->set )
   {
     XtSetArg(args[0], XmNset, hw->help_dialog.srch.allVolRadBtn == widget);
     XtSetValues(hw->help_dialog.srch.allVolRadBtn,args,1);
     XtSetArg(args[0], XmNset, hw->help_dialog.srch.selVolRadBtn == widget);
     XtSetValues(hw->help_dialog.srch.selVolRadBtn,args,1);
     XtSetArg(args[0], XmNset, hw->help_dialog.srch.curVolRadBtn == widget);
     XtSetValues(hw->help_dialog.srch.curVolRadBtn,args,1);
     return;
   }

   /*** if work when something is turned on; ignore the off messages 
        except when the button is the selected volumes button; we
        want to catch every click on that button */
   if (   XmCR_VALUE_CHANGED == status->reason 
       && True == status->set 
       && NULL != status->event ) /* event is NULL for 1st of the two calls 
                                     to this routine that are made when a 
                                      button is pressed. */
   {
     /*** now update the sources value according to button selected ***/
     hw->help_dialog.srch.volLeftCnt = 0;  /* change invalidates prior search */
     UpdateActionButtonLabel(hw,NULL,True);

     XtSetArg(args[0], XmNset, hw->help_dialog.srch.allVolRadBtn == widget);
     XtSetValues(hw->help_dialog.srch.allVolRadBtn,args,1);
     XtSetArg(args[0], XmNset, hw->help_dialog.srch.curVolRadBtn == widget);
     XtSetValues(hw->help_dialog.srch.curVolRadBtn,args,1);
     XtSetArg(args[0], XmNset, hw->help_dialog.srch.selVolRadBtn == widget);
     XtSetValues(hw->help_dialog.srch.selVolRadBtn,args,1);

     /*** selected volumes button activated ***/
     if (hw->help_dialog.srch.selVolRadBtn == widget) 
     {
        /* set state */
        hw->help_dialog.srch.srchSources = _DtHelpGlobSrchSelectedVolumes;

        /* only build the list of files if it hasn't been done */
        if (    False == hw->help_dialog.srch.volScanDone
             || NULL == hw->help_dialog.srch.volListHead)
        {
           /* Add on the Vol info; (T,T,T) search, display, selected only */
           VolListBuild(hw,True,True,True);
        }

        /* (F,F,F,F): disable search,disable display,no 0 hits,for all vols */
        SetVolStatus(hw->help_dialog.srch.volListHead,False,False,False,False);
        /* (T,T,T,T): enable search, enable display,
                    zero hits ok, only for selected volumes */
        /* Set these here so that the CountSelectedVolumes() works right */
        SetVolStatus(hw->help_dialog.srch.volListHead,True,True,True,True);

        /* count all volumes selected, including those already searched */
        if (CountSelectedVolumes(hw->help_dialog.srch.volListHead,True)==0)
        {
           StatusLabelUpdate(hw,NO_VOL_STATUS,False,0);
           DeleteListContents(&hw->help_dialog.srch);
           CreateVolSelDialog(hw);
           /* NOTE: don't start search here, start it from the callback */
           /* set state of start button correctly */
           UpdateSearchStartStatusCB(NULL, (XtPointer) hw, NULL);
        }
        else  /* volumes already selected; just display */
        {
           hw->help_dialog.srch.hitsFound = False; /*DBG*/
           StartSearchCB(NULL,(XtPointer) hw, NULL);
        }
     }   /* if selected volumes button */

     /*** all volumes button ***/
     else if (hw->help_dialog.srch.allVolRadBtn == widget)
     {
        /* close the selection dialog, if open */
        if (hw->help_dialog.srch.selectionDlg)
           XtUnmanageChild(hw->help_dialog.srch.selectionDlg);

        /* set sources state */
        hw->help_dialog.srch.srchSources = _DtHelpGlobSrchAllVolumes;
        hw->help_dialog.srch.hitsFound = False; /*DBG*/
        StartSearchCB(NULL,(XtPointer) hw, NULL);
     }
     /*** current volume button ***/
     else
     {  
        /* close the selection dialog, if open */
        if (hw->help_dialog.srch.selectionDlg)
           XtUnmanageChild(hw->help_dialog.srch.selectionDlg);

        /* set sources state */
        hw->help_dialog.srch.srchSources = _DtHelpGlobSrchCurVolume;
        hw->help_dialog.srch.hitsFound = False; /*DBG*/
        StartSearchCB(NULL,(XtPointer) hw, NULL);
     }

     /* update the Select... button status */
     /* do this last, so that while searching for volumes (VolListBuild),
        the button is not active. */
     XtSetSensitive(hw->help_dialog.srch.selectBtn,
                     (hw->help_dialog.srch.selVolRadBtn == widget) 
                  && (   NULL == hw->help_dialog.srch.selectionDlg
                      || !XtIsManaged(hw->help_dialog.srch.selectionDlg) ) );

   }  /* if value changed */
}



/*****************************************************************************
 * Function:	    void UpdateIndexSelectionCB(Widget w,
 *                                             XtPointer clientData,
 *                                             XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:  Void.
 *
 * Purpose: 	  Process user selection of a button in the Entries Radio box
 *
 *****************************************************************************/
static void UpdateIndexSelectionCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
   XmToggleButtonCallbackStruct *status = (XmToggleButtonCallbackStruct *) callData;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;
   Boolean containsState;
   int n;
   Arg args[5];

   /* change focus */
   if (hw->help_dialog.srch.hitsFound == True) 
     XmProcessTraversal(hw->help_dialog.srch.resultList,XmTRAVERSE_CURRENT);
   else
     XmProcessTraversal(hw->help_dialog.srch.allVolRadBtn,XmTRAVERSE_CURRENT);


   /* if turning off the button, just reinforce the current settings.
      This message occurs when an on button is hit again. */
   if ( False == status->set )
   {
     containsState = (hw->help_dialog.srch.containsRadBtn == widget);
     XtSetArg(args[0], XmNset, !containsState);
     XtSetValues(hw->help_dialog.srch.fullIndexRadBtn,args,1);
     XtSetArg(args[0], XmNset, containsState);
     XtSetValues(hw->help_dialog.srch.containsRadBtn,args,1);
     return;
   }

   /*** if work when something is turned on; ignore the off messages 
        except when the button is the selected volumes button; we
        want to catch every click on that button */
   if (   XmCR_VALUE_CHANGED == status->reason 
       && True == status->set 
       && NULL != status->event ) /* event is NULL for 1st of the two calls 
                                     to this routine that are made when a 
                                      button is pressed. */
   {
     /* if search in progress, stop it */
     if(hw->help_dialog.srch.searchInProgress||hw->help_dialog.srch.workProcId)
     {
         XmPushButtonCallbackStruct status;
   
         /* generate a fake event to reset the dialog */
         status.reason = XmCR_ACTIVATE;
         status.event = NULL;
         status.click_count = 1;
         StopSearchCB(hw->help_dialog.srch.actionBtn,
		      (XtPointer)hw,(XtPointer)&status);
     }

     containsState = (hw->help_dialog.srch.containsRadBtn == widget);
     XtSetArg(args[0], XmNset, !containsState);
     XtSetValues(hw->help_dialog.srch.fullIndexRadBtn,args,1);
     XtSetArg(args[0], XmNset, containsState);
     XtSetValues(hw->help_dialog.srch.containsRadBtn,args,1);

     /* sensitize/desensitize the Contains text field */
     XtSetSensitive(hw->help_dialog.srch.wordField,containsState);
     n=0;
     XtSetArg(args[n],XmNeditable,containsState);			n++;
     XtSetArg(args[n],XmNcursorPositionVisible,containsState);		n++;
     XtSetValues(hw->help_dialog.srch.wordField,args,n);

     /* set widget status here; do this way bec. srchFullIndex is a bit flag */
     hw->help_dialog.srch.fullIndex = (containsState ? False : True); /*flag*/
 
     /* set state of start button correctly */
     UpdateSearchStartStatusCB(NULL, (XtPointer) hw, NULL);

     if (containsState)
     {  /* if changing to "Containing" */
        /* change focus to the search word */ 
        XmProcessTraversal(hw->help_dialog.srch.wordField,XmTRAVERSE_CURRENT);
        /* user will need to hit CR to initiate search */
     }
     else
     {  /* if changing to full index */
        /* indicate that no contains word is valid by deleting it */
        /* FIX: I'm concerned that normWordStr & localeWordStr aren't in synch */
        XtFree(hw->help_dialog.srch.normWordStr);
        hw->help_dialog.srch.normWordStr = NULL;
        /* search full index */
        StartSearchCB(NULL,(XtPointer) hw, NULL);
     }

     /* and search to update results to the new state */
   } /* if value changed */
}



/*****************************************************************************
 * Function:	    void ContainsDisarmCB(Widget w,
 *                                        XtPointer clientData,
 *                                        XtPointer callData);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:  Void.
 *
 * Purpose: 	  Process disarm of the contains radio button
 *
 *****************************************************************************/
static void ContainsDisarmCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
   XmToggleButtonCallbackStruct *status = (XmToggleButtonCallbackStruct *) callData;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) clientData;

   /* Force focus to the word field if set. */
   if (True==status->set)
     XmProcessTraversal(hw->help_dialog.srch.wordField,XmTRAVERSE_CURRENT);
}



/*****************************************************************************
 * Function:	    CreateCurVolBtnLabel()
 *                             
 * 
 * Parameters:      
 *
 * Return Value:    True or False if current vol has an index
 *
 * Purpose: 	    Creates the label for the current volume button
 *
 *****************************************************************************/
static Boolean CreateCurVolBtnLabel(
     DtHelpDialogWidget hw,
     Boolean *          out_curVolAvailable,
     XmString *         out_labelString,
     char * *           out_mnemonic)
{
   char *   path = NULL;
   char *   preTitle;
   char *   postTitle;
   XmString volTitleString = NULL;
   XmString preTitleString = NULL;
   XmString postTitleString = NULL;
   XmString newTitle;
   XmString labelString;
   char *   mnemonic;
   Boolean  curVolAvail;
   int      n;
   Arg      args[5];

#if 0   /* DBG */
   if (out_curVolAvailable) 
       *out_curVolAvailable = False;
   if (out_labelString) 
       *out_labelString = XmStringCreateLocalized("dum");
   if (out_mnemonic) 
       *out_mnemonic = "";
   return False;
#endif

   /* if help content is a volume, get the title */
   if ( hw->help_dialog.display.helpType == DtHELP_TYPE_TOPIC )
   {
      /* determine whether a current volume index is available */
      curVolAvail = VolumeHasIndexP(
                           hw->help_dialog.srch.srchSources,
                           hw->help_dialog.display.helpType,
                           hw->help_dialog.display.helpVolume);
   
      /* change False to True to search home dir; may want this */
      path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, hw->help_dialog.display.helpVolume,
                                     _DtHelpFileSuffixList,False,R_OK);
   
      /* title needs to be an XmString to use volume's font */
      if (path) 
      {
         XmFontList fontList = NULL;
         Boolean    mod = False;
   
         /* get the font list of the btn */
         n = 0;
         XtSetArg (args[n], XmNfontList, &fontList);  n++;
         XtGetValues (hw->help_dialog.srch.curVolRadBtn, args, n);
   
#if defined(DONT_USE_CDExc22774)
         /* copy the list before passing it in for modification */
         fontList = XmFontListCopy (fontList);
#endif
   
         GetVolumeInfoCB(hw->help_dialog.help.pDisplayArea,path,
                 NULL,&volTitleString,NULL,NULL,NULL,&fontList,&mod);
   
         /* if font list was changed, install it */
         if (mod)
         {
            /* set the font list of the btn */
            n = 0;
            XtSetArg (args[n], XmNfontList, fontList);  n++;
            XtSetValues (hw->help_dialog.srch.curVolRadBtn, args, n);
            if (fontList) XmFontListFree (fontList);
         }
      }
   
      /* just in case */
      if (NULL == volTitleString)
         volTitleString = XmStringCreateLocalized(
                  hw->help_dialog.display.helpVolume);
   } /* if helpType == TOPIC */
   else
   { /* if helpType != TOPIC */
      curVolAvail = False;
      switch (hw->help_dialog.display.helpType)
      {
      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
         volTitleString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 32,"Help Message")));
         break;
      case DtHELP_TYPE_FILE:
         volTitleString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 33,"Help File")));
         break;
      case DtHELP_TYPE_MAN_PAGE:
         volTitleString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 34,"Manual Page")));
         break;
      default:
         volTitleString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 35,"Unknown Format")));
         break;
      } /* switch on helpType */
   } /* if helpType != TOPIC */

   /* record path in the widget */
   XtFree(hw->help_dialog.srch.curVolPath);
   hw->help_dialog.srch.curVolPath = path;

   /* create title */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (GSSET, 3,"Current")));
   /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 4,"r"));*/
   mnemonic = 0;

   /* append volume name to title */
   preTitle = (char *)_DTGETMESSAGE(GSSET, 60," (");
   preTitleString = XmStringCreateLocalized(preTitle);
   postTitle = (char *)_DTGETMESSAGE(GSSET, 61,")");
   postTitleString = XmStringCreateLocalized(postTitle);

   newTitle = XmStringConcat(labelString,preTitleString);
   XmStringFree(labelString);
   XmStringFree(preTitleString);
   labelString = newTitle;

   newTitle = XmStringConcat(labelString,volTitleString);
   XmStringFree(labelString);
   XmStringFree(volTitleString);
   labelString = newTitle;

   newTitle = XmStringConcat(labelString,postTitleString);
   XmStringFree(labelString);
   XmStringFree(postTitleString);
   labelString = newTitle;

   /* set the out values */
   if (out_curVolAvailable) *out_curVolAvailable = curVolAvail;
   if (out_mnemonic) *out_mnemonic = mnemonic;
   if (out_labelString) *out_labelString = labelString;
   else XmStringFree(labelString);

   return curVolAvail;
}




/*****************************************************************************
 * Function:	    void CreateGlobSrchDialog()
 *                             
 * 
 * Parameters:      
 *
 * Return Value:
 *
 * Purpose: 	    Creates and displays an instance of the search dialog.
 *
 *****************************************************************************/
static void CreateGlobSrchDialog(
    Widget nw,
    char * searchWord)
{
   Widget        parentForm;
   Widget        separator;
   Widget        srchShell;
   /* Widget        topWidget; */
   Widget        volumesFrame;
   Widget        volumesForm;
   Widget        showFrame;
   Widget        showForm;
   Widget        frameTitle;
   XmString      labelString;
   /* char *        mnemonic; */
   Dimension     widgetHeight;
   Dimension     widgetBorderHeight;
   char *        title;
   DtHelpListStruct *pHelpInfo;
   Atom           wm_delete_window;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;
   XtTranslations btnTransTable;
   /* XtTranslations listTransTable; */
   /* XtTranslations mgrTransTable; */
   Boolean        curVolAvailable;
   int            n;
   Arg            args[20];

   /* get state of the volume */
   curVolAvailable = VolumeHasIndexP (
                          hw->help_dialog.srch.srchSources,
                          hw->help_dialog.display.helpType,
                          hw->help_dialog.display.helpVolume);

   /* Set up the translations table stuff */
   btnTransTable = XtParseTranslationTable(defaultBtnTranslations);
   /* listTransTable = XtParseTranslationTable(defaultListTranslations); */
   /* mgrTransTable = XtParseTranslationTable(defaultMgrTranslations); */
  
   /*  Create the shell used for the dialog.  */
   title = XtNewString(((char *)_DTGETMESSAGE(GSSET,1,"Help - Index Search")));
   n = 0;
   XtSetArg (args[n], XmNtitle, title);	 	 			n++;
   XtSetArg (args[n], XmNallowShellResize, True);			n++;
   srchShell = XmCreateDialogShell((Widget) hw, "searchShell", args, n);
   XtFree(title);

   /* Set the useAsyncGeo on the shell */
   n = 0;
   XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
   XtSetValues (XtParent(srchShell), args, n);  /* parent is new's shell */
#if 0
   /*  Adjust the decorations for the dialog shell of the dialog  */
   n = 0;
   XtSetArg(args[n], XmNmwmFunctions, 0);                               n++;
   XtSetArg (args[n], XmNmwmDecorations, 
             MWM_DECOR_BORDER | MWM_DECOR_TITLE);			n++;
   XtSetValues (srchShell, args, n);
#endif

   /* Grab the window mgr close and install a close dialog 
      callback when closed from dialog WM menu; this prevents
      the dialog from closing the host application. */
   wm_delete_window = XmInternAtom(XtDisplay(srchShell),
                                            "WM_DELETE_WINDOW", FALSE);
   XtSetArg(args[0], XmNdeleteResponse, XmDO_NOTHING);
   XmAddWMProtocolCallback(srchShell,wm_delete_window,
                          CloseSearchCB, (XtPointer)&hw->help_dialog.srch);
   XtSetValues(srchShell, args, 1);

   /* set the callback that positions the dialog when popped up */
   XtAddCallback (srchShell, XmNpopupCallback, 
               (XtCallbackProc)_DtHelpMapCB, (XtPointer) XtParent(hw));

   /***  Create the form used for the dialog.  ***/
   /* setting RESIZE_NONE is important to prevent the resultList from
      dynamically resizing as items with various fonts are added and
      deleted from it. */
   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);				n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNshadowThickness, 1);				n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);			n++;
   XtSetArg (args[n], XmNautoUnmanage, False);				n++;
   XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE);			n++;
   parentForm = XmCreateForm (srchShell, "parentForm", args, n);
   hw->help_dialog.srch.srchForm = parentForm;

   /*==============================================================*/
   /****** Create the volumes region ******/
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightOffset, 5);                       n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNmarginWidth, 5);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 2);                      n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);	n++;/*EXP*/
   volumesFrame = XmCreateFrame(parentForm, "volFrame", args, n);
   XtManageChild (volumesFrame);

   /* put form inside frame */
   volumesForm = XmCreateForm (volumesFrame, "volumesForm", args, 0);
   XtManageChild (volumesForm);

   /* create the frame title */
   labelString = XmStringCreateLocalized ((_DTGETMESSAGE(GSSET,2, "Search")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   XtSetArg (args[n], XmNchildType, XmFRAME_TITLE_CHILD);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   frameTitle = XmCreateLabelGadget(volumesFrame, "volFrameTitle", args, n);
   XtManageChild (frameTitle);
   XmStringFree (labelString);

   /* create Current Volume radio button */
   /* Use a dummy label until CreateCurVolBtnLabel can be used.
      It refs the srchCurVolRadBtn widget and other things that need 
      initalizing. */

   labelString = XmStringCreateLocalized (" ");
   n = 0;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);			n++;
   XtSetArg (args[n], XmNset, TRUE);		              		n++;

   XtSetArg(args[n], XmNlabelString, labelString); 			n++;
   XtSetArg (args[n], XmNmarginHeight, 0);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  		n++;
   XtSetArg (args[n], XmNtopOffset, 0);			        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   /* Attaching to form causes the hilite outline of the button to 
      stretch to the form edge when selected.  But it also forces
      the form to resize when the current volume title is too big
      for the available space. */
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNbottomOffset, 0);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.curVolRadBtn = 
            XmCreateToggleButtonGadget(volumesForm,"srchCurVolRadBtn",args,n);
   XtAddCallback(hw->help_dialog.srch.curVolRadBtn,XmNvalueChangedCallback,
                 UpdateSearchVolumesCB, (XtPointer) hw);
   /* ??? XtSetSensitive (hw->help_dialog.srch.curVolRadBtn,False); */
   XtManageChild (hw->help_dialog.srch.curVolRadBtn);
   XmStringFree (labelString);

   /* create All Volumes radio button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 5,"All Volumes")));
   /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 7,"A"));*/

   n = 0;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);			n++;
   XtSetArg (args[n], XmNset, FALSE);              			n++;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNmarginHeight, 0);				n++;
   XtSetArg (args[n], XmNmarginWidth, 2);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, hw->help_dialog.srch.curVolRadBtn);	n++;
   XtSetArg (args[n], XmNtopOffset, 1);			        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.allVolRadBtn = 
            XmCreateToggleButtonGadget(volumesForm,"srchAllVolRadBtn",args, n);
   XtAddCallback(hw->help_dialog.srch.allVolRadBtn,XmNvalueChangedCallback,
                 UpdateSearchVolumesCB, (XtPointer) hw);
   XtManageChild (hw->help_dialog.srch.allVolRadBtn);
   XmStringFree (labelString);

   /* create Selected Volumes radio button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 6,"Selected")));
   /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 9,"d"));*/

   n = 0;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);			n++;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNmarginHeight, 0);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, hw->help_dialog.srch.allVolRadBtn);	n++;
   XtSetArg (args[n], XmNtopOffset, 1);			        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNbottomOffset, 2);				n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.selVolRadBtn = 
            XmCreateToggleButtonGadget(volumesForm,"srchSelVolRadBtn", args, n);
   XtAddCallback(hw->help_dialog.srch.selVolRadBtn,XmNvalueChangedCallback,
                 UpdateSearchVolumesCB, (XtPointer) hw);
   XtManageChild (hw->help_dialog.srch.selVolRadBtn);
   XmStringFree (labelString);


   /* Create Selected... button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                     (GSSET, 7,"Select Volumes...")));

   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);               n++;
   XtSetArg (args[n], XmNtopOffset, 0);                                 n++;
   XtSetArg (args[n], XmNtopWidget, hw->help_dialog.srch.allVolRadBtn); n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
   XtSetArg (args[n], XmNrightOffset, 10);                              n++;
   XtSetArg (args[n], XmNbottomOffset, 2);                              n++;
   XtSetArg (args[n], XmNmarginTop, 0);                                 n++;
   XtSetArg (args[n], XmNmarginHeight, 3);                              n++;
   XtSetArg (args[n], XmNlabelString, labelString);                     n++;
   hw->help_dialog.srch.selectBtn =
                     XmCreatePushButtonGadget(volumesForm,"selectBtn", args, n);
   XtAddCallback (hw->help_dialog.srch.selectBtn, XmNactivateCallback,
                  OpenSelectDialogCB, (XtPointer) hw);
   XtManageChild (hw->help_dialog.srch.selectBtn);
   XtSetSensitive (hw->help_dialog.srch.selectBtn,False);
   XmStringFree (labelString);

   /****** Create the show selection region ******/
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, volumesFrame);              n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightOffset, 5);                       n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
   XtSetArg (args[n], XmNmarginWidth, 5);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 2);                      n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);	n++;/*EXP*/
   showFrame = XmCreateFrame(parentForm, "showFrame", args, n);
   XtManageChild (showFrame);

   /* FIX: is this doing enough? Goal: CR activation of FullIndex */
   /* Setup the proper translation on the title box widget */
   /* XtAugmentTranslations(showFrame, mgrTransTable);   */

   /* put form inside frame */
   showForm = XmCreateForm (showFrame, "showForm", args, 0);
   XtManageChild (showForm);

   /* create the frame title */
   labelString = XmStringCreateLocalized ((_DTGETMESSAGE(GSSET,8, "Show")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   XtSetArg (args[n], XmNchildType, XmFRAME_TITLE_CHILD);       n++;
   frameTitle = XmCreateLabelGadget(showFrame, "showFrameTitle", args, n);
   XtManageChild (frameTitle);
   XmStringFree (labelString);

   /* create the All Entries button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 9,"Complete Index")));

   n = 0;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);			n++;
   XtSetArg (args[n], XmNset, True);					n++;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  		n++;
   XtSetArg (args[n], XmNtopOffset, 0);			        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNbottomOffset, 0);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.fullIndexRadBtn = 
            XmCreateToggleButtonGadget(showForm,"srchFullIndexRadBtn",args,n);
   XtAddCallback(hw->help_dialog.srch.fullIndexRadBtn,XmNvalueChangedCallback,
                 UpdateIndexSelectionCB, (XtPointer) hw);
   XtManageChild (hw->help_dialog.srch.fullIndexRadBtn);
   XmStringFree (labelString);

   /* create Contains radio button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                        (GSSET, 10,"Entries with:")));
   /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 16,"n"));*/

   n = 0;
   XtSetArg (args[n], XmNindicatorType, XmONE_OF_MANY);			n++;
   XtSetArg (args[n], XmNset, False);					n++;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget,hw->help_dialog.srch.fullIndexRadBtn);n++;
   XtSetArg (args[n], XmNtopOffset, 1);			        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 4);                              n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.containsRadBtn = 
            XmCreateToggleButtonGadget(showForm,"srchContainsRadBtn",args,n);
   XtAddCallback(hw->help_dialog.srch.containsRadBtn,XmNvalueChangedCallback,
                 UpdateIndexSelectionCB, (XtPointer) hw);
   XtAddCallback(hw->help_dialog.srch.containsRadBtn,XmNdisarmCallback,
                 ContainsDisarmCB, (XtPointer) hw);
   XtManageChild (hw->help_dialog.srch.containsRadBtn);
   XmStringFree (labelString);

   /* create the search name text field */
   n = 0;
   XtSetArg (args[n], XmNvalue, "");					n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget,hw->help_dialog.srch.fullIndexRadBtn); n++;
   XtSetArg (args[n], XmNtopOffset, 0);			        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNleftWidget,hw->help_dialog.srch.containsRadBtn); n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, 3);				n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);			n++;
   XtSetArg (args[n], XmNcursorPositionVisible, False);			n++;
   hw->help_dialog.srch.wordField = 
           XmCreateTextField (showForm,"srchWord",args, n);
   XtSetSensitive(hw->help_dialog.srch.wordField,False);
   XtManageChild (hw->help_dialog.srch.wordField);
   /* Because the actionBtn is the default button on the form,
      and it activates a search, we do not need to (nor do we want
      to, since ActionButtonCB toggles between states) make it
      a callback on activation of the text field */
   XtAddCallback (hw->help_dialog.srch.wordField, XmNvalueChangedCallback,
                  UpdateSearchStartStatusCB, (XtPointer) hw);

   /* Setup the proper translation on the text field widget */
   XtAugmentTranslations(hw->help_dialog.srch.wordField, btnTransTable);   

   /* separator */
   /*  Create a separator search specs and result area */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, showFrame);                         n++;
   XtSetArg (args[n], XmNtopOffset, 10);				n++;
   separator =  XmCreateSeparatorGadget (parentForm, "separator", args, n);
   XtManageChild (separator);

   /*==============================================================*/
   /****** result related stuff *******/
   /* Create result List Label */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 10);		        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);		n++;
   hw->help_dialog.srch.statusLabel = 
         XmCreateLabelGadget (parentForm, "resultListLabel", args, n);
   StatusLabelUpdate(hw,FIRST_PROMPT_STATUS,True,0);     /* installs a label */
   XtManageChild (hw->help_dialog.srch.statusLabel);

   /* Create our result topics scrolled list (not placed on form) */
   n = 0;
   XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT);			n++;
   XtSetArg (args[n], XmNselectionPolicy, XmSINGLE_SELECT);        	n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);			n++;
   XtSetArg (args[n], XmNscrollBarDisplayPolicy, XmSTATIC);		n++;
   hw->help_dialog.srch.resultList =
            XmCreateScrolledList (parentForm, "resultList", args, n);
   XtManageChild (hw->help_dialog.srch.resultList);

   XtAddCallback (hw->help_dialog.srch.resultList, XmNsingleSelectionCallback, 
                  ProcessResultSelectionCB, (XtPointer) hw);
   XtAddCallback (hw->help_dialog.srch.resultList, XmNdefaultActionCallback, 
                  ProcessResultSelectionCB, (XtPointer) hw);

   /* Set the constraints on our scrolled list (place on form) */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, hw->help_dialog.srch.statusLabel);	n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 70);				n++;
   /* 70 is just a rough value that will be updated with a calculated
      value below, after we know the height of the buttons */
   XtSetValues (XtParent (hw->help_dialog.srch.resultList), args, n);

   /*  Create a separator between the buttons  */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget,  
                  XtParent (hw->help_dialog.srch.resultList));          n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   separator =  XmCreateSeparatorGadget (parentForm, "separator", args, n);
   XtManageChild (separator);

   /***** search spec-related stuff *****/
   /* Create start button (left of stop button)*/
   _DtHelpProcessLock();
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
          (GSSET, START_SEARCH_CAT,START_SEARCH_STR)));
   _DtHelpProcessUnlock();
   /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 20,START_SEARCH_MNEM));*/

   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   /* L & R position set below */
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 5);				n++;
   /*XtSetArg (args[n], XmNmarginHeight, 3);				n++;*/
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.actionBtn = 
                     XmCreatePushButtonGadget(parentForm,"actionBtn", args, n);
   XtAddCallback (hw->help_dialog.srch.actionBtn, XmNactivateCallback, 
                  ActionButtonCB, (XtPointer) hw);
   XtSetSensitive (hw->help_dialog.srch.actionBtn,False);
   XtManageChild (hw->help_dialog.srch.actionBtn);
   XmStringFree (labelString);

   XtSetArg (args[0], XmNdefaultButton, hw->help_dialog.srch.actionBtn);
   XtSetValues (parentForm, args, 1);

   /* Build the Cancel Button */
   _DtHelpProcessLock();
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
               (GSSET, CLOSE_BTN_CAT,CLOSE_BTN_STR)));
   _DtHelpProcessUnlock();
   /*mnemonic = ((char *)_DTGETMESSAGE(GSSET, 25,CLOSE_BTN_MNEM));*/
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   /* L & R position set below */
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 3);				n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.closeBtn = 
              XmCreatePushButtonGadget (parentForm, "srchCloseBtn", args, n);
   
   XtAddCallback(hw->help_dialog.srch.closeBtn, 
               XmNactivateCallback, CloseSearchCB,(XtPointer) &hw->help_dialog.srch);
  
   XtManageChild (hw->help_dialog.srch.closeBtn);
   XmStringFree (labelString);

   /* Build the Help button */
   _DtHelpProcessLock();
   labelString = XmStringCreateLocalized((char *)_DTGETMESSAGE
                  (GSSET, HELP_BTN_CAT,HELP_BTN_STR));
   _DtHelpProcessUnlock();
   /*mnemonic = (char *)_DTGETMESSAGE(GSSET, 27,HELP_BTN_MNEM);*/

   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   /* L & R position set below */
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 5);				n++;
   XtSetArg (args[n], XmNmarginHeight, 3);				n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True);			n++;
   hw->help_dialog.srch.helpBtn = 
          XmCreatePushButtonGadget (parentForm, "srchHelpBtn", args, n);
   XtManageChild (hw->help_dialog.srch.helpBtn);
   XmStringFree (labelString);

   pHelpInfo = _DtHelpListAdd(DtHELP_srchIndexHelpBtn_STR,
                        (Widget) hw, &hw->help_dialog.help, 
			&hw->help_dialog.help.pHelpListHead);
   XtAddCallback(hw->help_dialog.srch.helpBtn, XmNactivateCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);
  
   /*==============================================================*/
   /****** finishing touches *******/
   /* set the Form cancel button (for KCancel) */
   n = 0;
   XtSetArg (args[n], XmNcancelButton, hw->help_dialog.srch.closeBtn); n++;
   XtSetValues (parentForm, args, n);

   { /* position buttons */
      /* This code adds up the sizes of the buttons to go into
         the bottom row and calculates the form position percentages.
         All buttons are the same size, and are able to hold all
         the strings that may be dynamically placed in them.
         All buttons are 5% apart. */
      /* This code is specifically written to handle 3 buttons
         and assumes that the first 3 strings are to the ActionBtn */
   #define NUMSTRS 5
   Dimension maxWidth = 0;
   Dimension borderWidth = 0;
   Dimension sumWidth = 0;
   float scale = 0.0;
   float posList[11];  /* need 11 due to algorithm, which uses index 10 */
   XmFontList fontList = NULL;
   int i;

   /* get the fontList for the button */
   XtSetArg (args[0], XmNborderWidth, &borderWidth);
   XtSetArg (args[1], XmNfontList, &fontList);
   XtGetValues (hw->help_dialog.srch.actionBtn, args, 2);

#define BETBUTSPACE 5           /* pixels */
   sumWidth = BETBUTSPACE;      /* left side space */
   posList[0] = (float) sumWidth;
   for (i=0; i<NUMSTRS; i++)
   {
      XmString labelString;
      Dimension width;
      _DtHelpProcessLock();
      s_GlobSrchDlgBtnStrs[i] = (char *) _DTGETMESSAGE(GSSET,
                     s_GlobSrchDlgBtnCatNum[i],s_GlobSrchDlgBtnStrs[i]);
      labelString = XmStringCreateLocalized(s_GlobSrchDlgBtnStrs[i]);
      _DtHelpProcessUnlock();
#define MARGINS  4        /* pixel margins: 2=L, 2=R */
      width = XmStringWidth(fontList,labelString) + borderWidth + MARGINS;
      if (i<=2) maxWidth = max(width,maxWidth);
      if (i==2) width = maxWidth; /* 1st three labels go in 1st button */
      if (i>=2) /* after scanning just labels, do buttons */
      { 
         sumWidth += width;
         posList[i*2+1] = (float) sumWidth;
         sumWidth += BETBUTSPACE;
         posList[i*2+2] = (float) sumWidth;
      }
      XmStringFree(labelString);
   } /* for calcing widths */

   /* scale pixels to percent */
   scale = 100.0 / (float) sumWidth;

   n = 0;
   XtSetArg (args[n], XmNwidth, maxWidth);				n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, (Dimension) (posList[0]*scale));	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition,(Dimension) (posList[5]*scale));	n++;
   XtSetValues (hw->help_dialog.srch.actionBtn, args, n);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, (Dimension) (posList[6]*scale));	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition,(Dimension) (posList[7]*scale));	n++;
   XtSetValues (hw->help_dialog.srch.closeBtn, args, n);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, (Dimension) (posList[8]*scale));	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition,(Dimension) (posList[9]*scale));	n++;
   XtSetValues (hw->help_dialog.srch.helpBtn, args, n);
   } /* */

   { /* set the proper offset between the bottom of the results list
        and the bottom of the form to maintain a constant sized
        button bar. */
   int offset = 0;

   /* first calc offset of list to form bottom based on earlier sizes */
#define KNOWN_OFFSETS_BELOW_LIST 20   /* actually only 15, but 15 fails */
   n = 0;
   XtSetArg(args[n], XmNborderWidth, &widgetBorderHeight);	n++;
   XtSetArg(args[n], XmNheight, &widgetHeight);			n++;
   XtGetValues(hw->help_dialog.srch.actionBtn, args, n);
   offset = widgetHeight + 2 * widgetBorderHeight;
   XtGetValues(separator, args, n);
   offset += widgetHeight + 2 * widgetBorderHeight;
   XtGetValues(XtParent(hw->help_dialog.srch.resultList), args, 1);
   offset += widgetBorderHeight;
   offset += KNOWN_OFFSETS_BELOW_LIST;

   /* then set the offset */
   n = 0;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, offset);				n++;
   XtSetValues (XtParent (hw->help_dialog.srch.resultList), args, n);
   }

   /** force tabs to go to each widget and in right order **/
   XtSetArg (args[0], XmNnavigationType, XmSTICKY_TAB_GROUP);
   XtSetValues (hw->help_dialog.srch.curVolRadBtn,args,1);
   XtSetValues (hw->help_dialog.srch.allVolRadBtn,args,1);
   XtSetValues (hw->help_dialog.srch.selVolRadBtn,args,1);
   XtSetValues (hw->help_dialog.srch.selectBtn,args,1);
   XtSetValues (hw->help_dialog.srch.fullIndexRadBtn,args,1);
   XtSetValues (hw->help_dialog.srch.containsRadBtn,args,1);
   XtSetValues (hw->help_dialog.srch.wordField,args,1);
   XtSetValues (hw->help_dialog.srch.resultList,args,1);
   XtSetValues (hw->help_dialog.srch.actionBtn,args,1);
   XtSetValues (hw->help_dialog.srch.closeBtn,args,1);
   XtSetValues (hw->help_dialog.srch.helpBtn,args,1);

   /** put focus on the text field **/
   XtSetArg (args[0], XmNinitialFocus, hw->help_dialog.srch.wordField);
   XtSetValues (parentForm,args,1);

   /* Add the proper help callback to the print dialog shell "F1" support */
   pHelpInfo = _DtHelpListAdd(DtHELP_srchIndexShell_STR,
                        (Widget) hw, &hw->help_dialog.help, 
			&hw->help_dialog.help.pHelpListHead);
   XtAddCallback(parentForm, XmNhelpCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);
}


/*****************************************************************************
 * Function:	    updateDisplay()
 *                             
 * 
 * Parameters:      hw:  the help widget
 *
 * Return Value:    0
 *
 * Purpose: 	    forces an update on index search dialog widgets
 *
 *****************************************************************************/
static int updateDisplay(
                DtHelpDialogWidget hw)
{
    XmUpdateDisplay((Widget)hw->help_dialog.srch.actionBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.curVolRadBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.allVolRadBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.selVolRadBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.selectBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.fullIndexRadBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.containsRadBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.wordField);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.statusLabel);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.resultList);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.closeBtn);/*DBG*/
    XmUpdateDisplay((Widget)hw->help_dialog.srch.helpBtn);/*DBG*/
    XmUpdateDisplay(XtParent((Widget)hw->help_dialog.srch.srchForm));/*DBG*/
    return 0;
}



/*****************************************************************************
 * Function:	    void _DtHelpGlobSrchDisplayDialog()
 *                             
 * 
 * Parameters:      parent      Specifies the parent widget
 *                  searchWord  word to put into searchWord field
 *        remove this when integrated into DtHelp:
 *                  curVolume   volume considered the current volume
 *
 * Return Value:
 *
 * Purpose: 	    Setsup the proper data in the search dialog and 
 *                  displays an instance of the search dialog.
 *
 *****************************************************************************/
void _DtHelpGlobSrchDisplayDialog(
    Widget w,
    char * searchWord,
    char * curVolume)
{
   DtHelpDialogWidget hw = (DtHelpDialogWidget) w;
   XmToggleButtonCallbackStruct status;        /* the call data */
   Widget                       sourceBtn;     /* widget owning event */
   Dimension     height;
   Dimension     width;
   XmString      labelString = NULL;
   int           n;
   Arg           args[10];

   /* make the dialog itself */
   if (NULL == hw->help_dialog.srch.srchForm)
   {
      CreateGlobSrchDialog((Widget) hw,searchWord);

      /* create the button label */
      CreateCurVolBtnLabel(hw, NULL, &labelString, NULL);
      XtSetArg (args[0], XmNlabelString, labelString);
      XtSetValues (hw->help_dialog.srch.curVolRadBtn, args, 1);
      XmStringFree (labelString);

      /*** map widget and update display before doing any searching ***/
      /* Make sure the Search Dialog is managed */
      XtManageChild(hw->help_dialog.srch.srchForm);
      XtMapWidget(hw->help_dialog.srch.srchForm);

      /* set focus to contains word */
      XmProcessTraversal(hw->help_dialog.srch.wordField,XmTRAVERSE_CURRENT);
    
      /* force several updates to make sure it is fully displayed and
         that height/width are correctly inited */
      XmUpdateDisplay(XtParent((Widget)hw->help_dialog.srch.srchForm));
      XmUpdateDisplay(XtParent((Widget)hw->help_dialog.srch.srchForm));
      XmUpdateDisplay(XtParent((Widget)hw->help_dialog.srch.srchForm));
    
      /*** now update settings, which may also invoke a search ***/
      /* set the cur vol btn sensitivity */
      UpdateCurVolBtnSens(hw,True);

      /* set default show index state */
      /* set full index if current vol has an index, contains word if not */
      if (hw->help_dialog.srch.curVolRadBtnSens) /* set in UpdateCurVolBtnSens*/
         sourceBtn = hw->help_dialog.srch.fullIndexRadBtn;
      else
         sourceBtn = hw->help_dialog.srch.containsRadBtn;

      /* set the show index state */
      status.reason = XmCR_VALUE_CHANGED;
      status.set = True;
      status.event = (XEvent *) 1; /* thwart == NULL test in Update...() */
      UpdateIndexSelectionCB(sourceBtn,(XtPointer)hw,(XtPointer)&status);
      StatusLabelUpdate(hw,FIRST_PROMPT_STATUS,False,0);
   
      updateDisplay(hw);  /* DBG */

#if 0  /* 11/23/94 */
      /** Set min size for the dialog **/
      n = 0;
      XtSetArg(args[n], XmNheight, &height); ++n;
      XtSetArg(args[n], XmNwidth, &width);  ++n;
      XtGetValues(XtParent((Widget)hw->help_dialog.srch.srchForm), args, n);
      n = 0;
      XtSetArg(args[n], XmNminHeight, height - 200); ++n;  /* 200: arbitrary */
      XtSetArg(args[n], XmNminWidth, width); ++n;
      XtSetValues(XtParent((Widget)hw->help_dialog.srch.srchForm), args, n);
#endif
   }
   else /* properly update dialog to support new word and cur vol */
   {
      /* fixup current volume, if need be */
      _DtHelpGlobSrchUpdateCurVol((Widget)hw);
     
      /* fixup search word, if need be */
      if (NULL != searchWord)
      {
         String srchWord = XtNewString(searchWord);
         /* srchWord is freed or used in CheckSearchWord() */
         if (CheckSearchWord(hw,srchWord,True) == True)
         {  /* words are different */
            /* free all hit-related data and reset flags */
            HitListFreeAllVolHits(hw,True);  /*True=free everything*/
         }
      }
    
      /* if its not managed, manage it */
      if ( XtIsManaged(hw->help_dialog.srch.srchForm) == False )
      {
         /* manage and map the Search Dialog */
         XtManageChild(hw->help_dialog.srch.srchForm);
         XtMapWidget((Widget)hw->help_dialog.srch.srchForm);

         /* see if the selection dialog was already open */
         if (   hw->help_dialog.srch.selectionDlg
             && XtIsManaged(hw->help_dialog.srch.selectionDlg) )
         {
            XtManageChild(hw->help_dialog.srch.selectionDlg);
            XtMapWidget(hw->help_dialog.srch.selectionDlg);
         }
      }
      else  /* if it is managed, bring it forward */
      {
        Widget parent = XtParent(hw->help_dialog.srch.srchForm);
        XRaiseWindow ( XtDisplay(parent), XtWindow(parent) );
      }
   
   /* change focus */
   
   if (hw->help_dialog.srch.hitsFound == True) 
     XmProcessTraversal(hw->help_dialog.srch.resultList,XmTRAVERSE_CURRENT);
   else
     XmProcessTraversal(hw->help_dialog.srch.allVolRadBtn,XmTRAVERSE_CURRENT);



   }  /* end if create a new dialog */
}   



/*****************************************************************************
 * Function:	    void _DtHelpGlobSrchUpdateCurVol()
 * 
 * Parameters:      new          the help widget
 *
 * Return Value:
 *
 * Purpose: 	    Adjusts the current volume of the dialog
 *
 *****************************************************************************/
void _DtHelpGlobSrchUpdateCurVol(
    Widget w)
{
  String path;
  DtHelpDialogWidget hw = (DtHelpDialogWidget) w;

#if 1
  if (   NULL == hw->help_dialog.srch.srchForm )
#else
  if (   NULL == hw->help_dialog.srch.srchForm
      || XtIsManaged(hw->help_dialog.srch.srchForm) == False ) 
#endif
      return;                                         /* RETURN */

  /* get full pathname for the volume */
  path = NULL;
  if (hw->help_dialog.display.helpVolume)
     path = _DtHelpFileLocate(DtHelpVOLUME_TYPE, 
                                /* FIX: will helpVolume have matching path? */
                                hw->help_dialog.display.helpVolume,
                                _DtHelpFileSuffixList,False,R_OK);

  if (   _DtHelpGlobSrchCurVolume == hw->help_dialog.srch.srchSources
      && hw->help_dialog.display.helpType == DtHELP_TYPE_TOPIC
      && NULL != path
      && NULL != hw->help_dialog.srch.curVolPath
      && _DtHelpFileIsSameP(path,hw->help_dialog.srch.curVolPath,
                 GetVolumeInfoCB, _DtHELP_FILE_NAME, 
                 hw->help_dialog.help.pDisplayArea) )
  {
     /* leave current status as is */
     XtFree(path);
     return;                                        /* RETURN */
  }
 

  /* a different current volume or change of topic */
  {
     XmString labelString;
     Arg    args[5];

     /* if still searching previous cur volume, stop any search in progress */
     if (    _DtHelpGlobSrchCurVolume == hw->help_dialog.srch.srchSources
          && hw->help_dialog.srch.workProcId)
     {
         /* cancel the search */
         StopSearchCB(NULL,(XtPointer) hw, NULL);

         /* (F,F,F,F): disable search,disable display,no 0 hits,for all vols */
         SetVolStatus(hw->help_dialog.srch.volListHead,False,False,False,False);

         /* zero search data */
         hw->help_dialog.srch.volLeftCnt = 0;
         hw->help_dialog.srch.curSrchVol = NULL;

         /* assumption is, that even though the search was incomplete,
            nothing bad will happen if we don't free the hit data,
            and the search could resume where left off, if necessary. */
     }

     CreateCurVolBtnLabel(hw, NULL, &labelString, NULL);
     XtSetArg(args[0],XmNlabelString,labelString);
     XtSetValues(hw->help_dialog.srch.curVolRadBtn,args,1);
     XmStringFree(labelString);

     /* set the cur vol btn sensitivity */
     UpdateCurVolBtnSens(hw,False);

     /* set state of start button correctly */
     UpdateSearchStartStatusCB(NULL, (XtPointer) hw, NULL);

     /* and search if needed */
     if ( _DtHelpGlobSrchCurVolume == hw->help_dialog.srch.srchSources )
     {
        /* if to search a volume, start the search */
        if (hw->help_dialog.display.helpType == DtHELP_TYPE_TOPIC)
           StartSearchCB(NULL,(XtPointer) hw, NULL);
        else /* if current isn't a volume, clear the display & update status */
        {
           DeleteListContents(&hw->help_dialog.srch);
           StatusLabelUpdate(hw,NO_VOL_STATUS,False,0);
        }
     }
  }
}



/*****************************************************************************
 * Function:	    void _DtHelpGlobSrchInitVars()
 * 
 * Parameters:      srch	search main data structure
 *
 * Return Value:
 *
 * Purpose: 	    Init the contents of the control data structure
 *
 *****************************************************************************/
void  _DtHelpGlobSrchInitVars(
       _DtHelpGlobSearchStuff * srch)
{
    /* set the font resource */
    if (srch->hitPrefixFont != _DtHelpDefaultSrchHitPrefixFont)
       srch->hitPrefixFont = XtNewString(srch->hitPrefixFont);

    /* File Selection Dialog font list */
    srch->volTitlesFontList = NULL;

    /* Set our search dialog widgets to NULL */
    srch->srchForm      = NULL;
    srch->actionBtn     = NULL;
    srch->curVolRadBtn  = NULL;
    srch->allVolRadBtn  = NULL;
    srch->selVolRadBtn  = NULL;
    srch->selectBtn     = NULL;
    srch->fullIndexRadBtn = NULL;
    srch->containsRadBtn = NULL;
    srch->wordField     = NULL;
    srch->statusLabel   = NULL;
    srch->resultList    = NULL;
    srch->gotoBtn       = NULL;
    srch->closeBtn      = NULL;
    srch->helpBtn       = NULL;
    srch->selectionDlg  = NULL;

    /* init dialog content variables */
    srch->curVolPath    = NULL;
    srch->rawWordStr    = NULL;
    srch->normWordStr   = NULL;
    srch->localeWordStr = NULL;
    srch->wordFieldFirstChar = 0;
    srch->wordFieldLen       = 0;
    srch->statusLineUsage = 0;  /* empty */

    /* init srch processing variables */
    srch->iconv3Codeset = NULL;         /* iconv(3)-compatible code set of current locale */
    srch->iconv3Context = NULL;
    srch->srchSources   = _DtHelpGlobSrchVolumeUndef;      /* radio btn usage */
    srch->curSrchVol    = NULL; 	/* volume currently in search */
    srch->hitsFontLoaded= False;	/* is font loaded? */
    srch->volScanDone   = False;	/* is the volume list complete? */
    srch->fullIndex     = False;
    srch->hitsFound     = False;	/* state of search */
    srch->readyToStart  = False;	/* state of search */
    srch->searchInProgress= False;	/* state of search */
    srch->curVolRadBtnSens = False;
    srch->volLeftCnt    = 0;
    srch->volListHead   = NULL; 	/* info on search topics found */
    srch->workProcId    = 0;
}


/*****************************************************************************
 * Function:	    void _DtHelpGlobSrchCleanAndClose()
 * 
 * Parameters:      srch      search main data structure
 *                  destroy   flag to signal srch dialog should be destroyed
 *
 * Return Value:
 *
 * Purpose: 	    Free the contents of the control data structure
 *
 *****************************************************************************/
void  _DtHelpGlobSrchCleanAndClose(
       _DtHelpGlobSearchStuff * srch,
       Boolean               destroy)
{
    /* close conversion context */
    _DtHelpCeIconvClose(&srch->iconv3Context);
  
    /* free the font list */
    if(srch->volTitlesFontList)
    {
         XmFontListFree(srch->volTitlesFontList);
         srch->volTitlesFontList = NULL;
    }
  
    /*
     * Make sure CloseSearchCB does not try using an invalid
     * widget id in its XtUnmanageChild and XtUnmapWidget calls.
     */
    if (destroy)
	srch->srchForm = NULL;

    /* make dialog invisible; update sensitivities */
    CloseSearchCB(NULL,(XtPointer)srch, NULL);

    /* Destroy our index search dialog? */
    if (destroy)
    {
        /* set the font resource */
        if (srch->hitPrefixFont != _DtHelpDefaultSrchHitPrefixFont)
           XtFree(srch->hitPrefixFont);

       /*
        * mark widgets as destroyed.
        * The XtDestroyWidget that called me will have called the destroy
        * callback of the widgets. So I don't have to do anything except
        * make sure I know they are destroyed.
        */
       srch->selectionDlg = NULL;
       srch->srchForm     = NULL;
       srch->resultList   = NULL;

       /* free other data released to search dialog */
       VolListFree(srch);

       /* free the locale word string, if allocated
	* (i.e. diff from normWordStr) */
       if(srch->localeWordStr!=srch->normWordStr)
       {
          XtFree(srch->localeWordStr);
          srch->localeWordStr=NULL;
       }
       XtFree(srch->normWordStr);
       srch->normWordStr=NULL;
       XtFree(srch->rawWordStr);
       srch->rawWordStr=NULL;
       XtFree(srch->curVolPath);
       srch->curVolPath=NULL;
       XtFree(srch->iconv3Codeset);
       srch->iconv3Codeset = NULL;
    }
}
